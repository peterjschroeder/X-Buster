// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_pmove.c -- both games player movement code
// takes a playerstate and a usercmd as input and returns a modifed playerstate

#include "../qcommon/q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

pmove_t		*pm;
pml_t		pml;

// movement parameters
float	pm_stopspeed = 100.0f;
float	pm_duckScale = 0.25f;
float	pm_swimScale = 0.50f;
float	pm_wadeScale = 0.70f;
float	pm_ladderScale = 0.50f;

float	pm_accelerate = 10.0f;
float	pm_airaccelerate = 1.0f;
float	pm_wateraccelerate = 4.0f;
float	pm_flyaccelerate = 8.0f;
float   pm_ladderAccelerate = 3000.0f;

float	pm_friction = 6.0f;
float	pm_waterfriction = 1.0f;
float	pm_flightfriction = 3.0f;
float	pm_wallfriction = 10.0f;
float	pm_spectatorfriction = 5.0f;
float   pm_ladderfriction = 3000.0f;

int		c_pmove = 0;


/*
===============
PM_AddEvent

===============
*/
void PM_AddEvent( int newEvent )
{
    BG_AddPredictableEventToPlayerstate( newEvent, 0, pm->ps );
}

/*
===============
PM_AddTouchEnt
===============
*/
void PM_AddTouchEnt( int entityNum )
{
    int		i;

    if ( entityNum == ENTITYNUM_WORLD )
    {
        return;
    }
    if ( pm->numtouch == MAXTOUCH )
    {
        return;
    }

    // see if it is already added
    for ( i = 0 ; i < pm->numtouch ; i++ )
    {
        if ( pm->touchents[ i ] == entityNum )
        {
            return;
        }
    }

    // add it
    pm->touchents[pm->numtouch] = entityNum;
    pm->numtouch++;
}

/*
===================
PM_StartTorsoAnim
===================
*/
// for head animations
static void PM_StartHeadAnim( int anim )
{
    if ( pm->ps->pm_type >= PM_DEAD || (pm->ps->pm_type == PM_FREEZE))
    {
        return;
    }
    pm->ps->generic1 = ( ( pm->ps->generic1 & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
                       | anim;
}
static void PM_ContinueHeadAnim( int anim )
{
    if ( ( pm->ps->generic1 & ~ANIM_TOGGLEBIT ) == anim || (pm->ps->pm_type == PM_FREEZE))
    {
        return;
    }
    PM_StartHeadAnim( anim );
}

static void PM_StartTorsoAnim( int anim )
{
    if ( pm->ps->pm_type >= PM_DEAD || (pm->ps->pm_type == PM_FREEZE))
    {
        return;
    }
    pm->ps->torsoAnim = ( ( pm->ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
                        | anim;
}
static void PM_StartLegsAnim( int anim )
{
    if ( pm->ps->pm_type >= PM_DEAD || (pm->ps->pm_type == PM_FREEZE))
    {
        return;
    }
    if ( pm->ps->legsTimer > 0 )
    {
        return;		// a high priority animation is running
    }
    pm->ps->legsAnim = ( ( pm->ps->legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )
                       | anim;
}

static void PM_ContinueLegsAnim( int anim )
{
    if ( ( pm->ps->legsAnim & ~ANIM_TOGGLEBIT ) == anim || (pm->ps->pm_type == PM_FREEZE))
    {
        return;
    }
    if ( pm->ps->legsTimer > 0 )
    {
        return;		// a high priority animation is running
    }
    PM_StartLegsAnim( anim );
}

static void PM_ContinueTorsoAnim( int anim )
{
    if ( ( pm->ps->torsoAnim & ~ANIM_TOGGLEBIT ) == anim || (pm->ps->pm_type == PM_FREEZE))
    {
        return;
    }
    if ( pm->ps->torsoTimer > 0 )
    {
        return;		// a high priority animation is running
    }
    PM_StartTorsoAnim( anim );
}

static void PM_ForceLegsAnim( int anim )
{
    if (pm->ps->pm_type == PM_FREEZE)
        return;

    pm->ps->legsTimer = 0;
    PM_StartLegsAnim( anim );
}

static void PM_ForceTorsoAnim( int anim )
{
    if (pm->ps->pm_type == PM_FREEZE)
        return;

    pm->ps->torsoTimer = 0;
    PM_StartTorsoAnim( anim );
}

/*
==================
PM_ClipVelocity

Slide off of the impacting surface
==================
*/
void PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce )
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
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
static void PM_Friction( void )
{
    vec3_t	vec;
    float	*vel;
    float	speed, newspeed, control;
    float	drop;

    vel = pm->ps->velocity;

    VectorCopy( vel, vec );
    if ( pml.walking )
    {
        vec[2] = 0;	// ignore slope movement
    }

    speed = VectorLength(vec);
    if (speed < 1)
    {
        vel[0] = 0;
        vel[1] = 0;		// allow sinking underwater
        // FIXME: still have z friction underwater?
        return;
    }

    drop = 0;

    // apply ground friction
    //if ( pm->waterlevel <= 1 ) {
    if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) )
    {
        // if getting knocked back, no friction
        if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) )
        {
            control = speed < pm_stopspeed ? pm_stopspeed : speed;
            drop += control*pm_friction*pml.frametime;
        }
    }
    //}

    // apply water friction even if just wading
    // we only want the friction if you're going down
    if ( pm->waterlevel && ( pm->ps->velocity[2] < 0 ) )
    {
        drop += speed*pm_waterfriction*pm->waterlevel*pml.frametime;
    }

    if ( pml.ladder ) // If they're on a ladder...
    {
        drop += speed*pm_ladderfriction*pml.frametime;  // Add ladder friction!
    }

    // friction for Dash
    if ( pm->ps->powerups[PW_DASH])
    {
        drop += speed*pm_flightfriction*pml.frametime;
    }
    // friction for Jetpack
    if ( pm->ps->powerups[PW_JETPACK])
    {
        drop += speed*pm_flightfriction*pml.frametime;
    }
    // friction for Wall Hanging
    if( pm->ps->pm_flags & PMF_WALL_HANG )
    {
        drop += speed*pm_wallfriction*pml.frametime;
    }

    if ( pm->ps->pm_type == PM_SPECTATOR)
    {
        drop += speed*pm_spectatorfriction*pml.frametime;
    }

    // scale the velocity
    newspeed = speed - drop;
    if (newspeed < 0)
    {
        newspeed = 0;
    }
    newspeed /= speed;

    vel[0] = vel[0] * newspeed;
    vel[1] = vel[1] * newspeed;
    vel[2] = vel[2] * newspeed;
}


/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/
static void PM_Accelerate( vec3_t wishdir, float wishspeed, float accel )
{
#if 1
    // proper way (avoids strafe jump maxspeed bug), but feels bad
    vec3_t		wishVelocity;
    vec3_t		pushDir;
    float		pushLen;
    float		canPush;

    VectorScale( wishdir, wishspeed, wishVelocity );
    VectorSubtract( wishVelocity, pm->ps->velocity, pushDir );
    pushLen = VectorNormalize( pushDir );

    canPush = accel*pml.frametime*wishspeed;
    if (canPush > pushLen)
    {
        canPush = pushLen;
    }
    VectorMA( pm->ps->velocity, canPush, pushDir, pm->ps->velocity );
}
#else
// q2 style
    int			i;
    float		addspeed, accelspeed, currentspeed;

    currentspeed = DotProduct (pm->ps->velocity, wishdir);
    addspeed = wishspeed - currentspeed;
    if (addspeed <= 0)
    {
        return;
    }
    accelspeed = accel*pml.frametime*wishspeed;
    if (accelspeed > addspeed)
    {
        accelspeed = addspeed;
    }

    for (i=0 ; i<3 ; i++)
    {
        pm->ps->velocity[i] += accelspeed*wishdir[i];
    }
#endif

/*
============
PM_CmdScale

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_CmdScale( usercmd_t *cmd )
{
    int		max;
    float	total;
    float	scale;

    max = abs( cmd->forwardmove );
    if ( abs( cmd->rightmove ) > max )
    {
        max = abs( cmd->rightmove );
    }
    if ( abs( cmd->upmove ) > max )
    {
        max = abs( cmd->upmove );
    }
    if ( !max )
    {
        return 0;
    }

    total = sqrt( cmd->forwardmove * cmd->forwardmove
                  + cmd->rightmove * cmd->rightmove + cmd->upmove * cmd->upmove );
    scale = (float)pm->ps->speed * max / ( 127.0 * total );

    return scale;
}


/*
================
PM_SetMovementDir

Determine the rotation of the legs reletive
to the facing dir
================
*/
static void PM_SetMovementDir( void )
{
    if ( pm->cmd.forwardmove || pm->cmd.rightmove )
    {
        if ( pm->cmd.rightmove == 0 && pm->cmd.forwardmove > 0 )
        {
            pm->ps->movementDir = 0;
        }
        else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove > 0 )
        {
            pm->ps->movementDir = 1;
        }
        else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove == 0 )
        {
            pm->ps->movementDir = 2;
        }
        else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove < 0 )
        {
            pm->ps->movementDir = 3;
        }
        else if ( pm->cmd.rightmove == 0 && pm->cmd.forwardmove < 0 )
        {
            pm->ps->movementDir = 4;
        }
        else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove < 0 )
        {
            pm->ps->movementDir = 5;
        }
        else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove == 0 )
        {
            pm->ps->movementDir = 6;
        }
        else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove > 0 )
        {
            pm->ps->movementDir = 7;
        }
    }
    else
    {
        // if they aren't actively going directly sideways,
        // change the animation to the diagonal so they
        // don't stop too crooked
        if ( pm->ps->movementDir == 2 )
        {
            pm->ps->movementDir = 1;
        }
        else if ( pm->ps->movementDir == 6 )
        {
            pm->ps->movementDir = 7;
        }
    }
}

/*
=============
PM_CheckAirJump
=============
*/
static qboolean PM_CheckAirJump( void )
{
    qboolean	not_charging = ( pm->ps->weaponstate != WEAPON_CHARGING ) && ( pm->ps->weaponstate != WEAPON_OHCHARGING );

    if ( pm->ps->pm_flags & PMF_RESPAWNED )
    {
        return qfalse;		// don't allow jump until all buttons are up
    }

    if ( pm->cmd.upmove < 10 )
    {
        // not holding jump
        return qfalse;
    }

    // must wait for jump to be released
    if ( pm->ps->pm_flags & PMF_JUMP_HELD )
    {
        // clear upmove so cmdscale doesn't lower running speed
        pm->cmd.upmove = 0;
        return qfalse;
    }

    if( pml.groundPlane )
        return qfalse;
    //if you dont even have the right upgrade, go away!
    if( !(pm->ps->persistant[PERS_UPGRADES] & (1 << UG_AIRJUMP)) )
        return qfalse;

    //if all jump flags are set, go away!
    if( pm->ps->pm_flags & PMF_AIR_JUMP1 )
    {
        return qfalse;
    }

    pm->ps->pm_flags |= PMF_AIR_JUMP1;

    //cancel tech attacks
    if( pm->ps->weaponstate >= WEAPON_TECHUPCUT )
    {
        pm->ps->weaponstate = WEAPON_READY;
    }

    pml.groundPlane = qfalse;		// jumping away
    pml.walking = qfalse;
    pm->ps->pm_flags |= PMF_JUMP_HELD;

    pm->ps->groundEntityNum = ENTITYNUM_NONE;
    if( pm->ps->velocity[2] < bg_playerlist[pm->ps->persistant[PLAYERCLASS]].jumpvelocity )
    {
        pm->ps->velocity[2] = pm->ps->velocity[2] = bg_playerlist[pm->ps->persistant[PLAYERCLASS]].jumpvelocity;
        PM_AddEvent( EV_JUMP );
    }

    if ( pm->cmd.forwardmove >= 0 )
    {
        if( not_charging )
        {
            PM_StartHeadAnim( HEAD_DJUMP );//
            PM_StartTorsoAnim( TORSO_DJUMP );//
            pm->ps->torsoTimer = 500;
        }
        else
        {
            PM_StartHeadAnim( HEAD_CHARGE_JUMP );
            PM_StartTorsoAnim( TORSO_CHARGE_JUMP );
            pm->ps->torsoTimer = 500;
        }
        PM_ForceLegsAnim( LEGS_DJUMP );//
        pm->ps->legsTimer = 500;
        pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
    }
    else
    {
        if( not_charging )
        {
            PM_StartHeadAnim( HEAD_DJUMP );
            PM_StartTorsoAnim( TORSO_DJUMP );//
            pm->ps->torsoTimer = 500;
        }
        else
        {
            PM_StartHeadAnim( HEAD_CHARGE2_JUMP );
            PM_StartTorsoAnim( TORSO_CHARGE2_JUMP );
            pm->ps->torsoTimer = 500;
        }
        PM_ForceLegsAnim( LEGS_DJUMP );//
        pm->ps->legsTimer = 500;
        pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
    }

    return qtrue;
}

