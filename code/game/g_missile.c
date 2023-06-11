// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"


/*
================
G_BounceMissile

================
*/
void G_BounceMissile( gentity_t *ent, trace_t *trace )
{
    vec3_t	velocity;
    float	dot;
    int		hitTime;

    // reflect the velocity on the trace plane
    hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
    BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
    dot = DotProduct( velocity, trace->plane.normal );
    VectorMA( velocity, -2*dot, trace->plane.normal, ent->s.pos.trDelta );

    if ( ent->s.eFlags & EF_BOUNCE_HALF )
    {
        VectorScale( ent->s.pos.trDelta, 0.65, ent->s.pos.trDelta );
        // check for stop
        if ( trace->plane.normal[2] > 0.2 && VectorLength( ent->s.pos.trDelta ) < 40 )
        {
            G_SetOrigin( ent, trace->endpos );
            return;
        }
    }

    VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
    VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
    ent->s.pos.trTime = level.time;
}
/*
====================
G_GetCardinalVectors - based on 2 vectors
====================
*/
void G_GetCardinalVectors( vec3_t cross1, vec3_t cross2, vec3_t dir[9] )
{
    VectorCopy( cross1, dir[0] );			//forward (up)
    VectorNormalize( dir[0] );
    CrossProduct( dir[0], cross2, dir[1] );	//up (forward)
    CrossProduct( dir[0], dir[1], dir[3] );	//right
    VectorNormalize( dir[1] );
    VectorNegate( dir[1], dir[2] );			//down (back)
    VectorNormalize( dir[3] );
    VectorNegate( dir[3], dir[4] );			//left
    VectorAdd( dir[1], dir[3], dir[6] );	//up-right (forward-right)
    VectorNormalize( dir[6] );
    VectorNegate( dir[6], dir[7] );			//down-left (back-left)
    VectorAdd( dir[2], dir[3], dir[5] );	//down-right (back-right)
    VectorNormalize( dir[5] );
    VectorNegate( dir[5], dir[8] );			//up-left (forward-left)
}

/*
====================
G_GetCardinalVectors2 - based on 2 vectors
====================
*/
void G_GetCardinalVectors2( vec3_t cross1, vec3_t cross2, vec3_t dir[14] )
{
    VectorCopy( cross1, dir[0] );			//forward (up)
    VectorNormalize( dir[0] );
    VectorNegate( dir[0], dir[9] );			//back (down)
    CrossProduct( dir[0], cross2, dir[1] );	//up (forward)
    CrossProduct( dir[0], dir[1], dir[3] );	//right
    VectorNormalize( dir[1] );
    VectorNegate( dir[1], dir[2] );			//down (back)
    VectorNormalize( dir[3] );
    VectorNegate( dir[3], dir[4] );			//left
    VectorAdd( dir[1], dir[3], dir[6] );	//up-right (forward-right)
    VectorAdd( dir[6], dir[0], dir[10] );	//forward-up-right
    VectorAdd( dir[6], dir[9], dir[6] );	//forward-down-right
    VectorNormalize( dir[6] );
    VectorNormalize( dir[10] );
    VectorNegate( dir[10], dir[11] );		//down-back-left
    VectorNegate( dir[6], dir[7] );			//back-up-left
    VectorAdd( dir[2], dir[3], dir[5] );	//down-right (back-right)
    VectorAdd( dir[9], dir[1], dir[12] );
    VectorAdd( dir[12], dir[4], dir[12] );	//back-down-right
    VectorAdd( dir[5], dir[9], dir[5] );	//back-up-right
    VectorNormalize( dir[5] );
    VectorNegate( dir[5], dir[8] );			//forward-down-left
    VectorNormalize( dir[12] );
    VectorNegate( dir[12], dir[13] );		//back-up-left
    //for( k = 0; k < 14; k++ ) {
    //	G_Printf( "dir[%i] has components <%f, %f, %f>\n", k, dir[k][0],dir[k][1], dir[k][2] );
    //}
}

/*
================
G_ExplodeMissile

Explode a missile without an impact
================
*/
void G_ExplodeMissile( gentity_t *ent )
{
    vec3_t		dir;
    vec3_t		origin;

    BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
    SnapVector( origin );
    G_SetOrigin( ent, origin );

    // we don't have a valid direction, so just point straight up
    dir[0] = dir[1] = 0;
    dir[2] = 1;

    ent->s.eType = ET_GENERAL;
    G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( dir ) );

    ent->freeAfterEvent = qtrue;

    // splash damage
    if ( ent->splashDamage )
    {
        if( G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, ent
                            , ent->splashMethodOfDeath ) )
        {
            g_entities[ent->r.ownerNum].client->accuracy_hits++;
        }
    }

    trap_LinkEntity( ent );
}




