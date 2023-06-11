/**************************************************************************************************************************
 *                                                                                                  		          *
 *                                    @;##@@@@@@##@@M9@				X-buster                                  *
 *                                ;@@A                 @@@s			by DonX and Peter J. Schroeder            *
 *                             ;@B;                       :@@;                                                            *
 *                           :@:                             ;@X,		Using the Quake III Engine                *
 *                        rG              @@@@@@@@              @#              Copyright (C) 1999-2000 Id Software, Inc. *
 *                       @@               #@    @@              .@@                                                       *
 *                     .@@s               @@    3@              S@@@:		Characters and setting based upon         *
 *                    .H@;           @@@G2i.     2iXG@@3           ,@@r         the Capcom series Mega Man © Capcom.      *
 *                   X@@r            @s               @@            .@@i   	All rights reserved.                      *
 *                  ,@9  .s@##@      @@               BB      rH@@3   A@,                                                 *
 *                 ;@@@M@@2@@@@      M@@@@@B     h@@@@@@       i@@@@. ,@@;	X-buster source code is free software     *
 *                 :@@@@@A5rs@@           @@    A@            @ir@@M@hh@@,      You can redistribute it or modify it      *
 *                 .@@@@Ah@@              ##    #B                @@@@@@@,      the terms of the GNU General Public       *
 *                 @@@@                   9@@@@@@i                  MHM@@,      License v3 as published by the            *
 *                i@@                                                @@@@i      Free Software Foundation                  *
 *               M@             .;SB@@@#AX5SissrsiX#2X5Siss             B@9                                               *
 *              ;@2           ,S@@@@A#                hMs,:;,            @G     You should have received a copy of the    *
 *             r@#,       ;i@@MXs                         &@s r:         @@:    GNU General Public License along with     *
 *            r@@   . .A#@@&r.        .2#3.      ;@@s          i2S.@@@    @@.   X-buster source code; if not, write to    *
 *          :#@@;@@,@@@@;            .h  @X.    ,#  @r           :;#M@@@@#@S@:  the Free Software Foundation, Inc., 51    *
 *            #@;    5@@h             r@@5      ,#@@s             ;@@@   @#A    51 Franklin St, Fifth Floor, Boston, MA   *
 *            @@     @#@@@.                                     r@@A@@   @G@    02110-1301  USA                           *
 *            @@#r:;;#@@@@h                 @X                @@@#@@@i.,2@@.                                              *
 *             3@@@@@@@@#;@@@9;.          ,2@@A            @33@@@@@@@@@@@@@                                               *
 *           :@@            ,BBSr      SM@     @@M      iB                s@r                                             *
 *         :@@@.                 :AHX @@;   @9  @@   G#                    @@;                                            *
 *        A@@                      @#Xs;;  @@@M  i@@;                        @@#.                                         *
 *     ,M@@                          s@   Ar2@#@  @@                           @@A                                        *
 *    :@@                              hh  XA@@  #s                              @@r                                      *
 *   :@@                                2@ 5@A @@,                                @@s                                     *
 *  @@                                  &@     @&                                   @@,                                   *
 * ,@@                                   3@@@@@r                                    #@                                    *
 * :@@                                G@@r   X&@3                                  B@&                                    *
 *   B@@@                      2@@@@@@s         @@@A:                         #@@@ss                                      *
 *    .,B@@@@@@@@@@HA@@@@@@@@@@Ar@3               .,B@@@@@@@@@@HA@@@@@@@@@@Ar@3                                           *
 *                                                                                                                        *
 **************************************************************************************************************************/

//#include "g_local.h"
//#include "be_aas.h"

extern	vec3_t	forward, right, up, muzzle;

gentity_t *fire_pbuster (gentity_t *self, vec3_t start, vec3_t dir);


