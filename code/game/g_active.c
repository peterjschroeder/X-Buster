// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"


/*
===============
G_DamageFeedback

Called just before a snapshot is sent to the given player.
Totals up all damage and generates both the player_state_t
damage values to that client for pain blends and kicks, and
global pain sound events for all clients.
===============
*/
void P_DamageFeedback( gentity_t *player )
{
    gclient_t	*client;
    float	count;
    vec3_t	angles;

    client = player->client;
    if ( client->ps.pm_type == PM_DEAD )
    {
        return;
    }

    // total points of damage shot at the player this frame
    count = client->damage_blood + client->damage_armor;
    if ( count == 0 )
    {
        return;		// didn't take any damage
    }

    if ( count > 255 )
    {
        count = 255;
    }

    // send the information to the client

    // world damage (falling, slime, etc) uses a special code
    // to make the blend blob centered instead of positional
    if ( client->damage_fromWorld )
    {
        client->ps.damagePitch = 255;
        client->ps.damageYaw = 255;

        client->damage_fromWorld = qfalse;
    }
    else
    {
        vectoangles( client->damage_from, angles );
        client->ps.damagePitch = angles[PITCH]/360.0 * 256;
        client->ps.damageYaw = angles[YAW]/360.0 * 256;
    }

    // play an apropriate pain sound
    if ( (level.time > player->pain_debounce_time) && !(player->flags & FL_GODMODE) )
    {
        player->pain_debounce_time = level.time + 700;
        G_AddEvent( player, EV_PAIN, player->health );
        client->ps.damageEvent++;
    }


    client->ps.damageCount = count;

    //
    // clear totals
    //
    client->damage_blood = 0;
    client->damage_armor = 0;
    client->damage_knockback = 0;
}



/*
=============
P_WorldEffects

Check for lava / slime contents and drowning
=============
*/
void P_WorldEffects( gentity_t *ent )
{
    int			waterlevel;

    if ( ent->client->noclip )
    {
        ent->client->airOutTime = level.time + 12000;	// don't need air
        return;
    }

    waterlevel = ent->waterlevel;

    //
    // check for sizzle damage (move to pmove?)
    //
    if (waterlevel &&
            (ent->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
    {
        if (ent->health > 0
                && ent->pain_debounce_time <= level.time	)
        {

            /*if ( envirosuit ) {
            	G_AddEvent( ent, EV_POWERUP_BATTLESUIT, 0 );
            } else {*/
            if (ent->watertype & CONTENTS_LAVA)
            {
                G_Damage (ent, NULL, NULL, NULL, NULL,
                          30*waterlevel, 0, MOD_LAVA);
            }

            if (ent->watertype & CONTENTS_SLIME)
            {
                G_Damage (ent, NULL, NULL, NULL, NULL,
                          10*waterlevel, 0, MOD_SLIME);
            }
            //}
        }
    }
}



/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound( gentity_t *ent )
{
    if (ent->waterlevel && (ent->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
    {
        ent->client->ps.loopSound = level.snd_fry;
    }
    else
    {
        ent->client->ps.loopSound = 0;
    }
}



//==============================================================

/*
==============
ClientImpacts
==============
*/
void ClientImpacts( gentity_t *ent, pmove_t *pm )
{
    int		i, j;
    trace_t	trace;
    gentity_t	*other;

    memset( &trace, 0, sizeof( trace ) );
    for (i=0 ; i<pm->numtouch ; i++)
    {
        for (j=0 ; j<i ; j++)
        {
            if (pm->touchents[j] == pm->touchents[i] )
            {
                break;
            }
        }
        if (j != i)
        {
            continue;	// duplicated
        }
        other = &g_entities[ pm->touchents[i] ];

        if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) )
        {
            ent->touch( ent, other, &trace );
        }

        if ( !other->touch )
        {
            continue;
        }

        other->touch( other, ent, &trace );
    }

}

/*
============
G_TouchTriggers

Find all trigger entities that ent's current position touches.
Spectators will only interact with teleporters.
============
*/
void	G_TouchTriggers( gentity_t *ent )
{
    int			i, num;
    int			touch[MAX_GENTITIES];
    gentity_t	*hit;
    trace_t		trace;
    vec3_t		mins, maxs;
    static vec3_t	range = { 40, 40, 52 };

    if ( !ent->client )
    {
        return;
    }

    // dead clients don't activate triggers!
    if ( ent->client->ps.stats[STAT_HEALTH] <= 0 )
    {
        return;
    }

    VectorSubtract( ent->client->ps.origin, range, mins );
    VectorAdd( ent->client->ps.origin, range, maxs );

    num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

    // can't use ent->absmin, because that has a one unit pad
    VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
    VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

    for ( i=0 ; i<num ; i++ )
    {
        hit = &g_entities[touch[i]];

        if ( !hit->touch && !ent->touch )
        {
            continue;
        }
        if ( !( hit->r.contents & CONTENTS_TRIGGER ) )
        {
            continue;
        }

        // ignore most entities if a spectator
        if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
        {
            if ( hit->s.eType != ET_TELEPORT_TRIGGER &&
                    // this is ugly but adding a new ET_? type will
                    // most likely cause network incompatibilities
                    hit->touch != Touch_DoorTrigger)
            {
                continue;
            }
        }

        // use seperate code for determining if an item is picked up
        // so you don't have to actually contact its bounding box
        if ( hit->s.eType == ET_ITEM )
        {
            if ( !BG_PlayerTouchesItem( &ent->client->ps, &hit->s, level.time ) )
            {
                continue;
            }
        }
        else
        {
            if ( !trap_EntityContact( mins, maxs, hit ) )
            {
                continue;
            }
        }

        memset( &trace, 0, sizeof(trace) );

        if ( hit->touch )
        {
            hit->touch (hit, ent, &trace);
        }

        if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) )
        {
            ent->touch( ent, hit, &trace );
        }
    }

    // if we didn't touch a jump pad this pmove frame
    if ( ent->client->ps.jumppad_frame != ent->client->ps.pmove_framecount )
    {
        ent->client->ps.jumppad_frame = 0;
        ent->client->ps.jumppad_ent = 0;
    }
}