/*
================
G_MissileImpact
================
*/
void G_MissileImpact( gentity_t *ent, trace_t *trace )
{
    gentity_t		*other, *m[9];
    qboolean		hitClient = qfalse;
    vec3_t			relativeX, forward, neworigin;

    other = &g_entities[trace->entityNum];

    // check for bounce
    if ( !other->takedamage &&
            ( ent->s.eFlags & ( EF_BOUNCE | EF_BOUNCE_HALF ) ) )
    {
        G_BounceMissile( ent, trace );
        G_AddEvent( ent, EV_GRENADE_BOUNCE, 0 );
        return;
    }
    // rolling weapons
    switch ( ent->s.weapon )
    {
    case WP_SICE3:
    case WP_AWAVE:
    case WP_RSTONE:
        ent->s.pos.trTime = level.time;
        CrossProduct( ent->s.pos.trDelta, trace->plane.normal, relativeX );
        CrossProduct( trace->plane.normal, relativeX, forward );
        VectorNormalize( forward );
        VectorScale( forward, 400, ent->s.pos.trDelta );
        VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
        ent->s.pos.trType = TR_LINEAR;
        return;
    case WP_FBURST:
        if( other->client )
        {
            other->client->freeze_time = level.time + 5000;
            SET_BIT( other->client->ps.eFlags, EF_FROZEN );
        }
        break;
    case WP_BSHIELD:
        if( other->client )
        {
            other->client->shock_time = level.time + 5000;
            SET_BIT( other->client->ps.eFlags, EF_SHOCKED );
        }
    case WP_TRTHUNDER:
        if( other->client )
        {
            other->client->shock_time = level.time + 5000;
            SET_BIT( other->client->ps.eFlags, EF_SHOCKED );
        }
        break;
    default:
        break;
    }
    // check if the weapon spawns more weapons upon impact
    if( ent->s.pos.trDuration )
    {
        vec3_t dir[9]; //up, left, right, forward, back, fr, fl, br, bl
        int k = 0;
        switch( ent->s.weapon )
        {
        case WP_PSTRIKE:
            G_GetCardinalVectors( trace->plane.normal, ent->s.pos.trDelta, dir );
            for( ; k < 9; k++ )
            {
                if( k != 0 )
                    VectorAdd( dir[k], dir[0], dir[k] );//so that they all move away from the plane
                VectorNormalize( dir[k] );
                m[k] = fire_pstrike2( ent->parent, ent->r.currentOrigin, dir[k] );
            }
            break;
        case WP_HWAVE:
            VectorCopy( trace->plane.normal, dir[0]);
            VectorNormalize( dir[0] );
            for( ; k < 4; k++ )
            {
                m[k] = fire_hwave2( ent->parent, ent->r.currentOrigin, dir[0], k+1 );
            }
            break;
        case WP_SICE:
            G_GetCardinalVectors( trace->plane.normal, ent->s.pos.trDelta, dir );
            for( ; k < 9; k++ )
            {
                if( k != 0 )
                    VectorAdd( dir[k], dir[0], dir[k] );//so that they all move away from the plane
                VectorNormalize( dir[k] );
                m[k] = fire_sice2( ent->parent, ent->r.currentOrigin, dir[k] );
            }
            break;
        case WP_ASPLASHER:
            G_GetCardinalVectors( trace->plane.normal, ent->s.pos.trDelta, dir );
            for( ; k < 5; k++ )
            {
                if( k != 0 )
                    VectorAdd( dir[k], dir[0], dir[k] );//so that they all move away from the plane
                VectorNormalize( dir[k] );
                m[k] = fire_asplasher2( ent->parent, ent->r.currentOrigin, dir[k] );
            }
            break;
        case WP_BCRYSTAL2:
            G_GetCardinalVectors( trace->plane.normal, ent->s.pos.trDelta, dir );
            for( ; k < 9; k++ )
            {
                if( k != 0 )
                    VectorAdd( dir[k], dir[0], dir[k] );//so that they all move away from the plane
                VectorNormalize( dir[k] );
                m[k] = fire_bcrystal3( ent->parent, ent->r.currentOrigin, dir[k] );
            }
            break;
        case WP_BCRYSTAL3:
            G_GetCardinalVectors( trace->plane.normal, ent->s.pos.trDelta, dir );
            for( ; k < 5; k++ )
            {
                if( k != 0 )
                    VectorAdd( dir[k], dir[0], dir[k] );//so that they all move away from the plane
                VectorNormalize( dir[k] );
                if( k != 0 )
                    m[k] = fire_bcrystal4( ent->parent, ent->r.currentOrigin, dir[k] );
            }
            break;
        default:
            break;
        }
    }
    // impact damage
    if (other->takedamage)
    {
        // FIXME: wrong damage direction?
        if ( ent->damage )
        {
            vec3_t	velocity;

            if( LogAccuracyHit( other, &g_entities[ent->r.ownerNum] ) )
            {
                g_entities[ent->r.ownerNum].client->accuracy_hits++;
                hitClient = qtrue;
            }
            BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, velocity );
            if ( VectorLength( velocity ) == 0 )
            {
                velocity[2] = 1;	// stepped on a grenade
            }
            G_Damage (other, ent, &g_entities[ent->r.ownerNum], velocity,
                      ent->s.origin, ent->damage,
                      0, ent->methodOfDeath);
        }
    }

    if (!strcmp(ent->classname, "hook"))
    {
        gentity_t *nent;
        vec3_t v;

        nent = G_Spawn();
        if ( other->takedamage && other->client )
        {

            G_AddEvent( nent, EV_MISSILE_HIT, DirToByte( trace->plane.normal ) );
            nent->s.otherEntityNum = other->s.number;

            ent->enemy = other;

            v[0] = other->r.currentOrigin[0] + (other->r.mins[0] + other->r.maxs[0]) * 0.5;
            v[1] = other->r.currentOrigin[1] + (other->r.mins[1] + other->r.maxs[1]) * 0.5;
            v[2] = other->r.currentOrigin[2] + (other->r.mins[2] + other->r.maxs[2]) * 0.5;

            SnapVectorTowards( v, ent->s.pos.trBase );	// save net bandwidth
        }
        else
        {
            VectorCopy(trace->endpos, v);
            G_AddEvent( nent, EV_MISSILE_MISS, DirToByte( trace->plane.normal ) );
            ent->enemy = NULL;
        }

        SnapVectorTowards( v, ent->s.pos.trBase );	// save net bandwidth

        nent->freeAfterEvent = qtrue;
        // change over to a normal entity right at the point of impact
        nent->s.eType = ET_GENERAL;
        ent->s.eType = ET_GRAPPLE;

        G_SetOrigin( ent, v );
        G_SetOrigin( nent, v );

        ent->think = Weapon_HookThink;
        ent->nextthink = level.time + FRAMETIME;

        ent->parent->client->ps.pm_flags |= PMF_GRAPPLE_PULL;
        VectorCopy( ent->r.currentOrigin, ent->parent->client->ps.grapplePoint);

        trap_LinkEntity( ent );
        trap_LinkEntity( nent );

        return;
    }

    // is it cheaper in bandwidth to just remove this ent and create a new
    // one, rather than changing the missile into the explosion?

    if ( other->takedamage && other->client )
    {
        G_AddEvent( ent, EV_MISSILE_HIT, DirToByte( trace->plane.normal ) );
        ent->s.otherEntityNum = other->s.number;
        // some weapons "ignore" players
        switch ( ent->s.weapon )
        {
        case WP_DSHOT:
            VectorCopy( ent->s.pos.trDelta, forward );
            VectorNormalize( forward );
            VectorAdd( forward, trace->endpos, neworigin );
            fire_dshot( ent->parent, neworigin,  forward, 100 );
            break;
        case WP_AFIST:
            VectorCopy( ent->s.pos.trDelta, forward );
            VectorNormalize( forward );
            VectorAdd( forward, trace->endpos, neworigin );
            fire_afist( ent->parent, neworigin,  forward, 100 );
            break;
        default:
            break;
        }

    }
    else if( trace->surfaceFlags & SURF_METALSTEPS )
    {
        G_AddEvent( ent, EV_MISSILE_MISS_METAL, DirToByte( trace->plane.normal ) );
    }
    else
    {
        G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( trace->plane.normal ) );
    }

    ent->freeAfterEvent = qtrue;

    // change over to a normal entity right at the point of impact
    ent->s.eType = ET_GENERAL;

    SnapVectorTowards( trace->endpos, ent->s.pos.trBase );	// save net bandwidth

    G_SetOrigin( ent, trace->endpos );

    // splash damage (doesn't apply to person directly hit)
    if ( ent->splashDamage )
    {
        if( G_RadiusDamage( trace->endpos, ent->parent, ent->splashDamage, ent->splashRadius,
                            other, ent->splashMethodOfDeath ) )
        {
            if( !hitClient )
            {
                g_entities[ent->r.ownerNum].client->accuracy_hits++;
            }
        }
    }

    trap_LinkEntity( ent );
}
/*
==================
G_ClipVelocity

Slide off of the impacting surface
==================
*/
void G_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce )
{
    float	backoff;
    float	change;
    int		i;

    backoff = DotProduct (in, normal);

    if ( backoff < 0 )
    {
        backoff *= overbounce;
    }
    else
    {
        backoff /= overbounce;
    }

    for ( i=0 ; i<3 ; i++ )
    {
        change = normal[i]*backoff;
        out[i] = in[i] - change;
    }
}
/*
================
G_RollMissile
roll something along the ground
================
*/
void G_RollMissile( gentity_t *ent, trace_t *trace )
{
    vec3_t	end;
    trace_t	down;

    VectorCopy( ent->r.currentOrigin, end );
    end[2] -= 25;
    trap_Trace( &down, ent->r.currentOrigin, NULL, NULL, end, ent->r.ownerNum, ent->clipmask );
    //G_Printf( "down.fraction is %f\n", down.fraction );
    //G_Printf( "currentorigin is <%f, %f, %f>\n", ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] );
    //G_Printf( "current velocity is <%f, %f, %f>\n", ent->s.pos.trDelta[0], ent->s.pos.trDelta[1], ent->s.pos.trDelta[2] );
    if( down.fraction < 1.0f )
    {
    }
}
//Credit goes to Spk
void G_GuideMissile (gentity_t *missile)
{
    vec3_t forward, right, up;
    vec3_t muzzle;
    float dist;
    gentity_t *player = missile->parent;

    missile->nextthink = level.time + 20;
    if (level.time > missile->wait)
    {
        G_ExplodeMissile( missile );
        return;
    }
    // If our owner can't be found, just return
    if (!player)
    {
        //G_Printf( "G_GuideMissile : missile has no owner!\n");
        missile->nextthink = level.time + 1000;
        missile->think = G_ExplodeMissile;
        return;
    }
    if ( player->client->ps.stats[STAT_HEALTH] <= 0 )
    {
        //G_Printf( "G_GuideMissile : owner has died!\n");
        missile->nextthink = level.time + 1000;
        missile->think = G_ExplodeMissile;
        return;
    }
    // Get our forward, right, up vector from the view angle of the player
    AngleVectors (player->client->ps.viewangles, forward, right, up);
    // Calculate the player's eyes origin, and store this origin in muzzle
    CalcMuzzlePoint ( player, forward, right, up, muzzle );
    // Tells the engine that our movement starts at the current missile's origin
    VectorCopy (missile->r.currentOrigin, missile->s.pos.trBase );
    // Trajectory type setup (linear move - fly)
    missile->s.pos.trType = TR_LINEAR;
    missile->s.pos.trTime = level.time - 50;
    // Get the dir vector between the player's point of view and the rocket
    // and store it into muzzle again
    VectorSubtract (muzzle, missile->r.currentOrigin, muzzle);
    // Add some range to our "line" so we can go behind blocks
    // We could have used a trace function here, but the rocket would
    // have come back if player was aiming on a block while the rocket is behind it
    // as the trace stops on solid blocks
    dist = VectorLength (muzzle) + 400;	 //give the range of our muzzle vector + 400 units
    VectorScale (forward, dist, forward);
    // line straight forward
    VectorAdd (forward, muzzle, muzzle);
    // Normalize the vector so it's 1 unit long, but keep its direction
    VectorNormalize (muzzle);
    // Slow the rocket down a bit, so we can handle it
    VectorScale (muzzle, VectorLength(missile->s.pos.trDelta), forward);
    // Set the rockets's velocity so it'll move toward our new direction
    VectorCopy (forward, missile->s.pos.trDelta);
    // Change the rocket's angle so it'll point toward the new direction
    vectoangles (muzzle, missile->s.angles);
    // This should "save net bandwidth" =D
    SnapVector( missile->s.pos.trDelta );
    // Call this function in 0,1 s
    missile->nextthink = level.time + FRAMETIME;
}

