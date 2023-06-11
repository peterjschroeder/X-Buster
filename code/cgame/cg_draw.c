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

// used for scoreboard
extern displayContextDef_t cgDC;
extern int HudGroupFlag;
menuDef_t *menuScoreboard = NULL;

int drawTeamOverlayModificationCount = -1;
int sortedTeamPlayers[TEAM_MAXOVERLAY];
int	numSortedTeamPlayers;

char systemChat[256];
char teamChat1[256];
char teamChat2[256];

int CG_Text_Width(const char *text, float scale, int limit)
{
    int count,len;
    float out;
    glyphInfo_t *glyph;
    float useScale;
// FIXME: see ui_main.c, same problem
//	const unsigned char *s = text;
    const char *s = text;
    fontInfo_t *font = &cgDC.Assets.textFont;
    if (scale <= cg_smallFont.value)
    {
        font = &cgDC.Assets.smallFont;
    }
    else if (scale > cg_bigFont.value)
    {
        font = &cgDC.Assets.bigFont;
    }
    useScale = scale * font->glyphScale;
    out = 0;
    if (text)
    {
        len = strlen(text);
        if (limit > 0 && len > limit)
        {
            len = limit;
        }
        count = 0;
        while (s && *s && count < len)
        {
            if ( Q_IsColorString(s) )
            {
                s += 2;
                continue;
            }
            else
            {
                glyph = &font->glyphs[(int)*s]; // TTimo: FIXME: getting nasty warnings without the cast, hopefully this doesn't break the VM build
                out += glyph->xSkip;
                s++;
                count++;
            }
        }
    }
    return out * useScale;
}

int CG_Text_Height(const char *text, float scale, int limit)
{
    int len, count;
    float max;
    glyphInfo_t *glyph;
    float useScale;
// TTimo: FIXME
//	const unsigned char *s = text;
    const char *s = text;
    fontInfo_t *font = &cgDC.Assets.textFont;
    if (scale <= cg_smallFont.value)
    {
        font = &cgDC.Assets.smallFont;
    }
    else if (scale > cg_bigFont.value)
    {
        font = &cgDC.Assets.bigFont;
    }
    useScale = scale * font->glyphScale;
    max = 0;
    if (text)
    {
        len = strlen(text);
        if (limit > 0 && len > limit)
        {
            len = limit;
        }
        count = 0;
        while (s && *s && count < len)
        {
            if ( Q_IsColorString(s) )
            {
                s += 2;
                continue;
            }
            else
            {
                glyph = &font->glyphs[(int)*s]; // TTimo: FIXME: getting nasty warnings without the cast, hopefully this doesn't break the VM build
                if (max < glyph->height)
                {
                    max = glyph->height;
                }
                s++;
                count++;
            }
        }
    }
    return max * useScale;
}

void CG_Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader)
{
    float w, h;
    w = width * scale;
    h = height * scale;
    CG_AdjustFrom640( &x, &y, &w, &h );
    trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style)
{
    int len, count;
    vec4_t newColor;
    glyphInfo_t *glyph;
    float useScale;
    fontInfo_t *font = &cgDC.Assets.textFont;
    if (scale <= cg_smallFont.value)
    {
        font = &cgDC.Assets.smallFont;
    }
    else if (scale > cg_bigFont.value)
    {
        font = &cgDC.Assets.bigFont;
    }
    useScale = scale * font->glyphScale;
    if (text)
    {
// TTimo: FIXME
//		const unsigned char *s = text;
        const char *s = text;
        trap_R_SetColor( color );
        memcpy(&newColor[0], &color[0], sizeof(vec4_t));
        len = strlen(text);
        if (limit > 0 && len > limit)
        {
            len = limit;
        }
        count = 0;
        while (s && *s && count < len)
        {
            glyph = &font->glyphs[(int)*s]; // TTimo: FIXME: getting nasty warnings without the cast, hopefully this doesn't break the VM build
            //int yadj = Assets.textFont.glyphs[text[i]].bottom + Assets.textFont.glyphs[text[i]].top;
            //float yadj = scale * (Assets.textFont.glyphs[text[i]].imageHeight - Assets.textFont.glyphs[text[i]].height);
            if ( Q_IsColorString( s ) )
            {
                memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
                newColor[3] = color[3];
                trap_R_SetColor( newColor );
                s += 2;
                continue;
            }
            else
            {
                float yadj = useScale * glyph->top;
                if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE)
                {
                    int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
                    colorBlack[3] = newColor[3];
                    trap_R_SetColor( colorBlack );
                    CG_Text_PaintChar(x + ofs, y - yadj + ofs,
                                      glyph->imageWidth,
                                      glyph->imageHeight,
                                      useScale,
                                      glyph->s,
                                      glyph->t,
                                      glyph->s2,
                                      glyph->t2,
                                      glyph->glyph);
                    colorBlack[3] = 1.0;
                    trap_R_SetColor( newColor );

                    // loadingscreen
                    // based on Tremulous code
                }
                else if( style == ITEM_TEXTSTYLE_NEON )
                {
                    vec4_t glow, outer, inner, white;

                    glow[ 0 ] = newColor[ 0 ] * 0.5;
                    glow[ 1 ] = newColor[ 1 ] * 0.5;
                    glow[ 2 ] = newColor[ 2 ] * 0.5;
                    glow[ 3 ] = newColor[ 3 ] * 0.2;

                    outer[ 0 ] = newColor[ 0 ];
                    outer[ 1 ] = newColor[ 1 ];
                    outer[ 2 ] = newColor[ 2 ];
                    outer[ 3 ] = newColor[ 3 ];

                    inner[ 0 ] = newColor[ 0 ] * 1.5 > 1.0f ? 1.0f : newColor[ 0 ] * 1.5;
                    inner[ 1 ] = newColor[ 1 ] * 1.5 > 1.0f ? 1.0f : newColor[ 1 ] * 1.5;
                    inner[ 2 ] = newColor[ 2 ] * 1.5 > 1.0f ? 1.0f : newColor[ 2 ] * 1.5;
                    inner[ 3 ] = newColor[ 3 ];

                    white[ 0 ] = white[ 1 ] = white[ 2 ] = white[ 3 ] = 1.0f;

                    trap_R_SetColor( glow );
                    CG_Text_PaintChar(  x - 3, y - yadj - 3,
                                        glyph->imageWidth + 6,
                                        glyph->imageHeight + 6,
                                        useScale,
                                        glyph->s,
                                        glyph->t,
                                        glyph->s2,
                                        glyph->t2,
                                        glyph->glyph );

                    trap_R_SetColor( outer );
                    CG_Text_PaintChar(  x - 1, y - yadj - 1,
                                        glyph->imageWidth + 2,
                                        glyph->imageHeight + 2,
                                        useScale,
                                        glyph->s,
                                        glyph->t,
                                        glyph->s2,
                                        glyph->t2,
                                        glyph->glyph );

                    trap_R_SetColor( inner );
                    CG_Text_PaintChar(  x - 0.5, y - yadj - 0.5,
                                        glyph->imageWidth + 1,
                                        glyph->imageHeight + 1,
                                        useScale,
                                        glyph->s,
                                        glyph->t,
                                        glyph->s2,
                                        glyph->t2,
                                        glyph->glyph );

                    trap_R_SetColor( white );
                    // end loadingscreen
                }
                CG_Text_PaintChar(x, y - yadj,
                                  glyph->imageWidth,
                                  glyph->imageHeight,
                                  useScale,
                                  glyph->s,
                                  glyph->t,
                                  glyph->s2,
                                  glyph->t2,
                                  glyph->glyph);
                // CG_DrawPic(x, y - yadj, scale * cgDC.Assets.textFont.glyphs[text[i]].imageWidth, scale * cgDC.Assets.textFont.glyphs[text[i]].imageHeight, cgDC.Assets.textFont.glyphs[text[i]].glyph);
                x += (glyph->xSkip * useScale) + adjust;
                s++;
                count++;
            }
        }
        trap_R_SetColor( NULL );
    }
}


/*
================
CG_Draw3DModel

================
*/
void CG_Draw3DModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles )
{
    refdef_t		refdef;
    refEntity_t		ent;

    if ( !cg_draw3dIcons.integer || !cg_drawIcons.integer )
    {
        return;
    }

    CG_AdjustFrom640( &x, &y, &w, &h );

    memset( &refdef, 0, sizeof( refdef ) );

    memset( &ent, 0, sizeof( ent ) );
    AnglesToAxis( angles, ent.axis );
    VectorCopy( origin, ent.origin );
    ent.hModel = model;
    ent.customSkin = skin;
    ent.renderfx = RF_NOSHADOW;		// no stencil shadows

    refdef.rdflags = RDF_NOWORLDMODEL | RDF_NOSHADOWS;

    AxisClear( refdef.viewaxis );

    refdef.fov_x = 30;
    refdef.fov_y = 30;

    refdef.x = x;
    refdef.y = y;
    refdef.width = w;
    refdef.height = h;

    refdef.time = cg.time;

    trap_R_ClearScene();
    trap_R_AddRefEntityToScene( &ent );
    trap_R_RenderScene( &refdef );
}

/*
================
CG_DrawHead

Used for both the status bar and the scoreboard
================
*/
void CG_DrawHead( float x, float y, float w, float h, int clientNum, vec3_t headAngles )
{
    clipHandle_t	cm;
    clientInfo_t	*ci;
    float			len;
    vec3_t			origin;
    vec3_t			mins, maxs;

    ci = &cgs.clientinfo[ clientNum ];

    if ( cg_draw3dIcons.integer )
    {
        cm = ci->headModel;
        if ( !cm )
        {
            return;
        }

        // offset the origin y and z to center the head
        trap_R_ModelBounds( cm, mins, maxs );

        origin[2] = -0.5 * ( mins[2] + maxs[2] );
        origin[1] = 0.5 * ( mins[1] + maxs[1] );

        // calculate distance so the head nearly fills the box
        // assume heads are taller than wide
        len = 0.7 * ( maxs[2] - mins[2] );
        origin[0] = len / 0.268;	// len / tan( fov/2 )

        // allow per-model tweaking
        VectorAdd( origin, ci->headOffset, origin );

        CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, origin, headAngles );
    }
    else if ( cg_drawIcons.integer )
    {
        CG_DrawPic( x, y, w, h, ci->modelIcon );
    }

    // if they are deferred, draw a cross out
    if ( ci->deferred )
    {
        CG_DrawPic( x, y, w, h, cgs.media.deferShader );
    }
}


void CG_DrawWeapon( void )
{
    clipHandle_t	cm;
    clientInfo_t	*ci;
    vec3_t			origin;
    vec3_t			mins, maxs;
    vec3_t angles;

    if( cg.renderingThirdPerson || !cg_drawGun.integer)
        return;

    ci = &cgs.clientinfo[ cg.clientNum ];

    cm = trap_R_RegisterModel(va("models/players/%s/weapon.md3", ci->modelName));

    if ( !cm )
        return;

    // offset the origin y and z to center the weapon
    trap_R_ModelBounds( cm, mins, maxs );



    origin[0] = 30;
    origin[2] = -5;

    angles[0] = -60;
    angles[1] = 60;
    angles[2] = 30;

    CG_Draw3DModel( 320, 320, 320, 320, cm,
                    0, origin, angles );
}

/*
================
CG_DrawFlagModel

Used for both the status bar and the scoreboard
================
*/
void CG_DrawFlagModel( float x, float y, float w, float h, int team, qboolean force2D )
{
    qhandle_t		cm;
    float			len;
    vec3_t			origin, angles;
    vec3_t			mins, maxs;
    qhandle_t		handle;

    if ( !force2D && cg_draw3dIcons.integer )
    {

        VectorClear( angles );

        cm = cgs.media.redFlagModel;

        // offset the origin y and z to center the flag
        trap_R_ModelBounds( cm, mins, maxs );

        origin[2] = -0.5 * ( mins[2] + maxs[2] );
        origin[1] = 0.5 * ( mins[1] + maxs[1] );

        // calculate distance so the flag nearly fills the box
        // assume heads are taller than wide
        len = 0.5 * ( maxs[2] - mins[2] );
        origin[0] = len / 0.268;	// len / tan( fov/2 )

        angles[YAW] = 60 * sin( cg.time / 2000.0 );;

        if( team == TEAM_RED )
        {
            handle = cgs.media.redFlagModel;
        }
        else if( team == TEAM_BLUE )
        {
            handle = cgs.media.blueFlagModel;
        }
        else if( team == TEAM_FREE )
        {
            handle = cgs.media.neutralFlagModel;
        }
        else
        {
            return;
        }
        CG_Draw3DModel( x, y, w, h, handle, 0, origin, angles );
    }
    else if ( cg_drawIcons.integer )
    {
        gitem_t *item;

        if( team == TEAM_RED )
        {
            item = BG_FindItemForPowerup( PW_REDFLAG );
        }
        else if( team == TEAM_BLUE )
        {
            item = BG_FindItemForPowerup( PW_BLUEFLAG );
        }
        else if( team == TEAM_FREE )
        {
            item = BG_FindItemForPowerup( PW_NEUTRALFLAG );
        }
        else
        {
            return;
        }
        if (item)
        {
            CG_DrawPic( x, y, w, h, cg_items[ ITEM_INDEX(item) ].icon );
        }
    }
}

/*
================
CG_DrawTeamBackground

================
*/
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team )
{
    vec4_t		hcolor;

    hcolor[3] = alpha;
    if ( team == TEAM_RED )
    {
        hcolor[0] = 1;
        hcolor[1] = 0;
        hcolor[2] = 0;
    }
    else if ( team == TEAM_BLUE )
    {
        hcolor[0] = 0;
        hcolor[1] = 0;
        hcolor[2] = 1;
    }
    else
    {
        return;
    }
    trap_R_SetColor( hcolor );
    CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
    trap_R_SetColor( NULL );
}


static void CG_DrawRadar( void )
{
    vec3_t      tmpVec, eAngle, forward, dAngle;
    clientInfo_t	*ci;
    int	pclass;
    int         x, y, z, i, h;

    if (!IS_SET(cg.snap->ps.persistant[PERS_UPGRADES], (1 << UG_RADAR)))
        return;

    CG_DrawPic(246, 2, 152, 100, cgs.media.radarShader);
    for (i = 0; i < cg.snap->numEntities; i++) // Go through all entities in VIS range
    {
        if (cg.snap->entities[i].eType == ET_PLAYER) // If the Entity is a Player
        {
            ci = &cgs.clientinfo[ cg.snap->entities[i].clientNum ];
            pclass = CG_ClassforPlayer( ci->modelName, ci->skinName );

            // Calculate How Far Away They Are
            x = (cg.snap->entities[i].pos.trBase[0] - cg.predictedPlayerState.origin[0]);
            y = (cg.snap->entities[i].pos.trBase[1] - cg.predictedPlayerState.origin[1]);
            z = (cg.snap->entities[i].pos.trBase[2] - cg.predictedPlayerState.origin[2]);
            tmpVec[0] = x;
            tmpVec[1] = y;
            tmpVec[2] = 0.0;

            // Convert Vector to Angle
            vectoangles(tmpVec, eAngle);
            h = sqrt((x*x) + (y*y)); // Get Range

            // We only Want "YAW" value
            dAngle[0] = 0.0;
            dAngle[1] = AngleSubtract(eAngle[1] - 180, cg.predictedPlayerState.viewangles[1]) + 180;
            dAngle[0] = 0.0;

            // Convert Angle back to Vector
            AngleVectors(dAngle, forward, NULL, NULL);
            VectorScale(forward, bg_playerlist[pclass].radarrange, forward);

            if (h/bg_playerlist[pclass].radarrange < 100 && h/bg_playerlist[pclass].radarrange > 0) // Limit Radar Range
            {
                // Draw up arrow if above, down if below, or an ordinary blip if level
                // With tolerance of +- 5 units
                if (z > 5)
                    CG_DrawPic(318 - forward[1], 48 - forward[0], 7, 7, cgs.media.rd_up);
                else if (z < -5)
                    CG_DrawPic(318 - forward[1], 48 - forward[0], 7, 7, cgs.media.rd_down);
                else
                    CG_DrawPic(318 - forward[1], 48 - forward[0], 7, 7, cgs.media.rd_level);
            }
        }
    }
}


#define SENSE_RANGE 1800
/*
=================
CG_DrawSense
Author: mide
=================
*/
static void CG_DrawSense( void )
{
    char      msg[30];
    int         i;
    vec3_t      dir;

    if (!IS_SET(cg.snap->ps.persistant[PERS_UPGRADES], (1 << UG_SENSE)))
        return;

    for( i = 0; i < MAX_CLIENTS; i++ )
    {
        if( !cg_entities[i].currentValid )
            continue;

        if( cg_entities[i].currentState.number == cg.clientNum )
            continue;

        VectorSubtract( cg_entities[i].lerpOrigin, cg_entities[cg.clientNum].lerpOrigin, dir );
        VectorNormalize( dir );

        // mide - we dont want a message when a player is in front of us.
        if( DotProduct( dir, cg.refdef.viewaxis[0] ) > 0 )
            continue;

        if( Distance( cg_entities[i].lerpOrigin, cg_entities[cg.clientNum].lerpOrigin ) > SENSE_RANGE )
            continue;

        Com_sprintf( msg, sizeof( msg ), "%s is close!", cgs.clientinfo[i].name );
        CG_DrawStringExt( 5, SCREEN_HEIGHT/2+(i*5), msg, colorRed, qfalse, qfalse, 10, 12, 0 );
    }
}



/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

/*
================
CG_DrawAttacker

================
*/
static float CG_DrawAttacker( float y )
{
    int			t;
    float		size;
    vec3_t		angles;
    const char	*info;
    const char	*name;
    int			clientNum;

    if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 )
    {
        return y;
    }

    if ( !cg.attackerTime )
    {
        return y;
    }

    clientNum = cg.predictedPlayerState.persistant[PERS_ATTACKER];
    if ( clientNum < 0 || clientNum >= MAX_CLIENTS || clientNum == cg.snap->ps.clientNum )
    {
        return y;
    }

    t = cg.time - cg.attackerTime;
    if ( t > ATTACKER_HEAD_TIME )
    {
        cg.attackerTime = 0;
        return y;
    }

    size = ICON_SIZE * 1.25;

    angles[PITCH] = 0;
    angles[YAW] = 180;
    angles[ROLL] = 0;
    CG_DrawHead( 640 - size, y, size, size, clientNum, angles );

    info = CG_ConfigString( CS_PLAYERS + clientNum );
    name = Info_ValueForKey(  info, "n" );
    y += size;
    CG_DrawBigString( 640 - ( Q_PrintStrlen( name ) * BIGCHAR_WIDTH), y, name, 0.5 );

    return y + BIGCHAR_HEIGHT + 2;
}

