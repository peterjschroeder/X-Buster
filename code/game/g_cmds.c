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

#include "g_local.h"


/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage( gentity_t *ent )
{
    char		entry[1024];
    char		string[1400];
    int			stringlength;
    int			i, j;
    gclient_t	*cl;
    int			numSorted, scoreFlags, accuracy, perfect;

    // send the latest information on all clients
    string[0] = 0;
    stringlength = 0;
    scoreFlags = 0;

    numSorted = level.numConnectedClients;

    for (i=0 ; i < numSorted ; i++)
    {
        int		ping;

        cl = &level.clients[level.sortedClients[i]];

        if ( cl->pers.connected == CON_CONNECTING )
        {
            ping = -1;
        }
        else
        {
            ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
        }

        if( cl->accuracy_shots )
        {
            accuracy = cl->accuracy_hits * 100 / cl->accuracy_shots;
        }
        else
        {
            accuracy = 0;
        }
        perfect = ( cl->ps.persistant[PERS_RANK] == 0 && cl->ps.persistant[PERS_KILLED] == 0 ) ? 1 : 0;

        Com_sprintf (entry, sizeof(entry),
                     " %i %i %i %i %i %i %i %i %i", level.sortedClients[i],
                     cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime)/60000,
                     scoreFlags, g_entities[level.sortedClients[i]].s.powerups, accuracy,
                     perfect,
                     cl->ps.persistant[PERS_CAPTURES]);

        j = strlen(entry);
        if (stringlength + j > 1024)
            break;
        strcpy (string + stringlength, entry);
        stringlength += j;
    }

    trap_SendServerCommand( ent-g_entities, va("scores %i %i %i%s", i,
                            level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE],
                            string ) );
}


/*
==================
do_score

Request current scoreboard information
==================
*/
void do_score(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    DeathmatchScoreboardMessage( ent );
}


void do_teamtask(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    char userinfo[MAX_INFO_STRING];
    int task;
    int client = ent->client - level.clients;

    if ( !*argument )
    {
        return;
    }

    task = atoi( argument );

    trap_GetUserinfo(client, userinfo, sizeof(userinfo));
    Info_SetValueForKey(userinfo, "teamtask", va("%d", task));
    trap_SetUserinfo(client, userinfo);
    ClientUserinfoChanged(client);
}



void do_kill(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        return;
    }
    if (ent->health <= 0)
    {
        return;
    }
    ent->flags &= ~FL_GODMODE;
    ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
    player_die (ent, ent, ent, 100000, MOD_SUICIDE);
}

/*
=================
BroadCastTeamChange

Let everyone know about a team change
=================
*/
void BroadcastTeamChange( gclient_t *client, int oldTeam )
{
    if ( client->sess.sessionTeam == TEAM_RED )
    {
        trap_SendServerCommand( -1, va("cp \"%s" S_COLOR_WHITE " joined the red team.\n\"",
                                       client->pers.netname) );
    }
    else if ( client->sess.sessionTeam == TEAM_BLUE )
    {
        trap_SendServerCommand( -1, va("cp \"%s" S_COLOR_WHITE " joined the blue team.\n\"",
                                       client->pers.netname));
    }
    else if ( client->sess.sessionTeam == TEAM_SPECTATOR && oldTeam != TEAM_SPECTATOR )
    {
        trap_SendServerCommand( -1, va("cp \"%s" S_COLOR_WHITE " joined the spectators.\n\"",
                                       client->pers.netname));
    }
    else if ( client->sess.sessionTeam == TEAM_FREE )
    {
        trap_SendServerCommand( -1, va("cp \"%s" S_COLOR_WHITE " joined the battle.\n\"",
                                       client->pers.netname));
    }
}

