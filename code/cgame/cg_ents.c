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
======================
CG_PositionEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void CG_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
                             qhandle_t parentModel, char *tagName )
{
    int				i;
    orientation_t	lerped;

    // lerp the tag
    trap_R_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
                    1.0 - parent->backlerp, tagName );

    // FIXME: allow origin offsets along tag?
    VectorCopy( parent->origin, entity->origin );
    for ( i = 0 ; i < 3 ; i++ )
    {
        VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
    }

    // had to cast away the const to avoid compiler problems...
    MatrixMultiply( lerped.axis, ((refEntity_t *)parent)->axis, entity->axis );
    entity->backlerp = parent->backlerp;
}


/*
======================
CG_PositionRotatedEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void CG_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
                                    qhandle_t parentModel, char *tagName )
{
    int				i;
    orientation_t	lerped;
    vec3_t			tempAxis[3];

//AxisClear( entity->axis );
    // lerp the tag
    trap_R_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
                    1.0 - parent->backlerp, tagName );

    // FIXME: allow origin offsets along tag?
    VectorCopy( parent->origin, entity->origin );
    for ( i = 0 ; i < 3 ; i++ )
    {
        VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
    }

    // had to cast away the const to avoid compiler problems...
    MatrixMultiply( entity->axis, lerped.axis, tempAxis );
    MatrixMultiply( tempAxis, ((refEntity_t *)parent)->axis, entity->axis );
}



/*
==========================================================================

FUNCTIONS CALLED EACH FRAME

==========================================================================
*/

/*
======================
CG_SetEntitySoundPosition

Also called by event processing code
======================
*/
void CG_SetEntitySoundPosition( centity_t *cent )
{
    if ( cent->currentState.solid == SOLID_BMODEL )
    {
        vec3_t	origin;
        float	*v;

        v = cgs.inlineModelMidpoints[ cent->currentState.modelindex ];
        VectorAdd( cent->lerpOrigin, v, origin );
        trap_S_UpdateEntityPosition( cent->currentState.number, origin );
    }
    else
    {
        trap_S_UpdateEntityPosition( cent->currentState.number, cent->lerpOrigin );
    }
}

/*
==================
CG_EntityEffects

Add continuous entity effects, like local entity emission and lighting
==================
*/
static void CG_EntityEffects( centity_t *cent )
{

    // update sound origins
    CG_SetEntitySoundPosition( cent );

    // add loop sound
    if ( cent->currentState.loopSound )
    {
        if (cent->currentState.eType != ET_SPEAKER)
        {
            trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin,
                                    cgs.gameSounds[ cent->currentState.loopSound ] );
        }
        else
        {
            trap_S_AddRealLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin,
                                        cgs.gameSounds[ cent->currentState.loopSound ] );
        }
    }


    // constant light glow
    if ( cent->currentState.constantLight )
    {
        int		cl;
        int		i, r, g, b;

        cl = cent->currentState.constantLight;
        r = cl & 255;
        g = ( cl >> 8 ) & 255;
        b = ( cl >> 16 ) & 255;
        i = ( ( cl >> 24 ) & 255 ) * 4;
        trap_R_AddLightToScene( cent->lerpOrigin, i, r, g, b );
    }

}


/*
==================
CG_General
==================
*/
static void CG_General( centity_t *cent )
{
    refEntity_t			ent;
    entityState_t		*s1;

    s1 = &cent->currentState;

    // if set to invisible, skip
    if (!s1->modelindex)
    {
        return;
    }

    memset (&ent, 0, sizeof(ent));

    // set frame

    ent.frame = s1->frame;
    ent.oldframe = ent.frame;
    ent.backlerp = 0;

    VectorCopy( cent->lerpOrigin, ent.origin);
    VectorCopy( cent->lerpOrigin, ent.oldorigin);

    ent.hModel = cgs.gameModels[s1->modelindex];

    // player model
    if (s1->number == cg.snap->ps.clientNum)
    {
        ent.renderfx |= RF_THIRD_PERSON;	// only draw from mirrors
    }

    // convert angles to axis
    AnglesToAxis( cent->lerpAngles, ent.axis );

    // add to refresh list
    trap_R_AddRefEntityToScene (&ent);
}

static qboolean CG_ParseMD3AnimationFile( const char *filename, animation_t *anim )
{
    char		fname[MAX_QPATH];
    int			len;
    char		*token;
    float		fps;
    char		text[10000],*text_p;
    fileHandle_t	f;
    strcpy(fname,filename);
    text_p=fname+strlen(fname);
    while (*text_p!='.' && *text_p!='\\' && text_p!=fname)
        text_p--;
    if (*text_p=='.')
        *text_p=0;
    Q_strcat(fname,sizeof(fname),".anim");
    // load the file
    len = trap_FS_FOpenFile( fname, &f, FS_READ );
    if ( len <= 0 )
    {
        CG_Printf( NULL, "File %s too short (exists?)\n", fname );
        return qfalse;
    }
    if ( len >= sizeof( text ) - 1 )
    {
        CG_Printf( NULL, "File %s too long\n", fname );
        return qfalse;
    }
    trap_FS_Read( text, len, f );
    text[len] = 0;
    trap_FS_FCloseFile( f );
    // parse the text
    text_p=text;
    token = COM_Parse( &text_p );
    if ( !*token )
    {
        CG_Printf( NULL, "%s%s\n",fname);
        return qfalse;
    }
    anim->firstFrame = atoi( token );
    token = COM_Parse( &text_p );
    if ( !*token )
    {
        CG_Printf( NULL, "%s%s\n",fname);
        return qfalse;
    }
    anim->numFrames = atoi( token );
    anim->reversed = qfalse;
    anim->flipflop = qfalse;
    // if numFrames is negative the animation is reversed
    if (anim->numFrames < 0)
    {
        anim->numFrames = -anim->numFrames;
        anim->reversed = qtrue;
    }
    token = COM_Parse( &text_p );
    if ( !*token )
    {
        CG_Printf( NULL, "%s%s\n",fname);
        return qfalse;
    }
    anim->loopFrames = atoi( token );
    token = COM_Parse( &text_p );
    if ( !*token )
    {
        CG_Printf( NULL, "%s%s\n",fname);
        return qfalse;
    }
    fps = atof( token );
    if ( fps == 0 )
    {
        fps = 1;
    }
    anim->frameLerp = 1000 / fps;
    anim->initialLerp = 1000 / fps;
    return qtrue;
}