/*
==================
CG_DrawSnapshot
==================
*/
static float CG_DrawSnapshot( float y )
{
    char		*s;
    int			w;

    s = va( "time:%i snap:%i cmd:%i", cg.snap->serverTime,
            cg.latestSnapshotNum, cgs.serverCommandSequence );
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

    CG_DrawBigString( 635 - w, y + 2, s, 1.0F);

    return y + BIGCHAR_HEIGHT + 4;
}

/*
==================
CG_DrawFPS
==================
*/
#define	FPS_FRAMES	4
static float CG_DrawFPS( float y )
{
    char		*s;
    int			w;
    static int	previousTimes[FPS_FRAMES];
    static int	index;
    int		i, total;
    int		fps;
    static	int	previous;
    int		t, frameTime;

    // don't use serverTime, because that will be drifting to
    // correct for internet lag changes, timescales, timedemos, etc
    t = trap_Milliseconds();
    frameTime = t - previous;
    previous = t;

    previousTimes[index % FPS_FRAMES] = frameTime;
    index++;
    if ( index > FPS_FRAMES )
    {
        // average multiple frames together to smooth changes out a bit
        total = 0;
        for ( i = 0 ; i < FPS_FRAMES ; i++ )
        {
            total += previousTimes[i];
        }
        if ( !total )
        {
            total = 1;
        }
        fps = 1000 * FPS_FRAMES / total;

        s = va( "%ifps", fps );
        w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

        CG_DrawBigString( 635 - w, y + 2, s, 1.0F);
    }

    return y + BIGCHAR_HEIGHT + 4;
}


/*
=================
CG_DrawTimer
=================
Peter FIXME: Lets get rid of the old cg stuff
*/
static float CG_DrawTimerOld( float y )
{
    char		*s;
    int			w;
    int			mins, seconds, tens;
    int			msec;

    msec = cg.time - cgs.levelStartTime;

    seconds = msec / 1000;
    mins = seconds / 60;
    seconds -= mins * 60;
    tens = seconds / 10;
    seconds -= tens * 10;

    s = va( "%i:%i%i", mins, tens, seconds );
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

    CG_DrawBigString( 635 - w, y + 2, s, 1.0F);

    return y + BIGCHAR_HEIGHT + 4;
}

/*=================
           CG_DrawTimer
=================*/
static void CG_DrawTimer( rectDef_t *rect, float text_x, float text_y,
                          float scale, vec4_t color, int align, int textStyle )
{
    char *s;
    int i, tx, w, totalWidth, strLength;
    int mins, seconds, tens;
    int msec;

    if(!(HudGroupFlag & UPPERIGHT))
        return;

    if( !cg_drawTimer.integer )
        return;

    msec = cg.time - cgs.levelStartTime;

    seconds = msec / 1000;
    mins = seconds / 60;
    seconds -= mins * 60;
    tens = seconds / 10;
    seconds -= tens * 10;

    s = va( "%d:%d%d", mins, tens, seconds );
    w = CG_Text_Width( "0", scale, 0 );
    strLength = CG_DrawStrlen( s );
    totalWidth = w * strLength;

    switch( align )
    {
    case ITEM_ALIGN_LEFT:
        tx = rect->x;
        break;

    case ITEM_ALIGN_RIGHT:
        tx = rect->x + rect->w - totalWidth;
        break;

    case ITEM_ALIGN_CENTER:
        tx = rect->x + ( rect->w / 2.0f ) - ( totalWidth / 2.0f );
        break;

    default:
        tx = 0.0f;
    }

    for( i = 0; i < strLength; i++ )
    {
        char c[ 2 ];

        c[ 0 ] = s[ i ];
        c[ 1 ] = '\0';

        CG_Text_Paint( text_x + tx + i * w, rect->y + text_y, scale, color, c, 0, 0, textStyle );
    }
}


/*
=================
CG_DrawTeamOverlay
=================
*/

static float CG_DrawTeamOverlay( float y, qboolean right, qboolean upper )
{
    int x, w, h, xx;
    int i, j, len;
    const char *p;
    vec4_t		hcolor;
    int pwidth, lwidth;
    int plyrs;
    char st[16];
    clientInfo_t *ci;
    gitem_t	*item;
    int ret_y, count;
    int weap;

    if ( !cg_drawTeamOverlay.integer )
    {
        return y;
    }

    if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_RED && cg.snap->ps.persistant[PERS_TEAM] != TEAM_BLUE )
    {
        return y; // Not on any team
    }

    plyrs = 0;

    // max player name width
    pwidth = 0;
    count = (numSortedTeamPlayers > 8) ? 8 : numSortedTeamPlayers;
    for (i = 0; i < count; i++)
    {
        ci = cgs.clientinfo + sortedTeamPlayers[i];
        weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[ci->curWeapon].weapon;
        if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM])
        {
            plyrs++;
            len = CG_DrawStrlen(ci->name);
            if (len > pwidth)
                pwidth = len;
        }
    }

    if (!plyrs)
        return y;

    if (pwidth > TEAM_OVERLAY_MAXNAME_WIDTH)
        pwidth = TEAM_OVERLAY_MAXNAME_WIDTH;

    // max location name width
    lwidth = 0;
    for (i = 1; i < MAX_LOCATIONS; i++)
    {
        p = CG_ConfigString(CS_LOCATIONS + i);
        if (p && *p)
        {
            len = CG_DrawStrlen(p);
            if (len > lwidth)
                lwidth = len;
        }
    }

    if (lwidth > TEAM_OVERLAY_MAXLOCATION_WIDTH)
        lwidth = TEAM_OVERLAY_MAXLOCATION_WIDTH;

    w = (pwidth + lwidth + 4 + 7) * TINYCHAR_WIDTH;

    if ( right )
        x = 640 - w;
    else
        x = 0;

    h = plyrs * TINYCHAR_HEIGHT;

    if ( upper )
    {
        ret_y = y + h;
    }
    else
    {
        y -= h;
        ret_y = y;
    }

    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED )
    {
        hcolor[0] = 1.0f;
        hcolor[1] = 0.0f;
        hcolor[2] = 0.0f;
        hcolor[3] = 0.33f;
    }
    else     // if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
    {
        hcolor[0] = 0.0f;
        hcolor[1] = 0.0f;
        hcolor[2] = 1.0f;
        hcolor[3] = 0.33f;
    }
    trap_R_SetColor( hcolor );
    CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
    trap_R_SetColor( NULL );

    for (i = 0; i < count; i++)
    {
        ci = cgs.clientinfo + sortedTeamPlayers[i];
        if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM])
        {

            hcolor[0] = hcolor[1] = hcolor[2] = hcolor[3] = 1.0;

            xx = x + TINYCHAR_WIDTH;

            CG_DrawStringExt( xx, y,
                              ci->name, hcolor, qfalse, qfalse,
                              TINYCHAR_WIDTH, TINYCHAR_HEIGHT, TEAM_OVERLAY_MAXNAME_WIDTH);

            if (lwidth)
            {
                p = CG_ConfigString(CS_LOCATIONS + ci->location);
                if (!p || !*p)
                    p = "unknown";
                len = CG_DrawStrlen(p);
                if (len > lwidth)
                    len = lwidth;

//				xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth +
//					((lwidth/2 - len/2) * TINYCHAR_WIDTH);
                xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth;
                CG_DrawStringExt( xx, y,
                                  p, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
                                  TEAM_OVERLAY_MAXLOCATION_WIDTH);
            }

            CG_GetColorForHealth( ci->health, ci->armor, hcolor );

            Com_sprintf (st, sizeof(st), "%3i %3i", ci->health,	ci->armor);

            xx = x + TINYCHAR_WIDTH * 3 +
                 TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;

            CG_DrawStringExt( xx, y,
                              st, hcolor, qfalse, qfalse,
                              TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );

            // draw weapon icon
            xx += TINYCHAR_WIDTH * 3;

            if ( cg_weapons[ci->curWeapon].weaponIcon )
            {
                CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
                            cg_weapons[ci->curWeapon].weaponIcon );
            }
            else
            {
                CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
                            cgs.media.deferShader );
            }

            // Draw powerup icons
            if (right)
            {
                xx = x;
            }
            else
            {
                xx = x + w - TINYCHAR_WIDTH;
            }
            for (j = 0; j <= PW_NUM_POWERUPS; j++)
            {
                if (ci->powerups & (1 << j))
                {

                    item = BG_FindItemForPowerup( j );

                    if (item)
                    {
                        CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
                                    trap_R_RegisterShader( item->icon ) );
                        if (right)
                        {
                            xx -= TINYCHAR_WIDTH;
                        }
                        else
                        {
                            xx += TINYCHAR_WIDTH;
                        }
                    }
                }
            }

            y += TINYCHAR_HEIGHT;
        }
    }

    return ret_y;
//#endif
}


/*
=====================
CG_DrawUpperRight

=====================
*/
static void CG_DrawUpperRight( void )
{
    float	y;

    y = 0;

    if ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer == 1 )
    {
        y = CG_DrawTeamOverlay( y, qtrue, qtrue );
    }
    if ( cg_drawSnapshot.integer )
    {
        y = CG_DrawSnapshot( y );
    }
    if ( cg_drawFPS.integer )
    {
        y = CG_DrawFPS( y );
    }
    if ( cg_drawTimer.integer )
    {
        y = CG_DrawTimerOld( y );
    }
    if ( cg_drawAttacker.integer )
    {
        y = CG_DrawAttacker( y );
    }

}

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/


/*
===================
CG_DrawReward
===================
*/
static void CG_DrawReward( void )
{
    float	*color;
    int		i, count;
    float	x, y;
    char	buf[32];

    if ( !cg_drawRewards.integer )
    {
        return;
    }

    color = CG_FadeColor( cg.rewardTime, REWARD_TIME );
    if ( !color )
    {
        if (cg.rewardStack > 0)
        {
            for(i = 0; i < cg.rewardStack; i++)
            {
                cg.rewardSound[i] = cg.rewardSound[i+1];
                cg.rewardShader[i] = cg.rewardShader[i+1];
                cg.rewardCount[i] = cg.rewardCount[i+1];
            }
            cg.rewardTime = cg.time;
            cg.rewardStack--;
            color = CG_FadeColor( cg.rewardTime, REWARD_TIME );
            trap_S_StartLocalSound(cg.rewardSound[0], CHAN_ANNOUNCER);
        }
        else
        {
            return;
        }
    }

    trap_R_SetColor( color );

    /*
    count = cg.rewardCount[0]/10;				// number of big rewards to draw

    if (count) {
    	y = 4;
    	x = 320 - count * ICON_SIZE;
    	for ( i = 0 ; i < count ; i++ ) {
    		CG_DrawPic( x, y, (ICON_SIZE*2)-4, (ICON_SIZE*2)-4, cg.rewardShader[0] );
    		x += (ICON_SIZE*2);
    	}
    }

    count = cg.rewardCount[0] - count*10;		// number of small rewards to draw
    */

    if ( cg.rewardCount[0] >= 10 )
    {
        y = 56;
        x = 320 - ICON_SIZE/2;
        CG_DrawPic( x, y, ICON_SIZE-4, ICON_SIZE-4, cg.rewardShader[0] );
        Com_sprintf(buf, sizeof(buf), "%d", cg.rewardCount[0]);
        x = ( SCREEN_WIDTH - SMALLCHAR_WIDTH * CG_DrawStrlen( buf ) ) / 2;
        CG_DrawStringExt( x, y+ICON_SIZE, buf, color, qfalse, qtrue,
                          SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
    }
    else
    {

        count = cg.rewardCount[0];

        y = 56;
        x = 320 - count * ICON_SIZE/2;
        for ( i = 0 ; i < count ; i++ )
        {
            CG_DrawPic( x, y, ICON_SIZE-4, ICON_SIZE-4, cg.rewardShader[0] );
            x += ICON_SIZE;
        }
    }
    trap_R_SetColor( NULL );
}


/*
===============================================================================

LAGOMETER

===============================================================================
*/

#define LAG_SAMPLES 128

typedef struct
{
    int frameSamples[ LAG_SAMPLES ];
    int frameCount;
    int snapshotFlags[ LAG_SAMPLES ];
    int snapshotSamples[ LAG_SAMPLES ];
    int snapshotCount;
} lagometer_t;

lagometer_t lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
*/
void CG_AddLagometerFrameInfo( void )
{
    int			offset;

    offset = cg.time - cg.latestSnapshotTime;
    lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1) ] = offset;
    lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
*/
void CG_AddLagometerSnapshotInfo( snapshot_t *snap )
{
    // dropped packet
    if ( !snap )
    {
        lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = -1;
        lagometer.snapshotCount++;
        return;
    }

    // add this snapshot's info
    lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->ping;
    lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->snapFlags;
    lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
static void CG_DrawDisconnect( void )
{
    float		x, y;
    int			cmdNum;
    usercmd_t	cmd;
    const char		*s;
    int			w;  // bk010215 - FIXME char message[1024];

    // draw the phone jack if we are completely past our buffers
    cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
    trap_GetUserCmd( cmdNum, &cmd );
    if ( cmd.serverTime <= cg.snap->ps.commandTime
            || cmd.serverTime > cg.time )  	// special check for map_restart // bk 0102165 - FIXME
    {
        return;
    }

    // also add text in center of screen
    s = "Connection Interrupted"; // bk 010215 - FIXME
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
    CG_DrawBigString( 320 - w/2, 100, s, 1.0F);

    // blink the icon
    if ( ( cg.time >> 9 ) & 1 )
    {
        return;
    }

    x = 640 - 48;
    y = 480 - 48;

    CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader("gfx/2d/net.tga" ) );
}


#define	MAX_LAGOMETER_PING	900
#define	MAX_LAGOMETER_RANGE	300

/*
==============
CG_DrawLagometer
==============
*/
static void CG_DrawLagometer( void )
{
    int		a, x, y, i;
    float	v;
    float	ax, ay, aw, ah, mid, range;
    int		color;
    float	vscale;

    if ( !cg_lagometer.integer || cgs.localServer )
    {
        CG_DrawDisconnect();
        return;
    }

    //
    // draw the graph
    //
    x = 640 - 48;
    y = 480 - 48;

    trap_R_SetColor( NULL );
    CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

    ax = x;
    ay = y;
    aw = 48;
    ah = 48;
    CG_AdjustFrom640( &ax, &ay, &aw, &ah );

    color = -1;
    range = ah / 3;
    mid = ay + range;

    vscale = range / MAX_LAGOMETER_RANGE;

    // draw the frame interpoalte / extrapolate graph
    for ( a = 0 ; a < aw ; a++ )
    {
        i = ( lagometer.frameCount - 1 - a ) & (LAG_SAMPLES - 1);
        v = lagometer.frameSamples[i];
        v *= vscale;
        if ( v > 0 )
        {
            if ( color != 1 )
            {
                color = 1;
                trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
            }
            if ( v > range )
            {
                v = range;
            }
            trap_R_DrawStretchPic ( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
        }
        else if ( v < 0 )
        {
            if ( color != 2 )
            {
                color = 2;
                trap_R_SetColor( g_color_table[ColorIndex(COLOR_BLUE)] );
            }
            v = -v;
            if ( v > range )
            {
                v = range;
            }
            trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
        }
    }

    // draw the snapshot latency / drop graph
    range = ah / 2;
    vscale = range / MAX_LAGOMETER_PING;

    for ( a = 0 ; a < aw ; a++ )
    {
        i = ( lagometer.snapshotCount - 1 - a ) & (LAG_SAMPLES - 1);
        v = lagometer.snapshotSamples[i];
        if ( v > 0 )
        {
            if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED )
            {
                if ( color != 5 )
                {
                    color = 5;	// YELLOW for rate delay
                    trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
                }
            }
            else
            {
                if ( color != 3 )
                {
                    color = 3;
                    trap_R_SetColor( g_color_table[ColorIndex(COLOR_GREEN)] );
                }
            }
            v = v * vscale;
            if ( v > range )
            {
                v = range;
            }
            trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
        }
        else if ( v < 0 )
        {
            if ( color != 4 )
            {
                color = 4;		// RED for dropped snapshots
                trap_R_SetColor( g_color_table[ColorIndex(COLOR_RED)] );
            }
            trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
        }
    }

    trap_R_SetColor( NULL );

    if ( cg_nopredict.integer || cg_synchronousClients.integer )
    {
        CG_DrawBigString( ax, ay, "snc", 1.0 );
    }

    CG_DrawDisconnect();
}



/*
===============================================================================

CENTER PRINTING

===============================================================================
*/


/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void CG_CenterPrint( const char *str, int y, int charWidth )
{
    char	*s;

    Q_strncpyz( cg.centerPrint, str, sizeof(cg.centerPrint) );

    cg.centerPrintTime = cg.time;
    cg.centerPrintY = y;
    cg.centerPrintCharWidth = charWidth;

    // count the number of lines for centering
    cg.centerPrintLines = 1;
    s = cg.centerPrint;
    while( *s )
    {
        if (*s == '\n')
            cg.centerPrintLines++;
        s++;
    }
}


