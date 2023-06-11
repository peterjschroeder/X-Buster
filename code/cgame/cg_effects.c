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


/*
==================
CG_BubbleTrail

Bullets shot underwater
==================
*/
void CG_BubbleTrail( vec3_t start, vec3_t end, float spacing )
{
    vec3_t		move;
    vec3_t		vec;
    float		len;
    int			i;

    if ( cg_noProjectileTrail.integer )
    {
        return;
    }

    VectorCopy (start, move);
    VectorSubtract (end, start, vec);
    len = VectorNormalize (vec);

    // advance a random amount first
    i = genrand_int32() % (int)spacing;
    VectorMA( move, i, vec, move );

    VectorScale (vec, spacing, vec);

    for ( ; i < len; i += spacing )
    {
        localEntity_t	*le;
        refEntity_t		*re;

        le = CG_AllocLocalEntity();
        le->leFlags = LEF_PUFF_DONT_SCALE;
        le->leType = LE_MOVE_SCALE_FADE;
        le->startTime = cg.time;
        le->endTime = cg.time + 1000 + random() * 250;
        le->lifeRate = 1.0 / ( le->endTime - le->startTime );

        re = &le->refEntity;
        re->shaderTime = cg.time / 1000.0f;

        re->reType = RT_SPRITE;
        re->rotation = 0;
        re->radius = 3;
        re->customShader = cgs.media.waterBubbleShader;
        re->shaderRGBA[0] = 0xff;
        re->shaderRGBA[1] = 0xff;
        re->shaderRGBA[2] = 0xff;
        re->shaderRGBA[3] = 0xff;

        le->color[3] = 1.0;

        le->pos.trType = TR_LINEAR;
        le->pos.trTime = cg.time;
        VectorCopy( move, le->pos.trBase );
        le->pos.trDelta[0] = crandom()*5;
        le->pos.trDelta[1] = crandom()*5;
        le->pos.trDelta[2] = crandom()*5 + 6;

        VectorAdd (move, vec, move);
    }
}

/*
=====================
CG_SmokePuff

Adds a smoke puff or blood trail localEntity.
=====================
*/
localEntity_t *CG_SmokePuff( const vec3_t p, const vec3_t vel,
                             float radius,
                             float r, float g, float b, float a,
                             float duration,
                             int startTime,
                             int fadeInTime,
                             int leFlags,
                             qhandle_t hShader )
{
    static int	seed = 0x92;
    localEntity_t	*le;
    refEntity_t		*re;
//	int fadeInTime = startTime + duration / 2;

    le = CG_AllocLocalEntity();
    le->leFlags = leFlags;
    le->radius = radius;

    re = &le->refEntity;
    re->rotation = Q_random( &seed ) * 360;
    re->radius = radius;
    re->shaderTime = startTime / 1000.0f;

    le->leType = LE_MOVE_SCALE_FADE;
    le->startTime = startTime;
    le->fadeInTime = fadeInTime;
    le->endTime = startTime + duration;
    if ( fadeInTime > startTime )
    {
        le->lifeRate = 1.0 / ( le->endTime - le->fadeInTime );
    }
    else
    {
        le->lifeRate = 1.0 / ( le->endTime - le->startTime );
    }
    le->color[0] = r;
    le->color[1] = g;
    le->color[2] = b;
    le->color[3] = a;


    le->pos.trType = TR_LINEAR;
    le->pos.trTime = startTime;
    VectorCopy( vel, le->pos.trDelta );
    VectorCopy( p, le->pos.trBase );

    VectorCopy( p, re->origin );
    re->customShader = hShader;

    // rage pro can't alpha fade, so use a different shader
    if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO )
    {
        re->customShader = cgs.media.smokePuffRageProShader;
        re->shaderRGBA[0] = 0xff;
        re->shaderRGBA[1] = 0xff;
        re->shaderRGBA[2] = 0xff;
        re->shaderRGBA[3] = 0xff;
    }
    else
    {
        re->shaderRGBA[0] = le->color[0] * 0xff;
        re->shaderRGBA[1] = le->color[1] * 0xff;
        re->shaderRGBA[2] = le->color[2] * 0xff;
        re->shaderRGBA[3] = 0xff;
    }

    re->reType = RT_SPRITE;
    re->radius = le->radius;

    return le;
}
/*
=====================
CG_GravityTrail

Adds a trail that is susceptible to gravity
=====================
*/
localEntity_t *CG_GravityTrail( const vec3_t p, const vec3_t vel,
                                float radius,
                                float r, float g, float b, float a,
                                float duration,
                                int startTime,
                                int fadeInTime,
                                int leFlags,
                                qhandle_t hShader )
{
    static int	seed = 0x92;
    localEntity_t	*le;
    refEntity_t		*re;
//	int fadeInTime = startTime + duration / 2;

    le = CG_AllocLocalEntity();
    le->leFlags = leFlags;
    le->radius = radius;

    re = &le->refEntity;
    re->rotation = Q_random( &seed ) * 360;
    re->radius = radius;
    re->shaderTime = startTime / 1000.0f;

    le->leType = LE_FRAGMENT;
    le->startTime = startTime;
    le->fadeInTime = fadeInTime;
    le->endTime = startTime + duration;
    if ( fadeInTime > startTime )
    {
        le->lifeRate = 1.0 / ( le->endTime - le->fadeInTime );
    }
    else
    {
        le->lifeRate = 1.0 / ( le->endTime - le->startTime );
    }
    le->color[0] = r;
    le->color[1] = g;
    le->color[2] = b;
    le->color[3] = a;
    re->shaderRGBA[0] = r * 255;
    re->shaderRGBA[1] = b * 255;
    re->shaderRGBA[2] = g * 255;
    re->shaderRGBA[3] = a * 255;

    le->pos.trType = TR_GRAVITY;
    le->pos.trTime = startTime;
    VectorCopy( vel, le->pos.trDelta );
    VectorCopy( p, le->pos.trBase );

    VectorCopy( p, re->origin );
    re->customShader = hShader;

    re->reType = RT_SPRITE;
    re->radius = le->radius;

    return le;
}


