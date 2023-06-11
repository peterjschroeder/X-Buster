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

char	*cg_customSoundNames[MAX_CUSTOM_SOUNDS] =
{
    "*death1.wav",
    "*death2.wav",
    "*death3.wav",
    "*jump1.wav",
    "*pain25_1.wav",
    "*pain50_1.wav",
    "*pain75_1.wav",
    "*pain100_1.wav",
    "*falling1.wav",
    "*gasp.wav",
    "*fall1.wav",
    "*taunt.wav",
    "*enter.wav",
    "*victory.wav",
    "*insult.wav",
    "*theme.wav"
};


/*
================
CG_CustomSound

================
*/
sfxHandle_t	CG_CustomSound( int clientNum, const char *soundName )
{
    clientInfo_t *ci;
    int			i;

    if ( soundName[0] != '*' )
    {
        return trap_S_RegisterSound( soundName, qfalse );
    }

    if ( clientNum < 0 || clientNum >= MAX_CLIENTS )
    {
        clientNum = 0;
    }
    ci = &cgs.clientinfo[ clientNum ];

    for ( i = 0 ; i < MAX_CUSTOM_SOUNDS && cg_customSoundNames[i] ; i++ )
    {
        if ( !strcmp( soundName, cg_customSoundNames[i] ) )
        {
            return ci->sounds[i];
        }
    }

    CG_Error( "Unknown custom sound: %s", soundName );
    return 0;
}



/*
=============================================================================

CLIENT INFO

=============================================================================
*/

/*
================
CG_CopyAnimation
================
*/
void CG_CopyAnimation( animation_t *animations, int put, int pick )
{
    animations[put].firstFrame = animations[pick].firstFrame;
    animations[put].numFrames = animations[pick].numFrames;
    animations[put].loopFrames = animations[pick].loopFrames;
    animations[put].frameLerp = animations[pick].frameLerp;
    animations[put].initialLerp = animations[pick].initialLerp;
    animations[put].reversed = animations[pick].reversed;
}

/*
======================
CG_ParseAnimationFile

Read a configuration file containing animation coutns and rates
models/players/visor/animation.cfg, etc
======================
*/
static qboolean	CG_ParseAnimationFile( const char *filename, clientInfo_t *ci )
{
    char		*text_p, *prev;
    int			len;
    int			i;
    char		*token;
    float		fps;
    int			skip;
    char		text[20000];
    fileHandle_t	f;
    animation_t *animations;

    animations = ci->animations;

    // load the file
    len = trap_FS_FOpenFile( filename, &f, FS_READ );
    if ( len <= 0 )
    {
        return qfalse;
    }
    if ( len >= sizeof( text ) - 1 )
    {
        CG_Printf( NULL,"File %s too long\n", filename );
        return qfalse;
    }
    trap_FS_Read( text, len, f );
    text[len] = 0;
    trap_FS_FCloseFile( f );

    // parse the text
    text_p = text;
    skip = 0;	// quite the compiler warning

    ci->footsteps = FOOTSTEP_NORMAL;
    VectorClear( ci->headOffset );
    ci->gender = GENDER_MALE;
    ci->fixedlegs = qfalse;
    ci->fixedtorso = qfalse;

    // read optional parameters
    while ( 1 )
    {
        prev = text_p;	// so we can unget
        token = COM_Parse( &text_p );
        if ( !token )
        {
            break;
        }
        if ( !Q_stricmp( token, "footsteps" ) )
        {
            token = COM_Parse( &text_p );
            if ( !token )
            {
                break;
            }
            if ( !Q_stricmp( token, "default" ) || !Q_stricmp( token, "normal" ) )
            {
                ci->footsteps = FOOTSTEP_NORMAL;
            }
            else if ( !Q_stricmp( token, "boot" ) )
            {
                ci->footsteps = FOOTSTEP_BOOT;
            }
            else if ( !Q_stricmp( token, "flesh" ) )
            {
                ci->footsteps = FOOTSTEP_FLESH;
            }
            else if ( !Q_stricmp( token, "mech" ) )
            {
                ci->footsteps = FOOTSTEP_MECH;
            }
            else if ( !Q_stricmp( token, "energy" ) )
            {
                ci->footsteps = FOOTSTEP_ENERGY;
            }
            else
            {
                CG_Printf( NULL,"Bad footsteps parm in %s: %s\n", filename, token );
            }
            continue;
        }
        else if ( !Q_stricmp( token, "headoffset" ) )
        {
            for ( i = 0 ; i < 3 ; i++ )
            {
                token = COM_Parse( &text_p );
                if ( !token )
                {
                    break;
                }
                ci->headOffset[i] = atof( token );
            }
            continue;
        }
        else if ( !Q_stricmp( token, "sex" ) )
        {
            token = COM_Parse( &text_p );
            if ( !token )
            {
                break;
            }
            if ( token[0] == 'f' || token[0] == 'F' )
            {
                ci->gender = GENDER_FEMALE;
            }
            else if ( token[0] == 'n' || token[0] == 'N' )
            {
                ci->gender = GENDER_NEUTER;
            }
            else
            {
                ci->gender = GENDER_MALE;
            }
            continue;
        }
        else if ( !Q_stricmp( token, "fixedlegs" ) )
        {
            ci->fixedlegs = qtrue;
            continue;
        }
        else if ( !Q_stricmp( token, "fixedtorso" ) )
        {
            ci->fixedtorso = qtrue;
            continue;
        }

        // if it is a number, start parsing animations
        if ( token[0] >= '0' && token[0] <= '9' )
        {
            text_p = prev;	// unget the token
            break;
        }
        Com_Printf( "unknown token '%s' is %s\n", token, filename );
    }

    // read information for each frame
    for ( i = 0 ; i < MAX_ANIMATIONS ; i++ )
    {

        token = COM_Parse( &text_p );
        if ( !*token )
        {
            break;
        }
        animations[i].firstFrame = atoi( token );

        token = COM_Parse( &text_p );
        if ( !*token )
        {
            break;
        }
        animations[i].numFrames = atoi( token );

        animations[i].reversed = qfalse;
        animations[i].flipflop = qfalse;
        // if numFrames is negative the animation is reversed
        if (animations[i].numFrames < 0)
        {
            animations[i].numFrames = -animations[i].numFrames;
            animations[i].reversed = qtrue;
        }

        token = COM_Parse( &text_p );
        if ( !*token )
        {
            break;
        }
        animations[i].loopFrames = atoi( token );

        token = COM_Parse( &text_p );
        if ( !*token )
        {
            break;
        }
        fps = atof( token );
        if ( fps == 0 )
        {
            fps = 1;
        }
        animations[i].frameLerp = 1000 / fps;
        animations[i].initialLerp = 1000 / fps;
    }

    if ( i != MAX_ANIMATIONS )
    {
        CG_Printf( NULL,"Error parsing animation file: %s", filename );
        return qfalse;
    }

    // walk backward animation
    memcpy(&animations[LEGS_BACKWALK], &animations[LEGS_WALK], sizeof(animation_t));
    animations[LEGS_BACKWALK].reversed = qtrue;
    // flag moving fast
    animations[FLAG_RUN].firstFrame = 0;
    animations[FLAG_RUN].numFrames = 16;
    animations[FLAG_RUN].loopFrames = 16;
    animations[FLAG_RUN].frameLerp = 1000 / 15;
    animations[FLAG_RUN].initialLerp = 1000 / 15;
    animations[FLAG_RUN].reversed = qfalse;
    // flag not moving or moving slowly
    animations[FLAG_STAND].firstFrame = 16;
    animations[FLAG_STAND].numFrames = 5;
    animations[FLAG_STAND].loopFrames = 0;
    animations[FLAG_STAND].frameLerp = 1000 / 20;
    animations[FLAG_STAND].initialLerp = 1000 / 20;
    animations[FLAG_STAND].reversed = qfalse;
    // flag speeding up
    animations[FLAG_STAND2RUN].firstFrame = 16;
    animations[FLAG_STAND2RUN].numFrames = 5;
    animations[FLAG_STAND2RUN].loopFrames = 1;
    animations[FLAG_STAND2RUN].frameLerp = 1000 / 15;
    animations[FLAG_STAND2RUN].initialLerp = 1000 / 15;
    animations[FLAG_STAND2RUN].reversed = qtrue;


    // Peter: Fix incomplete animation scripts
    for ( i = 0 ; i < MAX_ANIMATIONS ; i++ )
    {
        if ( !animations[i].firstFrame && !animations[i].numFrames &&
                !animations[i].loopFrames )
        {
            switch (i)
            {
            case TORSO_CHARGE_RUN:
            case TORSO_CHARGE_JUMP:
            case TORSO_CHARGE_DASH:
            case TORSO_CHARGE:
            case TORSO_CHARGE_HOLD:
            case TORSO_CHARGE2_RUN:
            case TORSO_CHARGE2_DASH:
                CG_CopyAnimation( animations, i, TORSO_CHARGE_HOLD);
                break;
            case TORSO_DASH_FIN:
            case TORSO_DASH_B:
                CG_CopyAnimation( animations, i, TORSO_DASH);
                break;
            case LEGS_DASH_FIN:
            case LEGS_DASH_B:
                CG_CopyAnimation( animations, i, LEGS_DASH);
                break;
            case TORSO_ATTACK_JUMP:
            case TORSO_ATTACK_DASH:
            case TORSO_ATTACK_RUN:
            case TORSO_ATTACK_FIN:
            case TORSO_ATTACK2_JUMP:
            case TORSO_ATTACK2_DASH:
            case TORSO_ATTACK2_RUN:
                CG_CopyAnimation( animations, i, TORSO_ATTACK);
                break;
            case TORSO_JUMPB:
                CG_CopyAnimation( animations, i, TORSO_JUMP);
                break;
            case TORSO_FALLB:
                CG_CopyAnimation( animations, i, TORSO_FALL);
                break;
            }
        }
    }

    return qtrue;
}

/*
==========================
CG_FileExists
==========================
*/
static qboolean	CG_FileExists(const char *filename)
{
    int len;

    len = trap_FS_FOpenFile( filename, 0, FS_READ );
    if (len>0)
    {
        return qtrue;
    }
    return qfalse;
}

/*
==========================
CG_FindClientModelFile
==========================
*/
static qboolean	CG_FindClientModelFile( char *filename, int length, clientInfo_t *ci, const char *teamName, const char *modelName, const char *skinName, const char *base, const char *ext )
{
    char *team, *charactersFolder;
    int i;

    if ( cgs.gametype >= GT_TEAM )
    {
        switch ( ci->team )
        {
        case TEAM_BLUE:
        {
            team = "blue";
            break;
        }
        default:
        {
            team = "red";
            break;
        }
        }
    }
    else
    {
        team = "default";
    }
    charactersFolder = "";
    while(1)
    {
        for ( i = 0; i < 2; i++ )
        {
            if ( i == 0 && teamName && *teamName )
            {
                //								"models/players/characters/james/stroggs/lower_lily_red.skin"
                Com_sprintf( filename, length, "models/players/%s%s/%s%s_%s_%s.%s", charactersFolder, modelName, teamName, base, skinName, team, ext );
            }
            else
            {
                //								"models/players/characters/james/lower_lily_red.skin"
                Com_sprintf( filename, length, "models/players/%s%s/%s_%s_%s.%s", charactersFolder, modelName, base, skinName, team, ext );
            }
            if ( CG_FileExists( filename ) )
            {
                return qtrue;
            }
            if ( cgs.gametype >= GT_TEAM )
            {
                if ( i == 0 && teamName && *teamName )
                {
                    //								"models/players/characters/james/stroggs/lower_red.skin"
                    Com_sprintf( filename, length, "models/players/%s%s/%s%s_%s.%s", charactersFolder, modelName, teamName, base, team, ext );
                }
                else
                {
                    //								"models/players/characters/james/lower_red.skin"
                    Com_sprintf( filename, length, "models/players/%s%s/%s_%s.%s", charactersFolder, modelName, base, team, ext );
                }
            }
            else
            {
                if ( i == 0 && teamName && *teamName )
                {
                    //								"models/players/characters/james/stroggs/lower_lily.skin"
                    Com_sprintf( filename, length, "models/players/%s%s/%s%s_%s.%s", charactersFolder, modelName, teamName, base, skinName, ext );
                }
                else
                {
                    //								"models/players/characters/james/lower_lily.skin"
                    Com_sprintf( filename, length, "models/players/%s%s/%s_%s.%s", charactersFolder, modelName, base, skinName, ext );
                }
            }
            if ( CG_FileExists( filename ) )
            {
                return qtrue;
            }
            if ( !teamName || !*teamName )
            {
                break;
            }
        }
        // if tried the heads folder first
        if ( charactersFolder[0] )
        {
            break;
        }
        charactersFolder = "characters/";
    }

    return qfalse;
}