/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString( void )
{
    char	*start;
    int		l;
    int		x, y, w;
    int		h;
    float	*color;

    if ( !cg.centerPrintTime )
    {
        return;
    }

    color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
    if ( !color )
    {
        return;
    }

    trap_R_SetColor( color );

    start = cg.centerPrint;

    y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

    while ( 1 )
    {
        char linebuffer[1024];

        for ( l = 0; l < 50; l++ )
        {
            if ( !start[l] || start[l] == '\n' )
            {
                break;
            }
            linebuffer[l] = start[l];
        }
        linebuffer[l] = 0;

        w = CG_Text_Width(linebuffer, 0.5, 0);
        h = CG_Text_Height(linebuffer, 0.5, 0);
        x = (SCREEN_WIDTH - w) / 2;
        CG_Text_Paint(x, y + h, 0.5, color, linebuffer, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
        y += h + 6;

        while ( *start && ( *start != '\n' ) )
        {
            start++;
        }
        if ( !*start )
        {
            break;
        }
        start++;
    }

    trap_R_SetColor( NULL );
}



/*
================================================================================

CROSSHAIR

================================================================================
*/



qboolean CG_WorldCoordToScreenCoordFloat( vec3_t worldCoord, float *x, float *y )
{
    int	xcenter, ycenter;
    vec3_t	local, transformed;
    vec3_t	vfwd;
    vec3_t	vright;
    vec3_t	vup;
    float xzi;
    float yzi;

//	xcenter = cg.refdef.width / 2;//gives screen coords adjusted for resolution
//	ycenter = cg.refdef.height / 2;//gives screen coords adjusted for resolution

    //NOTE: did it this way because most draw functions expect virtual 640x480 coords
    //	and adjust them for current resolution
    xcenter = 640 / 2;	//gives screen coords in virtual 640x480, to be adjusted when drawn
    ycenter = 480 / 2;	//gives screen coords in virtual 640x480, to be adjusted when drawn

    AngleVectors( cg.refdefViewAngles, vfwd, vright, vup );

    VectorSubtract( worldCoord, cg.refdef.vieworg, local );

    transformed[0] = DotProduct(local,vright);
    transformed[1] = DotProduct(local,vup);
    transformed[2] = DotProduct(local,vfwd);

    // Make sure Z is not negative.
    if( transformed[2] < 0.01 )
        return qfalse;

    xzi = xcenter / transformed[2] * (90.0/cg.refdef.fov_x);
    yzi = ycenter / transformed[2] * (90.0/cg.refdef.fov_y);

    *x = xcenter + xzi * transformed[0];
    *y = ycenter - yzi * transformed[1];

    return qtrue;
}

qboolean CG_WorldCoordToScreenCoord( vec3_t worldCoord, int *x, int *y )
{
    float	xF, yF;
    qboolean retVal = CG_WorldCoordToScreenCoordFloat( worldCoord, &xF, &yF );
    *x = (int)xF;
    *y = (int)yF;
    return retVal;
}

/*
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair( vec3_t worldPoint, int is_ent )
{
    float		w, h;
    float		f;
    qhandle_t	hShader;
    float		x, y;
    int			ca;

    if( !cg_drawCrosshair.integer )
        return;

    // colors// set color based on health
    if ( cg_crosshairHealth.integer )
    {
        vec4_t		hcolor;

        CG_ColorForHealth( hcolor );
        trap_R_SetColor( hcolor );
    }
    else
    {
        trap_R_SetColor( NULL );
    }

    w = h = cg_crosshairSize.value;
    // pulse the size of the crosshair when picking up items
    f = cg.time - cg.itemPickupBlendTime;
    if ( f > 0 && f < ITEM_BLOB_TIME )
    {
        f /= ITEM_BLOB_TIME;
        w *= ( 1 + f );
        h *= ( 1 + f );
    }

    if( worldPoint && VectorLength(worldPoint) )
    {
        if( !CG_WorldCoordToScreenCoordFloat( worldPoint, &x, &y ) )
            return;		//off screen, don't draw it

        x -= 320;
        y -= 240;
    }
    else
    {
        x = cg_crosshairX.integer;
        y = cg_crosshairY.integer;
    }

    ca = cg_drawCrosshair.integer;
    if (ca < 0)
    {
        ca = 0;
    }
    hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

    CG_AdjustFrom640( &x, &y, &w, &h );

    trap_R_DrawStretchPic(	x + cg.refdef.x + 0.5 * (cg.refdef.width - w),
                            y + cg.refdef.y + 0.5 * (cg.refdef.height - h),
                            w, h, 0, 0, 1, 1, hShader );
}


/*
=================
CG_ScanForCrosshairEntity
=================
*/
void CG_ScanForCrosshairEntity( void )
{
    trace_t		trace;
    vec3_t		start, end;
    int			content;


    /*	VectorCopy( cg.refdef.vieworg, start );
    	VectorMA( start, 131072, cg.refdef.viewaxis[0], end );

    	CG_Trace( &trace, start, vec3_origin, vec3_origin, end,
    		cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY );
    	if ( trace.entityNum >= MAX_CLIENTS ) {
    		return;
    	}
    */
    if( 1 )
    {
        vec3_t d_f, d_rt, d_up;
        vec3_t pitchConstraint;
        vec3_t	muzzle;
        vec3_t	forward;
//		int i;

        VectorCopy( cg.snap->ps.origin, muzzle );
        muzzle[2] += cg.snap->ps.viewheight;
        AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
        VectorMA( muzzle, 14, forward, muzzle );

        if( cg.renderingThirdPerson )
        {
            VectorCopy(cg.predictedPlayerState.viewangles, pitchConstraint);
        }
        else
        {
            VectorCopy(cg.refdefViewAngles, pitchConstraint);
        }

        AngleVectors( pitchConstraint, d_f, d_rt, d_up );

        VectorClear(start);
        VectorAdd( start, muzzle, start );

        //VectorScale(start, (1.0f / (float)i), start);

        VectorMA( start, 4096, d_f, end );//was 8192
    }
    else
    {
        VectorCopy( cg.refdef.vieworg, start );
        VectorMA( start, 131072, cg.refdef.viewaxis[0], end );
    }

    CG_Trace( &trace, start, vec3_origin, vec3_origin, end,
              cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY );

    if( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
    {
        //CG_Printf( NULL, "drawing crosshair!\n");
        if( trace.entityNum < /*MAX_CLIENTS*/ENTITYNUM_WORLD )
            CG_DrawCrosshair( trace.endpos, 1 );
        else
            CG_DrawCrosshair( trace.endpos, 0 );
    }

    // if the player is in fog, don't show it
    content = trap_CM_PointContents( trace.endpos, 0 );
    if ( content & CONTENTS_FOG )
    {
        return;
    }

    if ( trace.entityNum >= MAX_CLIENTS )
    {
        return;
    }

    // if the player is cloaked, don't show it
    if ( cg_entities[ trace.entityNum ].currentState.powerups & ( 1 << PW_CLOAK ) )
    {
        return;
    }

    // update the fade timer
    cg.crosshairClientNum = trace.entityNum;
    cg.crosshairClientTime = cg.time;
}


/*
=====================
CG_DrawCrosshairNames
=====================
*/
static void CG_DrawCrosshairNames( void )
{
    float		*color;
    char		*name;
    float		w;

    if ( !cg_drawCrosshair.integer )
    {
        return;
    }
    if ( !cg_drawCrosshairNames.integer )
    {
        return;
    }
    /*if ( cg.renderingThirdPerson ) {
    	return;
    }*/

    // scan the known entities to see if the crosshair is sighted on one
    CG_ScanForCrosshairEntity();

    // draw the name of the player being looked at
    color = CG_FadeColor( cg.crosshairClientTime, 1000 );
    if ( !color )
    {
        trap_R_SetColor( NULL );
        return;
    }

    name = cgs.clientinfo[ cg.crosshairClientNum ].name;
    color[3] *= 0.5f;
    w = CG_Text_Width(name, 0.3f, 0);
    CG_Text_Paint( 320 - w / 2, 190, 0.3f, color, name, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
    trap_R_SetColor( NULL );
}


//==============================================================================

/*
=================
CG_DrawSpectator
=================
*/
static void CG_DrawSpectator(void)
{
    CG_DrawBigString(320 - 9 * 8, 440, "SPECTATOR", 1.0F);
    if ( cgs.gametype == GT_TOURNAMENT )
    {
        CG_DrawBigString(320 - 15 * 8, 460, "waiting to play", 1.0F);
    }
    else if ( cgs.gametype >= GT_TEAM )
    {
        CG_DrawBigString(320 - 39 * 8, 460, "press ESC and use the JOIN menu to play", 1.0F);
    }
}
/*
=================
CG_DrawVote
=================
*/
static void CG_DrawVote(void)
{
    char	*s;
    int		sec;

    if ( !cgs.voteTime )
    {
        return;
    }

    // play a talk beep whenever it is modified
    if ( cgs.voteModified )
    {
        cgs.voteModified = qfalse;
        trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
    }

    sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
    if ( sec < 0 )
    {
        sec = 0;
    }
    s = va("VOTE(%i):%s yes:%i no:%i", sec, cgs.voteString, cgs.voteYes, cgs.voteNo );
    CG_DrawSmallString( 0, 58, s, 1.0F );
}

/*
=================
CG_DrawTeamVote
=================
*/
static void CG_DrawTeamVote(void)
{
    char	*s;
    int		sec, cs_offset;

    if ( cgs.clientinfo->team == TEAM_RED )
        cs_offset = 0;
    else if ( cgs.clientinfo->team == TEAM_BLUE )
        cs_offset = 1;
    else
        return;

    if ( !cgs.teamVoteTime[cs_offset] )
    {
        return;
    }

    // play a talk beep whenever it is modified
    if ( cgs.teamVoteModified[cs_offset] )
    {
        cgs.teamVoteModified[cs_offset] = qfalse;
        trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
    }

    sec = ( VOTE_TIME - ( cg.time - cgs.teamVoteTime[cs_offset] ) ) / 1000;
    if ( sec < 0 )
    {
        sec = 0;
    }
    s = va("TEAMVOTE(%i):%s yes:%i no:%i", sec, cgs.teamVoteString[cs_offset],
           cgs.teamVoteYes[cs_offset], cgs.teamVoteNo[cs_offset] );
    CG_DrawSmallString( 0, 90, s, 1.0F );
}


static qboolean CG_DrawScoreboard(void)
{
    static qboolean firstTime = qtrue;
    float fade, *fadeColor;

    if (menuScoreboard)
    {
        menuScoreboard->window.flags &= ~WINDOW_FORCED;
    }
    if (cg_paused.integer)
    {
        cg.deferredPlayerLoading = 0;
        firstTime = qtrue;
        return qfalse;
    }

    // should never happen in Team Arena
    if (cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION )
    {
        cg.deferredPlayerLoading = 0;
        firstTime = qtrue;
        return qfalse;
    }

    // don't draw scoreboard during death while warmup up
    if ( cg.warmup && !cg.showScores )
    {
        return qfalse;
    }

    if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION )
    {
        fade = 1.0;
        fadeColor = colorWhite;
    }
    else
    {
        fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );
        if ( !fadeColor )
        {
            // next time scoreboard comes up, don't print killer
            cg.deferredPlayerLoading = 0;
            cg.killerName[0] = 0;
            firstTime = qtrue;
            return qfalse;
        }
        fade = *fadeColor;
    }


    if (menuScoreboard == NULL)
    {
        if ( cgs.gametype >= GT_TEAM )
        {
            menuScoreboard = Menus_FindByName("teamscore_menu");
        }
        else
        {
            menuScoreboard = Menus_FindByName("score_menu");
        }
    }

    if (menuScoreboard)
    {
        if (firstTime)
        {
            CG_SetScoreSelection(menuScoreboard);
            firstTime = qfalse;
        }
        Menu_Paint(menuScoreboard, qtrue);
    }

    // load any models that have been deferred
    if ( ++cg.deferredPlayerLoading > 10 )
    {
        CG_LoadDeferredPlayers();
    }

    return qtrue;
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission( void )
{
//	int key;
    //if (cg_singlePlayer.integer) {
    //	CG_DrawCenterString();
    //	return;
    //}
    cg.scoreFadeTime = cg.time;
    cg.scoreBoardShowing = CG_DrawScoreboard();
}

/*
=================
CG_DrawFollow
=================
*/
static qboolean CG_DrawFollow( void )
{
    float		x;
    vec4_t		color;
    const char	*name;

    if ( !(cg.snap->ps.pm_flags & PMF_FOLLOW) )
    {
        return qfalse;
    }
    color[0] = 1;
    color[1] = 1;
    color[2] = 1;
    color[3] = 1;


    CG_DrawBigString( 320 - 9 * 8, 24, "following", 1.0F );

    name = cgs.clientinfo[ cg.snap->ps.clientNum ].name;

    x = 0.5 * ( 640 - GIANT_WIDTH * CG_DrawStrlen( name ) );

    CG_DrawStringExt( x, 40, name, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );

    return qtrue;
}



/*
=================
CG_DrawAmmoWarning
=================
*/
static void CG_DrawAmmoWarning( void )
{
    const char	*s;
    int			w;

    if ( cg_drawAmmoWarning.integer == 0 )
    {
        return;
    }

    if ( !cg.lowAmmoWarning )
    {
        return;
    }

    if ( cg.lowAmmoWarning == 2 )
    {
        s = "OUT OF ENERGY";
    }
    else
    {
        s = "LOW ENERGY WARNING";
    }
    w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
    CG_DrawBigString(320 - w / 2, 64, s, 1.0F);
}



/*
=================
CG_DrawWarmup
=================
*/
static void CG_DrawWarmup( void )
{
    int			w;
    int			sec;
    int			i;
    float scale;
    clientInfo_t	*ci1, *ci2;
    int			cw;
    const char	*s;

    sec = cg.warmup;
    if ( !sec )
    {
        return;
    }

    if ( sec < 0 )
    {
        s = "Waiting for players";
        w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
        CG_DrawBigString(320 - w / 2, 24, s, 1.0F);
        cg.warmupCount = 0;
        return;
    }

    if (cgs.gametype == GT_TOURNAMENT)
    {
        // find the two active players
        ci1 = NULL;
        ci2 = NULL;
        for ( i = 0 ; i < cgs.maxclients ; i++ )
        {
            if ( cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_FREE )
            {
                if ( !ci1 )
                {
                    ci1 = &cgs.clientinfo[i];
                }
                else
                {
                    ci2 = &cgs.clientinfo[i];
                }
            }
        }

        if ( ci1 && ci2 )
        {
            s = va( "%s vs %s", ci1->name, ci2->name );
            w = CG_Text_Width(s, 0.6f, 0);
            CG_Text_Paint(320 - w / 2, 60, 0.6f, colorWhite, s, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
        }
    }
    else
    {
        if ( cgs.gametype == GT_FFA )
        {
            s = "Free For All";
        }
        else if ( cgs.gametype == GT_TEAM )
        {
            s = "Team Deathmatch";
        }
        else if ( cgs.gametype == GT_CTF )
        {
            s = "Capture the Flag";
        }
        else
        {
            s = "";
        }
        w = CG_Text_Width(s, 0.6f, 0);
        CG_Text_Paint(320 - w / 2, 90, 0.6f, colorWhite, s, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
    }

    sec = ( sec - cg.time ) / 1000;
    if ( sec < 0 )
    {
        cg.warmup = 0;
        sec = 0;
    }
    s = va( "Starts in: %i", sec + 1 );
    if ( sec != cg.warmupCount )
    {
        cg.warmupCount = sec;
        switch ( sec )
        {
        case 0:
            trap_S_StartLocalSound( cgs.media.count1Sound, CHAN_ANNOUNCER );
            break;
        case 1:
            trap_S_StartLocalSound( cgs.media.count2Sound, CHAN_ANNOUNCER );
            break;
        case 2:
            trap_S_StartLocalSound( cgs.media.count3Sound, CHAN_ANNOUNCER );
            break;
        default:
            break;
        }
    }
    scale = 0.45f;
    switch ( cg.warmupCount )
    {
    case 0:
        cw = 28;
        scale = 0.54f;
        break;
    case 1:
        cw = 24;
        scale = 0.51f;
        break;
    case 2:
        cw = 20;
        scale = 0.48f;
        break;
    default:
        cw = 16;
        scale = 0.45f;
        break;
    }

    w = CG_Text_Width(s, scale, 0);
    CG_Text_Paint(320 - w / 2, 125, scale, colorWhite, s, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
}

//==================================================================================
/*
=================
CG_DrawTimedMenus
=================
*/
void CG_DrawTimedMenus(void)
{
    if (cg.voiceTime)
    {
        int t = cg.time - cg.voiceTime;
        if ( t > 2500 )
        {
            Menus_CloseByName("voiceMenu");
            trap_Cvar_Set("cl_conXOffset", "0");
            cg.voiceTime = 0;
        }
    }
}

//==============================
// DRAW S.tiCREEN FLASH EFFECTS
// authors: Nightz / DonX
// Peter: You both suck, the shit was broke, i recoded it as 2 functions and now it works correctly
//==============================
void CG_DrawScreenFlash (void)
{
    if ( cg.screenFlash.screenFlashID == SF_NONE || (cg.time >= cg.screenFlash.screenFlashTime) )
        return;

    CG_DrawPic( 0, 0, 640, 480, cg.screenFlash.screenFlashShader);
}

void CG_SetScreenFlash (int screenFlashID)
{

    cg.screenFlash.screenFlashID = screenFlashID;;

    switch( screenFlashID )
    {
    case SF_LIGHTNINGSTRIKE:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Lightning1;
        cg.screenFlash.screenFlashTime = cg.time+1000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_CLOAK:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Cloak;
        //sFlash->screenFlashTime = cg.predictedPlayerState.powerups = PW_CLOAK;
        break;
    case SF_RAGE:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Rage;
        cg.screenFlash.screenFlashTime = cg.time+20000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_BURNED:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Burn;
        cg.screenFlash.screenFlashTime = cg.time+6000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_PAIN:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Pain;
        cg.screenFlash.screenFlashTime = cg.time+1000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_TRACKING:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Tracking;
        cg.screenFlash.screenFlashTime = cg.time+300000;
        cg.screenFlash.screenFlashOff	= qfalse;
        break;
    case SF_FROZEN:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Frozen;
        cg.screenFlash.screenFlashTime = cg.time+5000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_SHOCKED:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Shocked;
        cg.screenFlash.screenFlashTime = cg.time+4000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_VIRUS:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Virus;
        cg.screenFlash.screenFlashTime = cg.time+20000;
        cg.screenFlash.screenFlashOff	= qfalse;
        break;
    case SF_POISON:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Poison;
        cg.screenFlash.screenFlashTime = cg.time+6000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_FLASH:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Flash;
        cg.screenFlash.screenFlashTime = cg.time+1000;
        cg.screenFlash.screenFlashOff	= qtrue;
        break;
    case SF_GRAVITY:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Gravity;
        cg.screenFlash.screenFlashTime = cg.time+6000;
        cg.screenFlash.screenFlashOff	= cg.time+qtrue;
        break;
    case SF_SENSE:
        cg.screenFlash.screenFlashTime = cg.time+40000;
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Sense;
        cg.screenFlash.screenFlashOff	= qfalse;
        break;
    case SF_DMG:
        cg.screenFlash.screenFlashShader = cgs.media.ScreenFlashFX_Dmg;
        cg.screenFlash.screenFlashTime = cg.time+cg.predictedPlayerState.externalEventTime;
        cg.screenFlash.screenFlashOff	= qfalse;
        break;
    case SF_NONE:
        break;
    default:
        break;
    }
}

/*
=================
CG_Draw2D
=================
*/
static void CG_Draw2D( void )
{

    if (cgs.orderPending && cg.time > cgs.orderTime)
    {
        CG_CheckOrderPending();
    }

    // if we are taking a levelshot for the menu, don't draw anything
    if ( cg.levelShot )
    {
        return;
    }

    if ( cg_draw2D.integer == 0 )
    {
        return;
    }

    if ( cg.snap->ps.pm_type == PM_INTERMISSION )
    {
        CG_DrawIntermission();
        return;
    }

    /*
    	if (cg.cameraMode) {
    		return;
    	}
    	*/

    // Peter: Draw 1st person weapon model
    CG_DrawWeapon();

    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
    {
        CG_DrawSpectator();
        CG_DrawCrosshair( NULL, 0 );

        if(!(HudGroupFlag & XHAIRNAME))
            CG_DrawCrosshairNames();
    }
    else
    {
        // don't draw any status if dead or the scoreboard is being explicitly shown
        if ( !cg.showScores && cg.snap->ps.stats[STAT_HEALTH] > 0 )
        {

            if ( cg_drawStatus.integer )
            {
                Menu_PaintAll();
                CG_DrawTimedMenus();
            }

            CG_DrawRadar();
            CG_DrawSense();

            CG_DrawAmmoWarning();

            if(!(HudGroupFlag & XHAIRNAME))
                CG_DrawCrosshairNames();

            if(!(HudGroupFlag & XWEAPONSELECT))
                CG_DrawWeaponSelect();

            CG_DrawReward();
        }
    }

    CG_DrawVote();
    CG_DrawTeamVote();

    if(!(HudGroupFlag & UPPERIGHT))
        CG_DrawLagometer();

    if (!cg_paused.integer)
    {
        if(!(HudGroupFlag & UPPERIGHT))
            CG_DrawUpperRight();
    }

    if ( !CG_DrawFollow() )
    {
        CG_DrawWarmup();
    }

    // don't draw center string if scoreboard is up
    cg.scoreBoardShowing = CG_DrawScoreboard();
    if ( !cg.scoreBoardShowing)
    {
        if(!(HudGroupFlag & XCENTERPRINT))
            CG_DrawCenterString();
    }

    // DonX - FLASH SCREEN
    CG_DrawScreenFlash ();
}


/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void CG_DrawActive( stereoFrame_t stereoView )
{
    float		separation;
    vec3_t		baseOrg;

    // optionally draw the info screen instead
    if ( !cg.snap )
    {
        menuDef_t *loading = Menus_FindByName( "Loading" );

        if(loading==NULL)
            CG_DrawInformation();

        return;
    }

    switch ( stereoView )
    {
    case STEREO_CENTER:
        separation = 0;
        break;
    case STEREO_LEFT:
        separation = -cg_stereoSeparation.value / 2;
        break;
    case STEREO_RIGHT:
        separation = cg_stereoSeparation.value / 2;
        break;
    default:
        separation = 0;
        CG_Error( "CG_DrawActive: Undefined stereoView" );
    }


    // clear around the rendered view if sized down
    CG_TileClear();

    // offset vieworg appropriately if we're doing stereo separation
    VectorCopy( cg.refdef.vieworg, baseOrg );
    if ( separation != 0 )
    {
        VectorMA( cg.refdef.vieworg, -separation, cg.refdef.viewaxis[1], cg.refdef.vieworg );
    }

    CG_Earthquake();

    // draw 3D view
    trap_R_RenderScene( &cg.refdef );

    // restore original viewpoint if running stereo
    if ( separation != 0 )
    {
        VectorCopy( baseOrg, cg.refdef.vieworg );
    }

    // draw status bar and other floating elements
    CG_Draw2D();

    if (black_bars)
        CG_BlackBars();
}

/*
======================
CG_UpdateMediaFraction
======================
*/
void CG_UpdateMediaFraction( float newFract )
{
    cg.mediaFraction = newFract;
    trap_UpdateScreen( );
}
/*
======================
CG_UpdateSoundFraction
======================
*/
void CG_UpdateSoundFraction( float newFract )
{
    cg.soundFraction = newFract;
    trap_UpdateScreen( );
}
/*
======================
CG_UpdateGraphicFraction
======================
*/
void CG_UpdateGraphicFraction( float newFract )
{
    cg.graphicFraction = newFract;
    trap_UpdateScreen( );
}
/*
====================
CG_DrawLoadingScreen
====================
*/
void CG_DrawLoadingScreen( void )
{
    menuDef_t *loading = Menus_FindByName( "Loading" );
    if(loading!=NULL)
    {
        Menu_Paint( loading, qtrue );
    }
    else
    {
        CG_DrawInformation();
    }
}

void CG_InitTeamChat()
{
    memset(teamChat1, 0, sizeof(teamChat1));
    memset(teamChat2, 0, sizeof(teamChat2));
    memset(systemChat, 0, sizeof(systemChat));
}

void CG_SetPrintString(int type, const char *p)
{
    if (type == SYSTEM_PRINT)
    {
        strcpy(systemChat, p);
    }
    else
    {
        strcpy(teamChat2, teamChat1);
        strcpy(teamChat1, p);
    }
}

void CG_CheckOrderPending()
{
    if (cgs.gametype < GT_CTF)
    {
        return;
    }
    if (cgs.orderPending)
    {
        //clientInfo_t *ci = cgs.clientinfo + sortedTeamPlayers[cg_currentSelectedPlayer.integer];
        const char *p1, *p2, *b;
        p1 = p2 = b = NULL;
        switch (cgs.currentOrder)
        {
        case TEAMTASK_OFFENSE:
            b = "+button7; wait; -button7";
            break;
        case TEAMTASK_DEFENSE:
            b = "+button8; wait; -button8";
            break;
        case TEAMTASK_PATROL:
            b = "+button9; wait; -button9";
            break;
        case TEAMTASK_FOLLOW:
            b = "+button10; wait; -button10";
            break;
        }

        if (b)
        {
            trap_SendConsoleCommand(b);
        }
        cgs.orderPending = qfalse;
    }
}

static void CG_SetSelectedPlayerName(void)
{
    if (cg_currentSelectedPlayer.integer >= 0 && cg_currentSelectedPlayer.integer < numSortedTeamPlayers)
    {
        clientInfo_t *ci = cgs.clientinfo + sortedTeamPlayers[cg_currentSelectedPlayer.integer];
        if (ci)
        {
            trap_Cvar_Set("cg_selectedPlayerName", ci->name);
            trap_Cvar_Set("cg_selectedPlayer", va("%d", sortedTeamPlayers[cg_currentSelectedPlayer.integer]));
            cgs.currentOrder = ci->teamTask;
        }
    }
    else
    {
        trap_Cvar_Set("cg_selectedPlayerName", "Everyone");
    }
}
int CG_GetSelectedPlayer(void)
{
    if (cg_currentSelectedPlayer.integer < 0 || cg_currentSelectedPlayer.integer >= numSortedTeamPlayers)
    {
        cg_currentSelectedPlayer.integer = 0;
    }
    return cg_currentSelectedPlayer.integer;
}

void CG_SelectNextPlayer(void)
{
    CG_CheckOrderPending();
    cg_currentSelectedPlayer.integer +=1;
    if (cg_currentSelectedPlayer.integer > numSortedTeamPlayers)
        cg_currentSelectedPlayer.integer = 0;
    CG_SetSelectedPlayerName();
}

void CG_SelectPrevPlayer(void)
{
    CG_CheckOrderPending();
    cg_currentSelectedPlayer.integer -=1;
    if (cg_currentSelectedPlayer.integer < 0)
        cg_currentSelectedPlayer.integer = numSortedTeamPlayers;
    CG_SetSelectedPlayerName();
}


static void CG_DrawPlayerArmorIcon( rectDef_t *rect, qboolean draw2D )
{
    centity_t	*cent;
    playerState_t	*ps;
    vec3_t		angles;
    vec3_t		origin;

    if ( cg_drawStatus.integer == 0 )
    {
        return;
    }

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;

    if ( draw2D || ( !cg_draw3dIcons.integer && cg_drawIcons.integer) )   // bk001206 - parentheses
    {
        CG_DrawPic( rect->x, rect->y + rect->h/2 + 1, rect->w, rect->h, cgs.media.armorIcon );
    }
    else if (cg_draw3dIcons.integer)
    {
        VectorClear( angles );
        origin[0] = 90;
        origin[1] = 0;
        origin[2] = -10;
        angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0;

        CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cgs.media.armorModel, 0, origin, angles );
    }

}

static void CG_DrawPlayerArmorValue(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle, int align)
{
    char	num[16];
    int value, tx;
    centity_t	*cent;
    playerState_t	*ps;

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;

    value = ps->stats[STAT_ARMOR];


    if (shader)
    {
        trap_R_SetColor( color );
        CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
        trap_R_SetColor( NULL );
    }
    else
    {
        Com_sprintf (num, sizeof(num), "%i", value);
        value = CG_Text_Width(num, scale, 0);
        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x;
            break;
        case ITEM_ALIGN_RIGHT:
            tx = rect->x + rect->w - value;
            break;
        case ITEM_ALIGN_CENTER:
            tx = rect->x + ( rect->w / 2.0f ) - ( value / 2.0f );
            break;
        default:
            tx = rect->x;
            break;
        }
        CG_Text_Paint(tx, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
    }
}


static void CG_DrawPlayerAmmoBar( rectDef_t *rect )
{
    centity_t	*cent;
    playerState_t	*ps;
    int weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[cg.predictedPlayerState.weapon].weapon;
    int width;

    if (!weap)
        return;

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;

    if ( bg_itemlist[weap].giTag == WP_XBUSTER ||
            bg_itemlist[weap].giTag == WP_FBUSTER ||
            bg_itemlist[weap].giTag == WP_MBUSTER ||
            bg_itemlist[weap].giTag == WP_PBUSTER ||
            bg_itemlist[weap].giTag == WP_ZBUSTER )
        width = rect->w;
    else
        width = ps->ammo[cent->currentState.weapon];

    if (width > rect->w)
        width = rect->w;

    CG_DrawPic( rect->x, rect->y, width, rect->h, trap_R_RegisterShader(va("ui/xbuster/hud/eseg1_%s", bg_itemlist[weap].wtype_name)) );
}

static void CG_DrawPlayerAmmoIcon( rectDef_t *rect, qboolean draw2D )
{
    centity_t	*cent;
    playerState_t	*ps;
    vec3_t		angles;
    vec3_t		origin;
    int weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[cg.predictedPlayerState.weapon].weapon;

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;

    if ( draw2D || (!cg_draw3dIcons.integer && cg_drawIcons.integer) )   // bk001206 - parentheses
    {
        qhandle_t	icon;

        icon = cg_weapons[ weap ].ammoIcon;
        if ( icon )
        {
            CG_DrawPic( rect->x, rect->y, rect->w, rect->h, icon );
        }

        weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[cent->currentState.weapon].weapon;
    }
    else if (cg_draw3dIcons.integer)
    {
        if ( cent->currentState.weapon && cg_weapons[ cent->currentState.weapon ].ammoModel )
        {
            VectorClear( angles );
            origin[0] = 70;
            origin[1] = 0;
            origin[2] = 0;
            angles[YAW] = 90 + 20 * sin( cg.time / 1000.0 );
            CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cg_weapons[ cent->currentState.weapon ].ammoModel, 0, origin, angles );
        }
    }
}

static void CG_DrawPlayerAmmoValue(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle, int align)
{
    char	num[16];
    int 	value, tx;
    centity_t	*cent;
    playerState_t	*ps;

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;

    if ( cent->currentState.weapon )
    {
        value = ps->ammo[cent->currentState.weapon];
        if ( value > -1 )
        {
            if (shader)
            {
                trap_R_SetColor( color );
                CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
                trap_R_SetColor( NULL );
            }
            else
            {
                Com_sprintf (num, sizeof(num), "%i", value);
                value = CG_Text_Width(num, scale, 0);

                switch( align )
                {
                case ITEM_ALIGN_LEFT:
                    tx = rect->x;
                    break;
                case ITEM_ALIGN_RIGHT:
                    tx = rect->x + rect->w - value;
                    break;
                case ITEM_ALIGN_CENTER:
                    tx = rect->x + ( rect->w / 2.0f ) - ( value / 2.0f );
                    break;
                default:
                    tx = rect->x;
                    break;
                }
                CG_Text_Paint(tx, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
            }
        }
    }

}



static void CG_DrawPlayerHead(rectDef_t *rect, qboolean draw2D)
{
    vec3_t		angles;
    float		size, stretch;
    float		frac;
    float		x = rect->x;

    VectorClear( angles );

    if ( cg.damageTime && cg.time - cg.damageTime < DAMAGE_TIME )
    {
        frac = (float)(cg.time - cg.damageTime ) / DAMAGE_TIME;
        size = rect->w * 1.25 * ( 1.5 - frac * 0.5 );

        stretch = size - rect->w * 1.25;
        // kick in the direction of damage
        x -= stretch * 0.5 + cg.damageX * stretch * 0.5;

        cg.headStartYaw = 180 + cg.damageX * 45;

        cg.headEndYaw = 180 + 20 * cos( crandom()*M_PI );
        cg.headEndPitch = 5 * cos( crandom()*M_PI );

        cg.headStartTime = cg.time;
        cg.headEndTime = cg.time + 100 + random() * 2000;
    }
    else
    {
        if ( cg.time >= cg.headEndTime )
        {
            // select a new head angle
            cg.headStartYaw = cg.headEndYaw;
            cg.headStartPitch = cg.headEndPitch;
            cg.headStartTime = cg.headEndTime;
            cg.headEndTime = cg.time + 100 + random() * 2000;

            cg.headEndYaw = 180 + 20 * cos( crandom()*M_PI );
            cg.headEndPitch = 5 * cos( crandom()*M_PI );
        }

        size = rect->w * 1.25;
    }

    // if the server was frozen for a while we may have a bad head start time
    if ( cg.headStartTime > cg.time )
    {
        cg.headStartTime = cg.time;
    }

    frac = ( cg.time - cg.headStartTime ) / (float)( cg.headEndTime - cg.headStartTime );
    frac = frac * frac * ( 3 - 2 * frac );
    angles[YAW] = cg.headStartYaw + ( cg.headEndYaw - cg.headStartYaw ) * frac;
    angles[PITCH] = cg.headStartPitch + ( cg.headEndPitch - cg.headStartPitch ) * frac;

    CG_DrawHead( x, rect->y, rect->w, rect->h, cg.snap->ps.clientNum, angles );
}

static void CG_DrawSelectedPlayerHealth( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    clientInfo_t *ci;
    int value;
    char num[16];

    ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
    if (ci)
    {
        if (shader)
        {
            trap_R_SetColor( color );
            CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
            trap_R_SetColor( NULL );
        }
        else
        {
            Com_sprintf (num, sizeof(num), "%i", ci->health);
            value = CG_Text_Width(num, scale, 0);
            CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
        }
    }
}

static void CG_DrawSelectedPlayerArmor( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    clientInfo_t *ci;
    int value;
    char num[16];
    ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
    if (ci)
    {
        if (ci->armor > 0)
        {
            if (shader)
            {
                trap_R_SetColor( color );
                CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
                trap_R_SetColor( NULL );
            }
            else
            {
                Com_sprintf (num, sizeof(num), "%i", ci->armor);
                value = CG_Text_Width(num, scale, 0);
                CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
            }
        }
    }
}

qhandle_t CG_StatusHandle(int task)
{
    qhandle_t h = cgs.media.assaultShader;
    switch (task)
    {
    case TEAMTASK_OFFENSE :
        h = cgs.media.assaultShader;
        break;
    case TEAMTASK_DEFENSE :
        h = cgs.media.defendShader;
        break;
    case TEAMTASK_PATROL :
        h = cgs.media.patrolShader;
        break;
    case TEAMTASK_FOLLOW :
        h = cgs.media.followShader;
        break;
    case TEAMTASK_CAMP :
        h = cgs.media.campShader;
        break;
    case TEAMTASK_RETRIEVE :
        h = cgs.media.retrieveShader;
        break;
    case TEAMTASK_ESCORT :
        h = cgs.media.escortShader;
        break;
    default :
        h = cgs.media.assaultShader;
        break;
    }
    return h;
}

static void CG_DrawSelectedPlayerStatus( rectDef_t *rect )
{
    clientInfo_t *ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
    if (ci)
    {
        qhandle_t h;
        if (cgs.orderPending)
        {
            // blink the icon
            if ( cg.time > cgs.orderTime - 2500 && (cg.time >> 9 ) & 1 )
            {
                return;
            }
            h = CG_StatusHandle(cgs.currentOrder);
        }
        else
        {
            h = CG_StatusHandle(ci->teamTask);
        }
        CG_DrawPic( rect->x, rect->y, rect->w, rect->h, h );
    }
}


static void CG_DrawPlayerStatus( rectDef_t *rect )
{
    clientInfo_t *ci = &cgs.clientinfo[cg.snap->ps.clientNum];
    if (ci)
    {
        qhandle_t h = CG_StatusHandle(ci->teamTask);
        CG_DrawPic( rect->x, rect->y, rect->w, rect->h, h);
    }
}


static void CG_DrawSelectedPlayerName( rectDef_t *rect, float scale, vec4_t color, qboolean voice, int textStyle)
{
    clientInfo_t *ci;
    ci = cgs.clientinfo + ((voice) ? cgs.currentVoiceClient : sortedTeamPlayers[CG_GetSelectedPlayer()]);
    if (ci)
    {
        CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, ci->name, 0, 0, textStyle);
    }
}

static void CG_DrawSelectedPlayerLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    clientInfo_t *ci;
    ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
    if (ci)
    {
        const char *p = CG_ConfigString(CS_LOCATIONS + ci->location);
        if (!p || !*p)
        {
            p = "unknown";
        }
        CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, p, 0, 0, textStyle);
    }
}

static void CG_DrawPlayerLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle  )
{
    clientInfo_t *ci = &cgs.clientinfo[cg.snap->ps.clientNum];
    if (ci)
    {
        const char *p = CG_ConfigString(CS_LOCATIONS + ci->location);
        if (!p || !*p)
        {
            p = "unknown";
        }
        CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, p, 0, 0, textStyle);
    }
}