/*
==================
CG_SpawnEffect

Player teleporting in or out

Peter: New teleport effect, oldscool megaman beam from sky

==================
*/
void CG_SpawnEffect ( centity_t *cent, vec3_t org )
{
    vec3_t end;
    localEntity_t *le;
    refEntity_t   *re;
    trace_t tr;
    clientInfo_t	*ci = &cgs.clientinfo[ cent->currentState.clientNum ];
    int	pclass = CG_ClassforPlayer( ci->modelName, ci->skinName );

    VectorCopy( org, end );

    while (1)
    {
        CG_Trace( &tr, org, NULL, NULL, end, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER );

        end[2]++;

        if( tr.surfaceFlags & SURF_SKY )  	// Found the sky, beam ends here.
            break;

        /* Peter: If we are higher than 4096 just incase there is no sky
                setting this to high, causes major slowdown */
        if( end[2] >= 4096 )
        {
            end[2] = 4096;
            break;
        }
    }

    le = CG_AllocLocalEntity();
    re = &le->refEntity;

    le->leType = LE_FADE_RGB;
    le->startTime = cg.time;
    le->endTime = cg.time + 1000;
    le->lifeRate = 1.0 / (le->endTime - le->startTime);

    re->shaderTime = cg.time / 1000.0f;
    re->reType = RT_RAIL_CORE;
    re->width = 18;

    switch ( ci->color )
    {
    case C_BLACK:
        re->customShader = cgs.media.blackSpawnShader;
        break;
    case C_RED:
        re->customShader = cgs.media.redSpawnShader;
        break;
    case C_GREEN:
        re->customShader = cgs.media.greenSpawnShader;
        break;
    case C_YELLOW:
        re->customShader = cgs.media.yellowSpawnShader;
        break;
    case C_BLUE:
        re->customShader = cgs.media.blueSpawnShader;
        break;
    case C_CYAN:
        re->customShader = cgs.media.cyanSpawnShader;
        break;
    case C_MAGENTA:
        re->customShader = cgs.media.purpleSpawnShader;
        break;
    case C_WHITE:
        re->customShader = cgs.media.whiteSpawnShader;
        break;
    default:
        if( pclass == CLASS_DWNINFINITY )
            re->customShader = cgs.media.redSpawnShader;
        else if ( pclass == CLASS_DRN00X )
            re->customShader = cgs.media.blueSpawnShader;
        else if ( pclass == CLASS_SWN001 )
            re->customShader = cgs.media.purpleSpawnShader;
        else if ( pclass == CLASS_DRN000 )
            re->customShader = cgs.media.greenSpawnShader;
        else
            re->customShader = cgs.media.yellowSpawnShader;
        break;
    }


    VectorCopy(org, re->origin);
    VectorCopy(end, re->oldorigin);

    /* Add the splash */
    le = CG_AllocLocalEntity();
    le->leFlags = LEF_PUFF_DONT_SCALE;
    le->leFlags |= LEF_PUFF_FADE_RGB;
    le->leType = LE_MOVE_SCALE_FADE;
    le->pos.trType = TR_LINEAR;
    le->startTime = cg.time;
    le->endTime = cg.time + 1000;
    le->lifeRate = 1.0 / ( le->endTime - le->startTime );

    re = &le->refEntity;

    re->reType = RT_SPRITE;
    le->radius = re->radius = 40;
    re->shaderTime = cg.time / 1000.0f;

    switch ( ci->color )
    {
    case C_BLACK:
        re->customShader = cgs.media.blackSplashShader;
        break;
    case C_RED:
        re->customShader = cgs.media.redSplashShader;
        break;
    case C_GREEN:
        re->customShader = cgs.media.greenSplashShader;
        break;
    case C_YELLOW:
        re->customShader = cgs.media.yellowSplashShader;
        break;
    case C_BLUE:
        re->customShader = cgs.media.blueSplashShader;
        break;
    case C_CYAN:
        re->customShader = cgs.media.cyanSplashShader;
        break;
    case C_MAGENTA:
        re->customShader = cgs.media.purpleSplashShader;
        break;
    case C_WHITE:
        re->customShader = cgs.media.whiteSplashShader;
        break;
    default:
        if( pclass == CLASS_DWNINFINITY )
            re->customShader = cgs.media.redSplashShader;
        else if ( pclass == CLASS_DRN00X )
            re->customShader = cgs.media.blueSplashShader;
        else if ( pclass == CLASS_SWN001 )
            re->customShader = cgs.media.purpleSplashShader;
        else
            re->customShader = cgs.media.greenSpawnShader;
        break;
    }

    VectorCopy( org, re->origin );
    VectorCopy( re->origin, le->pos.trBase );
    trap_R_AddRefEntityToScene( re );
}



