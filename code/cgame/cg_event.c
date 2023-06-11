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

//==========================================================================

/*
===================
CG_PlaceString

Also called by scoreboard drawing
===================
*/
const char	*CG_PlaceString( int rank )
{
    static char	str[64];
    char	*s, *t;

    if ( rank & RANK_TIED_FLAG )
    {
        rank &= ~RANK_TIED_FLAG;
        t = "Tied for ";
    }
    else
    {
        t = "";
    }

    if ( rank == 1 )
    {
        s = S_COLOR_BLUE "1st" S_COLOR_WHITE;		// draw in blue
    }
    else if ( rank == 2 )
    {
        s = S_COLOR_RED "2nd" S_COLOR_WHITE;		// draw in red
    }
    else if ( rank == 3 )
    {
        s = S_COLOR_YELLOW "3rd" S_COLOR_WHITE;		// draw in yellow
    }
    else if ( rank == 11 )
    {
        s = "11th";
    }
    else if ( rank == 12 )
    {
        s = "12th";
    }
    else if ( rank == 13 )
    {
        s = "13th";
    }
    else if ( rank % 10 == 1 )
    {
        s = va("%ist", rank);
    }
    else if ( rank % 10 == 2 )
    {
        s = va("%ind", rank);
    }
    else if ( rank % 10 == 3 )
    {
        s = va("%ird", rank);
    }
    else
    {
        s = va("%ith", rank);
    }

    Com_sprintf( str, sizeof( str ), "%s%s", t, s );
    return str;
}

