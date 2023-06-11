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

gentity_t *fire_fbuster (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_pstrike (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_pstrike2 (gentity_t *self, vec3_t start, vec3_t dir);
//gentity_t *fire_wstrike (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_tempest (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_sburst (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_asplasher (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_dmatter (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_nburst (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_blightning (gentity_t *self, vec3_t start, vec3_t dir );


//FBUSTER - only one level
void Weapon_FBuster_Fire (gentity_t *ent)
{
    const int	upgrades = ent->client->ps.persistant[PERS_UPGRADES];
    gentity_t	*m;
    float newforward[] = {0,0,0};

    if( upgrades & (1 << UG_DOUBLE))
    {
        // === Double Projectile Streams == Author: Ic3p1ck // Moded: DonX
        // 1st Pulse
        AngleVectors( ent->client->ps.viewangles, forward, right, up );
        VectorCopy( forward, newforward );
        if	( forward[0] >= 0.5 && forward[0] <= 1 )
        {
            newforward[1] += .25;
        }
        else if ( forward[0] <= -0.5 && forward[0] >= -1 )
        {
            newforward[1] += .25;
        }
        else
        {
            newforward[0] += .25;
        }
        VectorNormalize( newforward );
        VectorAdd( newforward, forward, forward );
        CalcMuzzlePoint( ent, forward, right, up, muzzle );

        m = fire_fbuster (ent, muzzle, forward);

        // 2nd Pulse
        AngleVectors( ent->client->ps.viewangles, forward, right, up );
        VectorCopy( forward, newforward );
        if	( forward[0] >= 0.5 && forward[0] <= 1 )
        {
            newforward[1] -= .25;
        }
        else if ( forward[0] <= -0.5 && forward[0] >= -1 )
        {
            newforward[1] -= .25;
        }
        else
        {
            newforward[0] -= .25;
        }
        VectorNormalize( newforward );
        VectorAdd( newforward, forward, forward );
        CalcMuzzlePoint( ent, forward, right, up, muzzle );

        m = fire_fbuster (ent, muzzle, forward);
    }
    else
    {

        m = fire_fbuster (ent, muzzle, forward );
    }
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_fbuster
=================
*/
gentity_t *fire_fbuster (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "fbuster";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 05;
    bolt->splashDamage = 05;
    bolt->splashRadius = 05;
    bolt->methodOfDeath = MOD_FBUSTER;
    bolt->splashMethodOfDeath = MOD_FBUSTER;
    bolt->s.weapon = WP_FBUSTER;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, -8, -8, -8);
    VectorSet(bolt->r.maxs, 8, 8, 8);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 800, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);


    return bolt;
}

void Weapon_PStrike_Fire (gentity_t *ent)
{
    gentity_t	*m;

    m = fire_pstrike (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
/*
=================
fire_pstrike
=================
*/
gentity_t *fire_pstrike (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "pstrike";
    bolt->nextthink = level.time + 15000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->s.weapon = WP_PSTRIKE;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 15;
    bolt->splashDamage = 15;
    bolt->splashRadius = 30;
    bolt->methodOfDeath = MOD_PSTRIKE;
    bolt->splashMethodOfDeath = MOD_PSTRIKE;
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
    VectorScale( dir, 900, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
/*
=================
fire_pstrike2
=================
*/
gentity_t *fire_pstrike2 (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "pstrike";
    bolt->nextthink = level.time + 5000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 10;
    bolt->splashDamage = 10;
    bolt->splashRadius = 10;
    bolt->methodOfDeath = MOD_PSTRIKE;
    bolt->splashMethodOfDeath = MOD_PSTRIKE;
    bolt->s.weapon = WP_PSTRIKE;
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
    VectorScale( dir, 400, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

#define	MAX_RAIL_HITS	4
void Weapon_WStrike_Fire (gentity_t *ent)
{
    vec3_t		end;
    trace_t		trace;
    gentity_t	*tent;
    gentity_t	*traceEnt;
    int			damage, i, hits, unlinked, passent;
    gentity_t	*unlinkedEntities[MAX_RAIL_HITS];

    damage = 20;
    VectorMA (muzzle, 8192, forward, end);
    // trace only against the solids, so the railgun will go through people
    unlinked = 0;
    hits = 0;
    passent = ent->s.number;
    do
    {
        trap_Trace (&trace, muzzle, NULL, NULL, end, passent, MASK_SHOT );
        if ( trace.entityNum >= ENTITYNUM_MAX_NORMAL )
        {
            break;
        }
        traceEnt = &g_entities[ trace.entityNum ];
        if ( traceEnt->takedamage )
        {
            if( LogAccuracyHit( traceEnt, ent ) )
            {
                hits++;
            }
            G_Damage (traceEnt, ent, ent, forward, trace.endpos, damage, 0, MOD_WSTRIKE);
        }
        if ( trace.contents & CONTENTS_SOLID )
        {
            break;		// we hit something solid enough to stop the beam
        }
        // unlink this entity, so the next trace will go past it
        trap_UnlinkEntity( traceEnt );
        unlinkedEntities[unlinked] = traceEnt;
        unlinked++;
    }
    while ( unlinked < MAX_RAIL_HITS );

    // link back in any entities we unlinked
    for ( i = 0 ; i < unlinked ; i++ )
    {
        trap_LinkEntity( unlinkedEntities[i] );
    }
    // the final trace endpos will be the terminal point of the rail trail
    // snap the endpos to integers to save net bandwidth, but nudged towards the line
    SnapVectorTowards( trace.endpos, muzzle );
    // send railgun beam effect
    tent = G_TempEntity( trace.endpos, EV_RAILTRAIL );
    // set player number for custom colors on the railtrail
    tent->s.clientNum = ent->s.clientNum;
    VectorCopy( muzzle, tent->s.origin2 );
    // move origin a bit to come closer to the drawn gun muzzle
    VectorMA( tent->s.origin2, 4, right, tent->s.origin2 );
    VectorMA( tent->s.origin2, -1, up, tent->s.origin2 );
    // no explosion at end if SURF_NOIMPACT, but still make the trail
    if ( trace.surfaceFlags & SURF_NOIMPACT )
    {
        tent->s.eventParm = 255;	// don't make the explosion at the end
    }
    else
    {
        tent->s.eventParm = DirToByte( trace.plane.normal );
    }
    tent->s.clientNum = ent->s.clientNum;
}

void Weapon_Tempest_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_tempest (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_tempest (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "tempest";
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
    //bolt->s.eFlags |= EF_BOUNCE_HALF;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 15;
    bolt->splashDamage = 15;
    bolt->splashRadius = 100;
    bolt->methodOfDeath = MOD_TEMPEST;
    bolt->splashMethodOfDeath = MOD_TEMPEST;
    bolt->s.weapon = WP_TEMPEST;
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
    VectorScale( dir, 500, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_SBurst_Fire( gentity_t *ent )
{
    int k = 0;
    float spread = .2f;
    gentity_t	*m[9];
    vec3_t dir[9]; //forward, right, left, up, down, ur, dr, ul, dl
    //9 shards
    VectorCopy( forward, dir[0]);
    VectorNormalize( dir[0] );
    CrossProduct( dir[0], up, dir[1] );
    CrossProduct( dir[0], dir[1], dir[3] );
    VectorNormalize( dir[1] );
    VectorScale( dir[1], crandom()*spread, dir[1] );
    VectorScale( dir[1], -1.0f*crandom(), dir[2] );
    VectorNormalize( dir[3] );
    VectorScale( dir[3], crandom()*spread, dir[3] );
    VectorScale( dir[3], -1.0f*crandom(), dir[4] );
    VectorAdd( dir[1], dir[3], dir[6] );
    VectorNormalize( dir[6] );
    VectorScale( dir[6], crandom()*spread, dir[6] );
    VectorScale( dir[6], -1.0f*crandom(), dir[7] );
    VectorAdd( dir[2], dir[3], dir[5] );
    VectorNormalize( dir[5] );
    VectorScale( dir[5], crandom()*spread, dir[5] );
    VectorScale( dir[5], -1.0f*crandom(), dir[8] );
    for( k = 0; k < 9; k++ )
    {
        if( k != 0 )
        {
            VectorAdd( dir[k], dir[0], dir[k] );
        }
        VectorNormalize( dir[k] );
        m[k] = fire_sburst( ent, muzzle, dir[k] );
    }
    //	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_sburst (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "sburst";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 8;
    bolt->splashDamage = 8;
    bolt->splashRadius = 10;
    bolt->methodOfDeath = MOD_SBURST;
    bolt->splashMethodOfDeath = MOD_SBURST;
    bolt->s.weapon = WP_SBURST;
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
    VectorScale( dir, 1400, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_DShot_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_dshot (ent, muzzle, forward, 300 );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_dshot (gentity_t *self, vec3_t start, vec3_t dir, float speed )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "dshot";
    bolt->nextthink = level.time + 10000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 10;
    bolt->splashDamage = 10;
    bolt->splashRadius = 10;
    bolt->methodOfDeath = MOD_DSHOT;
    bolt->splashMethodOfDeath = MOD_DSHOT;
    bolt->s.weapon = WP_DSHOT;
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

void Weapon_ASplasher_Fire( gentity_t *ent )
{
    gentity_t	*m;
    m = fire_asplasher (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_asplasher (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "asplasher";
    bolt->nextthink = level.time + 3000;
    bolt->think = G_SplitMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->s.eFlags |= EF_BOUNCE;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 12;
    bolt->splashDamage = 12;
    bolt->splashRadius = 20;
    bolt->methodOfDeath = MOD_ASPLASHER;
    bolt->splashMethodOfDeath = MOD_ASPLASHER;
    bolt->s.weapon = WP_ASPLASHER;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    /*VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);*/

    bolt->s.pos.trType = TR_GRAVITY;
    bolt->s.pos.trDuration = 1;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
gentity_t *fire_asplasher2 (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "asplasher";
    bolt->nextthink = level.time + 5000;
    bolt->think = G_ExplodeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 10;
    bolt->splashDamage = 7;
    bolt->splashRadius = 10;
    bolt->methodOfDeath = MOD_ASPLASHER;
    bolt->splashMethodOfDeath = MOD_ASPLASHER;
    bolt->s.weapon = WP_ASPLASHER;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    /*VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);*/

    bolt->s.pos.trType = TR_GRAVITY;
    bolt->s.pos.trTime = level.time;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_DMatter_Fire( gentity_t *ent )
{
    gentity_t	*m;
    vec3_t		dir;
    dir[0] = crandom()*.02;
    dir[1] = crandom()*.02;
    dir[2] = crandom()*.02;
    VectorAdd( dir, forward, dir );
    m = fire_dmatter ( ent, muzzle, dir );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_dmatter (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "dmatter";
    bolt->nextthink = level.time + 20;
    bolt->wait = level.time + 10000;
    bolt->think = G_HomeMissile;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 8;
    bolt->splashDamage = 12;
    bolt->splashRadius = 5;
    bolt->methodOfDeath = MOD_DMATTER;
    bolt->splashMethodOfDeath = MOD_DMATTER;
    bolt->s.weapon = WP_DMATTER;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    bolt->s.pos.trDuration = 550;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 1500, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_NBurst_Fire( gentity_t *ent )
{
    gentity_t	*m;
    //12 shards
    m = fire_nburst (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_nburst (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "nburst";
    bolt->nextthink = level.time + 500;
    bolt->wait = level.time + 10000;
    bolt->think = G_Strike;
    bolt->s.eType = ET_MISSILE;
    bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    bolt->r.ownerNum = self->s.number;
    bolt->parent = self;
    bolt->damage = 30;
    bolt->splashDamage = 10;
    bolt->splashRadius = 100;
    bolt->methodOfDeath = MOD_NBURST;
    bolt->splashMethodOfDeath = MOD_NBURST;
    bolt->s.weapon = WP_NBURST;
    bolt->clipmask = MASK_SHOT;
    bolt->target_ent = NULL;
    //bolt->r.contents = CONTENTS_BODY;
    VectorSet(bolt->r.mins, 0, 0, 0);
    VectorSet(bolt->r.maxs, 20, 20, 20);
    VectorCopy(bolt->r.mins, bolt->r.absmin);
    VectorCopy(bolt->r.maxs, bolt->r.absmax);

    bolt->s.pos.trType = TR_LINEAR;
    bolt->s.pos.trTime = level.time;
    //bolt->s.pos.trDuration = 500;
    VectorCopy( start, bolt->s.pos.trBase );
    VectorScale( dir, 700, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}

void Weapon_BLightning_Fire( gentity_t *ent )
{
    gentity_t	*m;
    //bounces
    m = fire_blightning (ent, muzzle, forward );
//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}
gentity_t *fire_blightning (gentity_t *self, vec3_t start, vec3_t dir )
{
    gentity_t	*bolt;

    VectorNormalize (dir);

    bolt = G_Spawn();
    bolt->classname = "blightning";
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
    bolt->methodOfDeath = MOD_BLIGHTNING;
    bolt->splashMethodOfDeath = MOD_BLIGHTNING;
    bolt->s.weapon = WP_BLIGHTNING;
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
    VectorScale( dir, 400, bolt->s.pos.trDelta );
    SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, bolt->r.currentOrigin);

    return bolt;
}