/*
==========================
CG_FindClientHeadFile
==========================
*/
static qboolean	CG_FindClientHeadFile( char *filename, int length, clientInfo_t *ci, const char *teamName, const char *headModelName, const char *headSkinName, const char *base, const char *ext )
{
    char *team, *headsFolder;
    int i;

    if ( cgs.gametype >= GT_TEAM )
    {
        switch ( ci->team )
        {
        case TEAM_BLUE:
        {
            team = "blue";
            break;
        }
        default:
        {
            team = "red";
            break;
        }
        }
    }
    else
    {
        team = "default";
    }

    if ( headModelName[0] == '*' )
    {
        headsFolder = "heads/";
        headModelName++;
    }
    else
    {
        headsFolder = "";
    }
    while(1)
    {
        for ( i = 0; i < 2; i++ )
        {
            if ( i == 0 && teamName && *teamName )
            {
                Com_sprintf( filename, length, "models/players/%s%s/%s/%s%s_%s.%s", headsFolder, headModelName, headSkinName, teamName, base, team, ext );
            }
            else
            {
                Com_sprintf( filename, length, "models/players/%s%s/%s/%s_%s.%s", headsFolder, headModelName, headSkinName, base, team, ext );
            }
            if ( CG_FileExists( filename ) )
            {
                return qtrue;
            }
            if ( cgs.gametype >= GT_TEAM )
            {
                if ( i == 0 &&  teamName && *teamName )
                {
                    Com_sprintf( filename, length, "models/players/%s%s/%s%s_%s.%s", headsFolder, headModelName, teamName, base, team, ext );
                }
                else
                {
                    Com_sprintf( filename, length, "models/players/%s%s/%s_%s.%s", headsFolder, headModelName, base, team, ext );
                }
            }
            else
            {
                if ( i == 0 && teamName && *teamName )
                {
                    Com_sprintf( filename, length, "models/players/%s%s/%s%s_%s.%s", headsFolder, headModelName, teamName, base, headSkinName, ext );
                }
                else
                {
                    Com_sprintf( filename, length, "models/players/%s%s/%s_%s.%s", headsFolder, headModelName, base, headSkinName, ext );
                }
            }
            if ( CG_FileExists( filename ) )
            {
                return qtrue;
            }
            if ( !teamName || !*teamName )
            {
                break;
            }
        }
        // if tried the heads folder first
        if ( headsFolder[0] )
        {
            break;
        }
        headsFolder = "heads/";
    }

    return qfalse;
}

/*
==========================
CG_RegisterClientSkin
==========================
*/
static qboolean	CG_RegisterClientSkin( clientInfo_t *ci, const char *teamName, const char *modelName, const char *skinName, const char *headModelName, const char *headSkinName )
{
    char filename[MAX_QPATH];

    /*
    Com_sprintf( filename, sizeof( filename ), "models/players/%s/%slower_%s.skin", modelName, teamName, skinName );
    ci->legsSkin = trap_R_RegisterSkin( filename );
    if (!ci->legsSkin) {
    	Com_sprintf( filename, sizeof( filename ), "models/players/characters/%s/%slower_%s.skin", modelName, teamName, skinName );
    	ci->legsSkin = trap_R_RegisterSkin( filename );
    	if (!ci->legsSkin) {
    		Com_Printf( "Leg skin load failure: %s\n", filename );
    	}
    }


    Com_sprintf( filename, sizeof( filename ), "models/players/%s/%supper_%s.skin", modelName, teamName, skinName );
    ci->torsoSkin = trap_R_RegisterSkin( filename );
    if (!ci->torsoSkin) {
    	Com_sprintf( filename, sizeof( filename ), "models/players/characters/%s/%supper_%s.skin", modelName, teamName, skinName );
    	ci->torsoSkin = trap_R_RegisterSkin( filename );
    	if (!ci->torsoSkin) {
    		Com_Printf( "Torso skin load failure: %s\n", filename );
    	}
    }
    */
    if ( CG_FindClientModelFile( filename, sizeof(filename), ci, teamName, modelName, skinName, "lower", "skin" ) )
    {
        ci->legsSkin = trap_R_RegisterSkin( filename );
    }
    if (!ci->legsSkin)
    {
        Com_Printf( "Leg skin load failure: %s\n", filename );
    }

    if ( CG_FindClientModelFile( filename, sizeof(filename), ci, teamName, modelName, skinName, "upper", "skin" ) )
    {
        ci->torsoSkin = trap_R_RegisterSkin( filename );
    }
    if (!ci->torsoSkin)
    {
        Com_Printf( "Torso skin load failure: %s\n", filename );
    }

    if ( CG_FindClientHeadFile( filename, sizeof(filename), ci, teamName, headModelName, headSkinName, "head", "skin" ) )
    {
        ci->headSkin = trap_R_RegisterSkin( filename );
    }
    if (!ci->headSkin)
    {
        Com_Printf( "Head skin load failure: %s\n", filename );
    }
    // for Zero's hair
    if ( CG_FindClientHeadFile( filename, sizeof(filename), ci, teamName, headModelName, headSkinName, "hair", "skin" ) )
    {
        ci->hairSkin = trap_R_RegisterSkin( filename );
    }
    if (!ci->hairSkin)
    {
        CG_Log( "Hair skin load failure: %s\n", filename );
    }

    // if any skins failed to load
    if ( !ci->legsSkin || !ci->torsoSkin || !ci->headSkin )
    {
        return qfalse;
    }
    return qtrue;
}

/*
==========================
CG_RegisterClientModelname
==========================
*/
static qboolean CG_RegisterClientModelname( clientInfo_t *ci, const char *modelName, const char *skinName, const char *headModelName, const char *headSkinName, const char *teamName )
{
    char	filename[MAX_QPATH*2];
    const char		*headName;
    char newTeamName[MAX_QPATH*2];

    if ( headModelName[0] == '\0' )
    {
        headName = modelName;
    }
    else
    {
        headName = headModelName;
    }

    // Peter: Don't load non-X-buster player models
    if (strcmp(modelName, "mmx") && strcmp(modelName, "megaman") &&
            strcmp(modelName, "forte") && strcmp(modelName, "proto") &&
            strcmp(modelName, "zero") && strcmp(modelName, "zero_s") &&
            strcmp(modelName, "vile") && strcmp(modelName, "burnerman"))
        return qfalse;

    Com_sprintf( filename, sizeof( filename ), "models/players/%s/lower.md3", modelName );
    ci->legsModel = trap_R_RegisterModel( filename );
    if ( !ci->legsModel )
    {
        Com_sprintf( filename, sizeof( filename ), "models/players/characters/%s/lower.md3", modelName );
        ci->legsModel = trap_R_RegisterModel( filename );
        if ( !ci->legsModel )
        {
            Com_Printf( "Failed to load model file %s\n", filename );
            return qfalse;
        }
    }

    Com_sprintf( filename, sizeof( filename ), "models/players/%s/upper.md3", modelName );
    ci->torsoModel = trap_R_RegisterModel( filename );
    if ( !ci->torsoModel )
    {
        Com_sprintf( filename, sizeof( filename ), "models/players/characters/%s/upper.md3", modelName );
        ci->torsoModel = trap_R_RegisterModel( filename );
        if ( !ci->torsoModel )
        {
            Com_Printf( "Failed to load model file %s\n", filename );
            return qfalse;
        }
    }

    if( headName[0] == '*' )
    {
        Com_sprintf( filename, sizeof( filename ), "models/players/heads/%s/%s.md3", &headModelName[1], &headModelName[1] );
    }
    else
    {
        Com_sprintf( filename, sizeof( filename ), "models/players/%s/head.md3", headName );
    }
    ci->headModel = trap_R_RegisterModel( filename );
    // if the head model could not be found and we didn't load from the heads folder try to load from there
    if ( !ci->headModel && headName[0] != '*' )
    {
        Com_sprintf( filename, sizeof( filename ), "models/players/heads/%s/%s.md3", headModelName, headModelName );
        ci->headModel = trap_R_RegisterModel( filename );
    }
    if ( !ci->headModel )
    {
        Com_Printf( "Failed to load model file %s\n", filename );
        return qfalse;
    }

    // load Zero's hair
    Com_sprintf( filename, sizeof( filename ), "models/players/%s/hair.md3", modelName );
    //CG_Printf( NULL,"hairModel is %s\n", filename );
    ci->hairModel = trap_R_RegisterModel( filename );

    // if any skins failed to load, return failure
    if ( !CG_RegisterClientSkin( ci, teamName, modelName, skinName, headName, headSkinName ) )
    {
        if ( teamName && *teamName)
        {
            Com_Printf( "Failed to load skin file: %s : %s : %s, %s : %s\n", teamName, modelName, skinName, headName, headSkinName );
            if( ci->team == TEAM_BLUE )
            {
                Com_sprintf(newTeamName, sizeof(newTeamName), "%s/", DEFAULT_BLUETEAM_NAME);
            }
            else
            {
                Com_sprintf(newTeamName, sizeof(newTeamName), "%s/", DEFAULT_REDTEAM_NAME);
            }
            if ( !CG_RegisterClientSkin( ci, newTeamName, modelName, skinName, headName, headSkinName ) )
            {
                Com_Printf( "Failed to load skin file: %s : %s : %s, %s : %s\n", newTeamName, modelName, skinName, headName, headSkinName );
                return qfalse;
            }
        }
        else
        {
            Com_Printf( "Failed to load skin file: %s : %s, %s : %s\n", modelName, skinName, headName, headSkinName );
            return qfalse;
        }
    }

    // load the animations
    Com_sprintf( filename, sizeof( filename ), "models/players/%s/animation.cfg", modelName );
    if ( !CG_ParseAnimationFile( filename, ci ) )
    {
        Com_sprintf( filename, sizeof( filename ), "models/players/characters/%s/animation.cfg", modelName );
        if ( !CG_ParseAnimationFile( filename, ci ) )
        {
            Com_Printf( "Failed to load animation file %s\n", filename );
            return qfalse;
        }
    }

    if ( CG_FindClientHeadFile( filename, sizeof(filename), ci, teamName, headName, headSkinName, "icon", "skin" ) )
    {
        ci->modelIcon = trap_R_RegisterShaderNoMip( filename );
    }
    else if ( CG_FindClientHeadFile( filename, sizeof(filename), ci, teamName, headName, headSkinName, "icon", "tga" ) )
    {
        ci->modelIcon = trap_R_RegisterShaderNoMip( filename );
    }

    if ( !ci->modelIcon )
    {
        return qfalse;
    }

    return qtrue;
}

/*
====================
CG_ColorFromString
====================
*/
static void CG_ColorFromString( const char *v, vec3_t color )
{
    int val;

    VectorClear( color );

    val = atoi( v );

    if ( val < 1 || val > 7 )
    {
        VectorSet( color, 1, 1, 1 );
        return;
    }

    if ( val & 1 )
    {
        color[2] = 1.0f;
    }
    if ( val & 2 )
    {
        color[1] = 1.0f;
    }
    if ( val & 4 )
    {
        color[0] = 1.0f;
    }
}

/*
===================
CG_LoadClientInfo

Load it now, taking the disk hits.
This will usually be deferred to a safe time
===================
*/
static void CG_LoadClientInfo( clientInfo_t *ci )
{
    const char	*dir, *fallback;
    int			i, modelloaded;
    const char	*s;
    int			clientNum;
    char		teamname[MAX_QPATH];

    teamname[0] = 0;
    modelloaded = qtrue;

    if ( !CG_RegisterClientModelname( ci, ci->modelName, ci->skinName, ci->headModelName, ci->headSkinName, teamname ) )
    {
        if ( cg_buildScript.integer )
        {
            CG_Error( "CG_RegisterClientModelname( %s, %s, %s, %s %s ) failed", ci->modelName, ci->skinName, ci->headModelName, ci->headSkinName, teamname );
        }

        // fall back to default team name
        if( cgs.gametype >= GT_TEAM)
        {
            // keep skin name
            if( ci->team == TEAM_BLUE )
            {
                Q_strncpyz(teamname, DEFAULT_BLUETEAM_NAME, sizeof(teamname) );
            }
            else
            {
                Q_strncpyz(teamname, DEFAULT_REDTEAM_NAME, sizeof(teamname) );
            }
            if ( !CG_RegisterClientModelname( ci, DEFAULT_TEAM_MODEL, ci->skinName, DEFAULT_TEAM_HEAD, ci->skinName, teamname ) )
            {
                CG_Error( "DEFAULT_TEAM_MODEL / skin (%s/%s) failed to register", DEFAULT_TEAM_MODEL, ci->skinName );
            }
        }
        else
        {
            if ( !CG_RegisterClientModelname( ci, DEFAULT_MODEL, "default", DEFAULT_MODEL, "default", teamname ) )
            {
                CG_Error( "DEFAULT_MODEL (%s) failed to register", DEFAULT_MODEL );
            }
        }
        modelloaded = qfalse;
    }

    ci->newAnims = qfalse;
    if ( ci->torsoModel )
    {
        orientation_t tag;
        // if the torso model has the "tag_flag"
        if ( trap_R_LerpTag( &tag, ci->torsoModel, 0, 0, 1, "tag_flag" ) )
        {
            ci->newAnims = qtrue;
        }
    }

    // sounds
    dir = ci->modelName;
    fallback = (cgs.gametype >= GT_TEAM) ? DEFAULT_TEAM_MODEL : DEFAULT_MODEL;

    for ( i = 0 ; i < MAX_CUSTOM_SOUNDS ; i++ )
    {
        s = cg_customSoundNames[i];
        if ( !s )
        {
            break;
        }
        ci->sounds[i] = 0;
        // if the model didn't load use the sounds of the default model
        if (modelloaded)
        {
            ci->sounds[i] = trap_S_RegisterSound( va("sound/player/%s/%s", dir, s + 1), qfalse );
        }
        if ( !ci->sounds[i] )
        {
            ci->sounds[i] = trap_S_RegisterSound( va("sound/player/%s/%s", fallback, s + 1), qfalse );
        }
    }

    ci->deferred = qfalse;

    // reset any existing players and bodies, because they might be in bad
    // frames for this new model
    clientNum = ci - cgs.clientinfo;
    for ( i = 0 ; i < MAX_GENTITIES ; i++ )
    {
        if ( cg_entities[i].currentState.clientNum == clientNum
                && cg_entities[i].currentState.eType == ET_PLAYER )
        {
            CG_ResetPlayerEntity( &cg_entities[i] );
        }
    }
}