/*
====================
CG_Lightning_Discharge
====================
*/
void CG_Lightning_Discharge (vec3_t origin, int msec)
{
    localEntity_t		*le;

    if (msec <= 0) CG_Error ("CG_Lightning_Discharge: msec = %i", msec);

    le = CG_SmokePuff (	origin,			// where
                        vec3_origin,			// where to
                        ((48 + (msec * 10)) / 16),	// radius
                        1, 1, 1, 1,			// RGBA color shift
                        300 + msec,			// duration
                        cg.time,			// start when?
                        0,					// fade in time
                        0,				// flags (?)
                        trap_R_RegisterShader ("models/weaphits/electric.tga"));

    le->leType = LE_SCALE_FADE;
}

/*
==================
CG_ScorePlum
==================
*/
void CG_ScorePlum( int client, vec3_t org, int score )
{
    localEntity_t	*le;
    refEntity_t		*re;
    vec3_t			angles;
    static vec3_t lastPos;

    // only visualize for the client that scored
    if (client != cg.predictedPlayerState.clientNum || cg_scorePlum.integer == 0)
    {
        return;
    }

    le = CG_AllocLocalEntity();
    le->leFlags = 0;
    le->leType = LE_SCOREPLUM;
    le->startTime = cg.time;
    le->endTime = cg.time + 4000;
    le->lifeRate = 1.0 / ( le->endTime - le->startTime );


    le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;
    le->radius = score;

    VectorCopy( org, le->pos.trBase );
    if (org[2] >= lastPos[2] - 20 && org[2] <= lastPos[2] + 20)
    {
        le->pos.trBase[2] -= 20;
    }

    //CG_Printf( NULL, "Plum origin %i %i %i -- %i\n", (int)org[0], (int)org[1], (int)org[2], (int)Distance(org, lastPos));
    VectorCopy(org, lastPos);


    re = &le->refEntity;

    re->reType = RT_SPRITE;
    re->radius = 16;

    VectorClear(angles);
    AnglesToAxis( angles, re->axis );
}