/*
=============
PM_CheckJump
=============
*/
static qboolean PM_CheckJump( void )
{
    qboolean	not_charging = ( pm->ps->weaponstate != WEAPON_CHARGING ) && ( pm->ps->weaponstate != WEAPON_OHCHARGING );

    if ( pm->ps->pm_flags & PMF_RESPAWNED )
    {
        return qfalse;		// don't allow jump until all buttons are up
    }

    if( pm->ps->weaponstate != WEAPON_TECHUPCUT )
    {
        if ( pm->cmd.upmove < 10 )
        {
            // not holding jump
            return qfalse;
        }
        // must wait for jump to be released
        if ( pm->ps->pm_flags & PMF_JUMP_HELD )
        {
            // clear upmove so cmdscale doesn't lower running speed
            pm->cmd.upmove = 0;
            return qfalse;
        }
    }

    pml.groundPlane = qfalse;		// jumping away
    pml.walking = qfalse;
    pm->ps->pm_flags |= PMF_JUMP_HELD;

    pm->ps->groundEntityNum = ENTITYNUM_NONE;
    //if you are in the megaman class, and you have the leg upgrade, your jumpheight is increased
    if( pm->ps->persistant[PERS_UPGRADES] & (1 << UG_HIGHJUMP) )
        pm->ps->velocity[2] = bg_playerlist[pm->ps->persistant[PLAYERCLASS]].jumpvelocity*1.75f;
    else
        pm->ps->velocity[2] = bg_playerlist[pm->ps->persistant[PLAYERCLASS]].jumpvelocity;
    PM_AddEvent( EV_JUMP );

    if ( pm->cmd.forwardmove >= 0 )
    {
        if( not_charging )
        {
            PM_StartHeadAnim( HEAD_JUMP );
            PM_StartTorsoAnim( TORSO_JUMP );
            pm->ps->torsoTimer = 400; //
        }
        PM_ForceLegsAnim( LEGS_JUMP );
        pm->ps->legsTimer = 400; //
        pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
    }
    else
    {
        if( not_charging )
        {
            PM_StartHeadAnim( HEAD_JUMPB );
            PM_StartTorsoAnim( TORSO_JUMPB );
            pm->ps->torsoTimer = 500;
        }
        PM_ForceLegsAnim( LEGS_JUMPB );
        pm->ps->legsTimer = 500;
        pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
    }

    return qtrue;
}

/*
===================
NIGHTZ - WALLJUMPMOVE
===================
*/
static void PM_WallJumpMove( void )
{
    int			i;
    vec3_t		wishvel;
    float		fmove, smove;
    vec3_t		wishdir;
    float		wishspeed;
    float		scale;
    usercmd_t	cmd;

    PM_Friction();

    PM_ForceTorsoAnim (TORSO_WALL);
    PM_ForceLegsAnim (LEGS_WALL);

    pm->ps->eFlags |= EF_WALLHANG;

    if (pm->ps->commandLock)
    {
        fmove = smove = 0;
        pm->ps->commandLock--;

        if (pm->ps->commandLock < 0)
            pm->ps->commandLock = 0;
    }
    else
    {
        fmove = pm->cmd.forwardmove;
        smove = pm->cmd.rightmove;
    }

    cmd = pm->cmd;
    scale = PM_CmdScale( &cmd );

    // set the movementDir so clients can rotate the legs for strafing
    PM_SetMovementDir();

    // project moves down to flat plane
    pml.forward[2] = 0;
    pml.right[2] = 0;
    VectorNormalize (pml.forward);
    VectorNormalize (pml.right);

    for ( i = 0 ; i < 2 ; i++ )
    {
        wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
    }
    wishvel[2] = 0;

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);
    wishspeed *= scale;

    // Thats Walljump heh
    if (cmd.upmove > 0)
    {
        pm->ps->wallJumpTimer++;
    }
    else
        // Common walljump
        if ((cmd.upmove <= 0)&&(pm->ps->wallJumpTimer > 0)&&(pm->ps->wallJumpTimer < 20))
        {
            vec3_t direction;

            PM_ForceTorsoAnim (TORSO_JUMP);
            PM_ForceLegsAnim (LEGS_JUMP);

            // This will drive the player a bit backward the wall, like in megaman series
            VectorCopy (pml.forward, direction);
            VectorNegate (direction, direction);
            VectorScale (direction, 40, direction);
            direction[2] = 500;
            VectorCopy (direction, pm->ps->velocity);

            pm->ps->wallJumpTimer = 0;
            pm->ps->smoothAngle = 0;
            pm->ps->commandLock = 12;
            pm->cmd.forwardmove = 0;
        }
        else
            // Jump backwards
            if ((cmd.upmove <= 0)&&(pm->ps->wallJumpTimer >= 20))
            {
                vec3_t temp;

                PM_ForceTorsoAnim (TORSO_JUMP);
                PM_ForceLegsAnim (LEGS_JUMP);

                VectorCopy (pml.forward, temp);
                VectorNormalize (temp);
                VectorScale (temp, 150, temp);
                VectorNegate (temp, temp);
                temp[2] = 400;

                VectorCopy (temp, pm->ps->velocity);
                pm->ps->wallJumpTimer = 0;
                pm->ps->smoothAngle = 1;
            }

    // not on ground, so little effect on velocity
    PM_Accelerate (wishdir, wishspeed, pm_airaccelerate);

    // we may have a ground plane that is very steep, even
    // though we don't have a groundentity
    // slide along the steep plane
    if ( pml.groundPlane )
    {
        PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal,
                         pm->ps->velocity, OVERCLIP );
    }

    PM_StepSlideMove ( qtrue );
}

/*
=============
NIGHTZ - WALLJUMP CHECK
=============
*/
static qboolean PM_CheckWallJump ( void )
{
    vec3_t	spot;
    vec3_t	flatforward;
    vec3_t	groundVector;
    trace_t	wallTrace;
    trace_t	groundTrace;

    if (pm->ps->pm_time)
        return qfalse;

    // check for water jump
    if ( pm->waterlevel > 1 )
        return qfalse;

    if(!(pm->ps->persistant[PERS_UPGRADES] & (1 << UG_WALLJUMP)))
    {
        pm->ps->pm_flags &= ~PMF_WALL_HANG;
        pm->ps->eFlags &= ~EF_WALLHANG;
        return qfalse;
    }

    flatforward[0] = pml.forward[0];
    flatforward[1] = pml.forward[1];
    flatforward[2] = 0;
    VectorNormalize (flatforward);

    VectorCopy (pm->ps->origin, groundVector);
    groundVector [2] -= 1;

    VectorMA (pm->ps->origin, 1, flatforward, spot);
    spot[2] += 5;

    pm->trace (&wallTrace, pm->ps->origin, pm->mins, pm->maxs, spot, pm->ps->clientNum, MASK_SOLID);
    pm->trace (&groundTrace, pm->ps->origin, pm->mins, pm->maxs, groundVector, pm->ps->clientNum, MASK_SOLID);

    if (wallTrace.fraction < 1 && groundTrace.fraction >= 1)
    {
        pm->ps->pm_flags |= PMF_WALL_HANG;
        return qtrue;
    }
    else
    {
        pm->ps->pm_flags &= ~PMF_WALL_HANG;
        pm->ps->eFlags &= ~EF_WALLHANG;
        return qfalse;
    }
}

//============================================================================


/*
===================
PM_WaterJumpMove

Flying out of the water
===================
*/
static void PM_WaterJumpMove( void )
{
    // waterjump has no control, but falls

    PM_StepSlideMove( qtrue );

    pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
    if (pm->ps->velocity[2] < 0)
    {
        // cancel as soon as we are falling down again
        pm->ps->pm_flags &= ~PMF_ALL_TIMES;
        pm->ps->pm_time = 0;
    }
}


/*
===================
PM_FlyMove

Only with the flight powerup
===================
*/
static void PM_FlyMove( void )
{
    int		i;
    vec3_t	wishvel;
    float	wishspeed;
    vec3_t	wishdir;
    float	scale;

    // normal slowdown
    PM_Friction ();

    scale = PM_CmdScale( &pm->cmd );
    //
    // user intentions
    //
    if ( !scale )
    {
        wishvel[0] = 0;
        wishvel[1] = 0;
        wishvel[2] = 0;
    }
    else
    {
        for (i=0 ; i<3 ; i++)
        {
            wishvel[i] = scale * pml.forward[i]*pm->cmd.forwardmove + scale * pml.right[i]*pm->cmd.rightmove;
        }

        wishvel[2] += scale * pm->cmd.upmove;
    }

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);

    PM_Accelerate (wishdir, wishspeed, pm_flyaccelerate);

    PM_StepSlideMove( qfalse );
}


/*
===================
PM_LadderMove()
by: Calrathan [Arthur Tomlin]

Right now all I know is that this works for VERTICAL ladders.
Ladders with angles on them (urban2 for AQ2) haven't been tested.
===================
*/
static void PM_LadderMove( void )
{
    int i;
    vec3_t wishvel;
    float wishspeed;
    vec3_t wishdir;
    float scale;
    float vel;

    PM_Friction ();

    scale = PM_CmdScale( &pm->cmd );

    // user intentions [what the user is attempting to do]
    if ( !scale )
    {
        wishvel[0] = 0;
        wishvel[1] = 0;
        wishvel[2] = 0;
    }
    else     // if they're trying to move... lets calculate it
    {
        for (i=0 ; i<3 ; i++)
            wishvel[i] = scale * pml.forward[i]*pm->cmd.forwardmove +
                         scale * pml.right[i]*pm->cmd.rightmove;
        wishvel[2] += scale * pm->cmd.upmove;
    }

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);

    if ( wishspeed > pm->ps->speed * pm_ladderScale )
    {
        wishspeed = pm->ps->speed * pm_ladderScale;
    }

    PM_Accelerate (wishdir, wishspeed, pm_ladderAccelerate);

    // This SHOULD help us with sloped ladders, but it remains untested.
    if ( pml.groundPlane && DotProduct( pm->ps->velocity,
                                        pml.groundTrace.plane.normal ) < 0 )
    {
        vel = VectorLength(pm->ps->velocity);
        // slide along the ground plane [the ladder section under our feet]
        PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal,
                         pm->ps->velocity, OVERCLIP );

        VectorNormalize(pm->ps->velocity);
        VectorScale(pm->ps->velocity, vel, pm->ps->velocity);
    }

    PM_SlideMove( qfalse ); // move without gravity
}


/*
=============
CheckLadder [ ARTHUR TOMLIN ]
=============
*/
void CheckLadder( void )
{
    vec3_t flatforward,spot;
    trace_t trace;
    pml.ladder = qfalse;
    // check for ladder
    flatforward[0] = pml.forward[0];
    flatforward[1] = pml.forward[1];
    flatforward[2] = 0;
    VectorNormalize (flatforward);
    VectorMA (pm->ps->origin, 1, flatforward, spot);
    pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, spot,
               pm->ps->clientNum, MASK_PLAYERSOLID);

    if ((trace.fraction < 1) && (trace.surfaceFlags & SURF_LADDER))
        pml.ladder = qtrue;

}

/*
===================
PM_AirMove

===================
*/
static void PM_AirMove( void )
{
    int			i;
    vec3_t		wishvel;
    float		fmove, smove;
    vec3_t		wishdir;
    float		wishspeed;
    float		scale;
    usercmd_t	cmd;

    // check for air jumps
    if ( !PM_CheckAirJump () && !PM_CheckWallJump() )
    {
        // play the fall animation only if you are falling past a certain speed and/or distance
        trace_t	trace;
        vec3_t	point;
        VectorCopy( pm->ps->origin, point );
        point[2] -= 16;
        pm->trace( &trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask );
        if((pm->ps->velocity[2] < 0) && (trace.fraction == 1.0f) && !(pm->ps->pm_flags & PMF_WALL_HANG) )
        {
            PM_ContinueLegsAnim( LEGS_FALL );
            if( pm->ps->weaponstate == WEAPON_TECHUPCUT )
            {
                pm->ps->weaponstate = WEAPON_READY;
            }
        }
    }

    PM_Friction();

    // NIGHTZ - LOCK COMMANDS
    if (pm->ps->commandLock)
    {
        fmove = smove = 0;
        pm->ps->commandLock--;

        if (pm->ps->commandLock < 0)
            pm->ps->commandLock = 0;
    }
    else
    {
        fmove = pm->cmd.forwardmove;
        smove = pm->cmd.rightmove;
    }

    cmd = pm->cmd;
    scale = PM_CmdScale( &cmd );

    // set the movementDir so clients can rotate the legs for strafing
    PM_SetMovementDir();

    // project moves down to flat plane
    pml.forward[2] = 0;
    pml.right[2] = 0;
    VectorNormalize (pml.forward);
    VectorNormalize (pml.right);

    for ( i = 0 ; i < 2 ; i++ )
    {
        wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
    }
    wishvel[2] = 0;

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);
    wishspeed *= scale;

    // not on ground, so little effect on velocity
    PM_Accelerate (wishdir, wishspeed, pm_airaccelerate);

    // we may have a ground plane that is very steep, even
    // though we don't have a groundentity
    // slide along the steep plane
    if ( pml.groundPlane )
    {
        PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal,
                         pm->ps->velocity, OVERCLIP );
    }

#if 0
    //ZOID:  If we are on the grapple, try stair-stepping
    //this allows a player to use the grapple to pull himself
    //over a ledge
    if (pm->ps->pm_flags & PMF_GRAPPLE_PULL)
        PM_StepSlideMove ( qtrue );
    else
        PM_SlideMove ( qtrue );
#endif

    PM_StepSlideMove ( qtrue );
}