static void CG_ModelAnimate( centity_t *cent, int *md3Old, int *md3, float *md3BackLerp )
{
    entityState_t		*s1;
    lerpFrame_t *model=&cent->md3.model;
    s1 = &cent->currentState;
    if (cent->currentState.eFlags & EF_FORCE_END_FRAME )
    {
        cent->md3.state &= ~2;
        cent->md3.state &= ~4;
    }
    if (s1->modelindex2!=cent->md3.lastCode)	// are we being triggered from the server?
    {
        cent->md3.lastCode=s1->modelindex2;
        if (!(cent->md3.state & 2))	// is it animating?
        {
            cent->md3.state |= 4;	// show model
            cent->md3.state |= 2;	// if not, activate animation
            cent->md3.state ^= ANIM_TOGGLEBIT;
            CG_ClearLerpFrameNPC(&cent->md3.anim, model, cent->md3.state & ANIM_TOGGLEBIT);
        }
    }
    if (cent->currentState.eFlags & EF_FORCE_END_FRAME )
    {
        model->frame = model->animation->firstFrame; // + model->animation->numFrames - 1;
        model->oldFrame = model->frame;
        model->backlerp = 0;
        cent->md3.state &= ~2;
    }
    if (cent->md3.state & 2)	// do we have to animate?
    {
        CG_RunLerpFrameNPC( &cent->md3.anim, model, cent->md3.state & ANIM_TOGGLEBIT, cent->md3.speed );
        if (model->oldFrame==model->frame &&
                model->frame==model->animation->numFrames+model->animation->firstFrame-1)	// finished?
        {
            cent->md3.state &= ~2;			// stop animation processing
            if (s1->generic1 & 2)		// dissapear if HIDDEN_END is turned on
                cent->md3.state &= ~4;
        }
    }
    *md3Old = model->oldFrame;
    *md3 = model->frame;
    *md3BackLerp = model->backlerp;
}

/*
==================
CG_ModelAnim
==================
*/
static void CG_ModelAnim( centity_t *cent )
{
    refEntity_t			ent;
    entityState_t		*s1;
    s1 = &cent->currentState;
    // if set to invisible, skip
    if (!s1->modelindex)
    {
        return;
    }
    if (!(cent->md3.state & 1))  // was animation config loaded?
    {
        const char *modelName;
        modelName = CG_ConfigString( CS_MODELS+cg_entities[s1->number].currentState.modelindex );
        if (!CG_ParseMD3AnimationFile(modelName,&cent->md3.anim))
        {
            // for debugging
            cent->md3.anim.firstFrame=0;
            cent->md3.anim.flipflop=0;
            cent->md3.anim.frameLerp=100;
            cent->md3.anim.initialLerp=100;
            cent->md3.anim.loopFrames=10;
            cent->md3.anim.numFrames=10;
            cent->md3.anim.reversed=0;
        }
        cent->md3.state |= 1;
        if (!(s1->generic1 & 1))	// show if HIDDEN_START is turned off
            cent->md3.state|=4;

        cent->md3.speed = (float)(s1->generic1 & 0xF0) / 16;
    }
    memset (&ent, 0, sizeof(ent));
    CG_ModelAnimate(cent,&ent.oldframe,&ent.frame,&ent.backlerp);
    if (!(cent->md3.state & 4))		// do nothing if visible flag is not set
        return;
    VectorCopy( cent->lerpOrigin, ent.origin);
    VectorCopy( cent->lerpOrigin, ent.oldorigin);
    ent.hModel = cgs.gameModels[s1->modelindex];
    // convert angles to axis
    AnglesToAxis( cent->lerpAngles, ent.axis );
    // add to refresh list
    trap_R_AddRefEntityToScene (&ent);
}

/*
==================
CG_Speaker

Speaker entities can automatically play sounds
==================
*/
static void CG_Speaker( centity_t *cent )
{
    if ( ! cent->currentState.clientNum )  	// FIXME: use something other than clientNum...
    {
        return;		// not auto triggering
    }

    if ( cg.time < cent->miscTime )
    {
        return;
    }

    trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.gameSounds[cent->currentState.eventParm] );

    //	ent->s.frame = ent->wait * 10;
    //	ent->s.clientNum = ent->random * 10;
    cent->miscTime = cg.time + cent->currentState.frame * 100 + cent->currentState.clientNum * 100 * crandom();
}

