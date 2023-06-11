// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_weapons.c -- events and effects dealing with weapons
#include "cg_local.h"



/*
==========================
CG_RailTrail
==========================
*/
void CG_RailTrail (clientInfo_t *ci, vec3_t start, vec3_t end)
{
    vec3_t axis[36], move, move2, next_move, vec, temp;
    float  len;
    int    i, j, skip;

    localEntity_t *le;
    refEntity_t   *re;

#define RADIUS   4
#define ROTATION 1
#define SPACING  5

    start[2] -= 4;
    VectorCopy (start, move);
    VectorSubtract (end, start, vec);
    len = VectorNormalize (vec);
    PerpendicularVector(temp, vec);
    for (i = 0 ; i < 36; i++)
    {
        RotatePointAroundVector(axis[i], vec, temp, i * 10);//banshee 2.4 was 10
    }

    le = CG_AllocLocalEntity();
    re = &le->refEntity;

    le->leType = LE_FADE_RGB;
    le->startTime = cg.time;
    le->endTime = cg.time + cg_railTrailTime.value;
    le->lifeRate = 1.0 / (le->endTime - le->startTime);

    re->shaderTime = cg.time / 1000.0f;
    re->reType = RT_RAIL_CORE;
    re->customShader = cgs.media.railCoreShader;

    VectorCopy(start, re->origin);
    VectorCopy(end, re->oldorigin);

    re->shaderRGBA[0] = 0;
    re->shaderRGBA[1] = 0;
    re->shaderRGBA[2] = 255;
    re->shaderRGBA[3] = 255;

    le->color[0] = 0;
    le->color[1] = 0;
    le->color[2] = 0.75f;
    le->color[3] = 1.0f;

    AxisClear( re->axis );

    VectorMA(move, 20, vec, move);
    VectorCopy(move, next_move);
    VectorScale (vec, SPACING, vec);

    if (cg_oldRail.integer != 0)
    {
        // nudge down a bit so it isn't exactly in center
        re->origin[2] -= 8;
        re->oldorigin[2] -= 8;
        return;
    }
    skip = -1;

    j = 18;
    for (i = 0; i < len; i += SPACING)
    {
        if (i != skip)
        {
            skip = i + SPACING;
            le = CG_AllocLocalEntity();
            re = &le->refEntity;
            le->leFlags = LEF_PUFF_DONT_SCALE;
            le->leType = LE_MOVE_SCALE_FADE;
            le->startTime = cg.time;
            le->endTime = cg.time + (i>>1) + 600;
            le->lifeRate = 1.0 / (le->endTime - le->startTime);

            re->shaderTime = cg.time / 1000.0f;
            re->reType = RT_SPRITE;
            re->radius = 1.1f;
            re->customShader = cgs.media.railRingsShader;

            re->shaderRGBA[0] = ci->color2[0] * 255;
            re->shaderRGBA[1] = ci->color2[1] * 255;
            re->shaderRGBA[2] = ci->color2[2] * 255;
            re->shaderRGBA[3] = 255;

            le->color[0] = ci->color2[0] * 0.75;
            le->color[1] = ci->color2[1] * 0.75;
            le->color[2] = ci->color2[2] * 0.75;
            le->color[3] = 1.0f;

            le->pos.trType = TR_LINEAR;
            le->pos.trTime = cg.time;

            VectorCopy( move, move2);
            VectorMA(move2, RADIUS , axis[j], move2);
            VectorCopy(move2, le->pos.trBase);

            le->pos.trDelta[0] = axis[j][0]*6;
            le->pos.trDelta[1] = axis[j][1]*6;
            le->pos.trDelta[2] = axis[j][2]*6;
        }

        VectorAdd (move, vec, move);

        j = j + ROTATION < 36 ? j + ROTATION : (j + ROTATION) % 36;
    }
}

/*
==========================
CG_RocketTrail
==========================
*/
static void CG_RocketTrail( centity_t *ent, const weaponInfo_t *wi )
{
    int		step, radius, range;
    float	trailTime;
    vec3_t	origin, lastPos;
    int		t;
    int		startTime, contents;
    int		lastContents;
    entityState_t	*es;
    vec3_t	up;
    localEntity_t	*smoke;
    qhandle_t	Shader;

    if ( cg_noProjectileTrail.integer )
    {
        return;
    }

    up[0] = 0;
    up[1] = 0;
    up[2] = 0;

    step = 50;

    es = &ent->currentState;
    switch( es->weapon )
    {
    default:
        Shader = cgs.media.xbSmokeShader;
        radius = wi->trailRadius;
        range = 15;
        trailTime = 400;
        break;
    }
    if( !radius )
        return;

    startTime = ent->trailTime;
    t = step * ( (startTime + step) / step );

    BG_EvaluateTrajectory( &es->pos, cg.time, origin );
    contents = CG_PointContents( origin, -1 );

    // if object (e.g. grenade) is stationary, don't toss up smoke
    if ( es->pos.trType == TR_STATIONARY )
    {
        ent->trailTime = cg.time;
        return;
    }

    BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
    lastContents = CG_PointContents( lastPos, -1 );

    ent->trailTime = cg.time;

    if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) )
    {
        if ( contents & lastContents & CONTENTS_WATER )
        {
            CG_BubbleTrail( lastPos, origin, 8 );
        }
        return;
    }

    for ( ; t <= ent->trailTime ; t += step )
    {
        BG_EvaluateTrajectory( &es->pos, t, lastPos );

        smoke = CG_SmokePuff( lastPos, up,
                              radius,
                              1, 1, 1, 0.46f,
                              trailTime,
                              t,
                              0,
                              0,
                              Shader );
        // use the optimized local entity add
        smoke->leType = LE_SCALE_FADE;
    }
}

/*
==========================
CG_IceTrail
==========================
*/
static void CG_IceTrail( centity_t *ent, const weaponInfo_t *wi )
{
    int		step;
    vec3_t	lastPos;
    int		t;
    int		startTime;
    int		lastContents;
    entityState_t	*es;
    vec3_t	up;
    localEntity_t	*smoke;

    if ( cg_noProjectileTrail.integer )
    {
        return;
    }

    up[0] = 0;
    up[1] = 0;
    up[2] = 0;

    step = 200;

    es = &ent->currentState;
    startTime = ent->trailTime;
    t = step * ( (startTime + step) / step );

    BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
    lastContents = CG_PointContents( lastPos, -1 );

    ent->trailTime = cg.time;

    for ( ; t <= ent->trailTime ; t += step )
    {
        BG_EvaluateTrajectory( &es->pos, t, lastPos );

        smoke = CG_GravityTrail( lastPos, up,
                                 wi->trailRadius,
                                 1, 1, 1, 1,
                                 wi->wiTrailTime,
                                 t,
                                 0,
                                 0,
                                 cgs.media.xbTrail_Water);
    }
}

/*
==========================
CG_BusterTrail
for the charged buster shots
==========================
*/
static void CG_BusterTrail( centity_t *ent, const weaponInfo_t *wi )
{
    int		step = 0, radius, range = 0;
    vec3_t	lastPos;
    int		t;
    int		startTime;
    float	trailTime = 0.0;
    entityState_t	*es;
    vec3_t	up, relativeX, forward;
    localEntity_t	*smoke;
    qhandle_t	Shader = 0;

    if ( cg_noProjectileTrail.integer )
    {
        return;
    }
    //lets make it depend on the charge level
    es = &ent->currentState;
    switch( es->weapon )
    {

    case WP_XBUSTER1:
    case WP_MBUSTER1:
    case WP_PBUSTER1:
    case WP_ZBUSTER1:
        Shader = cgs.media.busterTrail1Shader;
        radius = 5;
        range = 10;
        step = 30;
        trailTime = 250;
        break;
    case WP_XBUSTER2:
    case WP_MBUSTER2:
    case WP_PBUSTER2:
    case WP_ZBUSTER2:
        Shader = cgs.media.busterTrail2Shader;
        radius = 7;
        range = 15;
        step = 30;
        trailTime = 400;
        break;
    case WP_XBUSTER3:
    case WP_XBUSTER4:
        Shader = cgs.media.busterTrail3Shader;
        radius = 3;
        range = 5;
        step = 70;
        trailTime = 1000;
        break;
    default:
        radius = 0;
        break;
    }
    if( !radius )
        return;

    startTime = ent->trailTime;
    t = step * ( (startTime + step) / step );

    // if object (e.g. grenade) is stationary, don't toss up smoke
    if ( es->pos.trType == TR_STATIONARY )
    {
        ent->trailTime = cg.time;
        return;
    }
    VectorCopy( es->pos.trDelta, forward );
    VectorNormalizeFast( forward );
    PerpendicularVector( up, forward );
    CrossProduct( forward, up, relativeX );

    ent->trailTime = cg.time;

    for ( ; t <= ent->trailTime ; t += step )
    {
        BG_EvaluateTrajectory( &es->pos, t, lastPos );
        //this all adds an amount of the relative up and right to the particle so that it is randomly placed
        //along the axis of movement
        VectorNormalizeFast( up );
        VectorNormalizeFast( relativeX );
        VectorScale( up, crandom()*range, up );
        VectorScale( relativeX, crandom()*range, relativeX );
        VectorAdd( lastPos, up, lastPos );
        VectorAdd( lastPos, relativeX, lastPos );
        smoke = CG_SmokePuff( lastPos, vec3_origin, //we dont want it to move
                              radius,
                              1, 1, 1, 1,
                              trailTime + random()*250,
                              t,
                              0,
                              LEF_PUFF_DONT_SCALE,
                              Shader );
        // use the optimized local entity add
        smoke->leType = LE_FADE_RGB;
    }
}
/*
==========================
CG_FadeTrail
==========================
*/
static void CG_FadeTrail( centity_t *ent, const weaponInfo_t *wi )
{
    int		step = 0, radius, range = 0, leFlags, leType;
    float	trailTime = 0.0;
    vec3_t	lastPos;
    int		t;
    int		startTime;
    entityState_t	*es;
    vec3_t	up;
    localEntity_t	*smoke;
    qhandle_t	Shader = 0;

    if ( cg_noProjectileTrail.integer )
    {
        return;
    }

    up[0] = 0;
    up[1] = 0;
    up[2] = 0;

    step = 30; //every 30 milliseconds (If 1 sec = 1000) ??
    leFlags = LEF_PUFF_DONT_SCALE;
    es = &ent->currentState;
    switch( es->weapon )
    {
    case WP_FCUTTER2:
    case WP_RISFIRE:
    case WP_HWAVE:
        Shader = cgs.media.xbTrail_Fire;
        radius = 15;
        range = 10;
        step = 30;
        trailTime = 250;
        break;
    case WP_TBURSTER:
        Shader = cgs.media.xbTrail_Plant;
        radius = 40;
        range = 10;
        trailTime = 1000;
        step = 80;
        break;
    case WP_TEMPEST:
        Shader = cgs.media.xbTrail_Circle;
        radius = 7;
        range = 10;
        step = 10;
        trailTime = 250;
        //leFlags = LEF_PUFF_SHRINK;
        leType = LE_FADE_RGB;
        break;
    case WP_BCRYSTAL:
        Shader = cgs.media.xbTrail_Circle;
        radius = 7;
        range = 15;
        step = 30;
        trailTime = 400;
        break;
    default:
        radius = 0;
        leFlags = LEF_PUFF_DONT_SCALE;
        leType = LE_FADE_RGB;
        break;
    }
    if( !radius )
        return;

    startTime = ent->trailTime;
    t = step * ( (startTime + step) / step );

    // if object (e.g. grenade) is stationary, don't toss up smoke
    if ( es->pos.trType == TR_STATIONARY )
    {
        ent->trailTime = cg.time;
        return;
    }

    ent->trailTime = cg.time;

    for ( ; t <= ent->trailTime ; t += step )
    {

        BG_EvaluateTrajectory( &es->pos, t, lastPos );

        smoke = CG_SmokePuff( lastPos, up,
                              radius,
                              1.0f, 1.0f, 1.0f, 1.0f,
                              trailTime + random()*250,
                              t,
                              0,
                              leFlags,
                              Shader); //original (0.2, 0.2, 0.8, 1.0)
        // use the optimized local entity add
        smoke->leFlags = LEF_PUFF_DONT_SCALE;
        smoke->leType = LE_FADE_RGB;
    }
}