/*
===================
PM_GrappleMove

===================
*/
static void PM_GrappleMove( void )
{
    vec3_t vel, v;
    float vlen;

    VectorScale(pml.forward, -16, v);
    VectorAdd(pm->ps->grapplePoint, v, v);
    VectorSubtract(v, pm->ps->origin, vel);
    vlen = VectorLength(vel);
    VectorNormalize( vel );

    if (vlen <= 100)
        VectorScale(vel, 10 * vlen, vel);
    else
        VectorScale(vel, 800, vel);

    VectorCopy(vel, pm->ps->velocity);

    pml.groundPlane = qfalse;
}

/*
===================
PM_WalkMove

===================
*/
static void PM_WalkMove( void )
{
    int			i;
    vec3_t		wishvel;
    float		fmove, smove;
    vec3_t		wishdir;
    float		wishspeed;
    float		scale;
    usercmd_t	cmd;
    float		accelerate;
    float		vel;


    if ( PM_CheckJump () )
    {
        PM_AirMove();
        return;
    }

    PM_Friction ();

    fmove = pm->cmd.forwardmove;
    smove = pm->cmd.rightmove;

    cmd = pm->cmd;
    scale = PM_CmdScale( &cmd );

    // set the movementDir so clients can rotate the legs for strafing
    PM_SetMovementDir();

    // project moves down to flat plane
    pml.forward[2] = 0;
    pml.right[2] = 0;

    // project the forward and right directions onto the ground plane
    PM_ClipVelocity (pml.forward, pml.groundTrace.plane.normal, pml.forward, OVERCLIP );
    PM_ClipVelocity (pml.right, pml.groundTrace.plane.normal, pml.right, OVERCLIP );
    //
    VectorNormalize (pml.forward);
    VectorNormalize (pml.right);

    for ( i = 0 ; i < 3 ; i++ )
    {
        wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
    }
    // when going up or down slopes the wish velocity should Not be zero
//	wishvel[2] = 0;

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);
    wishspeed *= scale;

    // clamp the speed lower if wading or walking on the bottom
    if ( pm->waterlevel )
    {
        float	waterScale;

        waterScale = pm->waterlevel / 3.0;
        waterScale = 1.0 - ( 1.0 - pm_swimScale ) * waterScale;
        if ( wishspeed > pm->ps->speed * waterScale )
        {
            wishspeed = pm->ps->speed * waterScale;
        }
    }

    // when a player gets hit, they temporarily lose
    // full control, which allows them to be moved a bit
    if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK )
    {
        accelerate = pm_airaccelerate;
    }
    else
    {
        accelerate = pm_accelerate;
    }

    PM_Accelerate (wishdir, wishspeed, accelerate);

    //Com_Printf( "velocity = %1.1f %1.1f %1.1f\n", pm->ps->velocity[0], pm->ps->velocity[1], pm->ps->velocity[2]);
    //Com_Printf( "velocity1 = %1.1f\n", VectorLength(pm->ps->velocity));

    if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK )
    {
        pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
    }
    else
    {
        // don't reset the z velocity for slopes
//		pm->ps->velocity[2] = 0;
    }

    vel = VectorLength(pm->ps->velocity);

    // slide along the ground plane
    PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal,
                     pm->ps->velocity, OVERCLIP );

    // don't decrease velocity when going up or down a slope
    VectorNormalize(pm->ps->velocity);
    VectorScale(pm->ps->velocity, vel, pm->ps->velocity);

    // don't do anything if standing still
    if (!pm->ps->velocity[0] && !pm->ps->velocity[1])
    {
        return;
    }

    PM_StepSlideMove( qfalse );

    //Com_Printf( "velocity2 = %1.1f\n", VectorLength(pm->ps->velocity));

}


/*
==============
PM_DeadMove
==============
*/
static void PM_DeadMove( void )
{
    float	forward;

    if ( !pml.walking )
    {
        return;
    }

    // extra friction

    forward = VectorLength (pm->ps->velocity);
    forward -= 20;
    if ( forward <= 0 )
    {
        VectorClear (pm->ps->velocity);
    }
    else
    {
        VectorNormalize (pm->ps->velocity);
        VectorScale (pm->ps->velocity, forward, pm->ps->velocity);
    }
}


/*
===============
PM_NoclipMove
===============
*/
static void PM_NoclipMove( void )
{
    float	speed, drop, friction, control, newspeed;
    int			i;
    vec3_t		wishvel;
    float		fmove, smove;
    vec3_t		wishdir;
    float		wishspeed;
    float		scale;

    pm->ps->viewheight = DEFAULT_VIEWHEIGHT;

    // friction

    speed = VectorLength (pm->ps->velocity);
    if (speed < 1)
    {
        VectorCopy (vec3_origin, pm->ps->velocity);
    }
    else
    {
        drop = 0;

        friction = pm_friction*1.5;	// extra friction
        control = speed < pm_stopspeed ? pm_stopspeed : speed;
        drop += control*friction*pml.frametime;

        // scale the velocity
        newspeed = speed - drop;
        if (newspeed < 0)
            newspeed = 0;
        newspeed /= speed;

        VectorScale (pm->ps->velocity, newspeed, pm->ps->velocity);
    }

    // accelerate
    scale = PM_CmdScale( &pm->cmd );

    fmove = pm->cmd.forwardmove;
    smove = pm->cmd.rightmove;

    for (i=0 ; i<3 ; i++)
        wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
    wishvel[2] += pm->cmd.upmove;

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);
    wishspeed *= scale;

    PM_Accelerate( wishdir, wishspeed, pm_accelerate );

    // move
    VectorMA (pm->ps->origin, pml.frametime, pm->ps->velocity, pm->ps->origin);
}

//============================================================================

/*
================
PM_FootstepForSurface

Returns an event number apropriate for the groundsurface
================
*/
static int PM_FootstepForSurface( void )
{
    if ( pml.groundTrace.surfaceFlags & SURF_NOSTEPS )
    {
        return 0;
    }
    if ( pml.groundTrace.surfaceFlags & SURF_METALSTEPS )
    {
        return EV_FOOTSTEP_METAL;
    }
    return EV_FOOTSTEP;
}


/*
=================
PM_CrashLand

Check for hard landings that generate sound events
=================
*/
static void PM_CrashLand( void )
{
    float		delta;
    float		dist;
    float		vel, acc;
    float		t;
    float		a, b, c, den;
    qboolean	not_charging = ( pm->ps->weaponstate != WEAPON_CHARGING ) && ( pm->ps->weaponstate != WEAPON_OHCHARGING );

    // decide which landing animation to use
    if ( pm->ps->pm_flags & PMF_BACKWARDS_JUMP )
    {
        if( not_charging )
        {
            PM_StartHeadAnim( HEAD_LANDB );
            PM_StartTorsoAnim( TORSO_LANDB );
        }
        PM_ForceLegsAnim( LEGS_LANDB );
    }
    else
    {
        if( not_charging )
        {
            PM_StartHeadAnim( HEAD_LAND );
            PM_StartTorsoAnim( TORSO_LAND );
        }
        PM_ForceLegsAnim( LEGS_LAND );
    }

    pm->ps->legsTimer = TIMER_LAND;

    // calculate the exact velocity on landing
    dist = pm->ps->origin[2] - pml.previous_origin[2];
    vel = pml.previous_velocity[2];
    acc = -pm->ps->gravity;

    a = acc / 2;
    b = vel;
    c = -dist;

    den =  b * b - 4 * a * c;
    if ( den < 0 )
    {
        return;
    }
    t = (-b - sqrt( den ) ) / ( 2 * a );

    delta = vel + t * acc;
    delta = delta*delta * 0.0001;

    // never take falling damage if completely underwater
    if ( pm->waterlevel == 3 )
    {
        return;
    }

    // reduce falling damage if there is standing water
    if ( pm->waterlevel == 2 )
    {
        delta *= 0.25;
    }
    if ( pm->waterlevel == 1 )
    {
        delta *= 0.5;
    }

    if ( delta < 1 )
    {
        return;
    }

    // create a local entity event to play the sound

    // SURF_NODAMAGE is used for bounce pads where you don't ever
    // want to take damage or play a crunch sound
    if ( !(pml.groundTrace.surfaceFlags & SURF_NODAMAGE) )
    {
        if ( delta > 60 )
        {
            PM_AddEvent( EV_FALL_FAR );
        }
        else if ( delta > 40 )
        {
            // this is a pain grunt, so don't play it if dead
            if ( pm->ps->stats[STAT_HEALTH] > 0 )
            {
                PM_AddEvent( EV_FALL_MEDIUM );
            }
        }
        else if ( delta > 7 )
        {
            PM_AddEvent( EV_FALL_SHORT );
        }
        else
        {
            PM_AddEvent( PM_FootstepForSurface() );
        }
    }

    // start footstep cycle over
    pm->ps->bobCycle = 0;
}

/*
=============
PM_CheckStuck
=============
*/
/*
void PM_CheckStuck(void) {
	trace_t trace;

	pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, pm->ps->origin, pm->ps->clientNum, pm->tracemask);
	if (trace.allsolid) {
		//int shit = qtrue;
	}
}
*/

/*
=============
PM_CorrectAllSolid
=============
*/
static int PM_CorrectAllSolid( trace_t *trace )
{
    int			i, j, k;
    vec3_t		point;

    if ( pm->debugLevel )
    {
        Com_Printf( "%i:allsolid\n", c_pmove);
    }

    // jitter around
    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            for (k = -1; k <= 1; k++)
            {
                VectorCopy(pm->ps->origin, point);
                point[0] += (float) i;
                point[1] += (float) j;
                point[2] += (float) k;
                pm->trace (trace, point, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
                if ( !trace->allsolid )
                {
                    point[0] = pm->ps->origin[0];
                    point[1] = pm->ps->origin[1];
                    point[2] = pm->ps->origin[2] - 0.25;

                    pm->trace (trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
                    pml.groundTrace = *trace;
                    return qtrue;
                }
            }
        }
    }

    pm->ps->groundEntityNum = ENTITYNUM_NONE;
    pml.groundPlane = qfalse;
    pml.walking = qfalse;

    return qfalse;
}


/*
=============
PM_GroundTraceMissed

The ground trace didn't hit a surface, so we are in freefall
=============
*/
static void PM_GroundTraceMissed( void )
{
    trace_t		trace;
    vec3_t		point;
    qboolean	not_charging = ( pm->ps->weaponstate != WEAPON_CHARGING ) && ( pm->ps->weaponstate != WEAPON_OHCHARGING );

    if ( pm->ps->groundEntityNum != ENTITYNUM_NONE )
    {
        // we just transitioned into freefall
        if ( pm->debugLevel )
        {
            Com_Printf( "%i:lift\n", c_pmove);
        }

        // if they aren't in a jumping animation and the ground is a ways away, force into it
        // if we didn't do the trace, the player would be backflipping down staircases
        VectorCopy( pm->ps->origin, point );
        point[2] -= 64;

        pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, pm->tracemask);
        if ( trace.fraction == 1.0 )
        {
            if ( pm->cmd.forwardmove >= 0 )
            {
                if( not_charging )
                {
                    PM_StartHeadAnim( HEAD_JUMP );
                    PM_StartTorsoAnim( TORSO_JUMP );
                }
                PM_ForceLegsAnim( LEGS_JUMP );
                pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
            }
            else
            {
                if( not_charging )
                {
                    PM_StartHeadAnim( HEAD_JUMPB );
                    PM_StartTorsoAnim( TORSO_JUMPB );
                }
                PM_ForceLegsAnim( LEGS_JUMPB );
                pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
            }
        }
    }

    pm->ps->groundEntityNum = ENTITYNUM_NONE;
    pml.groundPlane = qfalse;
    pml.walking = qfalse;
}


/*
===================
PM_ClimbMove

===================
*/
static void PM_ClimbMove( void )
{
    int     i;
    vec3_t    wishvel;
    float   fmove, smove;
    vec3_t    wishdir;
    float   wishspeed;
    float   scale;
    usercmd_t cmd;
    float   accelerate;
    float   vel;

    if ( pm->waterlevel > 2 && DotProduct( pml.forward, pml.groundTrace.plane.normal ) > 0 )
    {
        // begin swimming
        //PM_WaterMove();
        return;
    }


    if ( PM_CheckJump( ) )
    {
        // jumped away
        if ( pm->waterlevel > 1 )
        {
            //PM_WaterMove();
        }
        else
        {
            PM_AirMove();
        }
        return;
    }

    PM_Friction ();

    fmove = pm->cmd.forwardmove;
    smove = pm->cmd.rightmove;

    cmd = pm->cmd;
    scale = PM_CmdScale( &cmd );

    // set the movementDir so clients can rotate the legs for strafing
    PM_SetMovementDir();

    // project the forward and right directions onto the ground plane
    PM_ClipVelocity (pml.forward, pml.groundTrace.plane.normal, pml.forward, OVERCLIP );
    PM_ClipVelocity (pml.right, pml.groundTrace.plane.normal, pml.right, OVERCLIP );
    //
    VectorNormalize (pml.forward);
    VectorNormalize (pml.right);

    for ( i = 0 ; i < 3 ; i++ )
    {
        wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
    }
    // when going up or down slopes the wish velocity should Not be zero
//  wishvel[2] = 0;

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);
    wishspeed *= scale;

    // clamp the speed lower if wading or walking on the bottom
    if ( pm->waterlevel )
    {
        float waterScale;

        waterScale = pm->waterlevel / 3.0;
        waterScale = 1.0 - ( 1.0 - pm_swimScale ) * waterScale;
        if ( wishspeed > pm->ps->speed * waterScale )
        {
            wishspeed = pm->ps->speed * waterScale;
        }
    }

    // when a player gets hit, they temporarily lose
    // full control, which allows them to be moved a bit
    if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags &

            PMF_TIME_KNOCKBACK )
    {
        accelerate = pm_airaccelerate;
    }
    else
    {
        accelerate = pm_accelerate;
    }

    PM_Accelerate (wishdir, wishspeed, accelerate);

    if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags &

            PMF_TIME_KNOCKBACK )
    {
        pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
    }

    vel = VectorLength(pm->ps->velocity);

    // slide along the ground plane
    PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal,
                     pm->ps->velocity, OVERCLIP );

    // don't decrease velocity when going up or down a slope
    VectorNormalize(pm->ps->velocity);
    VectorScale(pm->ps->velocity, vel, pm->ps->velocity);

    // don't do anything if standing still
    if (!pm->ps->velocity[0] && !pm->ps->velocity[1] && !pm->ps->velocity[2])
    {
        return;
    }

    PM_StepSlideMove( qfalse );
}




