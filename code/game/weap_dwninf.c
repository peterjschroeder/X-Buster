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

gentity_t *fire_zbuster (gentity_t *self, vec3_t start, vec3_t dir);

//ZBUSTER - levels 0-2
void Weapon_ZBuster_Fire (gentity_t *ent)
{
    gentity_t	*m;
    m = fire_zbuster (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_zbuster
=================
*/
gentity_t *fire_zbuster (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "zbuster";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->methodOfDeath = MOD_ZBUSTER;
    bolt->splashMethodOfDeath = MOD_ZBUSTER;
    bolt->s.pos.trType = TR_LINEAR;
    switch( self->client->ps.stats[STAT_CHARGE] )
    {
    case 0:
        bolt->damage = 10;
        bolt->splashDamage = 10;
        bolt->splashRadius = 10;
        bolt->s.weapon = WP_ZBUSTER;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        VectorSet(bolt->r.mins, 0, 0, 0);
        VectorSet(bolt->r.maxs, 8, 8, 8);
        break;
    case 1:
        bolt->damage = 15;
        bolt->splashDamage = 8;
        bolt->splashRadius = 20;
        bolt->s.weapon = WP_ZBUSTER1;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        break;
    case 2:
        bolt->damage = 25;
        bolt->splashDamage = 15;
        bolt->splashRadius = 30;
        bolt->s.weapon = WP_ZBUSTER2;
        VectorScale( dir, 1000, bolt->s.pos.trDelta );
        VectorSet(bolt->r.mins, -15, -40, -25);
        VectorSet(bolt->r.maxs, 15, 0, 25);
        break;
    default:
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

void Tech_Uppercut_Fire( gentity_t *ent )
{
    trace_t		tr;
    vec3_t		end;
    gentity_t	*tent;
    gentity_t	*traceEnt;
    int			damage;

    VectorMA (muzzle, 64, forward, end);
    VectorMA ( end, 8, up, end );

    trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);
    if ( tr.surfaceFlags & SURF_NOIMPACT )
    {
        return;
    }

    traceEnt = &g_entities[ tr.entityNum ];

    // send blood impact
    if ( traceEnt->takedamage && traceEnt->client )
    {
        tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
        tent->s.otherEntityNum = traceEnt->s.number;
        tent->s.eventParm = DirToByte( tr.plane.normal );
        tent->s.weapon = ent->s.weapon;
    }

    if ( !traceEnt->takedamage)
    {
        return;
    }

    damage = 50;

    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
              damage, 0, MOD_GAUNTLET );
}

void Tech_Spike_Fire( gentity_t *ent )
{
    trace_t		tr;
    vec3_t		end;
    gentity_t	*tent;
    gentity_t	*traceEnt;
    int			damage;

    //G_Printf( "trying to do a spike at time %i\n", level.time );
    VectorNegate( up, end );	//we want to point down
    VectorMA (muzzle, 64, end, end);
    VectorMA (end, 16, forward, end);

    trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);
    if ( tr.surfaceFlags & SURF_NOIMPACT )
    {
        return;
    }

    traceEnt = &g_entities[ tr.entityNum ];

    // send blood impact
    if ( traceEnt->takedamage && traceEnt->client )
    {
        tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
        tent->s.otherEntityNum = traceEnt->s.number;
        tent->s.eventParm = DirToByte( tr.plane.normal );
        tent->s.weapon = ent->s.weapon;
    }

    if ( !traceEnt->takedamage)
    {
        return;
    }

    damage = 50;

    G_Damage( traceEnt, ent, ent, forward, tr.endpos,
              damage, 0, MOD_GAUNTLET );
}

void Tech_Spin_Fire( gentity_t *ent )
{
}