/*
=================
SpectatorThink
=================
*/
void SpectatorThink( gentity_t *ent, usercmd_t *ucmd )
{
    pmove_t	pm;
    gclient_t	*client;

    client = ent->client;

    if ( client->sess.spectatorState != SPECTATOR_FOLLOW )
    {
        client->ps.pm_type = PM_SPECTATOR;
        client->ps.speed = 400;	// faster than normal

        // set up for pmove
        memset (&pm, 0, sizeof(pm));
        pm.ps = &client->ps;
        pm.cmd = *ucmd;
        pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;	// spectators can fly through bodies
        pm.trace = trap_Trace;
        pm.pointcontents = trap_PointContents;

        // perform a pmove
        Pmove (&pm);
        // save results of pmove
        VectorCopy( client->ps.origin, ent->s.origin );

        G_TouchTriggers( ent );
        trap_UnlinkEntity( ent );
    }

    client->oldbuttons = client->buttons;
    client->buttons = ucmd->buttons;

    // attack button cycles through spectators
    if ( ( client->buttons & BUTTON_ATTACK ) && ! ( client->oldbuttons & BUTTON_ATTACK ) )
    {
        do_followcycle( ent, NULL, 0, 1 );
    }
}



/*
=================
ClientInactivityTimer

Returns qfalse if the client is dropped
=================
*/
qboolean ClientInactivityTimer( gclient_t *client )
{
    if ( ! g_inactivity.integer )
    {
        // give everyone some time, so if the operator sets g_inactivity during
        // gameplay, everyone isn't kicked
        client->inactivityTime = level.time + 60 * 1000;
        client->inactivityWarning = qfalse;
    }
    else if ( client->pers.cmd.forwardmove ||
              client->pers.cmd.rightmove ||
              client->pers.cmd.upmove ||
              (client->pers.cmd.buttons & BUTTON_ATTACK) )
    {
        client->inactivityTime = level.time + g_inactivity.integer * 1000;
        client->inactivityWarning = qfalse;
    }
    else if ( !client->pers.localClient )
    {
        if ( level.time > client->inactivityTime )
        {
            trap_DropClient( client - level.clients, "Dropped due to inactivity" );
            return qfalse;
        }
        if ( level.time > client->inactivityTime - 10000 && !client->inactivityWarning )
        {
            client->inactivityWarning = qtrue;
            trap_SendServerCommand( client - level.clients, "cp \"Ten seconds until inactivity drop!\n\"" );
        }
    }
    return qtrue;
}