/*
======================
CG_CopyClientInfoModel
======================
*/
static void CG_CopyClientInfoModel( clientInfo_t *from, clientInfo_t *to )
{
    VectorCopy( from->headOffset, to->headOffset );
    to->footsteps = from->footsteps;
    to->gender = from->gender;

    to->legsModel = from->legsModel;
    to->legsSkin = from->legsSkin;
    to->torsoModel = from->torsoModel;
    to->torsoSkin = from->torsoSkin;
    to->headModel = from->headModel;
    to->headSkin = from->headSkin;
    to->hairModel = from->hairModel;
    to->hairSkin = from->hairSkin;

    to->modelIcon = from->modelIcon;

    to->newAnims = from->newAnims;

    memcpy( to->animations, from->animations, sizeof( to->animations ) );
    memcpy( to->sounds, from->sounds, sizeof( to->sounds ) );
}

/*
======================
CG_ScanForExistingClientInfo
======================
*/
static qboolean CG_ScanForExistingClientInfo( clientInfo_t *ci )
{
    int		i;
    clientInfo_t	*match;

    for ( i = 0 ; i < cgs.maxclients ; i++ )
    {
        match = &cgs.clientinfo[ i ];
        if ( !match->infoValid )
        {
            continue;
        }
        if ( match->deferred )
        {
            continue;
        }
        if ( !Q_stricmp( ci->modelName, match->modelName )
                && !Q_stricmp( ci->skinName, match->skinName )
                && !Q_stricmp( ci->headModelName, match->headModelName )
                && !Q_stricmp( ci->headSkinName, match->headSkinName )
                && !Q_stricmp( ci->blueTeam, match->blueTeam )
                && !Q_stricmp( ci->redTeam, match->redTeam )
                && (cgs.gametype < GT_TEAM || ci->team == match->team) )
        {
            // this clientinfo is identical, so use it's handles

            ci->deferred = qfalse;

            CG_CopyClientInfoModel( match, ci );

            return qtrue;
        }
    }

    // nothing matches, so defer the load
    return qfalse;
}

/*
======================
CG_SetDeferredClientInfo

We aren't going to load it now, so grab some other
client's info to use until we have some spare time.
======================
*/
static void CG_SetDeferredClientInfo( clientInfo_t *ci )
{
    int		i;
    clientInfo_t	*match;

    // if someone else is already the same models and skins we
    // can just load the client info
    for ( i = 0 ; i < cgs.maxclients ; i++ )
    {
        match = &cgs.clientinfo[ i ];
        if ( !match->infoValid || match->deferred )
        {
            continue;
        }
        if ( Q_stricmp( ci->skinName, match->skinName ) ||
                Q_stricmp( ci->modelName, match->modelName ) ||
//			 Q_stricmp( ci->headModelName, match->headModelName ) ||
//			 Q_stricmp( ci->headSkinName, match->headSkinName ) ||
                (cgs.gametype >= GT_TEAM && ci->team != match->team) )
        {
            continue;
        }
        // just load the real info cause it uses the same models and skins
        CG_LoadClientInfo( ci );
        return;
    }

    // if we are in teamplay, only grab a model if the skin is correct
    if ( cgs.gametype >= GT_TEAM )
    {
        for ( i = 0 ; i < cgs.maxclients ; i++ )
        {
            match = &cgs.clientinfo[ i ];
            if ( !match->infoValid || match->deferred )
            {
                continue;
            }
            if ( Q_stricmp( ci->skinName, match->skinName ) ||
                    (cgs.gametype >= GT_TEAM && ci->team != match->team) )
            {
                continue;
            }
            ci->deferred = qtrue;
            CG_CopyClientInfoModel( match, ci );
            return;
        }
        // load the full model, because we don't ever want to show
        // an improper team skin.  This will cause a hitch for the first
        // player, when the second enters.  Combat shouldn't be going on
        // yet, so it shouldn't matter
        CG_LoadClientInfo( ci );
        return;
    }

    // find the first valid clientinfo and grab its stuff
    for ( i = 0 ; i < cgs.maxclients ; i++ )
    {
        match = &cgs.clientinfo[ i ];
        if ( !match->infoValid )
        {
            continue;
        }

        ci->deferred = qtrue;
        CG_CopyClientInfoModel( match, ci );
        return;
    }

    // we should never get here...
    CG_Printf( NULL,"CG_SetDeferredClientInfo: no valid clients!\n" );

    CG_LoadClientInfo( ci );
}


/*
======================
CG_NewClientInfo
======================
*/
void CG_NewClientInfo( int clientNum )
{
    clientInfo_t *ci;
    clientInfo_t newInfo;
    const char	*configstring;
    const char	*v;
    char		*slash;

    ci = &cgs.clientinfo[clientNum];

    configstring = CG_ConfigString( clientNum + CS_PLAYERS );
    if ( !configstring[0] )
    {
        memset( ci, 0, sizeof( *ci ) );
        return;		// player just left
    }

    // build into a temp buffer so the defer checks can use
    // the old value
    memset( &newInfo, 0, sizeof( newInfo ) );

    // isolate the player's name
    v = Info_ValueForKey(configstring, "n");
    Q_strncpyz( newInfo.name, v, sizeof( newInfo.name ) );

    // colors
    v = Info_ValueForKey( configstring, "c" );
    newInfo.color = atoi( v );

    v = Info_ValueForKey( configstring, "c1" );
    CG_ColorFromString( v, newInfo.color1 );

    v = Info_ValueForKey( configstring, "c2" );
    CG_ColorFromString( v, newInfo.color2 );

    // bot skill
    v = Info_ValueForKey( configstring, "skill" );
    newInfo.botSkill = atoi( v );

    // handicap
    v = Info_ValueForKey( configstring, "hc" );
    newInfo.handicap = atoi( v );

    // wins
    v = Info_ValueForKey( configstring, "w" );
    newInfo.wins = atoi( v );

    // losses
    v = Info_ValueForKey( configstring, "l" );
    newInfo.losses = atoi( v );

    // team
    v = Info_ValueForKey( configstring, "t" );
    newInfo.team = atoi( v );

    // team task
    v = Info_ValueForKey( configstring, "tt" );
    newInfo.teamTask = atoi(v);

    // team leader
    v = Info_ValueForKey( configstring, "tl" );
    newInfo.teamLeader = atoi(v);

    v = Info_ValueForKey( configstring, "g_redteam" );
    Q_strncpyz(newInfo.redTeam, v, MAX_TEAMNAME);

    v = Info_ValueForKey( configstring, "g_blueteam" );
    Q_strncpyz(newInfo.blueTeam, v, MAX_TEAMNAME);

    // model
    v = Info_ValueForKey( configstring, "model" );
    if ( cg_forceModel.integer )
    {
        // forcemodel makes everyone use a single model
        // to prevent load hitches
        char modelStr[MAX_QPATH];
        char *skin;

        if( cgs.gametype >= GT_TEAM )
        {
            Q_strncpyz( newInfo.modelName, DEFAULT_TEAM_MODEL, sizeof( newInfo.modelName ) );
            Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );
        }
        else
        {
            trap_Cvar_VariableStringBuffer( "model", modelStr, sizeof( modelStr ) );
            if ( ( skin = strchr( modelStr, '/' ) ) == NULL)
            {
                skin = "default";
            }
            else
            {
                *skin++ = 0;
            }

            Q_strncpyz( newInfo.skinName, skin, sizeof( newInfo.skinName ) );
            Q_strncpyz( newInfo.modelName, modelStr, sizeof( newInfo.modelName ) );
        }

        if ( cgs.gametype >= GT_TEAM )
        {
            // keep skin name
            slash = strchr( v, '/' );
            if ( slash )
            {
                Q_strncpyz( newInfo.skinName, slash + 1, sizeof( newInfo.skinName ) );
            }
        }
    }
    else
    {
        Q_strncpyz( newInfo.modelName, v, sizeof( newInfo.modelName ) );

        slash = strchr( newInfo.modelName, '/' );
        if ( !slash )
        {
            // modelName didn not include a skin name
            Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );
        }
        else
        {
            Q_strncpyz( newInfo.skinName, slash + 1, sizeof( newInfo.skinName ) );
            // truncate modelName
            *slash = 0;
        }
    }

    // head model
    v = Info_ValueForKey( configstring, "hmodel" );
    if ( cg_forceModel.integer )
    {
        // forcemodel makes everyone use a single model
        // to prevent load hitches
        char modelStr[MAX_QPATH];
        char *skin;

        if( cgs.gametype >= GT_TEAM )
        {
            Q_strncpyz( newInfo.headModelName, DEFAULT_TEAM_MODEL, sizeof( newInfo.headModelName ) );
            Q_strncpyz( newInfo.headSkinName, "default", sizeof( newInfo.headSkinName ) );
        }
        else
        {
            trap_Cvar_VariableStringBuffer( "headmodel", modelStr, sizeof( modelStr ) );
            if ( ( skin = strchr( modelStr, '/' ) ) == NULL)
            {
                skin = "default";
            }
            else
            {
                *skin++ = 0;
            }

            Q_strncpyz( newInfo.headSkinName, skin, sizeof( newInfo.headSkinName ) );
            Q_strncpyz( newInfo.headModelName, modelStr, sizeof( newInfo.headModelName ) );
        }

        if ( cgs.gametype >= GT_TEAM )
        {
            // keep skin name
            slash = strchr( v, '/' );
            if ( slash )
            {
                Q_strncpyz( newInfo.headSkinName, slash + 1, sizeof( newInfo.headSkinName ) );
            }
        }
    }
    else
    {
        Q_strncpyz( newInfo.headModelName, v, sizeof( newInfo.headModelName ) );

        slash = strchr( newInfo.headModelName, '/' );
        if ( !slash )
        {
            // modelName didn not include a skin name
            Q_strncpyz( newInfo.headSkinName, "default", sizeof( newInfo.headSkinName ) );
        }
        else
        {
            Q_strncpyz( newInfo.headSkinName, slash + 1, sizeof( newInfo.headSkinName ) );
            // truncate modelName
            *slash = 0;
        }
    }

    // scan for an existing clientinfo that matches this modelname
    // so we can avoid loading checks if possible
    if ( !CG_ScanForExistingClientInfo( &newInfo ) )
    {
        qboolean	forceDefer;

        forceDefer = trap_MemoryRemaining() < 4000000;

        // if we are defering loads, just have it pick the first valid
        if ( forceDefer /* Peter FIXME: Pass g_autoskin to cgame so we can check || ( cg_deferPlayers.integer && !cg_buildScript.integer && !cg.loading ) */)
        {
            // keep whatever they had if it won't violate team skins
            CG_SetDeferredClientInfo( &newInfo );
            // if we are low on memory, leave them with this model
            if ( forceDefer )
            {
                CG_Printf( NULL,"Memory is low.  Using deferred model.\n" );
                newInfo.deferred = qfalse;
            }
        }
        else
        {
            CG_LoadClientInfo( &newInfo );
        }
    }

    // replace whatever was there with the new one
    newInfo.infoValid = qtrue;
    *ci = newInfo;
}



/*
======================
CG_LoadDeferredPlayers

Called each frame when a player is dead
and the scoreboard is up
so deferred players can be loaded
======================
*/
void CG_LoadDeferredPlayers( void )
{
    int		i;
    clientInfo_t	*ci;

    // scan for a deferred player to load
    for ( i = 0, ci = cgs.clientinfo ; i < cgs.maxclients ; i++, ci++ )
    {
        if ( ci->infoValid && ci->deferred )
        {
            // if we are low on memory, leave it deferred
            if ( trap_MemoryRemaining() < 4000000 )
            {
                CG_Printf( NULL,"Memory is low.  Using deferred model.\n" );
                ci->deferred = qfalse;
                continue;
            }
            CG_LoadClientInfo( ci );
//			break;
        }
    }
}

/*
=============================================================================

PLAYER ANIMATION

=============================================================================
*/