/*
=================
SetTeam
=================
*/
void SetTeam( gentity_t *ent, char *s )
{
    int					team, oldTeam;
    gclient_t			*client;
    int					clientNum;
    spectatorState_t	specState;
    int					specClient;
    int					teamLeader;

    //
    // see what change is requested
    //
    client = ent->client;

    clientNum = client - level.clients;
    specClient = 0;
    specState = SPECTATOR_NOT;
    if ( !Q_stricmp( s, "scoreboard" ) || !Q_stricmp( s, "score" )  )
    {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_SCOREBOARD;
    }
    else if ( !Q_stricmp( s, "follow1" ) )
    {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_FOLLOW;
        specClient = -1;
    }
    else if ( !Q_stricmp( s, "follow2" ) )
    {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_FOLLOW;
        specClient = -2;
    }
    else if ( !Q_stricmp( s, "spectator" ) || !Q_stricmp( s, "s" ) )
    {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_FREE;
    }
    else if ( g_gametype.integer >= GT_TEAM )
    {
        // if running a team game, assign player to one of the teams
        specState = SPECTATOR_NOT;
        if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) )
        {
            team = TEAM_RED;
        }
        else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) )
        {
            team = TEAM_BLUE;
        }
        else
        {
            // pick the team with the least number of players
            team = PickTeam( clientNum );
        }

        if ( g_teamForceBalance.integer  )
        {
            int		counts[TEAM_NUM_TEAMS];

            counts[TEAM_BLUE] = TeamCount( ent->client->ps.clientNum, TEAM_BLUE );
            counts[TEAM_RED] = TeamCount( ent->client->ps.clientNum, TEAM_RED );

            // We allow a spread of two
            if ( team == TEAM_RED && counts[TEAM_RED] - counts[TEAM_BLUE] > 1 )
            {
                trap_SendServerCommand( ent->client->ps.clientNum,
                                        "cp \"Red team has too many players.\n\"" );
                return; // ignore the request
            }
            if ( team == TEAM_BLUE && counts[TEAM_BLUE] - counts[TEAM_RED] > 1 )
            {
                trap_SendServerCommand( ent->client->ps.clientNum,
                                        "cp \"Blue team has too many players.\n\"" );
                return; // ignore the request
            }

            // It's ok, the team we are switching to has less or same number of players
        }

    }
    else
    {
        // force them to spectators if there aren't any spots free
        team = TEAM_FREE;
    }

    // override decision if limiting the players
    if ( (g_gametype.integer == GT_TOURNAMENT)
            && level.numNonSpectatorClients >= 2 )
    {
        team = TEAM_SPECTATOR;
    }
    else if ( g_maxGameClients.integer > 0 &&
              level.numNonSpectatorClients >= g_maxGameClients.integer )
    {
        team = TEAM_SPECTATOR;
    }

    //
    // decide if we will allow the change
    //
    oldTeam = client->sess.sessionTeam;
    if ( team == oldTeam && team != TEAM_SPECTATOR )
    {
        return;
    }

    //
    // execute the team change
    //

    // if the player was dead leave the body
    if ( client->ps.stats[STAT_HEALTH] <= 0 )
    {
        CopyToBodyQue(ent);
    }

    // he starts at 'base'
    client->pers.teamState.state = TEAM_BEGIN;
    if ( oldTeam != TEAM_SPECTATOR )
    {
        // Kill him (makes sure he loses flags, etc)
        ent->flags &= ~FL_GODMODE;
        ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
        player_die (ent, ent, ent, 100000, MOD_SUICIDE);

    }
    // they go to the end of the line for tournements
    if ( team == TEAM_SPECTATOR )
    {
        client->sess.spectatorTime = level.time;
    }

    client->sess.sessionTeam = team;
    client->sess.spectatorState = specState;
    client->sess.spectatorClient = specClient;

    client->sess.teamLeader = qfalse;
    if ( team == TEAM_RED || team == TEAM_BLUE )
    {
        teamLeader = TeamLeader( team );
        // if there is no team leader or the team leader is a bot and this client is not a bot
        if ( teamLeader == -1 || ( !(g_entities[clientNum].r.svFlags & SVF_BOT) && (g_entities[teamLeader].r.svFlags & SVF_BOT) ) )
        {
            SetLeader( team, clientNum );
        }
    }
    // make sure there is a team leader on the team the player came from
    if ( oldTeam == TEAM_RED || oldTeam == TEAM_BLUE )
    {
        CheckTeamLeader( oldTeam );
    }

    BroadcastTeamChange( client, oldTeam );

    // get and distribute relevent paramters
    ClientUserinfoChanged( clientNum );

    ClientBegin( clientNum );
}