/*
==================
ClientTimerActions

Actions that happen once a second
==================
*/
void ClientTimerActions( gentity_t *ent, int msec )
{
    gclient_t	*client;

    client = ent->client;
    client->timeResidual += msec;

    while ( client->timeResidual >= 1000 )
    {
        client->timeResidual -= 1000;

        // health regen
        if( IS_SET(client->ps.persistant[PERS_UPGRADES], (1 << UG_REGEN) ) )
        {
            if ( ent->health < client->ps.stats[STAT_MAX_HEALTH])
            {
                ent->health += 2;

                if ( ent->health > client->ps.stats[STAT_MAX_HEALTH])
                    ent->health = client->ps.stats[STAT_MAX_HEALTH];
            }
        }


        // count down health when over max
        if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] )
            ent->health--;

        if ( client->ps.stats[STAT_ARMOR] > client->pers.maxArmor )
        {
            client->ps.stats[STAT_ARMOR]--;
        }
    }
}

/*
====================
ClientIntermissionThink
====================
*/
void ClientIntermissionThink( gclient_t *client )
{
    client->ps.eFlags &= ~EF_TALK;
    client->ps.eFlags &= ~EF_FIRING;

    // the level will exit when everyone wants to or after timeouts

    // swap and latch button actions
    client->oldbuttons = client->buttons;
    client->buttons = client->pers.cmd.buttons;
    if ( client->buttons & ( BUTTON_ATTACK | BUTTON_USE_HOLDABLE ) & ( client->oldbuttons ^ client->buttons ) )
    {
        // this used to be an ^1 but once a player says ready, it should stick
        client->readyToExit = 1;
    }
}


/*
================
ClientEvents

Events will be passed on to the clients for presentation,
but any server game effects are handled here
================
*/
void ClientEvents( gentity_t *ent, int oldEventSequence )
{
    int		i;
    int		event;
    gclient_t *client;
    int		damage;
    vec3_t	dir;

    client = ent->client;

    if ( oldEventSequence < client->ps.eventSequence - MAX_PS_EVENTS )
    {
        oldEventSequence = client->ps.eventSequence - MAX_PS_EVENTS;
    }
    for ( i = oldEventSequence ; i < client->ps.eventSequence ; i++ )
    {
        event = client->ps.events[ i & (MAX_PS_EVENTS-1) ];

        switch ( event )
        {
        case EV_FALL_MEDIUM:
        case EV_FALL_FAR:
            if ( ent->s.eType != ET_PLAYER )
            {
                break;		// not in the player model
            }
            if ( g_dmflags.integer & DF_NO_FALLING )
            {
                break;
            }
            if ( event == EV_FALL_FAR )
            {
                damage = 10;
            }
            else
            {
                damage = 5;
            }
            VectorSet (dir, 0, 0, 1);
            ent->pain_debounce_time = level.time + 200;	// no normal pain sound
            break;

        case EV_FIRE_WEAPON:
            FireWeapon( ent );
            break;

        case EV_FIRE_OFFHAND:
            FireOffhand( ent );
            break;

        case EV_USE_ITEM1:		// subtank
            // Cap the subtank to 50
            if (ent->client->ps.persistant[PERS_SUBTANKS] > 50)
                ent->client->ps.persistant[PERS_SUBTANKS] = 50;

            ent->health += ent->client->ps.persistant[PERS_SUBTANKS];
            ent->client->ps.persistant[PERS_SUBTANKS] = 0;
            break;

        default:
            break;
        }
    }

}

void BotTestSolid(vec3_t origin);

