// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_misc.c

#include "g_local.h"
#include "../ui/ui_public.h"
#include "be_aas.h"


/*QUAKED func_group (0 0 0) ?
Used to group brushes together just for editor convenience.  They are turned into normal brushes by the utilities.
*/


/*QUAKED info_camp (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for calculations in the utilities (spotlights, etc), but removed during gameplay.
*/
void SP_info_camp( gentity_t *self )
{
    G_SetOrigin( self, self->s.origin );
}


/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for calculations in the utilities (spotlights, etc), but removed during gameplay.
*/
void SP_info_null( gentity_t *self )
{
    G_FreeEntity( self );
}


/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for in-game calculation, like jumppad targets.
target_position does the same thing
*/
void SP_info_notnull( gentity_t *self )
{
    G_SetOrigin( self, self->s.origin );
}


/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) linear
Non-displayed light.
"light" overrides the default 300 intensity.
Linear checbox gives linear falloff instead of inverse square
Lights pointed at a target will be spotlights.
"radius" overrides the default 64 unit radius of a spotlight at the target point.
*/
void SP_light(gentity_t * self)
{
    G_FreeEntity(self);
}


/*
=================================================================================

TELEPORTERS

=================================================================================
*/

void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles )
{
    gentity_t	*tent;

    // use temp events at source and destination to prevent the effect
    // from getting dropped by a second player event
    if ( player->client->sess.sessionTeam != TEAM_SPECTATOR )
    {
        tent = G_TempEntity( player->client->ps.origin, EV_PLAYER_TELEPORT_OUT );
        tent->s.clientNum = player->s.clientNum;

        if ( g_cheats.integer && player->poofout)
            G_Sound(player, CHAN_AUTO, G_SoundIndex(va("sound/%s", player->poofout)));
        else
            G_Sound(player, CHAN_AUTO, G_SoundIndex("sound/world/teleout.wav"));


        tent = G_TempEntity( origin, EV_PLAYER_TELEPORT_IN );
        tent->s.clientNum = player->s.clientNum;
    }

    if ( g_cheats.integer && player->poofin)
        G_Sound(player, CHAN_AUTO, G_SoundIndex(va("sound/%s", player->poofin)));
    else
        G_Sound(player, CHAN_AUTO, G_SoundIndex("sound/world/telein.wav"));

    // unlink to make sure it can't possibly interfere with G_KillBox
    trap_UnlinkEntity (player);

    VectorCopy ( origin, player->client->ps.origin );
    player->client->ps.origin[2] += 1;

    // spit the player out
    AngleVectors( angles, player->client->ps.velocity, NULL, NULL );
    VectorScale( player->client->ps.velocity, 400, player->client->ps.velocity );
    player->client->ps.pm_time = 160;		// hold time
    player->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

    // toggle the teleport bit so the client knows to not lerp
    player->client->ps.eFlags ^= EF_TELEPORT_BIT;

    // set angles
    SetClientViewAngle( player, angles );

    // kill anything at the destination
    if ( player->client->sess.sessionTeam != TEAM_SPECTATOR )
    {
        G_KillBox (player);
    }

    // save results of pmove
    BG_PlayerStateToEntityState( &player->client->ps, &player->s, qtrue );

    // use the precise origin for linking
    VectorCopy( player->client->ps.origin, player->r.currentOrigin );

    if ( player->client->sess.sessionTeam != TEAM_SPECTATOR )
    {
        trap_LinkEntity (player);
    }
}


/*QUAKED misc_teleporter_dest (1 0 0) (-32 -32 -24) (32 32 -16)
Point teleporters at these.
Now that we don't have teleport destination pads, this is just
an info_notnull
*/
void SP_misc_teleporter_dest( gentity_t *ent )
{
}


//===========================================================

/*QUAKED misc_model (1 0 0) (-16 -16 -16) (16 16 16)
"model"		arbitrary .md3 file to display
*/
void SP_misc_model( gentity_t *ent )
{

#if 0
    ent->s.modelindex = G_ModelIndex( ent->model );
    VectorSet (ent->mins, -16, -16, -16);
    VectorSet (ent->maxs, 16, 16, 16);
    trap_LinkEntity (ent);

    G_SetOrigin( ent, ent->s.origin );
    VectorCopy( ent->s.angles, ent->s.apos.trBase );
#else
    G_FreeEntity( ent );
#endif
}