/*
===============
CG_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void CG_SetLerpFrameAnimation( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation )
{
    animation_t	*anim;

    lf->animationNumber = newAnimation;
    newAnimation &= ~ANIM_TOGGLEBIT;

    if ( newAnimation < 0 || newAnimation >= MAX_TOTALANIMATIONS )
    {
        CG_Error( "Bad animation number: %i", newAnimation );
    }

    anim = &ci->animations[ newAnimation ];

    lf->animation = anim;
    lf->animationTime = lf->frameTime + anim->initialLerp;

    if ( cg_debugAnim.integer )
    {
        CG_Printf( NULL,"Anim: %i\n", newAnimation );
    }
}

static void CG_SetLerpFrameAnimationNPC( animation_t *ai, lerpFrame_t *lf, int newAnimation )
{
    animation_t	*anim;
    lf->animationNumber = newAnimation;
    newAnimation &= ~ANIM_TOGGLEBIT;
    if ( newAnimation < 0 || newAnimation >= MAX_TOTALANIMATIONS )
    {
        CG_Error( "Bad animation number: %i", newAnimation );
    }
    anim = ai+newAnimation;
    lf->animation = anim;
    lf->animationTime = lf->frameTime + anim->initialLerp;
    if ( cg_debugAnim.integer )
    {
        CG_Printf( NULL,"Anim: %i\n", newAnimation );
    }
}
/*
===============
CG_RunLerpFrameNPC
Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void CG_RunLerpFrameNPC( animation_t *ai, lerpFrame_t *lf, int newAnimation, float speedScale )
{
    int		f,numFrames;
    animation_t	*anim;
    // debugging tool to get no animations
    if ( cg_animSpeed.integer == 0 )
    {
        lf->oldFrame = lf->frame = lf->backlerp = 0;
        return;
    }
    // see if the animation sequence is switching
    if ( newAnimation != lf->animationNumber || !lf->animation )
    {
        CG_SetLerpFrameAnimationNPC( ai, lf, newAnimation );
    }
    // if we have passed the current frame, move it to
    // oldFrame and calculate a new frame
    if ( cg.time >= lf->frameTime )
    {
        lf->oldFrame = lf->frame;
        lf->oldFrameTime = lf->frameTime;
        // get the next frame based on the animation
        anim = lf->animation;
        if ( !anim->frameLerp )
        {
            return;		// shouldn't happen
        }
        if ( cg.time < lf->animationTime )
        {
            lf->frameTime = lf->animationTime;		// initial lerp
        }
        else
        {
            lf->frameTime = lf->oldFrameTime + anim->frameLerp;
        }
        f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
        f *= speedScale;		// adjust for haste, etc
        numFrames = anim->numFrames;
        if (anim->flipflop)
        {
            numFrames *= 2;
        }
        if ( f >= numFrames )
        {
            f -= numFrames;
            if ( anim->loopFrames )
            {
                f %= anim->loopFrames;
                f += anim->numFrames - anim->loopFrames;
            }
            else
            {
                f = numFrames - 1;
                // the animation is stuck at the end, so it
                // can immediately transition to another sequence
                lf->frameTime = cg.time;
            }
        }
        if ( anim->reversed )
        {
            lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
        }
        else if (anim->flipflop && f>=anim->numFrames)
        {
            lf->frame = anim->firstFrame + anim->numFrames - 1 - (f%anim->numFrames);
        }
        else
        {
            lf->frame = anim->firstFrame + f;
        }
        if ( cg.time > lf->frameTime )
        {
            lf->frameTime = cg.time;
            if ( cg_debugAnim.integer )
            {
                CG_Printf( NULL,"Clamp lf->frameTime\n");
            }
        }
    }
    if ( lf->frameTime > cg.time + 200 )
    {
        lf->frameTime = cg.time;
    }
    if ( lf->oldFrameTime > cg.time )
    {
        lf->oldFrameTime = cg.time;
    }
    // calculate current lerp value
    if ( lf->frameTime == lf->oldFrameTime )
    {
        lf->backlerp = 0;
    }
    else
    {
        lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
    }
}

/*
===============
CG_ClearLerpFrame
===============
*/
void CG_ClearLerpFrameNPC( animation_t *ai, lerpFrame_t *lf, int animationNumber )
{
    lf->frameTime = lf->oldFrameTime = cg.time;
    CG_SetLerpFrameAnimationNPC( ai, lf, animationNumber );
    lf->oldFrame = lf->frame = lf->animation->firstFrame;
}

/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
static void CG_RunLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale )
{
    int			f, numFrames;
    animation_t	*anim;

    // debugging tool to get no animations
    if ( cg_animSpeed.integer == 0 )
    {
        lf->oldFrame = lf->frame = lf->backlerp = 0;
        return;
    }

    // see if the animation sequence is switching
    if ( newAnimation != lf->animationNumber || !lf->animation )
    {
        CG_SetLerpFrameAnimation( ci, lf, newAnimation );
    }

    // if we have passed the current frame, move it to
    // oldFrame and calculate a new frame
    if ( cg.time >= lf->frameTime )
    {
        lf->oldFrame = lf->frame;
        lf->oldFrameTime = lf->frameTime;

        // get the next frame based on the animation
        anim = lf->animation;
        if ( !anim->frameLerp )
        {
            return;		// shouldn't happen
        }
        if ( cg.time < lf->animationTime )
        {
            lf->frameTime = lf->animationTime;		// initial lerp
        }
        else
        {
            lf->frameTime = lf->oldFrameTime + anim->frameLerp;
        }
        f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
        f *= speedScale;		// adjust for haste, etc

        numFrames = anim->numFrames;
        if (anim->flipflop)
        {
            numFrames *= 2;
        }
        if ( f >= numFrames )
        {
            f -= numFrames;
            if ( anim->loopFrames )
            {
                f %= anim->loopFrames;
                f += anim->numFrames - anim->loopFrames;
            }
            else
            {
                f = numFrames - 1;
                // the animation is stuck at the end, so it
                // can immediately transition to another sequence
                lf->frameTime = cg.time;
            }
        }
        if ( anim->reversed )
        {
            lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
        }
        else if (anim->flipflop && f>=anim->numFrames)
        {
            lf->frame = anim->firstFrame + anim->numFrames - 1 - (f%anim->numFrames);
        }
        else
        {
            lf->frame = anim->firstFrame + f;
        }
        if ( cg.time > lf->frameTime )
        {
            lf->frameTime = cg.time;
            if ( cg_debugAnim.integer )
            {
                CG_Printf( NULL,"Clamp lf->frameTime\n");
            }
        }
    }

    if ( lf->frameTime > cg.time + 200 )
    {
        lf->frameTime = cg.time;
    }

    if ( lf->oldFrameTime > cg.time )
    {
        lf->oldFrameTime = cg.time;
    }
    // calculate current lerp value
    if ( lf->frameTime == lf->oldFrameTime )
    {
        lf->backlerp = 0;
    }
    else
    {
        lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
    }
}


/*
===============
CG_ClearLerpFrame
===============
*/
static void CG_ClearLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int animationNumber )
{
    lf->frameTime = lf->oldFrameTime = cg.time;
    CG_SetLerpFrameAnimation( ci, lf, animationNumber );
    lf->oldFrame = lf->frame = lf->animation->firstFrame;
}


/*
===============
CG_PlayerAnimation
===============
*/
static void CG_PlayerAnimation( centity_t *cent, int *legsOld, int *legs, float *legsBackLerp,
                                int *torsoOld, int *torso, float *torsoBackLerp,
                                int *headOld, int *head, float *headBackLerp,
                                int *hairOld, int *hair, float *hairBackLerp )
{
    clientInfo_t	*ci;
    int				clientNum;
    float			speedScale;

    clientNum = cent->currentState.clientNum;

    if ( cg_noPlayerAnims.integer )
    {
        *hairOld = *hair = *headOld = *head = *legsOld = *legs = *torsoOld = *torso = 0;
        return;
    }

    speedScale = 1;

    ci = &cgs.clientinfo[ clientNum ];
// try to get some head animation going
    /*CG_RunLerpFrame( ci, &cent->pe.head, cent->currentState.generic1, speedScale );
    //CG_Printf( NULL,"current head animation is %i\n", (cent->currentState.generic1 & ~ANIM_TOGGLEBIT) );
    //CG_Printf( NULL,"old head frame was %i\n", cent->pe.head.oldFrame );
    //CG_Printf( NULL,"current head frame is %i\n", cent->pe.head.frame );
    *headOld = cent->pe.head.oldFrame;
    *head = cent->pe.head.frame;
    *headBackLerp = cent->pe.head.backlerp;*/
    CG_RunLerpFrame( ci, &cent->pe.hair, cent->currentState.generic1, speedScale );
    *hairOld = cent->pe.hair.oldFrame;
    *hair = cent->pe.hair.frame;
    *hairBackLerp = cent->pe.hair.backlerp;

    // do the shuffle turn frames locally
    CG_RunLerpFrame( ci, &cent->pe.legs, cent->currentState.legsAnim, speedScale );

    *legsOld = cent->pe.legs.oldFrame;
    *legs = cent->pe.legs.frame;
    *legsBackLerp = cent->pe.legs.backlerp;

    CG_RunLerpFrame( ci, &cent->pe.torso, cent->currentState.torsoAnim, speedScale );

    *torsoOld = cent->pe.torso.oldFrame;
    *torso = cent->pe.torso.frame;
    *torsoBackLerp = cent->pe.torso.backlerp;
}

/*
=============================================================================

PLAYER ANGLES

=============================================================================
*/

/*
==================
CG_SwingAngles
==================
*/
static void CG_SwingAngles( float destination, float swingTolerance, float clampTolerance,
                            float speed, float *angle, qboolean *swinging )
{
    float	swing;
    float	move;
    float	scale;

    if ( !*swinging )
    {
        // see if a swing should be started
        swing = AngleSubtract( *angle, destination );
        if ( swing > swingTolerance || swing < -swingTolerance )
        {
            *swinging = qtrue;
        }
    }

    if ( !*swinging )
    {
        return;
    }

    // modify the speed depending on the delta
    // so it doesn't seem so linear
    swing = AngleSubtract( destination, *angle );
    scale = fabs( swing );
    if ( scale < swingTolerance * 0.5 )
    {
        scale = 0.5;
    }
    else if ( scale < swingTolerance )
    {
        scale = 1.0;
    }
    else
    {
        scale = 2.0;
    }

    // swing towards the destination angle
    if ( swing >= 0 )
    {
        move = cg.frametime * scale * speed;
        if ( move >= swing )
        {
            move = swing;
            *swinging = qfalse;
        }
        *angle = AngleMod( *angle + move );
    }
    else if ( swing < 0 )
    {
        move = cg.frametime * scale * -speed;
        if ( move <= swing )
        {
            move = swing;
            *swinging = qfalse;
        }
        *angle = AngleMod( *angle + move );
    }

    // clamp to no more than tolerance
    swing = AngleSubtract( destination, *angle );
    if ( swing > clampTolerance )
    {
        *angle = AngleMod( destination - (clampTolerance - 1) );
    }
    else if ( swing < -clampTolerance )
    {
        *angle = AngleMod( destination + (clampTolerance - 1) );
    }
}

/*
=================
CG_AddPainTwitch
=================
*/
static void CG_AddPainTwitch( centity_t *cent, vec3_t torsoAngles )
{
    int		t;
    float	f;

    t = cg.time - cent->pe.painTime;
    if ( t >= PAIN_TWITCH_TIME )
    {
        return;
    }

    f = 1.0 - (float)t / PAIN_TWITCH_TIME;

    if ( cent->pe.painDirection )
    {
        torsoAngles[ROLL] += 20 * f;
    }
    else
    {
        torsoAngles[ROLL] -= 20 * f;
    }
}


/*
===============
CG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/
static void CG_PlayerAngles( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] )
{
    vec3_t		legsAngles, torsoAngles, headAngles;
    float		dest;
    static	int	movementOffsets[8] = { 0, 22, 45, -22, 0, 22, -45, -22 };
    vec3_t		velocity;
    float		speed;
    int			dir, clientNum;
    clientInfo_t	*ci;

    VectorCopy( cent->lerpAngles, headAngles );
    headAngles[YAW] = AngleMod( headAngles[YAW] );
    VectorClear( legsAngles );
    VectorClear( torsoAngles );

    // --------- yaw -------------

    // allow yaw to drift a bit
    if ( ( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) != LEGS_IDLE
            || ( cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT ) != TORSO_STAND
            || ( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) != LEGS_IDLEHURT
            || ( cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT ) != TORSO_IDLEHURT )
    {
        // if not standing still, always point all in the same direction
        cent->pe.torso.yawing = qtrue;	// always center
        cent->pe.torso.pitching = qtrue;	// always center
        cent->pe.legs.yawing = qtrue;	// always center
    }

    // adjust legs for movement dir
    if ( cent->currentState.eFlags & EF_DEAD )
    {
        // don't let dead bodies twitch
        dir = 0;
    }
    else
    {
        dir = cent->currentState.angles2[YAW];
        if ( dir < 0 || dir > 7 )
        {
            CG_Error( "Bad player movement angle" );
        }
    }
    legsAngles[YAW] = headAngles[YAW] + movementOffsets[ dir ];
    torsoAngles[YAW] = headAngles[YAW] + 0.25 * movementOffsets[ dir ];

    // torso
    CG_SwingAngles( torsoAngles[YAW], 25, 90, cg_swingSpeed.value, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing );
    CG_SwingAngles( legsAngles[YAW], 40, 90, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );

    torsoAngles[YAW] = cent->pe.torso.yawAngle;
    legsAngles[YAW] = cent->pe.legs.yawAngle;


    // --------- pitch -------------

    // only show a fraction of the pitch angle in the torso
    if ( headAngles[PITCH] > 180 )
    {
        dest = (-360 + headAngles[PITCH]) * 0.75f;
    }
    else
    {
        dest = headAngles[PITCH] * 0.75f;
    }
    CG_SwingAngles( dest, 15, 30, 0.1f, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
    torsoAngles[PITCH] = cent->pe.torso.pitchAngle;

    //
    clientNum = cent->currentState.clientNum;
    if ( clientNum >= 0 && clientNum < MAX_CLIENTS )
    {
        ci = &cgs.clientinfo[ clientNum ];
        if ( ci->fixedtorso )
        {
            torsoAngles[PITCH] = 0.0f;
        }
    }

    // --------- roll -------------


    // lean towards the direction of travel
    VectorCopy( cent->currentState.pos.trDelta, velocity );
    speed = VectorNormalize( velocity );
    clientNum = cent->currentState.clientNum;

    if ( clientNum >= 0 && clientNum < MAX_CLIENTS )
    {
        ci = &cgs.clientinfo[ clientNum ];
        if ( ci->fixedlegs )
        {
            legsAngles[YAW] = torsoAngles[YAW];
            legsAngles[PITCH] = 0.0f;
            legsAngles[ROLL] = 0.0f;
        }
    }

    // pain twitch
    CG_AddPainTwitch( cent, torsoAngles );

    // pull the angles back out of the hierarchial chain
    AnglesSubtract( headAngles, torsoAngles, headAngles );
    AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
    AnglesToAxis( legsAngles, legs );
    AnglesToAxis( torsoAngles, torso );
    AnglesToAxis( headAngles, head );
}


//==========================================================================

/*
===============
CG_JetSmoke
===============
*/
void CG_JetSmoke( centity_t *cent )
{
    localEntity_t	*smoke;
    vec3_t			origin;
    vec3_t			origin2;

    if ( cent->trailTime > cg.time )
    {
        return;
    }

    cent->trailTime += 100;
    if ( cent->trailTime < cg.time )
    {
        cent->trailTime = cg.time;
    }

    VectorCopy( cent->lerpOrigin, origin );
    VectorCopy( cent->lerpOrigin, origin2 );
    origin[2] -= 30;
    origin2[2] -= 60;

    smoke = CG_SmokePuff( origin, origin2,
                          50,
                          1, 1, 1, 1,
                          500,
                          cg.time,
                          0,
                          0,
                          cgs.media.jetSmokeShader );

    // use the optimized local entity add
    smoke->leType = LE_SCALE_FADE;
}