/*
=============
CG_Obituary
=============
*/
static void CG_Obituary( entityState_t *ent )
{
    int			mod;
    int			target, attacker;
    char		*message;
    char		*message2;
    const char	*targetInfo;
    const char	*attackerInfo;
    char		targetName[32];
    char		attackerName[32];
    gender_t	gender;
    clientInfo_t	*ci;
    qboolean	switched = qfalse;

    target = ent->otherEntityNum;
    attacker = ent->otherEntityNum2;
    mod = ent->eventParm;

    if ( target < 0 || target >= MAX_CLIENTS )
    {
        CG_Error( "CG_Obituary: target out of range" );
    }
    ci = &cgs.clientinfo[target];

    if ( attacker < 0 || attacker >= MAX_CLIENTS )
    {
        attacker = ENTITYNUM_WORLD;
        attackerInfo = NULL;
    }
    else
    {
        attackerInfo = CG_ConfigString( CS_PLAYERS + attacker );
    }

    targetInfo = CG_ConfigString( CS_PLAYERS + target );
    if ( !targetInfo )
    {
        return;
    }
    Q_strncpyz( targetName, Info_ValueForKey( targetInfo, "n" ), sizeof(targetName) - 2);
    strcat( targetName, S_COLOR_WHITE );

    message2 = "";

    // check for single client messages

    switch( mod )
    {
    case MOD_SUICIDE:
        message = "suicides";
        break;
    case MOD_FALLING:
        message = "cratered";
        break;
    case MOD_CRUSH:
        message = "was crushed";
        break;
    case MOD_WATER:
        message = "sank like a rock";
        break;
    case MOD_SLIME:
        message = "melted";
        break;
    case MOD_LAVA:
        message = "does a back flip into the lava";
        break;
    case MOD_ICE:
        message = "froze to death";
        break;
    case MOD_TARGET_LASER:
        message = "saw the light";
        break;
    case MOD_TRIGGER_HURT:
        message = "was in the wrong place";
        break;
    case MOD_EXPLOSIVE:
        message = "blew up";
        break;
    default:
        message = NULL;
        break;
    }

    if (attacker == target)
    {
        gender = ci->gender;
        switch (mod)
        {
        case MOD_GRENADE_SPLASH:
            if ( gender == GENDER_FEMALE )
                message = "tripped on her own grenade";
            else if ( gender == GENDER_NEUTER )
                message = "tripped on its own grenade";
            else
                message = "tripped on his own grenade";
            break;
        case MOD_ROCKET_SPLASH:
            if ( gender == GENDER_FEMALE )
                message = "blew herself up";
            else if ( gender == GENDER_NEUTER )
                message = "blew itself up";
            else
                message = "blew himself up";
            break;
        case MOD_PLASMA_SPLASH:
            if ( gender == GENDER_FEMALE )
                message = "melted herself";
            else if ( gender == GENDER_NEUTER )
                message = "melted itself";
            else
                message = "melted himself";
            break;
        case MOD_LIGHTNING_DISCHARGE:
            if (gender == GENDER_FEMALE)
                message = "discharged herself";
            else if (gender == GENDER_NEUTER)
                message = "discharged itself";
            else
                message = "discharged himself";
            break;
        case MOD_EARTHQUAKE:
            message = "got shook to death";
            break;
        default:
            if ( gender == GENDER_FEMALE )
                message = "killed herself";
            else if ( gender == GENDER_NEUTER )
                message = "killed itself";
            else
                message = "killed himself";
            break;
        }
    }

    if (message)
    {
        CG_Printf( NULL,"^5[INFO]: ^5%s ^5%s.\n", targetName, message);
        return;
    }

    // check for kill messages from the current clientNum
    if ( attacker == cg.snap->ps.clientNum )
    {
        char	*s;

        if ( cgs.gametype < GT_TEAM )
        {
            s = va("You fragged %s\n%s place with %i", targetName,
                   CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),
                   cg.snap->ps.persistant[PERS_SCORE] );
        }
        else
        {
            s = va("You fragged %s", targetName );
        }

        CG_CenterPrint( s, SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );

        // print the text message as well
    }

    // check for double client messages
    if ( !attackerInfo )
    {
        attacker = ENTITYNUM_WORLD;
        strcpy( attackerName, "noname" );
    }
    else
    {
        Q_strncpyz( attackerName, Info_ValueForKey( attackerInfo, "n" ), sizeof(attackerName) - 2);
        strcat( attackerName, S_COLOR_WHITE );
        // check for kill messages about the current clientNum
        if ( target == cg.snap->ps.clientNum )
        {
            Q_strncpyz( cg.killerName, attackerName, sizeof( cg.killerName ) );
        }
    }

    if ( attacker != ENTITYNUM_WORLD )
    {
        switch (mod)
        {
        case MOD_GRAPPLE:
            message = "was caught by";
            break;
        case MOD_GAUNTLET:
            message = "was pummeled by";
            break;
        case MOD_MACHINEGUN:
            message = "was machinegunned by";
            break;
        case MOD_GRENADE:
            message = "ate";
            message2 = "'s grenade";
            break;
        case MOD_GRENADE_SPLASH:
            message = "was shredded by";
            message2 = "'s shrapnel";
            break;
        case MOD_ROCKET:
            message = "ate";
            message2 = "'s rocket";
            break;
        case MOD_ROCKET_SPLASH:
            message = "almost dodged";
            message2 = "'s rocket";
            break;
        case MOD_PLASMA:
            message = "was melted by";
            message2 = "'s plasmagun";
            break;
        case MOD_PLASMA_SPLASH:
            message = "was melted by";
            message2 = "'s plasmagun";
            break;
        case MOD_RAILGUN:
            message = "was railed by";
            break;
        case MOD_LIGHTNING:
            message = "was electrocuted by";
            break;
        case MOD_LIGHTNING_DISCHARGE:
            message = "was discharged by";
            break;
        case MOD_EARTHQUAKE:
            message = "was shook by";
            break;
            //Default attacks (add charged versions)
        case MOD_XBUSTER:		//MMX's main cannon
            message = "was decommisioned by";
            message2 = "'s X-Buster";
            break;
        case MOD_XBUSTER2:		//supercharged level 3 version
            message = "ate pink death from";
            message2 = "'s supercharged X-Buster";
            break;
        case MOD_MBUSTER:		//MM's main cannon
            message = "fell victim to";
            message2 = "'s Mega Buster";
            break;
        case MOD_PBUSTER:		//MM's main cannon
            message = "fell victim to";
            message2 = "'s Proto Buster";
            break;
        case MOD_FBUSTER:		//Forte's main weapon of destruction
            message = "couldn't escape the hail of destruction from";
            //message2 = "'s F. Buster";
            break;
        case MOD_ZBUSTER:		//Zero's arm cannon
            message = "was tapped to death by";
            message2 = "'s Z. Buster";
            break;
            //Flame attackes
        case MOD_FCUTTER: //charge
            message = "suffered severe burns from";
            //message2 = "'s F. Cutter";
            break;
        case MOD_FCUTTER2: //charged
            message = "was quickly melted by";
            message2 = "'s Speed Burner";
            break;
        case MOD_RISFIRE:	// drn000
            message = "was melted down by";
            //message2 = "'s H. Wave";
            break;
        case MOD_HWAVE:	//buster only
            message = "was turned into molten metal by";
            //message2 = "'s H. Wave";
            break;
        case MOD_PSTRIKE:	//rapid
            message = "rose up as ashes from";
            message2 = "'s Phoenix Strike";
            break;
            //Water attacks
        case MOD_SICE:	//charge
            message = "caught a cold from";
            //message2 = "'s S. Ice";
            break;
        case MOD_SICE2:	//charged
            message = "was compacted by";
            message2 = "'s Great Ball of Ice";
            break;
        case MOD_FBURST:	//buster only
            message = "was frozen by";
            //message2 = "'s F. Burst";
            break;
        case MOD_WSTRIKE:	//rapid
            message = "was the victim of";
            message2 = "'s hydrocutting experiment";
            break;
            //Air attacks
        case MOD_AFIST:	//charge
            message = "has gone with";
            message2 = "'s wind";
            break;
        case MOD_AFIST2:	//charged
            message = "got whisked away by a tornado from";
            //message2 = "'s F. Cutter";
            break;
        case MOD_AWAVE:	//buster only
            message = "couldn't quite get away from";
            message2 = "'s tiny twisters";
            break;
        case MOD_TEMPEST:	//rapid
            message = "kept his eye on";
            message2 = "birdie";
            break;
            //Earth attacks
        case MOD_BCRYSTAL:	//charge
            message = "was too busy staring at";
            message2 = "'s bouncing jewels";
            break;
        case MOD_BCRYSTAL2:	//charged
            message = "couldn't escape";
            message2 = "'s shattering gems";
            break;
        case MOD_RSTONE:	//buster only
            message = "was rocked by";
            //message2 = "'s R. Stone";
            break;
        case MOD_SBURST:	//rapid
            message = "was stoned by";
            //message2 = "'s S. Burst";
            break;
            //Metal attacks
        case MOD_LTORPEDO:	//charge
            message = "wasn't fast enough to escape";
            message2 = "'s little buddy";
            break;
        case MOD_LTORPEDO2:	//charged
            message = "lost a game of hide and go seek to";
            message2 = "'s homing friends";
            break;
        case MOD_BLAUNCHER:	//buster only
            message = "couldn't outrun";
            message2 = "'s guided blades";
            break;
        case MOD_DSHOT:	//rapid
            message = "was too soft for";
            message2 = "'s drills";
            break;
            //Nature attacks
        case MOD_TBURSTER://charge
            message = "got poked in the wrong places by";
            //message2 = "'s T. Burster";
            break;
        case MOD_TBURSTER2://charged
            message = "was turned into a pincushion by";
            //message2 = "'s T. Burster";
            break;
        case MOD_LSTORM:	//buster only
            message = "was forced to take a 'leaf' of absence by";
            //message2 = "'s L. Storm";
            break;
        case MOD_ASPLASHER:	//rapid
            message = "went on a crazy acid trip thanks to";
            //message2 = "'s A. Splasher;
            break;
            //Gravity attacks
        case MOD_GBOMB:	//charge
            message = "probably thinks that";
            message2 = "'s gravity bomb really sucks now";
            break;
        case MOD_GBOMB2:	//charged
            message = "probably thinks that";
            message2 = "'s gravity bomb really sucks now";
            break;
        case MOD_MMINE:	//buster only
            message = "found";
            message2 = "'s mines a bit too attractive";
            break;
        case MOD_DMATTER:	//rapid
            message = "got blotted out by";
            message2 = "'s Darkmatter";
            break;
            //Light attacks
        case MOD_BLASER://charge
            switched = qtrue;
            message = "gave";
            message2 = "some special LASIK surgery";
            break;
        case MOD_BLASER2://charged
            message = "saw a huge light at the end of";
            message2 = "tunnel";
            break;
        case MOD_SSHREDDER:	//buster only
            message = "tried to reach out and touch";
            message2 = "'s stars";
            break;
        case MOD_NBURST:	//rapid
            message = "was turned into reploid dust by";
            //message2 = "'s F. Cutter";
            break;
            //Electrical attacks
        case MOD_SFIST:	//charge
            message = "received quite a punch from";
            //message2 = "'s S. Fist";
            break;
        case MOD_SFIST2:	//charged
            switched = qtrue;
            message = "rained on";
            message2 = "'s parade";
            break;
        case MOD_TRTHUNDER:	//buster only
            message = "was shocked by";
            //message2 = "'s B. Shield";
            break;
        case MOD_BSHIELD:	//buster only
            message = "was bounced around by";
            //message2 = "'s B. Shield";
            break;
        case MOD_BLIGHTNING:	//rapid
            message = "was pretty shocked after seeing";
            message2 = "'s ball...of lightning";
            break;
        case MOD_TELEFRAG:
            message = "was in the way when";
            message2 = "beamed in";
            break;
        default:
            message = "was taken out of action by";
            break;
        }

        if (message)
        {
            if(switched)
            {
                CG_Printf( NULL,"^5[INFO]: ^5%s ^5%s ^5%s^5%s\n",
                           attackerName, message, targetName, message2);
            }
            else
            {
                CG_Printf( NULL,"^5[INFO]: ^5%s ^5%s ^5%s^5%s\n",
                           targetName, message, attackerName, message2);
            }
            return;
        }
    }

    // we don't know what it was
    CG_Printf( NULL,"^5[INFO]: ^5%s ^5died.\n", targetName );
}