/*
====================
CG_MakeExplosion
====================
*/
localEntity_t *CG_MakeExplosion( vec3_t origin, vec3_t dir,
                                 qhandle_t hModel, qhandle_t shader,
                                 int msec, qboolean isSprite )
{
    //float			ang;
    localEntity_t	*ex; //*ex2; ex2 - Using for sprite plane flat
    int				offset;
    vec3_t			tmpVec, newOrigin;


    if ( msec <= 0 )
    {
        CG_Error( "CG_MakeExplosion: msec = %i", msec );
    }


    // skew the time a bit so they aren't all in sync
    offset = genrand_int32() & 63;

    ex = CG_AllocLocalEntity();
    if ( isSprite )
    {
        //cg_activeLocalEntities[]
        ex->leType = LE_SPRITE_EXPLOSION;

        /*// randomly rotate sprite orientation
        ex->refEntity.rotation = genrand_int32() % 360;*/
        VectorScale( dir, 0, tmpVec );
        VectorAdd( tmpVec, origin, newOrigin );

    }
    else
    {
        ex->leType = LE_EXPLOSION;
        VectorCopy( origin, newOrigin );

        // set axis with random rotate
        /*if ( !dir ) {
        	AxisClear( ex->refEntity.axis );
        } else {
        	ang = genrand_int32() % 360;*/
        VectorCopy( dir, ex->refEntity.axis[0] );
        //RotateAroundDirection( ex->refEntity.axis, ang );
    }

    ex->startTime = cg.time - offset;
    ex->endTime = ex->startTime + msec;

    // bias the time so all shader effects start correctly
    ex->refEntity.shaderTime = ex->startTime / 1000.0f;

    ex->refEntity.hModel = hModel;
    ex->refEntity.customShader = shader;

    // set origin
    VectorCopy( newOrigin, ex->refEntity.origin );
    VectorCopy( newOrigin, ex->refEntity.oldorigin );

    ex->color[0] = ex->color[1] = ex->color[2] = 1.0;

    return ex;
}


// particle function for the death spheres
//added for particle effects
vec3_t	sphereangles[] =
{
    //x-y plane
    { 1.0f, 1.0f, 0 },
    { 1.0f, 0, 0 },
    { 1.0f, -1.0f, 0 },
    { 0, 1.0f, 0 },
    { 0, -1.0f, 0 },
    { -1.0f, 1.0f, 0 },
    { -1.0f, 0, 0 },
    { -1.0f,-1.0f, 0 },
    //y-z plane
    { 0, 1.0f, 1.0f },
    { 0, 0, 1.0f },
    { 0, -1.0f, 1.0f },
    { 0, 1.0f, -1.0f },
    { 0, 0, -1.0f },
    { 0, -1.0f, -1.0f },
    //x-z plane
    { 1.0f, 0, 1 },
    { -1.0f, 0, 1 },
    { 1.0f, 0, -1.0f },
    { -1.0f, 0, -1.0f },
    //the corners
    //rotate on +y, -x axis
    { 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, -1.0f },
    { -1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, -1.0f },
    //rotate on -y,-x axis
    { 1.0f, -1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f },
    { -1.0f, 1.0f, -1.0f },
};
vec3_t	rotatex[3] =
{
    { 1,	0,				0 },
    { 0,	.924f,		.3827f },
    { 0,	-.3827f,	.924f },
};
vec3_t	rotatey[3] =
{
    { .924f,	0,	-.3827f },
    { 0,		1,		0 },
    { .3827f,	0,	.924f },
};
vec3_t	rotatez[3] =
{
    { .924f,	.3827f,		0 },
    { -.3827f,	.924f,		0 },
    { 0,			0,		1 },
};


