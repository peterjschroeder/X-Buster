// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_weapon.c
// perform the server side effects of a weapon firing

#include "g_local.h"

vec3_t	forward, right, up;
vec3_t	muzzle;

#define NUM_NAILSHOTS 15

/*
===============
SpreadFire_Powerup
===============
*/
void G_SpreadProjectile(gentity_t* ent, weaponLaunch fireFunc)
{
    gentity_t	*m;
    gclient_t	*client;
    vec3_t		newforward;
    vec3_t		angles;

    client = ent->client;

    //First shot, slightly to the right
    AngleVectors( client->ps.viewangles, forward, right, 0);
    VectorMA(forward, 0.1, right, newforward);
    VectorNormalize(newforward);
    vectoangles(newforward, angles);

    AngleVectors( angles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );

    m = fireFunc (ent, muzzle, forward);

    //Second shot, slightly to the left
    AngleVectors( client->ps.viewangles, forward, right, 0);
    VectorMA(forward, -0.1, right, newforward);
    VectorNormalize(newforward);
    vectoangles(newforward, angles);

    AngleVectors( angles, forward, right, up );
    CalcMuzzlePoint( ent, forward, right, up, muzzle );

    m = fireFunc(ent, muzzle, forward);
}

/*
================
G_BounceProjectile
================
*/
void G_BounceProjectile( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout )
{
    vec3_t v, newv;
    float dot;

    VectorSubtract( impact, start, v );
    dot = DotProduct( v, dir );
    VectorMA( v, -2*dot, dir, newv );

    VectorNormalize(newv);
    VectorMA(impact, 8192, newv, endout);
}


/*
======================================================================

GAUNTLET

======================================================================
*/

void Weapon_Gauntlet( gentity_t *ent )
{

}

/*
===============
CheckGauntletAttack
===============
*/
qboolean CheckGauntletAttack( gentity_t *ent )
{
    trace_t		tr;
    vec3_t		end;
    gentity_t	*tent;
    gentity_t	*traceEnt;
    int			damage;

    // set aiming directions
    AngleVectors (ent->client->ps.viewangles, forward, right, up);

    CalcMuzzlePoint ( ent, forward, right, up, muzzle );

    VectorMA (muzzle, 32, forward, end);

    trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);
    if ( tr.surfaceFlags & SURF_NOIMPACT )
    {
        return qfalse;
    }

    traceEnt = &g_entities[ tr.entityNum ];

    // send blood impact
    if ( traceEnt->takedamage && (traceEnt->client || traceEnt->s.eType==ET_NPC))
    {
        tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
        tent->s.otherEntityNum = traceEnt->s.number;
        tent->s.eventParm = DirToByte( tr.plane.normal );
        tent->s.weapon = ent->s.weapon;
    }

    if ( !traceEnt->takedamage)
    {
        return qfalse;
    }

    damage = 50;
    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
              damage, 0, MOD_GAUNTLET );

    return qtrue;
}


/*
======================================================================

MACHINEGUN

======================================================================
*/

/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/
void SnapVectorTowards( vec3_t v, vec3_t to )
{
    int		i;

    for ( i = 0 ; i < 3 ; i++ )
    {
        if ( to[i] <= v[i] )
        {
            v[i] = (int)v[i];
        }
        else
        {
            v[i] = (int)v[i] + 1;
        }
    }
}




/*
======================================================================

GRAPPLING HOOK

======================================================================
*/

void Weapon_GrapplingHook_Fire (gentity_t *ent)
{
    if (!ent->client->fireHeld && !ent->client->hook)
        fire_grapple (ent, muzzle, forward);

    ent->client->fireHeld = qtrue;
}

void Weapon_HookFree (gentity_t *ent)
{
    ent->parent->client->hook = NULL;
    ent->parent->client->ps.pm_flags &= ~PMF_GRAPPLE_PULL;
    G_FreeEntity( ent );
}

void Weapon_HookThink (gentity_t *ent)
{
    if (ent->enemy)
    {
        vec3_t v, oldorigin;

        VectorCopy(ent->r.currentOrigin, oldorigin);
        v[0] = ent->enemy->r.currentOrigin[0] + (ent->enemy->r.mins[0] + ent->enemy->r.maxs[0]) * 0.5;
        v[1] = ent->enemy->r.currentOrigin[1] + (ent->enemy->r.mins[1] + ent->enemy->r.maxs[1]) * 0.5;
        v[2] = ent->enemy->r.currentOrigin[2] + (ent->enemy->r.mins[2] + ent->enemy->r.maxs[2]) * 0.5;
        SnapVectorTowards( v, oldorigin );	// save net bandwidth

        G_SetOrigin( ent, v );
    }

    VectorCopy( ent->r.currentOrigin, ent->parent->client->ps.grapplePoint);
}