//===========================================================

void locateCamera( gentity_t *ent )
{
    vec3_t		dir;
    gentity_t	*target;
    gentity_t	*owner;

    owner = G_PickTarget( ent->target );
    if ( !owner )
    {
        G_Printf( "Couldn't find target for misc_partal_surface\n" );
        G_FreeEntity( ent );
        return;
    }
    ent->r.ownerNum = owner->s.number;

    // frame holds the rotate speed
    if ( owner->spawnflags & 1 )
    {
        ent->s.frame = 25;
    }
    else if ( owner->spawnflags & 2 )
    {
        ent->s.frame = 75;
    }

    // swing camera ?
    if ( owner->spawnflags & 4 )
    {
        // set to 0 for no rotation at all
        ent->s.powerups = 0;
    }
    else
    {
        ent->s.powerups = 1;
    }

    // clientNum holds the rotate offset
    ent->s.clientNum = owner->s.clientNum;

    VectorCopy( owner->s.origin, ent->s.origin2 );

    // see if the portal_camera has a target
    target = G_PickTarget( owner->target );
    if ( target )
    {
        VectorSubtract( target->s.origin, owner->s.origin, dir );
        VectorNormalize( dir );
    }
    else
    {
        G_SetMovedir( owner->s.angles, dir );
    }

    ent->s.eventParm = DirToByte( dir );
}

/*QUAKED misc_portal_surface (0 0 1) (-8 -8 -8) (8 8 8)
The portal surface nearest this entity will show a view from the targeted misc_portal_camera, or a mirror view if untargeted.
This must be within 64 world units of the surface!
*/
void SP_misc_portal_surface(gentity_t *ent)
{
    VectorClear( ent->r.mins );
    VectorClear( ent->r.maxs );
    trap_LinkEntity (ent);

    ent->r.svFlags = SVF_PORTAL;
    ent->s.eType = ET_PORTAL;

    if ( !ent->target )
    {
        VectorCopy( ent->s.origin, ent->s.origin2 );
    }
    else
    {
        ent->think = locateCamera;
        ent->nextthink = level.time + 100;
    }
}

/*QUAKED misc_portal_camera (0 0 1) (-8 -8 -8) (8 8 8) slowrotate fastrotate noswing
The target for a misc_portal_director.  You can set either angles or target another entity to determine the direction of view.
"roll" an angle modifier to orient the camera around the target vector;
*/
void SP_misc_portal_camera(gentity_t *ent)
{
    float	roll;

    VectorClear( ent->r.mins );
    VectorClear( ent->r.maxs );
    trap_LinkEntity (ent);

    G_SpawnFloat( "roll", "0", &roll );

    ent->s.clientNum = roll/360.0 * 256;
}

/*
======================================================================

  SHOOTERS

======================================================================
*/

void Use_Shooter( gentity_t *ent, gentity_t *other, gentity_t *activator )
{
    vec3_t		dir;
    float		deg;
    vec3_t		up, right;

    // see if we have a target
    if ( ent->enemy )
    {
        VectorSubtract( ent->enemy->r.currentOrigin, ent->s.origin, dir );
        VectorNormalize( dir );
    }
    else
    {
        VectorCopy( ent->movedir, dir );
    }

    // randomize a bit
    PerpendicularVector( up, dir );
    CrossProduct( up, dir, right );

    deg = crandom() * ent->random;
    VectorMA( dir, deg, up, dir );

    deg = crandom() * ent->random;
    VectorMA( dir, deg, right, dir );

    VectorNormalize( dir );

    switch ( ent->s.weapon )
    {
    case WP_BLAUNCHER:
        fire_blauncher( ent, ent->s.origin, dir );
        break;
    case WP_LTORPEDO:
        fire_ltorpedo( ent, ent->s.origin, dir );
        break;
    case WP_XBUSTER:
        fire_xbuster( ent, ent->s.origin, dir );
        break;
    case WP_PSTRIKE:
        fire_pstrike( ent, ent->s.origin, dir );
        break;
        //case WP_BLASER:
        //	fire_blaser( ent, ent->s.origin, dir );
        //	break;
        //case WP_ASPLASHER:
        //	fire_asplasher( ent, ent->s.origin, dir );
        //	break;
        //case WP_SBURST:
        //	fire_sburst( ent, ent->s.origin, dir );
        //	break;
    }
    G_AddEvent( ent, EV_FIRE_WEAPON, 0 );
}