/*
==========================
CG_GroundTrail
for trails that leave along the ground
==========================
*/
static void CG_GroundTrail( centity_t *ent, const weaponInfo_t *wi )
{
    int		step = 0, radius, range, leFlags;
    vec3_t	lastPos, end;
    int		t;
    int		startTime;
    float	trailTime = 0.0;
    entityState_t	*es;
    trace_t	result;
    localEntity_t	*trail;
    qhandle_t	Shader = 0;
    qhandle_t	Model = 0;
    qboolean	isModel = qfalse;

    if ( cg_noProjectileTrail.integer )
    {
        return;
    }
    leFlags = 0;
    //lets make it depend on the charge level
    es = &ent->currentState;
    switch( es->weapon )
    {

    case WP_FBURST:
        //Shader = trap_R_RegisterShader( "xb_ice" );
        Model = cgs.media.xbIceCrystal;
        isModel = qtrue;
        radius = 5;
        range = 10;
        step = 30;
        leFlags = LEF_PUFF_FADE_RGB;
        trailTime = 1000;
        break;
    default:
        radius = 0;
        break;
    }
    if( !radius )
        return;

    startTime = ent->trailTime;
    t = step * ( (startTime + step) / step );

    // if object (e.g. grenade) is stationary, don't toss up smoke
    if ( es->pos.trType == TR_STATIONARY )
    {
        ent->trailTime = cg.time;
        return;
    }
    ent->trailTime = cg.time;
    for ( ; t <= ent->trailTime ; t += step )
    {
        BG_EvaluateTrajectory( &es->pos, t, lastPos );
        VectorCopy( lastPos, end );
        end[2] -= 320;
        CG_Trace( &result, lastPos, NULL, NULL, end, ent->currentState.number, MASK_SOLID );
        if( result.fraction < 1.0 )
        {
            if( !isModel )
            {
                trail = CG_SmokePuff( result.endpos, vec3_origin, //we dont want it to move
                                      radius,
                                      1, 1, 1, 1,
                                      trailTime,
                                      t,
                                      0,
                                      LEF_PUFF_DONT_SCALE,
                                      Shader );
                // use the optimized local entity add
                trail->leType = LE_FADE_RGB;
            }
            else
            {
                trail = CG_DrawModel( result.endpos, 1, Model, Shader, leFlags, trailTime );
            }
        }
    }
}


/*
==========================
CG_GrappleTrail
==========================
*/
void CG_GrappleTrail( centity_t *ent, const weaponInfo_t *wi )
{
    vec3_t	origin;
    entityState_t	*es;
    vec3_t			forward, up;
    refEntity_t		beam;

    es = &ent->currentState;

    BG_EvaluateTrajectory( &es->pos, cg.time, origin );
    ent->trailTime = cg.time;

    memset( &beam, 0, sizeof( beam ) );
    //FIXME adjust for muzzle position
    VectorCopy ( cg_entities[ ent->currentState.otherEntityNum ].lerpOrigin, beam.origin );
    beam.origin[2] += 26;
    AngleVectors( cg_entities[ ent->currentState.otherEntityNum ].lerpAngles, forward, NULL, up );
    VectorMA( beam.origin, -6, up, beam.origin );
    VectorCopy( origin, beam.oldorigin );

    if (Distance( beam.origin, beam.oldorigin ) < 64 )
        return; // Don't draw if close

    beam.reType = RT_LIGHTNING;
    beam.customShader = cgs.media.grappleShader;

    AxisClear( beam.axis );
    beam.shaderRGBA[0] = 0xff;
    beam.shaderRGBA[1] = 0xff;
    beam.shaderRGBA[2] = 0xff;
    beam.shaderRGBA[3] = 0xff;
    trap_R_AddRefEntityToScene( &beam );
}