//==========================================================================

/*
===============
CG_UseItem
===============
*/
static void CG_UseItem( centity_t *cent )
{
    int			itemNum;
    gitem_t		*item;
    entityState_t *es;

    es = &cent->currentState;

    itemNum = (es->event & ~EV_EVENT_BITS) - EV_USE_ITEM0;
    if ( itemNum < 0 || itemNum > HI_NUM_HOLDABLE )
    {
        itemNum = 0;
    }

    // print a message if the local player
    if ( es->number == cg.snap->ps.clientNum )
    {
        if ( !itemNum )
        {
            CG_CenterPrint( "No item to use", SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
        }
        else
        {
            item = BG_FindItemForHoldable( itemNum );
            CG_CenterPrint( va("Use %s", item->pickup_name), SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
        }
    }

    switch ( itemNum )
    {
    default:
    case HI_NONE:
        trap_S_StartSound (NULL, es->number, CHAN_BODY, cgs.media.useNothingSound );
        break;

    case HI_SUBTANK:
        trap_S_StartSound (NULL, es->number, CHAN_BODY, cgs.media.subtankSound );
        break;
    }

}

/*
================
CG_ItemPickup

A new item was picked up this frame
================
*/
static void CG_ItemPickup( int itemNum )
{
    cg.itemPickup = itemNum;
    cg.itemPickupTime = cg.time;
    cg.itemPickupBlendTime = cg.time;
    // see if it should be the grabbed weapon
    if ( bg_itemlist[itemNum].giType == IT_WEAPON )
    {
        // select it immediately
        if ( cg_autoswitch.integer )
        {
            cg.weaponSelectTime = cg.time;

            if( bg_itemlist[itemNum].giTag > RWP_ELECTRICAL )
            {
                cg.weaponSelect = ceil( (float)(bg_itemlist[itemNum].giTag - RWP_ELECTRICAL )/4.0f);
            }
            else
            {
                cg.weaponSelect = bg_itemlist[itemNum].giTag;
            }
        }
    }

}


/*
================
CG_PainEvent

Also called by playerstate transition
================
*/
void CG_PainEvent( centity_t *cent, int health )
{
    char	*snd;

    // don't do more than two pain sounds a second
    if ( cg.time - cent->pe.painTime < 500 )
    {
        return;
    }

    if ( health < 25 )
    {
        snd = "*pain25_1.wav";
    }
    else if ( health < 50 )
    {
        snd = "*pain50_1.wav";
    }
    else if ( health < 75 )
    {
        snd = "*pain75_1.wav";
    }
    else
    {
        snd = "*pain100_1.wav";
    }
    trap_S_StartSound( NULL, cent->currentState.number, CHAN_VOICE,
                       CG_CustomSound( cent->currentState.number, snd ) );

    // save pain time for programitic twitch animation
    cent->pe.painTime = cg.time;
    cent->pe.painDirection ^= 1;

    CG_SetScreenFlash(SF_PAIN);
}



/*
==============
CG_EntityEvent

An entity has an event value
also called by CG_CheckPlayerstateEvents
==============
*/
#define	DEBUGNAME(x) if(cg_debugEvents.integer){CG_Printf( NULL,x"\n");}
void CG_EntityEvent( centity_t *cent, vec3_t position )
{
    entityState_t	*es;
    int				event;
    vec3_t			dir;
    const char		*s;
    int				clientNum;
    clientInfo_t	*ci;


    es = &cent->currentState;
    event = es->event & ~EV_EVENT_BITS;

    if ( cg_debugEvents.integer )
    {
        CG_Printf( NULL,"ent:%3i  event:%3i ", es->number, event );
    }

    if ( !event )
    {
        DEBUGNAME("ZEROEVENT");
        return;
    }

    clientNum = es->clientNum;
    if ( clientNum < 0 || clientNum >= MAX_CLIENTS )
    {
        clientNum = 0;
    }
    ci = &cgs.clientinfo[ clientNum ];


    switch ( event )
    {
        //
        // movement generated events
        //
    case EV_FOOTSTEP:
        DEBUGNAME("EV_FOOTSTEP");
        if (cg_footsteps.integer)
        {
            trap_S_StartSound (NULL, es->number, CHAN_BODY,
                               cgs.media.footsteps[ ci->footsteps ][genrand_int32()&3] );
        }
        break;
    case EV_FOOTSTEP_METAL:
        DEBUGNAME("EV_FOOTSTEP_METAL");
        if (cg_footsteps.integer)
        {
            trap_S_StartSound (NULL, es->number, CHAN_BODY,
                               cgs.media.footsteps[ FOOTSTEP_METAL ][genrand_int32()&3] );
        }
        break;
    case EV_FOOTSPLASH:
        DEBUGNAME("EV_FOOTSPLASH");
        if (cg_footsteps.integer)
        {
            trap_S_StartSound (NULL, es->number, CHAN_BODY,
                               cgs.media.footsteps[ FOOTSTEP_SPLASH ][genrand_int32()&3] );
        }
        break;
    case EV_FOOTWADE:
        DEBUGNAME("EV_FOOTWADE");
        if (cg_footsteps.integer)
        {
            trap_S_StartSound (NULL, es->number, CHAN_BODY,
                               cgs.media.footsteps[ FOOTSTEP_SPLASH ][genrand_int32()&3] );
        }
        break;
    case EV_SWIM:
        DEBUGNAME("EV_SWIM");
        if (cg_footsteps.integer)
        {
            trap_S_StartSound (NULL, es->number, CHAN_BODY,
                               cgs.media.footsteps[ FOOTSTEP_SPLASH ][genrand_int32()&3] );
        }
        break;


    case EV_FALL_SHORT:
        DEBUGNAME("EV_FALL_SHORT");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.landSound );
        if ( clientNum == cg.predictedPlayerState.clientNum )
        {
            // smooth landing z changes
            cg.landChange = -8;
            cg.landTime = cg.time;
        }
        break;
    case EV_FALL_MEDIUM:
        DEBUGNAME("EV_FALL_MEDIUM");
        // use normal pain sound
        trap_S_StartSound( NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*pain100_1.wav" ) );
        if ( clientNum == cg.predictedPlayerState.clientNum )
        {
            // smooth landing z changes
            cg.landChange = -16;
            cg.landTime = cg.time;
        }
        break;
    case EV_FALL_FAR:
        DEBUGNAME("EV_FALL_FAR");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*fall1.wav" ) );
        cent->pe.painTime = cg.time;	// don't play a pain sound right after this
        if ( clientNum == cg.predictedPlayerState.clientNum )
        {
            // smooth landing z changes
            cg.landChange = -24;
            cg.landTime = cg.time;
        }
        break;

    case EV_STEP_4:
    case EV_STEP_8:
    case EV_STEP_12:
    case EV_STEP_16:		// smooth out step up transitions
        DEBUGNAME("EV_STEP");
        {
            float	oldStep;
            int		delta;
            int		step;

            if ( clientNum != cg.predictedPlayerState.clientNum )
            {
                break;
            }
            // if we are interpolating, we don't need to smooth steps
            if ( cg.demoPlayback || (cg.snap->ps.pm_flags & PMF_FOLLOW) ||
                    cg_nopredict.integer || cg_synchronousClients.integer )
            {
                break;
            }
            // check for stepping up before a previous step is completed
            delta = cg.time - cg.stepTime;
            if (delta < STEP_TIME)
            {
                oldStep = cg.stepChange * (STEP_TIME - delta) / STEP_TIME;
            }
            else
            {
                oldStep = 0;
            }

            // add this amount
            step = 4 * (event - EV_STEP_4 + 1 );
            cg.stepChange = oldStep + step;
            if ( cg.stepChange > MAX_STEP_CHANGE )
            {
                cg.stepChange = MAX_STEP_CHANGE;
            }
            cg.stepTime = cg.time;
            break;
        }

    case EV_JUMP_PAD:
        DEBUGNAME("EV_JUMP_PAD");
//		CG_Printf( NULL,"EV_JUMP_PAD w/effect #%i\n", es->eventParm );
        {
            localEntity_t	*smoke;
            vec3_t			up = {0, 0, 1};


            smoke = CG_SmokePuff( cent->lerpOrigin, up,
                                  32,
                                  1, 1, 1, 0.33f,
                                  1000,
                                  cg.time, 0,
                                  LEF_PUFF_DONT_SCALE,
                                  cgs.media.smokePuffShader );
        }

        // boing sound at origin, jump sound on player
        trap_S_StartSound ( cent->lerpOrigin, -1, CHAN_VOICE, cgs.media.jumpPadSound );
        trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*jump1.wav" ) );
        break;

    case EV_JUMP:
        DEBUGNAME("EV_JUMP");
        trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*jump1.wav" ) );
        break;
    case EV_TAUNT:
        DEBUGNAME("EV_TAUNT");
        trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*taunt.wav" ) );
        break;
    case EV_WATER_TOUCH:
        DEBUGNAME("EV_WATER_TOUCH");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrInSound );
        break;
    case EV_WATER_LEAVE:
        DEBUGNAME("EV_WATER_LEAVE");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrOutSound );
        break;
    case EV_WATER_UNDER:
        DEBUGNAME("EV_WATER_UNDER");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrUnSound );
        break;
    case EV_WATER_CLEAR:
        DEBUGNAME("EV_WATER_CLEAR");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*gasp.wav" ) );
        break;

    case EV_ITEM_PICKUP:
        DEBUGNAME("EV_ITEM_PICKUP");
        {
            gitem_t	*item;
            int		index;

            index = es->eventParm;		// player predicted

            if ( index < 1 || index >= bg_numItems )
            {
                break;
            }
            item = &bg_itemlist[ index ];

            // powerups and team items will have a separate global sound, this one
            // will be played at prediction time
            if ( item->giType == IT_POWERUP || item->giType == IT_TEAM)
            {
                trap_S_StartSound (NULL, es->number, CHAN_AUTO,	cgs.media.n_healthSound );
            }
            else if (item->giType == IT_PERSISTANT_POWERUP)
            {
            }
            else
            {
                trap_S_StartSound (NULL, es->number, CHAN_AUTO,	trap_S_RegisterSound( item->pickup_sound, qfalse ) );
            }

            // show icon and name on status bar
            if ( es->number == cg.snap->ps.clientNum )
            {
                CG_ItemPickup( index );
            }
        }
        break;

    case EV_GLOBAL_ITEM_PICKUP:
        DEBUGNAME("EV_GLOBAL_ITEM_PICKUP");
        {
            gitem_t	*item;
            int		index;

            index = es->eventParm;		// player predicted

            if ( index < 1 || index >= bg_numItems )
            {
                break;
            }
            item = &bg_itemlist[ index ];
            // powerup pickups are global
            if( item->pickup_sound )
            {
                trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, trap_S_RegisterSound( item->pickup_sound, qfalse ) );
            }

            // show icon and name on status bar
            if ( es->number == cg.snap->ps.clientNum )
            {
                CG_ItemPickup( index );
            }
        }
        break;

        //
        // weapon events
        //
    case EV_NOAMMO:
        DEBUGNAME("EV_NOAMMO");