/*
===============
CG_TrailItem
===============
*/
static void CG_TrailItem( centity_t *cent, qhandle_t hModel )
{
    refEntity_t		ent;
    vec3_t			angles;
    vec3_t			axis[3];

    VectorCopy( cent->lerpAngles, angles );
    angles[PITCH] = 0;
    angles[ROLL] = 0;
    AnglesToAxis( angles, axis );

    memset( &ent, 0, sizeof( ent ) );
    VectorMA( cent->lerpOrigin, -16, axis[0], ent.origin );
    ent.origin[2] += 16;
    angles[YAW] += 90;
    AnglesToAxis( angles, ent.axis );

    ent.hModel = hModel;
    trap_R_AddRefEntityToScene( &ent );
}


/*
===============
CG_PlayerFlag
===============
*/
static void CG_PlayerFlag( centity_t *cent, qhandle_t hSkin, refEntity_t *torso )
{
    clientInfo_t	*ci;
    refEntity_t	pole;
    refEntity_t	flag;
    vec3_t		angles, dir;
    int			legsAnim, flagAnim, updateangles;
    float		angle, d;

    // show the flag pole model
    memset( &pole, 0, sizeof(pole) );
    pole.hModel = cgs.media.flagPoleModel;
    VectorCopy( torso->lightingOrigin, pole.lightingOrigin );
    pole.shadowPlane = torso->shadowPlane;
    pole.renderfx = torso->renderfx;
    CG_PositionEntityOnTag( &pole, torso, torso->hModel, "tag_flag" );
    trap_R_AddRefEntityToScene( &pole );

    // show the flag model
    memset( &flag, 0, sizeof(flag) );
    flag.hModel = cgs.media.flagFlapModel;
    flag.customSkin = hSkin;
    VectorCopy( torso->lightingOrigin, flag.lightingOrigin );
    flag.shadowPlane = torso->shadowPlane;
    flag.renderfx = torso->renderfx;

    VectorClear(angles);

    updateangles = qfalse;
    legsAnim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;

    if( legsAnim == LEGS_IDLE )
    {
        flagAnim = FLAG_STAND;
    }
    else if ( legsAnim == LEGS_WALK/* || legsAnim == LEGS_WALKCR*/ )
    {
        flagAnim = FLAG_STAND;
        updateangles = qtrue;
    }
    else
    {
        flagAnim = FLAG_RUN;
        updateangles = qtrue;
    }

    if ( updateangles )
    {

        VectorCopy( cent->currentState.pos.trDelta, dir );
        // add gravity
        dir[2] += 100;
        VectorNormalize( dir );
        d = DotProduct(pole.axis[2], dir);
        // if there is anough movement orthogonal to the flag pole
        if (fabs(d) < 0.9)
        {
            //
            d = DotProduct(pole.axis[0], dir);
            if (d > 1.0f)
            {
                d = 1.0f;
            }
            else if (d < -1.0f)
            {
                d = -1.0f;
            }
            angle = acos(d);

            d = DotProduct(pole.axis[1], dir);
            if (d < 0)
            {
                angles[YAW] = 360 - angle * 180 / M_PI;
            }
            else
            {
                angles[YAW] = angle * 180 / M_PI;
            }
            if (angles[YAW] < 0)
                angles[YAW] += 360;
            if (angles[YAW] > 360)
                angles[YAW] -= 360;

            //vectoangles( cent->currentState.pos.trDelta, tmpangles );
            //angles[YAW] = tmpangles[YAW] + 45 - cent->pe.torso.yawAngle;
            // change the yaw angle
            CG_SwingAngles( angles[YAW], 25, 90, 0.15f, &cent->pe.flag.yawAngle, &cent->pe.flag.yawing );
        }

        /*
        d = DotProduct(pole.axis[2], dir);
        angle = Q_acos(d);

        d = DotProduct(pole.axis[1], dir);
        if (d < 0) {
        	angle = 360 - angle * 180 / M_PI;
        }
        else {
        	angle = angle * 180 / M_PI;
        }
        if (angle > 340 && angle < 20) {
        	flagAnim = FLAG_RUNUP;
        }
        if (angle > 160 && angle < 200) {
        	flagAnim = FLAG_RUNDOWN;
        }
        */
    }

    // set the yaw angle
    angles[YAW] = cent->pe.flag.yawAngle;
    // lerp the flag animation frames
    ci = &cgs.clientinfo[ cent->currentState.clientNum ];
    CG_RunLerpFrame( ci, &cent->pe.flag, flagAnim, 1 );
    flag.oldframe = cent->pe.flag.oldFrame;
    flag.frame = cent->pe.flag.frame;
    flag.backlerp = cent->pe.flag.backlerp;

    AnglesToAxis( angles, flag.axis );
    CG_PositionRotatedEntityOnTag( &flag, &pole, pole.hModel, "tag_flag" );

    trap_R_AddRefEntityToScene( &flag );
}




/*
===============
CG_PlayerPowerups
===============
*/
static void CG_PlayerPowerups( centity_t *cent, refEntity_t *torso )
{
    int		powerups;
    clientInfo_t	*ci;

    powerups = cent->currentState.powerups;
    if ( !powerups )
    {
        return;
    }

    // add a sound for the jetpack
    if ( powerups & ( 1 << PW_JETPACK ) )
    {
        CG_JetSmoke( cent );
        trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.flightSound );
    }
    ci = &cgs.clientinfo[ cent->currentState.clientNum ];
    // redflag
    if ( powerups & ( 1 << PW_REDFLAG ) )
    {
        if (ci->newAnims)
        {
            CG_PlayerFlag( cent, cgs.media.redFlagFlapSkin, torso );
        }
        else
        {
            CG_TrailItem( cent, cgs.media.redFlagModel );
        }
        trap_R_AddLightToScene( cent->lerpOrigin, 200 + (genrand_int32()&31), 1.0, 0.2f, 0.2f );
    }

    // blueflag
    if ( powerups & ( 1 << PW_BLUEFLAG ) )
    {
        if (ci->newAnims)
        {
            CG_PlayerFlag( cent, cgs.media.blueFlagFlapSkin, torso );
        }
        else
        {
            CG_TrailItem( cent, cgs.media.blueFlagModel );
        }
        trap_R_AddLightToScene( cent->lerpOrigin, 200 + (genrand_int32()&31), 0.2f, 0.2f, 1.0 );
    }

    // neutralflag
    if ( powerups & ( 1 << PW_NEUTRALFLAG ) )
    {
        if (ci->newAnims)
        {
            CG_PlayerFlag( cent, cgs.media.neutralFlagFlapSkin, torso );
        }
        else
        {
            CG_TrailItem( cent, cgs.media.neutralFlagModel );
        }
        trap_R_AddLightToScene( cent->lerpOrigin, 200 + (genrand_int32()&31), 1.0, 1.0, 1.0 );
    }
}

/*
===============
CG_PlayerShadow

Returns the Z component of the surface being shadowed

  should it return a full plane instead of a Z?
===============
*/
#define	SHADOW_DISTANCE		128
static qboolean CG_PlayerShadow( centity_t *cent, float *shadowPlane )
{
    vec3_t		end, mins = {-15, -15, 0}, maxs = {15, 15, 2};
    trace_t		trace;
    float		alpha;

    *shadowPlane = 0;

    if ( cg_shadows.integer == 0 )
    {
        return qfalse;
    }

    // no shadows when cloaked
    if ( cent->currentState.powerups & ( 1 << PW_CLOAK ) )
    {
        return qfalse;
    }

    // send a trace down from the player to the ground
    VectorCopy( cent->lerpOrigin, end );
    end[2] -= SHADOW_DISTANCE;

    trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, mins, maxs, 0, MASK_PLAYERSOLID );

    // no shadow if too high
    if ( trace.fraction == 1.0 || trace.startsolid || trace.allsolid )
    {
        return qfalse;
    }

    *shadowPlane = trace.endpos[2] + 1;

    if ( cg_shadows.integer != 1 )  	// no mark for stencil or projection shadows
    {
        return qtrue;
    }

    // fade the shadow out with height
    alpha = 1.0 - trace.fraction;

    // bk0101022 - hack / FPE - bogus planes?
    //assert( DotProduct( trace.plane.normal, trace.plane.normal ) != 0.0f )

    // add the mark as a temporary, so it goes directly to the renderer
    // without taking a spot in the cg_marks array
    CG_ImpactMark( cgs.media.shadowMarkShader, trace.endpos, trace.plane.normal,
                   cent->pe.legs.yawAngle, alpha,alpha,alpha,1, qfalse, 24, qtrue );

    return qtrue;
}


/*
===============
CG_PlayerSplash

Draw a mark at the water surface
===============
*/
static void CG_PlayerSplash( centity_t *cent )
{
    vec3_t		start, end;
    trace_t		trace;
    int			contents;
    polyVert_t	verts[4];

    if ( !cg_shadows.integer )
    {
        return;
    }

    VectorCopy( cent->lerpOrigin, end );
    end[2] -= 24;

    // if the feet aren't in liquid, don't make a mark
    // this won't handle moving water brushes, but they wouldn't draw right anyway...
    contents = trap_CM_PointContents( end, 0 );
    if ( !( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) )
    {
        return;
    }

    VectorCopy( cent->lerpOrigin, start );
    start[2] += 32;

    // if the head isn't out of liquid, don't make a mark
    contents = trap_CM_PointContents( start, 0 );
    if ( contents & ( CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) )
    {
        return;
    }

    // trace down to find the surface
    trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) );

    if ( trace.fraction == 1.0 )
    {
        return;
    }

    // create a mark polygon
    VectorCopy( trace.endpos, verts[0].xyz );
    verts[0].xyz[0] -= 32;
    verts[0].xyz[1] -= 32;
    verts[0].st[0] = 0;
    verts[0].st[1] = 0;
    verts[0].modulate[0] = 255;
    verts[0].modulate[1] = 255;
    verts[0].modulate[2] = 255;
    verts[0].modulate[3] = 255;

    VectorCopy( trace.endpos, verts[1].xyz );
    verts[1].xyz[0] -= 32;
    verts[1].xyz[1] += 32;
    verts[1].st[0] = 0;
    verts[1].st[1] = 1;
    verts[1].modulate[0] = 255;
    verts[1].modulate[1] = 255;
    verts[1].modulate[2] = 255;
    verts[1].modulate[3] = 255;

    VectorCopy( trace.endpos, verts[2].xyz );
    verts[2].xyz[0] += 32;
    verts[2].xyz[1] += 32;
    verts[2].st[0] = 1;
    verts[2].st[1] = 1;
    verts[2].modulate[0] = 255;
    verts[2].modulate[1] = 255;
    verts[2].modulate[2] = 255;
    verts[2].modulate[3] = 255;

    VectorCopy( trace.endpos, verts[3].xyz );
    verts[3].xyz[0] += 32;
    verts[3].xyz[1] -= 32;
    verts[3].st[0] = 1;
    verts[3].st[1] = 0;
    verts[3].modulate[0] = 255;
    verts[3].modulate[1] = 255;
    verts[3].modulate[2] = 255;
    verts[3].modulate[3] = 255;

    trap_R_AddPolyToScene( cgs.media.wakeMarkShader, 4, verts );
}