/*
=================
CG_RegisterWeapon

The server says this item is used on this level
=================
*/
void CG_RegisterWeapon( int weaponNum, int chargelevel )
{
    weaponInfo_t	*weaponInfo;
    gitem_t			*item, *ammo;
    vec3_t			mins, maxs;
    int				i;

    weaponInfo = &cg_weapons[weaponNum];

    if ( weaponNum == 0 )
    {
        return;
    }

    if ( weaponInfo->registered )
    {
        return;
    }

    memset( weaponInfo, 0, sizeof( *weaponInfo ) );
    weaponInfo->registered = qtrue;

    for ( item = bg_itemlist + 1 ; item->classname ; item++ )
    {
        if ( item->giType == IT_WEAPON && item->giTag == weaponNum )
        {
            weaponInfo->item = item;
            break;
        }
    }
    if ( !item->classname )
    {
        CG_Error( "Couldn't find weapon %i", weaponNum );
    }
    CG_RegisterItemVisuals( item - bg_itemlist );

    // load cmodel before model so filecache works
    weaponInfo->weaponModel = trap_R_RegisterModel( item->world_model[0] );

    // calc midpoint for rotation
    trap_R_ModelBounds( weaponInfo->weaponModel, mins, maxs );
    for ( i = 0 ; i < 3 ; i++ )
    {
        weaponInfo->weaponMidpoint[i] = mins[i] + 0.5 * ( maxs[i] - mins[i] );
    }

    weaponInfo->weaponIcon = trap_R_RegisterShader( item->icon );
    weaponInfo->ammoIcon = trap_R_RegisterShader( item->icon );

    for ( ammo = bg_itemlist + 1 ; ammo->classname ; ammo++ )
    {
        if ( ammo->giType == IT_AMMO && ammo->giTag == weaponNum )
        {
            break;
        }
    }
    if ( ammo->classname && ammo->world_model[0] )
    {
        weaponInfo->ammoModel = trap_R_RegisterModel( ammo->world_model[0] );
    }
    /*
    	strcpy( path, item->world_model[0] );
    	COM_StripExtension( path, path );
    	strcat( path, "_flash.md3" );
    	weaponInfo->flashModel = trap_R_RegisterModel( path );
    */


    weaponInfo->loopFireSound = qfalse;

    switch ( weaponNum )
    {
    case WP_XBUSTER:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/fire.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 50;
        MAKERGB( weaponInfo->missileDlightColor, 1.0f, 1.0f, 0.75f );
        weaponInfo->missileTrailFunc = CG_BusterTrail;
        weaponInfo->wiTrailTime = 1000;
        weaponInfo->trailRadius = 5;
        break;
    case WP_MBUSTER:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weapfire_ms1.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 50;
        MAKERGB( weaponInfo->missileDlightColor, 1.0f, 1.0f, 0.75f );
        weaponInfo->missileTrailFunc = CG_BusterTrail;
        weaponInfo->wiTrailTime = 1000;
        weaponInfo->trailRadius = 5;
        break;
    case WP_PBUSTER:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weapfire_ms1.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 50;
        MAKERGB( weaponInfo->missileDlightColor, 1.0f, 1.0f, 0.75f );
        weaponInfo->missileTrailFunc = CG_BusterTrail;
        weaponInfo->wiTrailTime = 1000;
        weaponInfo->trailRadius = 5;
        break;
    case WP_FBUSTER:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/bfg/bfg_fire.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 50;
        MAKERGB( weaponInfo->missileDlightColor, 0.94f, 0, 0.94f );
        break;
    case WP_ZBUSTER:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/fire.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 50;
        MAKERGB( weaponInfo->missileDlightColor, 1.0f, 1.0f, 0.75f );
        weaponInfo->missileTrailFunc = CG_BusterTrail;
        weaponInfo->wiTrailTime = 1000;
        weaponInfo->trailRadius = 5;
        break;
        //fire
    case WP_FCUTTER:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 75;
        weaponInfo->loopFireSound = qtrue;
        MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
        break;
    case WP_FCUTTER2:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/bfg/bfg_fire.wav", qfalse );
        weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 75;
        weaponInfo->missileTrailFunc = CG_FadeTrail;
        weaponInfo->wiTrailTime = 300;
        weaponInfo->trailRadius = 25;
        MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
        break;
    case WP_RISFIRE:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/bfg/bfg_fire.wav", qfalse );
        weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 75;
        weaponInfo->missileTrailFunc = CG_FadeTrail;
        weaponInfo->wiTrailTime = 300;
        weaponInfo->trailRadius = 25;
        MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
        break;
    case WP_HWAVE:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/bfg/bfg_fire.wav", qfalse );
        weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 75;
        weaponInfo->missileTrailFunc = CG_FadeTrail;
        weaponInfo->wiTrailTime = 300;
        weaponInfo->trailRadius = 25;
        MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
        break;
    case WP_PSTRIKE:
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.75f, 0 );
        weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/rocket/rocket.md3" );
        weaponInfo->missileSound = trap_S_RegisterSound( "sound/xbuster/weapons/weaphit_bang4.wav", qfalse );
        weaponInfo->missileDlight = 75;
        MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
        break;
        //water
    case WP_SICE:
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/fire.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileTrailFunc = CG_IceTrail;
        weaponInfo->wiTrailTime = 500;
        weaponInfo->trailRadius = 10;
        break;
    case WP_FBURST:
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileTrailFunc = CG_GroundTrail;
        weaponInfo->wiTrailTime = 500;
        weaponInfo->trailRadius = 10;
        break;
    case WP_IBURST:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        break;
    case WP_WSTRIKE:
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.5f, 0 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/railgun/railgf1a.wav", qfalse );
        cgs.media.railExplosionShader = trap_R_RegisterShader( "railExplosion" );
        cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );
        cgs.media.railCoreShader = trap_R_RegisterShader( "railCore" );
        break;
        //air
    case WP_AFIST:
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weapfire_air7.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/wind/charge1.md3" );
        weaponInfo->missileSound = trap_S_RegisterSound ("sound/xbuster/weapons/weaponfly_air2.wav",qfalse);
        break;
    case WP_AWAVE:
        MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/wind/buster.md3" );
        weaponInfo->missileSound = trap_S_RegisterSound ("sound/xbuster/weapons/weaponfly_air2.wav",qfalse);
        break;
    case WP_TEMPEST:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weapfire_tempest1.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/wind/rapid.md3" );
        weaponInfo->missileTrailFunc = CG_FadeTrail;
        weaponInfo->wiTrailTime = 1000;
        weaponInfo->trailRadius = 20;
        weaponInfo->missileSound = trap_S_RegisterSound ("sound/xbuster/weapons/weaponfly_air2.wav",qfalse);
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_IMBOOMERANG:
        //weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weapfire_ms4.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/wind/wind_imboomerang.MD3" );
        weaponInfo->missileDlight = 50;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 1.0f, 1.0f, 0.75f );
        //weaponInfo->missileTrailFunc = CG_FadeTrail;
        //weaponInfo->wiTrailTime = 300;
        //weaponInfo->trailRadius = 5;
        break;
        //earth
    case WP_BCRYSTAL:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weapfire_ms2.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/earth/charge1.md3" );
        weaponInfo->missileTrailFunc = CG_FadeTrail;
        weaponInfo->wiTrailTime = 300;
        weaponInfo->trailRadius = 5;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_RSTONE:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weaphit_bang1.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/earth/buster1.md3" );
        break;
    case WP_SBURST:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weaphit_bang1.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/earth/rapid.md3" );
        break;
        //metal
    case WP_LTORPEDO:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/tech/charge_ht.md3" );
        weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
        weaponInfo->missileTrailFunc = CG_RocketTrail;
        weaponInfo->wiTrailTime = 500;
        weaponInfo->trailRadius = 20;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_BLAUNCHER:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        //weaponInfo->missileModel = trap_R_RegisterModel( "weapons/tech/buster_sb.md3" );
        weaponInfo->missileModel = trap_R_RegisterModel( "models/weapons/metalman/metalman_blade.md3" );
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_STRIKECHAIN:
        weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/lightning/lg_hum.wav", qfalse );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/lightning/lg_fire.wav", qfalse );
        cgs.media.grappleShader = trap_R_RegisterShader( "mmo_grapple_rope");
        cgs.media.lightningExplosionModel = trap_R_RegisterModel( "models/weaphits/crackle.md3" );
        weaponInfo->missileModel = trap_R_RegisterModel( "models/weapons/grapple/grapple.md3" );
        break;
    case WP_DSHOT:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/tech/rapid_d.md3" );
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        weaponInfo->missileTrailFunc = CG_RocketTrail;
        weaponInfo->wiTrailTime = 500;
        weaponInfo->trailRadius = 40;
        weaponInfo->missileDlight = 100;
        break;
        //nature
    case WP_TBURSTER:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/fire.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/plant/charge1.md3" );
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        weaponInfo->missileTrailFunc = CG_FadeTrail;
        weaponInfo->wiTrailTime = 500;
        weaponInfo->trailRadius = 40;
        weaponInfo->missileDlight = 100;
        break;
    case WP_LSTORM:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_ASPLASHER:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weaphit_acid2.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
        //gravity
    case WP_GBOMB:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/fire.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileSound = trap_S_RegisterSound ("sound/xbuster/weapons/weaponfly_gravity4.wav", qfalse);
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_GRAVWELL:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        break;
    case WP_MMINE:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        weaponInfo->missileSound = trap_S_RegisterSound ("sound/xbuster/weapons/weaponfly_gravity4.wav", qfalse);
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_DMATTER:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        weaponInfo->missileSound = trap_S_RegisterSound ("sound/xbuster/weapons/weaponfly_gravity4.wav", qfalse);
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
        //light
    case WP_BLASER:
        weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/lightning/lg_hum.wav", qfalse );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/lightning/lg_fire.wav", qfalse );
        cgs.media.lightningExplosionModel = trap_R_RegisterModel( "weapons/charge_hit1.md3" );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        cgs.media.sfx_lghit1 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit.wav", qfalse );
        cgs.media.sfx_lghit2 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit2.wav", qfalse );
        cgs.media.sfx_lghit3 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit3.wav", qfalse );
        break;
    case WP_SSHREDDER:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_NBURST:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weaphit_bang3.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
        //electrical
    case WP_SFIST:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/fire.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_TRTHUNDER:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/xbuster/weapons/weaphit_bang3.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        weaponInfo->missileSound = trap_S_RegisterSound ("sound/xbuster/weapons/weaponfly_energy1.wav", qfalse);
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_BSHIELD:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    case WP_BLIGHTNING:
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
        weaponInfo->missileModel = trap_R_RegisterModel( "weapons/energy/tbomb.md3" );
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        break;
    default:
        weaponInfo->missileModel = cgs.media.xbSpriteModel;
        weaponInfo->missileDlight = 100;
        MAKERGB( weaponInfo->missileDlightColor, 0.82f, 0.82f, 1.0f );
        MAKERGB( weaponInfo->flashDlightColor, 1, 1, 1 );
        weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
        break;
    }
}

/*
=================
CG_RegisterItemVisuals

The server says this item is used on this level
=================
*/
void CG_RegisterItemVisuals( int itemNum )
{
    itemInfo_t		*itemInfo;
    gitem_t			*item;

    if ( itemNum < 0 || itemNum >= bg_numItems )
    {
        CG_Error( "CG_RegisterItemVisuals: itemNum %d out of range [0-%d]", itemNum, bg_numItems-1 );
    }

    itemInfo = &cg_items[ itemNum ];
    if ( itemInfo->registered )
    {
        return;
    }

    item = &bg_itemlist[ itemNum ];

    memset( itemInfo, 0, sizeof( &itemInfo ) );
    itemInfo->registered = qtrue;

    itemInfo->models[0] = trap_R_RegisterModel( item->world_model[0] );

    itemInfo->icon = trap_R_RegisterShader( item->icon );

    if ( item->giType == IT_WEAPON )
    {
        CG_RegisterWeapon( item->giTag, 0 );
    }

    //
    // powerups have an accompanying ring or sphere
    //
    if ( item->giType == IT_POWERUP || item->giType == IT_HEALTH ||
            item->giType == IT_ARMOR || item->giType == IT_HOLDABLE )
    {
        if ( item->world_model[1] )
        {
            itemInfo->models[1] = trap_R_RegisterModel( item->world_model[1] );
        }
    }
}


/*
========================================================================================

VIEW WEAPON

========================================================================================
*/

/*
=================
CG_MapTorsoToWeaponFrame

=================
*/
static int CG_MapTorsoToWeaponFrame( clientInfo_t *ci, int frame )
{

    // change weapon
    if ( frame >= ci->animations[TORSO_DROP].firstFrame
            && frame < ci->animations[TORSO_DROP].firstFrame + 9 )
    {
        return frame - ci->animations[TORSO_DROP].firstFrame + 6;
    }

    // stand attack
    if ( frame >= ci->animations[TORSO_ATTACK].firstFrame
            && frame < ci->animations[TORSO_ATTACK].firstFrame + 6 )
    {
        return 1 + frame - ci->animations[TORSO_ATTACK].firstFrame;
    }

    // stand attack 2
    if ( frame >= ci->animations[TORSO_ATTACK2].firstFrame
            && frame < ci->animations[TORSO_ATTACK2].firstFrame + 6 )
    {
        return 1 + frame - ci->animations[TORSO_ATTACK2].firstFrame;
    }

    return 0;
}


/*
==============
CG_CalculateWeaponPosition
==============
*/
static void CG_CalculateWeaponPosition( vec3_t origin, vec3_t angles )
{
    float	scale;
    int		delta;
    float	fracsin;

    VectorCopy( cg.refdef.vieworg, origin );
    VectorCopy( cg.refdefViewAngles, angles );

    // on odd legs, invert some angles
    if ( cg.bobcycle & 1 )
    {
        scale = -cg.xyspeed;
    }
    else
    {
        scale = cg.xyspeed;
    }

    // gun angles from bobbing
    angles[ROLL] += scale * cg.bobfracsin * 0.005;
    angles[YAW] += scale * cg.bobfracsin * 0.01;
    angles[PITCH] += cg.xyspeed * cg.bobfracsin * 0.005;

    // drop the weapon when landing
    delta = cg.time - cg.landTime;
    if ( delta < LAND_DEFLECT_TIME )
    {
        origin[2] += cg.landChange*0.25 * delta / LAND_DEFLECT_TIME;
    }
    else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME )
    {
        origin[2] += cg.landChange*0.25 *
                     (LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta) / LAND_RETURN_TIME;
    }