/*
================
G_SplitMissile

Splits a missile into parts upon dying
================
*/
void G_SplitMissile( gentity_t *ent )
{
    vec3_t		dir[5]/*forward, up, left, right, down, ur, ul, dr, dl*/;
    gentity_t		*m[5]/*, *m6, *m7, *m8, *m9*/;
    int k;

    VectorCopy( ent->s.pos.trDelta, dir[0] );
    VectorNormalize( dir[0] );
    VectorSet( dir[1], 0, 0, 1 );
    VectorNegate(dir[1], dir[4]);
    if( (dir[0] == dir[1]) || (dir[0] == dir[4]) )
    {
        VectorSet( dir[1], 0, 1, 0 );
    }
    CrossProduct( dir[1], dir[0], dir[2] );
    VectorNormalize( dir[2] );
    VectorScale( dir[2], -.25f, dir[3] );
    VectorNegate(dir[3], dir[2]);
    VectorAdd( dir[0], dir[2], dir[2] );
    VectorAdd( dir[0], dir[3], dir[3] );
    VectorNormalize( dir[2] );
    VectorNormalize( dir[3] );
    CrossProduct( dir[0], dir[2], dir[1] );
    VectorNormalize( dir[1] );
    VectorScale( dir[1], -.25f, dir[4] );
    VectorNegate(dir[4], dir[1]);
    VectorAdd( dir[0], dir[1], dir[1] );
    VectorAdd( dir[0], dir[4], dir[4] );
    VectorNormalize( dir[1] );
    VectorNormalize( dir[4] );

    //G_Printf( "splitting should occur!\n");
    if( ent->s.weapon == WP_LTORPEDO2 )
    {
        for( k = 0; k < 5; k++ )
        {
            m[k] = fire_ltorpedo3( ent->parent, ent->r.currentOrigin, dir[k] );
        }
    }
    else if( ent->s.weapon == WP_ASPLASHER )
    {
        for( k = 0; k < 5; k++ )
        {
            m[k] = fire_asplasher2( ent->parent, ent->r.currentOrigin, dir[k] );
        }
    }
    else if( ent->s.weapon == WP_TBURSTER )
    {
        gentity_t *ents[14];
        vec3_t	dir2[14], up = {0,0,1}, right = {0,1,0};
        G_GetCardinalVectors2( up, right, dir2 );
        for( k = 0; k < 14; k++ )
        {
            ents[k] = fire_tburster2( ent->parent, ent->r.currentOrigin, dir2[k] );
        }
    }

    ent->s.eType = ET_GENERAL;
    G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( ent->s.pos.trDelta ) );
    ent->freeAfterEvent = qtrue;

    // splash damage
    if ( ent->splashDamage )
    {
        if( G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, ent, ent->splashMethodOfDeath ) )
        {
            g_entities[ent->r.ownerNum].client->accuracy_hits++;
        }
    }

    trap_LinkEntity( ent );
}


