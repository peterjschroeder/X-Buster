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
// display context for new ui stuff
displayContextDef_t cgDC;
int HudGroupFlag;

int forceModelModificationCount = -1;

void CG_Init( int serverMessageNum, int serverCommandSequence, int clientNum, int randomSeed );
void CG_Shutdown( void );


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  )
{

    switch ( command )
    {
    case CG_INIT:
        CG_Init( arg0, arg1, arg2, arg3 );
        return 0;
    case CG_SHUTDOWN:
        CG_Shutdown();
        return 0;
    case CG_CONSOLE_COMMAND:
        return CG_ConsoleCommand();
    case CG_DRAW_ACTIVE_FRAME:
        CG_DrawActiveFrame( arg0, arg1, arg2 );
        return 0;
    case CG_CROSSHAIR_PLAYER:
        return CG_CrosshairPlayer();
    case CG_LAST_ATTACKER:
        return CG_LastAttacker();
    case CG_KEY_EVENT:
        CG_KeyEvent(arg0, arg1);
        return 0;
    case CG_MOUSE_EVENT:
        cgDC.cursorx = cgs.cursorX;
        cgDC.cursory = cgs.cursorY;
        CG_MouseEvent(arg0, arg1);
        return 0;
    case CG_EVENT_HANDLING:
        CG_EventHandling(arg0);
        return 0;
    default:
        CG_Error( "vmMain: unknown command %i", command );
        break;
    }
    return -1;
}


cg_t				cg;
cgs_t				cgs;
centity_t			cg_entities[MAX_GENTITIES];
weaponInfo_t		cg_weapons[WP_NUM_WEAPONS];
itemInfo_t			cg_items[MAX_ITEMS];
npcInfo_t			cg_npcs[NPC_NUMNPCS];

vmCvar_t	cg_railTrailTime;
vmCvar_t	cg_centertime;
vmCvar_t	cg_runpitch;
vmCvar_t	cg_runroll;
vmCvar_t	cg_bobup;
vmCvar_t	cg_bobpitch;
vmCvar_t	cg_bobroll;
vmCvar_t	cg_swingSpeed;
vmCvar_t	cg_shadows;
vmCvar_t	cg_gibs;
vmCvar_t	cg_drawTimer;
vmCvar_t	cg_drawFPS;
vmCvar_t	cg_drawSnapshot;
vmCvar_t	cg_draw3dIcons;
vmCvar_t	cg_drawIcons;
vmCvar_t	cg_drawAmmoWarning;
vmCvar_t	cg_drawCrosshair;
vmCvar_t	cg_drawCrosshairNames;
vmCvar_t	cg_drawRewards;
vmCvar_t	cg_crosshairSize;
vmCvar_t	cg_crosshairX;
vmCvar_t	cg_crosshairY;
vmCvar_t	cg_crosshairHealth;
vmCvar_t	cg_draw2D;
vmCvar_t	cg_drawStatus;
vmCvar_t	cg_animSpeed;
vmCvar_t	cg_debugAnim;
vmCvar_t	cg_debugPosition;
vmCvar_t	cg_debugEvents;
vmCvar_t	cg_errorDecay;
vmCvar_t	cg_nopredict;
vmCvar_t	cg_noPlayerAnims;
vmCvar_t	cg_showmiss;
vmCvar_t	cg_footsteps;
vmCvar_t	cg_addMarks;
vmCvar_t	cg_brassTime;
vmCvar_t	cg_viewsize;
vmCvar_t	cg_drawGun;
vmCvar_t	cg_gun_frame;
vmCvar_t	cg_gun_x;
vmCvar_t	cg_gun_y;
vmCvar_t	cg_gun_z;
vmCvar_t	cg_tracerChance;
vmCvar_t	cg_tracerWidth;
vmCvar_t	cg_tracerLength;
vmCvar_t	cg_autoswitch;
vmCvar_t	cg_ignore;
vmCvar_t	cg_fov;
vmCvar_t	cg_zoomFov;
vmCvar_t	cg_thirdPerson;
vmCvar_t	cg_thirdPersonRange;
vmCvar_t	cg_thirdPersonAngle;
vmCvar_t	cg_stereoSeparation;
vmCvar_t	cg_lagometer;
vmCvar_t	cg_drawAttacker;
vmCvar_t	cg_synchronousClients;
vmCvar_t 	cg_teamChatTime;
vmCvar_t 	cg_teamChatHeight;
vmCvar_t 	cg_stats;
vmCvar_t 	cg_buildScript;
vmCvar_t 	cg_forceModel;
vmCvar_t	cg_paused;
vmCvar_t	cg_predictItems;
vmCvar_t	cg_deferPlayers;
vmCvar_t	cg_drawTeamOverlay;
vmCvar_t	cg_teamOverlayUserinfo;
vmCvar_t	cg_drawFriend;
vmCvar_t	cg_teamChatsOnly;
vmCvar_t	cg_hudFiles;
vmCvar_t 	cg_scorePlum;
vmCvar_t 	cg_smoothClients;
vmCvar_t	pmove_fixed;
//vmCvar_t	cg_pmove_fixed;
vmCvar_t	pmove_msec;
vmCvar_t	cg_pmove_msec;
vmCvar_t	cg_cameraMode;
vmCvar_t	cg_cameraOrbit;
vmCvar_t	cg_cameraOrbitDelay;
vmCvar_t	cg_timescaleFadeEnd;
vmCvar_t	cg_timescaleFadeSpeed;
vmCvar_t	cg_timescale;
vmCvar_t	cg_smallFont;
vmCvar_t	cg_bigFont;
vmCvar_t	cg_noTaunt;
vmCvar_t	cg_noProjectileTrail;
vmCvar_t	cg_oldRail;
vmCvar_t	cg_oldRocket;
vmCvar_t	cg_oldPlasma;
vmCvar_t	cg_trueLightning;
vmCvar_t 	cg_redTeamName;
vmCvar_t 	cg_blueTeamName;
vmCvar_t	cg_currentSelectedPlayer;
vmCvar_t    cg_consoleLatency;

vmCvar_t	cg_celShaded;
vmCvar_t	cg_color;
vmCvar_t	cg_atmosphericEffects;
vmCvar_t	cg_lowEffects;

typedef struct
{
    vmCvar_t	*vmCvar;
    char		*cvarName;
    char		*defaultString;
    int			cvarFlags;
} cvarTable_t;