/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void StopFollowing( gentity_t *ent )
{
    ent->client->ps.persistant[ PERS_TEAM ] = TEAM_SPECTATOR;
    ent->client->sess.sessionTeam = TEAM_SPECTATOR;
    ent->client->sess.spectatorState = SPECTATOR_FREE;
    ent->client->ps.pm_flags &= ~PMF_FOLLOW;
    ent->r.svFlags &= ~SVF_BOT;
    ent->client->ps.clientNum = ent - g_entities;
}


void do_team(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    int			oldTeam;

    if ( !*argument )
    {
        oldTeam = ent->client->sess.sessionTeam;
        switch ( oldTeam )
        {
        case TEAM_BLUE:
            trap_SendServerCommand( ent-g_entities, "print \"Blue team\n\"" );
            break;
        case TEAM_RED:
            trap_SendServerCommand( ent-g_entities, "print \"Red team\n\"" );
            break;
        case TEAM_FREE:
            trap_SendServerCommand( ent-g_entities, "print \"Free team\n\"" );
            break;
        case TEAM_SPECTATOR:
            trap_SendServerCommand( ent-g_entities, "print \"Spectator team\n\"" );
            break;
        }
        return;
    }

    if ( ent->client->switchTeamTime > level.time )
    {
        trap_SendServerCommand( ent-g_entities, "print \"May not switch teams more than once per 5 seconds.\n\"" );
        return;
    }

    // if they are playing a tournement game, count as a loss
    if ( (g_gametype.integer == GT_TOURNAMENT )
            && ent->client->sess.sessionTeam == TEAM_FREE )
    {
        ent->client->sess.losses++;
    }


    SetTeam( ent, argument );

    ent->client->switchTeamTime = level.time + 5000;
}



void do_follow(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    int		i;

    if ( !*argument )
    {
        if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
        {
            StopFollowing( ent );
        }
        return;
    }

    i = ClientNumberFromString( ent, argument );
    if ( i == -1 )
    {
        return;
    }

    // can't follow self
    if ( &level.clients[ i ] == ent->client )
    {
        return;
    }

    // can't follow another spectator
    if ( level.clients[ i ].sess.sessionTeam == TEAM_SPECTATOR )
    {
        return;
    }

    // if they are playing a tournement game, count as a loss
    if ( (g_gametype.integer == GT_TOURNAMENT )
            && ent->client->sess.sessionTeam == TEAM_FREE )
    {
        ent->client->sess.losses++;
    }

    // first set them to spectator
    if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR )
    {
        SetTeam( ent, "spectator" );
    }

    ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
    ent->client->sess.spectatorClient = i;
}


void do_followcycle(gentity_t *ent, char *argument, int cmd, int dir)
{
    int		clientnum;
    int		original;

    // if they are playing a tournement game, count as a loss
    if ( (g_gametype.integer == GT_TOURNAMENT )
            && ent->client->sess.sessionTeam == TEAM_FREE )
    {
        ent->client->sess.losses++;
    }
    // first set them to spectator
    if ( ent->client->sess.spectatorState == SPECTATOR_NOT )
    {
        SetTeam( ent, "spectator" );
    }

    if ( dir != 1 && dir != -1 )
    {
        G_Error( "do_followCycle: bad dir %i", dir );
    }

    clientnum = ent->client->sess.spectatorClient;
    original = clientnum;
    do
    {
        clientnum += dir;
        if ( clientnum >= level.maxclients )
        {
            clientnum = 0;
        }
        if ( clientnum < 0 )
        {
            clientnum = level.maxclients - 1;
        }

        // can only follow connected clients
        if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED )
        {
            continue;
        }

        // can't follow another spectator
        if ( level.clients[ clientnum ].sess.sessionTeam == TEAM_SPECTATOR )
        {
            continue;
        }

        // this is good, we can use it
        ent->client->sess.spectatorClient = clientnum;
        ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
        return;
    }
    while ( clientnum != original );

    // leave it where it was
}