void CG_DeathSpheres( vec3_t origin, int playerclass )
{
//	int number; // number of particles
    int speed; // speed of particles
    int light, k; // amount of light for each particle
    vec4_t lColor; // color of light for each particle
    qhandle_t shader; // shader to use for the particles
    int index, radius, timeoffset = 500; //the time that the spheres actually begin to show, because the death animation needs to play
    vec3_t randVec, tempVec;//, jumpVec;

    // set defaults
    speed = 50;
    light = 700;
    radius = 30;
    lColor[0] = 1.0f;
    lColor[1] = 1.0f;
    lColor[2] = 1.0f;
    lColor[3] = 1.0f; // alpha
    if( playerclass == CLASS_DRN00X )
    {
        shader = cgs.media.xbDeathShader;
    }
    else
    {
        shader = cgs.media.xbBrownDeathShader;
    }
//FIXME: we want two concentric rings...
    for( index = 0; index < 86; index++ )
    {
        localEntity_t *le;
        refEntity_t *re;

        le = CG_AllocLocalEntity(); //allocate a local entity
        re = &le->refEntity;
        le->startTime = cg.time; // set the start time of the particle to the current time
        if( index > 51 )   //spheres shooting out randomly
        {
            speed = 300;
            radius = 15;
            le->pos.trTime = cg.time + ( index - 51 )*20 + timeoffset;
            le->fadeInTime = cg.time + ( index - 51 )*20 + timeoffset;
        }
        else if( index > 25 )  		//second (inner) ring
        {
            le->pos.trTime = cg.time + 500 + timeoffset;
            le->fadeInTime = cg.time + 500 + timeoffset;
            VectorCopy( sphereangles[index-26], tempVec);
            if( index < 32 )
            {
                VectorRotate( tempVec, rotatez, tempVec );
            }
            else if( index < 40 )
            {
                VectorRotate( tempVec, rotatex, tempVec );
            }
            else if( index < 44 )
            {
                VectorRotate( tempVec, rotatey, tempVec );
            }
            else if( index < 52 )
            {
                tempVec[2] *= .5f;
            }
        }
        else     //first ring
        {
            le->pos.trTime = cg.time + timeoffset;
            le->fadeInTime = cg.time + timeoffset;
            VectorCopy( sphereangles[index], tempVec);
        }
        le->endTime = cg.time + 6000; //set the end time
        le->lifeRate = 1.0 / ( le->endTime - le->startTime );
        re->shaderTime = cg.time / 1000.0f;
        re->reType = RT_SPRITE;
        re->rotation = 0;
        le->radius = re->radius = radius;
        re->customShader = shader;
        for( k = 0; k < 4; k++ )
        {
            le->color[k] = 1.0f;
            re->shaderRGBA[k] = 255;
        }
        le->leFlags = LEF_SPRITE_DELAY; //don't show before your time
        le->leFlags |= LEF_PUFF_DONT_SCALE; //don't change the particle size
        le->leFlags |= LEF_PUFF_FADE_RGB; //fade rgb
        le->leType = LE_MOVE_SCALE_FADE; // particle should fade over time
        le->light = light;
        VectorCopy( lColor, le->lightColor );
        le->pos.trType = TR_LINEAR; // moves in a line
        //try to vary the angles by 45 degree differentials, we want every combination possible...but that may take more work than anticipated
        if( index > 51 )
        {
            tempVec[0] = crandom(); //between 1 and -1
            tempVec[1] = crandom();
            tempVec[2] = crandom();
        }
        VectorNormalize(tempVec);
        VectorCopy( origin, le->pos.trBase );
        VectorCopy( origin, re->origin );
        VectorScale(tempVec, speed, randVec);
        VectorCopy( randVec, le->pos.trDelta );
    }
}

/*
====================
CG_MakeSphericalExplosion
====================
*/
localEntity_t *CG_MakeSphericalExplosion( vec3_t origin, int radius,
        qhandle_t hModel, qhandle_t shader, int leFlags,
        int msec )
{
    localEntity_t	*ex;
    int				offset;

    if ( msec <= 0 )
    {
        CG_Error( "CG_MakeSphericalExplosion: msec = %i", msec );
    }

    // skew the time a bit so they aren't all in sync
    offset = genrand_int32() & 63;

    ex = CG_AllocLocalEntity();

    ex->leType = LE_SPHERE_EXPLOSION;

    AxisClear( ex->refEntity.axis );

    ex->startTime = cg.time - offset;
    ex->endTime = ex->startTime + msec;

    ex->refEntity.reType = RT_MODEL;
    // bias the time so all shader effects start correctly
    ex->refEntity.shaderTime = ex->startTime / 1000.0f;

    ex->refEntity.hModel = hModel;
    if( shader )
        ex->refEntity.customShader = shader;
    else
        ex->refEntity.customShader = cgs.media.xbExplosionShader1;

    ex->leFlags = leFlags;
    ex->color[3] = 1.0f;

    ex->radius = radius;

    // set origin
    VectorCopy( origin, ex->refEntity.origin );
    VectorCopy( origin, ex->refEntity.oldorigin );

    ex->color[0] = ex->color[1] = ex->color[2] = 1.0f;

    return ex;
}