/*
==================
CG_AddElementParticles
==================
*/
void CG_AddElementParticles( float itemTag, vec3_t origin )
{
    int	element;
    vec3_t	particleorigin, end;
    localEntity_t	*particle;
    if( itemTag > RWP_ELECTRICAL )
    {
        element = ceil( (itemTag - RWP_ELECTRICAL )/4.0f);
    }
    else
    {
        element = itemTag;
    }
    //CG_Printf( NULL,"element is %i\n", element );
    if( (cg.time%10) )
        return;

    VectorCopy( origin, particleorigin );
    switch ( element )
    {
    case WP_DEFAULT:
        break;
    case WP_FIRE:
        particleorigin[0] += 14*crandom(); //between 1 and -1
        particleorigin[1] += 14*crandom(); //between 1 and -1
        particleorigin[2] += 14*crandom(); //between 1 and -1
        VectorSet( end, 0, 0, 100);
        particle = CG_SmokePuff( particleorigin, end, //starting point, destination
                                 5, //radius
                                 1, 1, 1, .33f, //color
                                 400, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_PUFF_SHRINK,
                                 cgs.media.FireParticle );
        particle->leFlags |= LEF_PUFF_FADE_RGB; //force RGB fading
        break;
    case WP_WATER:
        particleorigin[0] += 14*crandom(); //between 1 and -1
        particleorigin[1] += 14*crandom(); //between 1 and -1
        particleorigin[2] += 14*crandom(); //between 1 and -1
        VectorSet( end, 0, 0, 100);
        particle = CG_SmokePuff( particleorigin, end, //starting point, destination
                                 3, //radius
                                 1, 1, 1, .33f, //color
                                 400, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_PUFF_FADE_RGB,
                                 cgs.media.WaterParticle );
        break;
    case WP_AIR:
    {
        float z_height;
        z_height = 14*crandom(); //between 1 and -1
        particleorigin[2] += z_height;
        VectorSet( end, 0, 0, 1);
        particle = CG_SmokePuff( particleorigin, end, //starting point, destination
                                 3, //radius
                                 1, 1, 1, .33f, //color
                                 400, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_PUFF_FADE_RGB,
                                 cgs.media.WindParticle );
        particle->pos.trType = TR_CIRCLE;
        particle->pos.trDuration = 1.5*z_height;
        break;
    }
    case WP_EARTH:
        end[0] = crandom(); //between 1 and -1
        end[1] = crandom();
        end[2] = crandom();
        VectorNormalize( end );
        VectorScale( end, 20, end );
        //VectorMA( particleorigin, -1.0f, end, particleorigin );
        particle = CG_SmokePuff( particleorigin, end, //starting point, destination
                                 4, //radius
                                 1, 1, 1, 1, //color
                                 500, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_PUFF_DONT_SCALE,
                                 cgs.media.EarthParticle );
        particle->leFlags |= LEF_PUFF_FADE_RGB; //force RGB fading
        particle->pos.trType = TR_SINE;
        particle->endTime = cg.time + 2000;
        particle->pos.trDuration = 1000;
        break;
    case WP_TECH:
        if( (cg.time%250) < 3 )
        {
            int k = 0;
            int	number = 3 + ( genrand_int32()%7 );
            localEntity_t *particles[10];
            for( k = 0; k < number; k++ )
            {
                end[0] = crandom(); //between 1 and -1
                end[1] = crandom();
                end[2] = 2;
                VectorScale( end, 100, end );
                particles[k] = CG_GravityTrail( particleorigin, end, //starting point, destination
                                                5, //radius
                                                1, 1, 1, 0.33f, //color
                                                700, //milliseconds in existence
                                                cg.time, 0, //start time, fadein time
                                                LEF_PUFF_FADE_RGB,
                                                cgs.media.TechParticle );
            }
        }
        break;
    case WP_NATURE:
    {
        float z_height;
        z_height = 14*crandom(); //between 1 and -1
        particleorigin[2] += z_height;
        if( crandom() )
        {
            VectorSet( end, 0, 0, 1 );
        }
        else
        {
            VectorSet( end, 0, 0, -1 );
        }
        particle = CG_SmokePuff( particleorigin, end, //starting point, destination
                                 3, //radius
                                 1, 1, 1, 1, //color
                                 400, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_PUFF_DONT_FADE,
                                 cgs.media.NatureParticle );
        particle->leFlags |= LEF_PUFF_DONT_SCALE; //dont scale them
        particle->pos.trType = TR_CIRCLE;
        particle->pos.trDuration = 2*(14 - fabs(z_height));
        break;
    }
    break;
    case WP_GRAVITY:
        end[0] = crandom(); //between 1 and -1
        end[1] = crandom();
        end[2] = crandom();
        VectorNormalize( end );
        VectorScale( end, 60, end );
        particle = CG_SmokePuff( particleorigin, end, //starting point, destination
                                 4, //radius
                                 1, 1, 1, 1, //color
                                 400, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_SPRITE_DELAY,
                                 cgs.media.GravityParticle );
        particle->leFlags |= LEF_PUFF_SHRINK; //shrink
        particle->leFlags |= LEF_PUFF_FADE_RGB; //force RGB fading
        particle->pos.trTime = cg.time + 400;
        break;
    case WP_LIGHT:
        end[0] = crandom(); //between 1 and -1
        end[1] = crandom();
        end[2] = crandom();
        VectorNormalize( end );
        VectorScale( end, 100, end );
        particle = CG_SmokePuff( particleorigin, end, //starting point, destination
                                 1, //radius
                                 1, 1, 1, 1, //color
                                 400, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_PUFF_FADE_RGB,
                                 cgs.media.EnergyParticle );
        break;
    case WP_ELECTRICAL:
        particleorigin[0] += 30*crandom(); //between 1 and -1
        particleorigin[1] += 30*crandom(); //between 1 and -1
        particleorigin[2] += 30*crandom(); //between 1 and -1
        particle = CG_SmokePuff( particleorigin, particleorigin, //starting point, destination
                                 3, //radius
                                 1, 1, 1, 1, //color
                                 400, //milliseconds in existence
                                 cg.time, 0, //start time, fadein time
                                 LEF_PUFF_SHRINK,
                                 cgs.media.ElectricParticle );
        particle->leFlags |= LEF_PUFF_FADE_RGB; //force RGB fading
        particle->leType = LE_SCALE_FADE; // particle should shrink over time
        break;
    default:
        break;
    }
}
/*
==================
CG_Item
==================
*/
static void CG_Item( centity_t *cent )
{
    refEntity_t		ent;
    entityState_t	*es;
    gitem_t			*item;
    int				msec;
    float			frac;
    float			scale;
    weaponInfo_t	*wi;
    int weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[cent->currentState.weapon].weapon;
    float			light;
    vec3_t spinAngles;

    es = &cent->currentState;
    if ( es->modelindex >= bg_numItems )
    {
        CG_Error( "Bad item index %i on entity", es->modelindex );
    }

    // if set to invisible, skip
    if ( !es->modelindex || ( es->eFlags & EF_NODRAW ) )
    {
        return;
    }

    item = &bg_itemlist[ es->modelindex ];

    // items bob up and down continuously
    scale = 0.005 + cent->currentState.number * 0.00001;
    cent->lerpOrigin[2] += 4 + cos( ( cg.time + 1000 ) *  scale ) * 4;

    memset (&ent, 0, sizeof(ent));

    // autorotate at one of two speeds
    if ( item->giType == IT_HEALTH )
    {
        VectorCopy( cg.autoAnglesFast, cent->lerpAngles );
        AxisCopy( cg.autoAxisFast, ent.axis );
    }
    else
    {
        VectorCopy( cg.autoAngles, cent->lerpAngles );
        AxisCopy( cg.autoAxis, ent.axis );
    }

    wi = NULL;
    // the weapons have their origin where they attatch to player
    // models, so we need to offset them or they will rotate
    // eccentricly
    if ( item->giType == IT_WEAPON )
    {
        wi = &cg_weapons[ weap ];
        cent->lerpOrigin[0] -=
            wi->weaponMidpoint[0] * ent.axis[0][0] +
            wi->weaponMidpoint[1] * ent.axis[1][0] +
            wi->weaponMidpoint[2] * ent.axis[2][0];
        cent->lerpOrigin[1] -=
            wi->weaponMidpoint[0] * ent.axis[0][1] +
            wi->weaponMidpoint[1] * ent.axis[1][1] +
            wi->weaponMidpoint[2] * ent.axis[2][1];
        cent->lerpOrigin[2] -=
            wi->weaponMidpoint[0] * ent.axis[0][2] +
            wi->weaponMidpoint[1] * ent.axis[1][2] +
            wi->weaponMidpoint[2] * ent.axis[2][2];

        cent->lerpOrigin[2] += 8;	// an extra height boost
    }

    switch( item->giType )
    {
    case IT_WEAPON:
        memset( &ent, 0, sizeof( ent ) );
        VectorCopy( cent->lerpOrigin, ent.origin);
        VectorCopy( cent->lerpOrigin, ent.oldorigin);
        ent.reType = RT_SPRITE;
        ent.customShader = cgs.media.WeaponModel;
        ent.radius = 14;
        ent.shaderRGBA[0] = 255;
        ent.shaderRGBA[1] = 255;
        ent.shaderRGBA[2] = 255;
        ent.shaderRGBA[3] = 255;
        trap_R_AddRefEntityToScene( &ent );
        CG_AddElementParticles( item->giTag, cent->lerpOrigin );
        return;
    case IT_AMMO:
        ent.hModel = trap_R_RegisterModel("models/items/energy/xb_energy_1.md3");
        break;
    case IT_CHIPS:
        ent.hModel = trap_R_RegisterModel("models/upgrades/bigbolt/bigbolt.md3");
        break;
    default:
        ent.hModel = cg_items[es->modelindex].models[0];
        break;
    }

    VectorCopy( cent->lerpOrigin, ent.origin);
    VectorCopy( cent->lerpOrigin, ent.oldorigin);

    ent.nonNormalizedAxes = qfalse;

    // if just respawned, slowly scale up
    msec = cg.time - cent->miscTime;
    if ( msec >= 0 && msec < ITEM_SCALEUP_TIME )
    {
        frac = (float)msec / ITEM_SCALEUP_TIME;
        VectorScale( ent.axis[0], frac, ent.axis[0] );
        VectorScale( ent.axis[1], frac, ent.axis[1] );
        VectorScale( ent.axis[2], frac, ent.axis[2] );
        ent.nonNormalizedAxes = qtrue;
    }
    else
    {
        frac = 1.0;
    }

    // items without glow textures need to keep a minimum light value
    // so they are always visible
    if ( ( item->giType == IT_WEAPON ) ||
            ( item->giType == IT_ARMOR ) )
    {
        ent.renderfx |= RF_MINLIGHT;
    }

    // increase the size of the weapons when they are presented as items
    if ( item->giType == IT_WEAPON )
    {
        VectorScale( ent.axis[0], 1.5, ent.axis[0] );
        VectorScale( ent.axis[1], 1.5, ent.axis[1] );
        VectorScale( ent.axis[2], 1.5, ent.axis[2] );
        ent.nonNormalizedAxes = qtrue;
    }

    // add to refresh list
    trap_R_AddRefEntityToScene(&ent);

    // Adding Dynamic Lights

    light = 400;
    //item->DlightRBG[0] = .427f;
    //item->DlightRBG[1] = .811f;
    //item->DlightRBG[2] = .811f;

    //light = item->Dlight * light;
    VectorCopy( cent->lerpOrigin, ent.origin );

    trap_R_AddLightToScene(ent.origin, light, .427f, .811f, .811f );

    // accompanying rings / spheres for powerups


    VectorClear( spinAngles );
    if ( item->giType == IT_HEALTH || item->giType == IT_POWERUP )
    {
        if ( ( ent.hModel = cg_items[es->modelindex].models[1] ) != 0 )
        {
            if ( item->giType == IT_POWERUP )
            {
                ent.origin[2] += 12;
                spinAngles[1] = ( cg.time & 1023 ) * 360 / -1024.0f;
            }
            AnglesToAxis( spinAngles, ent.axis );

            // scale up if respawning
            if ( frac != 1.0 )
            {
                VectorScale( ent.axis[0], frac, ent.axis[0] );
                VectorScale( ent.axis[1], frac, ent.axis[1] );
                VectorScale( ent.axis[2], frac, ent.axis[2] );
                ent.nonNormalizedAxes = qtrue;
            }

            trap_R_AddRefEntityToScene( &ent );
        }
    }
}