static char	*gc_orders[] =
{
    "hold your position",
    "hold this position",
    "come here",
    "cover me",
    "guard location",
    "search and destroy",
    "report"
};

void Cmd_GameCommand_f( gentity_t *ent )
{
    int		player;
    int		order;
    char	str[MAX_TOKEN_CHARS];

    trap_Argv( 1, str, sizeof( str ) );
    player = atoi( str );
    trap_Argv( 2, str, sizeof( str ) );
    order = atoi( str );

    if ( player < 0 || player >= MAX_CLIENTS )
    {
        return;
    }
    if ( order < 0 || order > sizeof(gc_orders)/sizeof(char *) )
    {
        return;
    }
    G_Say( ent, &g_entities[player], SAY_TELL, gc_orders[order] );
    G_Say( ent, ent, SAY_TELL, gc_orders[order] );
}



static const char *gameNames[] =
{
    "Free For All",
    "Tournament",
    "Single Player",
    "Team Deathmatch",
    "Capture the Flag"
};


void do_callvote(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    int		i;
    char	arg1[MAX_STRING_TOKENS];
    char	arg2[MAX_STRING_TOKENS];

    if ( !g_allowVote.integer )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
        return;
    }

    if ( level.voteTime )
    {
        trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
        return;
    }
    if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT )
    {
        trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of votes.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
        return;
    }

    // make sure it is a valid command to vote on
    half_chop(argument, arg1, arg2);

    if( strchr( arg1, ';' ) || strchr( arg2, ';' ) )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
        return;
    }

    if ( !Q_stricmp( arg1, "map_restart" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "nextmap" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "map" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "g_gametype" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "kick" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "clientkick" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "g_doWarmup" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "timelimit" ) )
    {
    }
    else if ( !Q_stricmp( arg1, "fraglimit" ) )
    {
    }
    else
    {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
        trap_SendServerCommand( ent-g_entities, "print \"Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, fraglimit <frags>.\n\"" );
        return;
    }

    // if there is still a vote to be executed
    if ( level.voteExecuteTime )
    {
        level.voteExecuteTime = 0;
        trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
    }

    // special case for g_gametype, check for bad values
    if ( !Q_stricmp( arg1, "g_gametype" ) )
    {
        i = atoi( arg2 );
        if( i == GT_SINGLE_PLAYER || i < GT_FFA || i >= GT_MAX_GAME_TYPE)
        {
            trap_SendServerCommand( ent-g_entities, "print \"Invalid gametype.\n\"" );
            return;
        }

        Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %d", arg1, i );
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s %s", arg1, gameNames[i] );
    }
    else if ( !Q_stricmp( arg1, "map" ) )
    {
        // special case for map changes, we want to reset the nextmap setting
        // this allows a player to change maps, but not upset the map rotation
        char	s[MAX_STRING_CHARS];

        trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
        if (*s)
        {
            Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s; set nextmap \"%s\"", arg1, arg2, s );
        }
        else
        {
            Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
        }
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
    }
    else if ( !Q_stricmp( arg1, "nextmap" ) )
    {
        char	s[MAX_STRING_CHARS];

        trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
        if (!*s)
        {
            trap_SendServerCommand( ent-g_entities, "print \"nextmap not set.\n\"" );
            return;
        }
        Com_sprintf( level.voteString, sizeof( level.voteString ), "vstr nextmap");
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
    }
    else
    {
        Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
    }

    trap_SendServerCommand( -1, va("print \"%s called a vote.\n\"", ent->client->pers.netname ) );

    // start the voting, the caller autoamtically votes yes
    level.voteTime = level.time;
    level.voteYes = 1;
    level.voteNo = 0;

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        level.clients[i].ps.eFlags &= ~EF_VOTED;
    }
    ent->client->ps.eFlags |= EF_VOTED;

    trap_SetConfigstring( CS_VOTE_TIME, va("%i", level.voteTime ) );
    trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
    trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
    trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );
}


