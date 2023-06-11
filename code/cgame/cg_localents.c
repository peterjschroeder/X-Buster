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

#define	MAX_LOCAL_ENTITIES	512
localEntity_t	cg_localEntities[MAX_LOCAL_ENTITIES];
localEntity_t	cg_activeLocalEntities;		// double linked list
localEntity_t	*cg_freeLocalEntities;		// single linked list

/*
===================
CG_InitLocalEntities

This is called at startup and for tournement restarts
===================
*/
void	CG_InitLocalEntities( void )
{
    int		i;

    memset( cg_localEntities, 0, sizeof( cg_localEntities ) );
    cg_activeLocalEntities.next = &cg_activeLocalEntities;
    cg_activeLocalEntities.prev = &cg_activeLocalEntities;
    cg_freeLocalEntities = cg_localEntities;
    for ( i = 0 ; i < MAX_LOCAL_ENTITIES - 1 ; i++ )
    {
        cg_localEntities[i].next = &cg_localEntities[i+1];
    }
}


/*
==================
CG_FreeLocalEntity
==================
*/
void CG_FreeLocalEntity( localEntity_t *le )
{
    if ( !le->prev )
    {
        CG_Error( "CG_FreeLocalEntity: not active" );
    }

    // remove from the doubly linked active list
    le->prev->next = le->next;
    le->next->prev = le->prev;

    // the free list is only singly linked
    le->next = cg_freeLocalEntities;
    cg_freeLocalEntities = le;
}

/*
===================
CG_AllocLocalEntity

Will allways succeed, even if it requires freeing an old active entity
===================
*/
localEntity_t	*CG_AllocLocalEntity( void )
{
    localEntity_t	*le;

    if ( !cg_freeLocalEntities )
    {
        // no free entities, so free the one at the end of the chain
        // remove the oldest active entity
        CG_FreeLocalEntity( cg_activeLocalEntities.prev );
    }

    le = cg_freeLocalEntities;
    cg_freeLocalEntities = cg_freeLocalEntities->next;

    memset( le, 0, sizeof( *le ) );

    // link into the active list
    le->next = cg_activeLocalEntities.next;
    le->prev = &cg_activeLocalEntities;
    cg_activeLocalEntities.next->prev = le;
    cg_activeLocalEntities.next = le;
    return le;
}


/*
====================================================================================

FRAGMENT PROCESSING

A fragment localentity interacts with the environment in some way (hitting walls),
or generates more localentities along a trail.

====================================================================================
*/

/*
================
CG_FragmentBounceMark
================
*/
void CG_FragmentBounceMark( localEntity_t *le, trace_t *trace )
{
    int			radius;


    radius = 8 + (genrand_int32()&15);
    CG_ImpactMark( cgs.media.burnMarkShader, trace->endpos, trace->plane.normal, random()*360,
                   1,1,1,1, qtrue, radius, qfalse );

    // don't allow a fragment to make multiple marks, or they
    // pile up while settling
    le->leMarkType = LEMT_NONE;
}

/*
================
CG_FragmentBounceSound
================
*/
void CG_FragmentBounceSound( localEntity_t *le, trace_t *trace )
{
    // don't allow a fragment to make multiple bounce sounds,
    // or it gets too noisy as they settle
    le->leBounceSoundType = LEBS_NONE;
}


/*
================
CG_ReflectVelocity
================
*/
void CG_ReflectVelocity( localEntity_t *le, trace_t *trace )
{
    vec3_t	velocity;
    float	dot;
    int		hitTime;

    // reflect the velocity on the trace plane
    hitTime = cg.time - cg.frametime + cg.frametime * trace->fraction;
    BG_EvaluateTrajectoryDelta( &le->pos, hitTime, velocity );
    dot = DotProduct( velocity, trace->plane.normal );
    VectorMA( velocity, -2*dot, trace->plane.normal, le->pos.trDelta );

    VectorScale( le->pos.trDelta, le->bounceFactor, le->pos.trDelta );

    VectorCopy( trace->endpos, le->pos.trBase );
    le->pos.trTime = cg.time;


    // check for stop, making sure that even on low FPS systems it doesn't bobble
    if ( trace->allsolid ||
            ( trace->plane.normal[2] > 0 &&
              ( le->pos.trDelta[2] < 40 || le->pos.trDelta[2] < -cg.frametime * le->pos.trDelta[2] ) ) )
    {
        le->pos.trType = TR_STATIONARY;
    }
    else
    {

    }
}