/*
===============
LogAccuracyHit
===============
*/
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker )
{
    if( !target->takedamage )
    {
        return qfalse;
    }

    if ( target == attacker )
    {
        return qfalse;
    }

    if( !target->client )
    {
        return qfalse;
    }

    if( !attacker->client )
    {
        return qfalse;
    }

    if( target->client->ps.stats[STAT_HEALTH] <= 0 )
    {
        return qfalse;
    }

    if ( OnSameTeam( target, attacker ) )
    {
        return qfalse;
    }

    return qtrue;
}


/*
===============
CalcMuzzlePoint

set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePoint(gentity_t * ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint)
{
    VectorCopy(ent->s.pos.trBase, muzzlePoint);
    muzzlePoint[2] += ent->client->ps.viewheight;
    VectorMA(muzzlePoint, 5, forward, muzzlePoint);
    // snap to integer coordinates for more efficient network bandwidth usage
    SnapVector(muzzlePoint);
}

/*
===============
CalcMuzzlePointOrigin

set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePointOrigin(gentity_t * ent, vec3_t origin, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint)
{
    VectorCopy(ent->s.pos.trBase, muzzlePoint);
    muzzlePoint[2] += ent->client->ps.viewheight;
    VectorMA(muzzlePoint, 5, forward, muzzlePoint);
    // snap to integer coordinates for more efficient network bandwidth usage
    SnapVector(muzzlePoint);
}



/*
===============
FireWeapon
===============
*/
void FireWeapon( gentity_t *ent )
{
    int weap = bg_playerlist[ent->client->pers.playerclass].weapons[ent->s.weapon].weapon;

    ent->client->accuracy_shots++;

    // set aiming directions
    AngleVectors (ent->client->ps.viewangles, forward, right, up);

    CalcMuzzlePointOrigin ( ent, ent->client->oldOrigin, forward, right, up, muzzle );

    // fire the specific weapon

    // check for techs first!
    if( ent->client->ps.weaponstate == WEAPON_TECHUPCUT )
    {
        Tech_Uppercut_Fire( ent );
        return;
    }
    else if( ent->client->ps.weaponstate == WEAPON_TECHSPIKE )
    {
        Tech_Spike_Fire( ent );
        return;
    }
    else if( ent->client->ps.weaponstate == WEAPON_TECHSPIN )
    {
        Tech_Spin_Fire( ent );
        return;
    }

    switch( weap )
    {
        //default weapons
    case WP_XBUSTER: //charge class follows this format
        Weapon_XBuster_Fire( ent );
        break;
    case WP_FBUSTER:
        Weapon_FBuster_Fire( ent );
        break;
    case WP_MBUSTER: //charge class follows this format
        Weapon_MBuster_Fire( ent );
        break;
    case WP_PBUSTER: //charge class follows this format
        Weapon_PBuster_Fire( ent );
        break;
        //fire element
    case WP_FCUTTER:
        Weapon_FCutter_Fire( ent );
        break;
    case WP_RISFIRE:
        Weapon_RisFire_Fire( ent );
        break;
    case WP_HWAVE:
        Weapon_HWave_Fire( ent );
        break;
    case WP_PSTRIKE:
        Weapon_PStrike_Fire( ent );
        break;
        //water element
    case WP_SICE:
        Weapon_SIce_Fire( ent );
        break;
    case WP_FBURST:
        Weapon_FBurst_Fire( ent );
        break;
    case WP_IBURST:
        Weapon_IceBurst_Fire( ent );
        break;
    case WP_WSTRIKE:
        Weapon_WStrike_Fire( ent );
        break;
        //Air attacks
    case WP_AFIST:
        Weapon_AFist_Fire( ent );
        break;
    case WP_AWAVE:
        Weapon_AWave_Fire( ent );
        break;
    case WP_TEMPEST:
        Weapon_Tempest_Fire( ent );
        break;
        //Earth attacks
    case WP_BCRYSTAL:	//charge
        Weapon_BCrystal_Fire( ent );
        break;
    case WP_RSTONE:	//buster only
        Weapon_RStone_Fire( ent );
        break;
    case WP_SBURST:	//rapid
        Weapon_SBurst_Fire( ent );
        break;
        //Metal attacks
    case WP_DSHOT:	//rapid
        Weapon_DShot_Fire( ent );
        break;
    case WP_BLAUNCHER:	//buster only
        Weapon_BLauncher_Fire( ent );
        break;
    case WP_STRIKECHAIN:	//proto
        Weapon_GrapplingHook_Fire( ent );
        break;
    case WP_LTORPEDO:	//charge
        Weapon_LTorpedo_Fire( ent );
        break;
    case WP_IMBOOMERANG: // open
        Weapon_IMBoomerang_Fire ( ent );
        break;
        //Nature attacks
    case WP_TBURSTER://charge
        Weapon_TBurster_Fire( ent );
        break;
    case WP_LSTORM:	//buster only
        Weapon_LStorm_Fire( ent );
        break;
    case WP_ASPLASHER:	//rapid
        Weapon_ASplasher_Fire( ent );
        break;
        //Gravity attacks
    case WP_GBOMB:	//charge
        Weapon_GBomb_Fire( ent );
        break;
    case WP_GRAVWELL:	//buster only
        Weapon_GravWell_Fire( ent );
        break;
    case WP_MMINE:	//buster only
        Weapon_MMine_Fire( ent );
        break;
    case WP_DMATTER:	//rapid
        Weapon_DMatter_Fire( ent );
        break;
        //Light attacks
    case WP_BLASER://charge
        Weapon_BLaser_Fire( ent );
        break;
    case WP_SSHREDDER:	//buster only
        Weapon_SShredder_Fire( ent );
        break;
    case WP_NBURST:	//rapid
        Weapon_NBurst_Fire( ent );
        break;
        //Electrical attacks
    case WP_SFIST:	//charge
        Weapon_SFist_Fire( ent );
        break;
    case WP_BSHIELD:	//buster only
        Weapon_BShield_Fire( ent );
        break;
    case WP_TRTHUNDER:	//buster only
        Weapon_TriadThunder_Fire( ent );
        break;
    case WP_BLIGHTNING:	//rapid
        Weapon_BLightning_Fire( ent );
        break;
    default:
// FIXME		G_Error( "Bad ent->s.weapon" );
        break;
    }
}