#if 0
    // drop the weapon when stair climbing
    delta = cg.time - cg.stepTime;
    if ( delta < STEP_TIME/2 )
    {
        origin[2] -= cg.stepChange*0.25 * delta / (STEP_TIME/2);
    }
    else if ( delta < STEP_TIME )
    {
        origin[2] -= cg.stepChange*0.25 * (STEP_TIME - delta) / (STEP_TIME/2);
    }
#endif

    // idle drift
    scale = cg.xyspeed + 40;
    fracsin = sin( cg.time * 0.001 );
    angles[ROLL] += scale * fracsin * 0.01;
    angles[YAW] += scale * fracsin * 0.01;
    angles[PITCH] += scale * fracsin * 0.01;
}


/*
===============
CG_LightningBolt

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
The cent should be the non-predicted cent if it is from the player,
so the endpoint will reflect the simulated strike (lagging the predicted
angle)
===============
*/
static void CG_LightningBolt( centity_t *cent, vec3_t origin, int weapon )
{
    trace_t  trace;
    refEntity_t  beam;
    vec3_t   forward;
    vec3_t   muzzlePoint, endPoint;

    if ( weapon != WP_BLASER )
    {
        return;
    }

    memset( &beam, 0, sizeof( beam ) );

    // CPMA  "true" lightning
    if ((cent->currentState.number == cg.predictedPlayerState.clientNum) && (cg_trueLightning.value != 0))
    {
        vec3_t angle;
        int i;

        for (i = 0; i < 3; i++)
        {
            float a = cent->lerpAngles[i] - cg.refdefViewAngles[i];
            if (a > 180)
            {
                a -= 360;
            }
            if (a < -180)
            {
                a += 360;
            }

            angle[i] = cg.refdefViewAngles[i] + a * (1.0 - cg_trueLightning.value);
            if (angle[i] < 0)
            {
                angle[i] += 360;
            }
            if (angle[i] > 360)
            {
                angle[i] -= 360;
            }
        }

        AngleVectors(angle, forward, NULL, NULL );
        VectorCopy(cent->lerpOrigin, muzzlePoint );
//		VectorCopy(cg.refdef.vieworg, muzzlePoint );
    }
    else
    {
        // !CPMA
        AngleVectors( cent->lerpAngles, forward, NULL, NULL );
        VectorCopy(cent->lerpOrigin, muzzlePoint );
    }

    // FIXME: crouch
    muzzlePoint[2] += DEFAULT_VIEWHEIGHT;

    VectorMA( muzzlePoint, 14, forward, muzzlePoint );

    // project forward by the lightning range
    VectorMA( muzzlePoint, LIGHTNING_RANGE, forward, endPoint );

    // Don't draw if it hit water
    if ((trap_CM_PointContents (muzzlePoint, 0) & MASK_WATER) &&
            weapon != WP_BLASER)
        return;

    // see if it hit a wall
    CG_Trace( &trace, muzzlePoint, vec3_origin, vec3_origin, endPoint,
              cent->currentState.number, MASK_SHOT );

    // this is the endpoint
    VectorCopy( trace.endpos, beam.oldorigin );

    // use the provided origin, even though it may be slightly
    // different than the muzzle origin
    VectorCopy( origin, beam.origin );

    beam.reType = RT_LIGHTNING;
    beam.customShader = cgs.media.chargeLightShot1;
    trap_R_AddRefEntityToScene( &beam );

    // add the impact flare if it hit something
    if ( trace.fraction < 1.0 )
    {
        vec3_t	angles;
        vec3_t	dir;

        VectorSubtract( beam.oldorigin, beam.origin, dir );
        VectorNormalize( dir );

        memset( &beam, 0, sizeof( beam ) );
        beam.hModel = cgs.media.chargeLightHitM;

        VectorMA( trace.endpos, -16, dir, beam.origin );

        // make a random orientation
        angles[0] = genrand_int32() % 360;
        angles[1] = genrand_int32() % 360;
        angles[2] = genrand_int32() % 360;
        AnglesToAxis( angles, beam.axis );
        trap_R_AddRefEntityToScene( &beam );
    }
}


/*
===============
CG_SpawnRailTrail

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
===============
*/
static void CG_SpawnRailTrail( centity_t *cent, vec3_t origin )
{
    clientInfo_t	*ci;

    if ( !cent->pe.railgunFlash )
    {
        return;
    }
    cent->pe.railgunFlash = qtrue;
    ci = &cgs.clientinfo[ cent->currentState.clientNum ];
    CG_RailTrail( ci, origin, cent->pe.railgunImpact );
}



/*
=============
CG_AddPlayerWeapon

Used for both the view weapon (ps is valid) and the world modelother character models (ps is NULL)
The main player will have this called for BOTH cases, so effects like light and
sound should only be done on the world model case.
=============
*/
void CG_AddPlayerWeapon( refEntity_t *parent, playerState_t *ps, centity_t *cent, int team )
{
    refEntity_t	gun;
    refEntity_t	flash;
    refEntity_t	gravwell;
    vec3_t angles;
    weapon_t weaponNum;
    weaponr_t weaponNum2;
    weaponInfo_t *weapon;
    centity_t *nonPredictedCent;
    clientInfo_t *ci = &cgs.clientinfo[cent->currentState.clientNum];
    refEntity_t	blade;	//saber blades
    weaponNum  = bg_playerlist[CG_ClassforPlayer( ci->modelName, ci->skinName )].weapons[cent->currentState.weapon].weapon;

    CG_RegisterWeapon( weaponNum, cent->chargelevel );
    weapon = &cg_weapons[weaponNum];

    // add the weapon
    memset( &gun, 0, sizeof( gun ) );
    VectorCopy( parent->lightingOrigin, gun.lightingOrigin );
    gun.shadowPlane = parent->shadowPlane;
    gun.renderfx = parent->renderfx;

    // set custom shading for railgun refire rate
    if ( ps )
    {
        gun.shaderRGBA[0] = 255;
        gun.shaderRGBA[1] = 255;
        gun.shaderRGBA[2] = 255;
        gun.shaderRGBA[3] = 255;
    }

    if ( !ps )
    {
        // add weapon ready sound
        cent->pe.lightningFiring = qfalse;
        if ( ( cent->currentState.eFlags & EF_FIRING ) && weapon->firingSound )
        {
            // lightning gun and guantlet make a different sound when fire is held down
            trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->firingSound );

            if( cent->chargelevel > 2 )
                cent->pe.lightningFiring = qfalse;
            else
                cent->pe.lightningFiring = qtrue;

        }
        else if ( weapon->readySound )
        {
            trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->readySound );
        }
    }


    // draw the saber
    if( !Q_stricmp (ci->modelName, "zero_s") ) //if you ARE zero, show the saber
    {
        //hilt
        CG_PositionEntityOnTag( &gun, parent, parent->hModel, "tag_saber_h");
        gun.hModel = cgs.media.xbSaberModel;
        //gun.customShader = cgs.media.xbSaberhiltShader;
        gun.shaderRGBA[0] = 255;
        gun.shaderRGBA[1] = 255;
        gun.shaderRGBA[2] = 255;
        gun.shaderRGBA[3] = 255;
        gun.shadowPlane = parent->shadowPlane;
        gun.renderfx = parent->renderfx;
        trap_R_AddRefEntityToScene( &gun );
        //blade
        memset( &blade, 0, sizeof( blade ) );
        VectorCopy( parent->lightingOrigin, blade.lightingOrigin );
        blade.hModel = cgs.media.xbSaberbladeModel;
        //needs its own shader file
        blade.customShader = cgs.media.xbSaberbladeShader;
        blade.shaderRGBA[0] = 255;
        blade.shaderRGBA[1] = 255;
        blade.shaderRGBA[2] = 255;
        blade.shaderRGBA[3] = 255;
        blade.shadowPlane = parent->shadowPlane;
        blade.renderfx = parent->renderfx;
        CG_PositionEntityOnTag( &blade, &gun, gun.hModel, "tag_blade" );
        trap_R_AddRefEntityToScene( &blade );
    }
    /*
    	// add the spinning barrel
    	if ( weapon->barrelModel ) {
    		memset( &barrel, 0, sizeof( barrel ) );
    		VectorCopy( parent->lightingOrigin, barrel.lightingOrigin );
    		barrel.shadowPlane = parent->shadowPlane;
    		barrel.renderfx = parent->renderfx;

    		barrel.hModel = weapon->barrelModel;
    		angles[YAW] = 0;
    		angles[PITCH] = 0;
    		angles[ROLL] = CG_MachinegunSpinAngle( cent );
    		AnglesToAxis( angles, barrel.axis );

    		CG_PositionRotatedEntityOnTag( &barrel, &gun, weapon->weaponModel, "tag_barrel" );
    	}*/

    // make sure we aren't looking at cg.predictedPlayerEntity for LG
    nonPredictedCent = &cg_entities[cent->currentState.clientNum];

    // if the index of the nonPredictedCent is not the same as the clientNum
    // then this is a fake player (like on teh single player podiums), so
    // go ahead and use the cent
    if( ( nonPredictedCent - cg_entities ) != cent->currentState.clientNum )
    {
        nonPredictedCent = cent;
    }

    // add the flash
    if ( ( nonPredictedCent->currentState.eFlags & EF_FIRING ) )
    {
        // continuous flash
        if (cg.predictedPlayerState.weaponstate != WEAPON_FIRING)
            return;
    }
    else
    {
        // impulse flash
        if ( ( cg.time - cent->muzzleFlashTime > MUZZLE_FLASH_TIME && !cent->pe.railgunFlash ) ||
                (cg.predictedPlayerState.weaponstate != WEAPON_READY &&
                 cg.predictedPlayerState.weaponstate != WEAPON_FIRING))
        {
            return;
        }
    }

    memset( &flash, 0, sizeof( flash ) );
    VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
    flash.shadowPlane = parent->shadowPlane;
    flash.renderfx = parent->renderfx;

    weaponNum2 = bg_playerlist[CG_ClassforPlayer( ci->modelName, ci->skinName )].weapons[cent->currentState.weapon].weapon;
    // Peter FIXME: Lets make this class specific // DonX - Yes, and also weapon specific with charge
    switch ( weaponNum2 )
    {
    case WP_XBUSTER1:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = trap_R_RegisterShaderNoMip("muz_xshot_1");
        break;
    case WP_FCUTTER:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeFireHit;
        break;
    case WP_SICE:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeWaterHit;
        break;
    case WP_FBURST:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.busterWaterHit;
        break;
    case WP_AFIST:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeWindHit;
        break;
    case WP_BCRYSTAL:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeEarthHit;
        break;
    case WP_LTORPEDO:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeTechHit;
        break;
    case WP_TBURSTER:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeNatureHit;
        break;
    case WP_GBOMB:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeGravityHit;
        break;
    case WP_BLASER:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeLightHit;
        break;
    case WP_SFIST:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = cgs.media.chargeElectricHit;
        break;
        // UPGRADE WEAPS
    case WP_MBUSTER1:
    case WP_PBUSTER1:
    case WP_ZBUSTER1:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = trap_R_RegisterShaderNoMip("muz_xshot_2");
        break;
    case WP_XBUSTER2:
    case WP_MBUSTER2:
    case WP_PBUSTER2:
    case WP_ZBUSTER2:
        flash.hModel = cgs.media.xshot2FlashModel;
        flash.customShader = trap_R_RegisterShaderNoMip("muz_xshot_3");
        break;
    case WP_XBUSTER4:
        flash.hModel = cgs.media.xshot2FlashModel;
        flash.customShader = trap_R_RegisterShaderNoMip("muz_xshot_3");
    case WP_LTORPEDO2:
        flash.hModel = cgs.media.xshot2FlashModel;
        flash.customShader = cgs.media.chargeTechHit;
        break;
    case WP_LTORPEDO3:
        flash.hModel = cgs.media.xshot2FlashModel;
        flash.customShader = cgs.media.chargeTechHit;
        break;
        /*case WP_FCUTTER2:
        	;
        	break;*/
    case WP_TBURSTER2:
        flash.hModel = cgs.media.xshot2FlashModel;
        flash.customShader = cgs.media.chargeNatureHit;
        break;
    default:
        flash.hModel = cgs.media.xshotFlashModel;
        flash.customShader = trap_R_RegisterShaderNoMip("muz_xshot_1");
        break;
    }


    //WP_XBUSTER2,
    //WP_XBUSTER3,
    //WP_XBUSTER4,
    //WP_MBUSTER1,
    //WP_MBUSTER2,
    //WP_PBUSTER1,
    //WP_PBUSTER2,
    //WP_ZBUSTER,
    //WP_ZBUSTER1,
    //WP_ZBUSTER2,
    //WP_FCUTTER2,
    //WP_HWAVE2,
    //WP_SICE2,
    //WP_SICE3,
    //WP_AFIST2,
    //WP_BCRYSTAL2,
    //WP_BCRYSTAL3,
    //WP_BCRYSTAL4,
    //WP_LTORPEDO2,
    //WP_LTORPEDO3,
    //WP_TBURSTER2,
    //WP_GBOMB2,
    //WP_BLASER2,
    //WP_SFIST2,

    // put the special charged forms down here, so that they change the model

    if (!flash.hModel)
    {
        return;
    }
    angles[YAW] = 0;
    angles[PITCH] = 0;
    angles[ROLL] = crandom() * 10;
    AnglesToAxis( angles, flash.axis );

    // add the gravity well
    if ( weaponNum == WP_GRAVWELL )
    {
        memset( &gravwell, 0, sizeof( gravwell ) );
        VectorCopy( parent->lightingOrigin, gravwell.lightingOrigin );
        gravwell.shadowPlane = parent->shadowPlane;
        gravwell.renderfx = parent->renderfx;
        gravwell.hModel = cgs.media.gravwellModel;
        AnglesToAxis( angles, gravwell.axis );
        CG_PositionRotatedEntityOnTag( &gravwell, parent, parent->hModel, "tag_flash");
        trap_R_AddRefEntityToScene( &gravwell );
    }

    // fortes smoke cloud from buster - Peter
    if ( weaponNum == WP_SBURST || weaponNum == WP_RSTONE || weaponNum == WP_FCUTTER || weaponNum == WP_RISFIRE )
    {
        refEntity_t	smoke;
        localEntity_t	*fin_smoke;

        memset( &smoke, 0, sizeof( smoke ) );
        VectorCopy( parent->lightingOrigin, smoke.lightingOrigin );
        smoke.shadowPlane = parent->shadowPlane;
        smoke.renderfx = parent->renderfx;
        smoke.hModel = cgs.media.xshotFlashModel;
        smoke.customShader = cgs.media.hastePuffShader;
        CG_PositionRotatedEntityOnTag( &smoke, parent, parent->hModel, "tag_flash");
        fin_smoke = CG_SmokePuff( smoke.origin, vec3_origin,
                                  32,
                                  1, 1, 1, 0.33f,
                                  1000,
                                  cg.time, 0,
                                  LEF_PUFF_DONT_SCALE,
                                  cgs.media.hastePuffShader );
    }

    CG_PositionRotatedEntityOnTag( &flash, parent, parent->hModel, "tag_flash");
    trap_R_AddRefEntityToScene( &flash );

    if ( ps || cg.renderingThirdPerson ||
            cent->currentState.number != cg.predictedPlayerState.clientNum )
    {
        // add lightning bolt
        //CG_Printf( NULL,"lightningfiring is %i\n", nonPredictedCent->pe.lightningFiring );
        CG_LightningBolt( nonPredictedCent, flash.origin, weaponNum );

        // add rail trail
        CG_SpawnRailTrail( cent, flash.origin );

        if ( weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2] )
        {
            trap_R_AddLightToScene( flash.origin, 300 + (genrand_int32()&31), weapon->flashDlightColor[0],
                                    weapon->flashDlightColor[1], weapon->flashDlightColor[2] );
        }
    }
}