static cvarTable_t cvarTable[] =   // bk001129
{
    { &cg_ignore, "cg_ignore", "0", 0 },	// used for debugging
    { &cg_autoswitch, "cg_autoswitch", "1", CVAR_ARCHIVE },
    { &cg_drawGun, "cg_drawGun", "1", CVAR_ARCHIVE },
    { &cg_zoomFov, "cg_zoomfov", "22.5", CVAR_ARCHIVE },
    { &cg_fov, "cg_fov", "90", CVAR_ARCHIVE },
    { &cg_viewsize, "cg_viewsize", "100", CVAR_ARCHIVE },
    { &cg_stereoSeparation, "cg_stereoSeparation", "0.4", CVAR_ARCHIVE  },
    { &cg_shadows, "cg_shadows", "1", CVAR_ARCHIVE  },
    { &cg_gibs, "cg_gibs", "1", CVAR_ARCHIVE  },
    { &cg_draw2D, "cg_draw2D", "1", CVAR_ARCHIVE  },
    { &cg_drawStatus, "cg_drawStatus", "1", CVAR_ARCHIVE  },
    { &cg_drawTimer, "cg_drawTimer", "0", CVAR_ARCHIVE  },
    { &cg_drawFPS, "cg_drawFPS", "0", CVAR_ARCHIVE  },
    { &cg_drawSnapshot, "cg_drawSnapshot", "0", CVAR_ARCHIVE  },
    { &cg_draw3dIcons, "cg_draw3dIcons", "1", CVAR_ARCHIVE  },
    { &cg_drawIcons, "cg_drawIcons", "1", CVAR_ARCHIVE  },
    { &cg_drawAmmoWarning, "cg_drawAmmoWarning", "1", CVAR_ARCHIVE  },
    { &cg_drawAttacker, "cg_drawAttacker", "1", CVAR_ARCHIVE  },
    { &cg_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE },
    { &cg_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
    { &cg_drawRewards, "cg_drawRewards", "1", CVAR_ARCHIVE },
    { &cg_crosshairSize, "cg_crosshairSize", "24", CVAR_ARCHIVE },
    { &cg_crosshairHealth, "cg_crosshairHealth", "1", CVAR_ARCHIVE },
    { &cg_crosshairX, "cg_crosshairX", "0", CVAR_ARCHIVE },
    { &cg_crosshairY, "cg_crosshairY", "0", CVAR_ARCHIVE },
    { &cg_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE },
    { &cg_addMarks, "cg_marks", "1", CVAR_ARCHIVE },
    { &cg_lagometer, "cg_lagometer", "1", CVAR_ARCHIVE },
    { &cg_railTrailTime, "cg_railTrailTime", "400", CVAR_ARCHIVE  },
    { &cg_gun_x, "cg_gunX", "0", CVAR_CHEAT },
    { &cg_gun_y, "cg_gunY", "0", CVAR_CHEAT },
    { &cg_gun_z, "cg_gunZ", "0", CVAR_CHEAT },
    { &cg_centertime, "cg_centertime", "3", CVAR_CHEAT },
    { &cg_runpitch, "cg_runpitch", "0.002", CVAR_ARCHIVE},
    { &cg_runroll, "cg_runroll", "0.005", CVAR_ARCHIVE },
    { &cg_bobup , "cg_bobup", "0.005", CVAR_CHEAT },
    { &cg_bobpitch, "cg_bobpitch", "0.002", CVAR_ARCHIVE },
    { &cg_bobroll, "cg_bobroll", "0.002", CVAR_ARCHIVE },
    { &cg_swingSpeed, "cg_swingSpeed", "0.3", CVAR_CHEAT },
    { &cg_animSpeed, "cg_animspeed", "1", CVAR_CHEAT },
    { &cg_debugAnim, "cg_debuganim", "0", CVAR_CHEAT },
    { &cg_debugPosition, "cg_debugposition", "0", CVAR_CHEAT },
    { &cg_debugEvents, "cg_debugevents", "0", CVAR_CHEAT },
    { &cg_errorDecay, "cg_errordecay", "100", 0 },
    { &cg_nopredict, "cg_nopredict", "0", 0 },
    { &cg_noPlayerAnims, "cg_noplayeranims", "0", CVAR_CHEAT },
    { &cg_showmiss, "cg_showmiss", "0", 0 },
    { &cg_footsteps, "cg_footsteps", "1", CVAR_CHEAT },
    { &cg_tracerChance, "cg_tracerchance", "0.4", CVAR_CHEAT },
    { &cg_tracerWidth, "cg_tracerwidth", "1", CVAR_CHEAT },
    { &cg_tracerLength, "cg_tracerlength", "100", CVAR_CHEAT },
    { &cg_thirdPersonRange, "cg_thirdPersonRange", "40", CVAR_ARCHIVE },
    { &cg_thirdPersonAngle, "cg_thirdPersonAngle", "0", CVAR_ARCHIVE },
    { &cg_thirdPerson, "cg_thirdPerson", "1", CVAR_ARCHIVE },
    { &cg_teamChatTime, "cg_teamChatTime", "3000", CVAR_ARCHIVE  },
    { &cg_teamChatHeight, "cg_teamChatHeight", "0", CVAR_ARCHIVE  },
    { &cg_forceModel, "cg_forceModel", "0", CVAR_ARCHIVE  },
    { &cg_predictItems, "cg_predictItems", "1", CVAR_ARCHIVE },
    { &cg_deferPlayers, "cg_deferPlayers", "1", CVAR_ARCHIVE },
    { &cg_drawTeamOverlay, "cg_drawTeamOverlay", "0", CVAR_ARCHIVE },
    { &cg_teamOverlayUserinfo, "teamoverlay", "0", CVAR_ROM | CVAR_USERINFO },
    { &cg_stats, "cg_stats", "0", 0 },
    { &cg_drawFriend, "cg_drawFriend", "1", CVAR_ARCHIVE },
    { &cg_teamChatsOnly, "cg_teamChatsOnly", "0", CVAR_ARCHIVE },
    // the following variables are created in other parts of the system,
    // but we also reference them here
    { &cg_buildScript, "com_buildScript", "0", 0 },	// force loading of all possible data amd error on failures
    { &cg_paused, "cl_paused", "0", CVAR_ROM },
    { &cg_synchronousClients, "g_synchronousClients", "0", 0 },	// communicated by systeminfo
    { &cg_redTeamName, "g_redteam", DEFAULT_REDTEAM_NAME, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO },
    { &cg_blueTeamName, "g_blueteam", DEFAULT_BLUETEAM_NAME, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO },
    { &cg_currentSelectedPlayer, "cg_currentSelectedPlayer", "0", CVAR_ARCHIVE},
    { &cg_consoleLatency, "cg_consoleLatency", "3000", CVAR_ARCHIVE },
    { &cg_cameraOrbit, "cg_cameraOrbit", "0", CVAR_CHEAT},
    { &cg_cameraOrbitDelay, "cg_cameraOrbitDelay", "50", CVAR_ARCHIVE},
    { &cg_timescaleFadeEnd, "cg_timescaleFadeEnd", "1", 0},
    { &cg_timescaleFadeSpeed, "cg_timescaleFadeSpeed", "0", 0},
    { &cg_timescale, "timescale", "1", 0},
    { &cg_scorePlum, "cg_scorePlums", "1", CVAR_USERINFO | CVAR_ARCHIVE},
    { &cg_smoothClients, "cg_smoothClients", "0", CVAR_USERINFO | CVAR_ARCHIVE},
    { &cg_cameraMode, "com_cameraMode", "0", CVAR_CHEAT},

    { &pmove_fixed, "pmove_fixed", "0", 0},
    { &pmove_msec, "pmove_msec", "8", 0},
    { &cg_noTaunt, "cg_noTaunt", "0", CVAR_ARCHIVE},
    { &cg_noProjectileTrail, "cg_noProjectileTrail", "0", CVAR_ARCHIVE},
    { &cg_smallFont, "ui_smallFont", "0.25", CVAR_ARCHIVE},
    { &cg_bigFont, "ui_bigFont", "0.4", CVAR_ARCHIVE},
    { &cg_atmosphericEffects, "cg_atmosphericEffects", "1", CVAR_ARCHIVE },
    { &cg_lowEffects, "cg_lowEffects", "0", CVAR_ARCHIVE },
    { &cg_oldRail, "cg_oldRail", "1", CVAR_ARCHIVE},
    { &cg_oldRocket, "cg_oldRocket", "1", CVAR_ARCHIVE},
    { &cg_oldPlasma, "cg_oldPlasma", "1", CVAR_ARCHIVE},
    { &cg_trueLightning, "cg_trueLightning", "0.0", CVAR_ARCHIVE},
//	{ &cg_pmove_fixed, "cg_pmove_fixed", "0", CVAR_USERINFO | CVAR_ARCHIVE }
    { &cg_celShaded, "cg_celShaded", "0", CVAR_ARCHIVE},
    { &cg_color, "cg_color", "0", CVAR_ARCHIVE},
};

static int  cvarTableSize = sizeof( cvarTable ) / sizeof( cvarTable[0] );

/*
=================
CG_RegisterCvars
=================
*/
void CG_RegisterCvars( void )
{
    int			i;
    cvarTable_t	*cv;
    char		var[MAX_TOKEN_CHARS];

    for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ )
    {
        trap_Cvar_Register( cv->vmCvar, cv->cvarName,
                            cv->defaultString, cv->cvarFlags );
    }

    // see if we are also running the server on this machine
    trap_Cvar_VariableStringBuffer( "sv_running", var, sizeof( var ) );
    cgs.localServer = atoi( var );

    forceModelModificationCount = cg_forceModel.modificationCount;

    trap_Cvar_Register(NULL, "model", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
    trap_Cvar_Register(NULL, "headmodel", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
    trap_Cvar_Register(NULL, "team_model", DEFAULT_TEAM_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
    trap_Cvar_Register(NULL, "team_headmodel", DEFAULT_TEAM_HEAD, CVAR_USERINFO | CVAR_ARCHIVE );
}

/*
===================
CG_ForceModelChange
===================
*/
static void CG_ForceModelChange( void )
{
    int		i;

    for (i=0 ; i<MAX_CLIENTS ; i++)
    {
        const char		*clientInfo;

        clientInfo = CG_ConfigString( CS_PLAYERS+i );
        if ( !clientInfo[0] )
        {
            continue;
        }
        CG_NewClientInfo( i );
    }
}

/*
=================
CG_UpdateCvars
=================
*/
void CG_UpdateCvars( void )
{
    int			i;
    cvarTable_t	*cv;

    for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ )
    {
        trap_Cvar_Update( cv->vmCvar );
    }

    // check for modications here

    // If team overlay is on, ask for updates from the server.  If its off,
    // let the server know so we don't receive it
    if ( drawTeamOverlayModificationCount != cg_drawTeamOverlay.modificationCount )
    {
        drawTeamOverlayModificationCount = cg_drawTeamOverlay.modificationCount;

        if ( cg_drawTeamOverlay.integer > 0 )
        {
            trap_Cvar_Set( "teamoverlay", "1" );
        }
        else
        {
            trap_Cvar_Set( "teamoverlay", "0" );
        }
        // FIXME E3 HACK
        trap_Cvar_Set( "teamoverlay", "1" );
    }

    // if force model changed
    if ( forceModelModificationCount != cg_forceModel.modificationCount )
    {
        forceModelModificationCount = cg_forceModel.modificationCount;
        CG_ForceModelChange();
    }
}

int CG_CrosshairPlayer( void )
{
    if ( cg.time > ( cg.crosshairClientTime + 1000 ) )
    {
        return -1;
    }
    return cg.crosshairClientNum;
}

int CG_LastAttacker( void )
{
    if ( !cg.attackerTime )
    {
        return -1;
    }
    return cg.snap->ps.persistant[PERS_ATTACKER];
}

void CG_RemoveConsoleLine( void )
{
    int i, offset, totalLength;

    if( cg.numConsoleLines == 0 )
        return;

    offset = cg.consoleLines[ 0 ].length;
    totalLength = strlen( cg.consoleText ) - offset;

    //slide up consoleText
    for( i = 0; i <= totalLength; i++ )
        cg.consoleText[ i ] = cg.consoleText[ i + offset ];

    //pop up the first consoleLine
    for( i = 0; i < cg.numConsoleLines; i++ )
        cg.consoleLines[ i ] = cg.consoleLines[ i + 1 ];

    cg.numConsoleLines--;
}

void CG_TAUIConsole( char *icon, const char *text )
{
    if( cg.numConsoleLines == MAX_CONSOLE_LINES )
        CG_RemoveConsoleLine( );

    if( cg.consoleValid )
    {
        strcat( cg.consoleText, text );
        cg.consoleLines[ cg.numConsoleLines ].time = cg.time;
        cg.consoleLines[ cg.numConsoleLines ].length = strlen( text );
        cg.consoleLines[ cg.numConsoleLines ].icon = icon;
        cg.numConsoleLines++;
    }

}

void QDECL CG_Printf( char *icon, const char *msg, ... )
{
    va_list		argptr;
    char		text[1024];

    va_start (argptr, msg);
    vsprintf (text, msg, argptr);
    va_end (argptr);
    CG_TAUIConsole( icon, text );

    trap_Print( text );
}

void QDECL CG_Error( const char *msg, ... )
{
    va_list		argptr;
    char		text[1024];

    va_start (argptr, msg);
    vsprintf (text, msg, argptr);
    va_end (argptr);

    trap_Error( text );
}

#ifndef CGAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link (FIXME)

void QDECL Com_Error( int level, const char *error, ... )
{
    va_list		argptr;
    char		text[1024];

    va_start (argptr, error);
    vsprintf (text, error, argptr);
    va_end (argptr);

    CG_Error( "%s", text);
}

void QDECL Com_Printf( const char *msg, ... )
{
    va_list		argptr;
    char		text[1024];

    va_start (argptr, msg);
    vsprintf (text, msg, argptr);
    va_end (argptr);

    if( cg.numConsoleLines == MAX_CONSOLE_LINES )
        CG_RemoveConsoleLine( );

    if( cg.consoleValid )
    {
        strcat( cg.consoleText, text );
        cg.consoleLines[ cg.numConsoleLines ].time = cg.time;
        cg.consoleLines[ cg.numConsoleLines ].length = strlen( text );
        cg.numConsoleLines++;
    }

    CG_Printf( NULL,"%s", text);
}

#endif

/*
================
CG_Argv
================
*/
const char *CG_Argv( int arg )
{
    static char	buffer[MAX_STRING_CHARS];

    trap_Argv( arg, buffer, sizeof( buffer ) );

    return buffer;
}


//========================================================================

/*
=================
CG_RegisterItemSounds

The server says this item is used on this level
=================
*/
static void CG_RegisterItemSounds( int itemNum )
{
    gitem_t			*item;
    char			data[MAX_QPATH];
    char			*s, *start;
    int				len;

    item = &bg_itemlist[ itemNum ];

    if( item->pickup_sound )
    {
        trap_S_RegisterSound( item->pickup_sound, qfalse );
    }

    // parse the space seperated precache string for other media
    s = item->sounds;
    if (!s || !s[0])
        return;

    while (*s)
    {
        start = s;
        while (*s && *s != ' ')
        {
            s++;
        }

        len = s-start;
        if (len >= MAX_QPATH || len < 5)
        {
            CG_Error( "PrecacheItem: %s has bad precache string",
                      item->classname);
            return;
        }
        memcpy (data, start, len);
        data[len] = 0;
        if ( *s )
        {
            s++;
        }

        if ( !strcmp(data+len-3, "wav" ))
        {
            trap_S_RegisterSound( data, qfalse );
        }
    }
}


/*
=================
CG_RegisterSounds

called during a precache command
=================
*/
static void CG_RegisterSounds( void )
{
    int		i;
    char	items[MAX_ITEMS+1];
    char	name[MAX_QPATH];
    const char	*soundName;


    cgs.media.oneMinuteSound = trap_S_RegisterSound( "sound/feedback/1_minute.wav", qtrue );
    cgs.media.fiveMinuteSound = trap_S_RegisterSound( "sound/feedback/5_minute.wav", qtrue );
    cgs.media.suddenDeathSound = trap_S_RegisterSound( "sound/feedback/sudden_death.wav", qtrue );
    cgs.media.oneFragSound = trap_S_RegisterSound( "sound/feedback/1_frag.wav", qtrue );
    cgs.media.twoFragSound = trap_S_RegisterSound( "sound/feedback/2_frags.wav", qtrue );
    cgs.media.threeFragSound = trap_S_RegisterSound( "sound/feedback/3_frags.wav", qtrue );
    cgs.media.count3Sound = trap_S_RegisterSound( "sound/feedback/three.wav", qtrue );
    cgs.media.count2Sound = trap_S_RegisterSound( "sound/feedback/two.wav", qtrue );
    cgs.media.count1Sound = trap_S_RegisterSound( "sound/feedback/one.wav", qtrue );
    cgs.media.countFightSound = trap_S_RegisterSound( "sound/feedback/fight.wav", qtrue );
    cgs.media.countPrepareSound = trap_S_RegisterSound( "sound/feedback/prepare.wav", qtrue );

    // loadingscreen
    CG_UpdateSoundFraction( 0.33f );
    CG_UpdateMediaFraction( 0.20f );

    if ( cgs.gametype >= GT_TEAM || cg_buildScript.integer )
    {

        cgs.media.redLeadsSound = trap_S_RegisterSound( "sound/feedback/redleads.wav", qtrue );
        cgs.media.blueLeadsSound = trap_S_RegisterSound( "sound/feedback/blueleads.wav", qtrue );
        cgs.media.teamsTiedSound = trap_S_RegisterSound( "sound/feedback/teamstied.wav", qtrue );
        cgs.media.hitTeamSound = trap_S_RegisterSound( "sound/feedback/hit_teammate.wav", qtrue );

        cgs.media.redScoredSound = trap_S_RegisterSound( "sound/teamplay/voc_red_scores.wav", qtrue );
        cgs.media.blueScoredSound = trap_S_RegisterSound( "sound/teamplay/voc_blue_scores.wav", qtrue );

        cgs.media.captureYourTeamSound = trap_S_RegisterSound( "sound/teamplay/flagcapture_yourteam.wav", qtrue );
        cgs.media.captureOpponentSound = trap_S_RegisterSound( "sound/teamplay/flagcapture_opponent.wav", qtrue );

        cgs.media.returnYourTeamSound = trap_S_RegisterSound( "sound/teamplay/flagreturn_yourteam.wav", qtrue );
        cgs.media.returnOpponentSound = trap_S_RegisterSound( "sound/teamplay/flagreturn_opponent.wav", qtrue );

        cgs.media.takenYourTeamSound = trap_S_RegisterSound( "sound/teamplay/flagtaken_yourteam.wav", qtrue );
        cgs.media.takenOpponentSound = trap_S_RegisterSound( "sound/teamplay/flagtaken_opponent.wav", qtrue );

        if ( cgs.gametype == GT_CTF || cg_buildScript.integer )
        {
            cgs.media.redFlagReturnedSound = trap_S_RegisterSound( "sound/teamplay/voc_red_returned.wav", qtrue );
            cgs.media.blueFlagReturnedSound = trap_S_RegisterSound( "sound/teamplay/voc_blue_returned.wav", qtrue );
            cgs.media.enemyTookYourFlagSound = trap_S_RegisterSound( "sound/teamplay/voc_enemy_flag.wav", qtrue );
            cgs.media.yourTeamTookEnemyFlagSound = trap_S_RegisterSound( "sound/teamplay/voc_team_flag.wav", qtrue );
        }

        cgs.media.youHaveFlagSound = trap_S_RegisterSound( "sound/teamplay/voc_you_flag.wav", qtrue );
        cgs.media.neutralFlagReturnedSound = trap_S_RegisterSound( "sound/teamplay/flagreturn_opponent.wav", qtrue );
        cgs.media.yourTeamTookTheFlagSound = trap_S_RegisterSound( "sound/teamplay/voc_team_1flag.wav", qtrue );
        cgs.media.enemyTookTheFlagSound = trap_S_RegisterSound( "sound/teamplay/voc_enemy_1flag.wav", qtrue );
    }

    // loadingscreen
    CG_UpdateSoundFraction( 0.60f );
    CG_UpdateMediaFraction( 0.30f );

    cgs.media.tracerSound = trap_S_RegisterSound( "sound/weapons/machinegun/buletby1.wav", qfalse );
    cgs.media.selectSound = trap_S_RegisterSound( "sound/weapons/change.wav", qfalse );
    cgs.media.wearOffSound = trap_S_RegisterSound( "sound/items/wearoff.wav", qfalse );
    cgs.media.useNothingSound = trap_S_RegisterSound( "sound/items/use_nothing.wav", qfalse );

    cgs.media.teleInSound = trap_S_RegisterSound( "sound/world/telein.wav", qfalse );
    cgs.media.teleOutSound = trap_S_RegisterSound( "sound/world/teleout.wav", qfalse );
    cgs.media.respawnSound = trap_S_RegisterSound( "sound/items/respawn1.wav", qfalse );
    cgs.media.lightningStrike = trap_S_RegisterSound( "sound/world/lightning.wav", qfalse );

    cgs.media.noAmmoSound = trap_S_RegisterSound( "sound/weapons/noammo.wav", qfalse );

    cgs.media.talkSound = trap_S_RegisterSound( "sound/player/talk.wav", qfalse );
    cgs.media.landSound = trap_S_RegisterSound( "sound/player/land1.wav", qfalse);

    cgs.media.hitSound = trap_S_RegisterSound( "sound/feedback/hit.wav", qfalse );

    cgs.media.takenLeadSound = trap_S_RegisterSound( "sound/feedback/takenlead.wav", qtrue);
    cgs.media.tiedLeadSound = trap_S_RegisterSound( "sound/feedback/tiedlead.wav", qtrue);
    cgs.media.lostLeadSound = trap_S_RegisterSound( "sound/feedback/lostlead.wav", qtrue);

    cgs.media.watrInSound = trap_S_RegisterSound( "sound/player/watr_in.wav", qfalse);
    cgs.media.watrOutSound = trap_S_RegisterSound( "sound/player/watr_out.wav", qfalse);
    cgs.media.watrUnSound = trap_S_RegisterSound( "sound/player/watr_un.wav", qfalse);

    cgs.media.jumpPadSound = trap_S_RegisterSound ("sound/world/jumppad.wav", qfalse );

    // loadingscreen
    CG_UpdateSoundFraction( 0.75f );
    CG_UpdateMediaFraction( 0.40f );

    for (i=0 ; i<4 ; i++)
    {
        Com_sprintf (name, sizeof(name), "sound/player/footsteps/step%i.wav", i+1);
        cgs.media.footsteps[FOOTSTEP_NORMAL][i] = trap_S_RegisterSound (name, qfalse);

        Com_sprintf (name, sizeof(name), "sound/player/footsteps/boot%i.wav", i+1);
        cgs.media.footsteps[FOOTSTEP_BOOT][i] = trap_S_RegisterSound (name, qfalse);

        Com_sprintf (name, sizeof(name), "sound/player/footsteps/flesh%i.wav", i+1);
        cgs.media.footsteps[FOOTSTEP_FLESH][i] = trap_S_RegisterSound (name, qfalse);

        Com_sprintf (name, sizeof(name), "sound/player/footsteps/repl%i.wav", i+1);
        cgs.media.footsteps[FOOTSTEP_MECH][i] = trap_S_RegisterSound (name, qfalse);

        Com_sprintf (name, sizeof(name), "sound/player/footsteps/energy%i.wav", i+1);
        cgs.media.footsteps[FOOTSTEP_ENERGY][i] = trap_S_RegisterSound (name, qfalse);

        Com_sprintf (name, sizeof(name), "sound/player/footsteps/splash%i.wav", i+1);
        cgs.media.footsteps[FOOTSTEP_SPLASH][i] = trap_S_RegisterSound (name, qfalse);

        Com_sprintf (name, sizeof(name), "sound/player/footsteps/clank%i.wav", i+1);
        cgs.media.footsteps[FOOTSTEP_METAL][i] = trap_S_RegisterSound (name, qfalse);
    }

    // only register the items that the server says we need
    strcpy( items, CG_ConfigString( CS_ITEMS ) );

    for ( i = 1 ; i < bg_numItems ; i++ )
    {
//		if ( items[ i ] == '1' || cg_buildScript.integer ) {
        CG_RegisterItemSounds( i );
//		}
    }

    for ( i = 1 ; i < MAX_SOUNDS ; i++ )
    {
        soundName = CG_ConfigString( CS_SOUNDS+i );
        if ( !soundName[0] )
        {
            break;
        }
        if ( soundName[0] == '*' )
        {
            continue;	// custom sound
        }
        cgs.gameSounds[i] = trap_S_RegisterSound( soundName, qfalse );
    }

    // loadingscreen
    CG_UpdateSoundFraction( 0.85f );
    CG_UpdateMediaFraction( 0.50f );

    // FIXME: only needed with item
    cgs.media.flightSound = trap_S_RegisterSound( "sound/items/flight.wav", qfalse );
    cgs.media.subtankSound = trap_S_RegisterSound ("sound/xbuster/xbitem_subtank.wav", qfalse);
    // Weapon Hit Sounds
    cgs.media.sfx_rockexp = trap_S_RegisterSound ("sound/weapons/rocket/rocklx1a.wav", qfalse);
    cgs.media.sfx_plasmaexp = trap_S_RegisterSound ("sound/weapons/plasma/plasmx1a.wav", qfalse);
    cgs.media.sfx_Earthexp = trap_S_RegisterSound ("sound/xbuster/weapons/weaphit_impact1.wav", qfalse);
    cgs.media.sfx_Bang1exp = trap_S_RegisterSound ("sound/xbuster/weapons/weaphit_bang1.wav", qfalse);
    cgs.media.sfx_Bang2exp = trap_S_RegisterSound ("sound/xbuster/weapons/weaphit_bang2.wav", qfalse);
    cgs.media.sfx_Bang3exp = trap_S_RegisterSound ("sound/xbuster/weapons/weaphit_bang3.wav", qfalse);
    cgs.media.sfx_Bang4exp = trap_S_RegisterSound ("sound/xbuster/weapons/weaphit_bang4.wav", qfalse);
    cgs.media.sfx_Tempest1exp = trap_S_RegisterSound ("sound/xbuster/weapons/weaphit_tempest1.wav", qfalse);
    cgs.media.sfx_Crystal1exp = trap_S_RegisterSound ("sound/xbuster/weapons/ping_metal.wav", qfalse);
    cgs.media.sfx_Acid2exp = trap_S_RegisterSound ("sound/xbuster/weapons/weaphit_Acid2.wav", qfalse);
    // Map Function Sounds
    cgs.media.glassbreakSound = trap_S_RegisterSound ("sound/world/glassbreak.wav", qfalse);

    cgs.media.winnerSound = trap_S_RegisterSound( "sound/feedback/voc_youwin.wav", qfalse );
    cgs.media.loserSound = trap_S_RegisterSound( "sound/feedback/voc_youlose.wav", qfalse );

    cgs.media.n_healthSound = trap_S_RegisterSound("sound/items/n_health.wav", qfalse );
    cgs.media.hgrenb1aSound = trap_S_RegisterSound("sound/weapons/grenade/hgrenb1a.wav", qfalse);
    cgs.media.hgrenb2aSound = trap_S_RegisterSound("sound/weapons/grenade/hgrenb2a.wav", qfalse);

    cgs.media.rainSound = trap_S_RegisterSound("sound/world/rain.wav",qfalse);
    cgs.media.earthquakeSound = trap_S_RegisterSound("sound/world/earthquake.wav",qfalse);
}


//===================================================================================


/*
=================
CG_RegisterGraphics

This function may execute for a couple of minutes with a slow disk.
=================
*/
static void CG_RegisterGraphics( void )
{
    int			i;
    char		items[MAX_ITEMS+1];
    char		npcs[NPC_NUMNPCS+1];
    static char		*sb_nums[11] =
    {
        "gfx/2d/numbers/zero_32b",
        "gfx/2d/numbers/one_32b",
        "gfx/2d/numbers/two_32b",
        "gfx/2d/numbers/three_32b",
        "gfx/2d/numbers/four_32b",
        "gfx/2d/numbers/five_32b",
        "gfx/2d/numbers/six_32b",
        "gfx/2d/numbers/seven_32b",
        "gfx/2d/numbers/eight_32b",
        "gfx/2d/numbers/nine_32b",
        "gfx/2d/numbers/minus_32b",
    };

    // clear any references to old media
    memset( &cg.refdef, 0, sizeof( cg.refdef ) );
    trap_R_ClearScene();

    CG_LoadingString( cgs.mapname );

    trap_R_LoadWorldMap( cgs.mapname );

    // loadingscreen
    CG_UpdateGraphicFraction( 0.20f );
    CG_UpdateMediaFraction( 0.66f );

    // precache status bar pics
    CG_LoadingString( "game media" );

    for ( i=0 ; i<11 ; i++)
    {
        cgs.media.numberShaders[i] = trap_R_RegisterShader( sb_nums[i] );
    }

    cgs.media.botSkillShaders[0] = trap_R_RegisterShader( "menu/art/skill1.tga" );
    cgs.media.botSkillShaders[1] = trap_R_RegisterShader( "menu/art/skill2.tga" );
    cgs.media.botSkillShaders[2] = trap_R_RegisterShader( "menu/art/skill3.tga" );
    cgs.media.botSkillShaders[3] = trap_R_RegisterShader( "menu/art/skill4.tga" );
    cgs.media.botSkillShaders[4] = trap_R_RegisterShader( "menu/art/skill5.tga" );

    cgs.media.viewBloodShader = trap_R_RegisterShader( "viewBloodBlend" );

    cgs.media.deferShader = trap_R_RegisterShaderNoMip( "gfx/2d/defer.tga" );

    cgs.media.smokePuffShader = trap_R_RegisterShader( "smokePuff" );
    cgs.media.smokePuffRageProShader = trap_R_RegisterShader( "smokePuffRagePro" );

    cgs.media.plasmaBallShader = trap_R_RegisterShader( "sprites/plasma1" );
    cgs.media.lagometerShader = trap_R_RegisterShader("lagometer" );
    cgs.media.connectionShader = trap_R_RegisterShader( "disconnected" );

    cgs.media.waterBubbleShader = trap_R_RegisterShader( "waterBubble" );

    cgs.media.tracerShader = trap_R_RegisterShader( "gfx/misc/tracer" );
    cgs.media.selectShader = trap_R_RegisterShader( "gfx/2d/select" );

    for ( i = 0 ; i < NUM_CROSSHAIRS ; i++ )
    {
        cgs.media.crosshairShader[i] = trap_R_RegisterShader( va("gfx/2d/crosshair%c", 'a'+i) );
    }

    cgs.media.backTileShader = trap_R_RegisterShader( "gfx/2d/backtile" );
    cgs.media.noammoShader = trap_R_RegisterShader( "icons/noammo" );

    // powerup shaders
    cgs.media.quadShader = trap_R_RegisterShader("powerups/quad" );
    cgs.media.quadWeaponShader = trap_R_RegisterShader("powerups/quadWeapon" );
    cgs.media.battleSuitShader = trap_R_RegisterShader("powerups/battleSuit" );
    cgs.media.battleWeaponShader = trap_R_RegisterShader("powerups/battleWeapon" );
    cgs.media.invisShader = trap_R_RegisterShader("powerups/invisibility" );
    cgs.media.regenShader = trap_R_RegisterShader("powerups/regen" );
    cgs.media.hastePuffShader = trap_R_RegisterShader("hasteSmokePuff" );
    cgs.media.jetSmokeShader = trap_R_RegisterShader("smokePuff" );

    if ( cgs.gametype == GT_CTF || cg_buildScript.integer )
    {
        cgs.media.redFlagModel = trap_R_RegisterModel( "models/flags/r_flag.md3" );
        cgs.media.blueFlagModel = trap_R_RegisterModel( "models/flags/b_flag.md3" );
        cgs.media.redFlagShader[0] = trap_R_RegisterShaderNoMip( "icons/iconf_red1" );
        cgs.media.redFlagShader[1] = trap_R_RegisterShaderNoMip( "icons/iconf_red2" );
        cgs.media.redFlagShader[2] = trap_R_RegisterShaderNoMip( "icons/iconf_red3" );
        cgs.media.blueFlagShader[0] = trap_R_RegisterShaderNoMip( "icons/iconf_blu1" );
        cgs.media.blueFlagShader[1] = trap_R_RegisterShaderNoMip( "icons/iconf_blu2" );
        cgs.media.blueFlagShader[2] = trap_R_RegisterShaderNoMip( "icons/iconf_blu3" );
    }


    if ( cgs.gametype >= GT_TEAM || cg_buildScript.integer )
    {
        cgs.media.friendShader = trap_R_RegisterShader( "sprites/foe" );
        cgs.media.redQuadShader = trap_R_RegisterShader("powerups/blueflag" );
        cgs.media.teamStatusBar = trap_R_RegisterShader( "gfx/2d/colorbar.tga" );
    }

    cgs.media.armorModel = trap_R_RegisterModel( "models/powerups/armor/armor_yel.md3" );
    cgs.media.armorIcon  = trap_R_RegisterShaderNoMip( "icons/iconr_yellow" );

    cgs.media.balloonShader = trap_R_RegisterShader( "sprites/balloon3" );

    // xb models / effects / etc
    cgs.media.ringFlashModel = trap_R_RegisterModel("models/effects/hit_flash1.md3");
    cgs.media.gravwellModel = trap_R_RegisterModel("weapons/gravity/sheild_gwell.md3");
    cgs.media.protoshieldModel = trap_R_RegisterModel("models/shields/xbsheild_proto1.md3");
    cgs.media.xbSpriteModel		= trap_R_RegisterModel("gfx/charweapons/mmx/buster_1.md3");

    //muzzle flashes
    cgs.media.xshotFlashModel = trap_R_RegisterModel("models/effects/xb_muz_flash0.md3");
    cgs.media.xshot2FlashModel = trap_R_RegisterModel ("models/effects/xb_muz_flash4.md3");
    cgs.media.bassFlashModel = trap_R_RegisterModel("models/effects/xb_muz_flash0.md3");
    cgs.media.mshotFlashModel = trap_R_RegisterModel("models/effects/xb_muz_flash0.md3");
    cgs.media.zshotFlashModel = trap_R_RegisterModel("models/effects/xb_muz_flash0.md3");

    //generic shots
    cgs.media.xShot = trap_R_RegisterShader( "x_blast1" );
    cgs.media.xShot4 = trap_R_RegisterShader( "x_blast4" );
    cgs.media.bassShot = trap_R_RegisterShader( "bass_blast1" );
    cgs.media.xShotShader = trap_R_RegisterShader("hit_xshot_1" );
    cgs.media.xShot1Shader = trap_R_RegisterShader("hit_xshot_2" );
    cgs.media.xShot2Shader = trap_R_RegisterShader("hit_xshot_3" );
    cgs.media.xShot3Shader = trap_R_RegisterShader("hit_xshot_4" );
    cgs.media.bassShotShader = trap_R_RegisterShader("hit_bassshot_1" );
    cgs.media.xbSaberModel = trap_R_RegisterModel( "models/weapons/sabers/zero/saber_zero.md3" );
    cgs.media.xbSaberhiltShader = trap_R_RegisterShader( "saber_zero" );
    cgs.media.xbSaberbladeModel = trap_R_RegisterModel( "models/weapons/sabers/zero/saber_zero_blade.md3" );
    cgs.media.xbSaberbladeShader = trap_R_RegisterShader( "saber_zero_blade" );
    //charge
    cgs.media.chargeFireShot1 = trap_R_RegisterShader( "weapon/fire/charge/shot1" );
    cgs.media.chargeFireShot2 = trap_R_RegisterShader("weapon/fire/charge/shot2" );
    cgs.media.chargeWaterShot1 = trap_R_RegisterShader("weapon/water/charge/shot1" );
    cgs.media.chargeWaterShot2 = trap_R_RegisterShader("weapon/water/charge/shot2" );
    cgs.media.chargeWaterShot3 = trap_R_RegisterShader("weapon/water/charge/shot3" );
    //cgs.media.chargeWindShot1 = trap_R_RegisterShader("weapon/wind/charge/shot1" );
    //cgs.media.chargeWindShot2 = trap_R_RegisterShader("weapon/wind/charge/shot2" );
    cgs.media.chargeEarthShot1 = trap_R_RegisterShader("weapon/earth/charge/shot1" );
    cgs.media.chargeEarthShot2 = trap_R_RegisterShader("weapon/earth/charge/shot2" );
    cgs.media.chargeNatureShot1 = trap_R_RegisterShader("weapon/plant/charge/shot1" );
    cgs.media.chargeNatureShot2 = trap_R_RegisterShader("weapon/nature/charge/shot2" );
    cgs.media.chargeGravityShot1 = trap_R_RegisterShader("weapon/gravity/charge/shot1" );
    cgs.media.chargeGravityShot2 = trap_R_RegisterShader("weapon/gravity/charge/shot2" );
    cgs.media.chargeLightShot1 = trap_R_RegisterShader("weapon/light/charge/beam1" );
    cgs.media.chargeLightShot2 = trap_R_RegisterShader("weapon/light/charge/shot2" );
    cgs.media.chargeElectricShot1 = trap_R_RegisterShader("weapon/energy/charge/shot1" );
    //cgs.media.chargeElectricShot2 = trap_R_RegisterShader("weapon/electric/charge/shot2" );
    cgs.media.chargeFireHit = trap_R_RegisterShader( "weapon/fire/charge/hit1" );
    cgs.media.chargeFireHit2 = trap_R_RegisterShader( "weapon/fire/charge/hit2" );
    cgs.media.chargeWaterHit = trap_R_RegisterShader("weapon/water/charge/hit1" );
    cgs.media.chargeWaterHit2 = trap_R_RegisterShader("weapon/water/charge/hit2" );
    cgs.media.chargeWaterHit3 = trap_R_RegisterShader("weapon/water/charge/hit3" );
    cgs.media.chargeWindHit = trap_R_RegisterShader("weapon/wind/charge1/hit1" );
    cgs.media.chargeEarthHit = trap_R_RegisterShader("weapon/earth/charge/hit1" );
    cgs.media.chargeNatureHit = trap_R_RegisterShader("weapon/plant/charge/hit1" );
    cgs.media.chargeTechHit = trap_R_RegisterShader("weapon/tech/charge/hit1" );
    cgs.media.chargeGravityHit = trap_R_RegisterShader("weapon/gravity/charge/hit1" );
    cgs.media.chargeLightHit = trap_R_RegisterShader("weapon/light/charge/hit1" );
    cgs.media.chargeLightHitM = trap_R_RegisterModel("weapons/light/charge_hit1.md3" );
    cgs.media.chargeElectricHit = trap_R_RegisterShader("weapon/energy/charge/hit1" );
    //buster
    cgs.media.busterFireShot = trap_R_RegisterShader( "weapon/fire/buster/shot1" );
    cgs.media.busterWaterShot = trap_R_RegisterShader("weapon/water/buster/shot1" );
    //cgs.media.busterWindShot = trap_R_RegisterShader("weapon/wind/buster/shot1" );
    cgs.media.busterNatureShot = trap_R_RegisterShader("weapon/nature/particle" );
    cgs.media.busterGravityShot = trap_R_RegisterModel("weapons/gravity/buster_mm.md3" );
    cgs.media.busterFireHit = trap_R_RegisterShader( "weapon/fire/buster/hit1" );
    cgs.media.busterWaterHit = trap_R_RegisterShader("weapon/water/buster/hit1" );
    cgs.media.busterWindHit = trap_R_RegisterShader("weapon/wind/buster/hit1" );
    cgs.media.busterNatureHit = trap_R_RegisterShader ("weapon/plant/buster/hit1" );
    cgs.media.busterGravityHit = trap_R_RegisterShader("weapon/gravity/buster/hit1" );
    cgs.media.busterEarthHit = trap_R_RegisterShader("weapon/earth/buster/hit1" );
    cgs.media.busterTechHit = trap_R_RegisterShader("weapon/tech/buster/hit1" );
    cgs.media.busterLightHit = trap_R_RegisterShader("weapon/light/buster/hit1" );
    //rapid
    cgs.media.rapidFireShot = trap_R_RegisterShader( "weapon/fire/rapid/shot1" );
    //cgs.media.rapidWaterShot = trap_R_RegisterShader("weapon/water/rapid/shot1" );
    //cgs.media.rapidWindShot = trap_R_RegisterShader("weapon/wind/rapid/shot1" );
    cgs.media.rapidNatureShot = trap_R_RegisterShader("weapon/plant/rapid/shot1" );
    cgs.media.rapidNatureShot2 = trap_R_RegisterShader("weapon/plant/rapid/shot2" );
    cgs.media.rapidGravityShot = trap_R_RegisterShader("weapon/gravity/rapid/shot1" );
    cgs.media.rapidLightShot = trap_R_RegisterShader("weapon/light/rapid/shot1" );
    cgs.media.rapidElectricShot = trap_R_RegisterShader("weapon/energy/rapid/shot1" );
    cgs.media.rapidFireHit = trap_R_RegisterShader( "weapon/fire/rapid/hit1" );
    cgs.media.rapidWaterHit = trap_R_RegisterShader("weapon/water/rapid/hit1" );
    cgs.media.rapidWindHit = trap_R_RegisterShader("weapon/wind/rapid/hit1" );
    cgs.media.rapidEarthHit = trap_R_RegisterShader("weapon/earth/rapid/hit1" );
    cgs.media.rapidNatureHit = trap_R_RegisterShader("weapon/plant/rapid/hit1" );
    cgs.media.rapidTechHit = trap_R_RegisterShader("weapon/tech/rapid/hit1" );
    cgs.media.rapidGravityHit = trap_R_RegisterShader("weapon/gravity/rapid/hit1" );
    cgs.media.rapidLightHit = trap_R_RegisterShader("weapon/light/rapid/hit1" );
    cgs.media.rapidElectricHit = trap_R_RegisterShader("weapon/energy/rapid/hit1" );
    //melee
    /*cgs.media.meleeFireShot = trap_R_RegisterShader( "weapon/fire/melee/shot1" );
    cgs.media.meleeWaterShot = trap_R_RegisterShader("weapon/water/melee/shot1" );
    cgs.media.meleeWindShot = trap_R_RegisterShader("weapon/wind/melee/shot1" );
    cgs.media.meleeEarthShot = trap_R_RegisterShader("weapon/earth/melee/shot1" );
    cgs.media.meleeNatureShot = trap_R_RegisterShader("weapon/plant/melee/shot1" );
    cgs.media.meleeTechShot = trap_R_RegisterShader("weapon/tech/melee/shot1" );
    cgs.media.meleeGravityShot = trap_R_RegisterShader("weapon/gravity/melee/shot1" );
    cgs.media.meleeLightShot = trap_R_RegisterShader("weapon/light/melee/shot1" );
    cgs.media.meleeElectricShot = trap_R_RegisterShader("weapon/energy/melee/shot1" );
    cgs.media.meleeFireHit = trap_R_RegisterShader( "weapon/fire/melee/hit1" );
    cgs.media.meleeWaterHit = trap_R_RegisterShader("weapon/water/melee/hit1" );
    cgs.media.meleeWindHit = trap_R_RegisterShader("weapon/wind/melee/hit1" );
    cgs.media.meleeEarthHit = trap_R_RegisterShader("weapon/earth/melee/hit1" );
    cgs.media.meleeNatureHit = trap_R_RegisterShader("weapon/plant/melee/hit1" );
    cgs.media.meleeTechHit = trap_R_RegisterShader("weapon/tech/melee/hit1" );
    cgs.media.meleeGravityHit = trap_R_RegisterShader("weapon/gravity/melee/hit1" );
    cgs.media.meleeLightHit = trap_R_RegisterShader("weapon/light/melee/hit1" );
    cgs.media.meleeElectricHit = trap_R_RegisterShader("weapon/energy/melee/hit1" );
    */
    // Sheild
    cgs.media.sheildFireShot = trap_R_RegisterShader( "weapon/fire/sheild/shot1" );
    //cgs.media.sheildWaterShot = trap_R_RegisterShader("weapon/water/sheild/shot1" );
    //cgs.media.sheildWindShot = trap_R_RegisterShader("weapon/wind/sheild/shot1" );
    //cgs.media.sheildEarthShot = trap_R_RegisterShader("weapon/earth/sheild/shot1" );
    cgs.media.sheildTechShot = trap_R_RegisterShader("weapon/tech/sheild/shot1" );
    //cgs.media.sheildNatureShot = trap_R_RegisterShader("weapon/nature/particle" );
    cgs.media.sheildElectricShot = trap_R_RegisterShader ( "weapon/energy/sheild/shot1" );
    cgs.media.sheildFireHit = trap_R_RegisterShader( "weapon/fire/sheild/hit1" );
    //cgs.media.sheildWaterHit = trap_R_RegisterShader("weapon/water/sheild/hit1" );
    //cgs.media.sheildWindHit = trap_R_RegisterShader("weapon/wind/sheild/hit1" );
    //cgs.media.sheildEarthHit = trap_R_RegisterShader("weapon/earth/sheild/hit1" );
    cgs.media.sheildTechHit = trap_R_RegisterShader("weapon/tech/sheild/hit1" );
    //cgs.media.sheildNatureHit = trap_R_RegisterShader("weapon/plant/sheild/hit1" );
    //cgs.media.sheildGravityHit = trap_R_RegisterShader("weapon/gravity/sheild/hit1" );
    //cgs.media.sheildLightHit = trap_R_RegisterShader("weapon/light/sheild/hit1" );
    cgs.media.sheildElectricHit = trap_R_RegisterShader("weapon/energy/sheild/hit1" );

    //defaults
    cgs.media.plasmaExplosionShader = trap_R_RegisterShader( "plasmaExplosion" );
    cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );

    cgs.media.lightningboltShader = trap_R_RegisterShader("xb_fx_lightningBolt1");
    cgs.media.celShader = trap_R_RegisterShader("models/players/effects/outline");
    cgs.media.gotHitShader = trap_R_RegisterShader( "pain_effect_blue" );
    cgs.media.chargeGlowShader = trap_R_RegisterShader( "charge_glow_blue" );
    cgs.media.chargeSphereShader = trap_R_RegisterShader( "xb_orb2" );
    cgs.media.chargeSphere2Shader = trap_R_RegisterShader( "xb_orb" );
    cgs.media.chargeSphere3Shader = trap_R_RegisterShader( "xb_orb3" );
    cgs.media.charge1Shader = trap_R_RegisterShader("charge_s1" );
    cgs.media.charge2Shader = trap_R_RegisterShader("charge_s2" );
    cgs.media.charge3Shader = trap_R_RegisterShader("charge_flash_white" );
    cgs.media.charge4Shader = trap_R_RegisterShader("charge_flash_purple" );
    cgs.media.xbSmokeShader = trap_R_RegisterShader("xb_smoke" );
    cgs.media.xbWallDustShader = trap_R_RegisterShader("xb_walldust" );
    cgs.media.xbSparkShader = trap_R_RegisterShader("xb_spark" );
    cgs.media.xbBlueSparkShader = trap_R_RegisterShader("xb_spark_blue" );
    cgs.media.xbDeathShader = trap_R_RegisterShader("xb_explode" );
    cgs.media.xbBrownDeathShader = trap_R_RegisterShader("xb_explodebrown" );
    //gfx - dashing
    cgs.media.blackDashEffectShader = trap_R_RegisterShader("dash_fx_black");
    cgs.media.redDashEffectShader = trap_R_RegisterShader("dash_fx_red");
    cgs.media.greenDashEffectShader = trap_R_RegisterShader("dash_fx_green");
    cgs.media.yellowDashEffectShader = trap_R_RegisterShader("dash_fx_yellow");
    cgs.media.blueDashEffectShader = trap_R_RegisterShader("dash_fx_blue");
    cgs.media.cyanDashEffectShader = trap_R_RegisterShader("dash_fx_cyan");
    cgs.media.purpleDashEffectShader = trap_R_RegisterShader("dash_fx_purple");
    cgs.media.whiteDashEffectShader = trap_R_RegisterShader("dash_fx_white");
    //effects - booster / teleport
    cgs.media.boostModel = trap_R_RegisterModel( "models/effects/thruster.md3" );
    cgs.media.boosterShader = trap_R_RegisterShader("thruster_1" );
    cgs.media.blackSpawnShader = trap_R_RegisterShader("teleport_black1");
    cgs.media.redSpawnShader = trap_R_RegisterShader("teleport_red1");
    cgs.media.greenSpawnShader = trap_R_RegisterShader("teleport_green1");
    cgs.media.yellowSpawnShader = trap_R_RegisterShader("teleport_yellow1");
    cgs.media.blueSpawnShader = trap_R_RegisterShader("teleport_blue1");
    cgs.media.cyanSpawnShader = trap_R_RegisterShader("teleport_cyan1");
    cgs.media.purpleSpawnShader = trap_R_RegisterShader("teleport_purple1");
    cgs.media.whiteSpawnShader = trap_R_RegisterShader("teleport_white1");
    cgs.media.blackSplashShader = trap_R_RegisterShader("teleport_black2");
    cgs.media.redSplashShader = trap_R_RegisterShader("teleport_red2");
    cgs.media.greenSplashShader = trap_R_RegisterShader("teleport_green2");
    cgs.media.yellowSplashShader = trap_R_RegisterShader("teleport_yellow2");
    cgs.media.blueSplashShader = trap_R_RegisterShader("teleport_blue2");
    cgs.media.cyanSplashShader = trap_R_RegisterShader("teleport_cyan2");
    cgs.media.purpleSplashShader = trap_R_RegisterShader("teleport_purple2");
    cgs.media.whiteSplashShader = trap_R_RegisterShader("teleport_white2");
    cgs.media.hitSparkShader = trap_R_RegisterShader("xb_hit_spark"); //added for particle effects
    cgs.media.crackMarkShader = trap_R_RegisterShader("xb_crack"); //added for cracked wall marks
    //weapon trails
    cgs.media.xbTrail_Circle = trap_R_RegisterShader("xb_trail_circle");
    cgs.media.xbTrail_Water = trap_R_RegisterShader("xb_trail_water");
    cgs.media.xbTrail_Wind = trap_R_RegisterShader("xb_trail_wind");
    cgs.media.xbTrail_Plant = trap_R_RegisterShader("xb_trail_plant");
    cgs.media.xbTrail_Fire = trap_R_RegisterShader("xb_trail_fire");
    cgs.media.busterTrail1Shader = trap_R_RegisterShader("xb_trail_buster1");
    cgs.media.busterTrail2Shader = trap_R_RegisterShader("xb_trail_buster2");
    cgs.media.busterTrail3Shader = trap_R_RegisterShader("xb_trail_buster3");
    //particle cloud shaders go here
    cgs.media.WeaponModel = trap_R_RegisterShader( "gfx/weaponconcept" );
    cgs.media.xbIceCrystal = trap_R_RegisterModel( "weapons/water/buster_ice.md3" );
    //explosion color variations
    cgs.media.xbExplosionSphere = trap_R_RegisterModel( "models/effects/xb_sphere.md3" );
    cgs.media.xbExplosionShader1 = trap_R_RegisterShader( "z_xb_explos_grey" );
    cgs.media.xbExplosionWhiteShader = trap_R_RegisterShader( "xb_explos_white" );
    cgs.media.xbExplosionBlueShader = trap_R_RegisterShader( "xb_explos_blue" );
    cgs.media.xbExplosionGreenShader = trap_R_RegisterShader( "xb_explos_green" );
    cgs.media.xbExplosionYellowShader = trap_R_RegisterShader( "xb_explos_yellow" );
    cgs.media.xbExplosionPurpleShader = trap_R_RegisterShader( "xb_explos_purple" );
    cgs.media.xbExplosionRedShader = trap_R_RegisterShader( "xb_explos_red" );
    cgs.media.xbExplosionBlackShader = trap_R_RegisterShader( "xb_explos_black" );
    cgs.media.xbExplosionGreyShader = trap_R_RegisterShader ( "xb_explos_grey");
    cgs.media.FireParticle = trap_R_RegisterShader( "weapon/fire/particle" );
    cgs.media.WaterParticle = trap_R_RegisterShader("weapon/water/particle" );
    cgs.media.WindParticle = trap_R_RegisterShader("weapon/wind/particle" );
    cgs.media.EarthParticle = trap_R_RegisterShader("weapon/earth/particle" );
    cgs.media.NatureParticle = trap_R_RegisterShader("weapon/nature/particle" );
    cgs.media.TechParticle = trap_R_RegisterShader("weapon/tech/particle" );
    cgs.media.GravityParticle = trap_R_RegisterShader("weapon/gravity/particle" );
    cgs.media.EnergyParticle = trap_R_RegisterShader("weapon/energy/particle" );
    cgs.media.ElectricParticle = trap_R_RegisterShader("weapon/energy/particle" );
    //effects - GroundFX
    cgs.media.GroundFXplayerSelected1 = trap_R_RegisterShader ("xb_groundfx_player1");
    cgs.media.GroundFXplayerSelected2 = trap_R_RegisterShader ("xb_groundfx_player2");
    cgs.media.GroundFXplayerSelected3 = trap_R_RegisterShader ("xb_groundfx_player3");
    cgs.media.GroundFXplayerSelected4 = trap_R_RegisterShader ("xb_groundfx_player4");
    cgs.media.GroundFXplayerTeamBlue = trap_R_RegisterShader ("xb_groundfx_team_blue");
    cgs.media.GroundFXplayerTeamRed = trap_R_RegisterShader ("xb_groundfx_team_red");
    cgs.media.GroundFXplayerUpgrade = trap_R_RegisterShader ("xb_groundfx_upgrade");
    cgs.media.GroundFXplayerHoming = trap_R_RegisterShader ("xb_groundfx_homing");
    cgs.media.GroundFXplayerLightningStruck = trap_R_RegisterShader ("xb_groundfx_lightning");
    cgs.media.GroundFXplayerTalk	= trap_R_RegisterShader ("xb_groundfx_talk");
    cgs.media.GroundFXplayerCharge = trap_R_RegisterShader ("xb_groundfx_charge");
    cgs.media.GroundFXplayerItem	= trap_R_RegisterShader ("xb_goundfx_item");
    //ScreenFlashFX
    cgs.media.ScreenFlashFX_Lightning1 = trap_R_RegisterShader ("xb_screenfx_lightning1");
    cgs.media.ScreenFlashFX_Red = trap_R_RegisterShader ("xb_screenfx_red");
    cgs.media.ScreenFlashFX_Green = trap_R_RegisterShader ("xb_screenfx_green");
    cgs.media.ScreenFlashFX_Tracking = trap_R_RegisterShader ("xb_screenfx_tracking");
    cgs.media.ScreenFlashFX_Pain = trap_R_RegisterShader ("xb_screenfx_pain");
    cgs.media.ScreenFlashFX_Rage = trap_R_RegisterShader ("xb_screenfx_rage");
    cgs.media.ScreenFlashFX_Cloak = trap_R_RegisterShader ("xb_screenfx_cloak");
    cgs.media.ScreenFlashFX_Burn = trap_R_RegisterShader ("xb_screenfx_burn");
    cgs.media.ScreenFlashFX_Frozen = trap_R_RegisterShader ("xb_screenfx_frozen");
    cgs.media.ScreenFlashFX_Virus = trap_R_RegisterShader ("xb_screenfx_virus");
    cgs.media.ScreenFlashFX_Flash = trap_R_RegisterShader ("xb_screenfx_flash");
    cgs.media.ScreenFlashFX_Sense = trap_R_RegisterShader ("xb_screenfx_sense");
    cgs.media.ScreenFlashFX_Dmg = trap_R_RegisterShader ("xb_screenfx_dmg");
    cgs.media.ScreenFlashFX_Shocked = trap_R_RegisterShader ("xb_screenfx_shocked");
    cgs.media.ScreenFlashFX_Gravity = trap_R_RegisterShader ("xb_screenfx_gravity");
    //upgrade addons go here
    /* Peter FIXME: Were going to replace these with model attachments
    cgs.media.XarmorUpgradeShader = trap_R_RegisterShader("upgrade_mmx_body");
    cgs.media.XbusterUpgradeShader = trap_R_RegisterShader("upgrade_mmx_buster");
    cgs.media.XhelmetUpgradeShader = trap_R_RegisterShader("upgrade_mmx_head");
    cgs.media.XlegsUpgradeShader = trap_R_RegisterShader("upgrade_mmx_legs");
    cgs.media.XbackUpgradeShader = trap_R_RegisterShader("upgrade_mmx_back");
    cgs.media.FarmorUpgradeShader = trap_R_RegisterShader("upgrade_forte_body");
    cgs.media.FbusterUpgradeShader = trap_R_RegisterShader("upgrade_forte_buster");
    cgs.media.FhelmetUpgradeShader = trap_R_RegisterShader("upgrade_forte_head");
    cgs.media.FlegsUpgradeShader = trap_R_RegisterShader("upgrade_forte_legs");
    cgs.media.FbackUpgradeShader = trap_R_RegisterShader("upgrade_forte_back");
    */
    //sounds
    cgs.media.chargeStartSound = trap_S_RegisterSound( "sound/xbuster/weapons/chargeup.wav", qfalse );
    cgs.media.chargeLoopSound = trap_S_RegisterSound( "sound/xbuster/weapons/chargeloop.wav", qfalse );
    cgs.media.heartShader = trap_R_RegisterShaderNoMip( "ui/assets/statusbar/selectedhealth.tga" );

    // breakables
    cgs.media.breakglass01 = trap_R_RegisterModel( "models/mapobjects/func_breakable/glass01.md3" );
    cgs.media.breakglass02 = trap_R_RegisterModel( "models/mapobjects/func_breakable/glass02.md3" );
    cgs.media.breakglass03 = trap_R_RegisterModel( "models/mapobjects/func_breakable/glass03.md3" );

    cgs.media.breakbrick01 = trap_R_RegisterModel( "models/mapobjects/func_breakable/brick01.md3" );
    cgs.media.breakbrick02 = trap_R_RegisterModel( "models/mapobjects/func_breakable/brick02.md3" );
    cgs.media.breakbrick03 = trap_R_RegisterModel( "models/mapobjects/func_breakable/brick03.md3" );

    // Peter: FIXME: May be in the wrong place: loadingscreen
    CG_UpdateGraphicFraction( 0.50f );
    CG_UpdateMediaFraction( 0.70f );

    memset( cg_items, 0, sizeof( cg_items ) );
    memset( cg_weapons, 0, sizeof( cg_weapons ) );

    // only register the items that the server says we need
    strcpy( items, CG_ConfigString( CS_ITEMS) );

    for ( i = 1 ; i < bg_numItems ; i++ )
    {
        if ( items[ i ] == '1' || cg_buildScript.integer )
        {
            CG_LoadingItem( i );
            CG_RegisterItemVisuals( i );
        }
    }

    // only register the NPC that the server says we need
    Q_strncpyz(npcs, CG_ConfigString( CS_NPCS), sizeof(npcs));
    for ( i = 0 ; i < NPC_NUMNPCS ; i++ )
    {
        if ( npcs[ i ] == '1' || cg_buildScript.integer )
        {
            CG_RegisterNPCVisuals( i );
        }
    }

    // wall marks
    cgs.media.bulletMarkShader = trap_R_RegisterShader( "gfx/damage/bullet_mrk" );
    cgs.media.burnMarkShader = trap_R_RegisterShader( "gfx/damage/burn_med_mrk" );
    cgs.media.holeMarkShader = trap_R_RegisterShader( "gfx/damage/hole_lg_mrk" );
    cgs.media.energyMarkShader = trap_R_RegisterShader( "gfx/damage/plasma_mrk" );
    cgs.media.shadowMarkShader = trap_R_RegisterShader( "markShadow" );
    cgs.media.wakeMarkShader = trap_R_RegisterShader( "wake" );

    // Peter: Radar
    cgs.media.radarShader = trap_R_RegisterShader( "radar" );
    cgs.media.rd_up = trap_R_RegisterShader( "rd_up" );
    cgs.media.rd_down = trap_R_RegisterShader( "rd_down" );
    cgs.media.rd_level = trap_R_RegisterShader( "rd_level" );

    // Peter: Player Effects
    cgs.media.pfx_frozen = trap_R_RegisterShader( "player_fx_frozen" );
    cgs.media.pfx_shocked = trap_R_RegisterShader( "player_fx_shocked" );
    cgs.media.pfx_spun =	trap_R_RegisterShader( "player_fx_spun" );
    cgs.media.pfx_poison =	trap_R_RegisterShader( "player_fx_poison" );
    cgs.media.pfx_hit =		trap_R_RegisterShader( "player_fx_hit" );
    cgs.media.pfx_burned = trap_R_RegisterShader( "player_fx_burned" );

    // register the inline models
    cgs.numInlineModels = trap_CM_NumInlineModels();
    for ( i = 1 ; i < cgs.numInlineModels ; i++ )
    {
        char	name[10];
        vec3_t			mins, maxs;
        int				j;

        Com_sprintf( name, sizeof(name), "*%i", i );
        cgs.inlineDrawModel[i] = trap_R_RegisterModel( name );
        trap_R_ModelBounds( cgs.inlineDrawModel[i], mins, maxs );
        for ( j = 0 ; j < 3 ; j++ )
        {
            cgs.inlineModelMidpoints[i][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );
        }
    }

    // register all the server specified models
    for (i=1 ; i<MAX_MODELS ; i++)
    {
        const char		*modelName;

        modelName = CG_ConfigString( CS_MODELS+i );
        if ( !modelName[0] )
        {
            break;
        }
        cgs.gameModels[i] = trap_R_RegisterModel( modelName );
    }

    // loadingscreen
    CG_UpdateGraphicFraction( 0.70f );
    CG_UpdateMediaFraction( 0.85f );

    cgs.media.patrolShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/patrol.tga");
    cgs.media.assaultShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/assault.tga");
    cgs.media.campShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/camp.tga");
    cgs.media.followShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/follow.tga");
    cgs.media.defendShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/defend.tga");
    cgs.media.teamLeaderShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/team_leader.tga");
    cgs.media.retrieveShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/retrieve.tga");
    cgs.media.escortShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/escort.tga");
    cgs.media.sizeCursor = trap_R_RegisterShaderNoMip( "ui/assets/sizecursor.tga" );
    cgs.media.selectCursor = trap_R_RegisterShaderNoMip( "ui/assets/selectcursor.tga" );
    cgs.media.flagShaders[0] = trap_R_RegisterShaderNoMip("ui/assets/statusbar/flag_in_base.tga");
    cgs.media.flagShaders[1] = trap_R_RegisterShaderNoMip("ui/assets/statusbar/flag_capture.tga");
    cgs.media.flagShaders[2] = trap_R_RegisterShaderNoMip("ui/assets/statusbar/flag_missing.tga");

    CG_ClearParticles ();
}



/*
=======================
CG_BuildSpectatorString

=======================
*/
void CG_BuildSpectatorString()
{
    int i;
    cg.spectatorList[0] = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_SPECTATOR )
        {
            Q_strcat(cg.spectatorList, sizeof(cg.spectatorList), va("%s     ", cgs.clientinfo[i].name));
        }
    }
    i = strlen(cg.spectatorList);
    if (i != cg.spectatorLen)
    {
        cg.spectatorLen = i;
        cg.spectatorWidth = -1;
    }
}