/*
================
CG_AddFragment
================
*/
void CG_AddFragment( localEntity_t *le )
{
    vec3_t	newOrigin;
    trace_t	trace;

    if ( le->pos.trType == TR_STATIONARY )
    {
        // sink into the ground if near the removal time
        int		t;
        float	oldZ;

        t = le->endTime - cg.time;
        if ( t < SINK_TIME )
        {
            // we must use an explicit lighting origin, otherwise the
            // lighting would be lost as soon as the origin went
            // into the ground
            VectorCopy( le->refEntity.origin, le->refEntity.lightingOrigin );
            le->refEntity.renderfx |= RF_LIGHTING_ORIGIN;
            oldZ = le->refEntity.origin[2];
            le->refEntity.origin[2] -= 16 * ( 1.0 - (float)t / SINK_TIME );
            trap_R_AddRefEntityToScene( &le->refEntity );
            le->refEntity.origin[2] = oldZ;
        }
        else
        {
            trap_R_AddRefEntityToScene( &le->refEntity );
        }

        return;
    }

    // calculate new position
    BG_EvaluateTrajectory( &le->pos, cg.time, newOrigin );

    // trace a line from previous position to new position
    CG_Trace( &trace, le->refEntity.origin, NULL, NULL, newOrigin, -1, CONTENTS_SOLID );
    if ( trace.fraction == 1.0 )
    {
        // still in free fall
        VectorCopy( newOrigin, le->refEntity.origin );

        if ( le->leFlags & LEF_TUMBLE )
        {
            vec3_t angles;

            BG_EvaluateTrajectory( &le->angles, cg.time, angles );
            AnglesToAxis( angles, le->refEntity.axis );
        }

        trap_R_AddRefEntityToScene( &le->refEntity );

        return;
    }

    // if it is in a nodrop zone, remove it
    // this keeps gibs from waiting at the bottom of pits of death
    // and floating levels
    if ( trap_CM_PointContents( trace.endpos, 0 ) & CONTENTS_NODROP )
    {
        CG_FreeLocalEntity( le );
        return;
    }

    // leave a mark
    CG_FragmentBounceMark( le, &trace );

    // do a bouncy sound
    CG_FragmentBounceSound( le, &trace );

    // reflect the velocity on the trace plane
    CG_ReflectVelocity( le, &trace );

    trap_R_AddRefEntityToScene( &le->refEntity );
}

/*
=====================================================================

TRIVIAL LOCAL ENTITIES

These only do simple scaling or modulation before passing to the renderer
=====================================================================
*/

/*
====================
CG_AddFadeRGB
====================
*/
void CG_AddFadeRGB( localEntity_t *le )
{
    refEntity_t *re;
    float c;

    re = &le->refEntity;

    c = ( le->endTime - cg.time ) * le->lifeRate;
    c *= 0xff;

    re->shaderRGBA[0] = le->color[0] * c;
    re->shaderRGBA[1] = le->color[1] * c;
    re->shaderRGBA[2] = le->color[2] * c;
    re->shaderRGBA[3] = le->color[3] * c;

    trap_R_AddRefEntityToScene( re );
}