/*
=============
PM_GroundClimbTrace
=============
*/
static void PM_GroundClimbTrace( void )
{
    vec3_t    surfNormal, movedir, lookdir, forward, right, point;
    vec3_t    refNormal = { 0.0f, 0.0f, 1.0f };
    vec3_t    ceilingNormal = { 0.0f, 0.0f, -1.0f };
    float     toAngles[3], surfAngles[3];
    trace_t   trace = { 0, 0, 0 };
    int       i;

    //used for delta correction
    vec3_t    traceCROSSsurf, traceCROSSref, surfCROSSref;
    float     traceDOTsurf, traceDOTref, surfDOTref, rTtDOTrTsTt;
    float     traceANGsurf, traceANGref, surfANGref;
    vec3_t    horizontal = { 1.0f, 0.0f, 0.0f }; //arbituary vector perpendicular to refNormal
    vec3_t    refTOtrace, refTOsurfTOtrace, tempVec;
    int       rTtANGrTsTt;
    float     ldDOTtCs, d;
    vec3_t    abc;

    //TA: If we're on the ceiling then grapplePoint is a rotation normal.. otherwise its a
    // surface normal.
    //    would have been nice if Carmack had left a few random variables in the ps struct for
    // mod makers

    if( pm->ps->pm_flags & PMF_WALLWALKINGCEILING )
        VectorCopy( ceilingNormal, surfNormal );
    else
        VectorCopy( pm->ps->grapplePoint, surfNormal );

    //construct a vector which reflects the direction the player is looking wrt the surface normal
    AngleVectors( pm->ps->viewangles, forward, NULL, NULL );
    CrossProduct( forward, surfNormal, right );
    VectorNormalize( right );
    CrossProduct( surfNormal, right, movedir );
    VectorNormalize( movedir );

    VectorCopy( movedir, lookdir );

    if( pm->cmd.forwardmove < 0 )
        VectorNegate( movedir, movedir );

    //allow strafe transitions
    if( pm->cmd.rightmove )
    {
        VectorCopy( right, movedir );

        if( pm->cmd.rightmove < 0 )
            VectorNegate( movedir, movedir );
    }

    for( i = 0; i <= 4; i++ )
    {
        switch ( i )
        {
        case 0:
            //trace into direction we are moving
            VectorMA( pm->ps->origin, 0.25f, movedir, point );
            pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum,

                       pm->tracemask);
            break;

        case 1:
            //trace straight down anto "ground" surface
            VectorMA( pm->ps->origin, -0.25f, surfNormal, point );
            pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum,

                       pm->tracemask);
            break;

        case 2:
            if( pml.groundPlane != qfalse )
            {
                //step down
                VectorMA( pm->ps->origin, -STEPSIZE, surfNormal, point );
                pm->trace( &trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum,

                           pm->tracemask );
            }
            else
                continue;
            break;

        case 3:
            if( pml.groundPlane != qfalse )
            {
                //trace "underneath" BBOX so we can traverse angles > 180deg
                VectorMA( pm->ps->origin, -25.0f, surfNormal, point );
                VectorMA( point, -25.0f, movedir, point );
                pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum,

                           pm->tracemask);
            }
            else
                continue;
            break;

        case 4:
            //fall back so we don't have to modify PM_GroundTrace too much
            VectorCopy( pm->ps->origin, point );
            point[2] = pm->ps->origin[2] - 0.25f;
            pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum,

                       pm->tracemask);
            break;
        }

        //if we hit something
        if( trace.fraction < 1.0 && !( trace.surfaceFlags & ( SURF_SKY | SURF_SLICK ) ) &&
                !( trace.entityNum != ENTITYNUM_WORLD && i != 4 ) )
        {
            if( i == 2 || i == 3 )
            {
                VectorCopy( trace.endpos, pm->ps->origin );
            }

            //calculate a bunch of stuff...
            CrossProduct( trace.plane.normal, surfNormal, traceCROSSsurf );
            VectorNormalize( traceCROSSsurf );

            CrossProduct( trace.plane.normal, refNormal, traceCROSSref );
            VectorNormalize( traceCROSSref );

            CrossProduct( surfNormal, refNormal, surfCROSSref );
            VectorNormalize( surfCROSSref );

            //calculate angle between surf and trace
            traceDOTsurf = DotProduct( trace.plane.normal, surfNormal );
            traceANGsurf = RAD2DEG( acos( traceDOTsurf ) );

            if( traceANGsurf > 180.0f )
                traceANGsurf -= 180.0f;

            //calculate angle between trace and ref
            traceDOTref = DotProduct( trace.plane.normal, refNormal );
            traceANGref = RAD2DEG( acos( traceDOTref ) );

            if( traceANGref > 180.0f )
                traceANGref -= 180.0f;

            //calculate angle between surf and ref
            surfDOTref = DotProduct( surfNormal, refNormal );
            surfANGref = RAD2DEG( acos( surfDOTref ) );

            if( surfANGref > 180.0f )
                surfANGref -= 180.0f;

            //if the trace result and old surface normal are different then we must have transided
            // to a new
            //surface... do some stuff...
            if( !VectorCompare( trace.plane.normal, surfNormal ) )
            {
                //if the trace result or the old vector is not the floor or ceiling correct the YAW angle
                if( !VectorCompare( trace.plane.normal, refNormal ) && !VectorCompare( surfNormal, refNormal ) &&
                        !VectorCompare( trace.plane.normal, ceilingNormal ) && !VectorCompare(

                            surfNormal, ceilingNormal ) )
                {
                    //behold the evil mindfuck from hell
                    //it has fucked mind like nothing has fucked mind before

                    //calculate reference rotated through to trace plane
                    RotatePointAroundVector( refTOtrace, traceCROSSref, horizontal, -traceANGref );

                    //calculate reference rotated through to surf plane then to trace plane
                    RotatePointAroundVector( tempVec, surfCROSSref, horizontal, -surfANGref );
                    RotatePointAroundVector( refTOsurfTOtrace, traceCROSSsurf, tempVec, -traceANGsurf );

                    //calculate angle between refTOtrace and refTOsurfTOtrace
                    rTtDOTrTsTt = DotProduct( refTOtrace, refTOsurfTOtrace );
                    rTtANGrTsTt = ANGLE2SHORT( RAD2DEG( acos( rTtDOTrTsTt ) ) );

                    if( rTtANGrTsTt > 32768 )
                        rTtANGrTsTt -= 32768;

                    //set the correction angle
                    if( traceCROSSsurf[ 2 ] < 0 )
                        rTtANGrTsTt = -rTtANGrTsTt;

                    //phew! - correct the angle
                    pm->ps->delta_angles[ YAW ] -= rTtANGrTsTt;
                }

                //construct a plane dividing the surf and trace normals
                CrossProduct( traceCROSSsurf, surfNormal, abc );
                VectorNormalize( abc );
                d = DotProduct( abc, pm->ps->origin );

                //construct a point representing where the player is looking
                VectorAdd( pm->ps->origin, lookdir, point );

                //check whether point is on one side of the plane, if so invert the correction angle
                if( ( abc[ 0 ] * point[ 0 ] + abc[ 1 ] * point[ 1 ] + abc[ 2 ] * point[ 2 ] - d ) >

                        0 )
                    traceANGsurf = -traceANGsurf;

                //find the . product of the lookdir and traceCROSSsurf
                if( ( ldDOTtCs = DotProduct( lookdir, traceCROSSsurf ) ) < 0.0f )
                {
                    VectorInverse( traceCROSSsurf );
                    ldDOTtCs = DotProduct( lookdir, traceCROSSsurf );
                }

                //set the correction angle
                traceANGsurf *= 1.0f - ldDOTtCs;

                //correct the angle
                pm->ps->delta_angles[ PITCH ] -= ANGLE2SHORT( traceANGsurf );

                //transition from wall to ceiling
                //normal for subsequent viewangle rotations
                if( VectorCompare( trace.plane.normal, ceilingNormal ) )
                {
                    CrossProduct( surfNormal, trace.plane.normal, pm->ps->grapplePoint );
                    VectorNormalize( pm->ps->grapplePoint );
                    pm->ps->pm_flags |= PMF_WALLWALKINGCEILING;
                }

                //transition from ceiling to wall
                //we need to do some different angle correction here cos GPISROTVEC
                if( VectorCompare( surfNormal, ceilingNormal ) )
                {
                    vectoangles( trace.plane.normal, toAngles );
                    vectoangles( pm->ps->grapplePoint, surfAngles );

                    pm->ps->delta_angles[1] -= ANGLE2SHORT( ( ( surfAngles[1] - toAngles[1] ) * 2 ) -

                                                            180 );
                }
            }

            pml.groundTrace = trace;

            //so everything knows where we're wallclimbing (ie client side)
            //pm->ps->legsAnim |= ANIM_WALLWALKING;

            //if we're not stuck to the ceiling then set grapplePoint to be a surface normal
            if( !VectorCompare( trace.plane.normal, ceilingNormal ) )
            {
                //so we know what surface we're stuck to
                VectorCopy( trace.plane.normal, pm->ps->grapplePoint );
                pm->ps->pm_flags &= ~PMF_WALLWALKINGCEILING;
            }

            //IMPORTANT: break out of the for loop if we've hit something
            break;
        }
        else if ( trace.allsolid )
        {
            // do something corrective if the trace starts in a solid...
            //TA: fuck knows what this does with all my new stuff :(
            if ( !PM_CorrectAllSolid(&trace) )
                return;
        }
    }

    if ( trace.fraction >= 1.0 )
    {
        // if the trace didn't hit anything, we are in free fall
        PM_GroundTraceMissed();
        pml.groundPlane = qfalse;
        pml.walking = qfalse;
        //pm->ps->legsAnim &= ~ANIM_WALLWALKING;

        pm->ps->pm_flags &= ~PMF_WALLWALKINGCEILING;

        //we get very bizarre effects if we don't do this :0
        VectorCopy( refNormal, pm->ps->grapplePoint );
        return;
    }

    pml.groundPlane = qtrue;
    pml.walking = qtrue;

    // hitting solid ground will end a waterjump
    if (pm->ps->pm_flags & PMF_TIME_WATERJUMP)
    {
        pm->ps->pm_flags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND);
        pm->ps->pm_time = 0;
    }

    pm->ps->groundEntityNum = trace.entityNum;

    // don't reset the z velocity for slopes
//  pm->ps->velocity[2] = 0;

    PM_AddTouchEnt( trace.entityNum );
}