/*
===================
CG_RegisterClients
===================
*/
static void CG_RegisterClients( void )
{
    int		i;

    CG_LoadingClient(cg.clientNum);
    CG_NewClientInfo(cg.clientNum);

    for (i=0 ; i<MAX_CLIENTS ; i++)
    {
        const char		*clientInfo;

        if (cg.clientNum == i)
        {
            continue;
        }

        clientInfo = CG_ConfigString( CS_PLAYERS+i );
        if ( !clientInfo[0])
        {
            continue;
        }
        CG_LoadingClient( i );
        CG_NewClientInfo( i );
    }
    CG_BuildSpectatorString();
}


//===========================================================================

/*
=================
CG_ConfigString
=================
*/
const char *CG_ConfigString( int index )
{
    if ( index < 0 || index >= MAX_CONFIGSTRINGS )
    {
        CG_Error( "CG_ConfigString: bad index: %i", index );
    }
    return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}

//==================================================================

/*
======================
CG_StartMusic

======================
*/
void CG_StartMusic( void )
{
    char	*s;
    char	parm1[MAX_QPATH], parm2[MAX_QPATH];

    // start the background music
    s = (char *)CG_ConfigString( CS_MUSIC );
    Q_strncpyz( parm1, COM_Parse( &s ), sizeof( parm1 ) );
    Q_strncpyz( parm2, COM_Parse( &s ), sizeof( parm2 ) );

    trap_S_StartBackgroundTrack( parm1, parm2 );
}