#define GSUCK_TIMING	50			// the think time interval of G_Suck
#define GSUCK_VELOCITY	2000			// the amount of kick each second gets
#define GSUCK_RADIUS	500
#define GSUCK_TRIGGER	32
/*
=================
G_Suck //tute11.3
=================
*/
void G_Suck( gentity_t *self )
{
    gentity_t *target;
    vec3_t start,dir,end,kvel,mins,maxs;
    int targNum[MAX_GENTITIES],num;


    target = NULL;
    //check if there are any entity's within a radius of 500 units.
    mins[0] = -GSUCK_RADIUS * 1.42;
    mins[1] = -GSUCK_RADIUS * 1.42;
    mins[2] = -GSUCK_RADIUS * 1.42;
    maxs[0] = GSUCK_RADIUS * 1.42;
    maxs[1] = GSUCK_RADIUS * 1.42;
    maxs[2] = GSUCK_RADIUS * 1.42;

    VectorAdd( self->r.currentOrigin, mins, mins );
    VectorAdd( self->r.currentOrigin, maxs, maxs );

    num = trap_EntitiesInBox(mins,maxs,targNum,MAX_GENTITIES);
    for(num--; num > 0; num--)      // count from num-1 down to 0
    {
        target = &g_entities[targNum[num]];


        // target must not be vortex grenade
        if (target == self)
            continue;
        // target must be a client
        if (!target->client)
            continue;
        // target must not be the player who fired the vortex grenade
        if (target == self->parent)
            continue;
        // target must be able to take damage
        if (!target->takedamage)
            continue;
        // target must actually be in GSUCK_RADIUS
        if ( Distance(self->r.currentOrigin,target->r.currentOrigin) > GSUCK_RADIUS )
            continue;

        // put target position in start
        VectorCopy(target->r.currentOrigin, start);
        // put grenade position in end
        VectorCopy(self->r.currentOrigin, end);
        // subtract start from end to get directional vector
        VectorSubtract(end, start, dir);
        VectorNormalize(dir);
        // scale directional vector by the kick factor and add to the targets velocity
        VectorScale(dir,GSUCK_VELOCITY / GSUCK_TIMING, kvel);
        // add the kick velocity to the player's velocity
        VectorAdd (target->client->ps.velocity,kvel, target->client->ps.velocity);
        // set the timer so that the other client can't cancel out the movement immediately
        if ( !target->client->ps.pm_time )
        {
            target->client->ps.pm_time = GSUCK_TIMING - 1;
            //the next G_Suck that works here will probably be the one that worked before
            target->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
        }
        // make targets move direction = to directional vector.
        VectorCopy(dir, target->movedir);
    }
    self->nextthink = level.time + 50;
    // check if vortext grenade is older than 20 seconds.
    if (level.time > self->wait)
        G_ExplodeMissile( self);
    mins[0] = -GSUCK_TRIGGER * 1.42f;
    mins[1] = -GSUCK_TRIGGER * 1.42f;
    mins[2] = -GSUCK_TRIGGER * 1.42f;
    maxs[0] = GSUCK_TRIGGER * 1.42f;
    maxs[1] = GSUCK_TRIGGER * 1.42f;
    maxs[2] = GSUCK_TRIGGER * 1.42f;

    VectorAdd( self->r.currentOrigin, mins, mins );
    VectorAdd( self->r.currentOrigin, maxs, maxs );

    num = trap_EntitiesInBox(mins,maxs,targNum,MAX_GENTITIES);
    for(num--; num > 0; num--)      // count from num-1 down to 0
    {
        target = &g_entities[targNum[num]];
        // target must be a client
        if (!target->client)
            continue;
        // target must not be the player who fired the vortex grenade
        if (target == self->parent)		// makes sense here
            continue;
        // target must be able to take damage
        if (!target->takedamage)
            continue;
        G_ExplodeMissile( self);			// EXPLODE goes the weasel!
    }
}