void do_vote(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if ( !level.voteTime )
    {
        trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
        return;
    }
    if ( ent->client->ps.eFlags & EF_VOTED )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
        return;
    }

    trap_SendServerCommand( ent-g_entities, "print \"Vote cast.\n\"" );

    ent->client->ps.eFlags |= EF_VOTED;

    if ( argument[0] == 'y' || argument[1] == 'Y' || argument[1] == '1' )
    {
        level.voteYes++;
        trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
    }
    else
    {
        level.voteNo++;
        trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );
    }

    // a majority will be determined in CheckVote, which will also account
    // for players entering or leaving
}


void do_callteamvote(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    int		i, team, cs_offset;
    char	arg1[MAX_STRING_TOKENS];
    char	arg2[MAX_STRING_TOKENS];

    team = ent->client->sess.sessionTeam;
    if ( team == TEAM_RED )
        cs_offset = 0;
    else if ( team == TEAM_BLUE )
        cs_offset = 1;
    else
        return;

    if ( !g_allowVote.integer )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
        return;
    }

    if ( level.teamVoteTime[cs_offset] )
    {
        trap_SendServerCommand( ent-g_entities, "print \"A team vote is already in progress.\n\"" );
        return;
    }
    if ( ent->client->pers.teamVoteCount >= MAX_VOTE_COUNT )
    {
        trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of team votes.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
        return;
    }

    // make sure it is a valid command to vote on
    half_chop(argument, arg1, arg2);

    if( strchr( arg1, ';' ) || strchr( arg2, ';' ) )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
        return;
    }

    if ( !Q_stricmp( arg1, "leader" ) )
    {
        char netname[MAX_NETNAME], leader[MAX_NETNAME];

        if ( !arg2[0] )
        {
            i = ent->client->ps.clientNum;
        }
        else
        {
            // numeric values are just slot numbers
            for (i = 0; i < 3; i++)
            {
                if ( !arg2[i] || arg2[i] < '0' || arg2[i] > '9' )
                    break;
            }
            if ( i >= 3 || !arg2[i])
            {
                i = atoi( arg2 );
                if ( i < 0 || i >= level.maxclients )
                {
                    trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %i\n\"", i) );
                    return;
                }

                if ( !g_entities[i].inuse )
                {
                    trap_SendServerCommand( ent-g_entities, va("print \"Client %i is not active\n\"", i) );
                    return;
                }
            }
            else
            {
                Q_strncpyz(leader, arg2, sizeof(leader));
                Q_CleanStr(leader);
                for ( i = 0 ; i < level.maxclients ; i++ )
                {
                    if ( level.clients[i].pers.connected == CON_DISCONNECTED )
                        continue;
                    if (level.clients[i].sess.sessionTeam != team)
                        continue;
                    Q_strncpyz(netname, level.clients[i].pers.netname, sizeof(netname));
                    Q_CleanStr(netname);
                    if ( !Q_stricmp(netname, leader) )
                    {
                        break;
                    }
                }
                if ( i >= level.maxclients )
                {
                    trap_SendServerCommand( ent-g_entities, va("print \"%s is not a valid player on your team.\n\"", arg2) );
                    return;
                }
            }
        }
        Com_sprintf(arg2, sizeof(arg2), "%d", i);
    }
    else
    {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
        trap_SendServerCommand( ent-g_entities, "print \"Team vote commands are: leader <player>.\n\"" );
        return;
    }

    Com_sprintf( level.teamVoteString[cs_offset], sizeof( level.teamVoteString[cs_offset] ), "%s %s", arg1, arg2 );

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if ( level.clients[i].pers.connected == CON_DISCONNECTED )
            continue;
        if (level.clients[i].sess.sessionTeam == team)
            trap_SendServerCommand( i, va("print \"%s called a team vote.\n\"", ent->client->pers.netname ) );
    }

    // start the voting, the caller autoamtically votes yes
    level.teamVoteTime[cs_offset] = level.time;
    level.teamVoteYes[cs_offset] = 1;
    level.teamVoteNo[cs_offset] = 0;

    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        if (level.clients[i].sess.sessionTeam == team)
            level.clients[i].ps.eFlags &= ~EF_TEAMVOTED;
    }
    ent->client->ps.eFlags |= EF_TEAMVOTED;

    trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, va("%i", level.teamVoteTime[cs_offset] ) );
    trap_SetConfigstring( CS_TEAMVOTE_STRING + cs_offset, level.teamVoteString[cs_offset] );
    trap_SetConfigstring( CS_TEAMVOTE_YES + cs_offset, va("%i", level.teamVoteYes[cs_offset] ) );
    trap_SetConfigstring( CS_TEAMVOTE_NO + cs_offset, va("%i", level.teamVoteNo[cs_offset] ) );
}