static void CG_DrawSelectedPlayerWeapon( rectDef_t *rect )
{
    clientInfo_t *ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
    int weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[ci->curWeapon].weapon;

    if (ci)
    {
        if ( cg_weapons[ weap ].weaponIcon )
        {
            CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_weapons[ weap ].weaponIcon );
        }
        else
        {
            CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.deferShader);
        }
    }
}

static void CG_DrawPlayerScore( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    char num[16];
    int value = cg.snap->ps.persistant[PERS_SCORE];

    if (shader)
    {
        trap_R_SetColor( color );
        CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
        trap_R_SetColor( NULL );
    }
    else
    {
        Com_sprintf (num, sizeof(num), "%i", value);
        value = CG_Text_Width(num, scale, 0);
        CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
    }
}

static void CG_DrawPlayerItem( rectDef_t *rect, float scale, qboolean draw2D)
{
    int		value;
    vec3_t origin, angles;

    value = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];
    if ( value )
    {
        CG_RegisterItemVisuals( value );

        if (qtrue)
        {
            CG_RegisterItemVisuals( value );
            CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].icon );
        }
        else
        {
            VectorClear( angles );
            origin[0] = 90;
            origin[1] = 0;
            origin[2] = -10;
            angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0;
            CG_Draw3DModel(rect->x, rect->y, rect->w, rect->h, cg_items[ value ].models[0], 0, origin, angles );
        }
    }

}