//============================================================================

/*
===============
CG_Missile
===============
*/
static void CG_Missile( centity_t *cent )
{
    refEntity_t			ent;
    entityState_t		*s1;
    const weaponInfo_t	*weapon;
    int					weaponNum, k;
    float				yaw;
    qboolean			needstrail = qtrue;
    s1 = &cent->currentState;

    // check charged weapons first
    switch ( s1->weapon )
    {
    case WP_XBUSTER1:
    case WP_XBUSTER2:
    case WP_XBUSTER3:
    case WP_XBUSTER4:
        weaponNum = WP_XBUSTER;
        //CG_Printf( NULL,"adjusting for charged weapon\n");
        break;
    case WP_MBUSTER1:
    case WP_MBUSTER2:
        weaponNum = WP_MBUSTER;
        //CG_Printf( NULL,"adjusting for charged weapon\n");
        break;
    case WP_PBUSTER1:
    case WP_PBUSTER2:
        weaponNum = WP_PBUSTER;
        //CG_Printf( NULL,"adjusting for charged weapon\n");
        break;
    case WP_ZBUSTER1:
    case WP_ZBUSTER2:
        weaponNum = WP_ZBUSTER;
        break;
    case WP_FCUTTER2:
        weaponNum = WP_FCUTTER;
        break;
    case WP_SICE2:
    case WP_SICE3:
        weaponNum = WP_SICE;
        needstrail = qfalse;
        break;
    case WP_HWAVE2:
        weaponNum = WP_HWAVE;
        break;
    case WP_AFIST2:
        weaponNum = WP_AFIST;
        break;
    case WP_BCRYSTAL2:
        weaponNum = WP_BCRYSTAL;
        break;
    case WP_BCRYSTAL3:
    case WP_BCRYSTAL4:
        weaponNum = WP_BCRYSTAL;
        needstrail = qfalse;
        break;
    case WP_LTORPEDO2:
    case WP_LTORPEDO3:
        weaponNum = WP_LTORPEDO;
        break;
    case WP_TBURSTER2:
        weaponNum = WP_TBURSTER;
        break;
    case WP_GBOMB2:
        weaponNum = WP_GBOMB;
        break;
    case WP_BLASER2:
        weaponNum = WP_BLASER;
        break;
    case WP_SFIST2:
        weaponNum = WP_SFIST;
        break;
    default:
        weaponNum = s1->weapon;
        break;
    }

    if ( ( s1->weapon > WP_NUM_WEAPONS ) && ( s1->weapon == weaponNum ) && ( s1->weapon != WP_ZBUSTER ) )
    {
        s1->weapon = 0;
    }

    // since WP_XBUSTER1-4 and WP_MBUSTER1-2 arent technically weapons, we need to keep that information around at least...
    weapon = &cg_weapons[weaponNum ];

    // calculate the axis
    VectorCopy( s1->angles, cent->lerpAngles);

    // add trails
    if ( weapon->missileTrailFunc && needstrail )
    {
        weapon->missileTrailFunc( cent, weapon );
    }
    // add dynamic light
    if ( weapon->missileDlight )
    {
        trap_R_AddLightToScene(cent->lerpOrigin, weapon->missileDlight,
                               weapon->missileDlightColor[0], weapon->missileDlightColor[1], weapon->missileDlightColor[2] );
    }

    // add missile sound
    if ( weapon->missileSound )
    {
        vec3_t	velocity;

        BG_EvaluateTrajectoryDelta( &cent->currentState.pos, cg.time, velocity );

        trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, velocity, weapon->missileSound );
    }

    // create the render entity
    memset (&ent, 0, sizeof(ent));
    VectorCopy( cent->lerpOrigin, ent.origin);
    VectorCopy( cent->lerpOrigin, ent.oldorigin);

    switch( cent->currentState.weapon )
    {
    case WP_XBUSTER1:
    case WP_XBUSTER2:
    case WP_MBUSTER1:
    case WP_MBUSTER2:
    case WP_PBUSTER1:
    case WP_PBUSTER2:
    case WP_ZBUSTER1:
    case WP_ZBUSTER2:
        break;
    case WP_XBUSTER:	// the default, uncharged buster shot
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 8;
        ent.customShader = cgs.media.xShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_XBUSTER3:
    case WP_XBUSTER4:	// level 3/4 blast
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 16 + 7*(5-(cent->currentState.pos.trDuration%5));
        ent.customShader = cgs.media.xShot4;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_FBUSTER:	// forte's cannon
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 16;
        ent.customShader = cgs.media.bassShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_MBUSTER:	// megaman's cannon uncharged
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 8;
        ent.customShader = cgs.media.xShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_PBUSTER:	// protoman's cannon uncharged
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 8;
        ent.customShader = cgs.media.xShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_ZBUSTER:	// Zero's cannon uncharged
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 8;
        ent.customShader = cgs.media.xShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_FCUTTER:	//The flamethrower
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.chargeFireShot1;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_FCUTTER2:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.chargeFireShot2;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_RISFIRE:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 25;
        ent.customShader = cgs.media.sheildFireShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_HWAVE:
    case WP_HWAVE2:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.busterFireShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_PSTRIKE:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.rapidFireShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_SICE:	//shotgun ice
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 30;
        ent.customShader = cgs.media.chargeWaterShot1;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_SICE2:	//shotgun ice shards
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 20;
        ent.customShader = cgs.media.chargeWaterShot2;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_SICE3:	//shotgun ice charged
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 100;
        ent.customShader = cgs.media.chargeWaterShot3;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_FBURST:	//freeze burst
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.busterWaterShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_IBURST:
        return;
        //air
    case WP_AFIST:
    case WP_AFIST2:
    case WP_AWAVE:
    case WP_TEMPEST:
        break;
        //earth
    case WP_BCRYSTAL:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.chargeEarthShot1;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_BCRYSTAL2:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 25;
        ent.customShader = cgs.media.chargeEarthShot1;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_BCRYSTAL3:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 10;
        ent.customShader = cgs.media.chargeEarthShot1;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_BCRYSTAL4:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 5;
        ent.customShader = cgs.media.chargeEarthShot1;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_RSTONE:
    case WP_SBURST:
        break;
        //metal
    case WP_DSHOT:
    case WP_LTORPEDO:
    case WP_LTORPEDO2:
    case WP_LTORPEDO3:
    case WP_BLAUNCHER:
    case WP_STRIKECHAIN:
        break;
        //nature
    case WP_TBURSTER:
    case WP_TBURSTER2:
        break;
    case WP_LSTORM:
    {
        refEntity_t ents[10];
        vec3_t	directions[10] =
        {
            { 1, 0, 0 },
            { -1, 0, 0 },
            { 0, 1, 0 },
            { 0, -1, 0 },
            { 0, 0, 1 },
            { 0, 0, -1 },
            { 1, 1, 0 },
            { 1, -1, 0 },
            { -1, 1, 0 },
            { -1, -1, 0 }
        };
        for( k = 0; k < 10; k++ )
        {
            ents[k].origin[0] = ent.origin[0] + 20*directions[k][0];
            ents[k].origin[1] = ent.origin[1] + 20*directions[k][1];
            ents[k].origin[2] = ent.origin[2] + 20*directions[k][2];
            ents[k].reType = RT_SPRITE;
            ents[k].rotation = 0;
            ents[k].radius = 4;
            ents[k].customShader = cgs.media.busterNatureShot;
            trap_R_AddRefEntityToScene( &ents[k] );
        }
    }
    return;
    case WP_ASPLASHER:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 25;
        ent.customShader = cgs.media.rapidNatureShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
        //gravity
    case WP_GBOMB:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.chargeGravityShot1;
        trap_R_AddRefEntityToScene( &ent );
    case WP_GBOMB2:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 25;
        ent.customShader = cgs.media.chargeGravityShot2;
        trap_R_AddRefEntityToScene( &ent );
        break;
    case WP_MMINE:
        ent.reType = RT_MODEL;
        VectorCopy( cent->lerpOrigin, ent.lightingOrigin );
        VectorCopy( cent->lerpOrigin, ent.origin );
        AnglesToAxis( cent->currentState.angles, ent.axis );
        ent.hModel = cgs.media.busterGravityShot;
        //ent.customShader = cgs.media.plasmaBallShader;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_DMATTER:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.rapidGravityShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
        //light
    case WP_BLASER:
        break;
    case WP_BLASER2:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.chargeLightShot2;
        trap_R_AddRefEntityToScene( &ent );
        return;
        /*case WP_SSHREDDER:
        	ent.reType = RT_SPRITE;
        	ent.rotation = 0;
        	ent.radius = 15;
        	ent.customShader = cgs.media.plasmaBallShader;
        	trap_R_AddRefEntityToScene( &ent );
        	return;*/
    case WP_NBURST:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 20;
        ent.customShader = cgs.media.rapidLightShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
        //electrical
    case WP_SFIST:
    case WP_SFIST2:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        //ent.customShader = cgs.media.chargeElectricShot1;
        ent.customShader = cgs.media.plasmaBallShader;
        trap_R_AddRefEntityToScene( &ent );
        return;
    case WP_TRTHUNDER:
        ent.reType = RT_SPRITE;
        ent.rotation = 0;
        ent.radius = 15;
        ent.customShader = cgs.media.sheildElectricShot;
        trap_R_AddRefEntityToScene( &ent );
        return;
        /*case WP_BSHIELD:
        	ent.reType = RT_SPRITE;
        	ent.rotation = 0;
        	ent.radius = 15;
        	ent.customShader = cgs.media.chargeElectricShot1;
        	trap_R_AddRefEntityToScene( &ent );
        	return;
        case WP_BLIGHTNING:
        	ent.reType = RT_SPRITE;
        	ent.rotation = 0;
        	ent.radius = 15;
        	ent.customShader = cgs.media.plasmaBallShader;
        	trap_R_AddRefEntityToScene( &ent );
        	return;
        	break;*/
    default:
        ent.radius = 15;
        ent.customShader = cgs.media.plasmaBallShader;
        trap_R_AddRefEntityToScene( &ent );
        return;
    }

    // flicker between two skins
    ent.skinNum = cg.clientFrame & 1;
    ent.hModel = weapon->missileModel;
    ent.renderfx = weapon->missileRenderfx | RF_NOSHADOW;

    // put the special charged forms down here, so that they change the model
    switch( cent->currentState.weapon )
    {
    case WP_XBUSTER1:
    case WP_MBUSTER1:
    case WP_PBUSTER1:
    case WP_ZBUSTER1:
        ent.hModel = trap_R_RegisterModel( "gfx/charweapons/mmx/buster_2.md3" );
        break;
    case WP_XBUSTER2:
    case WP_MBUSTER2:
    case WP_PBUSTER2:
    case WP_ZBUSTER2:
        ent.hModel = trap_R_RegisterModel( "gfx/charweapons/mmx/buster_3.md3" );
        break;
    case WP_LTORPEDO2:
        ent.hModel = trap_R_RegisterModel( "weapons/tech/charge_multi.md3" );
        break;
    case WP_LTORPEDO3:
        ent.hModel = trap_R_RegisterModel( "weapons/tech/charge_mini.md3" );
        break;
        /*case WP_FCUTTER2:
        	ent.hModel = trap_R_RegisterModel( "weapons/fire/charge_fb.md3" );
        	break;*/
    case WP_TBURSTER2:
        ent.hModel = trap_R_RegisterModel( "weapons/plant/charge1_thorn.md3" );
        break;
    default:
        break;
    }

    // convert direction of travel into axis
    if ( VectorNormalize2( s1->pos.trDelta, ent.axis[0] ) == 0 )
    {
        ent.axis[0][2] = 1;
    }

    // spin as it moves
    if ( s1->pos.trType != TR_STATIONARY )
    {
        // not all weapons will rotate
        if( s1->weapon == WP_BLAUNCHER )
        {
            yaw = 0 ;
        }
        else
        {
            yaw = cg.time / 4;
        }
        RotateAroundDirection( ent.axis, yaw );
        //}
        // some weapons rotate around Z axis
        //else if ( s1->weapon == WP_AWAVE || s1->weapon == WP_IMBOOMERANG ){
        //			ent.axis[0][0] = 1;
        //			yaw = cg.time / 4;
        //			RotateAroundDirection( ent.axis, yaw );
        //		}
        // some weapons rotate around Y axis
        //else if ( s1->weapon == WP_RSTONE ){
        //			ent.axis[0][1] = 1;
        //			yaw = cg.time / 4;
        //			RotateAroundDirection( ent.axis, yaw );
        //		}
    }
    else
    {
        RotateAroundDirection( ent.axis, s1->time );
    }

    // add to refresh list, possibly with quad glow
    CG_AddRefEntityWithPowerups( &ent, s1, TEAM_FREE );
}

