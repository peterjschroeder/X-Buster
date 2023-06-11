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



void do_god (gentity_t *ent)
{
    char	*msg;

    ent->flags ^= FL_GODMODE;
    if (!(ent->flags & FL_GODMODE) )
        msg = "godmode OFF\n";
    else
        msg = "godmode ON\n";

    trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}



void do_notarget(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    char	*msg;

    ent->flags ^= FL_NOTARGET;
    if (!(ent->flags & FL_NOTARGET) )
        msg = "notarget OFF\n";
    else
        msg = "notarget ON\n";

    trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}



void do_noclip(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    char	*msg;

    if ( ent->client->noclip )
    {
        msg = "noclip OFF\n";
    }
    else
    {
        msg = "noclip ON\n";
    }
    ent->client->noclip = !ent->client->noclip;

    trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
do_levelShot

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void do_levelshot(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    // doesn't work in single player
    if ( g_gametype.integer != 0 )
    {
        trap_SendServerCommand( ent-g_entities,
                                "print \"Must be in g_gametype 0 for levelshot\n\"" );
        return;
    }

    BeginIntermission();
    trap_SendServerCommand( ent-g_entities, "clientLevelShot" );
}


void do_setviewpos(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    vec3_t		origin, angles;
    char buff[MAX_TOKEN_CHARS];
    char buff2[MAX_TOKEN_CHARS];
    char arg1[MAX_TOKEN_CHARS];
    char arg2[MAX_TOKEN_CHARS];
    char arg3[MAX_TOKEN_CHARS];
    char arg4[MAX_TOKEN_CHARS];

    half_chop(argument, arg1, buff);
    half_chop(buff, arg2, buff2);
    half_chop(buff2, arg3, arg4);

    if ( !*arg1 || !*arg2 || !*arg3 || !*arg4 )
    {
        trap_SendServerCommand( ent-g_entities, va("print \"usage: setviewpos x y z yaw\n\""));
        return;
    }

    VectorClear( angles );

    origin[0] = atof( arg1 );
    origin[1] = atof( arg2 );
    origin[2] = atof( arg3 );

    angles[YAW] = atof( arg4 );

    TeleportPlayer( ent, origin, angles );
}


void do_class(gentity_t *ent, char *name, int cmd, int subcmd)
{
    if (Q_stricmp( name, "melee") == 0)
    {
        ent->client->pers.playerclass = ent->client->ps.persistant[PLAYERCLASS] = CLASS_DWNINFINITY;
        G_Printf( "your class is now melee\n");
    }
    else if (Q_stricmp( name, "buster_only") == 0)
    {
        ent->client->pers.playerclass = ent->client->ps.persistant[PLAYERCLASS] = CLASS_DRN001;
        G_Printf( "your class is drn-001\n");
    }
    else if (Q_stricmp( name, "buster_charge") == 0)
    {
        ent->client->pers.playerclass = ent->client->ps.persistant[PLAYERCLASS] = CLASS_DRN00X;
        G_Printf( "your class is now buster_charge\n");
    }
    else if (Q_stricmp( name, "buster_rapid") == 0)
    {
        ent->client->pers.playerclass = ent->client->ps.persistant[PLAYERCLASS] = CLASS_SWN001;
        G_Printf( "your class is now buster_rapid\n");
    }
    else
    {
        G_Printf( "incorrect choice! valid classes are:\n");
        G_Printf( "melee\n");
        G_Printf( "buster_only\n");
        G_Printf( "buster_rapid\n");
        G_Printf( "buster_charge\n");
    }
    ent->client->pers.maxArmor = ent->client->ps.persistant[PERS_MAX_ARMOR] = bg_playerlist[ent->client->pers.playerclass].maxarmor;
    ent->client->pers.maxEnergy = ent->client->ps.persistant[PERS_MAX_ENERGY] = bg_playerlist[ent->client->pers.playerclass].maxenergy;
    ent->client->pers.maxHealth = ent->client->ps.stats[STAT_MAX_HEALTH] = bg_playerlist[ent->client->pers.playerclass].maxhealth;
}

void do_where(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    int j;

    for (j = 0; j < level.maxclients; j++)
    {

        if (!g_entities[j].inuse)
            continue;
        if (!g_entities[j].client)
            continue;

        send_to_char( ent, "%s %f %f %f\n", g_entities[j].client->pers.netname,
                      g_entities[j].client->ps.origin[0],
                      g_entities[j].client->ps.origin[1], g_entities[j].client->ps.origin[2]);
    }
}

void do_escape(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    ClientBegin( ent->client - level.clients );
}

void do_invis(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if (ent->client->ps.powerups[PW_CLOAK])
    {
        send_to_char( ent, "You are now visible.\n");
        ent->client->ps.powerups[PW_CLOAK] = 0;
    }
    else
    {
        send_to_char( ent, "You are now invisible.\n");
        ent->client->ps.powerups[PW_CLOAK] = INT_MAX;
    }
}

void do_goto(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;
    vec3_t dest;

    if (!*argument)
    {
        send_to_char( ent, "Go where?\n" );
        return;
    }

    if (!(victim = get_char_world(argument)))
    {
        send_to_char( ent, "No such character.\n" );
        return;
    }

    VectorCopy (victim->client->ps.origin, dest);
    dest[2] += 100;
    TeleportPlayer(ent, dest, victim->s.angles);
}

void do_transfer(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;
    vec3_t dest;

    if (!*argument)
    {
        send_to_char( ent, "Transfer who?\n" );
        return;
    }

    if (!(victim = get_char_world(argument)))
    {
        send_to_char( ent, "No such character.\n" );
        return;
    }

    VectorCopy (ent->client->ps.origin, dest);
    dest[2] += 100;
    TeleportPlayer(victim, dest, ent->s.angles);
}

void do_restore(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;

    if (!*argument)
    {
        send_to_char( ent, "Restore who?\n" );
        return;
    }

    if (!(victim = get_char_world(argument)))
    {
        send_to_char( ent, "No such player.\n" );
        return;
    }

    victim->client->ps.stats[STAT_HEALTH] = victim->health = victim->client->pers.maxHealth;
}

void do_echo(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if (!*argument)
    {
        send_to_char(ent, "Echo what\n");
        return;
    }

    send_to_char(NULL, argument);
}

void do_voice(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    send_to_char(NULL, "The Voice of God says: %s\n", argument );
    return;
}

void do_recho(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if (!*argument)
    {
        send_to_char(ent, "Recho what\n");
        return;
    }

    send_to_room(ent, argument);
}

void do_world(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    // Get the system time
    time_t timestamp = time(0);
    char *tmstr = asctime(localtime(&timestamp));
    *(tmstr + strlen(tmstr) - 1) = '\0';

    send_to_char( ent, "^5Base source code:        ^3X-buster\n" );
    send_to_char( ent, "^5Current source revision: ^3%s\n", Q3_VERSION );
    send_to_char( ent, "^5The system time is:      ^3%s\n", tmstr);
    send_to_char( ent, "^5World date and time:     ^3%d/%d/%d %d:00\n", level.month, level.day,
                  level.year, level.hours );
    send_to_char( ent, "^5Number of players connected: ^3%d\n", num_players() );
    send_to_char( ent, "^5Number of bots loaded into the world: ^3%d\n", NumBots() );

}

void do_disconnect(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;

    if (!*argument)
    {
        send_to_char(ent, "Disconnect who?\n");
        return;
    }

    if (!(victim = get_char_world(argument)))
    {
        send_to_char(ent, "There not logged on.\n");
        return;
    }

    trap_SendServerCommand(ent->s.clientNum, "disconnect\n");

}

void do_slay(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *vict;

    if (!*argument)
    {
        send_to_char(ent, "Slay who.\n");
        return;
    }

    if (!(vict = get_char_room(ent, argument)))
    {
        send_to_char(ent, "There not here.\n");
        return;
    }


    send_to_char(ent, "You chop %s into little pieces.\n", vict->client->pers.netname);
    send_to_char(vict, "%s chops you into little pieces.\n", ent->client->pers.netname);
    G_Damage (vict, ent, ent, NULL, NULL, (vict->client->ps.stats[STAT_MAX_HEALTH]+10), 0, MOD_UNKNOWN);
}

void do_lag(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;

    if (!*argument)
    {
        send_to_char( ent, "Lag who?\n" );
        return;
    }

    if (!(victim = get_char_world(argument)) || !ent->client)
    {
        send_to_char( ent, "No such character.\n" );
        return;
    }

    if (IS_SET(ent->client->ps.eFlags, EF_LAGGED))
    {
        REMOVE_BIT(victim->client->ps.eFlags, EF_LAGGED);
        send_to_char( ent, "%s no longer lagged.\n", victim->client->pers.netname );
        return;
    }
    else
    {
        SET_BIT(victim->client->ps.eFlags, EF_LAGGED);
        send_to_char( ent, "%s lagged!\n", victim->client->pers.netname );
        ent->client->lag_time = level.time+2000;
        return;
    }
}

void do_freeze(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;

    if (!*argument)
    {
        send_to_char( ent, "Freeze who?\n" );
        return;
    }

    if (!(victim = get_char_world(argument)))
    {
        send_to_char( ent, "No such character.\n" );
        return;
    }

    if (IS_SET(ent->client->ps.eFlags, EF_FROZEN))
    {
        REMOVE_BIT(victim->client->ps.eFlags, EF_FROZEN);
        send_to_char( ent, "%s is no longer frozen.\n", victim->client->pers.netname );
        return;
    }
    else
    {
        SET_BIT(victim->client->ps.eFlags, EF_FROZEN);
        send_to_char( ent, "%s is now an frozen!\n", victim->client->pers.netname );
        return;
    }
}


void do_poofin(gentity_t *ent, char *argument, int cmd, int subcmd)
{

    if (!*argument)
    {
        send_to_char( ent, "Current poofin: %s\n", ent->poofin );
        return;
    }

    ent->poofin = strdup(argument);
}

void do_poofout(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if (!*argument)
    {
        send_to_char( ent, "Current poofout: %s\n", ent->poofin );
        return;
    }

    ent->poofout = strdup(argument);
}

void do_force(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;
    char arg1[MAX_STRING_CHARS];
    char arg2[MAX_STRING_CHARS];

    half_chop(argument, arg1, arg2);

    if (!*arg1 || !*arg2)
    {
        send_to_char( ent, "Force who to do what?\n" );
        return;
    }

    if (!(victim = get_char_world(arg1)))
    {
        send_to_char( ent, "No such character.\n" );
        return;
    }

    trap_SendConsoleCommand( EXEC_NOW, arg2 );
    send_to_char( ent, "You force %s to do %s!\n", victim->client->pers.netname, arg2 );
}


void do_wset(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    char arg1[MAX_STRING_CHARS];
    char arg2[MAX_STRING_CHARS];
    char stat[MAX_STRING_CHARS];
    char value[MAX_STRING_CHARS];
    gentity_t *victim;

    if (!*argument)
    {
        send_to_char (ent, "Set what?\n");
        return;
    }

    half_chop(argument, arg1, arg2);

    if (!strcmp(arg1, "weather"))
    {
        if (!*arg2)
        {
            send_to_char (ent, "Set the weather to what?\n");
            return;
        }
        if (!strcmp(arg2, "none"))
        {
            trap_SetConfigstring( CS_ATMOSEFFECT, "T=NONE,B=0 0,C=00,G=0,BV=0 0,GV=0 0,W=0 0,D=0" );  	  	  	// Atmospheric effect
            return;
        }
        else if (!strcmp(arg2, "rain"))
        {
            trap_SetConfigstring( CS_ATMOSEFFECT, "T=RAIN,B=5 10,C=0.5,G=0.5 2,BV=50 50,GV=200 200,W=1 2,D=2000" );  	  	  	// Atmospheric effect
            level.sky = SKY_RAINING;
            return;
        }
        else if (!strcmp(arg2, "snow"))
        {
            trap_SetConfigstring( CS_ATMOSEFFECT, "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,D=2400" );  	  	  	// Atmospheric effect
            level.sky = SKY_SNOWING;
            return;
        }
        else
        {
            send_to_char( ent, "Unknown weather type.\n" );
            return;
        }
    }
    else if (!strcmp(arg1, "sunlight"))
    {
        if (!strcmp(arg2, "sunrise"))
        {
            level.hours = 5;
        }
        else if (!strcmp(arg2, "day"))
        {
            level.hours = 15;
        }
        else if (!strcmp(arg2, "sunset"))
        {
            level.hours = 21;
        }
        else if (!strcmp(arg2, "night"))
        {
            level.hours = 22;
        }
    }
    else
    {
        if (!(victim = get_char_world(arg1)))
        {
            send_to_char(ent, "There not here.\n");
            return;
        }

        half_chop(arg2, stat, value);

        if (!is_number(value))
        {
            send_to_char( ent, "Invalid value.\n" );
            return;
        }
        else if (!strcmp(stat, "health"))
            victim->health = atoi(value);
        else if (!strcmp(stat, "maxhealth"))
            victim->client->ps.stats[STAT_MAX_HEALTH] =
                victim->client->pers.maxHealth = atoi(value);
        else if (!strcmp(stat, "credits"))
            victim->client->ps.persistant[PERS_CHIPS] = atoi(value);
        else if (!strcmp(stat, "kills"))
            victim->client->ps.persistant[PERS_KILLED] = atoi(value);
        else
        {
            send_to_char( ent, "Invalid stat.\n" );
            return;
        }

        send_to_char( ent, "^5%s's %s set to %d.\n", arg1, stat, atoi(value) );
        ClientUserinfoChanged(victim->client->ps.clientNum);
    }
}

void do_give(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gitem_t		*it;
    int			i;
    qboolean	give_all;
    gentity_t		*it_ent;
    trace_t		trace;

    if (Q_stricmp(argument, "all") == 0)
        give_all = qtrue;
    else
        give_all = qfalse;

    if (give_all || Q_stricmp( argument, "health") == 0)
    {
        ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
        if (!give_all)
            return;
    }

    if (give_all || Q_stricmp(argument, "weapons") == 0)
    {
        ent->client->ps.stats[STAT_WEAPONS] = (1 << 11) - 1/* -
			( 1 << RWP_ELECTRICAL )*/ - ( 1 << WP_NONE );
        if (!give_all)
            return;
    }

    if (give_all || Q_stricmp(argument, "ammo") == 0)
    {
        for ( i = WP_DEFAULT ; i < MAX_WEAPONS ; i++ )
        {
            ent->client->ps.ammo[i] = 800;
        }
        if (!give_all)
            return;
    }

    if (give_all || Q_stricmp(argument, "armor") == 0)
    {
        ent->client->ps.stats[STAT_ARMOR] = 200;

        if (!give_all)
            return;
    }

    // spawn a specific item right on the player
    if ( !give_all )
    {
        it = BG_FindItem (argument);
        if (!it)
        {
            return;
        }

        it_ent = G_Spawn();
        VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
        it_ent->classname = it->classname;
        G_SpawnItem (it_ent, it);
        FinishSpawningItem(it_ent );
        memset( &trace, 0, sizeof( trace ) );
        Touch_Item (it_ent, ent, &trace);
        if (it_ent->inuse)
        {
            G_FreeEntity( it_ent );
        }
    }
}

void do_snoop(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;

    if (!*argument)
    {
        if (ent->snoop->client->pers.netname)
        {
            send_to_char( ent, "You are no longer snooping %s\n", ent->snoop->client->pers.netname );
            ent->snoop = NULL;
        }
        else
            send_to_char( ent, "Snoop whom?\n" );

        return;
    }

    if (!(victim = get_char_world(argument)))
    {
        send_to_char( ent, "No such character.\n" );
        return;
    }

    ent->snoop = victim;
    send_to_char( ent, "You are now snooping %s\n", ent->snoop->client->pers.netname );
}