char *CG_GetMenuBuffer(const char *filename)
{
    int	len;
    fileHandle_t	f;
    static char buf[MAX_MENUFILE];

    len = trap_FS_FOpenFile( filename, &f, FS_READ );
    if ( !f )
    {
        trap_Print( va( S_COLOR_RED "menu file not found: %s, using default\n", filename ) );
        return NULL;
    }
    if ( len >= MAX_MENUFILE )
    {
        trap_Print( va( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i", filename, len, MAX_MENUFILE ) );
        trap_FS_FCloseFile( f );
        return NULL;
    }

    trap_FS_Read( buf, len, f );
    buf[len] = 0;
    trap_FS_FCloseFile( f );

    return buf;
}

//
// ==============================
// new hud stuff ( mission pack )
// ==============================
//
qboolean CG_Asset_Parse(int handle)
{
    pc_token_t token;
    const char *tempStr;

    if (!trap_PC_ReadToken(handle, &token))
        return qfalse;
    if (Q_stricmp(token.string, "{") != 0)
    {
        return qfalse;
    }

    while ( 1 )
    {
        if (!trap_PC_ReadToken(handle, &token))
            return qfalse;

        if (Q_stricmp(token.string, "}") == 0)
        {
            return qtrue;
        }

        // font
        if (Q_stricmp(token.string, "font") == 0)
        {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
            {
                return qfalse;
            }
            cgDC.registerFont(tempStr, pointSize, &cgDC.Assets.textFont);
            continue;
        }

        // smallFont
        if (Q_stricmp(token.string, "smallFont") == 0)
        {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
            {
                return qfalse;
            }
            cgDC.registerFont(tempStr, pointSize, &cgDC.Assets.smallFont);
            continue;
        }

        // font
        if (Q_stricmp(token.string, "bigfont") == 0)
        {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
            {
                return qfalse;
            }
            cgDC.registerFont(tempStr, pointSize, &cgDC.Assets.bigFont);
            continue;
        }

        // gradientbar
        if (Q_stricmp(token.string, "gradientbar") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.gradientBar = trap_R_RegisterShaderNoMip(tempStr);
            continue;
        }

        // enterMenuSound
        if (Q_stricmp(token.string, "menuEnterSound") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.menuEnterSound = trap_S_RegisterSound( tempStr, qfalse );
            continue;
        }

        // exitMenuSound
        if (Q_stricmp(token.string, "menuExitSound") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.menuExitSound = trap_S_RegisterSound( tempStr, qfalse );
            continue;
        }

        // itemFocusSound
        if (Q_stricmp(token.string, "itemFocusSound") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.itemFocusSound = trap_S_RegisterSound( tempStr, qfalse );
            continue;
        }

        // menuBuzzSound
        if (Q_stricmp(token.string, "menuBuzzSound") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.menuBuzzSound = trap_S_RegisterSound( tempStr, qfalse );
            continue;
        }

        if (Q_stricmp(token.string, "cursor") == 0)
        {
            if (!PC_String_Parse(handle, &cgDC.Assets.cursorStr))
            {
                return qfalse;
            }
            cgDC.Assets.cursor = trap_R_RegisterShaderNoMip( cgDC.Assets.cursorStr);
            continue;
        }

        if (Q_stricmp(token.string, "fadeClamp") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.fadeClamp))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "fadeCycle") == 0)
        {
            if (!PC_Int_Parse(handle, &cgDC.Assets.fadeCycle))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "fadeAmount") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.fadeAmount))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "shadowX") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.shadowX))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "shadowY") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.shadowY))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "shadowColor") == 0)
        {
            if (!PC_Color_Parse(handle, &cgDC.Assets.shadowColor))
            {
                return qfalse;
            }
            cgDC.Assets.shadowFadeClamp = cgDC.Assets.shadowColor[3];
            continue;
        }
        if (Q_stricmp(token.string, "scrollbarSize") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.scrollbarsize))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "sliderWidth") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.sliderwidth))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "sliderHeight") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.sliderheight))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "sliderthumbWidth") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.sliderthumbwidth))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "sliderthumbHeight") == 0)
        {
            if (!PC_Float_Parse(handle, &cgDC.Assets.sliderthumbheight))
            {
                return qfalse;
            }
            continue;
        }

        if (Q_stricmp(token.string, "sliderBar") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.sliderBar = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "sliderThumb") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "sliderThumbSel") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.sliderThumb_sel = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "scrollBarHorz") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.scrollBarHorz = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "scrollBarVert") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.scrollBarVert = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "scrollBarThumb") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "scrollBarArrowUp") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "scrollBarArrowDown") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "scrollBarArrowLeft") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "scrollBarArrowRight") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxBase") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxRed") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxYellow") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxGreen") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxTeal") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxBlue") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxCyan") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }

        if (Q_stricmp(token.string, "fxWhite") == 0)
        {
            if (!PC_String_Parse(handle, &tempStr))
            {
                return qfalse;
            }
            cgDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip( tempStr);
            continue;
        }
        if (Q_stricmp(token.string, "allHudExtensions") == 0)
        {
            int flag;
            if (!PC_Int_Parse(handle, &flag))
            {
                return qfalse;
            }
            if(flag)
            {
                HudGroupFlag = ALL_GROUPS;
            }
            continue;
        }
        if (Q_stricmp(token.string, "xhairExtension") == 0)
        {
            int flag;
            if (!PC_Int_Parse(handle, &flag))
            {
                return qfalse;
            }
            if(flag)
            {
                HudGroupFlag |= XHAIRNAME;
            }
            continue;
        }
        if (Q_stricmp(token.string, "upperRightExtension") == 0)
        {
            int flag;
            if (!PC_Int_Parse(handle, &flag))
            {
                return qfalse;
            }
            if(flag)
            {
                HudGroupFlag |= UPPERIGHT;
            }
            continue;
        }
        if (Q_stricmp(token.string, "consoleExtension") == 0)
        {
            int flag;
            if (!PC_Int_Parse(handle, &flag))
            {
                return qfalse;
            }
            if(flag)
            {
                HudGroupFlag |= XCONSOLE;
            }
            continue;
        }
        if (Q_stricmp(token.string, "weaponSelectExtension") == 0)
        {
            int flag;
            if (!PC_Int_Parse(handle, &flag))
            {
                return qfalse;
            }
            if(flag)
            {
                HudGroupFlag |= XWEAPONSELECT;
            }
            continue;
        }
        if (Q_stricmp(token.string, "centerPrintExtension") == 0)
        {
            int flag;
            if (!PC_Int_Parse(handle, &flag))
            {
                return qfalse;
            }
            if(flag)
            {
                HudGroupFlag |= XCENTERPRINT;
            }
            continue;
        }
    }
    return qfalse; // bk001204 - why not?
}