/*
===============
CG_AddRefEntityWithPowerups

Adds a piece with modifications or duplications for powerups
Also called by CG_Missile for quad rockets, but nobody can tell...
===============
*/
void CG_AddRefEntityWithPowerups( refEntity_t *ent, entityState_t *state, int team )
{
    trap_R_AddRefEntityToScene( ent );
    if( cg_celShaded.integer )
    {
        ent->customShader = cgs.media.celShader;
        trap_R_AddRefEntityToScene( ent );
    }
}

/*
=================
CG_LightVerts
=================
*/
int CG_LightVerts( vec3_t normal, int numVerts, polyVert_t *verts )
{
    int				i, j;
    float			incoming;
    vec3_t			ambientLight;
    vec3_t			lightDir;
    vec3_t			directedLight;

    trap_R_LightForPoint( verts[0].xyz, ambientLight, directedLight, lightDir );

    for (i = 0; i < numVerts; i++)
    {
        incoming = DotProduct (normal, lightDir);
        if ( incoming <= 0 )
        {
            verts[i].modulate[0] = ambientLight[0];
            verts[i].modulate[1] = ambientLight[1];
            verts[i].modulate[2] = ambientLight[2];
            verts[i].modulate[3] = 255;
            continue;
        }
        j = ( ambientLight[0] + incoming * directedLight[0] );
        if ( j > 255 )
        {
            j = 255;
        }
        verts[i].modulate[0] = j;

        j = ( ambientLight[1] + incoming * directedLight[1] );
        if ( j > 255 )
        {
            j = 255;
        }
        verts[i].modulate[1] = j;

        j = ( ambientLight[2] + incoming * directedLight[2] );
        if ( j > 255 )
        {
            j = 255;
        }
        verts[i].modulate[2] = j;

        verts[i].modulate[3] = 255;
    }
    return qtrue;
}

/*
===============
CG_DashTrail2
===============
*/
localEntity_t* CG_DashTrail2( centity_t *cent, refEntity_t *ent )
{
    localEntity_t	*le;
    vec3_t			xoffset;
    vec3_t			v[3];
    clientInfo_t	*ci = &cgs.clientinfo[ cent->currentState.clientNum ];
    int	pclass = CG_ClassforPlayer( ci->modelName, ci->skinName );

    le = CG_AllocLocalEntity();

    le->leType = LE_SCALE_FADE;
    AxisCopy( ent->axis, le->refEntity.axis );

    le->startTime = cg.time;
    le->endTime = le->startTime + 100;
    le->lifeRate = 1.0f / ( le->endTime - le->startTime );

    le->refEntity.reType = RT_MODEL;
    le->refEntity.shaderTime = le->startTime / 1000.0f;
    le->refEntity.renderfx = ent->renderfx;

    le->refEntity.hModel = ent->hModel;

    switch ( ci->color )
    {
    case C_BLACK:
        le->refEntity.customShader = cgs.media.blackDashEffectShader;
        break;
    case C_RED:
        le->refEntity.customShader = cgs.media.redDashEffectShader;
        break;
    case C_GREEN:
        le->refEntity.customShader = cgs.media.greenDashEffectShader;
        break;
    case C_YELLOW:
        le->refEntity.customShader = cgs.media.yellowDashEffectShader;
        break;
    case C_BLUE:
        le->refEntity.customShader = cgs.media.blueDashEffectShader;
        break;
    case C_CYAN:
        le->refEntity.customShader = cgs.media.cyanDashEffectShader;
        break;
    case C_MAGENTA:
        le->refEntity.customShader = cgs.media.purpleDashEffectShader;
        break;
    case C_WHITE:
        le->refEntity.customShader = cgs.media.whiteDashEffectShader;
        break;
    default:
        if( pclass == CLASS_DWNINFINITY )
            le->refEntity.customShader = cgs.media.redDashEffectShader;
        else if ( pclass == CLASS_DRN00X )
            le->refEntity.customShader = cgs.media.blueDashEffectShader;
        else if ( pclass == CLASS_SWN001 )
            le->refEntity.customShader = cgs.media.purpleDashEffectShader;
        else if ( pclass == CLASS_DRN000 )
            le->refEntity.customShader = cgs.media.cyanDashEffectShader;
        else
            le->refEntity.customShader = cgs.media.greenDashEffectShader;
        break;
    }

    le->refEntity.frame = ent->frame;
    //offset the dash trail -15 units from the main body
    AnglesToAxis( cent->lerpAngles, v );
    xoffset[0] = -10 * v[0][0];
    xoffset[1] = -10 * v[0][1];
    xoffset[2] = 0;
    VectorAdd( ent->origin, xoffset, le->refEntity.origin );

    le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0f;
    le->refEntity.shaderRGBA[0] = le->refEntity.shaderRGBA[1] = le->refEntity.shaderRGBA[2] = 255;

    return le;
}
/*
===============
CG_DashTrail
===============
*/
void CG_DashTrail( centity_t *cent, refEntity_t *ent )
{
    trace_t			trace;
    vec3_t			point;
    vec3_t	mins = { -18, -18, 0 };
    vec3_t	maxs = { 18, 18, 0 };
    localEntity_t	*trail;
//	clientInfo_t	*ci = &cgs.clientinfo[ cent->currentState.clientNum ];

    VectorCopy( cent->lerpOrigin, point );
    point[2] -= 32;
    CG_Trace( &trace, cent->lerpOrigin, mins, maxs, point, cent->currentState.number, MASK_PLAYERSOLID);
    if( (( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) == LEGS_DASH ) ||
            (( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) == LEGS_DASH_B ) )   //set flag to true if you're dashing...
    {
        cent->pe.showDashTrail = qtrue;
        //set it to false if you're NOT dashing, and you have hit the ground
    }
    else if( !( cent->currentState.powerups & ( 1 << PW_DASH ) ) && trace.contents )
    {
        cent->pe.showDashTrail = qfalse;
        return;
    }

    if( cent->pe.showDashTrail == qfalse )
        return;

    trail = CG_DashTrail2( cent, ent );
}

/*
===============
CG_ChargeEffect2
the spheres
===============
*/
localEntity_t* CG_ChargeEffect2( centity_t *cent, refEntity_t *torso )
{
    vec4_t	lColor; // color of light for each particle
    vec3_t	randVec, tempVec;
    localEntity_t *le;
    refEntity_t *re;
    int k;

    // set defaults
    lColor[0] = 1.0f;
    lColor[1] = 1.0f;
    lColor[2] = 1.0f;
    lColor[3] = 1.0f; // alpha

    le = CG_AllocLocalEntity(); //allocate a local entity
    re = &le->refEntity;

    le->leFlags = LEF_SPRITE_DELAY; //don't show if its not your time :p
    le->leFlags |= LEF_PUFF_FADE_RGB; //fade RGB
    le->leFlags |= LEF_PUFF_SHRINK; //particle should shrink over time
    le->leType = LE_MOVE_SCALE_FADE;
    le->startTime = cg.time; // set the start time of the particle to the current time
    le->pos.trTime = cg.time + 400;
    le->fadeInTime = cg.time;
    le->endTime = cg.time + 300; //set the end time
    le->lifeRate = 1.0 / ( le->endTime - le->startTime );
    re = &le->refEntity;
    re->shaderTime = cg.time / 1000.0f;
    re->reType = RT_SPRITE;
    re->rotation = 0;
    re->radius = le->radius = abs(crandom()*4);
    if( cent->chargelevel < 2 )
    {
        re->customShader = cgs.media.chargeSphereShader;
    }
    else if( cent->chargelevel < 3 )
    {
        re->customShader = cgs.media.chargeSphere2Shader;
    }
    else
    {
        re->customShader = cgs.media.chargeSphere3Shader;
    }
    for( k = 0; k < 4; k++ )
    {
        le->color[k] = 1.0f;
        re->shaderRGBA[k] = 255;
    }
    le->pos.trType = TR_LINEAR; // moves in a line
    tempVec[0] = crandom(); //between 1 and -1
    tempVec[1] = crandom();
    tempVec[2] = crandom();
    VectorNormalize(tempVec);
    VectorScale(tempVec, -33*cent->chargelevel, randVec);
    VectorCopy( randVec, le->pos.trDelta );
    CG_PositionEntityOnTag( re, torso, torso->hModel, "tag_flash");
    //VectorCopy( cent->lerpOrigin, le->pos.trBase );
    //VectorCopy( le->pos.trBase, re->origin );
    VectorCopy( re->origin, le->pos.trBase );
    return le;
}

/*
===============
CG_ChargeEffect1
===============
*/
void CG_ChargeEffect1( centity_t *cent, refEntity_t *ent, qboolean torso )
{
    localEntity_t	*sphere[2];
    vec3_t color;

    switch( cent->chargelevel )
    {
    case 0:
        return;
    case 1:
        ent->customShader = cgs.media.charge1Shader;
        VectorSet( color, .427f, .811f, .965f );
        break;
    case 2:
        ent->customShader = cgs.media.charge2Shader;
        VectorSet( color, 1, .925f, .086f );
        break;
    case 3:
        ent->customShader = cgs.media.charge3Shader;
        VectorSet( color, 1, 0, 1 );
        break;
    case 4:
        ent->customShader = cgs.media.charge4Shader;
        VectorSet( color, 1, 0, 1 );
        break;
    default:
        return;
    }
    if( cent->chargelevel > 1 )
        trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.chargeLoopSound );
    if( cg.time%5 && torso)
    {
        sphere[0] = CG_ChargeEffect2( cent, ent ); //the spheres! maybe send it a shader for different charge effects
        sphere[1] = CG_ChargeEffect2( cent, ent ); //the spheres! maybe send it a shader for different charge effects
    }
    trap_R_AddRefEntityToScene( ent );
}



/*
===============
CG_ChargeGlow

Glow effect for charging
===============
*/
void CG_ChargeGlow( refEntity_t *torso, refEntity_t *glow, int chargelevel )
{
    memset( glow, 0, sizeof( glow ) );
    glow->reType = RT_SPRITE;
    if (!cg.renderingThirdPerson)
        glow->renderfx = RF_THIRD_PERSON;
    glow->customShader = cgs.media.chargeGlowShader;
    glow->shaderRGBA[0] = glow->shaderRGBA[1] = glow->shaderRGBA[2] = glow->shaderRGBA[3] = 0xff;
    glow->radius = chargelevel*6 + 1;
    //CG_Printf( NULL,"charge.radius SHOULD FREAKING BE %f\n", glow->radius );
    CG_PositionEntityOnTag( glow, torso, torso->hModel, "tag_flash");
    trap_R_AddRefEntityToScene( glow );
    trap_R_AddLightToScene(glow->origin, chargelevel*15 + (genrand_int32()&31), .427f, .811f, .965f );
}

/*
===============
CG_HitEffects

You got hurt!
===============
*/
void CG_HitEffect( centity_t *cent , refEntity_t *effect )
{

    memset( effect, 0, sizeof( effect ) );
    effect->reType = RT_SPRITE;
    //if (!cg.renderingThirdPerson)
    //effect->renderfx = RF_THIRD_PERSON;
    effect->customShader = cgs.media.gotHitShader;
    effect->shaderTime = (cent->hitTime - 500) / 1000.0f;
    effect->shaderRGBA[0] = effect->shaderRGBA[1] = effect->shaderRGBA[2] = effect->shaderRGBA[3] = 0xff;
    effect->radius = 40;
    VectorCopy( cent->lerpOrigin, effect->origin );
    trap_R_AddRefEntityToScene( effect );
}

void CG_HitEffect2 ( centity_t *cent, refEntity_t *flash )
{

    //if (!cg.renderingThirdPerson)
    //flash->renderfx = RF_THIRD_PERSON;
    flash->shaderTime = (cent->hitTime - 500) / 1000.0f;
    flash->customShader = cgs.media.pfx_hit;

    trap_R_AddRefEntityToScene( flash );
}

/*
===============
CG_GenerateSmoke

Handle smoke events
===============
*/