/*
===============
Fire OFFhand Weapon
===============
*/
void FireOffhand( gentity_t *ent )
{

    // set aiming directions
    AngleVectors (ent->client->ps.viewangles, forward, right, up);

    CalcMuzzlePointOrigin ( ent, ent->client->oldOrigin, forward, right, up, muzzle );

    switch( ent->client->pers.playerclass )
    {
    case CLASS_DWNINFINITY:
        // track shots taken for accuracy tracking.  For now, only the melee class fires a weapon for offhand
        ent->client->accuracy_shots++;
        Weapon_ZBuster_Fire( ent );
        break;
    default:
// FIXME		G_Error( "Bad class" );
        break;
    }
}

void NPC_FireWeapon(gentity_t *ent,vec3_t target_pos)
{

}

/*
======================================================================

LIGHTNING GUN

======================================================================
*/

void Weapon_LightningFire( gentity_t *ent )
{
    trace_t		tr;
    vec3_t		end;
    gentity_t	*traceEnt, *tent;
    int			damage;

    damage = 8;

    VectorMA( muzzle, LIGHTNING_RANGE, forward, end );

// The SARACEN's Lightning Discharge - START
    if (trap_PointContents (muzzle, -1) & MASK_WATER)
    {
        int zaps;
        gentity_t *tent;

        zaps = ent->client->ps.ammo[RWP_ELECTRICAL];	// determines size/power of discharge
        if (!zaps) return;	// prevents any subsequent frames causing second discharge + error
        zaps++;		// pmove does an ammo[gun]--, so we must compensate
        SnapVectorTowards (muzzle, ent->s.origin);	// save bandwidth

        tent = G_TempEntity (muzzle, EV_LIGHTNING_DISCHARGE);
        tent->s.eventParm = zaps;				// duration / size of explosion graphic

        ent->client->ps.ammo[RWP_ELECTRICAL] = 0;		// drain ent's lightning count
        G_WaterRadiusDamage (muzzle, ent, damage * zaps,
                             (damage * zaps) + 16, NULL, MOD_LIGHTNING_DISCHARGE);

        return;
    }
// The SARACEN's Lightning Discharge - END

    trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );

    if ( tr.entityNum == ENTITYNUM_NONE )
    {
        return;
    }

    traceEnt = &g_entities[ tr.entityNum ];

    if ( traceEnt->takedamage && traceEnt->client )
    {
        tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
        tent->s.otherEntityNum = traceEnt->s.number;
        tent->s.eventParm = DirToByte( tr.plane.normal );
        tent->s.weapon = ent->s.weapon;
    }
    else if ( !( tr.surfaceFlags & SURF_NOIMPACT ) )
    {
        tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
        tent->s.eventParm = DirToByte( tr.plane.normal );
    }

    if ( traceEnt->takedamage)
    {
        G_Damage( traceEnt, ent, ent, forward, tr.endpos,
                  damage, 0, MOD_LIGHTNING);
    }
}