/*
==============
CG_AddViewWeapon

Add the weapon, and flash for the player's view
==============
*/
void CG_AddViewWeapon( playerState_t *ps )
{
    refEntity_t	hand;
    centity_t	*cent;
    clientInfo_t	*ci;
    float		fovOffset;
    vec3_t		angles;
    weaponInfo_t	*weapon;

    int weap = bg_playerlist[ps->persistant[PLAYERCLASS]].weapons[ps->weapon].weapon;;

    if ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR )
    {
        return;
    }

    if ( ps->pm_type == PM_INTERMISSION )
    {
        return;
    }

    // no gun if in third person view or a camera is active
    //if ( cg.renderingThirdPerson || cg.cameraMode) {
    if ( cg.renderingThirdPerson )
    {
        return;
    }

    if ( cg.predictedPlayerState.eFlags & EF_FIRING )
    {
        // special hack for lightning gun...
        VectorCopy( cg.refdef.vieworg, angles );
        VectorMA( angles, -8, cg.refdef.viewaxis[2], angles );
        //CG_Printf( NULL,"lightningfiring is %i\n", &cg_entities[ps->clientNum].pe.lightningFiring );
        CG_LightningBolt( &cg_entities[ps->clientNum], angles, weap );
    }
    return;


    // allow the gun to be completely removed
    if ( !cg_drawGun.integer )
    {
        vec3_t		origin;

        if ( cg.predictedPlayerState.eFlags & EF_FIRING )
        {
            // special hack for lightning gun...
            VectorCopy( cg.refdef.vieworg, origin );
            VectorMA( origin, -8, cg.refdef.viewaxis[2], origin );
            CG_LightningBolt( &cg_entities[ps->clientNum], origin, weap );
        }
        return;
    }

    // don't draw if testing a gun model
    if ( cg.testGun )
    {
        return;
    }

    // drop gun lower at higher fov
    if ( cg_fov.integer > 90 )
    {
        fovOffset = -0.2 * ( cg_fov.integer - 90 );
    }
    else
    {
        fovOffset = 0;
    }

    cent = &cg.predictedPlayerEntity;	// &cg_entities[cg.snap->ps.clientNum];
    CG_RegisterWeapon( weap, cent->chargelevel );
    weapon = &cg_weapons[ weap ];

    memset (&hand, 0, sizeof(hand));

    // set up gun position
    CG_CalculateWeaponPosition( hand.origin, angles );

    VectorMA( hand.origin, cg_gun_x.value, cg.refdef.viewaxis[0], hand.origin );
    VectorMA( hand.origin, cg_gun_y.value, cg.refdef.viewaxis[1], hand.origin );
    VectorMA( hand.origin, (cg_gun_z.value+fovOffset), cg.refdef.viewaxis[2], hand.origin );

    AnglesToAxis( angles, hand.axis );

    // map torso animations to weapon animations
    if ( cg_gun_frame.integer )
    {
        // development tool
        hand.frame = hand.oldframe = cg_gun_frame.integer;
        hand.backlerp = 0;
    }
    else
    {
        // get clientinfo for animation map
        ci = &cgs.clientinfo[ cent->currentState.clientNum ];
        hand.frame = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.frame );
        hand.oldframe = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.oldFrame );
        hand.backlerp = cent->pe.torso.backlerp;
    }

    hand.hModel = weapon->handsModel;
    hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON | RF_MINLIGHT;

    // add everything onto the hand
    CG_AddPlayerWeapon( &hand, ps, &cg.predictedPlayerEntity, ps->persistant[PERS_TEAM] );
}

/*
==============================================================================

WEAPON SELECTION

==============================================================================
*/


/*
===================
CG_DrawWeaponSelect
===================
*/

void CG_DrawWeaponSelect( void )
{
    int		i;
    int		bits;
    int		count;
    int		x, y, w;
    char	*name;
    float	*color;
    int		weap;

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
    for ( i = 0 ; i < 16 ; i++ )
    {
        if ( bits & ( 1 << i ) )
        {
            count++;
        }
    }

    x = 320 - count * 20;
    y = 380;

    for ( i = 0 ; i < 16 ; i++ )
    {
        if ( !( bits & ( 1 << i ) ) )
        {
            continue;
        }
        //added
        weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[i].weapon;

        CG_RegisterWeapon( weap, cg.snap->ps.stats[STAT_CHARGE] );

        // draw weapon icon
        CG_DrawPic( x, y, 32, 32, cg_weapons[/*i*/weap].weaponIcon );

        // draw selection marker
        if ( i == cg.weaponSelect )
        {
            CG_DrawPic( x-4, y-4, 40, 40, cgs.media.selectShader );
        }

        // no ammo cross on top
        if ( !cg.snap->ps.ammo[i] )
        {
            CG_DrawPic( x, y, 32, 32, cgs.media.noammoShader );
        }

        x += 40;
    }

    // draw the selected name
    weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[cg.weaponSelect].weapon;;
    if ( cg_weapons[ weap ].item )
    {
        name = cg_weapons[ weap ].item->pickup_name;
        if ( name )
        {
            w = CG_DrawStrlen( name ) * BIGCHAR_WIDTH;
            x = ( SCREEN_WIDTH - w ) / 2;
            CG_DrawBigStringColor(x, y - 22, name, color);
        }
    }

    trap_R_SetColor( NULL );
}