/*
=============
PM_GroundTrace
=============
*/
static void PM_GroundTrace( void )
{
    vec3_t    	refNormal = { 0.0f, 0.0f, 1.0f };
    vec3_t		point;
    trace_t		trace;
    qboolean	not_charging = ( pm->ps->weaponstate != WEAPON_CHARGING ) && ( pm->ps->weaponstate != WEAPON_OHCHARGING );


    //toggle wall climbing if holding the wallhang button
    if ( pm->cmd.buttons & BUTTON_WALLHANG )
        pm->ps->pm_flags |= PMF_WALLWALKING;
    else
        pm->ps->pm_flags &= ~PMF_WALLWALKING;

    if( pm->ps->pm_type == PM_DEAD )
        pm->ps->pm_flags &= ~PMF_WALLWALKING;

    if( pm->ps->pm_flags & PMF_WALLWALKING )
    {
        PM_GroundClimbTrace( );
        return;
    }

    pm->ps->pm_flags &= ~PMF_WALLWALKING;
    //pm->ps->legsAnim &= ~ANIM_WALLWALKING;

    //make sure that the surfNormal is reset to the ground
    VectorCopy( refNormal, pm->ps->grapplePoint );

    point[0] = pm->ps->origin[0];
    point[1] = pm->ps->origin[1];
    point[2] = pm->ps->origin[2] - 0.25;

    pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, point, pm->ps->clientNum, ( pm->tracemask & ~(CONTENTS_WATER) ));
    pml.groundTrace = trace;

    // do something corrective if the trace starts in a solid...
    if ( trace.allsolid )
    {
        if ( !PM_CorrectAllSolid(&trace) )
            return;
    }

    // if the trace didn't hit anything, we are in free fall
    if ( trace.fraction == 1.0 )
    {
        PM_GroundTraceMissed();
        pml.groundPlane = qfalse;
        pml.walking = qfalse;
        return;
    }

    // check if getting thrown off the ground
    if ( pm->ps->velocity[2] > 0 && DotProduct( pm->ps->velocity, trace.plane.normal ) > 10 )
    {
        if ( pm->debugLevel )
        {
            Com_Printf( "%i:kickoff\n", c_pmove);
        }
        // go into fall animation
        if ( pm->cmd.forwardmove >= 0 )
        {
            if( not_charging && !(pm->ps->pm_flags & PMF_WALL_HANG) )
            {
                PM_StartHeadAnim( HEAD_FALL );
                PM_StartTorsoAnim( TORSO_FALL );
                PM_ForceLegsAnim( LEGS_FALL );
                pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
            }
        }
        else
        {
            if( not_charging && !(pm->ps->pm_flags & PMF_WALL_HANG) )
            {
                PM_StartHeadAnim( HEAD_FALL );
                PM_StartTorsoAnim( TORSO_FALL );
                PM_ForceLegsAnim( LEGS_FALL );
                pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
            }
        }

        pm->ps->groundEntityNum = ENTITYNUM_NONE;
        pml.groundPlane = qfalse;
        pml.walking = qfalse;
        return;
    }

    // slopes that are too steep will not be considered onground
    if ( trace.plane.normal[2] < MIN_WALK_NORMAL )
    {
        if ( pm->debugLevel )
        {
            Com_Printf( "%i:steep\n", c_pmove);
        }
        // FIXME: if they can't slide down the slope, let them
        // walk (sharp crevices)
        pm->ps->groundEntityNum = ENTITYNUM_NONE;
        pml.groundPlane = qtrue;
        pml.walking = qfalse;
        return;
    }

    pml.groundPlane = qtrue;
    pml.walking = qtrue;

    // hitting solid ground will end a waterjump
    if (pm->ps->pm_flags & PMF_TIME_WATERJUMP)
    {
        pm->ps->pm_flags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND);
        pm->ps->pm_time = 0;
    }

    if ( pm->ps->groundEntityNum == ENTITYNUM_NONE )
    {
        // just hit the ground
        if ( pm->debugLevel )
        {
            Com_Printf( "%i:Land\n", c_pmove);
        }
        // reset some things upon landing
        pm->ps->pm_flags &= ~PMF_AIR_JUMP1;
        pm->ps->pm_flags &= ~PMF_WALL_HANG;
        pm->ps->pm_flags |= PMF_CAN_AIRDASH;
        pm->ps->pm_flags &= ~PMF_AIRDASHING;	//reset air dashing flag
        if( pm->ps->weaponstate >= WEAPON_TECHUPCUT )
            pm->ps->weaponstate = WEAPON_READY;

        PM_CrashLand();

        // don't do landing time if we were just going down a slope
        if ( pml.previous_velocity[2] < -200 )
        {
            // don't allow another jump for a little while
            pm->ps->pm_flags |= PMF_TIME_LAND;
            pm->ps->pm_time = 250;
        }
    }

    pm->ps->groundEntityNum = trace.entityNum;

    // don't reset the z velocity for slopes
//	pm->ps->velocity[2] = 0;

    PM_AddTouchEnt( trace.entityNum );
}


/*
=============
PM_SetWaterLevel	FIXME: avoid this twice?  certainly if not moving
=============
*/
static void PM_SetWaterLevel( void )
{
    vec3_t		point;
    int			cont;
    int			sample1;
    int			sample2;

    //
    // get waterlevel, accounting for ducking
    //
    pm->waterlevel = 0;
    pm->watertype = 0;

    point[0] = pm->ps->origin[0];
    point[1] = pm->ps->origin[1];
    point[2] = pm->ps->origin[2] + MINS_Z + 1;
    cont = pm->pointcontents( point, pm->ps->clientNum );

    if ( cont & MASK_WATER )
    {
        sample2 = pm->ps->viewheight - MINS_Z;
        sample1 = sample2 / 2;

        pm->watertype = cont;
        pm->waterlevel = 1;
        point[2] = pm->ps->origin[2] + MINS_Z + sample1;
        cont = pm->pointcontents (point, pm->ps->clientNum );
        if ( cont & MASK_WATER )
        {
            pm->waterlevel = 2;
            point[2] = pm->ps->origin[2] + MINS_Z + sample2;
            cont = pm->pointcontents (point, pm->ps->clientNum );
            if ( cont & MASK_WATER )
            {
                pm->waterlevel = 3;
            }
        }
    }

}

/*
==============
PM_CheckDuck

Sets mins, maxs, and pm->ps->viewheight
==============
*/
static void PM_CheckDuck (void)
{
    pm->mins[0] = -15;
    pm->mins[1] = -15;

    pm->maxs[0] = 15;
    pm->maxs[1] = 15;

    pm->mins[2] = MINS_Z;

    if (pm->ps->pm_type == PM_DEAD)
    {
        pm->maxs[2] = -8;
        pm->ps->viewheight = DEAD_VIEWHEIGHT;
        return;
    }

    pm->maxs[2] = 32;
    pm->ps->viewheight = DEFAULT_VIEWHEIGHT;
}



//===================================================================


/*
===============
PM_Footsteps
===============
*/
static void PM_Footsteps( void )
{
    float		bobmove;
    int			old;
    qboolean	footstep;

    //
    // calculate speed and cycle to be used for
    // all cyclic walking effects
    //
    pm->xyspeed = sqrt( pm->ps->velocity[0] * pm->ps->velocity[0]
                        +  pm->ps->velocity[1] * pm->ps->velocity[1] );
// dashing shouldnt have footsteps
    if( pm->ps->powerups[PW_DASH] )
    {
        return;
    }
    if ( pm->ps->groundEntityNum == ENTITYNUM_NONE )
    {
        return;
    }

    // if not trying to move
    if ( !pm->cmd.forwardmove && !pm->cmd.rightmove )
    {
        if (  pm->xyspeed < 5 )
        {
            pm->ps->bobCycle = 0;	// start at beginning of cycle again

            if ( pm->ps->persistant[PLAYERCLASS] == CLASS_DRN000 &&
                    pm->cmd.buttons & BUTTON_SHIELD )
            {
                PM_ContinueHeadAnim( HEAD_IDLE );
                PM_ContinueTorsoAnim( TORSO_SHEILD_IDLE );
                PM_ContinueLegsAnim( LEGS_IDLE );
            }
            else if( pm->ps->stats[STAT_HEALTH] > 40 )
            {
                PM_ContinueHeadAnim( HEAD_IDLE );
                PM_ContinueTorsoAnim( TORSO_STAND );
                PM_ContinueLegsAnim( LEGS_IDLE );
            }
            else
            {
                PM_ContinueHeadAnim( HEAD_IDLEHURT );
                PM_ContinueTorsoAnim( TORSO_IDLEHURT );
                PM_ContinueLegsAnim( LEGS_IDLEHURT );
            }
        }
        return;
    }

    footstep = qfalse;

    if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN )
    {
        if ( !( pm->cmd.buttons & BUTTON_WALKING ) )
        {
            bobmove = 0.4f;	// faster speeds bob faster
            PM_ContinueHeadAnim( HEAD_BACK );
            PM_ContinueTorsoAnim( TORSO_BACK );
            PM_ContinueLegsAnim( LEGS_BACK );
            footstep = qtrue;
        }
        else
        {
            bobmove = 0.3f;
            PM_ContinueLegsAnim( LEGS_BACKWALK );
        }


    }
    else
    {
        if ( !( pm->cmd.buttons & BUTTON_WALKING ) )
        {
            bobmove = 0.4f;	// faster speeds bob faster
            PM_ContinueHeadAnim( HEAD_RUN );
            PM_ContinueTorsoAnim( TORSO_RUN );
            PM_ContinueLegsAnim( LEGS_RUN );
            footstep = qtrue;
        }
        else
        {
            bobmove = 0.3f;	// walking bobs slow
            PM_ContinueHeadAnim( HEAD_WALK );
            PM_ContinueTorsoAnim( TORSO_WALK );
            PM_ContinueLegsAnim( LEGS_WALK );
        }
    }

    // check for footstep / splash sounds
    old = pm->ps->bobCycle;
    pm->ps->bobCycle = (int)( old + bobmove * pml.msec ) & 255;

    // if we just crossed a cycle boundary, play an apropriate footstep event
    if ( ( ( old + 64 ) ^ ( pm->ps->bobCycle + 64 ) ) & 128 )
    {
        if ( pm->waterlevel == 0 )
        {
            // on ground will only play sounds if running
            if ( footstep && !pm->noFootsteps )
            {
                PM_AddEvent( PM_FootstepForSurface() );
            }
        }
        else if ( pm->waterlevel == 1 )
        {
            // splashing
            PM_AddEvent( EV_FOOTSPLASH );
        }
        else if ( pm->waterlevel == 2 )
        {
            // wading / swimming at surface
            if ( footstep && !pm->noFootsteps )
            {
                PM_AddEvent( PM_FootstepForSurface() );
            }
        }
        else if ( pm->waterlevel == 3 )
        {
            if ( footstep && !pm->noFootsteps )
            {
                PM_AddEvent( PM_FootstepForSurface() );
            }
            // no sound when completely underwater

        }
    }
}

/*
==============
PM_WaterEvents

Generate sound events for entering and leaving water
==============
*/
static void PM_WaterEvents( void )  		// FIXME?
{
    //
    // if just entered a water volume, play a sound
    //
    if (!pml.previous_waterlevel && pm->waterlevel)
    {
        PM_AddEvent( EV_WATER_TOUCH );
    }

    //
    // if just completely exited a water volume, play a sound
    //
    if (pml.previous_waterlevel && !pm->waterlevel)
    {
        PM_AddEvent( EV_WATER_LEAVE );
    }

    //
    // check for head just going under water
    //
    if (pml.previous_waterlevel != 3 && pm->waterlevel == 3)
    {
        PM_AddEvent( EV_WATER_UNDER );
    }

    //
    // check for head just coming out of water
    //
    if (pml.previous_waterlevel == 3 && pm->waterlevel != 3)
    {
        PM_AddEvent( EV_WATER_CLEAR );
    }
}


/*
===============
PM_BeginWeaponChange
===============
*/
static void PM_BeginWeaponChange( int weapon )
{
    if ( weapon <= WP_NONE || weapon >= WP_NUM_WEAPONS )
    {
        return;
    }

    if ( !( pm->ps->stats[STAT_WEAPONS] & ( 1 << weapon ) ) )
    {
        return;
    }

    if ( pm->ps->weaponstate == WEAPON_DROPPING )
    {
        return;
    }

    PM_AddEvent( EV_CHANGE_WEAPON );
    pm->ps->weaponstate = WEAPON_DROPPING;
    pm->ps->weaponTime += 200;
    PM_StartHeadAnim( HEAD_DROP );
    PM_StartTorsoAnim( TORSO_DROP );
}


/*
===============
PM_FinishWeaponChange
===============
*/
static void PM_FinishWeaponChange( void )
{
    int		weapon;

    weapon = pm->cmd.weapon;
    if ( weapon < WP_NONE || weapon >= WP_NUM_WEAPONS )
    {
        weapon = WP_NONE;
    }

    if ( !( pm->ps->stats[STAT_WEAPONS] & ( 1 << weapon ) ) )
    {
        weapon = WP_NONE;
    }

    pm->ps->weapon = weapon;
    pm->ps->weaponstate = WEAPON_RAISING;
    pm->ps->weaponTime += 250;
    PM_StartHeadAnim( HEAD_RAISE );
    PM_StartTorsoAnim( TORSO_RAISE );
}


/*
==============
PM_TorsoAnimation

==============
*/
static void PM_TorsoAnimation( void )
{
    qboolean	not_charging = ( pm->ps->weaponstate != WEAPON_CHARGING ) && ( pm->ps->weaponstate != WEAPON_OHCHARGING );
// Peter FIXME: fix this animation
    //if youre supposed to be charging, your weapon is almost ready again, and no higher priority animations are running...
    if( !not_charging && (pm->ps->weaponTime < 50) && !pm->ps->torsoTimer )
    {
        if( pm->ps->weapon == WP_DEFAULT )  	//if its a default weapon that can be charged
        {
            if( (pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) != TORSO_CHARGE )
            {
                PM_StartHeadAnim( HEAD_CHARGE );
                PM_StartTorsoAnim( TORSO_CHARGE );
            }
            else
            {
                PM_ContinueHeadAnim( HEAD_CHARGE );
                PM_ContinueTorsoAnim( TORSO_CHARGE );
            }
        }
        else     //if its a secondary weapon
        {
            if( (pm->ps->torsoAnim & ~ANIM_TOGGLEBIT) != TORSO_CHARGE2 )
            {
                PM_StartHeadAnim( HEAD_CHARGE2 );
                PM_StartTorsoAnim( TORSO_CHARGE2 );
            }
            else
            {
                PM_ContinueHeadAnim( HEAD_CHARGE2 );
                PM_ContinueTorsoAnim( TORSO_CHARGE2 );
            }
        }
        pm->ps->torsoTimer = 100;
    }
    if( ( pm->ps->legsAnim & ~ANIM_TOGGLEBIT ) == LEGS_FALL )
    {
        if( not_charging )
        {
            PM_ContinueHeadAnim( HEAD_FALL );
            PM_ContinueTorsoAnim( TORSO_FALL );
        }
        return;
    }
}