static void CG_DrawSelectedPlayerPowerup( rectDef_t *rect, qboolean draw2D )
{
    clientInfo_t *ci;
    int j;
    float x, y;

    ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
    if (ci)
    {
        x = rect->x;
        y = rect->y;

        for (j = 0; j < PW_NUM_POWERUPS; j++)
        {
            if (ci->powerups & (1 << j))
            {
                gitem_t	*item;
                item = BG_FindItemForPowerup( j );
                if (item)
                {
                    CG_DrawPic( x, y, rect->w, rect->h, trap_R_RegisterShader( item->icon ) );
                    x += 3;
                    y += 3;
                    return;
                }
            }
        }

    }
}


static void CG_DrawSelectedPlayerHead( rectDef_t *rect, qboolean draw2D, qboolean voice )
{
    clipHandle_t	cm;
    clientInfo_t	*ci;
    float			len;
    vec3_t			origin;
    vec3_t			mins, maxs, angles;


    ci = cgs.clientinfo + ((voice) ? cgs.currentVoiceClient : sortedTeamPlayers[CG_GetSelectedPlayer()]);

    if (ci)
    {
        if ( cg_draw3dIcons.integer )
        {
            cm = ci->headModel;
            if ( !cm )
            {
                return;
            }

            // offset the origin y and z to center the head
            trap_R_ModelBounds( cm, mins, maxs );

            origin[2] = -0.5 * ( mins[2] + maxs[2] );
            origin[1] = 0.5 * ( mins[1] + maxs[1] );

            // calculate distance so the head nearly fills the box
            // assume heads are taller than wide
            len = 0.7 * ( maxs[2] - mins[2] );
            origin[0] = len / 0.268;	// len / tan( fov/2 )

            // allow per-model tweaking
            VectorAdd( origin, ci->headOffset, origin );

            angles[PITCH] = 0;
            angles[YAW] = 180;
            angles[ROLL] = 0;

            CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, ci->headModel, ci->headSkin, origin, angles );
        }
        else if ( cg_drawIcons.integer )
        {
            CG_DrawPic( rect->x, rect->y, rect->w, rect->h, ci->modelIcon );
        }

        // if they are deferred, draw a cross out
        if ( ci->deferred )
        {
            CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.deferShader );
        }
    }

}


static void CG_DrawPlayerHealth(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle, int align )
{
    playerState_t	*ps;
    int value, tx;
    char	num[16];

    ps = &cg.snap->ps;

    value = ps->stats[STAT_HEALTH];

    if (shader)
    {
        trap_R_SetColor( color );
        CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
        trap_R_SetColor( NULL );
    }
    else
    {
        Com_sprintf (num, sizeof(num), "%i", value);
        value = CG_Text_Width(num, scale, 0);

        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x;
            break;
        case ITEM_ALIGN_RIGHT:
            tx = rect->x + rect->w - value;
            break;
        case ITEM_ALIGN_CENTER:
            tx = rect->x + ( rect->w / 2.0f ) - ( value / 2.0f );
            break;
        default:
            tx = rect->x;
            break;
        }
        CG_Text_Paint(tx, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
    }
}

static void CG_DrawPlayerHealthBar( rectDef_t *rect )
{
    centity_t	*cent;
    playerState_t	*ps;
    int i, x;

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;
    x = rect->x;

    for (i = 1; i < ps->stats[STAT_HEALTH]*rect->w/ps->stats[STAT_MAX_HEALTH] && i < 41; i++)
    {
        CG_DrawPic( x, rect->y, 8, rect->h, trap_R_RegisterShader("ui/xbuster/hud/hseg1_blue") );
        x += 8;
    }
}

static void CG_DrawPlayerSubtank(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle, int align )
{
    playerState_t	*ps;
    int value, tx;
    char	num[16];

    ps = &cg.snap->ps;

    value = ps->persistant[PERS_SUBTANKS];

    if (shader)
    {
        trap_R_SetColor( color );
        CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
        trap_R_SetColor( NULL );
    }
    else
    {
        Com_sprintf (num, sizeof(num), "%i", value);
        value = CG_Text_Width(num, scale, 0);

        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x;
            break;
        case ITEM_ALIGN_RIGHT:
            tx = rect->x + rect->w - value;
            break;
        case ITEM_ALIGN_CENTER:
            tx = rect->x + ( rect->w / 2.0f ) - ( value / 2.0f );
            break;
        default:
            tx = rect->x;
            break;
        }
        CG_Text_Paint(tx, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
    }
}

static void CG_DrawPlayerSubtankBar( rectDef_t *rect )
{
    centity_t	*cent;
    playerState_t	*ps;
    int i, x;

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;
    x = rect->x;

    for (i = 1; i < ps->persistant[PERS_SUBTANKS]*rect->w/ps->stats[STAT_MAX_HEALTH] && i < 41; i++)
    {
        CG_DrawPic( x, rect->y, 8, rect->h, trap_R_RegisterShader("ui/xbuster/hud/hseg1_blue") );
        x += 8;
    }
}

static void CG_DrawPlayerChips(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle, int align )
{
    playerState_t	*ps;
    int value, tx;
    char	num[16];

    ps = &cg.snap->ps;

    value = ps->persistant[PERS_CHIPS];

    if (shader)
    {
        trap_R_SetColor( color );
        CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
        trap_R_SetColor( NULL );
    }
    else
    {
        Com_sprintf (num, sizeof(num), "%i", value);
        value = CG_Text_Width(num, scale, 0);

        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x;
            break;
        case ITEM_ALIGN_RIGHT:
            tx = rect->x + rect->w - value;
            break;
        case ITEM_ALIGN_CENTER:
            tx = rect->x + ( rect->w / 2.0f ) - ( value / 2.0f );
            break;
        default:
            tx = rect->x;
            break;
        }
        CG_Text_Paint(tx, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
    }
}

static void CG_DrawRedScore(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    int value;
    char num[16];
    if ( cgs.scores1 == SCORE_NOT_PRESENT )
    {
        Com_sprintf (num, sizeof(num), "-");
    }
    else
    {
        Com_sprintf (num, sizeof(num), "%i", cgs.scores1);
    }
    value = CG_Text_Width(num, scale, 0);
    CG_Text_Paint(rect->x + rect->w - value, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
}

static void CG_DrawBlueScore(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    int value;
    char num[16];

    if ( cgs.scores2 == SCORE_NOT_PRESENT )
    {
        Com_sprintf (num, sizeof(num), "-");
    }
    else
    {
        Com_sprintf (num, sizeof(num), "%i", cgs.scores2);
    }
    value = CG_Text_Width(num, scale, 0);
    CG_Text_Paint(rect->x + rect->w - value, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
}

// FIXME: team name support
static void CG_DrawRedName(rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cg_redTeamName.string , 0, 0, textStyle);
}

static void CG_DrawBlueName(rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cg_blueTeamName.string, 0, 0, textStyle);
}

static void CG_DrawBlueFlagName(rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    int i;
    for ( i = 0 ; i < cgs.maxclients ; i++ )
    {
        if ( cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_RED  && cgs.clientinfo[i].powerups & ( 1<< PW_BLUEFLAG ))
        {
            CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cgs.clientinfo[i].name, 0, 0, textStyle);
            return;
        }
    }
}

static void CG_DrawBlueFlagStatus(rectDef_t *rect, qhandle_t shader)
{
    if (cgs.gametype != GT_CTF)
        return;

    if (shader)
    {
        CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
    }
    else
    {
        gitem_t *item = BG_FindItemForPowerup( PW_BLUEFLAG );
        if (item)
        {
            vec4_t color = {0, 0, 1, 1};
            trap_R_SetColor(color);
            if( cgs.blueflag >= 0 && cgs.blueflag <= 2 )
            {
                CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[cgs.blueflag] );
            }
            else
            {
                CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[0] );
            }
            trap_R_SetColor(NULL);
        }
    }
}

static void CG_DrawBlueFlagHead(rectDef_t *rect)
{
    int i;
    for ( i = 0 ; i < cgs.maxclients ; i++ )
    {
        if ( cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_RED  && cgs.clientinfo[i].powerups & ( 1<< PW_BLUEFLAG ))
        {
            vec3_t angles;
            VectorClear( angles );
            angles[YAW] = 180 + 20 * sin( cg.time / 650.0 );;
            CG_DrawHead( rect->x, rect->y, rect->w, rect->h, 0,angles );
            return;
        }
    }
}

static void CG_DrawRedFlagName(rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    int i;
    for ( i = 0 ; i < cgs.maxclients ; i++ )
    {
        if ( cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_BLUE  && cgs.clientinfo[i].powerups & ( 1<< PW_REDFLAG ))
        {
            CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cgs.clientinfo[i].name, 0, 0, textStyle);
            return;
        }
    }
}

static void CG_DrawRedFlagStatus(rectDef_t *rect, qhandle_t shader)
{
    if (cgs.gametype != GT_CTF)
        return;

    if (shader)
    {
        CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
    }
    else
    {
        gitem_t *item = BG_FindItemForPowerup( PW_REDFLAG );
        if (item)
        {
            vec4_t color = {1, 0, 0, 1};
            trap_R_SetColor(color);
            if( cgs.redflag >= 0 && cgs.redflag <= 2)
            {
                CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[cgs.redflag] );
            }
            else
            {
                CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[0] );
            }
            trap_R_SetColor(NULL);
        }
    }
}

static void CG_DrawRedFlagHead(rectDef_t *rect)
{
    int i;
    for ( i = 0 ; i < cgs.maxclients ; i++ )
    {
        if ( cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_BLUE  && cgs.clientinfo[i].powerups & ( 1<< PW_REDFLAG ))
        {
            vec3_t angles;
            VectorClear( angles );
            angles[YAW] = 180 + 20 * sin( cg.time / 650.0 );;
            CG_DrawHead( rect->x, rect->y, rect->w, rect->h, 0,angles );
            return;
        }
    }
}


static void CG_DrawTeamColor(rectDef_t *rect, vec4_t color)
{
    CG_DrawTeamBackground(rect->x, rect->y, rect->w, rect->h, color[3], cg.snap->ps.persistant[PERS_TEAM]);
}

static void CG_DrawAreaPowerUp(rectDef_t *rect, int align, float special, float scale, vec4_t color)
{
    char num[16];
    int		sorted[MAX_POWERUPS];
    int		sortedTime[MAX_POWERUPS];
    int		i, j, k, t, w;
    int		active;
    playerState_t	*ps;
    gitem_t	*item;
    float	f;
    rectDef_t r2;
    float *inc;
    r2.x = rect->x;
    r2.y = rect->y;
    r2.w = rect->w;
    r2.h = rect->h;

    inc = (align == HUD_VERTICAL) ? &r2.y : &r2.x;

    ps = &cg.snap->ps;

    if ( ps->stats[STAT_HEALTH] <= 0 )
    {
        return;
    }

    // sort the list by time remaining
    active = 0;
    for ( i = 0 ; i < MAX_POWERUPS ; i++ )
    {
        if ( !ps->powerups[ i ] )
        {
            continue;
        }
        t = ps->powerups[ i ] - cg.time;
        // ZOID--don't draw if the power up has unlimited time (999 seconds)
        // This is true of the CTF flags
        if ( t <= 0 || t >= 999000)
        {
            continue;
        }

        // insert into the list
        for ( j = 0 ; j < active ; j++ )
        {
            if ( sortedTime[j] >= t )
            {
                for ( k = active - 1 ; k >= j ; k-- )
                {
                    sorted[k+1] = sorted[k];
                    sortedTime[k+1] = sortedTime[k];
                }
                break;
            }
        }
        sorted[j] = i;
        sortedTime[j] = t;
        active++;
    }

    // draw the icons and timers
    for ( i = 0 ; i < active ; i++ )
    {
        item = BG_FindItemForPowerup( sorted[i] );

        if (item)
        {
            t = ps->powerups[ sorted[i] ];
            if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME )
            {
                trap_R_SetColor( NULL );
            }
            else
            {
                vec4_t	modulate;

                f = (float)( t - cg.time ) / POWERUP_BLINK_TIME;
                f -= (int)f;
                modulate[0] = modulate[1] = modulate[2] = modulate[3] = f;
                trap_R_SetColor( modulate );
            }

            CG_DrawPic( r2.x, r2.y, r2.w * .75, r2.h, trap_R_RegisterShader( item->icon ) );

            Com_sprintf (num, sizeof(num), "%i", sortedTime[i] / 1000);
            w = ((r2.w * .75) - CG_Text_Width( num, scale, 0 ))/2;
            CG_Text_Paint(r2.x + w + 1, r2.y + r2.h, scale, color, num, 0, 0, 0);
            *inc += r2.w + special;
        }

    }
    trap_R_SetColor( NULL );

}

float CG_GetValue(int ownerDraw)
{
    centity_t	*cent;
    clientInfo_t *ci;
    playerState_t	*ps;

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;

    switch (ownerDraw)
    {
    case CG_SELECTEDPLAYER_ARMOR:
        ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
        return ci->armor;
        break;
    case CG_SELECTEDPLAYER_HEALTH:
        ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
        return ci->health;
        break;
    case CG_PLAYER_ARMOR_VALUE:
        return ps->stats[STAT_ARMOR];
        break;
    case CG_PLAYER_AMMO_VALUE:
        if ( cent->currentState.weapon )
        {
            return ps->ammo[cent->currentState.weapon];
        }
        break;
    case CG_PLAYER_SCORE:
        return cg.snap->ps.persistant[PERS_SCORE];
        break;
    case CG_PLAYER_HEALTH:
        return ps->stats[STAT_HEALTH];
        break;
    case CG_RED_SCORE:
        return cgs.scores1;
        break;
    case CG_BLUE_SCORE:
        return cgs.scores2;
        break;
    default:
        break;
    }
    return -1;
}

qboolean CG_OtherTeamHasFlag()
{
    if (cgs.gametype == GT_CTF)
    {
        int team = cg.snap->ps.persistant[PERS_TEAM];

        if (team == TEAM_RED && cgs.redflag == FLAG_TAKEN)
            return qtrue;
        else if (team == TEAM_BLUE && cgs.blueflag == FLAG_TAKEN)
            return qtrue;
        else
            return qfalse;
    }

    return qfalse;
}

qboolean CG_YourTeamHasFlag()
{
    if (cgs.gametype == GT_CTF)
    {
        int team = cg.snap->ps.persistant[PERS_TEAM];
        if (team == TEAM_RED && cgs.blueflag == FLAG_TAKEN)
        {
            return qtrue;
        }
        else if (team == TEAM_BLUE && cgs.redflag == FLAG_TAKEN)
        {
            return qtrue;
        }
        else
        {
            return qfalse;
        }
    }
    return qfalse;
}

// THINKABOUTME: should these be exclusive or inclusive..
//
qboolean CG_OwnerDrawVisible(int flags)
{

    if (flags & CG_SHOW_TEAMINFO)
    {
        return (cg_currentSelectedPlayer.integer == numSortedTeamPlayers);
    }

    if (flags & CG_SHOW_NOTEAMINFO)
    {
        return !(cg_currentSelectedPlayer.integer == numSortedTeamPlayers);
    }

    if (flags & CG_SHOW_OTHERTEAMHASFLAG)
    {
        return CG_OtherTeamHasFlag();
    }

    if (flags & CG_SHOW_YOURTEAMHASENEMYFLAG)
    {
        return CG_YourTeamHasFlag();
    }

    if (flags & (CG_SHOW_BLUE_TEAM_HAS_REDFLAG | CG_SHOW_RED_TEAM_HAS_BLUEFLAG))
    {
        if (flags & CG_SHOW_BLUE_TEAM_HAS_REDFLAG && (cgs.redflag == FLAG_TAKEN || cgs.flagStatus == FLAG_TAKEN_RED))
        {
            return qtrue;
        }
        else if (flags & CG_SHOW_RED_TEAM_HAS_BLUEFLAG && (cgs.blueflag == FLAG_TAKEN || cgs.flagStatus == FLAG_TAKEN_BLUE))
        {
            return qtrue;
        }
        return qfalse;
    }

    if (flags & CG_SHOW_ANYTEAMGAME)
    {
        if( cgs.gametype >= GT_TEAM)
        {
            return qtrue;
        }
    }

    if (flags & CG_SHOW_ANYNONTEAMGAME)
    {
        if( cgs.gametype < GT_TEAM)
        {
            return qtrue;
        }
    }

    if (flags & CG_SHOW_CTF)
    {
        if( cgs.gametype == GT_CTF )
        {
            return qtrue;
        }
    }


    if (flags & CG_SHOW_HEALTHCRITICAL)
    {
        if (cg.snap->ps.stats[STAT_HEALTH] < 25)
        {
            return qtrue;
        }
    }

    if (flags & CG_SHOW_HEALTHOK)
    {
        if (cg.snap->ps.stats[STAT_HEALTH] >= 25)
        {
            return qtrue;
        }
    }

    if (flags & CG_SHOW_SINGLEPLAYER)
    {
        if( cgs.gametype == GT_SINGLE_PLAYER )
        {
            return qtrue;
        }
    }

    if (flags & CG_SHOW_TOURNAMENT)
    {
        if( cgs.gametype == GT_TOURNAMENT )
        {
            return qtrue;
        }
    }

    if (flags & CG_SHOW_DURINGINCOMINGVOICE)
    {
    }

    if (flags & CG_SHOW_IF_PLAYER_HAS_FLAG)
    {
        if (cg.snap->ps.powerups[PW_REDFLAG] || cg.snap->ps.powerups[PW_BLUEFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG])
        {
            return qtrue;
        }
    }
    return qfalse;
}



static void CG_DrawPlayerHasFlag(rectDef_t *rect, qboolean force2D)
{
    int adj = (force2D) ? 0 : 2;
    if( cg.predictedPlayerState.powerups[PW_REDFLAG] )
    {
        CG_DrawFlagModel( rect->x + adj, rect->y + adj, rect->w - adj, rect->h - adj, TEAM_RED, force2D);
    }
    else if( cg.predictedPlayerState.powerups[PW_BLUEFLAG] )
    {
        CG_DrawFlagModel( rect->x + adj, rect->y + adj, rect->w - adj, rect->h - adj, TEAM_BLUE, force2D);
    }
    else if( cg.predictedPlayerState.powerups[PW_NEUTRALFLAG] )
    {
        CG_DrawFlagModel( rect->x + adj, rect->y + adj, rect->w - adj, rect->h - adj, TEAM_FREE, force2D);
    }
}

static void CG_DrawAreaSystemChat(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader)
{
    CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, systemChat, 0, 0, 0);
}

static void CG_DrawAreaTeamChat(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader)
{
    CG_Text_Paint(rect->x, rect->y + rect->h, scale, color,teamChat1, 0, 0, 0);
}