void CG_GenerateSmoke( centity_t *cent )
{
#define	NUMSTREAMS 8
    vec3_t	directions[NUMSTREAMS] =
    {
        {-1, 0, 0}, //left
        {1, 0, 0}, //right
        {0, 1, 0}, //forward
        {0, -1, 0}, //back
        {-1, 1, 0}, //frontleft
        {1, 1, 0}, //frontright
        {-1, -1, 0}, //rearleft
        {1, -1, 0}
    }; //rearright
    vec3_t	point;
    trace_t	trace;
    int contents, radius = 0, scale, k;
    localEntity_t	*smoke[NUMSTREAMS];
    qhandle_t Shader = 0;

    contents = CG_PointContents( cent->lerpOrigin, -1 ); //if we are in water, DONT use smoke on the ground

    //ground smoke if boosting
    if( !(cg.time % 2) )   //increase the number if you want more time between the smoke clouds
    {
        if( (cent->currentState.powerups & ( 1 << PW_DASH ) ) || (cent->currentState.powerups & ( 1 << PW_DASH ) ) )
        {
            VectorCopy(cent->lerpOrigin, point);
            point[2] -= 200;
            CG_Trace( &trace, cent->lerpOrigin, NULL, NULL, point, cent->currentState.number, MASK_ALL);
            //set scale for smoke streams
            scale = 100.0/trace.fraction;
            if( scale > 200)
                scale = 200;

            //boosting IN water
            if( ( trace.entityNum == ENTITYNUM_WORLD ) && ( contents & CONTENTS_WATER) )
            {
                radius = 20;
                scale = 200;
                Shader = cgs.media.waterBubbleShader;
            }

            //boosting over water
            else if( (trace.entityNum == ENTITYNUM_WORLD) && ( trace.contents & CONTENTS_WATER ) )
            {
                radius = 4.0/trace.fraction;
                if (radius > 15)
                    radius = 15;
                Shader = cgs.media.waterBubbleShader;
            }

            //boosting over land
            else if( ( trace.entityNum == ENTITYNUM_WORLD ) && (trace.contents & CONTENTS_SOLID) )
            {
                radius = 10.0/trace.fraction;
                if (radius > 15)
                    radius = 15;
                Shader = cgs.media.xbSmokeShader;
            }

            if( ( trace.entityNum == ENTITYNUM_WORLD ) && (trace.contents & (CONTENTS_SOLID|CONTENTS_WATER) ) )
            {
                for( k = 0; k < NUMSTREAMS; k++)
                {
                    if( k == floor( NUMSTREAMS/2.0f) )
                        scale *= .7;
                    VectorScale( directions[k], scale, directions[k]);
                }

                for( k = 0; k < NUMSTREAMS; k++)
                {
                    smoke[k] = CG_SmokePuff( trace.endpos, directions[k], //starting point, destination
                                             radius, //radius
                                             1, 1, 1, 0.33f, //color
                                             300, //milliseconds in existence
                                             cg.time, 0, //start time, fadein time
                                             0,
                                             Shader );
                }
            }
        }
    }

    // added for smoking players under 30 health
    //CG_Printf( NULL,"Flag is %i\n", cent->currentState.eFlags & EF_DAMAGED );
    if ( cent->currentState.eFlags & EF_DAMAGED )
    {
        vec3_t	end, origin;
        localEntity_t	*smoke, *spark;
        VectorCopy( cent->lerpOrigin, origin );
        origin[2] += 5;
        if( !(cg.time % 7) )
        {
            end[0] = crandom(); //between 1 and -1
            end[1] = crandom();
            end[2] = random();
            VectorNormalize( end );
            VectorScale( end, 50, end );
            smoke = CG_SmokePuff( cent->lerpOrigin, end, //starting point, destination
                                  20, //radius
                                  1, 1, 1, 0.33f, //color
                                  500, //milliseconds in existence
                                  cg.time, 0, //start time, fadein time
                                  0,
                                  cgs.media.xbSmokeShader );
            //smoke->leType = LE_FADE_RGB;
        }
        if( !(cg.time % 5) )
        {
            end[0] = crandom(); //between 1 and -1
            end[1] = crandom();
            end[2] = crandom();
            VectorNormalize( end );
            VectorScale( end, 150, end );
            spark = CG_GravityTrail( origin, end, //starting point, destination
                                     2, //radius
                                     1, 1, 1, 0.33f, //color
                                     400, //milliseconds in existence
                                     cg.time, 0, //start time, fadein time
                                     0,
                                     cgs.media.xbSparkShader );
        }
        if( !(cg.time % 6) )
        {
            end[0] = crandom(); //between 1 and -1
            end[1] = crandom();
            end[2] = crandom();
            VectorNormalize( end );
            VectorScale( end, 150, end );
            spark = CG_GravityTrail( origin, end, //starting point, destination
                                     5, //radius
                                     1, 1, 1, 0.33f, //color
                                     400, //milliseconds in existence
                                     cg.time, 0, //start time, fadein time
                                     0,
                                     cgs.media.xbSparkShader );
        }
    }
    if( cent->currentState.eFlags & EF_WALLHANG )
    {
        vec3_t	end, origin;
        localEntity_t	*smoke;
        VectorCopy( cent->lerpOrigin, origin );
        origin[2] -= 24;
        if( !(cg.time % 2) )
        {
            end[0] = 0;
            end[1] = 0;
            end[2] = 200;
            smoke = CG_SmokePuff( origin, end, //starting point, destination
                                  20, //radius
                                  1, 1, 1, 0.5f, //color
                                  300, //milliseconds in existence
                                  cg.time, 0, //start time, fadein time
                                  0,
                                  cgs.media.xbWallDustShader );
            //smoke->leType = LE_SCALE_FADE;
        }
    }
}


/*
===============
adds the upgrades on to the player
===============
*/
void CG_AddHeadWithUpgrades( refEntity_t *ent, entityState_t *state )
{
//	clientInfo_t	*ci = &cgs.clientinfo[state->clientNum];
//	int	pclass = CG_ClassforPlayer( ci->modelName, ci->skinName );

    trap_R_AddRefEntityToScene( ent );
    if( cg_celShaded.integer )
    {
        ent->customShader = cgs.media.celShader;
        trap_R_AddRefEntityToScene( ent );
    }
    /* Peter FIXME: Were going to replace these with model attachments
    if ( state->powerups & ( 1 << PW_FOCUS ) )
    {
    	switch( pclass ){
    		case CLASS_DRN00X:
    			ent->customShader = cgs.media.XhelmetUpgradeShader;
    			break;
    		case CLASS_SWN001:
    			ent->customShader = cgs.media.FhelmetUpgradeShader;
    			break;
    	}
    	trap_R_AddRefEntityToScene( ent );
    }
    */
    if ( state->eFlags & EF_FROZEN )
    {
        ent->customShader = cgs.media.pfx_frozen;
        trap_R_AddRefEntityToScene( ent );
        //cg.screenFlash = SF_FROZEN;
    }
    if ( state->eFlags & EF_SHOCKED )
    {
        ent->customShader = cgs.media.pfx_shocked;
        trap_R_AddRefEntityToScene( ent );
        //cg.screenFlash = SF_SHOCKED;
    }
}
void CG_AddHairWithUpgrades( refEntity_t *ent )
{

    trap_R_AddRefEntityToScene( ent );
    if( cg_celShaded.integer )
    {
        ent->customShader = cgs.media.celShader;
        trap_R_AddRefEntityToScene( ent );
    }
}
void CG_AddShieldWithUpgrades( refEntity_t *ent )
{

    trap_R_AddRefEntityToScene( ent );
    if( cg_celShaded.integer )
    {
        ent->customShader = cgs.media.celShader;
        trap_R_AddRefEntityToScene( ent );
    }
}
void CG_AddTorsoWithUpgrades( refEntity_t *ent, entityState_t *state )
{
//	clientInfo_t	*ci = &cgs.clientinfo[state->clientNum];
//	int	pclass = CG_ClassforPlayer( ci->modelName, ci->skinName );

    trap_R_AddRefEntityToScene( ent );
    if( cg_celShaded.integer )
    {
        ent->customShader = cgs.media.celShader;
        trap_R_AddRefEntityToScene( ent );
    }
    /* Peter FIXME: Were going to replace these with model attachments
    if ( state->powerups & ( 1 << PW_POWERCONVERTER ) ) {
    	switch( pclass ){
    		case CLASS_DRN00X:
    			ent->customShader = cgs.media.XarmorUpgradeShader;
    			break;
    		case CLASS_SWN001:
    			ent->customShader = cgs.media.FarmorUpgradeShader;
    			break;
    	}
    	trap_R_AddRefEntityToScene( ent );
    }
    if ( state->powerups & ( 1 << PW_BUSTER ) ) {
    	switch( pclass ){
    		case CLASS_DRN00X:
    			ent->customShader = cgs.media.XbusterUpgradeShader;
    			break;
    		case CLASS_SWN001:
    			ent->customShader = cgs.media.FbusterUpgradeShader;
    			break;
    	}
    	trap_R_AddRefEntityToScene( ent );
    }
    */
    if ( state->eFlags & EF_FROZEN )
    {
        ent->customShader = cgs.media.pfx_frozen;
        trap_R_AddRefEntityToScene( ent );
    }
    if ( state->eFlags & EF_SHOCKED )
    {
        ent->customShader = cgs.media.pfx_shocked;
        trap_R_AddRefEntityToScene( ent );
    }
}
void CG_AddLegsWithUpgrades( refEntity_t *ent, entityState_t *state )
{
//	clientInfo_t	*ci = &cgs.clientinfo[state->clientNum];
//	int	pclass = CG_ClassforPlayer( ci->modelName, ci->skinName );

    trap_R_AddRefEntityToScene( ent );
    if( cg_celShaded.integer )
    {
        ent->customShader = cgs.media.celShader;
        trap_R_AddRefEntityToScene( ent );
    }
    /* Peter FIXME: Were going to replace these with model attachments
    if ( state->powerups & ( 1 << PW_AIRJUMP ) )
    {
    	switch( pclass ){
    		case CLASS_DRN00X:
    			ent->customShader = cgs.media.XlegsUpgradeShader;
    			break;
    		case CLASS_SWN001:
    			ent->customShader = cgs.media.FlegsUpgradeShader;
    			break;
    	}
    	trap_R_AddRefEntityToScene( ent );
    }
    */
    if ( state->eFlags & EF_FROZEN )
    {
        ent->customShader = cgs.media.pfx_frozen;
        trap_R_AddRefEntityToScene( ent );
    }
    if ( state->eFlags & EF_SHOCKED )
    {
        ent->customShader = cgs.media.pfx_shocked;
        trap_R_AddRefEntityToScene( ent );
    }
}