static void InitShooter_Finish( gentity_t *ent )
{
    ent->enemy = G_PickTarget( ent->target );
    ent->think = 0;
    ent->nextthink = 0;
}

void InitShooter( gentity_t *ent, int weapon )
{
    ent->use = Use_Shooter;
    ent->s.weapon = weapon;

    RegisterItem( BG_FindItemForWeapon( weapon ) );

    G_SetMovedir( ent->s.angles, ent->movedir );

    if ( !ent->random )
    {
        ent->random = 1.0;
    }
    ent->random = sin( M_PI * ent->random / 180 );
    // target might be a moving object, so we can't set movedir for it
    if ( ent->target )
    {
        ent->think = InitShooter_Finish;
        ent->nextthink = level.time + 500;
    }
    trap_LinkEntity( ent );
}

/*QUAKED shooter_rocket (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_rocket( gentity_t *ent )
{
    InitShooter( ent, WP_LTORPEDO );
}

/*QUAKED shooter_plasma (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" is the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_plasma( gentity_t *ent )
{
    InitShooter( ent, WP_XBUSTER);
}

/*QUAKED shooter_grenade (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" is the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_grenade( gentity_t *ent )
{
    InitShooter( ent, WP_BLAUNCHER);
}

/*QUAKED shooter_pstrike (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" is the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_pstrike( gentity_t *ent )
{
    InitShooter( ent, WP_PSTRIKE);
}

/*QUAKED shooter_blaser (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" is the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_blaser( gentity_t *ent )
{
    InitShooter( ent, WP_BLASER);
}

/*
==============
Touch_Capsule

===============
*/
void Touch_Capsule(gentity_t *ent, gentity_t *other, trace_t *trace )
{
    if( !IS_SET( other->r.svFlags, SVF_BOT ) && other->client->menuTime < level.time )
    {
        SET_BIT( other->client->ps.stats[STAT_FLAGS], SF_CAPSULE );
        trap_SendServerCommand( other-g_entities, va("menu %d", UIMENU_CAPSULE) );
        other->client->ps.weaponstate = WEAPON_READY;
        other->client->ps.powerups[PW_CLOAK] = INT_MAX;
        other->client->noclip = qtrue;
    }
}


/*QUAKED func_capsule (1 0 0) (-16 -16 -16) (16 16 16)
 Explodes glass
 */
void SP_func_capsule( gentity_t *ent )
{
    // Make it appear as the brush
    trap_SetBrushModel( ent, ent->model );
    ent->takedamage = qfalse;
    ent->s.eType = ET_INVISIBLE;
    ent->r.contents = CONTENTS_TRIGGER;
    ent->touch = Touch_Capsule;

    // If the mapper gave it a model, use it
    /*if ( ent->model2 ) {
        ent->s.modelindex2 = G_ModelIndex( ent->model2 );
    }*/
    // Link all ^this^ info into the ent
    trap_LinkEntity (ent);
}


void Touch_Bank(gentity_t *ent, gentity_t *other, trace_t *trace )
{
    if( other->client->menuTime < level.time )
    {
        SET_BIT( other->client->ps.stats[STAT_FLAGS], SF_BANK );
        trap_SendServerCommand( other-g_entities, va("menu %d", UIMENU_BANK ) );
        other->client->ps.weaponstate = WEAPON_READY;
        other->client->ps.powerups[PW_CLOAK] = INT_MAX;
        other->client->noclip = qtrue;
    }
}

void SP_func_bank( gentity_t *ent )
{
    trap_SetBrushModel( ent, ent->model );
    ent->takedamage = qfalse;
    ent->s.eType = ET_INVISIBLE;
    ent->r.contents = CONTENTS_TRIGGER;
    ent->touch = Touch_Bank;

    trap_LinkEntity (ent);
}

void Touch_Garage(gentity_t *ent, gentity_t *other, trace_t *trace )
{
    if( other->client->menuTime < level.time )
    {
        SET_BIT( other->client->ps.stats[STAT_FLAGS], SF_GARAGE );
        trap_SendServerCommand( other-g_entities, va("menu %d", UIMENU_VEHICLES ) );
        other->client->ps.weaponstate = WEAPON_READY;
        other->client->ps.powerups[PW_CLOAK] = INT_MAX;
        other->client->noclip = qtrue;
    }
}

