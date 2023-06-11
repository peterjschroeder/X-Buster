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

gentity_t *fire_xbuster ( gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_xbuster3 ( gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_fcutter ( gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_fcutter2 ( gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_sice ( gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_sice3 ( gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_afist ( gentity_t *self, vec3_t start, vec3_t dir, float speed );
gentity_t *fire_afist2 ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_bcrystal ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_bcrystal2 ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_ltorpedo ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_ltorpedo2 ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_tburster ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_gbomb ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_gbomb2 ( gentity_t *self, vec3_t start, vec3_t dir );
void	   fire_blaser ( gentity_t *ent );
gentity_t *fire_blaser2 ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_sfist ( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_sfist2 ( gentity_t *self, vec3_t start, vec3_t dir );


//XBUSTER - levels 0-4
void Weapon_XBuster_Fire (gentity_t *ent)
{
    int			count;
    gentity_t	*m;

    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )

        m = fire_xbuster (ent, muzzle, forward );

    else   //charge level is 3 or 4
    {
        for( count = 0; count < 15; count++ )
        {
            m = fire_xbuster3 (ent, muzzle, forward );
            m->s.pos.trDuration = count;
            VectorSet(m->r.mins, -8, -8, -8);
            VectorSet(m->r.maxs, 8, 8, 8);
            VectorCopy(m->r.mins, m->r.absmin);
            VectorCopy(m->r.maxs, m->r.absmax);
        }
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_xbuster
=================
*/
gentity_t *fire_xbuster (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "xbuster";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->splashMethodOfDeath = MOD_XBUSTER;
    bolt->s.pos.trType = TR_LINEAR;
    VectorScale( dir, 800, bolt->s.pos.trDelta );
    switch( self->client->ps.stats[STAT_CHARGE] )
    {
    case 0:
        bolt->damage = 10;
        bolt->splashDamage = 10;
        bolt->splashRadius = 10;
        bolt->s.weapon = WP_XBUSTER;
        VectorSet(bolt->r.mins, 0, 0, 0);
        VectorSet(bolt->r.maxs, 8, 8, 8);
        break;
    case 1:
        bolt->damage = 15;
        bolt->splashDamage = 8;
        bolt->splashRadius = 20;
        bolt->s.weapon = WP_XBUSTER1;
        VectorSet(bolt->r.mins, 0, 0, 0);
        VectorSet(bolt->r.maxs, 8, 0, 8);
        break;
    case 2:
        bolt->damage = 25;
        bolt->splashDamage = 15;
        bolt->splashRadius = 30;
        bolt->s.weapon = WP_XBUSTER2;
        VectorSet(bolt->r.mins, 0, 0, 0);
        VectorSet(bolt->r.maxs, 10, 0, 10);
        //VectorSet(bolt->r.mins, -15, -40, -25);
        //VectorSet(bolt->r.maxs, 15, 0, 25);
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
/*
=================
fire_xbuster3
=================
*/
gentity_t *fire_xbuster3 (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "xbuster";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 15;
    bolt->splashDamage = 25;
    bolt->splashRadius = 15;
    bolt->methodOfDeath = MOD_XBUSTER2;
    bolt->splashMethodOfDeath = MOD_XBUSTER2;
    bolt->s.weapon = WP_XBUSTER3;
    bolt->s.pos.trType = TR_HELIX;
    bolt->clipmask = MASK_SHOT;
    //bolt->r.contents = CONTENTS_BODY;

    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_FCutter_Fire (gentity_t *ent)
{
    int			count;
    gentity_t	*m;
    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_fcutter (ent, muzzle, forward );
        //G_Printf( "velocity vector is <%f, %f, %f>\n", ent->client->ps.velocity[0], ent->client->ps.velocity[1], ent->client->ps.velocity[2]);
        //VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
    }
    else     //charge level is 3 or 4
    {
        for( count = 0; count < 6; count++ )
        {
            m = fire_fcutter2 (ent, muzzle, forward );
            m->s.pos.trDuration = count;
        }
    }
    //VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_fcutter
=================
*/
gentity_t *fire_fcutter (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "fcutter";
    bolt->nextthink = level.time + 1000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 5;
    bolt->splashDamage = 5;
    bolt->splashRadius = 8;
    bolt->methodOfDeath = MOD_FCUTTER;
    bolt->splashMethodOfDeath = MOD_FCUTTER;
    bolt->s.weapon = WP_FCUTTER;
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
    VectorScale( dir, 200, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
/*
=================
fire_fcutter2
=================
*/
gentity_t *fire_fcutter2 (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "fcutter";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 25;
    bolt->splashDamage = 20;
    bolt->splashRadius = 15;
    bolt->methodOfDeath = MOD_FCUTTER2;
    bolt->splashMethodOfDeath = MOD_FCUTTER2;
    bolt->s.weapon = WP_FCUTTER2;
    bolt->s.pos.trType = TR_HELIX2;
    bolt->clipmask = MASK_SHOT;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 15, 15, 15);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 800, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_SIce_Fire (gentity_t *ent)
{
    gentity_t	*m;

    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_sice (ent, muzzle, forward );
    }
    else
    {
        m = fire_sice3 (ent, muzzle, forward );
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_sice
=================
*/
gentity_t *fire_sice (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "sice";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_SICE;
    bolt->splashMethodOfDeath = MOD_SICE;
    bolt->s.weapon = WP_SICE;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trDuration = 1;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
/*
=================
fire_sice2
=================
*/
gentity_t *fire_sice2 (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "sice";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_SICE;
    bolt->splashMethodOfDeath = MOD_SICE;
    bolt->s.weapon = WP_SICE2;
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
/*
=================
fire_sice3
=================
*/
gentity_t *fire_sice3 (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "sice";
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
    bolt->methodOfDeath = MOD_SICE2;
    bolt->splashMethodOfDeath = MOD_SICE2;
    bolt->s.weapon = WP_SICE3;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 40, 40, 40);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_GRAVITY;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 600, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_AFist_Fire( gentity_t *ent )
{
    gentity_t	*m;
    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_afist (ent, muzzle, forward, 500 );
    }
    else
    {
        m = fire_afist2 (ent, muzzle, forward );
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_afist (gentity_t *self, vec3_t start, vec3_t dir, float speed )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "afist";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 5;
    bolt->splashDamage = 05;
    bolt->splashRadius = 10;
    bolt->methodOfDeath = MOD_AFIST;
    bolt->splashMethodOfDeath = MOD_AFIST;
    bolt->s.weapon = WP_AFIST;
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
    VectorScale( dir, speed, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
gentity_t *fire_afist2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "afist2";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_AFIST2;
    bolt->splashMethodOfDeath = MOD_AFIST2;
    bolt->s.weapon = WP_AFIST2;
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

void Weapon_BCrystal_Fire( gentity_t *ent )
{
    gentity_t	*m;
    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_bcrystal (ent, muzzle, forward );
    }
    else
    {
        m = fire_bcrystal2 (ent, muzzle, forward );
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_bcrystal (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "bcrystal";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->s.eFlags = EF_BOUNCE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_BCRYSTAL;
    bolt->splashMethodOfDeath = MOD_BCRYSTAL;
    bolt->s.weapon = WP_BCRYSTAL;
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
gentity_t *fire_bcrystal2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "bcrystal2";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_BCRYSTAL2;
    bolt->splashMethodOfDeath = MOD_BCRYSTAL2;
    bolt->s.weapon = WP_BCRYSTAL2;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trDuration = 1;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

gentity_t *fire_bcrystal3 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "bcrystal3";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_BCRYSTAL2;
    bolt->splashMethodOfDeath = MOD_BCRYSTAL2;
    bolt->s.weapon = WP_BCRYSTAL3;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trDuration = 1;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

gentity_t *fire_bcrystal4 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "bcrystal4";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_BCRYSTAL2;
    bolt->splashMethodOfDeath = MOD_BCRYSTAL2;
    bolt->s.weapon = WP_BCRYSTAL4;
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

void Weapon_LTorpedo_Fire( gentity_t *ent )
{
    gentity_t	*m;
    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_ltorpedo (ent, muzzle, forward );
    }
    else
    {
        m = fire_ltorpedo2 (ent, muzzle, forward );
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_ltorpedo (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "ltorpedo";
    bolt->nextthink = level.time + 20;
    bolt->think = G_HomeMissile2;
    bolt->wait = level.time + 20000;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_LTORPEDO;
    bolt->splashMethodOfDeath = MOD_LTORPEDO;
    bolt->s.weapon = WP_LTORPEDO;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    bolt->s.pos.trDuration = 400;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 200, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
gentity_t *fire_ltorpedo2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "ltorpedo2";
    bolt->nextthink = level.time + 10;
    bolt->think = G_HomeMissile2;
    bolt->wait = level.time + 6000;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_LTORPEDO;
    bolt->splashMethodOfDeath = MOD_LTORPEDO;
    bolt->s.weapon = WP_LTORPEDO2;
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
gentity_t *fire_ltorpedo3 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "ltorpedo3";
    bolt->nextthink = level.time + 20;
    bolt->think = G_HomeMissile2;
    bolt->wait = level.time + 7000;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 10;
    bolt->splashDamage = 10;
    bolt->splashRadius = 15;
    bolt->methodOfDeath = MOD_LTORPEDO2;
    bolt->splashMethodOfDeath = MOD_LTORPEDO2;
    bolt->s.weapon = WP_LTORPEDO3;
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

void Weapon_TBurster_Fire( gentity_t *ent )
{
    gentity_t	*m;
    //14 shards
    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_tburster (ent, muzzle, forward );
    }
    else
    {
        m = fire_tburster2 (ent, muzzle, forward );
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_tburster (gentity_t *self, vec3_t start, vec3_t dir )
{
    //14 shards
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "tburster";
    bolt->think = G_SplitMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_TBURSTER;
    bolt->splashMethodOfDeath = MOD_TBURSTER;
    bolt->s.weapon = WP_TBURSTER;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_ACCEL;
    bolt->s.pos.trDuration = -300;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    bolt->nextthink = level.time + fabs(VectorLength(bolt->s.pos.trDelta)/bolt->s.pos.trDuration) + 5000;
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
gentity_t *fire_tburster2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    //14 shards
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "tburster2";
    bolt->nextthink = level.time + 3000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_TBURSTER;
    bolt->splashMethodOfDeath = MOD_TBURSTER;
    bolt->s.weapon = WP_TBURSTER2;
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
    VectorScale( dir, 300, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_GBomb_Fire( gentity_t *ent )
{
    gentity_t	*m;
    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_gbomb (ent, muzzle, forward );
    }
    else
    {
        m = fire_gbomb2 (ent, muzzle, forward );
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_gbomb (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "gbomb";
    bolt->nextthink = level.time + 500; // call G_Suck in 1 second
    bolt->think = G_Suck;
    bolt->wait = level.time + 10000; // vortex grenade lifetime
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 8;
    bolt->splashDamage = 8;
    bolt->splashRadius = 10;
    bolt->methodOfDeath = MOD_GBOMB;
    bolt->splashMethodOfDeath = MOD_GBOMB;
    bolt->s.weapon = WP_GBOMB;
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
    VectorScale( dir, 320, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
gentity_t *fire_gbomb2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "gbomb2";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_Suck;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 10;
    bolt->splashDamage = 10;
    bolt->splashRadius = 10;
    bolt->methodOfDeath = MOD_GBOMB;
    bolt->splashMethodOfDeath = MOD_GBOMB;
    bolt->s.weapon = WP_GBOMB2;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_ACCEL;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 100, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_BLaser_Fire( gentity_t *ent )
{
    //lightning
    gentity_t	*m;

    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        fire_blaser( ent );
    }
    else
    {
        m = fire_blaser2 (ent, muzzle, forward );
    }
}

void fire_blaser( gentity_t *ent )
{
    trace_t		tr;
    vec3_t		end;
    gentity_t	*traceEnt, *tent;
    int			damage, i, passent;

    damage = 8;
    passent = ent->s.number;
    for (i = 0; i < 10; i++)
    {
        VectorMA( muzzle, LIGHTNING_RANGE, forward, end );

        trap_Trace( &tr, muzzle, NULL, NULL, end, passent, MASK_SHOT );

        if ( tr.entityNum == ENTITYNUM_NONE )
        {
            return;
        }

        traceEnt = &g_entities[ tr.entityNum ];

        if ( traceEnt->takedamage)
        {
            G_Damage( traceEnt, ent, ent, forward, tr.endpos,
                      damage, 0, MOD_BLASER);
        }

        if ( traceEnt->takedamage && traceEnt->client )
        {
            tent = G_TempEntity( tr.endpos, EV_BLASER_HIT );
            tent->s.otherEntityNum = traceEnt->s.number;
            tent->s.eventParm = DirToByte( tr.plane.normal );
            tent->s.weapon = ent->s.weapon;
            if( LogAccuracyHit( traceEnt, ent ) )
            {
                ent->client->accuracy_hits++;
            }
        }
        else if ( !( tr.surfaceFlags & SURF_NOIMPACT ) )
        {
            tent = G_TempEntity( tr.endpos, EV_BLASER_MISS );
            tent->s.eventParm = DirToByte( tr.plane.normal );
        }
        break;
    }
}
gentity_t *fire_blaser2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    //12 shards
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "blaser2";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_BLASER;
    bolt->splashMethodOfDeath = MOD_BLASER;
    bolt->s.weapon = WP_BLASER2;
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

void Weapon_SFist_Fire( gentity_t *ent )
{
    gentity_t	*m;
    if( ent->client->ps.stats[STAT_CHARGE] <= 2 )
    {
        m = fire_sfist (ent, muzzle, forward );
    }
    else
    {
        m = fire_sfist2 (ent, muzzle, forward );
    }
}
gentity_t *fire_sfist (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "sfist";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 10;
    bolt->splashDamage = 15;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_SFIST;
    bolt->splashMethodOfDeath = MOD_SFIST;
    bolt->s.weapon = WP_SFIST;
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
gentity_t *fire_sfist2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "sfist2";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 20;
    bolt->splashDamage = 20;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_SFIST2;
    bolt->splashMethodOfDeath = MOD_SFIST2;
    bolt->s.weapon = WP_SFIST2;
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