void CG_ParseMenu(const char *menuFile)
{
    pc_token_t token;
    int handle;

    handle = trap_PC_LoadSource(menuFile);
    if (!handle)
        handle = trap_PC_LoadSource("ui/testhud.menu");
    if (!handle)
        return;

    while ( 1 )
    {
        if (!trap_PC_ReadToken( handle, &token ))
        {
            break;
        }

        //if ( Q_stricmp( token, "{" ) ) {
        //	Com_Printf( "Missing { in menu file\n" );
        //	break;
        //}

        //if ( menuCount == MAX_MENUS ) {
        //	Com_Printf( "Too many menus!\n" );
        //	break;
        //}

        if ( token.string[0] == '}' )
        {
            break;
        }

        if (Q_stricmp(token.string, "assetGlobalDef") == 0)
        {
            if (CG_Asset_Parse(handle))
            {
                continue;
            }
            else
            {
                break;
            }
        }


        if (Q_stricmp(token.string, "menudef") == 0)
        {
            // start a new menu
            Menu_New(handle);
        }
    }
    trap_PC_FreeSource(handle);
}

qboolean CG_Load_Menu(char **p)
{
    char *token;

    token = COM_ParseExt(p, qtrue);

    if (token[0] != '{')
    {
        return qfalse;
    }

    while ( 1 )
    {

        token = COM_ParseExt(p, qtrue);

        if (Q_stricmp(token, "}") == 0)
        {
            return qtrue;
        }

        if ( !token || token[0] == 0 )
        {
            return qfalse;
        }

        CG_ParseMenu(token);
    }
    return qfalse;
}