/*
==============
PM_Weapon

Generates weapon events and modifes the weapon counter
==============
*/
static void PM_Weapon( void )
{
    const int	torsoAnim = ( pm->ps->torsoAnim & ~ANIM_TOGGLEBIT );
    const int	legsAnim = ( pm->ps->legsAnim & ~ANIM_TOGGLEBIT );
    const		weaponinfo2_t *weapon = &bg_playerlist[pm->ps->persistant[PLAYERCLASS]].weapons[pm->ps->weapon];
    const int	pclass = pm->ps->persistant[PLAYERCLASS];
    const int	upgrades = pm->ps->persistant[PERS_UPGRADES];

    // don't allow attack until all buttons are up
    if ( pm->ps->pm_flags & PMF_RESPAWNED )
    {
        return;
    }

    // ignore if spectator
    if ( pm->ps->persistant[PERS_TEAM] == TEAM_SPECTATOR )
    {
        return;
    }

    // check for dead player
    if ( pm->ps->stats[STAT_HEALTH] <= 0 )
    {
        pm->ps->weapon = WP_NONE;
        return;
    }

    // check for item using
    if ( pm->cmd.buttons & BUTTON_USE_HOLDABLE )
    {
        if ( ! ( pm->ps->pm_flags & PMF_USE_ITEM_HELD ) )
        {
            if ( bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag == HI_SUBTANK
                    && pm->ps->stats[STAT_HEALTH] >= pm->ps->stats[STAT_MAX_HEALTH] )
            {
                // don't use subtank if at max health
            }
            else
            {
                pm->ps->pm_flags |= PMF_USE_ITEM_HELD;
                PM_AddEvent( EV_USE_ITEM0 + bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag );
                if (bg_itemlist[pm->ps->stats[STAT_HOLDABLE_ITEM]].giTag != HI_SUBTANK)
                    pm->ps->stats[STAT_HOLDABLE_ITEM] = 0;
            }
            return;
        }
    }
    else
    {
        pm->ps->pm_flags &= ~PMF_USE_ITEM_HELD;
    }

    // make weapon function
    if ( pm->ps->weaponTime > 0 )
    {
        pm->ps->weaponTime -= pml.msec;
    }

    if ( !(pm->cmd.buttons & BUTTON_ATTACK) && ( pm->ps->weaponTime > 0 ) && ( pm->ps->weaponstate != WEAPON_CHARGING ) )
    {
        pm->ps->weaponstate = WEAPON_FIRING;
    }

    // Z. Buster!
    if( pclass == CLASS_DWNINFINITY )
    {
        if ( !(pm->cmd.buttons & BUTTON_OFFHAND1) && ( pm->ps->weaponTime > 0 ) && ( pm->ps->weaponstate == WEAPON_OHCHARGING ) )
        {
            pm->ps->weaponstate = WEAPON_OHFIRING;
        }
    }

    // stops weapon from adding a thousand events in a second
    if ( pm->ps->weaponTime > 0 )
    {
        return;
    }

    // check for techniques
    if( pm->ps->weaponstate == WEAPON_TECHUPCUT )
    {
        if( torsoAnim == BOTH_CHARGED_UPPER )
        {
            PM_ContinueHeadAnim( BOTH_CHARGED_UPPER );
            PM_ContinueTorsoAnim( BOTH_CHARGED_UPPER );
        }
        else
        {
            PM_StartHeadAnim( BOTH_CHARGED_UPPER );
            PM_StartTorsoAnim( BOTH_CHARGED_UPPER );
        }
        if( legsAnim == BOTH_CHARGED_UPPER )
            PM_ContinueLegsAnim( BOTH_CHARGED_UPPER );
        else
            PM_StartLegsAnim( BOTH_CHARGED_UPPER );

        pm->ps->torsoTimer = 800;
        pm->ps->legsTimer = 800;
        pm->ps->weaponTime += 80;
        PM_AddEvent( EV_FIRE_WEAPON );
        return;
    }
    else if( pm->ps->weaponstate == WEAPON_TECHSPIKE )
    {
        if( torsoAnim == BOTH_CHARGED_AIR_DROP )
        {
            PM_ContinueHeadAnim( BOTH_CHARGED_AIR_DROP );
            PM_ContinueTorsoAnim( BOTH_CHARGED_AIR_DROP );
        }
        else
        {
            PM_StartHeadAnim( BOTH_CHARGED_AIR_DROP );
            PM_StartTorsoAnim( BOTH_CHARGED_AIR_DROP );
        }
        if( legsAnim == BOTH_CHARGED_AIR_DROP )
            PM_ContinueLegsAnim( BOTH_CHARGED_AIR_DROP );
        else
            PM_StartLegsAnim( BOTH_CHARGED_AIR_DROP );

        pm->ps->torsoTimer = 500;
        pm->ps->legsTimer = 500;
        pm->ps->weaponTime += 50;
        PM_AddEvent( EV_FIRE_WEAPON );
        return;
    }
    else if( pm->ps->weaponstate == WEAPON_TECHSPIN )
    {
        if( torsoAnim == BOTH_CHARGED_SPIN_AIR )
        {
            PM_ContinueHeadAnim( BOTH_CHARGED_SPIN_AIR );
            PM_ContinueTorsoAnim( BOTH_CHARGED_SPIN_AIR );
        }
        else
        {
            PM_StartHeadAnim( BOTH_CHARGED_SPIN_AIR );
            PM_StartTorsoAnim( BOTH_CHARGED_SPIN_AIR );
        }
        if( legsAnim == BOTH_CHARGED_SPIN_AIR )
            PM_ContinueLegsAnim( BOTH_CHARGED_SPIN_AIR );
        else
            PM_StartLegsAnim( BOTH_CHARGED_SPIN_AIR );

        pm->ps->torsoTimer = 800; // 500
        pm->ps->legsTimer = 800;
        pm->ps->weaponTime += 80;
        PM_AddEvent( EV_FIRE_WEAPON );
        return;
    }

    // check for weapon change
    // can't change if weapon is firing, but can change
    // again if lowering or raising
    if ( pm->ps->weaponTime <= 0 || (pm->ps->weaponstate != WEAPON_FIRING) )
    {
        if ( pm->ps->weapon != pm->cmd.weapon )
        {
            PM_BeginWeaponChange( pm->cmd.weapon );
        }
    }


    // change weapon if time
    if ( pm->ps->weaponstate == WEAPON_DROPPING )
    {
        PM_FinishWeaponChange();
        return;
    }

    if ( pm->ps->weaponstate == WEAPON_RAISING )
    {
        pm->ps->weaponstate = WEAPON_READY;
        return;
    }

    // check for fire, charging and techniques
    if ( !(pm->cmd.buttons & BUTTON_ATTACK) && (pm->ps->weaponstate != WEAPON_CHARGING)
            && !(pm->cmd.buttons & BUTTON_OFFHAND1) && (pm->ps->weaponstate != WEAPON_OHCHARGING) )
    {
        pm->ps->weaponTime = 0;
        pm->ps->weaponstate = WEAPON_READY;
        if(torsoAnim == TORSO_ATTACK) // (pm->ps->weaponstate = WEAPON_FIRING)
        {
            PM_StartHeadAnim( HEAD_ATTACK_FIN );
            PM_StartTorsoAnim( TORSO_ATTACK_FIN );
        }
        return;
    }
    //if they didn't increase their charge level, and they let it go, don't fire
    if(!(pm->cmd.buttons & BUTTON_ATTACK) && ( pm->ps->weaponstate == WEAPON_CHARGING ) && ( !pm->ps->stats[STAT_CHARGE] ) )
    {
        pm->ps->weaponTime = 0;
        pm->ps->weaponstate = WEAPON_READY;
        return;
    }
    //same thing, for Z. Buster
    if( pclass == CLASS_DWNINFINITY )
    {
        if(!(pm->cmd.buttons & BUTTON_OFFHAND1) && ( pm->ps->weaponstate == WEAPON_OHCHARGING ) && ( !pm->ps->stats[STAT_CHARGE] ) )
        {
            pm->ps->weaponTime = 0;
            pm->ps->weaponstate = WEAPON_READY;
            return;
        }
    }
    //if you're using the fire cutter, and youre holding the attack button, and your charge level is maxed out, dont fire anything
    if( (weapon->weapon == WP_FCUTTER) && (pm->cmd.buttons & BUTTON_ATTACK) && (pm->ps->stats[STAT_CHARGE] > 2 ) )
    {
        return;
    }
    //if you're using the beam laser, and youre holding the attack button, and your charge level is maxed out, dont fire anything
    if( (weapon->weapon == WP_BLASER) && (pm->cmd.buttons & BUTTON_ATTACK) && (pm->ps->stats[STAT_CHARGE] > 2 ) )
    {
        return;
    }

    // if they are charging, keep it going! (unless they are firing the uncharged flame cutter or beam laser)
    if( ( pm->cmd.buttons & BUTTON_ATTACK ) && ( pm->ps->weaponstate == WEAPON_CHARGING ) && (weapon->weapon != WP_FCUTTER) && (weapon->weapon != WP_BLASER) )
    {
        return;
    }
    // for charging offhand weapons, keep it going!
    if( pclass == CLASS_DWNINFINITY )
    {
        if( ( pm->cmd.buttons & BUTTON_OFFHAND1 ) && ( pm->ps->weaponstate == WEAPON_OHCHARGING ) )
            return;
    }


    // check to see what the player is doing, to determine what attack to use (test for melee class later)
    if ( !pml.groundPlane && (pclass == CLASS_DWNINFINITY) ) //if in midair
    {
        if( pm->cmd.upmove < 0 )
        {
            if( pm->ps->velocity[2] < 0 )
            {
                pm->ps->weaponstate = WEAPON_TECHSPIKE;
                pm->ps->weaponTime += 50;
                return;
            }
        }
        else if( pm->ps->stats[STAT_CHARGE] && !(pm->cmd.buttons & BUTTON_ATTACK) )
        {
            pm->ps->weaponstate = WEAPON_TECHSPIN;
            pm->ps->weaponTime += 50;
            return;
        }
    }
    else if ( pclass == CLASS_DWNINFINITY )	//on the ground
    {
        if( pm->cmd.upmove < 0 )
        {
            pm->ps->weaponstate = WEAPON_TECHUPCUT;
            pm->ps->weaponTime += 50;
            return;
        }
    }
    //wall attack animations
    if( legsAnim == LEGS_WALL )
    {
        if( torsoAnim == TORSO_ATTACK_WALL )
        {
            PM_ContinueHeadAnim( HEAD_WALL );
            PM_ContinueTorsoAnim( TORSO_ATTACK_WALL );
        }
        else
        {
            PM_StartHeadAnim( HEAD_WALL );
            PM_StartTorsoAnim( TORSO_ATTACK_WALL );
        }
    }
    //dash attack animations
    else if( legsAnim == LEGS_DASH )
    {
        if ( pm->ps->persistant[PLAYERCLASS] == CLASS_DRN000 &&
                pm->cmd.buttons & BUTTON_SHIELD )
        {
            if( torsoAnim == TORSO_ATTACK2_DASH )
            {
                PM_ContinueHeadAnim( HEAD_ATTACK_DASH );
                PM_ContinueTorsoAnim( TORSO_ATTACK2_DASH );
            }
            else
            {
                PM_StartHeadAnim( HEAD_ATTACK_DASH );
                PM_StartTorsoAnim( TORSO_ATTACK2_DASH );
            }
        }
        else
        {
            if( torsoAnim == TORSO_ATTACK_DASH )
            {
                PM_ContinueHeadAnim( HEAD_ATTACK_DASH );
                PM_ContinueTorsoAnim( TORSO_ATTACK_DASH );
            }
            else
            {
                PM_StartHeadAnim( HEAD_ATTACK_DASH );
                PM_StartTorsoAnim( TORSO_ATTACK_DASH );
            }
        }
    }
    else if( legsAnim == LEGS_DASH_B )
    {
        if ( pm->ps->persistant[PLAYERCLASS] == CLASS_DRN000 &&
                pm->cmd.buttons & BUTTON_SHIELD )
        {
            if( torsoAnim == TORSO_ATTACK2_DASH )
            {
                PM_ContinueHeadAnim( HEAD_ATTACK_DASH );
                PM_ContinueTorsoAnim( TORSO_DASH_B );
            }
            else
            {
                PM_StartHeadAnim( HEAD_ATTACK_DASH );
                PM_StartTorsoAnim( TORSO_DASH_B );
            }
        }
        else
        {
            if( torsoAnim == TORSO_ATTACK_DASH )
            {
                PM_ContinueHeadAnim( HEAD_ATTACK_DASH );
                PM_ContinueTorsoAnim( TORSO_DASH_B );
            }
            else
            {
                PM_StartHeadAnim( HEAD_ATTACK_DASH );
                PM_StartTorsoAnim( TORSO_DASH_B );
            }
        }
    }
    //running attack animations
    else if( (legsAnim == LEGS_RUN ) || (legsAnim == LEGS_BACK ) )	//if running, play the running attack animation
    {
        if ( pm->ps->persistant[PLAYERCLASS] == CLASS_DRN000 &&
                pm->cmd.buttons & BUTTON_SHIELD )
        {
            if( torsoAnim == TORSO_ATTACK2_RUN )
            {
                PM_ContinueHeadAnim( HEAD_ATTACK_RUN );
                PM_ContinueTorsoAnim( TORSO_ATTACK2_RUN );
            }
            else
            {
                PM_StartHeadAnim( HEAD_ATTACK_RUN );
                PM_StartTorsoAnim( TORSO_ATTACK2_RUN );
            }
        }
        else
        {
            if( torsoAnim == TORSO_ATTACK_RUN )
            {
                PM_ContinueHeadAnim( HEAD_ATTACK_RUN );
                PM_ContinueTorsoAnim( TORSO_ATTACK_RUN );
            }
            else
            {
                PM_StartHeadAnim( HEAD_ATTACK_RUN );
                PM_StartTorsoAnim( TORSO_ATTACK_RUN );
            }
        }
    }
    //not running or dashing attack animations // regular attack
    else
    {
        if( torsoAnim == TORSO_ATTACK )
        {
            PM_ContinueHeadAnim( HEAD_ATTACK );
            PM_ContinueTorsoAnim( TORSO_ATTACK );
        }
        else
        {
            PM_StartHeadAnim( HEAD_ATTACK );
            PM_StartTorsoAnim( TORSO_ATTACK );
        }
    }

    //special weapon hacks
    if( weapon->weapon )  // This is the line I changed to get missing weaps to work **Tis Tis Shinster !!
    {
        if( ( pclass == CLASS_DWNINFINITY ) && ((pm->ps->weaponstate == WEAPON_OHCHARGING) || (pm->cmd.buttons & BUTTON_OFFHAND1)) )
        {
            pm->ps->weaponstate = WEAPON_OHFIRING;
        }
        else
        {
            pm->ps->weaponstate = WEAPON_FIRING;
        }
    }

    // check for out of energy, or if energy needed is greater than what is left
    if ( pm->ps->ammo[ pm->ps->weapon ] < weapon->energyuse )
    {
        PM_AddEvent( EV_NOAMMO );
        pm->ps->weaponTime += 400;
        return;
    }
    /*
    	// take an ammo away if not infinite
    	if ( pm->ps->ammo[ pm->ps->weapon ] != -1 ) {
    		pm->ps->ammo[ pm->ps->weapon ]--;
    	}
    	//check for out of energy
    	if( !pm->ps->stats[STAT_ENERGY]){
    		PM_AddEvent( EV_NOAMMO );
    		pm->ps->weaponTime += 500;
    		return;
    	}
    	*/

    // Offhand weapons
    //Com_Printf( "Weaponstate is %i\n", pm->ps->weaponstate );
    if( pm->ps->weaponstate == WEAPON_OHFIRING )
    {
        // fire offhand
        PM_AddEvent( EV_FIRE_OFFHAND );
    }
    else if( pm->ps->weaponstate == WEAPON_FIRING )
    {
        // fire weapon
        PM_AddEvent( EV_FIRE_WEAPON );
    }

    //default addTime switch-case was here

    // if you have the "focus" powerup, your time between shots is shortened
    if( upgrades & (1 << UG_FOCUS) )
        pm->ps->weaponTime += weapon->addtime * .75f;
    else
        pm->ps->weaponTime += weapon->addtime; // Here is where it sets weaponTime ( after fire events )

    // subtract from energy
    //if youre in the buster only class, and you have the buster upgrade, then you get half energy use!
    if( (pclass == CLASS_DRN000 || pclass == CLASS_DRN001) && (upgrades & (1 << UG_BUSTER)) )
        pm->ps->ammo[pm->ps->weapon] -= weapon->energyuse * .5f;
    else
        pm->ps->ammo[pm->ps->weapon] -= weapon->energyuse;

    // attack animation takes priority
    if( (legsAnim == LEGS_RUN) || (legsAnim == LEGS_BACK) )
        pm->ps->torsoTimer = weapon->addtime + 200;
    else
        pm->ps->torsoTimer = 350;

    //if you have a secondary weapon, and you are not in the charge class, you cant charge!
    if( pclass == CLASS_DWNINFINITY )
    {
        if( pm->cmd.buttons & BUTTON_ATTACK )
            pm->ps->weaponstate = WEAPON_CHARGING;
        else if( pm->cmd.buttons & BUTTON_OFFHAND1 )	//Z. Buster
            pm->ps->weaponstate = WEAPON_OHCHARGING;
    } // Here is where it sets the upgrade charge
    else if( ( pm->ps->weapon != WP_DEFAULT ) && (pclass == CLASS_DRN00X) && (upgrades & (1 << UG_BUSTER)) )
    {
        if( pm->cmd.buttons & BUTTON_ATTACK )
            pm->ps->weaponstate = WEAPON_CHARGING;
    }
    //set classes that can charge default weapons
    else if( ( pm->ps->weapon == WP_DEFAULT ) && ((pclass == CLASS_DRN00X) || (pclass == CLASS_DRN000 || pclass == CLASS_DRN001)) )
    {
        if( pm->cmd.buttons & BUTTON_ATTACK )
            pm->ps->weaponstate = WEAPON_CHARGING;
    }
}


