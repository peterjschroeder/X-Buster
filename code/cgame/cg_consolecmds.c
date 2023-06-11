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

#include "cg_local.h"
#include "../ui/ui_shared.h"



void CG_TargetCommand_f( void )
{
    int		targetNum;
    char	test[4];

    targetNum = CG_CrosshairPlayer();
    if (!targetNum )
    {
        return;
    }

    trap_Argv( 1, test, 4 );
    trap_SendConsoleCommand( va( "gc %i %i", targetNum, atoi( test ) ) );
}



/*
=================
CG_SizeUp_f

Keybinding command
=================
*/
static void CG_SizeUp_f (void)
{
    trap_Cvar_Set("cg_viewsize", va("%i",(int)(cg_viewsize.integer+10)));
}


/*
=================
CG_SizeDown_f

Keybinding command
=================
*/
static void CG_SizeDown_f (void)
{
    trap_Cvar_Set("cg_viewsize", va("%i",(int)(cg_viewsize.integer-10)));
}


/*
=============
CG_Viewpos_f

Debugging command to print the current position
=============
*/
static void CG_Viewpos_f (void)
{
    CG_Printf( NULL, "(%i %i %i) : %i\n", (int)cg.refdef.vieworg[0],
               (int)cg.refdef.vieworg[1], (int)cg.refdef.vieworg[2],
               (int)cg.refdefViewAngles[YAW]);
}


static void CG_ScoresDown_f( void )
{

    CG_BuildSpectatorString();

    if ( cg.scoresRequestTime + 2000 < cg.time )
    {
        // the scores are more than two seconds out of data,
        // so request new ones
        cg.scoresRequestTime = cg.time;
        trap_SendClientCommand( "score" );

        // leave the current scores up if they were already
        // displayed, but if this is the first hit, clear them out
        if ( !cg.showScores )
        {
            cg.showScores = qtrue;
            cg.numScores = 0;
        }
    }
    else
    {
        // show the cached contents even if they just pressed if it
        // is within two seconds
        cg.showScores = qtrue;
    }
}

static void CG_ScoresUp_f( void )
{
    if ( cg.showScores )
    {
        cg.showScores = qfalse;
        cg.scoreFadeTime = cg.time;
    }
}


extern menuDef_t *menuScoreboard;
void Menu_Reset(void);			// FIXME: add to right include file

static void CG_LoadHud_f( void)
{
    char buff[1024];
    const char *hudSet;
    memset(buff, 0, sizeof(buff));

    String_Init();
    Menu_Reset();

    trap_Cvar_VariableStringBuffer("cg_hudFiles", buff, sizeof(buff));
    hudSet = buff;
    if (hudSet[0] == '\0')
    {
        hudSet = "ui/hud.txt";
    }

    CG_LoadMenus(hudSet);
    menuScoreboard = NULL;
}


static void CG_scrollScoresDown_f( void)
{
    if (menuScoreboard && cg.scoreBoardShowing)
    {
        Menu_ScrollFeeder(menuScoreboard, FEEDER_SCOREBOARD, qtrue);
        Menu_ScrollFeeder(menuScoreboard, FEEDER_REDTEAM_LIST, qtrue);
        Menu_ScrollFeeder(menuScoreboard, FEEDER_BLUETEAM_LIST, qtrue);
    }
}


static void CG_scrollScoresUp_f( void)
{
    if (menuScoreboard && cg.scoreBoardShowing)
    {
        Menu_ScrollFeeder(menuScoreboard, FEEDER_SCOREBOARD, qfalse);
        Menu_ScrollFeeder(menuScoreboard, FEEDER_REDTEAM_LIST, qfalse);
        Menu_ScrollFeeder(menuScoreboard, FEEDER_BLUETEAM_LIST, qfalse);
    }
}



static void CG_TellTarget_f( void )
{
    int		clientNum;
    char	command[128];
    char	message[128];

    clientNum = CG_CrosshairPlayer();
    if ( clientNum == -1 )
    {
        return;
    }

    trap_Args( message, 128 );
    Com_sprintf( command, 128, "tell %i %s", clientNum, message );
    trap_SendClientCommand( command );
}

static void CG_TellAttacker_f( void )
{
    int		clientNum;
    char	command[128];
    char	message[128];

    clientNum = CG_LastAttacker();
    if ( clientNum == -1 )
    {
        return;
    }

    trap_Args( message, 128 );
    Com_sprintf( command, 128, "tell %i %s", clientNum, message );
    trap_SendClientCommand( command );
}

static void CG_VoiceTellTarget_f( void )
{
    int		clientNum;
    char	command[128];
    char	message[128];

    clientNum = CG_CrosshairPlayer();
    if ( clientNum == -1 )
    {
        return;
    }

    trap_Args( message, 128 );
    Com_sprintf( command, 128, "vtell %i %s", clientNum, message );
    trap_SendClientCommand( command );
}

static void CG_VoiceTellAttacker_f( void )
{
    int		clientNum;
    char	command[128];
    char	message[128];

    clientNum = CG_LastAttacker();
    if ( clientNum == -1 )
    {
        return;
    }

    trap_Args( message, 128 );
    Com_sprintf( command, 128, "vtell %i %s", clientNum, message );
    trap_SendClientCommand( command );
}