void CG_LoadMenus(const char *menuFile)
{
    char	*token;
    char *p;
    int	len, start;
    fileHandle_t	f;
    static char buf[MAX_MENUDEFFILE];

    start = trap_Milliseconds();

    len = trap_FS_FOpenFile( menuFile, &f, FS_READ );
    if ( !f )
    {
        trap_Error( va( S_COLOR_YELLOW "menu file not found: %s, using default\n", menuFile ) );
        len = trap_FS_FOpenFile( "ui/hud.txt", &f, FS_READ );
        if (!f)
        {
            trap_Error( va( S_COLOR_RED "default menu file not found: %s, unable to continue!\n", menuFile ) );
        }
    }

    if ( len >= MAX_MENUDEFFILE )
    {
        trap_Error( va( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i", menuFile, len, MAX_MENUDEFFILE ) );
        trap_FS_FCloseFile( f );
        return;
    }

    trap_FS_Read( buf, len, f );
    buf[len] = 0;
    trap_FS_FCloseFile( f );

    COM_Compress(buf);

    Menu_Reset();

    p = buf;

    while ( 1 )
    {
        token = COM_ParseExt( &p, qtrue );
        if( !token || token[0] == 0 || token[0] == '}')
        {
            break;
        }

        //if ( Q_stricmp( token, "{" ) ) {
        //	Com_Printf( "Missing { in menu file\n" );
        //	break;
        //}

        //if ( menuCount == MAX_MENUS ) {
        //	Com_Printf( "Too many menus!\n" );
        //	break;
        //}

        if ( Q_stricmp( token, "}" ) == 0 )
        {
            break;
        }

        if (Q_stricmp(token, "loadmenu") == 0)
        {
            if (CG_Load_Menu(&p))
            {
                continue;
            }
            else
            {
                break;
            }
        }
    }

    Com_Printf( "UI menu load time = %d milli seconds\n", trap_Milliseconds() - start);

}



static qboolean CG_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key)
{
    return qfalse;
}


static int CG_FeederCount(float feederID)
{
    int i, count;
    count = 0;
    if (feederID == FEEDER_REDTEAM_LIST)
    {
        for (i = 0; i < cg.numScores; i++)
        {
            if (cg.scores[i].team == TEAM_RED)
            {
                count++;
            }
        }
    }
    else if (feederID == FEEDER_BLUETEAM_LIST)
    {
        for (i = 0; i < cg.numScores; i++)
        {
            if (cg.scores[i].team == TEAM_BLUE)
            {
                count++;
            }
        }
    }
    else if (feederID == FEEDER_SCOREBOARD)
    {
        return cg.numScores;
    }
    return count;
}


void CG_SetScoreSelection(void *p)
{
    menuDef_t *menu = (menuDef_t*)p;
    playerState_t *ps = &cg.snap->ps;
    int i, red, blue;
    red = blue = 0;
    for (i = 0; i < cg.numScores; i++)
    {
        if (cg.scores[i].team == TEAM_RED)
        {
            red++;
        }
        else if (cg.scores[i].team == TEAM_BLUE)
        {
            blue++;
        }
        if (ps->clientNum == cg.scores[i].client)
        {
            cg.selectedScore = i;
        }
    }

    if (menu == NULL)
    {
        // just interested in setting the selected score
        return;
    }

    if ( cgs.gametype >= GT_TEAM )
    {
        int feeder = FEEDER_REDTEAM_LIST;
        i = red;
        if (cg.scores[cg.selectedScore].team == TEAM_BLUE)
        {
            feeder = FEEDER_BLUETEAM_LIST;
            i = blue;
        }
        Menu_SetFeederSelection(menu, feeder, i, NULL);
    }
    else
    {
        Menu_SetFeederSelection(menu, FEEDER_SCOREBOARD, cg.selectedScore, NULL);
    }
}

// FIXME: might need to cache this info
static clientInfo_t * CG_InfoFromScoreIndex(int index, int team, int *scoreIndex)
{
    int i, count;
    if ( cgs.gametype >= GT_TEAM )
    {
        count = 0;
        for (i = 0; i < cg.numScores; i++)
        {
            if (cg.scores[i].team == team)
            {
                if (count == index)
                {
                    *scoreIndex = i;
                    return &cgs.clientinfo[cg.scores[i].client];
                }
                count++;
            }
        }
    }
    *scoreIndex = index;
    return &cgs.clientinfo[ cg.scores[index].client ];
}

static const char *CG_FeederItemText(float feederID, int index, int column, qhandle_t *handle)
{
    gitem_t *item;
    int scoreIndex = 0;
    clientInfo_t *info = NULL;
    int team = -1;
    score_t *sp = NULL;

    *handle = -1;

    if (feederID == FEEDER_REDTEAM_LIST)
    {
        team = TEAM_RED;
    }
    else if (feederID == FEEDER_BLUETEAM_LIST)
    {
        team = TEAM_BLUE;
    }

    info = CG_InfoFromScoreIndex(index, team, &scoreIndex);
    sp = &cg.scores[scoreIndex];

    if (info && info->infoValid)
    {
        switch (column)
        {
        case 0:
            if ( info->powerups & ( 1 << PW_NEUTRALFLAG ) )
            {
                item = BG_FindItemForPowerup( PW_NEUTRALFLAG );
                *handle = cg_items[ ITEM_INDEX(item) ].icon;
            }
            else if ( info->powerups & ( 1 << PW_REDFLAG ) )
            {
                item = BG_FindItemForPowerup( PW_REDFLAG );
                *handle = cg_items[ ITEM_INDEX(item) ].icon;
            }
            else if ( info->powerups & ( 1 << PW_BLUEFLAG ) )
            {
                item = BG_FindItemForPowerup( PW_BLUEFLAG );
                *handle = cg_items[ ITEM_INDEX(item) ].icon;
            }
            else
            {
                if ( info->botSkill > 0 && info->botSkill <= 5 )
                {
                    *handle = cgs.media.botSkillShaders[ info->botSkill - 1 ];
                }
                else if ( info->handicap < 100 )
                {
                    return va("%i", info->handicap );
                }
            }
            break;
        case 1:
            if (team == -1)
            {
                return "";
            }
            else
            {
                *handle = CG_StatusHandle(info->teamTask);
            }
            break;
        case 2:
            if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << sp->client ) )
            {
                return "Ready";
            }
            if (team == -1)
            {
                if (cgs.gametype == GT_TOURNAMENT)
                {
                    return va("%i/%i", info->wins, info->losses);
                }
                else if (info->infoValid && info->team == TEAM_SPECTATOR )
                {
                    return "Spectator";
                }
                else
                {
                    return "";
                }
            }
            else
            {
                if (info->teamLeader)
                {
                    return "Leader";
                }
            }
            break;
        case 3:
            return info->name;
            break;
        case 4:
            return va("%i", info->score);
            break;
        case 5:
            return va("%4i", sp->time);
            break;
        case 6:
            if ( sp->ping == -1 )
            {
                return "connecting";
            }
            return va("%4i", sp->ping);
            break;
        }
    }

    return "";
}