/*
====================
CG_DrawModel
====================
*/
localEntity_t *CG_DrawModel( vec3_t origin, int scale,
                             qhandle_t hModel, qhandle_t shader, int leFlags,
                             int msec )
{
    localEntity_t	*ent;
    int				offset;

    if ( msec <= 0 )
    {
        CG_Error( "CG_DrawModel: msec = %i", msec );
    }

    // skew the time a bit so they aren't all in sync
    offset = genrand_int32() & 63;

    ent = CG_AllocLocalEntity();

    ent->leType = LE_SCALE_FADE;

    AxisClear( ent->refEntity.axis );

    ent->startTime = cg.time - offset;
    ent->endTime = ent->startTime + msec;
    ent->lifeRate = 1.0f / ( ent->endTime - ent->startTime );

    ent->refEntity.reType = RT_MODEL;
    // bias the time so all shader effects start correctly
    ent->refEntity.shaderTime = ent->startTime / 1000.0f;

    ent->refEntity.hModel = hModel;
    if( shader )
        ent->refEntity.customShader = shader;

    if( scale != 1.0f )
    {
        VectorNormalize( ent->refEntity.axis[0] );
        VectorNormalize( ent->refEntity.axis[1] );
        VectorNormalize( ent->refEntity.axis[2] );
        VectorScale( ent->refEntity.axis[0], scale, ent->refEntity.axis[0] );
        VectorScale( ent->refEntity.axis[1], scale, ent->refEntity.axis[1] );
        VectorScale( ent->refEntity.axis[2], scale, ent->refEntity.axis[2] );
        ent->refEntity.nonNormalizedAxes = qtrue;
    }
    ent->leFlags = leFlags;

    // set origin
    VectorCopy( origin, ent->refEntity.origin );
    VectorCopy( origin, ent->refEntity.oldorigin );

    ent->color[0] = ent->color[1] = ent->color[2] = ent->color[3] = 1.0f;

    return ent;
}

/*
===============
CG_GravityWell
===============
*/
localEntity_t* CG_GravityWell( const vec3_t p )
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
    re->customShader = cgs.media.chargeSphere3Shader;

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
    VectorScale(tempVec, -132, randVec);
    VectorCopy( randVec, le->pos.trDelta );
    VectorCopy( p, le->pos.trBase );
    return le;
}

/*
 ==================
 CG_LaunchGlass
 ==================
 */
void CG_LaunchGlass( vec3_t origin, vec3_t velocity, qhandle_t hModel )
{
    localEntity_t	*le;
    refEntity_t		*re;

    le = CG_AllocLocalEntity();
    re = &le->refEntity;

    le->leType = LE_FRAGMENT;
    le->startTime = cg.time;
    le->endTime = le->startTime + 30000 + random() * 3000;

    VectorCopy( origin, re->origin );
    AxisCopy( axisDefault, re->axis );
    re->hModel = hModel;

    le->pos.trType = TR_GRAVITY;
    VectorCopy( origin, le->pos.trBase );
    VectorCopy( velocity, le->pos.trDelta );
    le->pos.trTime = cg.time;

    le->bounceFactor = 0.3;

    le->leFlags = LEF_TUMBLE;
    le->leBounceSoundType = LEBS_BRASS;
    le->leMarkType = LEMT_NONE;
}

/*
===================
CG_BreakGlass

Generated a bunch of glass shards launching out from the glass location
===================
*/
#define	GLASS_VELOCITY	175
#define	GLASS_JUMP		125
void CG_BreakGlass( vec3_t playerOrigin )
{
    vec3_t	origin, velocity;
    int     value;
    // How many shards to generate
    int     count = 50;
    // The array of possible numbers
    int     states[] = {1,2,3};
    // Get the size of the array
    int     numstates = sizeof(states)/sizeof(states[0]);

    // Countdown "count" so this will subtract 1 from the "count"
    // X many times. X being the "count" value
    while ( count-- )
    {
        // Generate the random number every count so every shard is a
        // of the three. If this is placed above it only gets a random
        // number every time a piece of glass is broken.
        value = states[genrand_int32()%numstates];
        VectorCopy( playerOrigin, origin );
        velocity[0] = crandom()*GLASS_VELOCITY;
        velocity[1] = crandom()*GLASS_VELOCITY;
        velocity[2] = GLASS_JUMP + crandom()*GLASS_VELOCITY;
        switch (value)
        {
        case 1:
            // If our random number was 1, generate the 1st shard piece
            CG_LaunchGlass( origin, velocity, cgs.media.breakglass01 );
            break;
        case 2:
            CG_LaunchGlass( origin, velocity, cgs.media.breakglass02 );
            break;
        case 3:
            CG_LaunchGlass( origin, velocity, cgs.media.breakglass03 );
            break;
        }
    }
}