/*
================
PM_Animate
================
*/

static void PM_Animate( void )
{
    int		legsAnim = ( pm->ps->legsAnim & ~ANIM_TOGGLEBIT );

    if ( pm->cmd.buttons & BUTTON_GESTURE )
    {
        if ( pm->ps->torsoTimer == 0 )
        {
            PM_StartHeadAnim( BOTH_GESTURE );
            PM_StartTorsoAnim( BOTH_GESTURE );
            pm->ps->torsoTimer = TIMER_GESTURE;
            if ( ( pm->ps->legsTimer == 0 ) && ( legsAnim == LEGS_IDLE ) )
            {
                PM_StartLegsAnim( BOTH_GESTURE );
                pm->ps->legsTimer = TIMER_GESTURE;
            }
            PM_AddEvent( EV_TAUNT );
        }
    }

}


/*
================
PM_AnimateDeath
================
*/

static void PM_AnimateDeath( void )
{

    PM_StartHeadAnim( BOTH_DEATH );
    PM_StartTorsoAnim( BOTH_DEATH );
    pm->ps->torsoTimer = 500;
    PM_StartLegsAnim( BOTH_DEATH );
    pm->ps->legsTimer = 500;
}


/*
================
PM_DropTimers
================
*/
static void PM_DropTimers( void )
{
    // drop misc timing counter
    if ( pm->ps->pm_time )
    {
        if ( pml.msec >= pm->ps->pm_time )
        {
            pm->ps->pm_flags &= ~PMF_ALL_TIMES;
            pm->ps->pm_time = 0;
        }
        else
        {
            pm->ps->pm_time -= pml.msec;
        }
    }

    // drop animation counter
    if ( pm->ps->legsTimer > 0 )
    {
        pm->ps->legsTimer -= pml.msec;
        if ( pm->ps->legsTimer < 0 )
        {
            pm->ps->legsTimer = 0;
        }
    }

    if ( pm->ps->torsoTimer > 0 )
    {
        pm->ps->torsoTimer -= pml.msec;
        if ( pm->ps->torsoTimer < 0 )
        {
            pm->ps->torsoTimer = 0;
        }
    }
}

/*
================
PM_UpdateViewAngles

This can be used as another entry point when only the viewangles
are being updated isntead of a full move
================
*/
void PM_UpdateViewAngles( playerState_t *ps, const usercmd_t *cmd )
{
    short		temp;
    int		i;

    if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPINTERMISSION)
    {
        return;		// no view changes at all
    }

    if ( ps->pm_type != PM_SPECTATOR && ps->stats[STAT_HEALTH] <= 0 )
    {
        return;		// no view changes at all
    }

    // NIGHTZ - SMOOTH ANGLE TRANSITION
    if (pm->ps->smoothAngle)
    {
        pm->ps->delta_angles[YAW] = ANGLE2SHORT (AngleNormalize180(SHORT2ANGLE(pm->ps->delta_angles[YAW])-pm->ps->smoothAngle));
        pm->ps->smoothAngle++;

        // Dont ask me why its that way
        if (pm->ps->smoothAngle == 19)
            pm->ps->smoothAngle = 0;
    }

    // circularly clamp the angles with deltas
    for (i=0 ; i<3 ; i++)
    {
        temp = cmd->angles[i] + ps->delta_angles[i];
        if ( i == PITCH )
        {
            // don't let the player look up or down more than 90 degrees
            if ( temp > 16000 )
            {
                ps->delta_angles[i] = 16000 - cmd->angles[i];
                temp = 16000;
            }
            else if ( temp < -16000 )
            {
                ps->delta_angles[i] = -16000 - cmd->angles[i];
                temp = -16000;
            }
        }
        ps->viewangles[i] = SHORT2ANGLE(temp);
    }

}


/*
===================
PM_GravWellMove
===================
*/
static void PM_GravWellMove( void )
{
    vec3_t vel, v;
    float vlen;

    VectorScale(pml.forward, -16, v);
    VectorAdd(pm->ps->gravwell, v, v);
    VectorSubtract(v, pm->ps->origin, vel);
    vlen = VectorLength(vel);
    VectorNormalize( vel );

    if (vlen >= 100)
        VectorScale(vel, 10 * vlen, vel);
    else
        VectorScale(vel, 1000, vel);

    VectorCopy(vel, pm->ps->velocity);

    pml.groundPlane = qfalse;
}


/*
===================
PM_DashMove

When you dash, you're dashing!

Peter FIXME: This can probably be cleaned up some, by merging some of those ifs

===================
*/
static void PM_DashMove( void )
{
    vec3_t	forward, right;
    qboolean	not_charging = ( pm->ps->weaponstate != WEAPON_CHARGING ) && ( pm->ps->weaponstate != WEAPON_OHCHARGING );
    qboolean	dash_back = ( pm->ps->pm_flags & PMF_BACKWARDS_RUN );

    VectorCopy( pml.right, right );

    //any techs get cancelled other than uppercut
    if( pm->ps->weaponstate > WEAPON_TECHUPCUT )
        pm->ps->weaponstate = WEAPON_READY;

    //Com_Printf( "groundPlane is %i\n", pml.groundPlane);
    pml.walking = qfalse;
    // normal slowdown
    pm->ps->pm_flags |= PMF_DASH_HELD;
    PM_Friction ();


    if (dash_back)
    {
        if ( ( pm->cmd.rightmove < 0 ) && ( pm->cmd.forwardmove == 0 ) )
        {
            VectorCopy( right, forward );
        }
        else if ( ( pm->cmd.rightmove < 0 ) && ( pm->cmd.forwardmove == 0 ) )
        {
            VectorNegate( right, forward );
        }
        else if( pm->cmd.forwardmove <= 0 )
        {
            VectorCopy( pml.forward, forward );
            if( pm->cmd.rightmove < 0 )
            {
                VectorAdd( right, forward, forward );
            }
            else if( pm->cmd.rightmove > 0 )
            {
                VectorNegate( right, right );
                VectorAdd( right, forward, forward );
            }
        }
        else if( pm->cmd.forwardmove > 0 )
        {
            VectorNegate( pml.forward, forward );
            if( pm->cmd.rightmove < 0 )
            {
                VectorAdd( right, forward, forward );
            }
            else if( pm->cmd.rightmove > 0 )
            {
                VectorNegate( right, right );
                VectorAdd( right, forward, forward );
            }
        }
        if( pm->ps->persistant[PLAYERCLASS] == CLASS_DRN00X )
        {
            forward[2] = 0;
        }
        else if ( !(pm->ps->pm_flags & PMF_AIRDASHING) )
        {
            forward[2] = 0;
        }
        VectorNormalize( forward );

        VectorScale( forward, -600, pm->ps->velocity);

        if( ( pm->ps->legsAnim & ANIM_TOGGLEBIT ) != LEGS_DASH )
        {
            if( not_charging )
            {
                PM_StartHeadAnim( HEAD_DASH_B );
                PM_StartTorsoAnim( TORSO_DASH_B );
            }

            PM_ForceLegsAnim( LEGS_DASH_B );
            pm->ps->legsTimer = pm->ps->torsoTimer = 200;
        }
        else
        {
            if( not_charging )
            {
                PM_ContinueHeadAnim( HEAD_DASH_B );
                PM_ContinueTorsoAnim( TORSO_DASH_B );
            }

            PM_ContinueLegsAnim( LEGS_DASH_B );
        }
    }
    else
    {
        if ( ( pm->cmd.rightmove > 0 ) && ( pm->cmd.forwardmove == 0 ) )
        {
            VectorCopy( right, forward );
        }
        else if ( ( pm->cmd.rightmove < 0 ) && ( pm->cmd.forwardmove == 0 ) )
        {
            VectorNegate( right, forward );
        }
        else if( pm->cmd.forwardmove >= 0 )
        {
            VectorCopy( pml.forward, forward );
            if( pm->cmd.rightmove > 0 )
            {
                VectorAdd( right, forward, forward );
            }
            else if( pm->cmd.rightmove < 0 )
            {
                VectorNegate( right, right );
                VectorAdd( right, forward, forward );
            }
        }
        else if( pm->cmd.forwardmove < 0 )
        {
            VectorNegate( pml.forward, forward );
            if( pm->cmd.rightmove > 0 )
            {
                VectorAdd( right, forward, forward );
            }
            else if( pm->cmd.rightmove < 0 )
            {
                VectorNegate( right, right );
                VectorAdd( right, forward, forward );
            }
        }
        if( pm->ps->persistant[PLAYERCLASS] == CLASS_DRN00X )
        {
            forward[2] = 0;
        }
        else if ( !(pm->ps->pm_flags & PMF_AIRDASHING) )
        {
            forward[2] = 0;
        }

        VectorNormalize( forward );

        VectorScale( forward, 600, pm->ps->velocity);

        if( ( pm->ps->legsAnim & ANIM_TOGGLEBIT ) != LEGS_DASH )
        {
            if( not_charging )
            {
                PM_StartHeadAnim( HEAD_DASH );
                PM_StartTorsoAnim( TORSO_DASH );
            }

            PM_ForceLegsAnim( LEGS_DASH );
            pm->ps->legsTimer = pm->ps->torsoTimer = 200;
        }
        else
        {
            if( not_charging )
            {
                PM_ContinueHeadAnim( HEAD_DASH );
                PM_ContinueTorsoAnim( TORSO_DASH );
            }

            PM_ContinueLegsAnim( LEGS_DASH );
        }
    }

    PM_StepSlideMove( qfalse );
}