static qhandle_t CG_FeederItemImage(float feederID, int index)
{
    return 0;
}

static void CG_FeederSelection(float feederID, int index)
{
    if ( cgs.gametype >= GT_TEAM )
    {
        int i, count;
        int team = (feederID == FEEDER_REDTEAM_LIST) ? TEAM_RED : TEAM_BLUE;
        count = 0;
        for (i = 0; i < cg.numScores; i++)
        {
            if (cg.scores[i].team == team)
            {
                if (index == count)
                {
                    cg.selectedScore = i;
                }
                count++;
            }
        }
    }
    else
    {
        cg.selectedScore = index;
    }
}


static float CG_Cvar_Get(const char *cvar)
{
    char buff[128];
    memset(buff, 0, sizeof(buff));
    trap_Cvar_VariableStringBuffer(cvar, buff, sizeof(buff));
    return atof(buff);
}


void CG_Text_PaintWithCursor(float x, float y, float scale, vec4_t color, const char *text, int cursorPos, char cursor, int limit, int style)
{
    CG_Text_Paint(x, y, scale, color, text, 0, limit, style);
}

static int CG_OwnerDrawWidth(int ownerDraw, float scale)
{
    switch (ownerDraw)
    {
    case CG_GAME_TYPE:
        return CG_Text_Width(CG_GameTypeString(), scale, 0);
    case CG_GAME_STATUS:
        return CG_Text_Width(CG_GetGameStatusText(), scale, 0);
        break;
    case CG_KILLER:
        return CG_Text_Width(CG_GetKillerText(), scale, 0);
        break;
    case CG_RED_NAME:
        return CG_Text_Width(cg_redTeamName.string, scale, 0);
        break;
    case CG_BLUE_NAME:
        return CG_Text_Width(cg_blueTeamName.string, scale, 0);
        break;


    }
    return 0;
}