// NIGHTZ - CG_Breakable
void CG_Breakable ( centity_t *cent )
{
    refEntity_t			ent;
    entityState_t		*s1;

    s1 = &cent->currentState;

    // create the render entity
    memset (&ent, 0, sizeof(ent));
    VectorCopy( cent->lerpOrigin, ent.origin);
    VectorCopy( cent->lerpOrigin, ent.oldorigin);
    AnglesToAxis( cent->lerpAngles, ent.axis );

    ent.renderfx = RF_NOSHADOW;

    // get the model, either as a bmodel or a modelindex
    if ( s1->solid == SOLID_BMODEL )
    {
        ent.hModel = cgs.inlineDrawModel[s1->modelindex];
    }
    else
    {
        ent.hModel = cgs.gameModels[s1->modelindex];
    }

    // add to refresh list
    trap_R_AddRefEntityToScene(&ent);

    // add the secondary model
    if ( s1->modelindex2 )
    {
        ent.skinNum = 0;
        ent.hModel = cgs.gameModels[s1->modelindex2];
        trap_R_AddRefEntityToScene(&ent);
    }

}
// END NIGHTZ

int flagEarthquake=qfalse;
int earthquakeIntensity=0;
int earthquakeStoptime=0;

void CG_StartEarthquake(int intensity,int duration)
{
    flagEarthquake=qtrue;
    if (intensity<earthquakeIntensity) return;
    earthquakeIntensity=intensity;
    earthquakeStoptime=cg.time+duration;
}

void CG_Earthquake()
{
    static float terremotoX,terremotoY,terremotoZ;
    static int terremotoTime=0;
    float realInt;
    if (!flagEarthquake) return;
    if (earthquakeStoptime<cg.time)
    {
        flagEarthquake=qfalse;
        earthquakeIntensity=0;
        return;
    }
    if (terremotoTime<cg.time)
    {
        terremotoTime=cg.time+=50;
        realInt=((float)earthquakeIntensity+1.0)/2.0;
        terremotoX=random()*realInt-realInt/2;
        terremotoY=random()*realInt-realInt/2;
        terremotoZ=random()*realInt-realInt/2;
    }
    cg.refdefViewAngles[0]+=terremotoX;
    cg.refdefViewAngles[1]+=terremotoY;
    cg.refdefViewAngles[2]+=terremotoZ;
    AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
}

/*
===============
CG_BlackBars
===============
*/
void CG_BlackBars(void)
{
    float color[4] = {0,0,0,1};
    CG_FillRect(0,0,640,50, color);
    CG_FillRect(0,430,640,50, color);
}



void LightningBolt(vec3_t org, vec3_t end, float size, float fadeout)
{
    localEntity_t *le;
    refEntity_t   *re;

    le = CG_AllocLocalEntity();
    re = &le->refEntity;

    le->leType = LE_FADE_RGB;
    le->startTime = cg.time;
    le->endTime = cg.time + fadeout;
    le->lifeRate = 1.0 / (le->endTime - le->startTime);

    re->shaderTime = cg.time / 1000.0f;
    re->reType = RT_RAIL_CORE;
    re->customShader = cgs.media.lightningboltShader;

    VectorCopy(org, re->origin);
    VectorCopy(end, re->oldorigin);
}



float RandomFloat (void)
{
    float num;

    num = (genrand_int32 ()&0x7fff) / ((float)0x7fff);

    return num;
}

//normalized random vector
void randomvector (vec3_t v1)
{
    //find normalized random vector direction
    v1[0]= 512.0f * RandomFloat() - 256.0f;
    v1[1]= 512.0f * RandomFloat() - 256.0f;
    v1[2]= 512.0f * RandomFloat() - 256.0f;
    VectorNormalize(v1);
}