/*
==================
CG_AddMoveScaleFade
==================
*/
static void CG_AddMoveScaleFade( localEntity_t *le )
{
    refEntity_t	*re;
    float		c;
    vec3_t		delta;
    float		len;

    re = &le->refEntity;

    if ( (le->fadeInTime > le->startTime) && (cg.time < le->fadeInTime) )
    {
        // dont show the sprite if it is meant to be delayed
        if (  le->leFlags & LEF_SPRITE_DELAY )
        {
            return;
        }
    }

    // fade / grow time
    c = ( le->endTime - cg.time ) * le->lifeRate;

    if ( le->leFlags & LEF_PUFF_FADE_RGB )
    {
        c = ( le->endTime - cg.time ) * le->lifeRate;
        re->shaderRGBA[0] = le->color[0] * c * 255;
        re->shaderRGBA[1] = le->color[1] * c * 255;
        re->shaderRGBA[2] = le->color[2] * c * 255;
    }
    if( !(le->leFlags & LEF_PUFF_DONT_FADE) )
    {
        re->shaderRGBA[3] = 0xff * c * le->color[3];
    }

    if ( !( le->leFlags & LEF_PUFF_DONT_SCALE ) )
    {
        if( le->leFlags & LEF_PUFF_SHRINK )
        {
            re->radius = le->radius * c + .1;
        }
        else
        {
            re->radius = le->radius * ( 1.0 - c ) + 8;
        }
    }

    BG_EvaluateTrajectory( &le->pos, cg.time, re->origin );

    // if the view would be "inside" the sprite, kill the sprite
    // so it doesn't add too much overdraw
    VectorSubtract( re->origin, cg.refdef.vieworg, delta );
    len = VectorLength( delta );
    if ( len < le->radius )
    {
        CG_FreeLocalEntity( le );
        return;
    }

    trap_R_AddRefEntityToScene( re );
}

/*
===================
CG_AddScaleFade

For rocket smokes that hang in place, fade out, and are
removed if the view passes through them.
There are often many of these, so it needs to be simple.
===================
*/
static void CG_AddScaleFade( localEntity_t *le )
{
    refEntity_t	*re;
    float		c;
    vec3_t		delta;
    float		len;

    re = &le->refEntity;

    // fade / grow time
    c = ( le->endTime - cg.time ) * le->lifeRate;

    if ( le->leFlags & LEF_PUFF_FADE_RGB )
    {
        c = ( le->endTime - cg.time ) * le->lifeRate;
        re->shaderRGBA[0] = le->color[0] * c * 255;
        re->shaderRGBA[1] = le->color[1] * c * 255;
        re->shaderRGBA[2] = le->color[2] * c * 255;
    }
    if( !(le->leFlags & LEF_PUFF_DONT_FADE) )
    {
        re->shaderRGBA[3] = 0xff * c * le->color[3];
    }

    if ( !( le->leFlags & LEF_PUFF_DONT_SCALE ) )
    {
        if( le->leFlags & LEF_PUFF_SHRINK )
        {
            re->radius = le->radius * c + .1;
        }
        else
        {
            re->radius = le->radius * ( 1.0 - c ) + 1;
        }
    }

    // if the view would be "inside" the sprite, kill the sprite
    // so it doesn't add too much overdraw
    VectorSubtract( re->origin, cg.refdef.vieworg, delta );
    len = VectorLength( delta );
    if ( len < le->radius )
    {
        CG_FreeLocalEntity( le );
        return;
    }

    trap_R_AddRefEntityToScene( re );
}

/*
=================
CG_AddFallScaleFade

This is just an optimized CG_AddMoveScaleFade
For blood mists that drift down, fade out, and are
removed if the view passes through them.
There are often 100+ of these, so it needs to be simple.
=================
*/
static void CG_AddFallScaleFade( localEntity_t *le )
{
    refEntity_t	*re;
    float		c;
    vec3_t		delta;
    float		len;

    re = &le->refEntity;

    // fade time
    c = ( le->endTime - cg.time ) * le->lifeRate;

    if ( le->leFlags & LEF_PUFF_FADE_RGB )
    {
        c = ( le->endTime - cg.time ) * le->lifeRate;
        re->shaderRGBA[0] = le->color[0] * c * 255;
        re->shaderRGBA[1] = le->color[1] * c * 255;
        re->shaderRGBA[2] = le->color[2] * c * 255;
    }
    if( !(le->leFlags & LEF_PUFF_DONT_FADE) )
    {
        re->shaderRGBA[3] = 0xff * c * le->color[3];
    }

    re->origin[2] = le->pos.trBase[2] - ( 1.0 - c ) * le->pos.trDelta[2];

    if ( !( le->leFlags & LEF_PUFF_DONT_SCALE ) )
    {
        if( le->leFlags & LEF_PUFF_SHRINK )
        {
            re->radius = le->radius * c + .1;
        }
        else
        {
            re->radius = le->radius * ( 1.0 - c ) + 16;
        }
    }

    // if the view would be "inside" the sprite, kill the sprite
    // so it doesn't add too much overdraw
    VectorSubtract( re->origin, cg.refdef.vieworg, delta );
    len = VectorLength( delta );
    if ( len < le->radius )
    {
        CG_FreeLocalEntity( le );
        return;
    }

    trap_R_AddRefEntityToScene( re );
}