void SP_func_garage( gentity_t *ent )
{
    trap_SetBrushModel( ent, ent->model );
    ent->takedamage = qfalse;
    ent->s.eType = ET_INVISIBLE;
    ent->r.contents = CONTENTS_TRIGGER;
    ent->touch = Touch_Garage;

    trap_LinkEntity (ent);
}

/*QUAKED func_breakable (1 0 0) (-16 -16 -16) (16 16 16)
 Explodes glass
 */
void SP_func_breakable( gentity_t *ent )
{
    int health;

    // Make it appear as the brush
    trap_SetBrushModel( ent, ent->model );
    // Lets give it 200 health if the mapper did not set its health
    G_SpawnInt( "health", "0", &health );
    if( health <= 0 )
        health = 200;

    ent->health = health;
    // Let it take damage
    ent->takedamage = qtrue;
    // Let it know it is a breakable object
    ent->s.eType = ET_BREAKABLE;
    // If the mapper gave it a model, use it
    if ( ent->model2 )
    {
        ent->s.modelindex2 = G_ModelIndex( ent->model2 );
    }
    // Link all ^this^ info into the ent
    trap_LinkEntity (ent);
}

/*
 =================
 G_BreakGlass
 =================
 */
void G_BreakGlass(gentity_t *ent, vec3_t point, int mod)
{
    gentity_t   *tent = NULL;
    vec3_t      size;
    vec3_t      center;
    qboolean    splashdmg;

    // Get the center of the glass
    VectorSubtract(ent->r.maxs, ent->r.mins, size);
    VectorScale(size, 0.5, size);
    VectorAdd(ent->r.mins, size, center);

    // If the glass has no more life, BREAK IT
    if( ent->health <= 0 )
    {
        G_FreeEntity( ent );
        // Tell the program based on the gun if it has no splash dmg, no reason to ad ones with
        // splash dmg as qtrue as is that is the default
        switch( mod )
        {
        case MOD_GAUNTLET:
            splashdmg = qfalse;
            break;
        case MOD_MACHINEGUN:
            splashdmg = qfalse;
            break;
        case MOD_RAILGUN:
            splashdmg = qfalse;
            break;
        case MOD_LIGHTNING:
            splashdmg = qfalse;
            break;
        default:
            splashdmg = qtrue;
            break;
        }
        // Call the function to show the glass shards in cgame
        // center can be changed to point which will spawn the
        // where the killing bullet hit but wont work with Splash Damage weapons
        // so I just use the center of the glass
        switch( splashdmg )
        {
        case qtrue:
            tent = G_TempEntity( center, EV_BREAK_GLASS );
            break;
        case qfalse:
            tent = G_TempEntity( point, EV_BREAK_GLASS );
            break;
        }
        tent->s.eventParm = 0;
    }
}

void FinishSpawningMD3Anim( gentity_t *ent )
{
    trace_t		tr;
    vec3_t		dest;
    ent->s.modelindex = G_ModelIndex( ent->model );
    ent->s.eType = ET_MODELANIM;
    if ( ! (ent->spawnflags & 4 ))
    {
        // suspended
        G_SetOrigin( ent, ent->s.origin );
    }
    else
    {
        // drop to floor
        ent->r.contents = CONTENTS_BODY;
        VectorSet( ent->r.mins, -24, -24, -24 );
        VectorSet( ent->r.maxs, 24, 24, 24 );
        {
            char modelName[MAX_QPATH];
            char		fname[MAX_QPATH];
            int			len;
            char		text[10000],*text_p;
            fileHandle_t	f;
            trap_GetConfigstring( CS_MODELS+ent->s.modelindex ,modelName,sizeof(modelName));
            strcpy(fname,modelName);
            text_p=fname+strlen(fname);
            while (*text_p!='.' && *text_p!='\\' && text_p!=fname)
                text_p--;
            if (*text_p=='.')
                *text_p=0;
            Q_strcat(fname,sizeof(fname),".cfg");
            len = trap_FS_FOpenFile( fname, &f, FS_READ );
            if ( len <= 0 )
            {
                G_Printf( "No config file found for %s , defaults will be used.\n",modelName);
                goto read_times;
            }
            if ( len >= sizeof( text ) - 1 )
            {
                G_Printf( "File %s too long\n", fname );
                goto read_times;
            }
            trap_FS_Read( text, len, f );
            text[len] = 0;
            trap_FS_FCloseFile( f );
            text_p=text;
            Parse1DMatrix (&text_p, 3, ent->r.mins);
            Parse1DMatrix (&text_p, 3, ent->r.maxs);
read_times:
            ;
        }
        VectorSet( dest, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] - 4096 );
        trap_Trace( &tr, ent->s.origin, ent->r.mins, ent->r.maxs, dest, ent->s.number, MASK_SOLID );
        if ( tr.startsolid )
        {
            G_Printf( "FinishSpawningMD3Anim: %s startsolid at %s\n",
                      ent->classname, vtos(ent->s.origin));
            G_FreeEntity( ent );
            return;
        }
        // allow to ride movers
        ent->s.groundEntityNum = tr.entityNum;
        G_SetOrigin( ent, tr.endpos );
    }
    trap_LinkEntity (ent);
}