/*
===============
CG_Grapple

This is called when the grapple is sitting up against the wall
===============
*/
static void CG_Grapple( centity_t *cent )
{
    refEntity_t			ent;
    entityState_t		*s1;
    const weaponInfo_t		*weapon;
    int weap = bg_playerlist[cg.snap->ps.persistant[PLAYERCLASS]].weapons[cent->currentState.weapon].weapon;

    s1 = &cent->currentState;
    if ( s1->weapon > WP_NUM_WEAPONS )
    {
        s1->weapon = 0;
    }
    weapon = &cg_weapons[weap ];

    // calculate the axis
    VectorCopy( s1->angles, cent->lerpAngles);

#if 0 // FIXME add grapple pull sound here..?
    // add missile sound
    if ( weapon->missileSound )
    {
        trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->missileSound );
    }
#endif

    // Will draw cable if needed
    CG_GrappleTrail ( cent, weapon );

    // create the render entity
    memset (&ent, 0, sizeof(ent));
    VectorCopy( cent->lerpOrigin, ent.origin);
    VectorCopy( cent->lerpOrigin, ent.oldorigin);

    // flicker between two skins
    ent.skinNum = cg.clientFrame & 1;
    ent.hModel = weapon->missileModel;
    ent.renderfx = weapon->missileRenderfx | RF_NOSHADOW;

    // convert direction of travel into axis
    if ( VectorNormalize2( s1->pos.trDelta, ent.axis[0] ) == 0 )
    {
        ent.axis[0][2] = 1;
    }

    trap_R_AddRefEntityToScene( &ent );
}