static int CG_PlayCinematic(const char *name, float x, float y, float w, float h)
{
    return trap_CIN_PlayCinematic(name, x, y, w, h, CIN_loop);
}

static void CG_StopCinematic(int handle)
{
    trap_CIN_StopCinematic(handle);
}

static void CG_DrawCinematic(int handle, float x, float y, float w, float h)
{
    trap_CIN_SetExtents(handle, x, y, w, h);
    trap_CIN_DrawCinematic(handle);
}

static void CG_RunCinematicFrame(int handle)
{
    trap_CIN_RunCinematic(handle);
}

/*
=================
CG_LoadHudMenu();

=================
*/
void CG_LoadHudMenu(void)
{
    char buff[1024];
    const char *hudSet;

    HudGroupFlag = 0;

    cgDC.registerShaderNoMip = &trap_R_RegisterShaderNoMip;
    cgDC.setColor = &trap_R_SetColor;
    cgDC.drawHandlePic = &CG_DrawPic;
    cgDC.drawStretchPic = &trap_R_DrawStretchPic;
    cgDC.drawText = &CG_Text_Paint;
    cgDC.textWidth = &CG_Text_Width;
    cgDC.textHeight = &CG_Text_Height;
    cgDC.registerModel = &trap_R_RegisterModel;
    cgDC.modelBounds = &trap_R_ModelBounds;
    cgDC.fillRect = &CG_FillRect;
    cgDC.drawRect = &CG_DrawRect;
    cgDC.drawSides = &CG_DrawSides;
    cgDC.drawTopBottom = &CG_DrawTopBottom;
    cgDC.clearScene = &trap_R_ClearScene;
    cgDC.addRefEntityToScene = &trap_R_AddRefEntityToScene;
    cgDC.renderScene = &trap_R_RenderScene;
    cgDC.registerFont = &trap_R_RegisterFont;
    cgDC.ownerDrawItem = &CG_OwnerDraw;
    cgDC.getValue = &CG_GetValue;
    cgDC.ownerDrawVisible = &CG_OwnerDrawVisible;
    cgDC.runScript = &CG_RunMenuScript;
    cgDC.setCVar = trap_Cvar_Set;
    cgDC.getCVarString = trap_Cvar_VariableStringBuffer;
    cgDC.getCVarValue = CG_Cvar_Get;
    cgDC.drawTextWithCursor = &CG_Text_PaintWithCursor;
    //cgDC.setOverstrikeMode = &trap_Key_SetOverstrikeMode;
    //cgDC.getOverstrikeMode = &trap_Key_GetOverstrikeMode;
    cgDC.startLocalSound = &trap_S_StartLocalSound;
    cgDC.ownerDrawHandleKey = &CG_OwnerDrawHandleKey;
    cgDC.feederCount = &CG_FeederCount;
    cgDC.feederItemImage = &CG_FeederItemImage;
    cgDC.feederItemText = &CG_FeederItemText;
    cgDC.feederSelection = &CG_FeederSelection;
    //cgDC.setBinding = &trap_Key_SetBinding;
    //cgDC.getBindingBuf = &trap_Key_GetBindingBuf;
    //cgDC.keynumToStringBuf = &trap_Key_KeynumToStringBuf;
    //cgDC.executeText = &trap_Cmd_ExecuteText;
    cgDC.Error = &Com_Error;
    cgDC.Print = &Com_Printf;
    cgDC.ownerDrawWidth = &CG_OwnerDrawWidth;
    //cgDC.Pause = &CG_Pause;
    cgDC.registerSound = &trap_S_RegisterSound;
    cgDC.startBackgroundTrack = &trap_S_StartBackgroundTrack;
    cgDC.stopBackgroundTrack = &trap_S_StopBackgroundTrack;
    cgDC.playCinematic = &CG_PlayCinematic;
    cgDC.stopCinematic = &CG_StopCinematic;
    cgDC.drawCinematic = &CG_DrawCinematic;
    cgDC.runCinematicFrame = &CG_RunCinematicFrame;

    Init_Display(&cgDC);

    Menu_Reset();

    trap_Cvar_VariableStringBuffer("cg_hudFiles", buff, sizeof(buff));
    hudSet = buff;
    if (hudSet[0] == '\0')
    {
        hudSet = "ui/hud.txt";
    }

    CG_LoadMenus(hudSet);
}

void CG_AssetCache(void)
{
    cgDC.Assets.gradientBar = trap_R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
    cgDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
    cgDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
    cgDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
    cgDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
    cgDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_TEAL );
    cgDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
    cgDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
    cgDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
    cgDC.Assets.scrollBarHorz = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
    cgDC.Assets.scrollBarVert = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
    cgDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
    cgDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
    cgDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
    cgDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
    cgDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
    cgDC.Assets.sliderBar = trap_R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
    cgDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );
    cgDC.Assets.sliderThumb_sel = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB_SEL );
    cgDC.Assets.scrollbarsize = SCROLLBAR_SIZE;
    cgDC.Assets.sliderwidth = SLIDER_WIDTH;
    cgDC.Assets.sliderheight = SLIDER_HEIGHT;
    cgDC.Assets.sliderthumbwidth = SLIDER_THUMB_WIDTH;
    cgDC.Assets.sliderthumbheight = SLIDER_THUMB_HEIGHT;
}


/*
=================
CG_Init

Called after every level change or subsystem restart
Will perform callbacks to make the loading info screen update.
=================
*/
void CG_Init( int serverMessageNum, int serverCommandSequence, int clientNum, int randomSeed )
{
    const char	*s;
    const char	*info;
    char	*mapname;

    init_genrand( randomSeed );

    // clear everything
    memset( &cgs, 0, sizeof( cgs ) );
    memset( &cg, 0, sizeof( cg ) );
    memset( cg_entities, 0, sizeof(cg_entities) );
    memset( cg_weapons, 0, sizeof(cg_weapons) );
    memset( cg_items, 0, sizeof(cg_items) );
    memset( cg_npcs, 0, sizeof(cg_npcs) );

    cg.clientNum = clientNum;

    cgs.processedSnapshotNum = serverMessageNum;
    cgs.serverCommandSequence = serverCommandSequence;

    // load a few needed things before we do any screen updates
    cgs.media.charsetShader		= trap_R_RegisterShader( "gfx/2d/bigchars" );
    cgs.media.whiteShader		= trap_R_RegisterShader( "white" );
    cgs.media.charsetProp		= trap_R_RegisterShaderNoMip( "menu/art/font1_prop.tga" );
    cgs.media.charsetPropGlow	= trap_R_RegisterShaderNoMip( "menu/art/font1_prop_glo.tga" );
    cgs.media.charsetPropB		= trap_R_RegisterShaderNoMip( "menu/art/font2_prop.tga" );

    CG_RegisterCvars();

    CG_InitConsoleCommands();

    // loadingscreen
    String_Init();
    CG_AssetCache();
    CG_LoadHudMenu();      // load new hud stuff
    trap_Cvar_Set( "ui_loading", "1" );

    cg.weaponSelect = WP_DEFAULT;

    cgs.redflag = cgs.blueflag = -1; // For compatibily, default to unset for
    cgs.flagStatus = -1;
    // old servers

    // get the rendering configuration from the client system
    trap_GetGlconfig( &cgs.glconfig );
    cgs.screenXScale = cgs.glconfig.vidWidth / 640.0;
    cgs.screenYScale = cgs.glconfig.vidHeight / 480.0;

    // get the gamestate from the client system
    trap_GetGameState( &cgs.gameState );

    // check version
    s = CG_ConfigString( CS_GAME_VERSION );
    if ( strcmp( s, GAME_VERSION ) )
    {
        CG_Error( "Client/Server game mismatch: %s/%s", GAME_VERSION, s );
    }

    s = CG_ConfigString( CS_LEVEL_START_TIME );
    cgs.levelStartTime = atoi( s );

    CG_ParseServerinfo();

    trap_CM_LoadMap( cgs.mapname );

    cg.loading = qtrue;		// force players to load instead of defer

    CG_RegisterSounds();
    CG_UpdateSoundFraction( 1.0f );
    CG_UpdateMediaFraction( 0.60f );
    CG_RegisterGraphics();
    CG_UpdateGraphicFraction( 1.0f );
    CG_UpdateMediaFraction( 0.90f );
    CG_RegisterClients();		// if low on memory, some clients will be deferred
    CG_UpdateMediaFraction( 1.0f );

    cg.loading = qfalse;	// future players will be deferred

    CG_InitLocalEntities();

    CG_InitMarkPolys();

    // remove the last loading update
    cg.infoScreenText[0] = 0;

    // Make sure we have update values (scores)
    CG_SetConfigValues();

    CG_StartMusic();

    CG_LoadingString( "" );


    CG_ShaderStateChanged();

    trap_S_ClearLoopingSounds( qtrue );

    trap_Cvar_Set( "ui_loading", "0" );
    cg.consoleValid = qtrue;

    // Peter FIXME: May be wrong order

    //repress standard Q3 console
    if(HudGroupFlag & XCONSOLE)
        trap_Cvar_Set( "con_notifytime", "-2" );

    // Peter: Start the map music
    info = CG_ConfigString( CS_SERVERINFO );
    mapname = Info_ValueForKey( info, "mapname" );
    cgDC.startBackgroundTrack( va("music/%s.ogg", mapname), va("music/%s.ogg", mapname) );
}

/*
=================
CG_Shutdown

Called before every level change or subsystem restart
=================
*/
void CG_Shutdown( void )
{
    // some mods may need to do cleanup work here,
    // like closing files or archiving session data
}

// Log to a file
void CG_Log( const char *argument, ... )
{
    FILE *fp;
    va_list		argptr;
    char		logp[1024];

    va_start (argptr,argument);
    if (vsprintf (logp, argument, argptr) > sizeof(logp))
    {
        Com_Error ( ERR_DROP, "log overrun" );
    }
    va_end (argptr);

    if ( ( fp = fopen( "log/client.log", "a" ) ) )
    {
        fprintf( fp, "%s", logp );
        fclose ( fp );
    }
}

// Peter: Temporary for initial compile
#include "cg_atmospheric.c"
#include "cg_npcs.c"
#include "../qcommon/xb_math.c"
#include "../qcommon/xb_shared.c"