/*
================
G_Boomerang
by DonX

VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))

NIGHTZ=======
normalize end
z[0] = end[0] + sin(deg2rad(angle));
z[1] = end[1] + cos(deg2rad(angle));
================
*//*
void G_Boomerang( gentity_t *self , vec3_t start ){

//vec3_t		s1, s2, s3, sC, dir, start, end;
//int			force;
vec3_t			start, end;

	// FIXME add in force dependancies based on increasing, decreasing values = longer loop swirve
	//force = 10;
	//VectorAdd( self->r.currentOrigin, mins, mins );
	//VectorAdd( self->r.currentOrigin, maxs, maxs );
	//VectorScale(self->s.pos.trBase, force, sC)
	//VectorNormalize(sC);
	//s1 = PerpendicularVector(s1, sC);
	//VectorCopy(s1, self->s.pos.trDelta );
	// Jump entity to s1 vector position if first think = 1, ahh first determine s1
	//VectorCopy(end, target->r.currentOrigin);
	//RotatePointAroundVector (self->s.pos.trDelta, forward, self->s.pos.trDelta, 30);

	self->nextthink = level.time + 2000; // Set when to swirve next

	VectorCopy (self->r.currentOrigin, start); // Set origin to start vec

//make trDelta = end position, and angle y factor, x will adjust accordingly based on Vector function

	end =

	//Travel distance based on time, then flip trDelta to move oppisite dir

	// make target owner of missle
	if (self->parent){
		VectorCopy(self->parent.r.currentOrigin, end);}


	// subtract start from end to get directional vector
	VectorSubtract(end, start, dir);
	VectorNormalize(dir);
	//make the missile go back to owner
	VectorCopy (self->parent.r.currentOrigin, self->r.currentOrigin );
	//let's save some bandwidth
	SnapVector( self->s.pos.trDelta );
}*/