void ActivateAnimModel(gentity_t *ent,gentity_t *other,gentity_t *activator)
{
    ent->s.modelindex2++;
}

/*QUAKED misc_model_anim (1 0 0) (-16 -16 -16) (16 16 16) HIDDEN_START HIDDEN_END NOTSUSPENDED
Animated MD3, select HIDDEN_START for the model to be hidden until de animation
 is triggered, and HIDDEN_END for disappearing after it's finished.
"model"		arbitrary .md3 file to display/animate
"speed"		speed scale (not less than 1.0) - used to scale ".anim" fps data (default = 1)
"wait"		1=wait for trigger activation  0=animate w/o waiting (default = 0)
*/

void SP_misc_model_anim( gentity_t *ent )
{
    G_SpawnFloat( "speed", "1.0", &ent->speed);
    if (ent->speed<1) ent->speed=1;
    VectorCopy( ent->s.angles, ent->s.apos.trBase );
    ent->nextthink = level.time + FRAMETIME * 2;
    ent->think = FinishSpawningMD3Anim;
    G_SpawnFloat( "wait", "0", &ent->wait);
    ent->s.modelindex2=0;
    ent->s.generic1=ent->spawnflags + ((int)(ent->speed * 16) & 0xF0);	// solo 3 flags
    if (ent->wait==0)
        ActivateAnimModel(ent,ent,ent);
    else
        ent->use = ActivateAnimModel;
}

void G_RunMD3Anim( gentity_t *ent )
{
    vec3_t		origin;
    trace_t		tr;
    int			contents;
    int			mask;
    // if groundentity has been set to -1, it may have been pushed off an edge
    if ( ent->s.groundEntityNum == -1 )
    {
        if ( ent->s.pos.trType != TR_GRAVITY )
        {
            ent->s.pos.trType = TR_GRAVITY;
            ent->s.pos.trTime = level.time;
        }
    }
    if ( ent->s.pos.trType == TR_STATIONARY )
    {
        // check think function
        G_RunThink( ent );
        return;
    }
    // get current position
    BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
    // trace a line from the previous position to the current position
    if ( ent->clipmask )
    {
        mask = ent->clipmask;
    }
    else
    {
        mask = MASK_PLAYERSOLID & ~CONTENTS_BODY;//MASK_SOLID;
    }
    trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin,
                ent->r.ownerNum, mask );
    VectorCopy( tr.endpos, ent->r.currentOrigin );
    if ( tr.startsolid )
    {
        tr.fraction = 0;
    }
    trap_LinkEntity( ent );	// FIXME: avoid this for stationary?
    // check think function
    G_RunThink( ent );
    if ( tr.fraction == 1 )
    {
        return;
    }
    // if it is in a nodrop volume, remove it
    contents = trap_PointContents( ent->r.currentOrigin, -1 );
    if ( contents & CONTENTS_NODROP )
    {
        G_FreeEntity( ent );
        return;
    }
}

void PainRoom_Think (gentity_t *ent)
{
    int j;

    for (j = 0; j < level.maxclients; j++)
    {
        if (!g_entities[j].inuse || !g_entities[j].client ||
                !in_same_room(ent, &g_entities[j]) || !g_entities[j].takedamage)
            continue;

        G_Damage (&g_entities[j], ent, ent, NULL, NULL, ent->damage, DAMAGE_NO_PROTECTION, MOD_TRIGGER_HURT);
    }

}