/*
==============
SendPendingPredictableEvents
==============
*/
void SendPendingPredictableEvents( playerState_t *ps )
{
    gentity_t *t;
    int event, seq;
    int extEvent, number;

    // if there are still events pending
    if ( ps->entityEventSequence < ps->eventSequence )
    {
        // create a temporary entity for this event which is sent to everyone
        // except the client who generated the event
        seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
        event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
        // set external event to zero before calling BG_PlayerStateToEntityState
        extEvent = ps->externalEvent;
        ps->externalEvent = 0;
        // create temporary entity for event
        t = G_TempEntity( ps->origin, event );
        number = t->s.number;
        BG_PlayerStateToEntityState( ps, &t->s, qtrue );
        t->s.number = number;
        t->s.eType = ET_EVENTS + event;
        t->s.eFlags |= EF_PLAYER_EVENT;
        t->s.otherEntityNum = ps->clientNum;
        // send to everyone except the client who generated the event
        t->r.svFlags |= SVF_NOTSINGLECLIENT;
        t->r.singleClient = ps->clientNum;
        // set back external event
        ps->externalEvent = extEvent;
    }
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame on fast clients.

If "g_synchronousClients 1" is set, this will be called exactly
once for each server frame, which makes for smooth demo recording.
==============
*/
void ClientThink_real( gentity_t *ent )
{
    gclient_t	*client;
    pmove_t		pm;
    int			oldEventSequence;
    int			msec;
    usercmd_t	*ucmd;
    qboolean	canairdash = qfalse;
    int			chargeleveltime = 1000; //the time to get the next charge level

    client = ent->client;

    // don't think if the client is not yet connected (and thus not yet spawned in)
    if (client->pers.connected != CON_CONNECTED)
    {
        return;
    }
    // mark the time, so the connection sprite can be removed
    ucmd = &ent->client->pers.cmd;

    if (ent->stop_event)
    {
        if (ent->stop_event>level.time)
        {
            ucmd->forwardmove=0;
            ucmd->rightmove=0;
            ucmd->upmove=0;
            ucmd->buttons=0;
        }
        else
            ent->stop_event=0;
    }

    ucmd->buttons &= ~BUTTON_USE_HOLDABLE;
    if (ent->slow_event)
    {
        if (ent->slow_event>level.time)
        {
            ucmd->forwardmove/=4;
            ucmd->rightmove/=4;
            ucmd->upmove/=4;
        }
        else
            ent->slow_event=0;
    }

    // sanity check the command time to prevent speedup cheating
    if ( ucmd->serverTime > level.time + 200 )
    {
        ucmd->serverTime = level.time + 200;
//		G_Printf( "serverTime <<<<<\n" );
    }
    if ( ucmd->serverTime < level.time - 1000 )
    {
        ucmd->serverTime = level.time - 1000;
//		G_Printf( "serverTime >>>>>\n" );
    }

    msec = ucmd->serverTime - client->ps.commandTime;
    // following others may result in bad times, but we still want
    // to check for follow toggles
    if ( msec < 1 && client->sess.spectatorState != SPECTATOR_FOLLOW )
    {
        return;
    }
    if ( msec > 200 )
    {
        msec = 200;
    }

    if ( pmove_msec.integer < 8 )
    {
        trap_Cvar_Set("pmove_msec", "8");
    }
    else if (pmove_msec.integer > 33)
    {
        trap_Cvar_Set("pmove_msec", "33");
    }

    if ( pmove_fixed.integer || client->pers.pmoveFixed )
    {
        ucmd->serverTime = ((ucmd->serverTime + pmove_msec.integer-1) / pmove_msec.integer) * pmove_msec.integer;
        //if (ucmd->serverTime - client->ps.commandTime <= 0)
        //	return;
    }

    //
    // check for exiting intermission
    //
    if ( level.intermissiontime )
    {
        ClientIntermissionThink( client );
        return;
    }

    // spectators don't do much
    if ( client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD )
        {
            return;
        }
        SpectatorThink( ent, ucmd );
        return;
    }

    // check for inactivity timer, but never drop the local client of a non-dedicated server
    if ( !ClientInactivityTimer( client ) )
    {
        return;
    }

    if ( client->noclip )
    {
        client->ps.pm_type = PM_NOCLIP;
    }
    else if ( client->ps.stats[STAT_HEALTH] <= 0 )
    {
        client->ps.pm_type = PM_DEAD;
    }
    else if ( IS_SET(client->ps.eFlags, EF_FROZEN) )
    {
        if (level.time >= ent->client->freeze_time)
        {
            client->ps.pm_type = PM_NORMAL;
            REMOVE_BIT(ent->client->ps.eFlags, EF_FROZEN);
        }
        else
        {
            client->ps.pm_type = PM_FREEZE;
            G_Damage (ent, NULL, NULL, NULL, NULL, 1, 0, MOD_ICE);
        }
    }
    else if ( IS_SET(client->ps.eFlags, EF_LIGHTNSTRUCK) )
    {
        if (level.time >= ent->client->lstruck_time)
        {
            client->ps.pm_type = PM_NORMAL;
            REMOVE_BIT(ent->client->ps.eFlags, EF_LIGHTNSTRUCK);
        }
        else
        {
            client->ps.pm_type = PM_FREEZE;
        }
    }
    else
    {
        client->ps.pm_type = PM_NORMAL;
    }

    // Peter: Check for shock damage
    if ( IS_SET(client->ps.eFlags, EF_SHOCKED) )
    {
        if (level.time >= ent->client->shock_time)
            REMOVE_BIT(ent->client->ps.eFlags, EF_SHOCKED);
        else
            G_Damage (ent, NULL, NULL, NULL, NULL, 1, 0, MOD_LIGHTNING);
    }

    if( ent->waterlevel == 3 )
        client->ps.gravity = .5f*g_gravity.value;
    else if( ent->client->ps.persistant[PERS_UPGRADES] & (1 << UG_MOONBOOTS) )
        client->ps.gravity = g_gravity.value * 0.20;
    else
        client->ps.gravity = g_gravity.value;


    // Let go of the hook if we aren't firing
    // Peter FIXME: For some reason the weapon is not == WP_STRIKECHAIN
    if ( /*client->ps.weapon == WP_STRIKECHAIN &&
		*/client->hook && !( ucmd->buttons & BUTTON_ATTACK ) )
    {
        Weapon_HookFree(client->hook);
    }


    // set up for pmove
    oldEventSequence = client->ps.eventSequence;

    memset (&pm, 0, sizeof(pm));

    if ( ent->flags & FL_FORCE_GESTURE )
    {
        ent->flags &= ~FL_FORCE_GESTURE;
        ent->client->pers.cmd.buttons |= BUTTON_GESTURE;
    }

    pm.ps = &client->ps;
    pm.cmd = *ucmd;
    if ( pm.ps->pm_type == PM_DEAD )
    {
        pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
    }
    else if ( ent->r.svFlags & SVF_BOT )
    {
        pm.tracemask = MASK_PLAYERSOLID | CONTENTS_BOTCLIP;
    }
    else
    {
        pm.tracemask = MASK_PLAYERSOLID;
    }
    pm.trace = trap_Trace;
    pm.pointcontents = trap_PointContents;
    pm.debugLevel = g_debugMove.integer;
    pm.noFootsteps = ( g_dmflags.integer & DF_NO_FOOTSTEPS ) > 0;

    pm.pmove_fixed = pmove_fixed.integer | client->pers.pmoveFixed;
    pm.pmove_msec = pmove_msec.integer;

    VectorCopy( client->ps.origin, client->oldOrigin );

    Pmove (&pm);

    // save results of pmove
    if ( ent->client->ps.eventSequence != oldEventSequence )
    {
        ent->eventTime = level.time;
    }
    if (g_smoothClients.integer)
    {
        BG_PlayerStateToEntityStateExtraPolate( &ent->client->ps, &ent->s, ent->client->ps.commandTime, qtrue );
    }
    else
    {
        BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
    }
    SendPendingPredictableEvents( &ent->client->ps );

    if ( !( ent->client->ps.eFlags & EF_FIRING ) )
    {
        client->fireHeld = qfalse;		// for grapple
    }

    // use the snapped origin for linking so it matches client predicted versions
    VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );

    VectorCopy (pm.mins, ent->r.mins);
    VectorCopy (pm.maxs, ent->r.maxs);

    ent->waterlevel = pm.waterlevel;
    ent->watertype = pm.watertype;

    // execute client events
    ClientEvents( ent, oldEventSequence );

    // link entity now, after any personal teleporters have been used
    trap_LinkEntity (ent);
    if ( !ent->client->noclip )
    {
        G_TouchTriggers( ent );
    }

    // NOTE: now copy the exact origin over otherwise clients can be snapped into solid
    VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );

    //test for solid areas in the AAS file
    BotTestAAS(ent->r.currentOrigin);

    // touch other objects
    ClientImpacts( ent, &pm );

    // save results of triggers and client events
    if (ent->client->ps.eventSequence != oldEventSequence)
    {
        ent->eventTime = level.time;
    }

    // swap and latch button actions
    client->oldbuttons = client->buttons;
    client->buttons = ucmd->buttons;
    client->latched_buttons |= client->buttons & ~client->oldbuttons;

    // check for respawning
    if ( client->ps.stats[STAT_HEALTH] <= 0 )
    {
        // wait for the attack button to be pressed
        if ( level.time > client->respawnTime )
        {
            // forcerespawn is to prevent users from waiting out powerups
            if ( g_forcerespawn.integer > 0 &&
                    ( level.time - client->respawnTime ) > g_forcerespawn.integer * 1000 )
            {
                respawn( ent );
                return;
            }

            // pressing attack or use is the normal respawn method
            if ( ucmd->buttons & ( BUTTON_ATTACK | BUTTON_USE_HOLDABLE ) )
            {
                respawn( ent );
            }
        }
        return;
    }

    // Peter: If there affected by slow, half the speed
    if (IS_SET( client->ps.eFlags, EF_SLOW ))
        client->ps.speed = 80;
    else
        client->ps.speed = g_speed.value*bg_playerlist[client->pers.playerclass].speed; //Peter FIXME: Take in account of body damage

    // apply speed boost upgrade
    if (IS_SET(client->ps.persistant[PERS_UPGRADES], (1 << UG_SPEEDBOOST)))
        client->ps.speed *= 1;

    // check to see if you can even airdash
    if( client->ps.persistant[PERS_UPGRADES] & ( 1 << UG_DASH ) )
        canairdash = qtrue;

    //if you are uppercutting, you cannot do any dashing
    if( client->ps.weaponstate == WEAPON_TECHUPCUT )
        canairdash = qfalse;

    // powerup checks
    if( ( pm.cmd.buttons & BUTTON_DASH ) && ( client->dashTime < level.time) && !(client->ps.pm_flags & PMF_DASH_HELD) &&
            (client->ps.persistant[PERS_UPGRADES] & (1 << UG_DASH)))
    {
        vec3_t point; //we are going to see whether you are in the ground or air
        trace_t trace;
        VectorCopy( client->ps.origin, point );
        point[2] -= 1;
        trap_Trace ( &trace, client->ps.origin, pm.mins, pm.maxs, point, client->ps.clientNum, pm.tracemask );

        client->ps.powerups[PW_DASH] = level.time + 200; //set defaults
        client->dashTime = level.time + 700;
        // we need to make sure youre on the ground before giving you the boost if you dont have air dash
        if( !trace.contents )   //if you are in the air...
        {
            if( !canairdash ) //if you dont have the air dash powerup
            {
                client->ps.powerups[PW_DASH] = 0; //dont dash!
                client->dashTime = 0;
            }
            else
            {
                //if you DO have the air dash powerup
                if( client->ps.pm_flags & PMF_CAN_AIRDASH )   //if you have not airdashed yet
                {
                    client->ps.pm_flags |= PMF_AIRDASHING;
                    client->ps.pm_flags &= ~PMF_CAN_AIRDASH; //no more dashes till you land
                    client->ps.pm_flags &= ~PMF_WALL_HANG; //no wall hanging
                    client->ps.eFlags &= ~EF_WALLHANG;
                    client->dashStop = qfalse;
                    G_Sound( ent, CHAN_AUTO, G_SoundIndex("sound/player/dash_start.wav") );
                }
                else     //if you HAVE airdashed and have not landed
                {
                    client->ps.powerups[PW_DASH] = 0; //dont dash!
                    client->dashTime = 0;
                }
            }
        }
        else
            G_Sound( ent, CHAN_AUTO, G_SoundIndex("sound/player/dash_start.wav") );
    }

    if( (( client->ps.weaponstate == WEAPON_CHARGING ) || ( client->ps.weaponstate == WEAPON_OHCHARGING ))&& (client->ps.weaponTime < 50 ))
    {
        int	chargeTime;
        //if you have the quick charge upgrade, you get slightly faster charge times
        if( client->ps.persistant[PERS_UPGRADES] & ( 1 << UG_QUICKCHARGE ) )
            chargeleveltime *= .75f;
        //if you havent started charging, start darn it!
        if( !client->chargeStart )
            client->chargeStart = level.time;

        chargeTime = level.time - client->chargeStart;

        if( (client->pers.playerclass == CLASS_DRN00X) && (client->ps.persistant[PERS_UPGRADES] & ( 1 << UG_BUSTER )) && (chargeTime > chargeleveltime*3.5f) )
        {
            //if( client->ps.stats[STAT_CHARGE] != 4 )
            //	BG_AddPredictableEventToPlayerstate( EV_CHARGE4, 0, &client->ps );
            //	client->ps.stats[STAT_CHARGE] = 4;
            //G_Printf( "Charge level is now 4!\n");
        }
        else if( (client->pers.playerclass == CLASS_DRN00X) && (client->ps.persistant[PERS_UPGRADES] & ( 1 << UG_BUSTER )) && (chargeTime > chargeleveltime*2.5f) )
        {
            if( client->ps.stats[STAT_CHARGE] != 3 )
                BG_AddPredictableEventToPlayerstate( EV_CHARGE3, 0, &client->ps );
            client->ps.stats[STAT_CHARGE] = 3;
            //G_Printf( "Charge level is now 3!\n");
        }
        else if( chargeTime > chargeleveltime*2.0f )
        {
            if( client->ps.stats[STAT_CHARGE] != 2 )
                BG_AddPredictableEventToPlayerstate( EV_CHARGE2, 0, &client->ps );
            client->ps.stats[STAT_CHARGE] = 2;
            //G_Printf( "Charge level is now 2!\n");
        }
        else if( chargeTime > chargeleveltime*1.0f )
        {
            if( client->ps.stats[STAT_CHARGE] != 1 )
                BG_AddPredictableEventToPlayerstate( EV_CHARGE1, 0, &client->ps );
            client->ps.stats[STAT_CHARGE] = 1;
            //G_Printf( "Charge level is now 1!\n");
        }
        else
        {
            client->ps.stats[STAT_CHARGE] = 0;
            BG_AddPredictableEventToPlayerstate( EV_NOCHARGE, 0, &client->ps );//===sep10
        }
    }
    else if ( client->ps.stats[STAT_CHARGE] )
    {
        client->chargeStart = 0;
        client->ps.stats[STAT_CHARGE] = 0;
        BG_AddPredictableEventToPlayerstate( EV_NOCHARGE, 0, &client->ps );
    }
    // add the powerup corresponding to the upgrade if the player has it
    if( client->ps.persistant[PERS_UPGRADES] & (1 << UG_FOCUS) )
        client->ps.powerups[PW_FOCUS] = INT_MAX;
    else
        client->ps.powerups[PW_FOCUS] = 0;

    if( client->ps.persistant[PERS_UPGRADES] & (1 << UG_POWERCONVERTER) )
        client->ps.powerups[PW_POWERCONVERTER] = INT_MAX;
    else
        client->ps.powerups[PW_POWERCONVERTER] = 0;

    if( client->ps.persistant[PERS_UPGRADES] & (1 << UG_AIRJUMP) )
        client->ps.powerups[PW_AIRJUMP] = INT_MAX;
    else
        client->ps.powerups[PW_AIRJUMP] = 0;

    if( client->ps.persistant[PERS_UPGRADES] & (1 << UG_BUSTER) )
        client->ps.powerups[PW_BUSTER] = INT_MAX;
    else
        client->ps.powerups[PW_BUSTER] = 0;

    if (IS_SET(ent->client->ps.eFlags, EF_LAGGED))
        ent->client->lag_time = level.time+2000;


    // perform once-a-second actions
    ClientTimerActions( ent, msec );
}