/*
===================
PM_CheckJetPack

Can the player even use the jetpack?
===================
*/
static void PM_CheckJetPack( void )
{

    // add the fuel back if not using the jetpack
    if( !(pm->ps->persistant[PERS_UPGRADES] & (1 << UG_JETBOOTS) ) )
        return;

    if( pm->cmd.upmove <= 0 )
    {
        pm->ps->powerups[PW_JETPACK] = 0;
        //Com_Printf( " You have %i fuel left!\n", pm->ps->stats[STAT_FUEL]);
        pm->ps->stats[STAT_FUEL] += pml.msec*.5f;
        if( pm->ps->stats[STAT_FUEL] > 2000 )
            pm->ps->stats[STAT_FUEL] = 2000;
        return;
    }

    if( pm->ps->pm_flags & PMF_JUMP_HELD )
    {
        return;
    }

    if( !pml.groundPlane )   //if you are in the air...
    {
        if ( (pm->ps->stats[STAT_FUEL] > 200)  && pm->cmd.upmove )  //if you have just pressed jump...
        {
            pm->ps->powerups[PW_JETPACK] = INT_MAX; //turn it on
        }
        if( pm->ps->stats[STAT_FUEL] <= 0 )  	//if you run out of fuel, or let the jump key go...
        {
            //Com_Printf( "BOO! upmove is %i\n", pm->cmd.upmove );
            pm->ps->powerups[PW_JETPACK] = 0;
        }
    }
    else  	//youre on the ground, or youre not even holding the jump key
    {
        pm->ps->powerups[PW_JETPACK] = 0;
    }
}
/*
===================
PM_JetPackMove

Only with the jetpack upgrade
===================
*/
static void PM_JetPackMove( void )
{
    int		i;
    vec3_t	wishvel;
    float	wishspeed;
    vec3_t	wishdir;
    float	scale;

    // normal slowdown
    PM_Friction ();
    pm->ps->stats[STAT_FUEL] -= pml.msec;
    pml.walking = qfalse;
    //Com_Printf( " You have %i fuel left!\n", pm->ps->stats[STAT_FUEL]);

    scale = PM_CmdScale( &pm->cmd );
    //
    // user intentions
    //
    if ( !scale )
    {
        wishvel[0] = 0;
        wishvel[1] = 0;
        wishvel[2] = 0;
    }
    else
    {
        for (i=0 ; i<3 ; i++)
        {
            wishvel[i] = scale * pml.forward[i]*pm->cmd.forwardmove + scale * pml.right[i]*pm->cmd.rightmove;
        }

        wishvel[2] += scale * pm->cmd.upmove;
    }

    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);

    PM_Accelerate (wishdir, wishspeed, 5.0f);

    PM_StepSlideMove( qfalse );
}


/*
================
PmoveSingle

================
*/
void trap_SnapVector( float *v );

void PmoveSingle (pmove_t *pmove)
{
    pm = pmove;

    // this counter lets us debug movement problems with a journal
    // by setting a conditional breakpoint fot the previous frame
    c_pmove++;

    // clear results
    pm->numtouch = 0;
    pm->watertype = 0;
    pm->waterlevel = 0;

    if ( pm->ps->stats[STAT_HEALTH] <= 0 )
    {
        pm->tracemask &= ~CONTENTS_BODY;	// corpses can fly through bodies
    }

    if ( pm->ps->stats[STAT_HEALTH] > 0 && pm->ps->stats[STAT_HEALTH] < 30 )
    {
        pm->ps->eFlags |= EF_DAMAGED; //smoking players under 30 health
    }
    else
    {
        pm->ps->eFlags &= ~EF_DAMAGED;
    }

    // make sure walking button is clear if they are running, to avoid
    // proxy no-footsteps cheats
    if ( abs( pm->cmd.forwardmove ) > 64 || abs( pm->cmd.rightmove ) > 64 )
    {
        pm->cmd.buttons &= ~BUTTON_WALKING;
    }

    // set the talk balloon flag
    if ( pm->cmd.buttons & BUTTON_TALK )
    {
        pm->ps->eFlags |= EF_TALK;
    }
    else
    {
        pm->ps->eFlags &= ~EF_TALK;
    }

    // set the firing flag for continuous beam weapons
    if ( !(pm->ps->pm_flags & PMF_RESPAWNED) && pm->ps->pm_type != PM_INTERMISSION
            && ( pm->cmd.buttons & BUTTON_ATTACK ) && pm->ps->ammo[ pm->ps->weapon ] )
    {
        pm->ps->eFlags |= EF_FIRING;
    }
    else
    {
        pm->ps->eFlags &= ~EF_FIRING;
    }

    // Add in smoothAngle = 1 for certain key moments in gameplay
    if ( pm->ps->pm_flags & PMF_RESPAWNED )
    {
        pm->ps->smoothAngle = 1;
    }

    // clear the respawned flag if attack and use are cleared
    if ( pm->ps->stats[STAT_HEALTH] > 0 &&
            !( pm->cmd.buttons & (BUTTON_ATTACK | BUTTON_USE_HOLDABLE) ) )
    {
        pm->ps->pm_flags &= ~PMF_RESPAWNED;
    }

    // if talk button is down, dissallow all other input
    // this is to prevent any possible intercept proxy from
    // adding fake talk balloons
    if ( pmove->cmd.buttons & BUTTON_TALK )
    {
        // keep the talk button set tho for when the cmd.serverTime > 66 msec
        // and the same cmd is used multiple times in Pmove
        pmove->cmd.buttons = BUTTON_TALK;
        pmove->cmd.forwardmove = 0;
        pmove->cmd.rightmove = 0;
        pmove->cmd.upmove = 0;
    }

    // clear all pmove local vars
    memset (&pml, 0, sizeof(pml));

    // determine the time
    pml.msec = pmove->cmd.serverTime - pm->ps->commandTime;
    if ( pml.msec < 1 )
    {
        pml.msec = 1;
    }
    else if ( pml.msec > 200 )
    {
        pml.msec = 200;
    }
    pm->ps->commandTime = pmove->cmd.serverTime;

    // save old org in case we get stuck
    VectorCopy (pm->ps->origin, pml.previous_origin);

    // save old velocity for crashlanding
    VectorCopy (pm->ps->velocity, pml.previous_velocity);

    pml.frametime = pml.msec * 0.001;

    // update the viewangles

    PM_UpdateViewAngles( pm->ps, &pm->cmd );

    AngleVectors (pm->ps->viewangles, pml.forward, pml.right, pml.up);

    if ( pm->cmd.upmove < 10 )
    {
        // not holding jump
        pm->ps->pm_flags &= ~PMF_JUMP_HELD;
    }
    // are they holding dash?
    if( !(pm->cmd.buttons & BUTTON_DASH) )
    {
        pm->ps->pm_flags &= ~PMF_DASH_HELD;
    }

    // decide if backpedaling animations should be used
    if ( pm->cmd.forwardmove < 0 )
    {
        pm->ps->pm_flags |= PMF_BACKWARDS_RUN;
    }
    else if ( pm->cmd.forwardmove > 0 || ( pm->cmd.forwardmove == 0 && pm->cmd.rightmove ) )
    {
        pm->ps->pm_flags &= ~PMF_BACKWARDS_RUN;
    }

    if ( pm->ps->pm_type >= PM_DEAD )
    {
        pm->cmd.forwardmove = 0;
        pm->cmd.rightmove = 0;
        pm->cmd.upmove = 0;
    }

    if( pm->ps->stats[STAT_FLAGS] & SF_CAPSULE )
    {
        pm->cmd.forwardmove = 0;
        pm->cmd.rightmove = 0;
        pm->cmd.upmove = 0;
        PM_Animate();
        // set groundentity, watertype, and waterlevel
        PM_GroundTrace();
        PM_SetWaterLevel();
        pml.previous_waterlevel = pmove->waterlevel;
        pm->ps->powerups[PW_DASH] = 0;
        return;
    }

    if ( pm->ps->pm_type == PM_SPECTATOR )
    {
        PM_CheckDuck ();
        PM_FlyMove ();
        PM_DropTimers ();
        return;
    }

    if ( pm->ps->pm_type == PM_NOCLIP )
    {
        PM_NoclipMove ();
        PM_DropTimers ();
        return;
    }

    if (pm->ps->pm_type == PM_FREEZE)
    {
        return;		// no movement at all
    }

    if ( pm->ps->pm_type == PM_INTERMISSION || pm->ps->pm_type == PM_SPINTERMISSION)
    {
        return;		// no movement at all
    }

    // set watertype, and waterlevel
    PM_SetWaterLevel();
    pml.previous_waterlevel = pmove->waterlevel;

    // set mins, maxs, and viewheight
    PM_CheckDuck ();

    // set groundentity
    PM_GroundTrace();

    PM_CheckJetPack();	// Just make sure you can use it

    if ( pm->ps->pm_type == PM_DEAD )
    {
        PM_DeadMove ();
        PM_AnimateDeath();
    }

    PM_DropTimers();
    CheckLadder();

    // NIGHTZ - WALLJUMP MOVE
    PM_CheckWallJump ();

    // NIGHTZ - WALLJUMP
    if (pm->ps->pm_flags & PMF_WALL_HANG)
        PM_WallJumpMove ();
    else if ( pm->ps->pm_flags & PMF_GRAVWELL )
    {
        PM_GravWellMove();
        // We can wiggle a bit
        PM_AirMove();
    }
    else if( pm->ps->powerups[PW_DASH] )
        PM_DashMove();
    else if ( pm->ps->powerups[PW_JETPACK] && (pm->ps->stats[STAT_FUEL] > 0) )
    {
        //Rush Jetpack!
        PM_JetPackMove();
    }
    else if (pm->ps->pm_flags & PMF_GRAPPLE_PULL)
    {
        PM_GrappleMove();
        // We can wiggle a bit
        PM_AirMove();
    }
    else if (pm->ps->pm_flags & PMF_TIME_WATERJUMP)
    {
        PM_WaterJumpMove();
    }
    else if (pml.ladder)
    {
        PM_LadderMove();
    }
    else if ( pml.walking )
    {
        if( pm->ps->pm_flags & PMF_WALLWALKING )
            PM_ClimbMove( ); //TA: walking on any surface
        else
            // walking on ground
            PM_WalkMove();
    }
    else if ( pm->waterlevel > 1 )
    {
        PM_AirMove();
    }
    else
    {
        // airborne
        PM_AirMove();
    }

    PM_Animate();

    // set groundentity, watertype, and waterlevel
    PM_GroundTrace();
    PM_SetWaterLevel();

    // weapons
    PM_Weapon();

    // torso animation
    PM_TorsoAnimation();

    // footstep events / legs animations
    PM_Footsteps();

    // entering / leaving water splashes
    PM_WaterEvents();

    // snap some parts of playerstate to save network bandwidth
    trap_SnapVector( pm->ps->velocity );
}


/*
================
Pmove

Can be called by either the server or the client
================
*/
void Pmove (pmove_t *pmove)
{
    int			finalTime;

    finalTime = pmove->cmd.serverTime;

    if ( finalTime < pmove->ps->commandTime )
    {
        return;	// should not happen
    }

    if ( finalTime > pmove->ps->commandTime + 1000 )
    {
        pmove->ps->commandTime = finalTime - 1000;
    }

    pmove->ps->pmove_framecount = (pmove->ps->pmove_framecount+1) & ((1<<PS_PMOVEFRAMECOUNTBITS)-1);

    // chop the move up if it is too long, to prevent framerate
    // dependent behavior
    while ( pmove->ps->commandTime != finalTime )
    {
        int		msec;

        msec = finalTime - pmove->ps->commandTime;

        if ( pmove->pmove_fixed )
        {
            if ( msec > pmove->pmove_msec )
            {
                msec = pmove->pmove_msec;
            }
        }
        else
        {
            if ( msec > 66 )
            {
                msec = 66;
            }
        }
        pmove->cmd.serverTime = pmove->ps->commandTime + msec;
        PmoveSingle( pmove );

        if ( pmove->ps->pm_flags & PMF_JUMP_HELD )
        {
            pmove->cmd.upmove = 20;
        }
    }

    //PM_CheckStuck();

}