static void CG_DrawAreaChat(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader)
{
    CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, teamChat2, 0, 0, 0);
}

const char *CG_GetKillerText()
{
    const char *s = "";
    if ( cg.killerName[0] )
    {
        s = va("Fragged by %s", cg.killerName );
    }
    return s;
}


static void CG_DrawKiller(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    // fragged by ... line
    if ( cg.killerName[0] )
    {
        int x = rect->x + rect->w / 2;
        CG_Text_Paint(x - CG_Text_Width(CG_GetKillerText(), scale, 0) / 2, rect->y + rect->h, scale, color, CG_GetKillerText(), 0, 0, textStyle);
    }

}


static void CG_DrawCapFragLimit(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle)
{
    int limit = (cgs.gametype >= GT_CTF) ? cgs.capturelimit : cgs.fraglimit;
    CG_Text_Paint(rect->x, rect->y, scale, color, va("%2i", limit),0, 0, textStyle);
}

static void CG_Draw1stPlace(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle)
{
    if (cgs.scores1 != SCORE_NOT_PRESENT)
    {
        CG_Text_Paint(rect->x, rect->y, scale, color, va("%2i", cgs.scores1),0, 0, textStyle);
    }
}

static void CG_Draw2ndPlace(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle)
{
    if (cgs.scores2 != SCORE_NOT_PRESENT)
    {
        CG_Text_Paint(rect->x, rect->y, scale, color, va("%2i", cgs.scores2),0, 0, textStyle);
    }
}

const char *CG_GetGameStatusText()
{
    const char *s = "";
    if ( cgs.gametype < GT_TEAM)
    {
        if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
        {
            s = va("%s place with %i",CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),cg.snap->ps.persistant[PERS_SCORE] );
        }
    }
    else
    {
        if ( cg.teamScores[0] == cg.teamScores[1] )
        {
            s = va("Teams are tied at %i", cg.teamScores[0] );
        }
        else if ( cg.teamScores[0] >= cg.teamScores[1] )
        {
            s = va("Red leads Blue, %i to %i", cg.teamScores[0], cg.teamScores[1] );
        }
        else
        {
            s = va("Blue leads Red, %i to %i", cg.teamScores[1], cg.teamScores[0] );
        }
    }
    return s;
}

static void CG_DrawGameStatus(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, CG_GetGameStatusText(), 0, 0, textStyle);
}

const char *CG_GameTypeString()
{
    if ( cgs.gametype == GT_FFA )
    {
        return "Free For All";
    }
    else if ( cgs.gametype == GT_TEAM )
    {
        return "Team Deathmatch";
    }
    else if ( cgs.gametype == GT_CTF )
    {
        return "Capture the Flag";
    }
    return "";
}
static void CG_DrawGameType(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle )
{
    CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, CG_GameTypeString(), 0, 0, textStyle);
}

static void CG_Text_Paint_Limit(float *maxX, float x, float y, float scale, vec4_t color, const char* text, float adjust, int limit)
{
    int len, count;
    vec4_t newColor;
    glyphInfo_t *glyph;
    if (text)
    {
// TTimo: FIXME
//    const unsigned char *s = text; // bk001206 - unsigned
        const char *s = text;
        float max = *maxX;
        float useScale;
        fontInfo_t *font = &cgDC.Assets.textFont;
        if (scale <= cg_smallFont.value)
        {
            font = &cgDC.Assets.smallFont;
        }
        else if (scale > cg_bigFont.value)
        {
            font = &cgDC.Assets.bigFont;
        }
        useScale = scale * font->glyphScale;
        trap_R_SetColor( color );
        len = strlen(text);
        if (limit > 0 && len > limit)
        {
            len = limit;
        }
        count = 0;
        while (s && *s && count < len)
        {
            glyph = &font->glyphs[(int)*s]; // TTimo: FIXME: getting nasty warnings without the cast, hopefully this doesn't break the VM build
            if ( Q_IsColorString( s ) )
            {
                memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
                newColor[3] = color[3];
                trap_R_SetColor( newColor );
                s += 2;
                continue;
            }
            else
            {
                float yadj = useScale * glyph->top;
                if (CG_Text_Width(s, useScale, 1) + x > max)
                {
                    *maxX = 0;
                    break;
                }
                CG_Text_PaintChar(x, y - yadj,
                                  glyph->imageWidth,
                                  glyph->imageHeight,
                                  useScale,
                                  glyph->s,
                                  glyph->t,
                                  glyph->s2,
                                  glyph->t2,
                                  glyph->glyph);
                x += (glyph->xSkip * useScale) + adjust;
                *maxX = x;
                count++;
                s++;
            }
        }
        trap_R_SetColor( NULL );
    }

}



#define PIC_WIDTH 12

void CG_DrawNewTeamInfo(rectDef_t *rect, float text_x, float text_y, float scale, vec4_t color, qhandle_t shader)
{
    int xx;
    float y;
    int i, j, len, count;
    const char *p;
    vec4_t		hcolor;
    float pwidth, lwidth, maxx, leftOver;
    clientInfo_t *ci;
    gitem_t	*item;
    qhandle_t h;
    int weap;

    // max player name width
    pwidth = 0;
    count = (numSortedTeamPlayers > 8) ? 8 : numSortedTeamPlayers;
    for (i = 0; i < count; i++)
    {
        ci = cgs.clientinfo + sortedTeamPlayers[i];
        weap =  bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[ci->curWeapon].weapon;

        if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM])
        {
            len = CG_Text_Width( ci->name, scale, 0);
            if (len > pwidth)
                pwidth = len;
        }
    }

    // max location name width
    lwidth = 0;
    for (i = 1; i < MAX_LOCATIONS; i++)
    {
        p = CG_ConfigString(CS_LOCATIONS + i);
        if (p && *p)
        {
            len = CG_Text_Width(p, scale, 0);
            if (len > lwidth)
                lwidth = len;
        }
    }

    y = rect->y;

    for (i = 0; i < count; i++)
    {
        ci = cgs.clientinfo + sortedTeamPlayers[i];
        if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM])
        {

            xx = rect->x + 1;
            for (j = 0; j <= PW_NUM_POWERUPS; j++)
            {
                if (ci->powerups & (1 << j))
                {

                    item = BG_FindItemForPowerup( j );

                    if (item)
                    {
                        CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, trap_R_RegisterShader( item->icon ) );
                        xx += PIC_WIDTH;
                    }
                }
            }

            // FIXME: max of 3 powerups shown properly
            xx = rect->x + (PIC_WIDTH * 3) + 2;

            CG_GetColorForHealth( ci->health, ci->armor, hcolor );
            trap_R_SetColor(hcolor);
            CG_DrawPic( xx, y + 1, PIC_WIDTH - 2, PIC_WIDTH - 2, cgs.media.heartShader );

            //Com_sprintf (st, sizeof(st), "%3i %3i", ci->health,	ci->armor);
            //CG_Text_Paint(xx, y + text_y, scale, hcolor, st, 0, 0);

            // draw weapon icon
            xx += PIC_WIDTH + 1;

// weapon used is not that useful, use the space for task
#if 0
            if ( cg_weapons[ci->curWeapon].weaponIcon )
            {
                CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, cg_weapons[ci->curWeapon].weaponIcon );
            }
            else
            {
                CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, cgs.media.deferShader );
            }
#endif

            trap_R_SetColor(NULL);
            if (cgs.orderPending)
            {
                // blink the icon
                if ( cg.time > cgs.orderTime - 2500 && (cg.time >> 9 ) & 1 )
                {
                    h = 0;
                }
                else
                {
                    h = CG_StatusHandle(cgs.currentOrder);
                }
            }
            else
            {
                h = CG_StatusHandle(ci->teamTask);
            }

            if (h)
            {
                CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, h);
            }

            xx += PIC_WIDTH + 1;

            leftOver = rect->w - xx;
            maxx = xx + leftOver / 3;



            CG_Text_Paint_Limit(&maxx, xx, y + text_y, scale, color, ci->name, 0, 0);

            p = CG_ConfigString(CS_LOCATIONS + ci->location);
            if (!p || !*p)
            {
                p = "unknown";
            }

            xx += leftOver / 3 + 2;
            maxx = rect->w - 4;

            CG_Text_Paint_Limit(&maxx, xx, y + text_y, scale, color, p, 0, 0);
            y += text_y + 2;
            if ( y + text_y + 2 > rect->y + rect->h )
            {
                break;
            }

        }
    }
}


void CG_DrawTeamSpectators(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader)
{
    if (cg.spectatorLen)
    {
        float maxX;

        if (cg.spectatorWidth == -1)
        {
            cg.spectatorWidth = 0;
            cg.spectatorPaintX = rect->x + 1;
            cg.spectatorPaintX2 = -1;
        }

        if (cg.spectatorOffset > cg.spectatorLen)
        {
            cg.spectatorOffset = 0;
            cg.spectatorPaintX = rect->x + 1;
            cg.spectatorPaintX2 = -1;
        }

        if (cg.time > cg.spectatorTime)
        {
            cg.spectatorTime = cg.time + 10;
            if (cg.spectatorPaintX <= rect->x + 2)
            {
                if (cg.spectatorOffset < cg.spectatorLen)
                {
                    cg.spectatorPaintX += CG_Text_Width(&cg.spectatorList[cg.spectatorOffset], scale, 1) - 1;
                    cg.spectatorOffset++;
                }
                else
                {
                    cg.spectatorOffset = 0;
                    if (cg.spectatorPaintX2 >= 0)
                    {
                        cg.spectatorPaintX = cg.spectatorPaintX2;
                    }
                    else
                    {
                        cg.spectatorPaintX = rect->x + rect->w - 2;
                    }
                    cg.spectatorPaintX2 = -1;
                }
            }
            else
            {
                cg.spectatorPaintX--;
                if (cg.spectatorPaintX2 >= 0)
                {
                    cg.spectatorPaintX2--;
                }
            }
        }

        maxX = rect->x + rect->w - 2;
        CG_Text_Paint_Limit(&maxX, cg.spectatorPaintX, rect->y + rect->h - 3, scale, color, &cg.spectatorList[cg.spectatorOffset], 0, 0);
        if (cg.spectatorPaintX2 >= 0)
        {
            float maxX2 = rect->x + rect->w - 2;
            CG_Text_Paint_Limit(&maxX2, cg.spectatorPaintX2, rect->y + rect->h - 3, scale, color, cg.spectatorList, 0, cg.spectatorOffset);
        }
        if (cg.spectatorOffset && maxX > 0)
        {
            // if we have an offset ( we are skipping the first part of the string ) and we fit the string
            if (cg.spectatorPaintX2 == -1)
            {
                cg.spectatorPaintX2 = rect->x + rect->w - 2;
            }
        }
        else
        {
            cg.spectatorPaintX2 = -1;
        }

    }
}



void CG_DrawMedal(int ownerDraw, rectDef_t *rect, float scale, vec4_t color, qhandle_t shader)
{
    score_t *score = &cg.scores[cg.selectedScore];
    float value = 0;
    char *text = NULL;
    color[3] = 0.25;

    switch (ownerDraw)
    {
    case CG_ACCURACY:
        value = score->accuracy;
        break;
    case CG_ASSISTS:
        value = score->assistCount;
        break;
    case CG_DEFEND:
        value = score->defendCount;
        break;
    case CG_EXCELLENT:
        value = score->excellentCount;
        break;
    case CG_IMPRESSIVE:
        value = score->impressiveCount;
        break;
    case CG_PERFECT:
        value = score->perfect;
        break;
    case CG_GAUNTLET:
        value = score->guantletCount;
        break;
    case CG_CAPTURES:
        value = score->captures;
        break;
    }

    if (value > 0)
    {
        if (ownerDraw != CG_PERFECT)
        {
            if (ownerDraw == CG_ACCURACY)
            {
                text = va("%i%%", (int)value);
                if (value > 50)
                {
                    color[3] = 1.0;
                }
            }
            else
            {
                text = va("%i", (int)value);
                color[3] = 1.0;
            }
        }
        else
        {
            if (value)
            {
                color[3] = 1.0;
            }
            text = "Wow";
        }
    }

    trap_R_SetColor(color);
    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );

    if (text)
    {
        color[3] = 1.0;
        value = CG_Text_Width(text, scale, 0);
        CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h + 10 , scale, color, text, 0, 0, 0);
    }
    trap_R_SetColor(NULL);

}

// loadingscreen
// code based on Tremulous source code

static void CG_DrawProgressBar( rectDef_t *rect, vec4_t color, float scale,
                                int align, int textStyle, int special, float progress )
{
    float rimWidth = rect->h / 20.0f;
    float doneWidth, leftWidth;
    float tx, ty, tw, th;
    char textBuffer[ 8 ];

    if( rimWidth < 0.6f )
        rimWidth = 0.6f;

    if( special >= 0.0f )
        rimWidth = special;

    if( progress < 0.0f )
        progress = 0.0f;
    else if( progress > 1.0f )
        progress = 1.0f;

    doneWidth = ( rect->w - 2 * rimWidth ) * progress;
    leftWidth = ( rect->w - 2 * rimWidth ) - doneWidth;

    trap_R_SetColor( color );

    //draw rim and bar
    if( align == ITEM_ALIGN_RIGHT )
    {
        CG_DrawPic( rect->x, rect->y, rimWidth, rect->h, cgs.media.whiteShader );
        CG_DrawPic( rect->x + rimWidth, rect->y,
                    leftWidth, rimWidth, cgs.media.whiteShader );
        CG_DrawPic( rect->x + rimWidth, rect->y + rect->h - rimWidth,
                    leftWidth, rimWidth, cgs.media.whiteShader );
        CG_DrawPic( rect->x + rimWidth + leftWidth, rect->y,
                    rimWidth + doneWidth, rect->h, cgs.media.whiteShader );
    }
    else
    {
        CG_DrawPic( rect->x, rect->y, rimWidth + doneWidth, rect->h, cgs.media.whiteShader );
        CG_DrawPic( rimWidth + rect->x + doneWidth, rect->y,
                    leftWidth, rimWidth, cgs.media.whiteShader );
        CG_DrawPic( rimWidth + rect->x + doneWidth, rect->y + rect->h - rimWidth,
                    leftWidth, rimWidth, cgs.media.whiteShader );
        CG_DrawPic( rect->x + rect->w - rimWidth, rect->y, rimWidth, rect->h, cgs.media.whiteShader );
    }

    trap_R_SetColor( NULL );

    //draw text
    if( scale > 0.0 )
    {
        Com_sprintf( textBuffer, sizeof( textBuffer ), "%d%%", (int)( progress * 100 ) );
        tw = CG_Text_Width( textBuffer, scale, 0 );
        th = scale * 40.0f;

        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x + ( rect->w / 10.0f );
            ty = rect->y + ( rect->h / 2.0f ) + ( th / 2.0f );
            break;

        case ITEM_ALIGN_RIGHT:
            tx = rect->x + rect->w - ( rect->w / 10.0f ) - tw;
            ty = rect->y + ( rect->h / 2.0f ) + ( th / 2.0f );
            break;

        case ITEM_ALIGN_CENTER:
            tx = rect->x + ( rect->w / 2.0f ) - ( tw / 2.0f );
            ty = rect->y + ( rect->h / 2.0f ) + ( th / 2.0f );
            break;

        default:
            tx = ty = 0.0f;
        }

        CG_Text_Paint( tx, ty, scale, color, textBuffer, 0, 0, textStyle );
    }
}

static void CG_DrawProgressLabel( rectDef_t *rect, float text_x, float text_y, vec4_t color,
                                  float scale, int align, const char *s, float fraction )
{
    vec4_t white = { 1.0f, 1.0f, 1.0f, 1.0f };
    float tx, tw = CG_Text_Width( s, scale, 0 );

    switch( align )
    {
    case ITEM_ALIGN_LEFT:
        tx = 0.0f;
        break;

    case ITEM_ALIGN_RIGHT:
        tx = rect->w - tw;
        break;

    case ITEM_ALIGN_CENTER:
        tx = ( rect->w / 2.0f ) - ( tw / 2.0f );
        break;

    default:
        tx = 0.0f;
    }

    if( fraction < 1.0f )
        CG_Text_Paint( rect->x + text_x + tx, rect->y + text_y, scale, white,
                       s, 0, 0, ITEM_TEXTSTYLE_NORMAL );
    else
        CG_Text_Paint( rect->x + text_x + tx, rect->y + text_y, scale, color,
                       s, 0, 0, ITEM_TEXTSTYLE_NEON );
}

static void CG_DrawMediaProgress( rectDef_t *rect, vec4_t color, float scale,
                                  int align, int textStyle, int special )
{
    CG_DrawProgressBar( rect, color, scale, align, textStyle, special, cg.mediaFraction );
}

static void CG_DrawMediaProgressLabel( rectDef_t *rect, float text_x, float text_y,
                                       vec4_t color, float scale, int align )
{
    CG_DrawProgressLabel( rect, text_x, text_y, color, scale, align, "Overall", cg.mediaFraction );
}

static void CG_DrawSoundProgress( rectDef_t *rect, vec4_t color, float scale,
                                  int align, int textStyle, int special )
{
    CG_DrawProgressBar( rect, color, scale, align, textStyle, special, cg.soundFraction );
}

static void CG_DrawSoundProgressLabel( rectDef_t *rect, float text_x, float text_y,
                                       vec4_t color, float scale, int align )
{
    CG_DrawProgressLabel( rect, text_x, text_y, color, scale, align, "Sound", cg.soundFraction );
}

static void CG_DrawGraphicProgress( rectDef_t *rect, vec4_t color, float scale,
                                    int align, int textStyle, int special )
{
    CG_DrawProgressBar( rect, color, scale, align, textStyle, special, cg.graphicFraction );
}

static void CG_DrawGraphicProgressLabel( rectDef_t *rect, float text_x, float text_y,
        vec4_t color, float scale, int align )
{
    CG_DrawProgressLabel( rect, text_x, text_y, color, scale, align, "Graphics", cg.graphicFraction );
}

static void CG_DrawLevelShot( rectDef_t *rect )
{
    const char *s;
    const char *info;
    qhandle_t levelshot;
    qhandle_t detail;

    info = CG_ConfigString( CS_SERVERINFO );
    s = Info_ValueForKey( info, "mapname" );
    levelshot = trap_R_RegisterShaderNoMip( va( "levelshots/%s.tga", s ) );

    if( !levelshot )
        levelshot = trap_R_RegisterShaderNoMip( "menu/art/unknownmap" );

    trap_R_SetColor( NULL );
    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, levelshot );

    // blend a detail texture over it
    detail = trap_R_RegisterShader( "levelShotDetail" );
    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, detail );
}