/*
==================
ClientThink

A new command has arrived from the client
==================
*/
void ClientThink( int clientNum )
{
    gentity_t *ent;

    ent = g_entities + clientNum;

    if (IS_SET(ent->client->ps.eFlags, EF_LAGGED) && ent->client->lag_time >= level.time)
        return;

    trap_GetUsercmd( clientNum, &ent->client->pers.cmd );

    // mark the time we got info, so we can display the
    // phone jack if they don't get any for a while
    ent->client->lastCmdTime = level.time;

    if ( !(ent->r.svFlags & SVF_BOT) && !g_synchronousClients.integer )
    {
        ClientThink_real( ent );
    }
}


void G_RunClient( gentity_t *ent )
{
    if ( !(ent->r.svFlags & SVF_BOT) && !g_synchronousClients.integer )
    {
        return;
    }
    ent->client->pers.cmd.serverTime = level.time;
    ClientThink_real( ent );
}


/*
==================
SpectatorClientEndFrame

==================
*/
void SpectatorClientEndFrame( gentity_t *ent )
{
    gclient_t	*cl;

    // if we are doing a chase cam or a remote view, grab the latest info
    if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
    {
        int		clientNum, flags;

        clientNum = ent->client->sess.spectatorClient;

        // team follow1 and team follow2 go to whatever clients are playing
        if ( clientNum == -1 )
        {
            clientNum = level.follow1;
        }
        else if ( clientNum == -2 )
        {
            clientNum = level.follow2;
        }
        if ( clientNum >= 0 )
        {
            cl = &level.clients[ clientNum ];
            if ( cl->pers.connected == CON_CONNECTED && cl->sess.sessionTeam != TEAM_SPECTATOR )
            {
                flags = (cl->ps.eFlags & ~(EF_VOTED | EF_TEAMVOTED)) | (ent->client->ps.eFlags & (EF_VOTED | EF_TEAMVOTED));
                ent->client->ps = cl->ps;
                ent->client->ps.pm_flags |= PMF_FOLLOW;
                ent->client->ps.eFlags = flags;
                return;
            }
            else
            {
                // drop them to free spectators unless they are dedicated camera followers
                if ( ent->client->sess.spectatorClient >= 0 )
                {
                    ent->client->sess.spectatorState = SPECTATOR_FREE;
                    ClientBegin( ent->client - level.clients );
                }
            }
        }
    }

    if ( ent->client->sess.spectatorState == SPECTATOR_SCOREBOARD )
    {
        ent->client->ps.pm_flags |= PMF_SCOREBOARD;
    }
    else
    {
        ent->client->ps.pm_flags &= ~PMF_SCOREBOARD;
    }
}

