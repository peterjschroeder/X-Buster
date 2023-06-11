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

extern	vec3_t	forward, right, up, muzzle;

gentity_t *fire_mbuster (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_hwave (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_fburst (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_awave (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_rstone (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_blauncher (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_lstorm (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_mmine (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_sshredder (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_bshield (gentity_t *self, vec3_t start, vec3_t dir );

//MBUSTER - levels 0-2
void Weapon_MBuster_Fire (gentity_t *ent)
{
    gentity_t	*m;
    m = fire_mbuster (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_mbuster
=================
*/
gentity_t *fire_mbuster (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "mbuster";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    switch( self->client->ps.stats[STAT_CHARGE] )
    {
    case 0:
        bolt->damage = 10;
        bolt->splashDamage = 10;
        bolt->splashRadius = 10;
        bolt->methodOfDeath = MOD_MBUSTER;
        bolt->splashMethodOfDeath = MOD_MBUSTER;
        bolt->s.weapon = WP_MBUSTER;
        bolt->s.pos.trType = TR_LINEAR;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        VectorSet(bolt->r.mins, 0, 0, 0);
        VectorSet(bolt->r.maxs, 8, 8, 8);
        break;
    case 1:
        bolt->damage = 15;
        bolt->splashDamage = 8;
        bolt->splashRadius = 20;
        bolt->methodOfDeath = MOD_MBUSTER;
        bolt->splashMethodOfDeath = MOD_MBUSTER;
        bolt->s.weapon = WP_MBUSTER1;
        bolt->s.pos.trType = TR_LINEAR;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        break;
    case 2:
        bolt->damage = 25;
        bolt->splashDamage = 15;
        bolt->splashRadius = 30;
        bolt->methodOfDeath = MOD_MBUSTER;
        bolt->splashMethodOfDeath = MOD_MBUSTER;
        bolt->s.weapon = WP_MBUSTER2;
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



void Weapon_HWave_Fire (gentity_t *ent)
{
    gentity_t	*m;
    m = fire_hwave (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_hwave
=================
*/
gentity_t *fire_hwave (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "hwave";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_HWAVE;
    bolt->splashMethodOfDeath = MOD_HWAVE;
    bolt->s.weapon = WP_HWAVE;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_GRAVITY;
    bolt->s.pos.trDuration = 1;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 1000, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
/*
=================
fire_hwave2
=================
*/
gentity_t *fire_hwave2 (gentity_t *self, vec3_t start, vec3_t dir, int number)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "hwave";
    bolt->nextthink = level.time + 100;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_HWAVE;
    bolt->splashMethodOfDeath = MOD_HWAVE;
    bolt->s.weapon = WP_HWAVE2;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 100*number, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_FBurst_Fire (gentity_t *ent)
{
    gentity_t	*m;
    m = fire_fburst (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_fburst
=================
*/
gentity_t *fire_fburst (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "fburst";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_FBURST;
    bolt->splashMethodOfDeath = MOD_FBURST;
    bolt->s.weapon = WP_FBURST;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    // VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_AWave_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_awave (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_awave (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "awave";
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
    bolt->methodOfDeath = MOD_AWAVE;
    bolt->splashMethodOfDeath = MOD_AWAVE;
    bolt->s.weapon = WP_AWAVE;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, -40, -40, -40);
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

void Weapon_RStone_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_rstone (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_rstone (gentity_t *self, vec3_t start, vec3_t dir )
{
    //9 shards
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "rstone";
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
    bolt->methodOfDeath = MOD_RSTONE;
    bolt->splashMethodOfDeath = MOD_RSTONE;
    bolt->s.weapon = WP_RSTONE;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, -18, -18, -18);
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

void Weapon_BLauncher_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_blauncher (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_blauncher (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "blauncher";
    //### NEW ###
    //bolt->nextthink = level.time + 10000;
    //bolt->think = G_ExplodeMissile;
    if (self->client)		// && check for bots
    {
        bolt->think = G_GuideMissile;
        bolt->nextthink = level.time + FRAMETIME;
        bolt->wait = level.time + 10000;
    }
    else
    {
        bolt->nextthink = level.time + 10000;
        bolt->think = G_ExplodeMissile;
    }
    //### END NEW ###
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_BLAUNCHER;
    bolt->splashMethodOfDeath = MOD_BLAUNCHER;
    bolt->s.weapon = WP_BLAUNCHER;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 400, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_LStorm_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_lstorm (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_lstorm (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "lstorm";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_LSTORM;
    bolt->splashMethodOfDeath = MOD_LSTORM;
    bolt->s.weapon = WP_LSTORM;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_MMine_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_mmine (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_mmine (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "mmine";
    bolt->nextthink = level.time + 20;
    bolt->think = G_HomeMissile2;
    bolt->wait = level.time + 10000;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 30;
    bolt->splashDamage = 30;
    bolt->splashRadius = 100;
    bolt->methodOfDeath = MOD_MMINE;
    bolt->splashMethodOfDeath = MOD_MMINE;
    bolt->s.weapon = WP_MMINE;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    //VectorSet(bolt->r.mins, 0, 0, 0);
    //VectorSet(bolt->r.maxs, 20, 20, 20);
    //VectorCopy(bolt->r.mins, bolt->r.absmin);
    //VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 160, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_SShredder_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_sshredder (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_sshredder (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "lstorm";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_SSHREDDER;
    bolt->splashMethodOfDeath = MOD_SSHREDDER;
    bolt->s.weapon = WP_SSHREDDER;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_BShield_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_bshield (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_bshield (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "bshield";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->s.eFlags |= EF_BOUNCE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_BSHIELD;
    bolt->splashMethodOfDeath = MOD_BSHIELD;
    bolt->s.weapon = WP_BSHIELD;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    /*VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);*/

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