#define HOME_TIMING		100			// the think time interval of G_HomeMissile
#define HOME_VELOCITY	50			// the amount of kick each second gets
#define HOME_RADIUS		4000		//the length of the search cone
//#define HOME_TRIGGER	10			//how close it will get before detonating
/*
================
G_HomeMissile
for the homing missiles
================
*/
void G_HomeMissile( gentity_t *self )
{
    gentity_t *testtarget, *target;
    vec3_t start, dir, end, mins, maxs, kvel;
    int targNum[MAX_GENTITIES], num;
    float bestlength = HOME_RADIUS;


    target = NULL;
    //check for special actions like splitting missiles
    if (level.time > self->wait)
    {
        //G_Printf( "checking wait times...\n");
        if( self->s.weapon == WP_LTORPEDO2 )
        {
            G_SplitMissile( self );
        }
        else
        {
            G_ExplodeMissile( self );
        }
        return;
    }
    self->nextthink = level.time + 40;
    //check if there are any entities within a radius of HOME_RADIUS units.
    mins[0] = -HOME_RADIUS * 1.42f;
    mins[1] = -HOME_RADIUS * 1.42f;
    mins[2] = -HOME_RADIUS * 1.42f;
    maxs[0] = HOME_RADIUS * 1.42f;
    maxs[1] = HOME_RADIUS * 1.42f;
    maxs[2] = HOME_RADIUS * 1.42f;

    VectorAdd( self->r.currentOrigin, mins, mins );
    VectorAdd( self->r.currentOrigin, maxs, maxs );

    num = trap_EntitiesInBox(mins,maxs,targNum,MAX_GENTITIES);
    for(num--; num > 0; num--)      // count from num-1 down to 0
    {
        testtarget = &g_entities[targNum[num]];

        // target must not be the missile itself
        if (testtarget == self)
            continue;
        // target must be a client
        if (!testtarget->client)
            continue;
        // target must not be the player who fired the missile
        if (testtarget == self->parent)
            continue;
        // target must be able to take damage
        if (!testtarget->takedamage)
            continue;
        // target must actually be in HOME_RADIUS
        if ( Distance(self->r.currentOrigin,testtarget->r.currentOrigin) > bestlength )
            continue;
        //can we see him?
        if( !visible( self, testtarget ) )
            continue;
        //we got a good one!
        bestlength = Distance(self->r.currentOrigin,testtarget->r.currentOrigin);
        target = testtarget;
    }
    if(!target)
    {
        return;
    }
    // put target position in end
    VectorCopy(target->r.currentOrigin, end);
    // put missle position in start
    VectorCopy(self->r.currentOrigin, start);
    // subtract start from end to get directional vector
    VectorSubtract(end, start, dir);
    VectorNormalize(dir);
    // scale directional vector by the kick factor and add to the targets velocity
    if( self->s.weapon == WP_LTORPEDO3 )
    {
        VectorScale(dir,1.25*HOME_VELOCITY / HOME_TIMING, kvel);
    }
    else if ( self->s.weapon == WP_DMATTER )
    {
        VectorScale(dir,2*HOME_VELOCITY / HOME_TIMING, kvel);
    }
    else
    {
        VectorScale(dir,HOME_VELOCITY / HOME_TIMING, kvel);
    }
    // add the kick velocity to the missile's velocity
    VectorAdd (self->s.pos.trDelta, kvel, dir);
    VectorCopy (dir, self->s.pos.trDelta);
    VectorNormalize( dir );
    //make the missile point toward the target
    vectoangles (dir, self->s.angles);
    //let's save some bandwidth
    SnapVector( self->s.pos.trDelta );
    /*if ( Distance(self->r.currentOrigin,target->r.currentOrigin) < HOME_TRIGGER )
    		G_ExplodeMissile( self);*/
}