/*
===============
CG_Mover
===============
*/
static void CG_Mover( centity_t *cent )
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

    // flicker between two skins (FIXME?)
    ent.skinNum = ( cg.time >> 6 ) & 1;

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

/*
===============
CG_Beam

Also called as an event
===============
*/
void CG_Beam( centity_t *cent )
{
    refEntity_t			ent;
    entityState_t		*s1;

    s1 = &cent->currentState;

    // create the render entity
    memset (&ent, 0, sizeof(ent));
    VectorCopy( s1->pos.trBase, ent.origin );
    VectorCopy( s1->origin2, ent.oldorigin );
    AxisClear( ent.axis );
    ent.reType = RT_BEAM;

    ent.renderfx = RF_NOSHADOW;

    // add to refresh list
    trap_R_AddRefEntityToScene(&ent);
}


/*
===============
CG_Portal
===============
*/
static void CG_Portal( centity_t *cent )
{
    refEntity_t			ent;
    entityState_t		*s1;

    s1 = &cent->currentState;

    // create the render entity
    memset (&ent, 0, sizeof(ent));
    VectorCopy( cent->lerpOrigin, ent.origin );
    VectorCopy( s1->origin2, ent.oldorigin );
    ByteToDir( s1->eventParm, ent.axis[0] );
    PerpendicularVector( ent.axis[1], ent.axis[0] );

    // negating this tends to get the directions like they want
    // we really should have a camera roll value
    VectorSubtract( vec3_origin, ent.axis[1], ent.axis[1] );

    CrossProduct( ent.axis[0], ent.axis[1], ent.axis[2] );
    ent.reType = RT_PORTALSURFACE;
    ent.oldframe = s1->powerups;
    ent.frame = s1->frame;		// rotation speed
    ent.skinNum = s1->clientNum/256.0 * 360;	// roll offset

    // add to refresh list
    trap_R_AddRefEntityToScene(&ent);
}