//		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.noAmmoSound );
        if ( es->number == cg.snap->ps.clientNum )
        {
            CG_OutOfAmmoChange();
        }
        break;
    case EV_CHANGE_WEAPON:
        DEBUGNAME("EV_CHANGE_WEAPON");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.selectSound );
        break;
    case EV_FIRE_WEAPON:
        DEBUGNAME("EV_FIRE_WEAPON");
        CG_FireWeapon( cent );
        break;
    case EV_FIRE_OFFHAND:
        DEBUGNAME("EV_FIRE_OFFHAND");
        CG_FireOffhand( cent );
        break;

    case EV_USE_ITEM0:
        DEBUGNAME("EV_USE_ITEM0");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM1:
        DEBUGNAME("EV_USE_ITEM1");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM2:
        DEBUGNAME("EV_USE_ITEM2");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM3:
        DEBUGNAME("EV_USE_ITEM3");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM4:
        DEBUGNAME("EV_USE_ITEM4");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM5:
        DEBUGNAME("EV_USE_ITEM5");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM6:
        DEBUGNAME("EV_USE_ITEM6");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM7:
        DEBUGNAME("EV_USE_ITEM7");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM8:
        DEBUGNAME("EV_USE_ITEM8");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM9:
        DEBUGNAME("EV_USE_ITEM9");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM10:
        DEBUGNAME("EV_USE_ITEM10");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM11:
        DEBUGNAME("EV_USE_ITEM11");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM12:
        DEBUGNAME("EV_USE_ITEM12");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM13:
        DEBUGNAME("EV_USE_ITEM13");
        CG_UseItem( cent );
        break;
    case EV_USE_ITEM14:
        DEBUGNAME("EV_USE_ITEM14");
        CG_UseItem( cent );
        break;
    case EV_EARTHQUAKE:
        CG_StartEarthquake(5,5000);			// HULK attack
        trap_S_StartSound(NULL,0,CHAN_AUTO,cgs.media.earthquakeSound);
        break;

    case EV_PLAYERSTOP:
        player_stop=cg.time+(es->eventParm&0x7F)*2000;
        if (es->eventParm&0x80) black_bars=1;
        break;

        //=================================================================

        //
        // other events
        //

    case EV_PLAYER_TELEPORT_IN:
        DEBUGNAME("EV_PLAYER_TELEPORT_IN");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.teleInSound );
        CG_SpawnEffect( cent, position );
        // Peter: Enter sound for player
        trap_S_StartSound( NULL, es->number, CHAN_BODY, CG_CustomSound( es->number, "*enter.wav" ) );
        break;

    case EV_PLAYER_TELEPORT_OUT:
        DEBUGNAME("EV_PLAYER_TELEPORT_OUT");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.teleOutSound );
        CG_SpawnEffect( cent, position );
        break;

    case EV_SKYLIGHTNING:
        DEBUGNAME("EV_SKYLIGHTNING");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.lightningStrike );
        CG_SkyLightning (position, 3, 16, 128, 128, 1000); // position, 3, 16, 128, 128, 1000 - Original Settings
        break;

    case EV_ITEM_POP:
        DEBUGNAME("EV_ITEM_POP");
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.respawnSound );
        break;
    case EV_ITEM_RESPAWN:
        DEBUGNAME("EV_ITEM_RESPAWN");
        cent->miscTime = cg.time;	// scale up from this
        trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.respawnSound );
        break;

    case EV_GRENADE_BOUNCE:
        DEBUGNAME("EV_GRENADE_BOUNCE");
        {
            localEntity_t	*smoke[3];
            int				k;
            vec3_t			up = {0, 0, 32};

            for( k = 0; k < 3; k++ )
            {
                up[0] = 15*crandom();
                up[1] = 15*crandom();
                smoke[k] = CG_SmokePuff( cent->lerpOrigin, up,
                                         20,
                                         1, 1, 1, 0.33f,
                                         1000,
                                         cg.time, 0,
                                         0,
                                         cgs.media.smokePuffShader );
            }
        }

        CG_ExplosionParticles( es->weapon, cent->lerpOrigin );
        if ( genrand_int32() & 1 )
        {
            trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.hgrenb1aSound );
        }
        else
        {
            trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.hgrenb2aSound );
        }
        break;

    case EV_GRAVITYWELL:
        DEBUGNAME("EV_GRAVITYWELL");
        CG_GravityWell( es->pos.trBase );
        break;

    case EV_SCOREPLUM:
        DEBUGNAME("EV_SCOREPLUM");
        CG_ScorePlum( cent->currentState.otherEntityNum, cent->lerpOrigin, cent->currentState.time );
        break;

        //
        // missile impacts
        //
    case EV_MISSILE_HIT:
        DEBUGNAME("EV_MISSILE_HIT");
        ByteToDir( es->eventParm, dir );
        CG_MissileHitPlayer( es->weapon, position, dir, es->otherEntityNum );
        break;

    case EV_MISSILE_MISS:
        DEBUGNAME("EV_MISSILE_MISS");
        ByteToDir( es->eventParm, dir );
        CG_MissileHitWall( es->weapon, 0, position, dir, IMPACTSOUND_DEFAULT );
        break;

    case EV_MISSILE_MISS_METAL:
        DEBUGNAME("EV_MISSILE_MISS_METAL");
        ByteToDir( es->eventParm, dir );
        CG_MissileHitWall( es->weapon, 0, position, dir, IMPACTSOUND_METAL );
        break;
    case EV_BLASER_HIT:
        DEBUGNAME("EV_BLASER_HIT");
        ByteToDir( es->eventParm, dir );
        CG_MissileHitPlayer( WP_BLASER, position, dir, es->otherEntityNum );
        break;

    case EV_BLASER_MISS:
        DEBUGNAME("EV_BLASER_MISS");
        ByteToDir( es->eventParm, dir );
        CG_MissileHitWall( WP_BLASER, 0, position, dir, IMPACTSOUND_DEFAULT );
        break;
    case EV_RAILTRAIL:
        DEBUGNAME("EV_RAILTRAIL");
        cent->currentState.weapon = WP_WSTRIKE; //some weird hack I have to do...
        // if the end was on a nomark surface, don't make an explosion
        CG_RailTrail( ci, es->origin2, es->pos.trBase );
        if ( es->eventParm != 255 )
        {
            ByteToDir( es->eventParm, dir );
            CG_MissileHitWall( es->weapon, es->clientNum, position, dir, IMPACTSOUND_DEFAULT );
        }
        break;

    case EV_LIGHTNING_DISCHARGE:
        DEBUGNAME("EV_LIGHTNING_DISCHARGE");
        CG_Lightning_Discharge (position, es->eventParm);	// eventParm is duration/size
        break;

    case EV_BULLET_HIT_WALL:
        DEBUGNAME("EV_BULLET_HIT_WALL");
        ByteToDir( es->eventParm, dir );
        CG_Bullet( es->pos.trBase, es->otherEntityNum, dir, qfalse, ENTITYNUM_WORLD );
        break;

    case EV_BULLET_HIT_FLESH:
        DEBUGNAME("EV_BULLET_HIT_FLESH");
        CG_Bullet( es->pos.trBase, es->otherEntityNum, dir, qtrue, es->eventParm );
        break;

    case EV_GENERAL_SOUND:
        DEBUGNAME("EV_GENERAL_SOUND");
        if ( cgs.gameSounds[ es->eventParm ] )
        {
            trap_S_StartSound (NULL, es->number, CHAN_VOICE, cgs.gameSounds[ es->eventParm ] );
        }
        else
        {
            s = CG_ConfigString( CS_SOUNDS + es->eventParm );
            trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, s ) );
        }
        break;

    case EV_GLOBAL_SOUND:	// play from the player's head so it never diminishes
        DEBUGNAME("EV_GLOBAL_SOUND");
        if ( cgs.gameSounds[ es->eventParm ] )
        {
            trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.gameSounds[ es->eventParm ] );
        }
        else
        {
            s = CG_ConfigString( CS_SOUNDS + es->eventParm );
            trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, CG_CustomSound( es->number, s ) );
        }
        break;

    case EV_GLOBAL_TEAM_SOUND:	// play from the player's head so it never diminishes
    {
        DEBUGNAME("EV_GLOBAL_TEAM_SOUND");
        switch( es->eventParm )
        {
        case GTS_RED_CAPTURE: // CTF: red team captured the blue flag, 1FCTF: red team captured the neutral flag
            if ( cgs.clientinfo[cg.clientNum].team == TEAM_RED )
                CG_AddBufferedSound( cgs.media.captureYourTeamSound );
            else
                CG_AddBufferedSound( cgs.media.captureOpponentSound );
            break;
        case GTS_BLUE_CAPTURE: // CTF: blue team captured the red flag, 1FCTF: blue team captured the neutral flag
            if ( cgs.clientinfo[cg.clientNum].team == TEAM_BLUE )
                CG_AddBufferedSound( cgs.media.captureYourTeamSound );
            else
                CG_AddBufferedSound( cgs.media.captureOpponentSound );
            break;
        case GTS_RED_RETURN: // CTF: blue flag returned, 1FCTF: never used
            if ( cgs.clientinfo[cg.clientNum].team == TEAM_RED )
                CG_AddBufferedSound( cgs.media.returnYourTeamSound );
            else
                CG_AddBufferedSound( cgs.media.returnOpponentSound );
            //
            CG_AddBufferedSound( cgs.media.blueFlagReturnedSound );
            break;
        case GTS_BLUE_RETURN: // CTF red flag returned, 1FCTF: neutral flag returned
            if ( cgs.clientinfo[cg.clientNum].team == TEAM_BLUE )
                CG_AddBufferedSound( cgs.media.returnYourTeamSound );
            else
                CG_AddBufferedSound( cgs.media.returnOpponentSound );
            //
            CG_AddBufferedSound( cgs.media.redFlagReturnedSound );
            break;

        case GTS_RED_TAKEN: // CTF: red team took blue flag, 1FCTF: blue team took the neutral flag
            // if this player picked up the flag then a sound is played in CG_CheckLocalSounds
            if (cg.snap->ps.powerups[PW_BLUEFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG])
            {
            }
            else
            {
                if (cgs.clientinfo[cg.clientNum].team == TEAM_BLUE)
                {
                    CG_AddBufferedSound( cgs.media.enemyTookYourFlagSound );
                }
                else if (cgs.clientinfo[cg.clientNum].team == TEAM_RED)
                {
                    CG_AddBufferedSound( cgs.media.yourTeamTookEnemyFlagSound );
                }
            }
            break;
        case GTS_BLUE_TAKEN: // CTF: blue team took the red flag, 1FCTF red team took the neutral flag
            // if this player picked up the flag then a sound is played in CG_CheckLocalSounds
            if (cg.snap->ps.powerups[PW_REDFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG])
            {
            }
            else
            {
                if (cgs.clientinfo[cg.clientNum].team == TEAM_RED)
                {
                    CG_AddBufferedSound( cgs.media.enemyTookYourFlagSound );
                }
                else if (cgs.clientinfo[cg.clientNum].team == TEAM_BLUE)
                {
                    CG_AddBufferedSound( cgs.media.yourTeamTookEnemyFlagSound );
                }
            }
            break;
        case GTS_REDTEAM_SCORED:
            CG_AddBufferedSound(cgs.media.redScoredSound);
            break;
        case GTS_BLUETEAM_SCORED:
            CG_AddBufferedSound(cgs.media.blueScoredSound);
            break;
        case GTS_REDTEAM_TOOK_LEAD:
            CG_AddBufferedSound(cgs.media.redLeadsSound);
            break;
        case GTS_BLUETEAM_TOOK_LEAD:
            CG_AddBufferedSound(cgs.media.blueLeadsSound);
            break;
        case GTS_TEAMS_ARE_TIED:
            CG_AddBufferedSound( cgs.media.teamsTiedSound );
            break;
        default:
            break;
        }
        break;
    }

    case EV_PAIN:
        // local player sounds are triggered in CG_CheckLocalSounds,
        // so ignore events on the player
        DEBUGNAME("EV_PAIN");
        if ( cent->currentState.number != cg.snap->ps.clientNum )
        {
            CG_PainEvent( cent, es->eventParm );
            cent->hitTime = cg.time + 500;
            CG_SetScreenFlash(SF_PAIN);
        }
        break;

    case EV_DEATH1:
    case EV_DEATH2:
    case EV_DEATH3:
        DEBUGNAME("EV_DEATHx");
        //PM_StartTorsoAnim( anim );
        cent->pe.deathTime = cg.time + 5000;
        trap_S_StartSound( NULL, es->number, CHAN_VOICE,
                           CG_CustomSound( es->number, va("*death%i.wav", event - EV_DEATH1 + 1) ) );
        break;

    case EV_OBITUARY:
        DEBUGNAME("EV_OBITUARY");
        CG_Obituary( es );
        break;

    case EV_GIB_PLAYER:
        DEBUGNAME("EV_GIB_PLAYER");
        trap_S_StartSound( NULL, es->number, CHAN_BODY, CG_CustomSound( es->number, "*death1.wav" ) );
        cent->pe.deathTime = cg.time + 5000; // This doesnt seem to work ..?
        //CG_Printf( NULL,"deathTime is %i\n", cent->pe.deathTime);
        // FIXME: DonX // Make new death sphere explosion function
        CG_MakeSphericalExplosion (dir, 250, cgs.media.xbExplosionSphere, cgs.media.xbExplosionBlueShader, LEF_PUFF_FADE_RGB, 300 );
        CG_DeathSpheres( cent->lerpOrigin, CG_ClassforPlayer( ci->modelName, ci->skinName ) );
        break;

    case EV_BREAK_GLASS:
        DEBUGNAME("EV_BREAK_GLASS");
        // Change cgs.media.gibSound to whatever sound you want it to use
        // I think the gib sound sounds pretty good
        trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.glassbreakSound );
        CG_BreakGlass( cent->lerpOrigin );
        break;

    case EV_EXPLOSIVE:
        DEBUGNAME("EV_EXPLOSIVE");
        CG_MakeSphericalExplosion( cent->lerpOrigin, 100, cgs.media.xbExplosionSphere, cgs.media.xbExplosionShader1, LEF_PUFF_FADE_RGB, 300 );
        CG_SmokePuff (cent->lerpOrigin, vec3_origin, 10, 1, 1, 1, 1, 1000, cg.time, 0, 0, cgs.media.smokePuffShader);
        trap_S_StartSound (NULL, es->number, CHAN_BODY, cgs.media.sfx_rockexp);
        break;

    case EV_STOPLOOPINGSOUND:
        DEBUGNAME("EV_STOPLOOPINGSOUND");
        trap_S_StopLoopingSound( es->number );
        es->loopSound = 0;
        break;

    case EV_DEBUG_LINE:
        DEBUGNAME("EV_DEBUG_LINE");
        CG_Beam( cent );
        break;
    case EV_NOCHARGE:
        DEBUGNAME("EV_NOCHARGE");
        cent->chargelevel = 0;
        break;
    case EV_CHARGE1:
        DEBUGNAME("EV_CHARGE1");
        cent->chargelevel = 1;
        trap_S_StartSound (NULL, es->number, CHAN_WEAPON, cgs.media.chargeStartSound );
        break;
    case EV_CHARGE2:
        DEBUGNAME("EV_CHARGE2");
        cent->chargelevel = 2;
        trap_S_StartSound (NULL, es->number, CHAN_WEAPON, trap_S_RegisterSound("sound/player/mmx/charge3.wav", qfalse) );
        break;
    case EV_CHARGE3:
        DEBUGNAME("EV_CHARGE3");
        cent->chargelevel = 3;
        trap_S_StartSound (NULL, es->number, CHAN_WEAPON, trap_S_RegisterSound("sound/player/mmx/charge4.wav", qfalse) );
        CG_SetScreenFlash(SF_FLASH);
        break;
    case EV_CHARGE4:
        DEBUGNAME("EV_CHARGE4");
        cent->chargelevel = 4;
        break;
    default:
        DEBUGNAME("UNKNOWN");
        CG_Error( "Unknown event: %i", event );
        break;
    }

}


/*
==============
CG_CheckEvents

==============
*/
void CG_CheckEvents( centity_t *cent )
{
    // check for event-only entities
    if ( cent->currentState.eType > ET_EVENTS )
    {
        if ( cent->previousEvent )
        {
            return;	// already fired
        }
        // if this is a player event set the entity number of the client entity number
        if ( cent->currentState.eFlags & EF_PLAYER_EVENT )
        {
            cent->currentState.number = cent->currentState.otherEntityNum;
        }

        cent->previousEvent = 1;

        cent->currentState.event = cent->currentState.eType - ET_EVENTS;
    }
    else
    {
        // check for events riding with another entity
        if ( cent->currentState.event == cent->previousEvent )
        {
            return;
        }
        cent->previousEvent = cent->currentState.event;
        if ( ( cent->currentState.event & ~EV_EVENT_BITS ) == 0 )
        {
            return;
        }
    }

    // calculate the position at exactly the frame time
    BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, cent->lerpOrigin );
    CG_SetEntitySoundPosition( cent );
    CG_EntityEvent( cent, cent->lerpOrigin );
}