void CG_SkyLightning( vec3_t end, int nodes, float size, float nodes_area, float fork_length, float fadeout )
{
    vec3_t dir, node1, node2, tmp, tmp2, start;
    trace_t tr;
    float skylightning_length, bounded_fork_length, vlen;
    float percent, posbound, negbound;
    int i;

    //bound values
    if ((nodes < 3) || (nodes > 10))
    {
        Com_Printf( "Skylightning: Nodes range 3 to 10\n");
        nodes= 3;
    }
    size= fabs(size);
    nodes_area= fabs(nodes_area);
    fork_length= fabs(fork_length);
    fadeout= fabs(fadeout);
    if ((size < 1) || (size > 1024))
    {
        Com_Printf( "Skylightning: size 1 to 1024\n");
        size= 16.0f;
    }
    if ((nodes_area < 16) || (nodes_area > 1024))
    {
        Com_Printf( "Skylightning: nodes_area 16 to 1024\n");
        nodes_area= 16.0f;
    }
    if ((fork_length < 16) || (fork_length > 1024))
    {
        Com_Printf( "Skylightning: fork_length 16 to 1024\n");
        fork_length= 16.0f;
    }
    if (fadeout < 0.0001f)
    {
        Com_Printf( "Skylightning: fadeout < 0.0001\n");
        fadeout= 0.5f;
    }


    VectorCopy( end, start );

    // SCREEN FLASH
    CG_SetScreenFlash(SF_LIGHTNINGSTRIKE);

    while (1)
    {
        CG_Trace( &tr, end, NULL, NULL, start, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER );

        start[2]++;

        if( tr.surfaceFlags & SURF_SKY )  	// Found the sky, lightning starts here.
            break;

        /* Peter: If we are higher than 4096 just incase there is no sky
                setting this to high, causes major slowdown */
        if( start[2] >= 4096 )
        {
            start[2] = 4096;
            break;
        }
    }

    //find normalized vector and length from start to end
    VectorSubtract(end, start, dir);
    skylightning_length= VectorNormalize (dir);

    //form lightning strike
    VectorCopy( start, node1 );
    for (i=1 ; i <= nodes ; i++)
    {
        //get lightning nodes positions along line
        //node2= start + dir * (skylightning_length*percent)
        percent= (float)i/(float)(nodes+1);
        VectorMA (start, skylightning_length*percent, dir, node2);

        //find node's -x and +x world bound
        VectorCopy(node2, tmp );
        tmp[0]-= 10000;
        CG_Trace (&tr, node2, NULL, NULL, tmp, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER);
        VectorCopy(tr.endpos, tmp2);
        negbound= tmp2[0];
        VectorCopy(node2, tmp );
        tmp[0]+= 10000;
        CG_Trace (&tr, node2, NULL, NULL, tmp, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER);
        VectorCopy(tr.endpos, tmp2);
        posbound= tmp2[0];
        //randomize x within world bound
        node2[0]+= (nodes_area*2) * RandomFloat() - nodes_area;
        if (node2[0] < negbound)  node2[0]= negbound;
        if (node2[0] > posbound)  node2[0]= posbound;

        //find node's -y and +y world bound
        VectorCopy(node2, tmp );
        tmp[1]-= 10000;
        CG_Trace (&tr, node2, NULL, NULL, tmp, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER);
        VectorCopy(tr.endpos, tmp2);
        negbound= tmp2[1];
        VectorCopy(node2, tmp );
        tmp[1]+= 10000;
        CG_Trace (&tr, node2, NULL, NULL, tmp, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER);
        VectorCopy(tr.endpos, tmp2);
        posbound= tmp2[1];
        //randomize y within world bound
        node2[1]+= (nodes_area*2) * RandomFloat() - nodes_area;
        if (node2[1] < negbound)  node2[1]= negbound;
        if (node2[1] > posbound)  node2[1]= posbound;

        //connect lightning nodes
        LightningBolt(node1, node2, size, fadeout);

        //make fork length bounded to world
        randomvector(tmp2);
        VectorMA (node2, 1024.0f, tmp2, tmp);
        CG_Trace (&tr, node2, NULL, NULL, tmp, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER);
        VectorCopy(tr.endpos, tmp);
        VectorSubtract(tmp, node2, tmp);
        bounded_fork_length= VectorNormalize (tmp);
        if (fork_length > bounded_fork_length)
            vlen= bounded_fork_length;
        else
            vlen= fork_length;

        VectorMA (node2, vlen, tmp2, tmp);
        LightningBolt(node2, tmp, size, fadeout);

        //setup for next node, node1= node2
        VectorCopy(node2, node1);
    }

    LightningBolt(node1, end, size, fadeout);
}