// NIGHTZ - CG_Explosive
static void CG_Explosive ( centity_t *cent )
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


/*
=========================
CG_AdjustPositionForMover

Also called by client movement prediction code
=========================
*/
void CG_AdjustPositionForMover( const vec3_t in, int moverNum, int fromTime, int toTime, vec3_t out )
{
    centity_t	*cent;
    vec3_t	oldOrigin, origin, deltaOrigin;
    vec3_t	oldAngles, angles, deltaAngles;

    if ( moverNum <= 0 || moverNum >= ENTITYNUM_MAX_NORMAL )
    {
        VectorCopy( in, out );
        return;
    }

    cent = &cg_entities[ moverNum ];
    if ( cent->currentState.eType != ET_MOVER )
    {
        VectorCopy( in, out );
        return;
    }

    BG_EvaluateTrajectory( &cent->currentState.pos, fromTime, oldOrigin );
    BG_EvaluateTrajectory( &cent->currentState.apos, fromTime, oldAngles );

    BG_EvaluateTrajectory( &cent->currentState.pos, toTime, origin );
    BG_EvaluateTrajectory( &cent->currentState.apos, toTime, angles );

    VectorSubtract( origin, oldOrigin, deltaOrigin );
    VectorSubtract( angles, oldAngles, deltaAngles );

    VectorAdd( in, deltaOrigin, out );

    // FIXME: origin change when on a rotating object
}


/*
=============================
CG_InterpolateEntityPosition
=============================
*/
static void CG_InterpolateEntityPosition( centity_t *cent )
{
    vec3_t		current, next;
    float		f;

    // it would be an internal error to find an entity that interpolates without
    // a snapshot ahead of the current one
    if ( cg.nextSnap == NULL )
    {
        CG_Error( "CG_InterpoateEntityPosition: cg.nextSnap == NULL" );
    }

    f = cg.frameInterpolation;

    // this will linearize a sine or parabolic curve, but it is important
    // to not extrapolate player positions if more recent data is available
    BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, current );
    BG_EvaluateTrajectory( &cent->nextState.pos, cg.nextSnap->serverTime, next );

    cent->lerpOrigin[0] = current[0] + f * ( next[0] - current[0] );
    cent->lerpOrigin[1] = current[1] + f * ( next[1] - current[1] );
    cent->lerpOrigin[2] = current[2] + f * ( next[2] - current[2] );

    BG_EvaluateTrajectory( &cent->currentState.apos, cg.snap->serverTime, current );
    BG_EvaluateTrajectory( &cent->nextState.apos, cg.nextSnap->serverTime, next );

    cent->lerpAngles[0] = LerpAngle( current[0], next[0], f );
    cent->lerpAngles[1] = LerpAngle( current[1], next[1], f );
    cent->lerpAngles[2] = LerpAngle( current[2], next[2], f );

}