/*
===============
CG_WeaponSelectable
===============
*/
static qboolean CG_WeaponSelectable( int i )
{
    if ( !cg.snap->ps.ammo[i] )
    {
        return qfalse;
    }
    if ( ! (cg.snap->ps.stats[ STAT_WEAPONS ] & ( 1 << i ) ) )
    {
        return qfalse;
    }

    return qtrue;
}

/*
===============
CG_NextWeapon_f
===============
*/
void CG_NextWeapon_f( void )
{
    int		i;
    int		original;

    if ( !cg.snap )
    {
        return;
    }
    if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
    {
        return;
    }

    cg.weaponSelectTime = cg.time;
    original = cg.weaponSelect;

    for ( i = 0 ; i < 16 ; i++ )
    {
        cg.weaponSelect++;
        if ( cg.weaponSelect == 16 )
        {
            cg.weaponSelect = 0;
        }
        if ( CG_WeaponSelectable( cg.weaponSelect ) )
        {
            break;
        }
    }
    if ( i == 16 )
    {
        cg.weaponSelect = original;
    }
}

/*
===============
CG_PrevWeapon_f
===============
*/
void CG_PrevWeapon_f( void )
{
    int		i;
    int		original;

    if ( !cg.snap )
    {
        return;
    }
    if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
    {
        return;
    }

    cg.weaponSelectTime = cg.time;
    original = cg.weaponSelect;

    for ( i = 0 ; i < 16 ; i++ )
    {
        cg.weaponSelect--;
        if ( cg.weaponSelect == -1 )
        {
            cg.weaponSelect = 16;
        }
        if ( CG_WeaponSelectable( cg.weaponSelect ) )
        {
            break;
        }
    }
    if ( i == 16 )
    {
        cg.weaponSelect = original;
    }
}

/*
===============
CG_Weapon_f
===============
*/

void CG_Weapon_f( void )
{
    int		num;

    if ( !cg.snap )
    {
        return;
    }
    if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
    {
        return;
    }

    num = atoi( CG_Argv( 1 ) );

    if ( num < 0 || num > 16 )
    {
        return;
    }

    cg.weaponSelectTime = cg.time;

    if ( ! ( cg.snap->ps.stats[STAT_WEAPONS] & ( 1 << num ) ) )
    {
        return;		// don't have the weapon
    }

    cg.weaponSelect = num;
}

/*
===================
CG_OutOfAmmoChange

The current weapon has just run out of ammo
===================
*/
void CG_OutOfAmmoChange( void )
{
    int		i;

    cg.weaponSelectTime = cg.time;

    for ( i = 16 ; i > 0 ; i-- )
    {
        if ( CG_WeaponSelectable( i ) )
        {
            cg.weaponSelect = i;
            break;
        }
    }
}



/*
===================================================================================================

WEAPON EVENTS

===================================================================================================
*/

/*
================
CG_FireWeapon

Caused by an EV_FIRE_WEAPON event
================
*/
void CG_FireWeapon( centity_t *cent )
{
    entityState_t *ent;
    int				c;
    weaponInfo_t	*weap;
    clientInfo_t	*ci = &cgs.clientinfo[cent->currentState.clientNum];
    int weapindex;

    ent = &cent->currentState;
    weapindex = bg_playerlist[CG_ClassforPlayer( ci->modelName, ci->skinName )].weapons[ent->weapon].weapon;

    if ( ent->weapon == WP_NONE )
    {
        return;
    }
    if ( ent->weapon >= WP_NUM_WEAPONS )
    {
        CG_Error( "CG_FireWeapon: ent->weapon >= WP_NUM_WEAPONS" );
        return;
    }

    weap = &cg_weapons[ weapindex ];
    // mark the entity as muzzle flashing, so when it is added it will
    // append the flash to the weapon model
    cent->muzzleFlashTime = cg.time;

    // blaser only does this this on initial press
    if ( weapindex == WP_BLASER )
    {
        if ( cent->pe.lightningFiring )
        {
            return;
        }
    }

    // play a sound
    for ( c = 0 ; c < 4 ; c++ )
    {
        if ( !weap->flashSound[c] )
        {
            break;
        }
    }

    if( cent->chargelevel )
    {
        switch( weapindex )
        {
        case WP_XBUSTER:
            if( cent->chargelevel >= 3 )
                trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, trap_S_RegisterSound( "sound/xbuster/weapons/weapfire_blast4_helix1.wav", qfalse ) );
            else if( cent->chargelevel >= 1 )
                trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, trap_S_RegisterSound( "sound/player/mmx/chargefire2.wav", qfalse ) );
            break;
        case WP_MBUSTER:
            trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, trap_S_RegisterSound( "sound/player/mmx/chargefire2.wav", qfalse ) );
            break;
        case WP_PBUSTER:
            trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, trap_S_RegisterSound( "sound/player/mmx/chargefire2.wav", qfalse ) );
            break;
        default:
            if ( c > 0 )
            {
                c = genrand_int32() % c;
                if ( weap->flashSound[c] )
                {
                    trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound[c] );
                }
            }
            break;
        }
    }
    else if ( c > 0 )
    {
        c = genrand_int32() % c;
        if ( weap->flashSound[c] )
        {
            trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound[c] );
        }
    }
    if( weapindex != WP_FCUTTER ) // we don't want the fcutter to reset this way, but rather when you stop firing
    {
        cent->chargelevel = 0;
    }
    // do brass ejection
    if ( weap->ejectBrassFunc && cg_brassTime.integer > 0 )
    {
        weap->ejectBrassFunc( cent );
    }
}

/*
================
CG_FireOffhand

Caused by an EV_FIRE_OFFHAND event
================
*/
void CG_FireOffhand( centity_t *cent )
{
    clientInfo_t	*ci = &cgs.clientinfo[cent->currentState.clientNum];

    // mark the entity as muzzle flashing, so when it is added it will
    // append the flash to the weapon model
    cent->muzzleFlashTime = cg.time;

    if( CG_ClassforPlayer( ci->modelName, ci->skinName ) == CLASS_DWNINFINITY )
    {
        trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, trap_S_RegisterSound( "sound/player/mmx/chargefire2.wav", qfalse ) );
    }
}

/*
================
CG_ExplosionParticles

Caused by a missile hitting a wall
================
*/
void CG_ExplosionParticles( int weapon, vec3_t origin )
{
    int number; // number of particles
    int jump; // amount to nudge the particles trajectory vector up by
    int speed; // speed of particles
    int light; // amount of light for each particle
    vec4_t lColor; // color of light for each particle
    qhandle_t shader; // shader to use for the particles
    int index, radius;
    vec3_t randVec, tempVec;

    // set defaults
    number = 32;
    jump = 50;
    speed = 300;
    light = 50;
    radius = 3;
    lColor[0] = 1.0f;
    lColor[1] = 1.0f;
    lColor[2] = 1.0f;
    lColor[3] = 1.0f; // alpha

    switch( weapon )
    {
    case -2: //exploding player
        number = 600;
        jump = 0;
        light = 700;
        lColor[0] = 1.0f;
        lColor[1] = 1.0f;
        lColor[2] = 0.0f;
        shader = cgs.media.hitSparkShader;
        break;
    case -1: //when player gets hit
        number = 32;
        jump = 30;
        light = 100;
        lColor[0] = 1.0f;
        lColor[1] = 1.0f;
        lColor[2] = 0.0f;
        shader = cgs.media.hitSparkShader;
        break;
        //added weapons
    case WP_XBUSTER:
    case WP_XBUSTER3:
    case WP_XBUSTER4:
    case WP_MBUSTER:
    case WP_PBUSTER:
    case WP_ZBUSTER:
    case WP_FBUSTER:
    case WP_ZSABER:
    case WP_BCRYSTAL:
    case WP_BSHIELD:
    case WP_TRTHUNDER:
        number = 32;
        jump = 30;
        light = 100;
        lColor[0] = 1.0f;
        lColor[1] = 0.56f;
        lColor[2] = 0.0f;
        shader = cgs.media.hitSparkShader;
        break;
    case WP_MBUSTER1:
    case WP_PBUSTER1:
    case WP_XBUSTER1:
    case WP_ZBUSTER1:
    case WP_BLAUNCHER:
    case WP_STRIKECHAIN:
    case WP_DSHOT:
        number = 50;
        jump = 70;
        light = 150;
        lColor[0] = 1.0f;
        lColor[1] = 0.56f;
        lColor[2] = 0.0f;
        shader = cgs.media.hitSparkShader;
        break;
    case WP_MBUSTER2:
    case WP_PBUSTER2:
    case WP_XBUSTER2:
    case WP_ZBUSTER2:
    case WP_LTORPEDO:
    case WP_LTORPEDO2:
    case WP_LTORPEDO3:
        number = 64;
        jump = 70;
        light = 200;
        lColor[0] = 1.0f;
        lColor[1] = 0.56f;
        lColor[2] = 0.0f;
        shader = cgs.media.hitSparkShader;
        break;
//special freaks
    case WP_SICE3:
        number = 2;
        jump = 70;
        light = 200;
        lColor[0] = 1.0f;
        lColor[1] = 0.56f;
        lColor[2] = 0.0f;
        shader = cgs.media.busterWaterShot;
        break;
    case WP_RSTONE:
        number = 2;
        jump = 70;
        light = 200;
        lColor[0] = 1.0f;
        lColor[1] = 0.56f;
        lColor[2] = 0.0f;
        shader = cgs.media.hitSparkShader;
        break;
    case WP_SBURST:
        number = 4;
        jump = 70;
        light = 200;
        lColor[0] = 1.0f;
        lColor[1] = 0.56f;
        lColor[2] = 0.0f;
        shader = cgs.media.hitSparkShader;
        break;
    case WP_ASPLASHER:
        number = 4;
        jump = 70;
        light = 200;
        lColor[0] = 1.0f;
        lColor[1] = 0.56f;
        lColor[2] = 0.0f;
        radius = 10;
        shader = cgs.media.rapidNatureShot2;
        break;
    default:
        return;
    }
//end new weapons
    for( index = 0; index < number; index++ )
    {
        localEntity_t *le;
        refEntity_t *re;

        le = CG_AllocLocalEntity(); //allocate a local entity
        re = &le->refEntity;
        le->leFlags = LEF_PUFF_DONT_SCALE; //don't change the particle size
        le->leType = LE_MOVE_SCALE_FADE; // particle should fade over time
        le->startTime = cg.time; // set the start time of the particle to the current time
        le->endTime = cg.time + 3000 + random() * 250; //set the end time
        le->lifeRate = 1.0 / ( le->endTime - le->startTime );
        re = &le->refEntity;
        re->shaderTime = cg.time / 1000.0f;
        re->reType = RT_SPRITE;
        re->rotation = 0;
        re->radius = radius;
        re->customShader = shader;
        re->shaderRGBA[0] = 0xff;
        re->shaderRGBA[1] = 0xff;
        re->shaderRGBA[2] = 0xff;
        re->shaderRGBA[3] = 0xff;
        le->light = light;
        VectorCopy( lColor, le->lightColor );
        le->color[3] = 1.0;
        le->pos.trType = TR_GRAVITY; // moves according to gravity
        le->pos.trTime = cg.time;
        VectorCopy( origin, le->pos.trBase );
        VectorCopy( origin, re->origin );

        tempVec[0] = crandom(); //between 1 and -1
        tempVec[1] = crandom();
        tempVec[2] = crandom();
        VectorNormalize(tempVec);
        VectorScale(tempVec, speed, randVec);
        randVec[2] += jump; //nudge the particles up a bit
        VectorCopy( randVec, le->pos.trDelta );
    }
}