/*
=================
fire_pbuster
=================
*/
gentity_t *fire_pbuster (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "pbuster";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    switch( self->client->ps.stats[STAT_CHARGE] )
    {
    case 0:
        bolt->damage = 05;
        bolt->splashDamage = 05;
        bolt->splashRadius = 0;
        bolt->methodOfDeath = MOD_PBUSTER;
        bolt->splashMethodOfDeath = MOD_PBUSTER;
        bolt->s.weapon = WP_PBUSTER;
        bolt->s.pos.trType = TR_LINEAR;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        VectorSet(bolt->r.mins, 0, 0, 0);
        VectorSet(bolt->r.maxs, 8, 8, 8);
        break;
    case 1:
        bolt->damage = 20;
        bolt->splashDamage = 10;
        bolt->splashRadius = 10;
        bolt->methodOfDeath = MOD_PBUSTER;
        bolt->splashMethodOfDeath = MOD_PBUSTER;
        bolt->s.weapon = WP_MBUSTER1;
        bolt->s.pos.trType = TR_LINEAR;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        break;
    case 2:
        bolt->damage = 30;
        bolt->splashDamage = 20;
        bolt->splashRadius = 30;
        bolt->methodOfDeath = MOD_PBUSTER;
        bolt->splashMethodOfDeath = MOD_PBUSTER;
        bolt->s.weapon = WP_PBUSTER2;
        bolt->s.pos.trType = TR_LINEAR;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        VectorSet(bolt->r.mins, -15, -40, -25);
        VectorSet(bolt->r.maxs, 15, 0, 25);
        break;
    }
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

//PBUSTER - levels 0-2
void Weapon_PBuster_Fire (gentity_t *ent)
{
    gentity_t	*m;
    m = fire_pbuster (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}

/* Ice Burst */

void Weapon_IceBurst_Fire (gentity_t *ent)
{
    gentity_t	*iceburst;
    vec3_t start;
    vec3_t offset;

    iceburst = G_Spawn();
    iceburst->classname = "IceBurst";
    iceburst->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    //iceburst->r.ownerNum = ent->s.number;
    iceburst->r.contents = CONTENTS_SOLID;
    iceburst->s.weapon = WP_IBURST;
    iceburst->s.modelindex = G_ModelIndex ("models/weapons/iceburst/iceburst.md3");

    VectorSet (iceburst->r.mins, -15, -15, -3);
    VectorSet (iceburst->r.maxs, 15, 15, 3);
    VectorCopy(iceburst->r.mins, iceburst->r.absmin);
    VectorCopy(iceburst->r.maxs, iceburst->r.absmax);

    AngleVectors (ent->client->ps.viewangles, forward, right, NULL);
    VectorSet(offset, 0+50,
              0,
              0+ent->client->ps.viewheight);
    G_ProjectSource (ent->client->ps.origin, offset, forward, right, start);
    G_SetOrigin( iceburst, start );
    vectoangles (forward, iceburst->s.angles);

    trap_LinkEntity(iceburst);

}

gentity_t *fire_imboomerang (gentity_t *self, vec3_t start, vec3_t dir )
{
    // G entity Info Intializations
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "imboomerang";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    //bolt->s.eFlags |= EF_BOUNCE_HALF;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
//	bolt->methodOfDeath = MOD_GDASH;
//	bolt->splashMethodOfDeath = MOD_GDASH;
    bolt->s.weapon = WP_IMBOOMERANG;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    //bolt->s.pos.trDuration = 200;
    bolt->s.pos.trTime = level.time; // sets the dynamically updating level.time into trTime ( or is it static ?)
    VectorCopy( start, bolt->s.pos.trBase ); // copies start (muzzle) vector to trBase
    VectorScale( dir, 700, bolt->s.pos.trDelta ); // creates vector point 700 units from dir and saves into trDelta
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth  - Snaps those vector values to grid
    VectorCopy (start, bolt->r.currentOrigin); // also saves the start (muzzle) vector to r.currentOrigin

    return bolt;
}
/*
ent -
muzzle - origin of spawn
forward - dir of view angle
*/
void Weapon_IMBoomerang_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_imboomerang (ent, muzzle, forward );
    //G_Boomerang ( ent, muzzle, forward );
}


gentity_t *fire_gdash (gentity_t *self, vec3_t start, vec3_t dir )
{
    //9 shards
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "gdash";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    //bolt->s.eFlags |= EF_BOUNCE_HALF;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
//	bolt->methodOfDeath = MOD_GDASH;
//	bolt->splashMethodOfDeath = MOD_GDASH;
//	bolt->s.weapon = WP_GDASH;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_GRAVITY;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_GDash_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_gdash (ent, muzzle, forward );
}


void gravwell_touch(gentity_t *ent, gentity_t *other, trace_t *trace )
{
    if (other->client && ent->r.ownerNum != other->s.number)
    {
        G_Damage (other, &g_entities[ent->r.ownerNum], &g_entities[ent->r.ownerNum], NULL, NULL,
                  100000, 0, MOD_GRAVWELL);
        other->client->ps.stats[STAT_HEALTH] = other->health = -999;
        player_die (other, &g_entities[ent->r.ownerNum], &g_entities[ent->r.ownerNum], 100000, MOD_GRAVWELL);
    }
}

void gravwell_think(gentity_t *gravwell)
{
    if ( !gravwell->health )
    {
        G_FreeEntity( gravwell );
        return;
    }

    G_AddEvent( gravwell, EV_GRAVITYWELL, 0 );
    gravwell->health --;
    gravwell->nextthink = level.time + FRAMETIME;

}

void Weapon_GravWell_Fire( gentity_t *ent )
{
    SET_BIT(ent->client->ps.eFlags, EF_GRAVWELL);
}

gentity_t *fire_spball (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "spball";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    //bolt->s.eFlags |= EF_BOUNCE_HALF;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
//	bolt->methodOfDeath = MOD_SPBALL;
//	bolt->splashMethodOfDeath = MOD_SPBALL;
//	bolt->s.weapon = WP_SPBALL;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_GRAVITY;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_SPBall_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_spball (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}

gentity_t *fire_risfire (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "risfire";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_RISFIRE;
    bolt->splashMethodOfDeath = MOD_RISFIRE;
    bolt->s.weapon = WP_RISFIRE;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_RISE;
    bolt->s.pos.trDuration = 1;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 1000, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_RisFire_Fire (gentity_t *ent)
{
    gentity_t	*m;
    m = fire_risfire (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}

gentity_t *fire_trthunder (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "trthunder";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 0;
    bolt->methodOfDeath = MOD_TRTHUNDER;
    bolt->splashMethodOfDeath = MOD_TRTHUNDER;
    bolt->s.weapon = WP_TRTHUNDER;
    bolt->s.pos.trType = TR_LINEAR;
    VectorScale( dir, 1000, bolt->s.pos.trDelta );
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 8, 8, 8);
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_TriadThunder_Fire (gentity_t *ent)
{
    gentity_t	*m;
    m = fire_trthunder (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
    G_SpreadProjectile( ent, fire_trthunder );
}