/*
==============
ClientEndFrame

Called at the end of each server frame for each connected client
A fast client will have multiple ClientThink for each ClientEdFrame,
while a slow client may have multiple ClientEndFrame between ClientThink.
==============
*/
void ClientEndFrame( gentity_t *ent )
{
    int			i;
    clientPersistant_t	*pers;

    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        SpectatorClientEndFrame( ent );
        return;
    }

    pers = &ent->client->pers;

    // Peter FIXME: Do we need this??
    // turn off any expired powerups
    // some powerups we want to keep hanging around...
    for ( i = 0 ; i < MAX_POWERUPS ; i++ )
    {
        if ( ent->client->ps.powerups[ i ] < level.time )
        {
            //if( i != PW_DASH )
            ent->client->ps.powerups[ i ] = 0;
        }
    }

    // save network bandwidth
#if 0
    if ( !g_synchronousClients->integer && ent->client->ps.pm_type == PM_NORMAL )
    {
        // FIXME: this must change eventually for non-sync demo recording
        VectorClear( ent->client->ps.viewangles );
    }
#endif

    //
    // If the end of unit layout is displayed, don't give
    // the player any normal movement attributes
    //
    if ( level.intermissiontime )
    {
        return;
    }

    // burn from lava, etc
    P_WorldEffects (ent);

    // apply all the damage taken this frame
    P_DamageFeedback (ent);

    // add the EF_CONNECTION flag if we haven't gotten commands recently
    if ( level.time - ent->client->lastCmdTime > 1000 )
    {
        ent->s.eFlags |= EF_CONNECTION;
    }
    else
    {
        ent->s.eFlags &= ~EF_CONNECTION;
    }

    ent->client->ps.stats[STAT_HEALTH] = ent->health;	// FIXME: get rid of ent->health...

    G_SetClientSound (ent);

    // set the latest infor
    if (g_smoothClients.integer)
    {
        BG_PlayerStateToEntityStateExtraPolate( &ent->client->ps, &ent->s, ent->client->ps.commandTime, qtrue );
    }
    else
    {
        BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
    }
    SendPendingPredictableEvents( &ent->client->ps );

    // set the bit for the reachability area the client is currently in
//	i = trap_AAS_PointReachabilityAreaIndex( ent->client->ps.origin );
//	ent->client->areabits[i >> 3] |= 1 << (i & 7);
}