#define MMINE_TIMING	50			// the think time interval of G_HomeMissile2
#define MMINE_VELOCITY	300			// the amount of kick each second gets
#define MMINE_RADIUS 384
#define MMINE_TRIGGER	10
/*
================
G_HomeMissile2
for the magnet mines
================
*/
void G_HomeMissile2( gentity_t *self )
{
    gentity_t *testtarget, *target;
    vec3_t start,dir,end,kvel,mins,maxs;
    int targNum[MAX_GENTITIES],num;
    float bestlength = MMINE_RADIUS;


    target = NULL;
    self->nextthink = level.time + 30;
    if (level.time > self->wait)
        G_ExplodeMissile( self);
    //check if there are any entities within a radius of 384 units.
    mins[0] = -MMINE_RADIUS * 1.42f;
    mins[1] = -MMINE_RADIUS * 1.42f;
    mins[2] = -MMINE_RADIUS * 1.42f;
    maxs[0] = MMINE_RADIUS * 1.42f;
    maxs[1] = MMINE_RADIUS * 1.42f;
    maxs[2] = MMINE_RADIUS * 1.42f;

    VectorAdd( self->r.currentOrigin, mins, mins );
    VectorAdd( self->r.currentOrigin, maxs, maxs );

    num = trap_EntitiesInBox(mins,maxs,targNum,MAX_GENTITIES);
    for(num--; num > 0; num--)      // count from num-1 down to 0
    {
        testtarget = &g_entities[targNum[num]];

        // target must not be the mine itself
        if (testtarget == self)
            continue;
        // target must be a client
        if (!testtarget->client)
            continue;
        // target must not be the player who fired the mine
        if (testtarget == self->parent)
            continue;
        // target must be able to take damage
        if (!testtarget->takedamage)
            continue;
        // target must actually be in MMINE_RADIUS
        if ( Distance(self->r.currentOrigin,testtarget->r.currentOrigin) > bestlength )
            continue;
        if( !visible( self, testtarget ) )
            continue;
        //we got a good one!
        bestlength = Distance(self->r.currentOrigin,testtarget->r.currentOrigin);
        target = testtarget;
    }
    if(!target)
    {
        //VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
        //VectorScale( self->s.pos.trDelta, 160, self->s.pos.trDelta );
        return;
    }

    // put target position in end
    VectorCopy(target->r.currentOrigin, end);
    // put grenade position in start
    VectorCopy(self->r.currentOrigin, start);
    // subtract start from end to get directional vector
    VectorSubtract(end, start, dir);
    VectorNormalize(dir);
    // scale directional vector by the kick factor and add to the targets velocity
    VectorScale(dir,MMINE_VELOCITY / MMINE_TIMING, kvel);
    // add the kick velocity to the missile's velocity
    //VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
    //self->s.pos.trTime = level.time - MMINE_TIMING;
    VectorAdd (self->s.pos.trDelta,kvel, self->s.pos.trDelta);
    /*if( VectorLength( self->s.pos.trDelta ) > 300 ){
    	VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
    	VectorNormalize( self->s.pos.trDelta );
    	VectorScale( self->s.pos.trDelta, 300, self->s.pos.trDelta );
    }*/
    //make the missile point toward the target
    //vectoangles (dir, self->s.angles);
    //let's save some bandwidth
    SnapVector( self->s.pos.trDelta );

    //Detonate when it gets close enough
    if ( Distance(self->r.currentOrigin,target->r.currentOrigin) < MMINE_TRIGGER )
        G_ExplodeMissile( self);
}