void SP_func_painroom (gentity_t *ent)
{
    ent->think = PainRoom_Think;
    ent->nextthink = level.time + 2000;
}

void RegenRoom_Think (gentity_t *ent)
{
    int j;

    for (j = 0; j < level.maxclients; j++)
    {
        if (!g_entities[j].inuse || !g_entities[j].client ||
                !in_same_room(ent, &g_entities[j]) || !g_entities[j].takedamage)
            continue;

        if ( g_entities[j].health < g_entities[j].client->ps.stats[STAT_MAX_HEALTH])
            g_entities[j].health += ent->damage;

        if ( g_entities[j].health > g_entities[j].client->ps.stats[STAT_MAX_HEALTH])
            g_entities[j].health = g_entities[j].client->ps.stats[STAT_MAX_HEALTH];
    }
}

void SP_func_regenroom (gentity_t *ent)
{
    ent->think = RegenRoom_Think;
    ent->nextthink = level.time + 2000;
}

// NIGHTZ - EXPLOSIVE
void SP_func_explosive (gentity_t *ent)
{
    int health;
    int respawn;
    int radius;
    int damage;

    trap_SetBrushModel (ent, ent->model);

    // if we specified a model, use it here =)
    if (ent->model2)
        ent->s.modelindex2 = G_ModelIndex (ent->model2);

    G_SpawnInt( "health", "0", &health );
    G_SpawnInt( "respawn", "0", &respawn );
    G_SpawnInt( "radius", "0", &radius );
    G_SpawnInt( "damage", "0", &damage );

    if (!ent->health)
    {
        health = 50;
        ent->health = health;
    }

    if (!ent->respawn)
    {
        respawn = 5000;
        ent->respawn = respawn;
    }

    if (!ent->damage)
    {
        damage = 1000;
        ent->damage = damage;
    }

    if (!ent->radius)
    {
        radius = 200;
        ent->radius = radius;
    }

//	ent->maxHealth = ent->health;
    ent->takedamage = qtrue;
    ent->s.pos.trType = TR_STATIONARY;
    ent->s.eType = ET_EXPLOSIVE;
    ent->neverFree = qtrue;

    trap_LinkEntity (ent);
}



void misc_sunlight_think (gentity_t *self)
{
    int      r, g, b, i;
    vec3_t      color;
    qboolean colorSet;

    VectorCopy(self->s.pos.trBase, self->pos1);
    VectorCopy(self->s.pos.trBase, self->pos2);
    VectorCopy(self->s.pos.trBase, self->s.pos.trDelta);
    VectorCopy(self->s.origin, self->s.origin2);

    colorSet = G_SpawnVector( "color", "255 255 255", color );

    switch (level.hours)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 10;
        break;
    case 3:
        i = 30;
        break;
    case 4:
        i = 50;
        break;
    case 5:
        i = 70;
        break;
    case 6:
        i = 90;
        break;
    case 7:
        i = 110;
        break;
    case 8:
        i = 130;
        break;
    case 9:
        i = 150;
        break;
    case 10:
        i = 170;
        break;
    case 11:
        i = 190;
        break;
    case 12:
        i = 210;
        break;
    case 13:
        i = 230;
        break;
    case 14:
        i = 255;
        break;
    case 15:
        i = 230;
        break;
    case 16:
        i = 200;
        break;
    case 17:
        i = 170;
        break;
    case 18:
        i = 140;
        break;
    case 19:
        i = 110;
        break;
    case 20:
        i = 80;
        break;
    case 21:
        i = 50;
        break;
    case 22:
        i = 30;
        break;
    case 23:
        i = 10;
        break;
    case 24:
    default:
        i = 0;
        break;
    }

    if ( colorSet )
    {

        r = color[0] * 255;
        if ( r > 255 )
            r = 255;

        g = color[1] * 255;
        if ( g > 255 )
            g = 255;

        b = color[2] * 255;
        if ( b > 255 )
            b = 255;
    }
    else
    {
        r = 1;
        g = 1;
        b = 1;
    }

    self->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );
    self->s.generic1 = 1;
    trap_LinkEntity (self);
    self->nextthink = level.time + FRAMETIME;

}


void SP_sunlight (gentity_t *self)
{
    self->think = misc_sunlight_think;
    self->nextthink = level.time + FRAMETIME;
}