/*
===============
CG_Player
===============
*/
void CG_Player( centity_t *cent )
{
    clientInfo_t	*ci;
    refEntity_t		legs;
    refEntity_t		torso;
    refEntity_t		head;
    refEntity_t		hair;	// for Zero
    refEntity_t		shield;
    int				clientNum;
    int				renderfx;
    qboolean		shadow;
    float			shadowPlane;

    refEntity_t		camera; // NIGHTZ - CAMERA CODE

    refEntity_t		booster;	//the thruster flame
    refEntity_t		charge[5];	//for charge effects
    vec3_t			angles;

    usercmd_t       cmd;
    int				cmdNum;

    cmdNum = trap_GetCurrentCmdNumber();
    trap_GetUserCmd( cmdNum, &cmd );


    // the client number is stored in clientNum.  It can't be derived
    // from the entity number, because a single client may have
    // multiple corpses on the level using the same clientinfo
    clientNum = cent->currentState.clientNum;
    if ( clientNum < 0 || clientNum >= MAX_CLIENTS )
    {
        CG_Error( "Bad clientNum on player entity");
    }
    ci = &cgs.clientinfo[ clientNum ];

    // it is possible to see corpses from disconnected players that may
    // not have valid clientinfo
    if ( !ci->infoValid )
    {
        return;
    }

    // get the player model information
    renderfx = 0;
    if ( cent->currentState.number == cg.snap->ps.clientNum)
    {
        if (!cg.renderingThirdPerson)
        {
            renderfx = RF_THIRD_PERSON;			// only draw in mirrors
        }
        else
        {
            if (cg_cameraMode.integer)
            {
                return;
            }
        }
    }

    memset( &legs, 0, sizeof(legs) );
    memset( &torso, 0, sizeof(torso) );
    memset( &head, 0, sizeof(head) );
    memset( &hair, 0, sizeof(hair) );
    memset( &shield, 0, sizeof(shield) );

    // get the rotation information
    CG_PlayerAngles( cent, legs.axis, torso.axis, head.axis );

    // get the animation state (after rotation, to allow feet shuffle)
    CG_PlayerAnimation( cent, &legs.oldframe, &legs.frame, &legs.backlerp,
                        &torso.oldframe, &torso.frame, &torso.backlerp,
                        &head.oldframe, &head.frame, &head.backlerp,
                        &hair.oldframe, &hair.frame, &hair.backlerp);

    // add the talk baloon or disconnect icon
    //CG_PlayerSprites( cent );

    // add the shadow
    shadow = CG_PlayerShadow( cent, &shadowPlane );

    // add a water splash if partially in and out of water
    CG_PlayerSplash( cent );

    // add Ground FX for testing
    CG_GroundFX( cent );

    if ( cg_shadows.integer == 3 && shadow )
    {
        renderfx |= RF_SHADOW_PLANE;
    }
    renderfx |= RF_LIGHTING_ORIGIN;			// use the same origin for all

    //
    // add the legs
    //
    legs.hModel = ci->legsModel;
    legs.customSkin = ci->legsSkin;

    VectorCopy( cent->lerpOrigin, legs.origin );

    VectorCopy( cent->lerpOrigin, legs.lightingOrigin );
    legs.shadowPlane = shadowPlane;
    legs.renderfx = renderfx;
    VectorCopy (legs.origin, legs.oldorigin);	// don't positionally lerp at all

    // check for charge level
    CG_AddLegsWithUpgrades( &legs, &cent->currentState );
    CG_ChargeEffect1( cent, &legs, qfalse );

    // if the model failed, allow the default nullmodel to be displayed
    if (!legs.hModel)
    {
        return;
    }

    //
    // add the torso
    //
    torso.hModel = ci->torsoModel;
    if (!torso.hModel)
    {
        return;
    }

    torso.customSkin = ci->torsoSkin;

    VectorCopy( cent->lerpOrigin, torso.lightingOrigin );

    CG_PositionRotatedEntityOnTag( &torso, &legs, ci->legsModel, "tag_torso");

    torso.shadowPlane = shadowPlane;
    torso.renderfx = renderfx;

    // check for charge level
    if( cent->chargelevel )
    {
        CG_ChargeGlow( &torso, &charge[4], cent->chargelevel );
    }

    CG_AddTorsoWithUpgrades( &torso, &cent->currentState );
    CG_ChargeEffect1( cent, &torso, qtrue );


    //
    // add the head
    //
    head.hModel = ci->headModel;
    if (!head.hModel)
    {
        return;
    }
    head.customSkin = ci->headSkin;

    VectorCopy( cent->lerpOrigin, head.lightingOrigin );

    CG_PositionRotatedEntityOnTag( &head, &torso, ci->torsoModel, "tag_head");

    head.shadowPlane = shadowPlane;
    head.renderfx = renderfx;

    // check for charge level
    CG_AddHeadWithUpgrades( &head, &cent->currentState );
    CG_ChargeEffect1( cent, &head, qfalse );

    //add hair for Zero
    if( !Q_stricmp (ci->modelName, "zero_s") )
    {
        hair.hModel = ci->hairModel;
        hair.customSkin = ci->hairSkin;
        //hair.customShader = cgs.media.blueDashEffectShader;
        VectorCopy( cent->lerpOrigin, hair.lightingOrigin );
        //CG_Printf( NULL,"velocity is <%f,%f,%f>\n", cent->currentState.pos.trDelta[0], cent->currentState.pos.trDelta[1], cent->currentState.pos.trDelta[2] );
        //VectorNegate( cent->currentState.pos.trDelta, angles );
        //vectoangles( angles, angles );
        vectoangles( cent->currentState.pos.trDelta, angles );
        //CG_Printf( NULL,"angles: PITCH: %f\n YAW: %f\n ROLL: %f\n", angles[PITCH], angles[YAW], angles[ROLL] );
        //bound the angles
        /*if( angles[YAW] > 90 ) angles[YAW] = 90;
        else if( angles[YAW] < -90 ) angles[YAW] = -90;
        if( angles[PITCH] > 85 ) angles[PITCH] = 85;
        else if( angles[PITCH] < -45 ) angles[PITCH] = -45;
        angles[ROLL] = 0;*/
        //CG_Printf( NULL,"angles: PITCH: %f\n YAW: %f\n ROLL: %f\n", angles[PITCH], angles[YAW], angles[ROLL] );
        AnglesToAxis( angles, hair.axis );

        CG_PositionRotatedEntityOnTag( &hair, &head, head.hModel, "tag_hair" );

        hair.shadowPlane = shadowPlane;
        hair.renderfx = renderfx;
        CG_AddHairWithUpgrades( &hair );
        CG_ChargeEffect1( cent, &hair, qfalse );
        CG_DashTrail( cent, &hair );
    }

    //add shield for Protoman
    if( !Q_stricmp (ci->modelName, "proto") )
    {
        shield.hModel = cgs.media.protoshieldModel;
        VectorCopy( cent->lerpOrigin, shield.lightingOrigin );
        vectoangles( cent->currentState.pos.trDelta, angles );
        AnglesToAxis( angles, shield.axis );

        if ( IS_SET(cmd.buttons, BUTTON_SHIELD) )
            CG_PositionEntityOnTag( &shield, &torso, torso.hModel, "tag_sheildon" );
        else
            CG_PositionEntityOnTag( &shield, &torso, torso.hModel, "tag_sheildoff" );

        shield.shadowPlane = shadowPlane;
        shield.renderfx = renderfx;
        CG_AddShieldWithUpgrades( &shield );
        CG_ChargeEffect1( cent, &shield, qfalse );
        CG_DashTrail( cent, &shield );
    }

    // NIGHTZ - CAMERA CODE
    CG_PositionEntityOnTag (&camera, &head, ci->headModel, "tag_camera");
    VectorCopy (camera.origin, cg.cameraPos);
    // END NIGHTZ

    // add the dash effect
    if( ((( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) == LEGS_DASH) ||
            (( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) == LEGS_DASH_B)))   //set flag to true if you're dashing...
    {
        //show the booster effect on the feet
        memset( &booster, 0, sizeof( booster ) );
        VectorCopy( legs.lightingOrigin, booster.lightingOrigin );
        booster.shadowPlane = legs.shadowPlane;
        booster.renderfx = legs.renderfx;
        booster.hModel = cgs.media.boostModel;
        booster.customShader = cgs.media.boosterShader;
        CG_PositionEntityOnTag( &booster, &legs, legs.hModel, "tag_r_thrust");
        trap_R_AddRefEntityToScene( &booster );
        CG_PositionEntityOnTag( &booster, &legs, legs.hModel, "tag_l_thrust");
        trap_R_AddRefEntityToScene( &booster );
        //set it to false if you're NOT dashing, and you have hit the ground
    }
    CG_DashTrail( cent, &legs );
    CG_DashTrail( cent, &torso );
    CG_DashTrail( cent, &head );
    //added for smoke events
    CG_GenerateSmoke( cent );

    //
    // add the gun / barrel / flash
    //
    CG_AddPlayerWeapon( &torso, NULL, cent, ci->team );

    // add powerups floating behind the player
    CG_PlayerPowerups( cent, &torso );

    // add the hit effect flash
    if( cent->hitTime > cg.time )
    {
        // add hit flash for death animation
        if( cent->currentState.event == EV_DEATH1 || cent->currentState.event == EV_GIB_PLAYER )
        {
            CG_HitEffect( cent, &charge[3] );
            CG_HitEffect2( cent, &head );
            CG_HitEffect2( cent, &torso );
            CG_HitEffect2( cent, &legs );
            CG_HitEffect2( cent, &hair );
        }
        else
        {
            CG_HitEffect( cent, &charge[3] );
            CG_HitEffect2( cent, &head );
            CG_HitEffect2( cent, &torso );
            CG_HitEffect2( cent, &legs );
            CG_HitEffect2( cent, &hair );
        }
    }
    // FIXME: Kills I need you to add in a parm for when the sheild is hit
    //if ( cmd.buttons & BUTTON_SHIELD )
    //CG_HitEffect2( cent, &shield);
}


//=====================================================================

/*
===============
CG_ResetPlayerEntity

A player just came into view or teleported, so reset all animation info
===============
*/
void CG_ResetPlayerEntity( centity_t *cent )
{
    cent->errorTime = -99999;		// guarantee no error decay added
    cent->extrapolated = qfalse;

    CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.legs, cent->currentState.legsAnim );
    CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.torso, cent->currentState.torsoAnim );
    // for the head animations
    //CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.head, cent->currentState.generic1 );
    CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.hair, cent->currentState.generic1 );

    BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin );
    BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles );

    VectorCopy( cent->lerpOrigin, cent->rawOrigin );
    VectorCopy( cent->lerpAngles, cent->rawAngles );

    memset( &cent->pe.legs, 0, sizeof( cent->pe.legs ) );
    cent->pe.legs.yawAngle = cent->rawAngles[YAW];
    cent->pe.legs.yawing = qfalse;
    cent->pe.legs.pitchAngle = 0;
    cent->pe.legs.pitching = qfalse;

    memset( &cent->pe.torso, 0, sizeof( cent->pe.legs ) );
    cent->pe.torso.yawAngle = cent->rawAngles[YAW];
    cent->pe.torso.yawing = qfalse;
    cent->pe.torso.pitchAngle = cent->rawAngles[PITCH];
    cent->pe.torso.pitching = qfalse;

    // for the head animations
    /*memset( &cent->pe.head, 0, sizeof( cent->pe.legs ) );
    cent->pe.head.yawAngle = cent->rawAngles[YAW];
    cent->pe.head.yawing = qfalse;
    cent->pe.head.pitchAngle = cent->rawAngles[PITCH];
    cent->pe.head.pitching = qfalse;*/
    memset( &cent->pe.hair, 0, sizeof( cent->pe.legs ) );
    cent->pe.hair.yawAngle = cent->rawAngles[YAW];
    cent->pe.hair.yawing = qfalse;
    cent->pe.hair.pitchAngle = cent->rawAngles[PITCH];
    cent->pe.hair.pitching = qfalse;

    if ( cg_debugPosition.integer )
    {
        CG_Printf( NULL,"%i ResetPlayerEntity yaw=%i\n", cent->currentState.number, cent->pe.torso.yawAngle );
    }
}

/*
===============
CG_ClassforPlayer
Get the player's class number based on the modelName
===============
*/
int	CG_ClassforPlayer( char *modelName, char *skinName )
{
    if( !Q_stricmp (modelName, "mmx"))
        return CLASS_DRN00X;
    else if( !Q_stricmp (modelName, "proto"))
        return CLASS_DRN000;
    else if( !Q_stricmp (modelName, "megaman"))
        return CLASS_DRN001;
    else if( !Q_stricmp (modelName, "forte"))
        return CLASS_SWN001;
    else if( !Q_stricmp (modelName, "zero"))
        return CLASS_DWNINFINITY;
    else if( !Q_stricmp (modelName, "zero_s"))
        return CLASS_DWNINFINITY;
    else if (!Q_stricmp (modelName, "bass"))
        return CLASS_SWN001;
    else if (!Q_stricmp (modelName, "duo"))
        return CLASS_DWNINFINITY;
    else if (!Q_stricmp (modelName, "dynamo"))
        return CLASS_DWNINFINITY;
    else if (!Q_stricmp (modelName, "sigma"))
        return CLASS_DWNINFINITY;
    else if (!Q_stricmp (modelName, "vile"))
        return CLASS_DRN00X;
    else
        return CLASS_DRN001;
    //CG_Printf( NULL,"Classnum is equal to %i\n", classnum);
}
/*
===============
CG_GroundFX

Draw a normal at players feet - working on giving it parameters to control when it draws
===============
*/
void CG_GroundFX( centity_t *cent )
{
    int			event;
    float		scale;
    vec3_t		start, end;
    trace_t		trace;
    polyVert_t	verts[4];

    // 24 + 32 = height of bounding box in qworld
    VectorCopy( cent->lerpOrigin, end );
    end[2] -= 40;

    VectorCopy( cent->lerpOrigin, start );
    start[2] += 32;

    event = cent->currentState.event & ~EV_EVENT_BITS;

    // trace down to find the surface
    trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, ( CONTENTS_SOLID ) );

    if ( trace.fraction == 1.0 )
    {
        return;
    }

    scale = 8 * cg.time / 1.0f;

    // create a mark polygon
    VectorCopy( trace.endpos, verts[0].xyz );
    verts[0].xyz[0] -= 42;
    verts[0].xyz[1] -= 42;
    verts[0].st[0] = 0;
    verts[0].st[1] = 0;
    verts[0].modulate[0] = 255;
    verts[0].modulate[1] = 255;
    verts[0].modulate[2] = 255;
    verts[0].modulate[3] = 255;

    VectorCopy( trace.endpos, verts[1].xyz );
    verts[1].xyz[0] -= 42;
    verts[1].xyz[1] += 42;
    verts[1].st[0] = 0;
    verts[1].st[1] = 1;
    verts[1].modulate[0] = 255;
    verts[1].modulate[1] = 255;
    verts[1].modulate[2] = 255;
    verts[1].modulate[3] = 255;

    VectorCopy( trace.endpos, verts[2].xyz );
    verts[2].xyz[0] += 42;
    verts[2].xyz[1] += 42;
    verts[2].st[0] = 1;
    verts[2].st[1] = 1;
    verts[2].modulate[0] = 255;
    verts[2].modulate[1] = 255;
    verts[2].modulate[2] = 255;
    verts[2].modulate[3] = 255;

    VectorCopy( trace.endpos, verts[3].xyz );
    verts[3].xyz[0] += 42;
    verts[3].xyz[1] -= 42;
    verts[3].st[0] = 1;
    verts[3].st[1] = 0;
    verts[3].modulate[0] = 255;
    verts[3].modulate[1] = 255;
    verts[3].modulate[2] = 255;
    verts[3].modulate[3] = 255;

    // upgrade ground fx
    switch ( cent->currentState.powerups )
    {
    case PW_REDFLAG:
        trap_R_AddPolyToScene( cgs.media.GroundFXplayerTeamRed, 4, verts );
        break;
    case PW_BLUEFLAG:
        trap_R_AddPolyToScene( cgs.media.GroundFXplayerTeamBlue, 4, verts );
        break;
    default:
        break;
    }
    // event generated ground fx
    switch ( event )
    {
    case EV_CHARGE4:
        trap_R_AddPolyToScene( cgs.media.GroundFXplayerCharge, 4, verts );
        break;
    case EV_USE_ITEM0:
        trap_R_AddPolyToScene( cgs.media.GroundFXplayerItem, 4, verts );
        break;
    default:
        break;
    }
    // event flag generated ground fx
    switch ( cent->currentState.eFlags )
    {
    case EF_TALK:
        trap_R_AddPolyToScene( cgs.media.GroundFXplayerTalk, 4, verts );
        break;
    case EF_LIGHTNSTRUCK:
        trap_R_AddPolyToScene( cgs.media.GroundFXplayerLightningStruck, 4, verts );
        break;
    default:
        break;
    }

    // bobs player up and down
    //scale = 0.005 + cent->currentState.number * 0.00001;
    //cent->lerpOrigin[2] += 4 + cos( ( cg.time + 1000 ) *  scale ) * 4;
}