static void CG_NextTeamMember_f( void )
{
    CG_SelectNextPlayer();
}

static void CG_PrevTeamMember_f( void )
{
    CG_SelectPrevPlayer();
}


/*
==================
CG_StartOrbit_f
==================
*/

static void CG_StartOrbit_f( void )
{
    char var[MAX_TOKEN_CHARS];

    trap_Cvar_VariableStringBuffer( "developer", var, sizeof( var ) );
    if ( !atoi(var) )
    {
        return;
    }
    if (cg_cameraOrbit.value != 0)
    {
        trap_Cvar_Set ("cg_cameraOrbit", "0");
        trap_Cvar_Set("cg_thirdPerson", "0");
    }
    else
    {
        trap_Cvar_Set("cg_cameraOrbit", "5");
        trap_Cvar_Set("cg_thirdPerson", "1");
        trap_Cvar_Set("cg_thirdPersonAngle", "0");
        trap_Cvar_Set("cg_thirdPersonRange", "100");
    }
}

/*
static void CG_Camera_f( void ) {
	char name[1024];
	trap_Argv( 1, name, sizeof(name));
	if (trap_loadCamera(name)) {
		cg.cameraMode = qtrue;
		trap_startCamera(cg.time);
	} else {
		CG_Printf( NULL, "Unable to load camera %s\n",name);
	}
}
*/


typedef struct
{
    char	*cmd;
    void	(*function)(void);
} consoleCommand_t;

static consoleCommand_t	commands[] =
{
    { "testgun", CG_TestGun_f },
    { "testmodel", CG_TestModel_f },
    { "nextframe", CG_TestModelNextFrame_f },
    { "prevframe", CG_TestModelPrevFrame_f },
    { "nextskin", CG_TestModelNextSkin_f },
    { "prevskin", CG_TestModelPrevSkin_f },
    { "viewpos", CG_Viewpos_f },
    { "+scores", CG_ScoresDown_f },
    { "-scores", CG_ScoresUp_f },
    { "+zoom", CG_ZoomDown_f },
    { "-zoom", CG_ZoomUp_f },
    { "sizeup", CG_SizeUp_f },
    { "sizedown", CG_SizeDown_f },
    { "weapnext", CG_NextWeapon_f },
    { "weapprev", CG_PrevWeapon_f },
    { "weapon", CG_Weapon_f },
    { "tell_target", CG_TellTarget_f },
    { "tell_attacker", CG_TellAttacker_f },
    { "vtell_target", CG_VoiceTellTarget_f },
    { "vtell_attacker", CG_VoiceTellAttacker_f },
    { "tcmd", CG_TargetCommand_f },
    { "loadhud", CG_LoadHud_f },
    { "nextTeamMember", CG_NextTeamMember_f },
    { "prevTeamMember", CG_PrevTeamMember_f },
    { "scoresDown", CG_scrollScoresDown_f },
    { "scoresUp", CG_scrollScoresUp_f },
    { "startOrbit", CG_StartOrbit_f },
    //{ "camera", CG_Camera_f },
    { "loaddeferred", CG_LoadDeferredPlayers }
};


/*
=================
CG_ConsoleCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
qboolean CG_ConsoleCommand( void )
{
    const char	*cmd;
    int		i;

    cmd = CG_Argv(0);

    for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ )
    {
        if ( !Q_stricmp( cmd, commands[i].cmd ) )
        {
            commands[i].function();
            return qtrue;
        }
    }

    return qfalse;
}


/*
=================
CG_InitConsoleCommands

Let the client system know about all of our commands
so it can perform tab completion
=================
*/
void CG_InitConsoleCommands( void )
{
    int		i;

    for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ )
    {
        trap_AddCommand( commands[i].cmd );
    }

    //
    // the game server will interpret these commands, which will be automatically
    // forwarded to the server after they are not recognized locally
    //
    trap_AddCommand ("kill");
    trap_AddCommand ("chat");
    trap_AddCommand ("say");
    trap_AddCommand ("say_team");
    trap_AddCommand ("achat");
    trap_AddCommand ("psay");
    trap_AddCommand ("tell");
    trap_AddCommand ("vsay");
    trap_AddCommand ("vsay_team");
    trap_AddCommand ("vtell");
    trap_AddCommand ("vtaunt");
    trap_AddCommand ("vosay");
    trap_AddCommand ("vosay_team");
    trap_AddCommand ("votell");
    trap_AddCommand ("give");
    trap_AddCommand ("god");
    trap_AddCommand ("notarget");
    trap_AddCommand ("noclip");
    trap_AddCommand ("team");
    trap_AddCommand ("follow");
    trap_AddCommand ("levelshot");
    trap_AddCommand ("addbot");
    trap_AddCommand ("setviewpos");
    trap_AddCommand ("callvote");
    trap_AddCommand ("vote");
    trap_AddCommand ("callteamvote");
    trap_AddCommand ("teamvote");
    trap_AddCommand ("stats");
    trap_AddCommand ("teamtask");
    trap_AddCommand ("loaddefered");	// spelled wrong, but not changing for demo
    trap_AddCommand("lua_script");
    trap_AddCommand("lua_binaryfunction");
    trap_AddCommand("lua_stackdump");
    trap_AddCommand("lua_restart");
}