/*
================
CG_AddSphereExplosion
================
*/
static void CG_AddSphereExplosion( localEntity_t *ex )
{
    refEntity_t	*ent;
    float c;

    ent = &ex->refEntity;

    c = ( ex->endTime - cg.time ) / ( float ) ( ex->endTime - ex->startTime );
    if ( ex->leFlags & LEF_PUFF_FADE_RGB )
    {
        ent->shaderRGBA[0] = ex->color[0] * c * 255;
        ent->shaderRGBA[1] = ex->color[1] * c * 255;
        ent->shaderRGBA[2] = ex->color[2] * c * 255;
        ent->shaderRGBA[3] = ex->color[3] * c * 255;
    }

    if( !( ex->leFlags & LEF_PUFF_SHRINK ) )
        c = 1.0f - c;

    if ( c >= 1 )
    {
        c = 1.0f;	// can happen during connection problems
    }

    c *= (ex->radius/60.0f);
    VectorNormalize( ent->axis[0] );
    VectorNormalize( ent->axis[1] );
    VectorNormalize( ent->axis[2] );
    VectorScale( ent->axis[0], c, ent->axis[0] );
    VectorScale( ent->axis[1], c, ent->axis[1] );
    VectorScale( ent->axis[2], c, ent->axis[2] );
    ent->nonNormalizedAxes = qtrue;
    // add the entity
    trap_R_AddRefEntityToScene(ent);
}

/*
================
CG_AddExplosion
================
*/
static void CG_AddExplosion( localEntity_t *ex )
{
    refEntity_t	*ent;

    ent = &ex->refEntity;

    // add the entity
    trap_R_AddRefEntityToScene(ent);

    // add the dlight
    if ( ex->light )
    {
        float		light;

        light = (float)( cg.time - ex->startTime ) / ( ex->endTime - ex->startTime );
        if ( light < 0.5 )
        {
            light = 1.0;
        }
        else
        {
            light = 1.0 - ( light - 0.5 ) * 2;
        }
        light = ex->light * light;
        trap_R_AddLightToScene(ent->origin, light, ex->lightColor[0], ex->lightColor[1], ex->lightColor[2] );
    }
}

/*
================
CG_AddSpriteExplosion

DonX- Want to modify to pass 2 localEntitys and then use each to draw a unique explosion sprite
================
*/
static void CG_AddSpriteExplosion( localEntity_t *le )
{
    refEntity_t	re;
    float c;

    re = le->refEntity;

    c = ( le->endTime - cg.time ) / ( float ) ( le->endTime - le->startTime );
    if ( c > 1 )
    {
        c = 1.0;	// can happen during connection problems
    }

    re.shaderRGBA[0] = 0xff;
    re.shaderRGBA[1] = 0xff;
    re.shaderRGBA[2] = 0xff;
    re.shaderRGBA[3] = 0xff * c * 0.33;

    re.reType = RT_SPRITE;
    re.radius = 42 * ( 1.0 - c ) + 30;


    trap_R_AddRefEntityToScene( &re );


    trap_R_AddRefEntityToScene ( &re );

    /*
    	// create  a flat normal poly
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

    	trap_R_AddPolyToScene( cgs.media.GroundFXplayerTalk, 4, verts );
    */
    // add the dlight
    if ( le->light )
    {
        float		light;

        light = (float)( cg.time - le->startTime ) / ( le->endTime - le->startTime );
        if ( light < 0.5 )
        {
            light = 1.0;
        }
        else
        {
            light = 1.0 - ( light - 0.5 ) * 2;
        }
        light = le->light * light;
        trap_R_AddLightToScene(re.origin, light, le->lightColor[0], le->lightColor[1], le->lightColor[2] );
    }
}


/*
===================
CG_AddScorePlum
===================
*/
#define NUMBER_SIZE		8