void do_teamvote(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    int			team, cs_offset;

    team = ent->client->sess.sessionTeam;
    if ( team == TEAM_RED )
        cs_offset = 0;
    else if ( team == TEAM_BLUE )
        cs_offset = 1;
    else
        return;

    if ( !level.teamVoteTime[cs_offset] )
    {
        trap_SendServerCommand( ent-g_entities, "print \"No team vote in progress.\n\"" );
        return;
    }
    if ( ent->client->ps.eFlags & EF_TEAMVOTED )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Team vote already cast.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
        return;
    }

    trap_SendServerCommand( ent-g_entities, "print \"Team vote cast.\n\"" );

    ent->client->ps.eFlags |= EF_TEAMVOTED;

    if ( argument[0] == 'y' || argument[1] == 'Y' || argument[1] == '1' )
    {
        level.teamVoteYes[cs_offset]++;
        trap_SetConfigstring( CS_TEAMVOTE_YES + cs_offset, va("%i", level.teamVoteYes[cs_offset] ) );
    }
    else
    {
        level.teamVoteNo[cs_offset]++;
        trap_SetConfigstring( CS_TEAMVOTE_NO + cs_offset, va("%i", level.teamVoteNo[cs_offset] ) );
    }

    // a majority will be determined in TeamCheckVote, which will also account
    // for players entering or leaving
}