#define STRIKE_RADIUS		196			//the length of the search cone
/*
================
G_Strike
for the ball lightning, strikes a target with a hitscan within the radius
================
*/
void G_Strike( gentity_t *self )
{
    gentity_t *testtarget, *target;
    vec3_t mins, maxs;
    int targNum[MAX_GENTITIES], num;
    float bestlength = HOME_RADIUS;


    target = NULL;
    //check for special actions like splitting missiles
    if (level.time > self->wait)
    {
        G_ExplodeMissile( self );
    }
    self->nextthink = level.time + 500;
    //check if there are any entities within a radius of HOME_RADIUS units.
    mins[0] = -STRIKE_RADIUS * 1.42f;
    mins[1] = -STRIKE_RADIUS * 1.42f;
    mins[2] = -STRIKE_RADIUS * 1.42f;
    maxs[0] = STRIKE_RADIUS * 1.42f;
    maxs[1] = STRIKE_RADIUS * 1.42f;
    maxs[2] = STRIKE_RADIUS * 1.42f;

    VectorAdd( self->r.currentOrigin, mins, mins );
    VectorAdd( self->r.currentOrigin, maxs, maxs );

    num = trap_EntitiesInBox(mins,maxs,targNum,MAX_GENTITIES);
    for(num--; num > 0; num--)      // count from num-1 down to 0
    {
        testtarget = &g_entities[targNum[num]];

        // target must not be the missile itself
        if (testtarget == self)
            continue;
        // target must be a client
        if (!testtarget->client)
            continue;
        // target must not be the player who fired the missile
        if (testtarget == self->parent)
            continue;
        // target must be able to take damage
        if (!testtarget->takedamage)
            continue;
        // target must actually be in HOME_RADIUS
        if ( Distance(self->r.currentOrigin,testtarget->r.currentOrigin) > bestlength )
            continue;
        //can we see him?
        if( !visible( self, testtarget ) )
            continue;
        //we got a good one!
        bestlength = Distance(self->r.currentOrigin,testtarget->r.currentOrigin);
        target = testtarget;
    }
    if(!target)
    {
        return;
    }
    //damage the target
    G_Damage (target, self, self->parent, NULL,	NULL, 15, 0, self->methodOfDeath);
    //somehow signify the lightning strike
}

/*
================
G_RunMissile
================
*/
void G_RunMissile( gentity_t *ent )
{
    vec3_t		origin;
    trace_t		tr;
    int			passent;

    // get current position
    BG_EvaluateTrajectory( &ent->s.pos, level.time, origin ); // Passing s.pos trajectory_t struct (with all its values), dyamic level.time, and origin is the varible that the function will create a value for.

    // if this missile bounced off an invulnerability sphere
    if ( ent->target_ent )
    {
        passent = ent->target_ent->s.number;
    }
    else
    {
        // ignore interactions with the missile owner
        passent = ent->r.ownerNum;
    }
    // trace a line from the previous position to the current position
    trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, passent, ent->clipmask );

    // some weapons go through players or walls
    if( ( ent->s.weapon != WP_DSHOT) && tr.entityNum )
    {
        if ( tr.startsolid || tr.allsolid )
        {
            // make sure the tr.entityNum is set to the entity we're stuck in
            trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, passent, ent->clipmask );
            tr.fraction = 0;
        }
        else
        {
            VectorCopy( tr.endpos, ent->r.currentOrigin );
        }
    }

    trap_LinkEntity( ent );

    // check for rolling
    switch ( ent->s.weapon )
    {
    case WP_SICE3:
    case WP_AWAVE:
    case WP_RSTONE:
        //storm fist level 2
        //case WP_SFIST2:
        //case WP_TEMPEST:
        G_RollMissile( ent, &tr );
        break;
    default:
        break;
    }

    if ( tr.fraction != 1 )
    {
        // never explode or bounce on sky
        if ( tr.surfaceFlags & SURF_NOIMPACT )
        {
            // If grapple, reset owner
            if (ent->parent && ent->parent->client && ent->parent->client->hook == ent)
            {
                ent->parent->client->hook = NULL;
            }
            G_FreeEntity( ent );
            return;
        }

        G_MissileImpact( ent, &tr );
        if ( ent->s.eType != ET_MISSILE )
        {
            return;		// exploded
        }
    }

    // check think function after bouncing
    G_RunThink( ent );
}


//=============================================================================

/*
=================
fire_grapple
=================
*/
gentity_t *fire_grapple (gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t	*hook;

    VectorNormalize (dir);

    hook = G_Spawn();
    hook->classname = "hook";
    hook->nextthink = level.time + 10000;
    hook->think = Weapon_HookFree;
    hook->s.eType = ET_MISSILE;
    hook->r.svFlags = SVF_USE_CURRENT_ORIGIN;
    hook->s.weapon = WP_STRIKECHAIN;
    hook->r.ownerNum = self->s.number;
    hook->methodOfDeath = MOD_GRAPPLE;
    hook->clipmask = MASK_SHOT;
    hook->parent = self;
    hook->target_ent = NULL;

    hook->s.pos.trType = TR_LINEAR;
    hook->s.pos.trTime = level.time;
    hook->s.otherEntityNum = self->s.number; // use to match beam in client
    VectorCopy( start, hook->s.pos.trBase );
    VectorScale( dir, 800, hook->s.pos.trDelta );
    SnapVector( hook->s.pos.trDelta );			// save net bandwidth
    VectorCopy (start, hook->r.currentOrigin);

    self->client->hook = hook;

    return hook;
}