/*
===============
CG_CalcEntityLerpPositions

===============
*/
static void CG_CalcEntityLerpPositions( centity_t *cent )
{

    // if this player does not want to see extrapolated players
    if ( !cg_smoothClients.integer )
    {
        // make sure the clients use TR_INTERPOLATE
        if ( cent->currentState.number < MAX_CLIENTS )
        {
            cent->currentState.pos.trType = TR_INTERPOLATE;
            cent->nextState.pos.trType = TR_INTERPOLATE;
        }
    }

    if ( cent->interpolate && cent->currentState.pos.trType == TR_INTERPOLATE )
    {
        CG_InterpolateEntityPosition( cent );
        return;
    }

    // first see if we can interpolate between two snaps for
    // linear extrapolated clients
    if ( cent->interpolate && cent->currentState.pos.trType == TR_LINEAR_STOP &&
            cent->currentState.number < MAX_CLIENTS)
    {
        CG_InterpolateEntityPosition( cent );
        return;
    }

    // just use the current frame and evaluate as best we can
    BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin );
    BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles );

    // adjust for riding a mover if it wasn't rolled into the predicted
    // player state
    if ( cent != &cg.predictedPlayerEntity )
    {
        CG_AdjustPositionForMover( cent->lerpOrigin, cent->currentState.groundEntityNum,
                                   cg.snap->serverTime, cg.time, cent->lerpOrigin );
    }
}

/*
===============
CG_TeamBase
===============
*/
static void CG_TeamBase( centity_t *cent )
{
    refEntity_t model;

    if ( cgs.gametype == GT_CTF)
    {
        // show the flag base
        memset(&model, 0, sizeof(model));
        model.reType = RT_MODEL;
        VectorCopy( cent->lerpOrigin, model.lightingOrigin );
        VectorCopy( cent->lerpOrigin, model.origin );
        AnglesToAxis( cent->currentState.angles, model.axis );
        if ( cent->currentState.modelindex == TEAM_RED )
        {
            model.hModel = cgs.media.redFlagBaseModel;
        }
        else if ( cent->currentState.modelindex == TEAM_BLUE )
        {
            model.hModel = cgs.media.blueFlagBaseModel;
        }
        else
        {
            model.hModel = cgs.media.neutralFlagBaseModel;
        }
        trap_R_AddRefEntityToScene( &model );
    }
}

/*
===============
CG_AddCEntity

===============
*/
static void CG_AddCEntity( centity_t *cent )
{
    // event-only entities will have been dealt with already
    if ( cent->currentState.eType >= ET_EVENTS )
    {
        return;
    }

    // calculate the current origin
    CG_CalcEntityLerpPositions( cent );

    // add automatic effects
    CG_EntityEffects( cent );

    switch ( cent->currentState.eType )
    {
    default:
        CG_Error( "Bad entity type: %i\n", cent->currentState.eType );
        break;
    case ET_INVISIBLE:
    case ET_PUSH_TRIGGER:
    case ET_TELEPORT_TRIGGER:
        break;
    case ET_GENERAL:
        CG_General( cent );
        break;
    case ET_NPC:
        CG_NPC( cent );
        break;
    case ET_MODELANIM:
        CG_ModelAnim( cent );
        break;
    case ET_PLAYER:
        CG_Player( cent );
        break;
    case ET_ITEM:
        CG_Item( cent );
        break;
    case ET_MISSILE:
        CG_Missile( cent );
        break;
    case ET_MOVER:
        CG_Mover( cent );
        break;
    case ET_BREAKABLE:
        CG_Mover( cent );
        break;
    case ET_EXPLOSIVE:
        CG_Explosive( cent );
        break;
    case ET_BEAM:
        CG_Beam( cent );
        break;
    case ET_PORTAL:
        CG_Portal( cent );
        break;
    case ET_SPEAKER:
        CG_Speaker( cent );
        break;
    case ET_GRAPPLE:
        CG_Grapple( cent );
        break;
    case ET_TEAM:
        CG_TeamBase( cent );
        break;
    }
}

/*
===============
CG_AddPacketEntities

===============
*/
void CG_AddPacketEntities( void )
{
    int					num;
    centity_t			*cent;
    playerState_t		*ps;

    // set cg.frameInterpolation
    if ( cg.nextSnap )
    {
        int		delta;

        delta = (cg.nextSnap->serverTime - cg.snap->serverTime);
        if ( delta == 0 )
        {
            cg.frameInterpolation = 0;
        }
        else
        {
            cg.frameInterpolation = (float)( cg.time - cg.snap->serverTime ) / delta;
        }
    }
    else
    {
        cg.frameInterpolation = 0;	// actually, it should never be used, because
        // no entities should be marked as interpolating
    }

    // the auto-rotating items will all have the same axis
    cg.autoAngles[0] = 0;
    cg.autoAngles[1] = ( cg.time & 2047 ) * 360 / 2048.0;
    cg.autoAngles[2] = 0;

    cg.autoAnglesFast[0] = 0;
    cg.autoAnglesFast[1] = ( cg.time & 1023 ) * 360 / 1024.0f;
    cg.autoAnglesFast[2] = 0;

    AnglesToAxis( cg.autoAngles, cg.autoAxis );
    AnglesToAxis( cg.autoAnglesFast, cg.autoAxisFast );

    // generate and add the entity from the playerstate
    ps = &cg.predictedPlayerState;
    BG_PlayerStateToEntityState( ps, &cg.predictedPlayerEntity.currentState, qfalse );
    CG_AddCEntity( &cg.predictedPlayerEntity );

    // lerp the non-predicted value for lightning gun origins
    CG_CalcEntityLerpPositions( &cg_entities[ cg.snap->ps.clientNum ] );

    // add each entity sent over by the server
    for ( num = 0 ; num < cg.snap->numEntities ; num++ )
    {
        cent = &cg_entities[ cg.snap->entities[ num ].number ];
        CG_AddCEntity( cent );
    }
}