/*
=================
CG_MissileHitWall

Caused by an EV_MISSILE_MISS event, or directly by local bullet tracing
=================
*/
void CG_MissileHitWall( int weapon, int clientNum, vec3_t origin, vec3_t dir, impactSound_t soundType )
{
    qhandle_t		mod;
    qhandle_t		mark;
    qhandle_t		shader, sphereshader;
    sfxHandle_t		sfx;
    float			radius, sphereradius;
    float			light;
    vec3_t			lightColor;
    localEntity_t	*le;
    localEntity_t	*explosion;
    int				r;
    qboolean		alphaFade;
    qboolean		isSprite;
    int				duration, sphereduration;

    mark = 0;
    radius = 32;
    sphereradius = 0;
    sphereshader = cgs.media.xbExplosionShader1;
    sphereduration = 400;
    sfx = 0;
    mod = 0;
    shader = 0;
    light = 0;
    lightColor[0] = 1;
    lightColor[1] = 1;
    lightColor[2] = 0;

    // set defaults
    isSprite = qtrue;
    duration = 800;

    switch ( weapon )
    {
    case WP_XBUSTER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShotShader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 400;
        light = 50;
        radius = 25;
        lightColor[0] = 1.0f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.75f;
        break;
    case WP_XBUSTER1:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot1Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionGreenShader;
        sphereduration = 600;
        light = 100;
        radius = 80;
        lightColor[0] = 0.5f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.5f;
        break;
    case WP_XBUSTER2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot2Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 800;
        light = 200;
        radius = 100;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        break;
    case WP_XBUSTER3:
    case WP_XBUSTER4:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot3Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionPurpleShader;
        sphereduration = 600;
        light = 100;
        radius = 50;
        lightColor[0] = 1.0f;
        lightColor[1] = 0.5f;
        lightColor[2] = 1.0f;
        break;
    case WP_FBUSTER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.bassShotShader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionPurpleShader;
        sphereduration = 700;
        light = 100;
        radius = 60;
        lightColor[0] = 0.94f;
        lightColor[1] = 0;
        lightColor[2] = 0.94f;
        break;
    case WP_MBUSTER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShotShader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 400;
        light = 50;
        radius = 15;
        lightColor[0] = 1.0f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.75f;
        break;
    case WP_MBUSTER1:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot1Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionGreenShader;
        sphereduration = 400;
        light = 100;
        radius = 40;
        lightColor[0] = 0.5f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.5f;
        break;
    case WP_MBUSTER2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot2Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 800;
        light = 200;
        radius = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        break;
    case WP_PBUSTER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShotShader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 300;
        light = 50;
        radius = 10;
        lightColor[0] = 1.0f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.75f;
        break;
    case WP_PBUSTER1:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot1Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionGreenShader;
        sphereduration = 400;
        light = 100;
        radius = 30;
        lightColor[0] = 0.5f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.5f;
        break;
    case WP_PBUSTER2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot2Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 800;
        light = 200;
        radius = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        break;
    case WP_ZBUSTER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShotShader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 300;
        light = 50;
        radius = 10;
        lightColor[0] = 1.0f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.75f;
        break;
    case WP_ZBUSTER1:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot1Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionGreenShader;
        sphereduration = 600;
        light = 100;
        radius = 50;
        lightColor[0] = 0.5f;
        lightColor[1] = 1.0f;
        lightColor[2] = 0.5f;
        break;
    case WP_ZBUSTER2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.xShot2Shader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 800;
        light = 200;
        radius = 100;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        break;
        //fire
    case WP_FCUTTER:
    case WP_FCUTTER2:
        //sfx = cgs.media.sfx_rockexp;
        mark = cgs.media.burnMarkShader;
        //shader = cgs.media.chargeFireHit;
        radius = 20;
        light = 100;
        lightColor[0] = 1.0f;
        lightColor[1] = 0.75f;
        lightColor[2] = 0;
        break;
    case WP_RISFIRE:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterFireHit;
        sfx = cgs.media.sfx_rockexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 600;
        radius = 64;
        light = 100;
        lightColor[0] = 1.0f;
        lightColor[1] = 0.75f;
        lightColor[2] = 0;
        break;
    case WP_HWAVE:
        //mod = trap_R_RegisterModel( "weapons/fire/fire_up.md3" );
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterFireHit;
        sfx = cgs.media.sfx_rockexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 800;
        radius = 100;
        light = 100;
        lightColor[0] = 1.0f;
        lightColor[1] = 0.75f;
        lightColor[2] = 0;
        break;
    case WP_HWAVE2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterFireHit;
        sfx = cgs.media.sfx_rockexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 600;
        radius = 0;
        light = 100;
        lightColor[0] = 1.0f;
        lightColor[1] = 0.75f;
        lightColor[2] = 0;
        break;
    case WP_PSTRIKE:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidFireHit;
        sfx = cgs.media.sfx_rockexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionRedShader;
        sphereduration = 500;
        radius = 50;
        light = 100;
        lightColor[0] = 1.0f;
        lightColor[1] = 0.75f;
        lightColor[2] = 0;
        break;
        //water
    case WP_SICE:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeWaterHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 600;
        light = 100;
        radius = 100;
        lightColor[0] = 0.82f;
        lightColor[1] = 0.82f;
        lightColor[2] = 1.0f;
        break;
    case WP_SICE2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeWaterHit2;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 300;
        light = 100;
        radius = 0;
        lightColor[0] = 0.82f;
        lightColor[1] = 0.82f;
        lightColor[2] = 1.0f;
        break;
    case WP_SICE3:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeWaterHit3;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 300;
        light = 100;
        radius = 32;
        lightColor[0] = 0.82f;
        lightColor[1] = 0.82f;
        lightColor[2] = 1.0f;
        break;
    case WP_FBURST:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterWaterHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 300;
        light = 100;
        radius = 16;
        lightColor[0] = 0.82f;
        lightColor[1] = 0.82f;
        lightColor[2] = 1.0f;
        break;
    case WP_IBURST:
        break;
    case WP_WSTRIKE:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidWaterHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 500;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40;
        break;
        //air
    case WP_AFIST:
    case WP_AFIST2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeWindHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 600;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 100;
        break;
    case WP_AWAVE:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterWindHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 800;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 50;
        break;
    case WP_TEMPEST:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidWindHit;
        sfx = cgs.media.sfx_Tempest1exp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionWhiteShader;
        sphereduration = 600;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40;
        break;
        //earth
    case WP_BCRYSTAL:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeEarthHit;
        sfx = cgs.media.sfx_Crystal1exp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 600;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 60;
        break;
    case WP_BCRYSTAL2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeEarthHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40;
        break;
    case WP_BCRYSTAL3:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeEarthHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40;
        break;
    case WP_BCRYSTAL4:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeEarthHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40;
        break;
    case WP_RSTONE:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterEarthHit;
        sfx = cgs.media.sfx_Bang4exp;
        mark = cgs.media.energyMarkShader;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 0;
        break;
    case WP_SBURST:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidEarthHit;
        sfx = cgs.media.sfx_Bang4exp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 700;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 50;
        break;
        //metal
    case WP_LTORPEDO:
    case WP_LTORPEDO2:
    case WP_LTORPEDO3:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeTechHit;
        sfx = cgs.media.sfx_rockexp;
        mark = cgs.media.burnMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 800;
        radius = 100;
        light = 300;
        duration = 1000;
        lightColor[0] = 1;
        lightColor[1] = 0.75;
        lightColor[2] = 0.0;
        break;
    case WP_BLAUNCHER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterTechHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionRedShader;
        sphereduration = 600;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 60;
        break;
    case WP_STRIKECHAIN:
        break;
    case WP_DSHOT:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidTechHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.crackMarkShader;
        sphereshader = cgs.media.xbExplosionGreyShader;
        sphereduration = 500;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40;
        break;
    case WP_IMBOOMERANG:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.sheildTechHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.crackMarkShader;
        sphereshader = cgs.media.xbExplosionBlueShader;
        sphereduration = 500;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40;
        break;
        //nature
    case WP_TBURSTER:
    case WP_TBURSTER2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeNatureHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionGreenShader;
        sphereduration = 500;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 40; //24 original
        break;
    case WP_LSTORM:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterNatureHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionGreenShader;
        sphereduration = 200;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 24;
        break;
    case WP_ASPLASHER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidNatureHit;
        sfx = cgs.media.sfx_Acid2exp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionGreenShader;
        sphereduration = 600;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 60;
        break;
        //gravity
    case WP_GBOMB:
    case WP_GBOMB2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeGravityHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionPurpleShader;
        sphereduration = 600;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 50;
        break;
    case WP_GRAVWELL:
        break;
    case WP_DMATTER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidGravityHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionBlackShader;
        sphereduration = 300;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 0;
        break;
        //light
    case WP_BLASER:
    case WP_BLASER2:
        // no explosion at LG impact, it is added with the beam
        r = genrand_int32() & 3;
        if ( r < 2 )
        {
            sfx = cgs.media.sfx_lghit2;
        }
        else if ( r == 2 )
        {
            sfx = cgs.media.sfx_lghit1;
        }
        else
        {
            sfx = cgs.media.sfx_lghit3;
        }
        mark = cgs.media.holeMarkShader;
        radius = 12;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        break;
        /*case WP_SSHREDDER:
         mod = cgs.media.ringFlashModel;
         shader = cgs.media.busterLightHit;
         sfx = cgs.media.sfx_plasmaexp;
         mark = cgs.media.energyMarkShader;
         light = 50;
         lightColor[0] = 0.38f;
         lightColor[1] = 0.38f;
         lightColor[2] = 1.0f;
         radius = 24;
         break;*/
    case WP_NBURST:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.rapidLightHit;
        sfx = cgs.media.sfx_Bang3exp;
        mark = cgs.media.energyMarkShader;
        sphereshader = cgs.media.xbExplosionYellowShader;
        sphereduration = 1000;
        light = 800;
        lightColor[0] = 1.0f;
        lightColor[1] = 1.0f;
        lightColor[2] = 1.0f;
        radius = 200;
        break;
        //electrical
    case WP_SFIST:
    case WP_SFIST2:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.chargeElectricHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 24;
        break;
        /*case WP_BSHIELD:
         mod = cgs.media.ringFlashModel;
         shader = cgs.media.chargeElectricHit;
         sfx = cgs.media.sfx_plasmaexp;
         mark = cgs.media.energyMarkShader;
         light = 50;
         lightColor[0] = 0.38f;
         lightColor[1] = 0.38f;
         lightColor[2] = 1.0f;
         radius = 24;
         break;
        case WP_BLIGHTNING:
         mod = cgs.media.ringFlashModel;
         shader = cgs.media.rapidElectricHit;
         sfx = cgs.media.sfx_plasmaexp;
         mark = cgs.media.energyMarkShader;
         light = 50;
         lightColor[0] = 0.38f;
         lightColor[1] = 0.38f;
         lightColor[2] = 1.0f;
         radius = 24;
         break;*/
    case WP_TRTHUNDER:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.sheildElectricHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 24;
        break;
    case WP_MMINE:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.busterGravityHit;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        light = 50;
        lightColor[0] = 0.38f;
        lightColor[1] = 0.38f;
        lightColor[2] = 1.0f;
        radius = 50;
        break;
    default:
        mod = cgs.media.ringFlashModel;
        shader = cgs.media.plasmaExplosionShader;
        sfx = cgs.media.sfx_plasmaexp;
        mark = cgs.media.energyMarkShader;
        light = 100;
        radius = 16;
        lightColor[0] = 0.82f;
        lightColor[1] = 0.82f;
        lightColor[2] = 1.0f;
        break;
    }

    if( radius )
        sphereradius = radius;

    if ( sfx )
    {
        trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, sfx );
    }

    //
    // create the explosion
    //
    if ( mod )
    {
        le = CG_MakeExplosion( origin, dir,
                               mod,	shader,
                               duration, isSprite );
        le->light = light;
        VectorCopy( lightColor, le->lightColor );

        if( sphereradius )
            explosion = CG_MakeSphericalExplosion( origin, sphereradius, cgs.media.xbExplosionSphere, sphereshader, LEF_PUFF_FADE_RGB, sphereduration );
    }

    //
    // impact mark
    //
    alphaFade = (mark == cgs.media.energyMarkShader);	// plasma fades alpha, all others fade color
    if (radius )
    {
        CG_ImpactMark( mark, origin, dir, random()*360, 1,1,1,1, alphaFade, radius, qfalse );
    }
    // UNiSOL - GoGo smoke puffs..
    {
        vec3_t vel;

        VectorCopy( dir, vel );
        VectorScale( vel, 10, vel );
        vel[2] = 15 + crandom()*15;
        CG_SmokePuff( origin, vel, 15, 1, 1, 1, 0.4f + crandom()*0.3, 1000 + crandom()*1000, cg.time, 0, 0, cgs.media.smokePuffShader );
    }
    // End
    CG_ExplosionParticles( weapon, origin ); //added for particle effects

    // NIGHTZ - Shake Screen
    CG_ShakeScreen (origin, cg.snap->ps.origin);
}