static void CG_DrawLoadingString( rectDef_t *rect, float text_x, float text_y, vec4_t color,
                                  float scale, int align, int textStyle, const char *s )
{
    float tw, th, tx;
    int pos, i;
    char buffer[ 1024 ];
    char *end;

    if( !s[ 0 ] )
        return;

    strcpy( buffer, s );
    tw = CG_Text_Width( s, scale, 0 );
    th = scale * 40.0f;

    pos = i = 0;

    while( pos < strlen( s ) )
    {
        strcpy( buffer, &s[ pos ] );
        tw = CG_Text_Width( buffer, scale, 0 );

        while( tw > rect->w )
        {
            end = strrchr( buffer, ' ' );

            if( end == NULL )
                break;

            *end = '\0';
            tw = CG_Text_Width( buffer, scale, 0 );
        }

        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x;
            break;

        case ITEM_ALIGN_RIGHT:
            tx = rect->x + rect->w - tw;
            break;

        case ITEM_ALIGN_CENTER:
            tx = rect->x + ( rect->w / 2.0f ) - ( tw / 2.0f );
            break;

        default:
            tx = 0.0f;
        }

        CG_Text_Paint( tx + text_x, rect->y + text_y + i * ( th + 3 ), scale, color,
                       buffer, 0, 0, textStyle );

        pos += strlen( buffer ) + 1;
        i++;
    }
}

static void CG_DrawLevelName( rectDef_t *rect, float text_x, float text_y,
                              vec4_t color, float scale, int align, int textStyle )
{
    const char *s;

    s = CG_ConfigString( CS_MESSAGE );

    CG_DrawLoadingString( rect, text_x, text_y, color, scale, align, textStyle, s );
}

static void CG_DrawMOTD( rectDef_t *rect, float text_x, float text_y,
                         vec4_t color, float scale, int align, int textStyle )
{
    const char *s;

    s = CG_ConfigString( CS_MOTD );

    CG_DrawLoadingString( rect, text_x, text_y, color, scale, align, textStyle, s );
}

static void CG_DrawHostname( rectDef_t *rect, float text_x, float text_y,
                             vec4_t color, float scale, int align, int textStyle )
{
    char buffer[ 1024 ];
    const char *info;

    info = CG_ConfigString( CS_SERVERINFO );

    Q_strncpyz( buffer, Info_ValueForKey( info, "sv_hostname" ), 1024 );
    Q_CleanStr( buffer );

    CG_DrawLoadingString( rect, text_x, text_y, color, scale, align, textStyle, buffer );
}

/*
=====================
CG_DrawXCrosshairNames
=====================
*/
static void CG_DrawXCrosshairNames( rectDef_t *rect, float scale, int textStyle )
{
    float   *color;
    char    *name;
    float   w, x;

    if(!(HudGroupFlag & XHAIRNAME))
        return;

    if( !cg_drawCrosshair.integer )
        return;

    if( !cg_drawCrosshairNames.integer )
        return;

    if( cg.renderingThirdPerson )
        return;

// scan the known entities to see if the crosshair is sighted on one
    CG_ScanForCrosshairEntity( );

// draw the name of the player being looked at
    color = CG_FadeColor( cg.crosshairClientTime, 1000 );
    if( !color )
    {
        trap_R_SetColor( NULL );
        return;
    }

    name = cgs.clientinfo[ cg.crosshairClientNum ].name;
    w = CG_Text_Width( name, scale, 0 );
    x = rect->x + rect->w / 2;
    CG_Text_Paint( x - w / 2, rect->y + rect->h, scale, color, name, 0, 0, textStyle );
    trap_R_SetColor( NULL );
}

/*==============
    DrawFieldPadded

Draws large numbers for status bar and powerups
==============*/
static void CG_DrawFieldPadded( int x, int y, int width, int cw, int ch, int value )
{
    char num[ 16 ], *ptr;
    int l, orgL;
    int frame;
    int charWidth, charHeight;

    charWidth = cw;
    if( !charWidth )
        charWidth = CHAR_WIDTH;

    charHeight = ch;
    if( !charHeight )
        charWidth = CHAR_HEIGHT;

    if( width < 1 )
        return;

// draw number string
    if( width > 4 )
        width = 4;

    switch( width )
    {
    case 1:
        value = value > 9 ? 9 : value;
        value = value < 0 ? 0 : value;
        break;
    case 2:
        value = value > 99 ? 99 : value;
        value = value < -9 ? -9 : value;
        break;
    case 3:
        value = value > 999 ? 999 : value;
        value = value < -99 ? -99 : value;
        break;
    case 4:
        value = value > 9999 ? 9999 : value;
        value = value < -999 ? -999 : value;
        break;
    }

    Com_sprintf( num, sizeof( num ), "%d", value );
    l = strlen( num );

    if( l > width )
        l = width;

    orgL = l;

    x += 2;

    ptr = num;
    while( *ptr && l )
    {
        if( width > orgL )
        {
            CG_DrawPic( x,y, charWidth, charHeight, cgs.media.numberShaders[ 0 ] );
            width--;
            x += charWidth;
            continue;
        }

        if( *ptr == '-' )
            frame = STAT_MINUS;
        else
            frame = *ptr - '0';

        CG_DrawPic( x,y, charWidth, charHeight, cgs.media.numberShaders[ frame ] );
        x += charWidth;
        ptr++;
        l--;
    }
}

/*==============
           CG_DrawField

Draws large numbers for status bar and powerups
 ==============*/
static void CG_DrawField( int x, int y, int width, int cw, int ch, int value )
{
    char num[ 16 ], *ptr;
    int l;
    int frame;
    int charWidth, charHeight;

    charWidth = cw;
    if( !charWidth)
        charWidth = CHAR_WIDTH;

    charHeight = ch;
    if( !charHeight )
        charWidth = CHAR_HEIGHT;

    if( width < 1 )
        return;

// draw number string
    if( width > 4 )
        width = 4;

    switch( width )
    {
    case 1:
        value = value > 9 ? 9 : value;
        value = value < 0 ? 0 : value;
        break;
    case 2:
        value = value > 99 ? 99 : value;
        value = value < -9 ? -9 : value;
        break;
    case 3:
        value = value > 999 ? 999 : value;
        value = value < -99 ? -99 : value;
        break;
    case 4:
        value = value > 9999 ? 9999 : value;
        value = value < -999 ? -999 : value;
        break;
    }

    Com_sprintf( num, sizeof( num ), "%d", value );
    l = strlen( num );

    if( l > width )
        l = width;

    x += 2 + charWidth * ( width - l );

    ptr = num;
    while( *ptr && l )
    {
        if( *ptr == '-' )
            frame = STAT_MINUS;
        else
            frame = *ptr -'0';

        CG_DrawPic( x,y, charWidth, charHeight, cgs.media.numberShaders[ frame ] );
        x += charWidth;
        ptr++;
        l--;
    }
}

/*==================
           CG_DrawXFPS
==================*/
//TA: personally i think this should be longer - it should really be a cvar
#define FPS_XFRAMES 20
#define FPS_STRING "fps"
static void CG_DrawXFPS( rectDef_t *rect, float text_x, float text_y,
                         float scale, vec4_t color, int align, int textStyle,
                         qboolean scalableText )
{
    char *s;
    int tx, w, totalWidth, strLength;
    static int previousTimes[ FPS_XFRAMES ];
    static int index;
    int i, total;
    int fps;
    static int previous;
    int t, frameTime;

    if(!(HudGroupFlag & UPPERIGHT))
        return;

    if( !cg_drawFPS.integer )
        return;

    // don't use serverTime, because that will be drifting to
    // correct for internet lag changes, timescales, timedemos, etc
    t = trap_Milliseconds( );
    frameTime = t - previous;
    previous = t;

    previousTimes[ index % FPS_XFRAMES ] = frameTime;
    index++;

    if( index > FPS_XFRAMES )
    {
        // average multiple frames together to smooth changes out a bit
        total = 0;

        for( i = 0 ; i < FPS_XFRAMES ; i++ )
            total += previousTimes[ i ];

        if( !total )
            total = 1;

        fps = 1000 * FPS_XFRAMES / total;

        s = va( "%d", fps );
        w = CG_Text_Width( "0", scale, 0 );
        strLength = CG_DrawStrlen( s );
        totalWidth = CG_Text_Width( FPS_STRING, scale, 0 ) + w * strLength;

        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x;
            break;

        case ITEM_ALIGN_RIGHT:
            tx = rect->x + rect->w - totalWidth;
            break;

        case ITEM_ALIGN_CENTER:
            tx = rect->x + ( rect->w / 2.0f ) - ( totalWidth / 2.0f );
            break;

        default:
            tx = 0.0f;
        }

        if( scalableText )
        {
            for( i = 0; i < strLength; i++ )
            {
                char c[ 2 ];

                c[ 0 ] = s[ i ];
                c[ 1 ] = '\0';

                CG_Text_Paint( text_x + tx + i * w, rect->y + text_y, scale, color, c, 0, 0, textStyle );
            }
        }
        else
        {
            trap_R_SetColor( color );
            CG_DrawField( rect->x, rect->y, 3, rect->w / 3, rect->h, fps );
            trap_R_SetColor( NULL );
        }

        if( scalableText )
            CG_Text_Paint( text_x + tx + i * w, rect->y + text_y, scale, color, FPS_STRING, 0, 0, textStyle );
    }
}

/*=================
           CG_DrawTimerMins
=================*/
static void CG_DrawTimerMins( rectDef_t *rect, vec4_t color )
{
    int mins, seconds;
    int msec;

    if(!(HudGroupFlag & UPPERIGHT))
        return;

    if( !cg_drawTimer.integer )
        return;

    msec = cg.time - cgs.levelStartTime;

    seconds = msec / 1000;
    mins = seconds / 60;
    seconds -= mins * 60;

    trap_R_SetColor( color );
    CG_DrawField( rect->x, rect->y, 3, rect->w / 3, rect->h, mins );
    trap_R_SetColor( NULL );
}


/*=================
           CG_DrawTimerSecs
=================*/
static void CG_DrawTimerSecs( rectDef_t *rect, vec4_t color )
{
    int mins, seconds;
    int msec;

    if(!(HudGroupFlag & UPPERIGHT))
        return;

    if( !cg_drawTimer.integer )
        return;

    msec = cg.time - cgs.levelStartTime;

    seconds = msec / 1000;
    mins = seconds / 60;
    seconds -= mins * 60;

    trap_R_SetColor( color );
    CG_DrawFieldPadded( rect->x, rect->y, 2, rect->w / 2, rect->h, seconds );
    trap_R_SetColor( NULL );
}




/*==================
           CG_DrawXSnapshot
==================*/
static void CG_DrawXSnapshot( rectDef_t *rect, float text_x, float text_y,
                              float scale, vec4_t color, int align, int textStyle )
{
    char *s;
    int w, tx;

    if(!(HudGroupFlag & UPPERIGHT))
        return;

    if( !cg_drawSnapshot.integer )
        return;

    s = va( "time:%d snap:%d cmd:%d", cg.snap->serverTime,
            cg.latestSnapshotNum, cgs.serverCommandSequence );
    w = CG_Text_Width( s, scale, 0 );

    switch( align )
    {
    case ITEM_ALIGN_LEFT:
        tx = rect->x;
        break;

    case ITEM_ALIGN_RIGHT:
        tx = rect->x + rect->w - w;
        break;

    case ITEM_ALIGN_CENTER:
        tx = rect->x + ( rect->w / 2.0f ) - ( w / 2.0f );
        break;

    default:
        tx = 0.0f;
    }

    CG_Text_Paint( text_x + tx, rect->y + text_y, scale, color, s, 0, 0, textStyle );
}



/*==============
           CG_DrawXDisconnect

Should we draw something differnet for long lag vs no packets?
==============*/
static void CG_DrawXDisconnect( void )
{
    float x, y;
    int cmdNum;
    usercmd_t cmd;
    const char *s;
    int w;
    vec4_t color = { 1.0f, 1.0f, 1.0f, 1.0f };

// draw the phone jack if we are completely past our buffers
    cmdNum = trap_GetCurrentCmdNumber( ) - CMD_BACKUP + 1;
    trap_GetUserCmd( cmdNum, &cmd );

    // special check for map_restart
    if( cmd.serverTime <= cg.snap->ps.commandTime || cmd.serverTime > cg.time )
        return;

// also add text in center of screen
    s = "Connection Interrupted";
    w = CG_Text_Width( s, 0.7f, 0 );
    CG_Text_Paint( 320 - w / 2, 100, 0.7f, color, s, 0, 0, ITEM_TEXTSTYLE_SHADOWED );

// blink the icon
    if( ( cg.time >> 9 ) & 1 )
        return;

    x = 640 - 48;
    y = 480 - 48;

    CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader( "gfx/2d/net.tga" ) );
}

#define MAX_LAGOMETER_PING 900
#define MAX_LAGOMETER_RANGE 300
#define PING_FRAMES 40

/*==============
           CG_DrawXLagometer
==============*/
static void CG_DrawXLagometer( rectDef_t *rect, float text_x, float text_y,
                               float scale, vec4_t textColor )
{
    int a, x, y, i;
    float v;
    float ax, ay, aw, ah, mid, range;
    int color;
    vec4_t adjustedColor;
    float vscale;
    vec4_t white = { 1.0f, 1.0f, 1.0f, 1.0f };

    if(!(HudGroupFlag & UPPERIGHT))
        return;

    if( cg.snap->ps.pm_type == PM_INTERMISSION )
        return;

    if( !cg_lagometer.integer )
        return;

    if( cg.demoPlayback )
        return;

    Vector4Copy( textColor, adjustedColor );
    adjustedColor[ 3 ] = 0.25f;

    trap_R_SetColor( adjustedColor );
    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.whiteShader );
    trap_R_SetColor( NULL );

    //
    // draw the graph
    //
    ax = x = rect->x;
    ay = y = rect->y;
    aw = rect->w;
    ah = rect->h;

    trap_R_SetColor( NULL );

    CG_AdjustFrom640( &ax, &ay, &aw, &ah );

    color = -1;
    range = ah / 3;
    mid = ay + range;

    vscale = range / MAX_LAGOMETER_RANGE;

// draw the frame interpoalte / extrapolate graph
    for( a = 0 ; a < aw ; a++ )
    {
        i = ( lagometer.frameCount - 1 - a ) & ( LAG_SAMPLES - 1 );
        v = lagometer.frameSamples[ i ];
        v *= vscale;

        if( v > 0 )
        {
            if( color != 1 )
            {
                color = 1;
                trap_R_SetColor( g_color_table[ ColorIndex( COLOR_YELLOW ) ] );
            }

            if( v > range )
                v = range;

            trap_R_DrawStretchPic( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
        }
        else if( v < 0 )
        {
            if( color != 2 )
            {
                color = 2;
                trap_R_SetColor( g_color_table[ ColorIndex( COLOR_BLUE ) ] );
            }

            v = -v;
            if( v > range )
                v = range;

            trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
        }
    }

// draw the snapshot latency / drop graph
    range = ah / 2;
    vscale = range / MAX_LAGOMETER_PING;

    for( a = 0 ; a < aw ; a++ )
    {
        i = ( lagometer.snapshotCount - 1 - a ) & ( LAG_SAMPLES - 1 );
        v = lagometer.snapshotSamples[ i ];

        if( v > 0 )
        {
            if( lagometer.snapshotFlags[ i ] & SNAPFLAG_RATE_DELAYED )
            {
                if( color != 5 )
                {
                    color = 5; // YELLOW for rate delay
                    trap_R_SetColor( g_color_table[ ColorIndex( COLOR_YELLOW ) ] );
                }
            }
            else
            {
                if( color != 3 )
                {
                    color = 3;

                    trap_R_SetColor( g_color_table[ ColorIndex( COLOR_GREEN ) ] );
                }
            }

            v = v * vscale;

            if( v > range )
                v = range;

            trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
        }
        else if( v < 0 )
        {
            if( color != 4 )
            {
                color = 4; // RED for dropped snapshots
                trap_R_SetColor( g_color_table[ ColorIndex( COLOR_RED ) ] );
            }

            trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
        }
    }

    trap_R_SetColor( NULL );

    if( cg_nopredict.integer || cg_synchronousClients.integer )
        CG_Text_Paint( ax, ay, 0.5, white, "snc", 0, 0, ITEM_TEXTSTYLE_NORMAL );
    else
    {
        static int previousPings[ PING_FRAMES ];
        static int index;
        int i, ping = 0;
        char *s;

        previousPings[ index++ ] = cg.snap->ping;
        index = index % PING_FRAMES;

        for( i = 0; i < PING_FRAMES; i++ )
            ping += previousPings[ i ];

        ping /= PING_FRAMES;

        s = va( "%d", ping );
        ax = rect->x + ( rect->w / 2.0f ) - ( CG_Text_Width( s, scale, 0 ) / 2.0f ) + text_x;
        ay = rect->y + ( rect->h / 2.0f ) + ( CG_Text_Height( s, scale, 0 ) / 2.0f ) + text_y;

        Vector4Copy( textColor, adjustedColor );
        adjustedColor[ 3 ] = 0.5f;
        CG_Text_Paint( ax, ay, scale, adjustedColor, s, 0, 0, ITEM_TEXTSTYLE_NORMAL );
    }

    CG_DrawXDisconnect( );
}

/*==============
           CG_DrawConsole
==============*/
static void CG_DrawConsole( rectDef_t *rect, float text_x, float text_y, vec4_t color,
                            float scale, int align, int textStyle )
{
    float x, y, w, h;

//for some reason if these are stored locally occasionally rendering fails
    //even though they are both live until the end of the function, hence static
    //possible compiler bug??
    static menuDef_t dummyParent;
    static itemDef_t textItem;

    if(!(HudGroupFlag & XCONSOLE))
        return;

//offset the text
    x = rect->x+50;
    y = rect->y;
    w = rect->w - ( 16 + ( 2 * text_x ) ); //16 to ensure text within frame
    h = rect->h;

    // Draw the icons
    /* Peter FIXME: Disabled for now till i can figure out how to space, the text on the y coordinate
      for (i = 0; i<cg.numConsoleLines+1;i++)
      {
          if (cg.consoleLines[i].icon)
          {
              trap_R_DrawStretchPic( x-50, y+(SMALLCHAR_HEIGHT*i)-25, 40, 40, 0, 0, 1, 1, trap_R_RegisterShader(va("models/players/%s/icon_default", cg.consoleLines[i].icon)) );
          }
      }
    */
    // Draw the Text
    textItem.text = cg.consoleText;
    textItem.parent = &dummyParent;
    memcpy( textItem.window.foreColor, color, sizeof( vec4_t ) );
    textItem.window.flags = 0;

    switch( align )
    {
    case ITEM_ALIGN_LEFT:
        textItem.window.rect.x = x;
        break;

    case ITEM_ALIGN_RIGHT:
        textItem.window.rect.x = x + w;
        break;

    case ITEM_ALIGN_CENTER:
        textItem.window.rect.x = x + ( w / 2 );
        break;

    default:
        textItem.window.rect.x = x;
        break;
    }

    textItem.window.rect.y = y;
    textItem.window.rect.w = w;
    textItem.window.rect.h = h;
    textItem.window.borderSize = 0;
    textItem.textRect.x = 0;
    textItem.textRect.y = 0;
    textItem.textRect.w = 0;
    textItem.textRect.h = 0;
    textItem.textalignment = align;
    textItem.textalignx = text_x;
    textItem.textaligny = text_y;
    textItem.textscale = scale;
    textItem.textStyle = textStyle;

    //hack to utilise existing autowrap code
    Item_Text_AutoWrapped_Paint( &textItem );
}