void CG_AddScorePlum( localEntity_t *le )
{
    refEntity_t	*re;
    vec3_t		origin, delta, dir, vec, up = {0, 0, 1};
    float		c, len;
    int			i, score, digits[10], numdigits, negative;

    re = &le->refEntity;

    c = ( le->endTime - cg.time ) * le->lifeRate;

    score = le->radius;
    if (score < 0)
    {
        re->shaderRGBA[0] = 0xff;
        re->shaderRGBA[1] = 0x11;
        re->shaderRGBA[2] = 0x11;
    }
    else
    {
        re->shaderRGBA[0] = 0xff;
        re->shaderRGBA[1] = 0xff;
        re->shaderRGBA[2] = 0xff;
        if (score >= 50)
        {
            re->shaderRGBA[1] = 0;
        }
        else if (score >= 20)
        {
            re->shaderRGBA[0] = re->shaderRGBA[1] = 0;
        }
        else if (score >= 10)
        {
            re->shaderRGBA[2] = 0;
        }
        else if (score >= 2)
        {
            re->shaderRGBA[0] = re->shaderRGBA[2] = 0;
        }

    }
    if (c < 0.25)
        re->shaderRGBA[3] = 0xff * 4 * c;
    else
        re->shaderRGBA[3] = 0xff;

    re->radius = NUMBER_SIZE / 2;

    VectorCopy(le->pos.trBase, origin);
    origin[2] += 110 - c * 100;

    VectorSubtract(cg.refdef.vieworg, origin, dir);
    CrossProduct(dir, up, vec);
    VectorNormalize(vec);

    VectorMA(origin, -10 + 20 * sin(c * 2 * M_PI), vec, origin);

    // if the view would be "inside" the sprite, kill the sprite
    // so it doesn't add too much overdraw
    VectorSubtract( origin, cg.refdef.vieworg, delta );
    len = VectorLength( delta );
    if ( len < 20 )
    {
        CG_FreeLocalEntity( le );
        return;
    }

    negative = qfalse;
    if (score < 0)
    {
        negative = qtrue;
        score = -score;
    }

    for (numdigits = 0; !(numdigits && !score); numdigits++)
    {
        digits[numdigits] = score % 10;
        score = score / 10;
    }

    if (negative)
    {
        digits[numdigits] = 10;
        numdigits++;
    }

    for (i = 0; i < numdigits; i++)
    {
        VectorMA(origin, (float) (((float) numdigits / 2) - i) * NUMBER_SIZE, vec, re->origin);
        re->customShader = cgs.media.numberShaders[digits[numdigits-1-i]];
        trap_R_AddRefEntityToScene( re );
    }
}




//==============================================================================

/*
===================
CG_AddLocalEntities

===================
*/
void CG_AddLocalEntities( void )
{
    localEntity_t	*le, *next;

    // walk the list backwards, so any new local entities generated
    // (trails, marks, etc) will be present this frame
    le = cg_activeLocalEntities.prev;
    for ( ; le != &cg_activeLocalEntities ; le = next )
    {
        // grab next now, so if the local entity is freed we
        // still have it
        next = le->prev;

        if ( cg.time >= le->endTime )
        {
            CG_FreeLocalEntity( le );
            continue;
        }
        switch ( le->leType )
        {
        default:
            CG_Error( "Bad leType: %i", le->leType );
            break;

        case LE_MARK:
            break;

        case LE_SPHERE_EXPLOSION:
            CG_AddSphereExplosion( le );
            break;

        case LE_SPRITE_EXPLOSION:
            CG_AddSpriteExplosion( le );
            break;

        case LE_EXPLOSION:
            CG_AddExplosion( le );
            break;

        case LE_FRAGMENT:			// gibs and brass
            CG_AddFragment( le );
            break;

        case LE_MOVE_SCALE_FADE:		// water bubbles
            CG_AddMoveScaleFade( le );
            break;

        case LE_FADE_RGB:				// teleporters, railtrails
            CG_AddFadeRGB( le );
            break;

        case LE_FALL_SCALE_FADE: // gib blood trails
            CG_AddFallScaleFade( le );
            break;

        case LE_SCALE_FADE:		// rocket trails
            CG_AddScaleFade( le );
            break;

        case LE_SCOREPLUM:
            CG_AddScorePlum( le );
            break;
        }
    }
}