/*
=================
CG_MissileHitPlayer
=================
*/
void CG_MissileHitPlayer( int weapon, vec3_t origin, vec3_t dir, int entityNum )
{

    // Peter: Lets save this just incase we want different effects
    // when the player is hit with a weapon
}



/*
============================================================================

BULLETS

============================================================================
*/


/*
===============
CG_Tracer
===============
*/
void CG_Tracer( vec3_t source, vec3_t dest )
{
    vec3_t		forward, right;
    polyVert_t	verts[4];
    vec3_t		line;
    float		len, begin, end;
    vec3_t		start, finish;
    vec3_t		midpoint;

    // tracer
    VectorSubtract( dest, source, forward );
    len = VectorNormalize( forward );

    // start at least a little ways from the muzzle
    if ( len < 100 )
    {
        return;
    }
    begin = 50 + random() * (len - 60);
    end = begin + cg_tracerLength.value;
    if ( end > len )
    {
        end = len;
    }
    VectorMA( source, begin, forward, start );
    VectorMA( source, end, forward, finish );

    line[0] = DotProduct( forward, cg.refdef.viewaxis[1] );
    line[1] = DotProduct( forward, cg.refdef.viewaxis[2] );

    VectorScale( cg.refdef.viewaxis[1], line[1], right );
    VectorMA( right, -line[0], cg.refdef.viewaxis[2], right );
    VectorNormalize( right );

    VectorMA( finish, cg_tracerWidth.value, right, verts[0].xyz );
    verts[0].st[0] = 0;
    verts[0].st[1] = 1;
    verts[0].modulate[0] = 255;
    verts[0].modulate[1] = 255;
    verts[0].modulate[2] = 255;
    verts[0].modulate[3] = 255;

    VectorMA( finish, -cg_tracerWidth.value, right, verts[1].xyz );
    verts[1].st[0] = 1;
    verts[1].st[1] = 0;
    verts[1].modulate[0] = 255;
    verts[1].modulate[1] = 255;
    verts[1].modulate[2] = 255;
    verts[1].modulate[3] = 255;

    VectorMA( start, -cg_tracerWidth.value, right, verts[2].xyz );
    verts[2].st[0] = 1;
    verts[2].st[1] = 1;
    verts[2].modulate[0] = 255;
    verts[2].modulate[1] = 255;
    verts[2].modulate[2] = 255;
    verts[2].modulate[3] = 255;

    VectorMA( start, cg_tracerWidth.value, right, verts[3].xyz );
    verts[3].st[0] = 0;
    verts[3].st[1] = 0;
    verts[3].modulate[0] = 255;
    verts[3].modulate[1] = 255;
    verts[3].modulate[2] = 255;
    verts[3].modulate[3] = 255;

    trap_R_AddPolyToScene( cgs.media.tracerShader, 4, verts );

    midpoint[0] = ( start[0] + finish[0] ) * 0.5;
    midpoint[1] = ( start[1] + finish[1] ) * 0.5;
    midpoint[2] = ( start[2] + finish[2] ) * 0.5;

    // add the tracer sound
    trap_S_StartSound( midpoint, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.tracerSound );

}


/*
======================
CG_CalcMuzzlePoint
======================
*/
static qboolean	CG_CalcMuzzlePoint( int entityNum, vec3_t muzzle )
{
    vec3_t		forward;
    centity_t	*cent;
    int			anim;

    if ( entityNum == cg.snap->ps.clientNum )
    {
        VectorCopy( cg.snap->ps.origin, muzzle );
        muzzle[2] += cg.snap->ps.viewheight;
        AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
        VectorMA( muzzle, 14, forward, muzzle );
        return qtrue;
    }

    cent = &cg_entities[entityNum];
    if ( !cent->currentValid )
    {
        return qfalse;
    }

    VectorCopy( cent->currentState.pos.trBase, muzzle );

    AngleVectors( cent->currentState.apos.trBase, forward, NULL, NULL );
    anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
    muzzle[2] += DEFAULT_VIEWHEIGHT;

    VectorMA( muzzle, 14, forward, muzzle );

    return qtrue;

}

/*
======================
CG_Bullet

Renders bullet effects.
======================
*/
void CG_Bullet( vec3_t end, int sourceEntityNum, vec3_t normal, qboolean flesh, int fleshEntityNum )
{
    trace_t trace;
    int sourceContentType, destContentType;
    vec3_t		start;

    // if the shooter is currently valid, calc a source point and possibly
    // do trail effects
    if ( sourceEntityNum >= 0 && cg_tracerChance.value > 0 )
    {
        if ( CG_CalcMuzzlePoint( sourceEntityNum, start ) )
        {
            sourceContentType = trap_CM_PointContents( start, 0 );
            destContentType = trap_CM_PointContents( end, 0 );

            // do a complete bubble trail if necessary
            if ( ( sourceContentType == destContentType ) && ( sourceContentType & CONTENTS_WATER ) )
            {
                CG_BubbleTrail( start, end, 32 );
            }
            // bubble trail from water into air
            else if ( ( sourceContentType & CONTENTS_WATER ) )
            {
                trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
                CG_BubbleTrail( start, trace.endpos, 32 );
            }
            // bubble trail from air into water
            else if ( ( destContentType & CONTENTS_WATER ) )
            {
                trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
                CG_BubbleTrail( trace.endpos, end, 32 );
            }

            // draw a tracer
            if ( random() < cg_tracerChance.value )
            {
                CG_Tracer( start, end );
            }
        }
    }

    // impact splash and mark
    if ( flesh )
    {
    }
    else
    {
        CG_MissileHitWall( RWP_FIRE, 0, end, normal, IMPACTSOUND_DEFAULT );
    }

}