/*====================
CG_DrawXWeaponSelect
====================*/
static void CG_DrawXWeaponSelect(rectDef_t *rect, int align, float special, float scale,
                                 vec4_t forecolor, int textStyle)
{
    rectDef_t r2;
    int i;
    int bits;
    int count, weap;
    int x, y;
    float *color;

    if(!(HudGroupFlag & XWEAPONSELECT))
        return;

    r2.x = rect->x;
    r2.y = rect->y;
    r2.w = rect->w;
    r2.h = rect->h;

    if(special==0)
        special = 4;

// don't display if dead
    if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 )
    {
        return;
    }

    color = CG_FadeColor( cg.weaponSelectTime, WEAPON_SELECT_TIME );
    if ( !color )
    {
        return;
    }
    trap_R_SetColor( color );

// showing weapon select clears pickup item display, but not the blend blob
    cg.itemPickupTime = 0;

// count the number of weapons owned
    bits = cg.snap->ps.stats[ STAT_WEAPONS ];
    count = 0;
    for ( i = 1 ; i < 16 ; i++ )
    {
        if ( bits & ( 1 << i ) )
        {
            count++;
        }
    }

    if (align == HUD_VERTICAL)
    {
        x = r2.x;
        y = r2.y;
    }
    else
    {
        x = r2.x - count * (r2.w/2);
        y = r2.y;
    }

    for ( i = 1 ; i < 16 ; i++ )
    {
        if ( !( bits & ( 1 << i ) ) )
        {
            continue;
        }

        weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[i].weapon;
        CG_RegisterWeapon( weap, cg.snap->ps.stats[STAT_CHARGE] );


// draw weapon icon
        CG_DrawPic( x+special, y+special, r2.w-(special*2), r2.h-(special*2), trap_R_RegisterShader(bg_itemlist[weap].icon) );

// draw selection marker
        if ( i == cg.weaponSelect )
        {
            CG_DrawPic( x, y, r2.w, r2.h, cgs.media.selectShader );
        }

// no ammo cross on top
        if ( !cg.snap->ps.ammo[ i ] )
        {
            CG_DrawPic( x+special, y+special, r2.w-(special*2), r2.h-(special*2), cgs.media.noammoShader );
        }

        if (align == HUD_VERTICAL)
        {
            y += r2.h;
        }
        else
        {
            x += r2.w;
        }
    }

    trap_R_SetColor( NULL );

}

/*====================
           CG_XCenterPrint
====================*/
static void CG_XCenterPrint( rectDef_t *rect, float text_x, float text_y,
                             float scale, vec4_t forecolor, int align, int textStyle )
{
    char *start;
    int l;
    int y, w;
    int tx ;
    float *color;


    if(!(HudGroupFlag & XCENTERPRINT))
        return;

    if ( !cg.centerPrintTime )
    {
        return;
    }

    color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
    if ( !color )
    {
        return;
    }

    trap_R_SetColor( color );

    start = cg.centerPrint;

    y = text_y;

    while ( 1 )
    {
        char linebuffer[1024];

        for ( l = 0; l < 50; l++ )
        {
            if ( !start[l] || start[l] == '\n' )
            {
                break;
            }
            linebuffer[l] = start[l];
        }
        linebuffer[l] = 0;

        w = CG_Text_Width(linebuffer, scale, 0);
        switch( align )
        {
        case ITEM_ALIGN_LEFT:
            tx = rect->x + text_x;
            break;

        case ITEM_ALIGN_RIGHT:
            tx = rect->x - w + text_x;
            break;

        case ITEM_ALIGN_CENTER:
            tx = rect->x - ( w / 2.0f ) + text_x;
            break;

        default:
            tx = rect->x + text_x;
        }

        CG_Text_Paint(tx, rect->y + y, scale, color, linebuffer, 0, 0, textStyle);
        y += text_y;

        while ( *start && ( *start != '\n' ) )
        {
            start++;
        }
        if ( !*start )
        {
            break;
        }
        start++;
    }

    trap_R_SetColor( NULL );
}

//
void CG_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle)
{
    rectDef_t rect;

    if ( cg_drawStatus.integer == 0 )
    {
        return;
    }

    //if (ownerDrawFlags != 0 && !CG_OwnerDrawVisible(ownerDrawFlags)) {
    //	return;
    //}

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    switch (ownerDraw)
    {
    case CG_PLAYER_ARMOR_ICON:
        CG_DrawPlayerArmorIcon(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
        break;
    case CG_PLAYER_ARMOR_ICON2D:
        CG_DrawPlayerArmorIcon(&rect, qtrue);
        break;
    case CG_PLAYER_ARMOR_VALUE:
        CG_DrawPlayerArmorValue(&rect, scale, color, shader, textStyle, align);
        break;
    case CG_PLAYER_AMMO_BAR:
        CG_DrawPlayerAmmoBar(&rect);
        break;
    case CG_PLAYER_AMMO_ICON:
        CG_DrawPlayerAmmoIcon(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
        break;
    case CG_PLAYER_AMMO_ICON2D:
        CG_DrawPlayerAmmoIcon(&rect, qtrue);
        break;
    case CG_PLAYER_AMMO_VALUE:
        CG_DrawPlayerAmmoValue(&rect, scale, color, shader, textStyle, align);
        break;
    case CG_SELECTEDPLAYER_HEAD:
        CG_DrawSelectedPlayerHead(&rect, ownerDrawFlags & CG_SHOW_2DONLY, qfalse);
        break;
    case CG_SELECTEDPLAYER_STATUS:
        CG_DrawSelectedPlayerStatus(&rect);
        break;
    case CG_SELECTEDPLAYER_ARMOR:
        CG_DrawSelectedPlayerArmor(&rect, scale, color, shader, textStyle);
        break;
    case CG_SELECTEDPLAYER_HEALTH:
        CG_DrawSelectedPlayerHealth(&rect, scale, color, shader, textStyle);
        break;
    case CG_SELECTEDPLAYER_NAME:
        CG_DrawSelectedPlayerName(&rect, scale, color, qfalse, textStyle);
        break;
    case CG_SELECTEDPLAYER_LOCATION:
        CG_DrawSelectedPlayerLocation(&rect, scale, color, textStyle);
        break;
    case CG_SELECTEDPLAYER_WEAPON:
        CG_DrawSelectedPlayerWeapon(&rect);
        break;
    case CG_SELECTEDPLAYER_POWERUP:
        CG_DrawSelectedPlayerPowerup(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
        break;
    case CG_PLAYER_HEAD:
        CG_DrawPlayerHead(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
        break;
    case CG_PLAYER_ITEM:
        CG_DrawPlayerItem(&rect, scale, ownerDrawFlags & CG_SHOW_2DONLY);
        break;
    case CG_PLAYER_SCORE:
        CG_DrawPlayerScore(&rect, scale, color, shader, textStyle);
        break;
    case CG_PLAYER_HEALTH:
        CG_DrawPlayerHealth(&rect, scale, color, shader, textStyle, align);
        break;
    case CG_PLAYER_HEALTH_BAR:
        CG_DrawPlayerHealthBar(&rect);
        break;
    case CG_PLAYER_SUBTANK:
        CG_DrawPlayerSubtank(&rect, scale, color, shader, textStyle, align);
        break;
    case CG_PLAYER_SUBTANK_BAR:
        CG_DrawPlayerSubtankBar(&rect);
        break;
    case CG_RED_SCORE:
        CG_DrawRedScore(&rect, scale, color, shader, textStyle);
        break;
    case CG_BLUE_SCORE:
        CG_DrawBlueScore(&rect, scale, color, shader, textStyle);
        break;
    case CG_RED_NAME:
        CG_DrawRedName(&rect, scale, color, textStyle);
        break;
    case CG_BLUE_NAME:
        CG_DrawBlueName(&rect, scale, color, textStyle);
        break;
    case CG_BLUE_FLAGHEAD:
        CG_DrawBlueFlagHead(&rect);
        break;
    case CG_BLUE_FLAGSTATUS:
        CG_DrawBlueFlagStatus(&rect, shader);
        break;
    case CG_BLUE_FLAGNAME:
        CG_DrawBlueFlagName(&rect, scale, color, textStyle);
        break;
    case CG_RED_FLAGHEAD:
        CG_DrawRedFlagHead(&rect);
        break;
    case CG_RED_FLAGSTATUS:
        CG_DrawRedFlagStatus(&rect, shader);
        break;
    case CG_RED_FLAGNAME:
        CG_DrawRedFlagName(&rect, scale, color, textStyle);
        break;
    case CG_PLAYER_LOCATION:
        CG_DrawPlayerLocation(&rect, scale, color, textStyle);
        break;
    case CG_TEAM_COLOR:
        CG_DrawTeamColor(&rect, color);
        break;
    case CG_AREA_POWERUP:
        CG_DrawAreaPowerUp(&rect, align, special, scale, color);
        break;
    case CG_PLAYER_STATUS:
        CG_DrawPlayerStatus(&rect);
        break;
    case CG_PLAYER_HASFLAG:
        CG_DrawPlayerHasFlag(&rect, qfalse);
        break;
    case CG_PLAYER_HASFLAG2D:
        CG_DrawPlayerHasFlag(&rect, qtrue);
        break;
    case CG_AREA_SYSTEMCHAT:
        CG_DrawAreaSystemChat(&rect, scale, color, shader);
        break;
    case CG_AREA_TEAMCHAT:
        CG_DrawAreaTeamChat(&rect, scale, color, shader);
        break;
    case CG_AREA_CHAT:
        CG_DrawAreaChat(&rect, scale, color, shader);
        break;
    case CG_GAME_TYPE:
        CG_DrawGameType(&rect, scale, color, shader, textStyle);
        break;
    case CG_GAME_STATUS:
        CG_DrawGameStatus(&rect, scale, color, shader, textStyle);
        break;
    case CG_KILLER:
        CG_DrawKiller(&rect, scale, color, shader, textStyle);
        break;
    case CG_ACCURACY:
    case CG_ASSISTS:
    case CG_DEFEND:
    case CG_EXCELLENT:
    case CG_IMPRESSIVE:
    case CG_PERFECT:
    case CG_GAUNTLET:
    case CG_CAPTURES:
        CG_DrawMedal(ownerDraw, &rect, scale, color, shader);
        break;
    case CG_UGCHIPS:
        CG_DrawPlayerChips(&rect, scale, color, shader, textStyle, align);
        break;
    case CG_SPECTATORS:
        CG_DrawTeamSpectators(&rect, scale, color, shader);
        break;
    case CG_TEAMINFO:
        if (cg_currentSelectedPlayer.integer == numSortedTeamPlayers)
        {
            CG_DrawNewTeamInfo(&rect, text_x, text_y, scale, color, shader);
        }
        break;
    case CG_CAPFRAGLIMIT:
        CG_DrawCapFragLimit(&rect, scale, color, shader, textStyle);
        break;
    case CG_1STPLACE:
        CG_Draw1stPlace(&rect, scale, color, shader, textStyle);
        break;
    case CG_2NDPLACE:
        CG_Draw2ndPlace(&rect, scale, color, shader, textStyle);
        break;
    case CG_LOAD_LEVELSHOT:
        CG_DrawLevelShot( &rect );
        break;
    case CG_LOAD_MEDIA:
        CG_DrawMediaProgress( &rect, color, scale, align, textStyle, special );
        break;
    case CG_LOAD_MEDIA_LABEL:
        CG_DrawMediaProgressLabel( &rect, text_x, text_y, color, scale, align );
        break;
    case CG_LOAD_SOUND:
        CG_DrawSoundProgress( &rect, color, scale, align, textStyle, special );
        break;
    case CG_LOAD_SOUND_LABEL:
        CG_DrawSoundProgressLabel( &rect, text_x, text_y, color, scale, align );
        break;
    case CG_LOAD_GRAPHIC:
        CG_DrawGraphicProgress( &rect, color, scale, align, textStyle, special );
        break;
    case CG_LOAD_GRAPHIC_LABEL:
        CG_DrawGraphicProgressLabel( &rect, text_x, text_y, color, scale, align );
        break;
    case CG_LOAD_LEVELNAME:
        CG_DrawLevelName( &rect, text_x, text_y, color, scale, align, textStyle );
        break;
    case CG_LOAD_MOTD:
        CG_DrawMOTD( &rect, text_x, text_y, color, scale, align, textStyle );
        break;
    case CG_LOAD_HOSTNAME:
        CG_DrawHostname( &rect, text_x, text_y, color, scale, align, textStyle );
        break;
    case CG_PLAYER_CROSSHAIRNAMES:
        CG_DrawXCrosshairNames( &rect, scale, textStyle );
        break;
    case CG_FPS:
        CG_DrawXFPS( &rect, text_x, text_y, scale, color, align, textStyle, qtrue );
        break;
    case CG_FPS_FIXED:
        CG_DrawXFPS( &rect, text_x, text_y, scale, color, align, textStyle, qfalse );
        break;
    case CG_TIMER:
        CG_DrawTimer( &rect, text_x, text_y, scale, color, align, textStyle );
        break;
    case CG_TIMER_MINS:
        CG_DrawTimerMins( &rect, color );
        break;
    case CG_TIMER_SECS:
        CG_DrawTimerSecs( &rect, color );
        break;
    case CG_SNAPSHOT:
        CG_DrawXSnapshot( &rect, text_x, text_y, scale, color, align, textStyle );
        break;
    case CG_LAGOMETER:
        CG_DrawXLagometer( &rect, text_x, text_y, scale, color );
        break;
    case CG_CONSOLE:
        CG_DrawConsole( &rect, text_x, text_y, color, scale, align, textStyle );
        break;
    case CG_WEAPONSELECT:
        CG_DrawXWeaponSelect(&rect, align, special, scale, color, textStyle);
        break;
    case CG_CENTERPRINT:
        CG_XCenterPrint( &rect, text_x, text_y, scale, color, align, textStyle);
        break;
    default:
        break;
    }
}

void CG_MouseEvent(int x, int y)
{
    int n;

    if ( (cg.predictedPlayerState.pm_type == PM_NORMAL || cg.predictedPlayerState.pm_type == PM_SPECTATOR) && cg.showScores == qfalse)
    {
        trap_Key_SetCatcher(0);
        return;
    }

    cgs.cursorX+= x;
    if (cgs.cursorX < 0)
        cgs.cursorX = 0;
    else if (cgs.cursorX > 640)
        cgs.cursorX = 640;

    cgs.cursorY += y;
    if (cgs.cursorY < 0)
        cgs.cursorY = 0;
    else if (cgs.cursorY > 480)
        cgs.cursorY = 480;

    n = Display_CursorType(cgs.cursorX, cgs.cursorY);
    cgs.activeCursor = 0;
    if (n == CURSOR_ARROW)
    {
        cgs.activeCursor = cgs.media.selectCursor;
    }
    else if (n == CURSOR_SIZER)
    {
        cgs.activeCursor = cgs.media.sizeCursor;
    }

    if (cgs.capturedItem)
    {
        Display_MouseMove(cgs.capturedItem, x, y);
    }
    else
    {
        Display_MouseMove(NULL, cgs.cursorX, cgs.cursorY);
    }

}

/*
==================
CG_HideTeamMenus
==================

*/
void CG_HideTeamMenu(void)
{
    Menus_CloseByName("teamMenu");
    Menus_CloseByName("getMenu");
}

/*
==================
CG_ShowTeamMenus
==================

*/
void CG_ShowTeamMenu(void)
{
    Menus_OpenByName("teamMenu");
}




/*
==================
CG_EventHandling
==================
 type 0 - no event handling
      1 - team menu
      2 - hud editor

*/
void CG_EventHandling(int type)
{
    cgs.eventHandling = type;
    if (type == CGAME_EVENT_NONE)
    {
        CG_HideTeamMenu();
    }
    else if (type == CGAME_EVENT_TEAMMENU)
    {
        //CG_ShowTeamMenu();
    }
    else if (type == CGAME_EVENT_SCOREBOARD)
    {
    }

}



void CG_KeyEvent(int key, qboolean down)
{

    if (!down)
    {
        return;
    }

    if ( cg.predictedPlayerState.pm_type == PM_NORMAL || (cg.predictedPlayerState.pm_type == PM_SPECTATOR && cg.showScores == qfalse))
    {
        CG_EventHandling(CGAME_EVENT_NONE);
        trap_Key_SetCatcher(0);
        return;
    }

    //if (key == trap_Key_GetKey("teamMenu") || !Display_CaptureItem(cgs.cursorX, cgs.cursorY)) {
    // if we see this then we should always be visible
    //  CG_EventHandling(CGAME_EVENT_NONE);
    //  trap_Key_SetCatcher(0);
    //}



    Display_HandleKey(key, down, cgs.cursorX, cgs.cursorY);

    if (cgs.capturedItem)
    {
        cgs.capturedItem = NULL;
    }
    else
    {
        if (key == K_MOUSE2 && down)
        {
            cgs.capturedItem = Display_CaptureItem(cgs.cursorX, cgs.cursorY);
        }
    }
}

int CG_ClientNumFromName(const char *p)
{
    int i;
    for (i = 0; i < cgs.maxclients; i++)
    {
        if (cgs.clientinfo[i].infoValid && Q_stricmp(cgs.clientinfo[i].name, p) == 0)
        {
            return i;
        }
    }
    return -1;
}

void CG_ShowResponseHead()
{
    Menus_OpenByName("voiceMenu");
    trap_Cvar_Set("cl_conXOffset", "72");
    cg.voiceTime = cg.time;
}

void CG_RunMenuScript(char **args)
{
}


void CG_GetTeamColor(vec4_t *color)
{
    if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED)
    {
        (*color)[0] = 1.0f;
        (*color)[3] = 0.25f;
        (*color)[1] = (*color)[2] = 0.0f;
    }
    else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
    {
        (*color)[0] = (*color)[1] = 0.0f;
        (*color)[2] = 1.0f;
        (*color)[3] = 0.25f;
    }
    else
    {
        (*color)[0] = (*color)[2] = 0.0f;
        (*color)[1] = 0.17f;
        (*color)[3] = 0.25f;
    }
}