const struct command_info cmd_info[] =
{
    { "callteamvote",	do_callteamvote,		qfalse,			0				},
    { "callvote",		do_callvote,			qfalse,			0				},
    { "chat",			do_say,					qfalse,			SAY_CHAT		},
    { "cloak",		    do_cloak,				qfalse,			0				},
    { "commands",		do_commands,			qfalse,			0				},

    { "disconnect",		do_disconnect,			qtrue,			0				},
    { "downgrade",		do_downgrade,			qtrue,			0				},
    { "drag",			do_drag,				qfalse,			0				},

    { "echo",			do_echo,				qtrue,			0				},
    { "escape",			do_escape,				qtrue,			0				},

    { "follow",			do_follow,				qfalse,			0				},
    { "follownext",		do_followcycle,			qfalse,			1				},
    { "followprev",		do_followcycle,			qfalse,			-1				},
    { "force",			do_force,				qtrue,			0				},
    { "freeze",			do_freeze,				qtrue,			0				},

    { "give",		    do_give,				qtrue,			0				},
    { "goto",		    do_goto,				qtrue,			0				},

    { "ignore",			do_ignore,				qfalse,			0				},
    { "invis",		    do_invis,				qtrue,			0				},

    { "kill",			do_kill,		        qfalse,			0				},

    { "lag",			do_lag,					qtrue,			0				},
    { "levelshot",		do_levelshot,			qtrue,			0				},
    { "noclip",		    do_noclip,				qtrue,			0				},
    { "notarget",		do_notarget,		    qtrue,			0				},

    { "padd",			do_private_channel,		qfalse,			PRIVATE_ADD		},
    { "pclose",			do_private_channel,		qfalse,			PRIVATE_CLOSE	},
    { "phelp",			do_private_channel,		qfalse,			PRIVATE_HELP	},
    { "popen",			do_private_channel,		qfalse,			PRIVATE_OPEN	},
    { "premove",  		do_private_channel,		qfalse,			PRIVATE_REMOVE	},
    { "psay",  			do_say,		0,							SAY_PRIVATE		},
    { "pwho",			do_private_channel,		qfalse,			PRIVATE_WHO		},
    { "pcheck",			do_private_channel,		qtrue,			PRIVATE_CHECK	},
    { "poofin",		    do_poofin,				qtrue,			0				},
    { "poofout",		do_poofout,				qtrue,			0				},

    { "recho",		    do_recho,				qtrue,			0				},
    { "restore",		do_restore,				qtrue,			0				},

    { "say",			do_say,					qfalse,			SAY_SAY			},
    { "say_team",		do_say,					qfalse,			SAY_TEAM		},
    { "setviewpos",		do_setviewpos,			qtrue,			0				},
    { "slay",			do_slay,				qtrue,			0				},
    { "score",			do_score,				qfalse,			0				},
    { "shove",			do_shove,				qfalse,			0				},
    { "snoop",			do_snoop,				qtrue,			0				},

    { "team",			do_team,				qfalse,			0				},
    { "teamtask",		do_teamtask,			qfalse,			0				},
    { "teamvote",		do_teamvote,			qfalse,			0				},
    { "tell",			do_tell,				qfalse,			0				},
    { "transfer",		do_transfer,			qtrue,			0				},

    { "unignore",		do_unignore,			qfalse,			0				},
    { "upgrade",		do_upgrade,				qfalse,			0				},

    { "voice",		    do_voice,				qtrue,			0				},
    { "vote",			do_vote,				qfalse,			0				},

    { "where",		    do_where,				qtrue,			0				},
    { "world",			do_world,				qtrue,			0				},
    { "wset",			do_wset,				qtrue,			0				},

    { "\n", 0, 0, 0 }
};

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum )
{
    gentity_t *ent;
    char	cmd[128];
    char	cmd2[128];
    int command, length;

    ent = g_entities + clientNum;

    if (!ent->client)
        return;		// not fully in game yet

    trap_Argv( 0, cmd, 128 );
    trap_Args( cmd2, 128 );

    /* find the command */
    for (length = strlen(cmd), command = 0; *cmd_info[command].command != '\n'; command++)
    {
        if (cmd_info[command].cheat_command && !g_cheats.integer)
            continue;

        if (!strncmp(cmd_info[command].command, cmd, length))
            break;
    }

    if (*cmd_info[command].command == '\n')
        do_say( ent, va("%s %s", cmd, cmd2), 0, 0 );
    else if (cmd_info[command].command_pointer == NULL)
        send_to_char( ent, "Sorry, that command hasn't been implemented yet.\n" );
    else
        ((*cmd_info[command].command_pointer) (ent, cmd2, command, cmd_info[command].subcmd));

}

void do_commands( gentity_t *ent, char *argument, int cmd, int subcmd )
{
    int no, cmd_num;

    send_to_char( ent, "The following commands are available to you:\n");

    /* cmd_num starts at 1, not 0, to remove 'RESERVED' */
    for (no = 1, cmd_num = 1; cmd_info[cmd_num].command[0] != '\n'; cmd_num++)
    {

        if (cmd_info[cmd_num].cheat_command && !g_cheats.integer  )
            continue;


        send_to_char( ent, "%-11s%s", cmd_info[cmd_num].command, no++ % 7 == 0 ? "\n" : "");
    }

    if (no % 7 != 1)
        send_to_char( ent, "\n");
}
