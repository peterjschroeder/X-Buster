/**************************************************************************************************************************
 *                                                                                           		                  *
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

//#include "g_local.h"

void another_hour(void);
void weather_change(void);



void weather_and_time(void)
{
    another_hour();
    weather_change();
}



void another_hour(void)
{
    level.hours++;

    if (level.hours > 23)
    {
        level.hours -= 24;
        level.day++;

        if (level.day > 34)
        {
            level.day = 0;
            level.month++;

            if (level.month > 16)
            {
                level.month = 0;
                level.year++;
            }
        }
    }
}


void weather_change(void)
{
    int diff, change;
    if ((level.month >= 9) && (level.month <= 16))
        diff = (level.pressure > 985 ? -2 : 2);
    else
        diff = (level.pressure > 1015 ? -2 : 2);

    level.change += (dice(1, 4) * diff + dice(2, 6) - dice(2, 6));

    level.change = MIN(level.change, 12);
    level.change = MAX(level.change, -12);

    level.pressure += level.change;

    level.pressure = MIN(level.pressure, 1040);
    level.pressure = MAX(level.pressure, 960);

    change = 0;

    switch (level.sky)
    {
    case SKY_CLOUDLESS:
        if (level.pressure < 990)
            change = 1;
        else if (level.pressure < 1010)
            if (dice(1, 4) == 1)
                change = 1;
        break;
    case SKY_CLOUDY:
        if (level.pressure < 970)
            change = 2;
        else if (level.pressure < 990)
        {
            if (dice(1, 4) == 1)
                change = 2;
            else
                change = 0;
        }
        else if (level.pressure > 1030)
            if (dice(1, 4) == 1)
                change = 3;

        break;
    case SKY_RAINING:
    case SKY_SNOWING:
        if (level.pressure < 970)
        {
            if (dice(1, 4) == 1)
                change = 4;
            else
                change = 0;
        }
        else if (level.pressure > 1030)
            change = 5;
        else if (level.pressure > 1010)
            if (dice(1, 4) == 1)
                change = 5;

        break;
    case SKY_LIGHTNING:
        if (level.pressure > 1010)
            change = 6;
        else if (level.pressure > 990)
            if (dice(1, 4) == 1)
                change = 6;

        break;
    default:
        change = 0;
        level.sky = SKY_CLOUDLESS;
        break;
    }

    switch (change)
    {
    case 0:
        break;
    case 1:
        level.sky = SKY_CLOUDY;
        break;
    case 2:
        // Peter: If the last 2 months, or the first 2 months, than snow instead of rain
        // TODO: Do a temprature system, adds more realism
        if (level.month >= 15 || level.month <= 2)
        {
            level.sky = SKY_SNOWING;
            trap_SetConfigstring( CS_ATMOSEFFECT, "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,D=2400" );
        }
        else
        {
            level.sky = SKY_RAINING;
            trap_SetConfigstring( CS_ATMOSEFFECT, "T=RAIN,B=5 10,C=0.5,G=0.5 2,BV=50 50,GV=200 200,W=1 2,D=2000" );
        }
        break;
    case 3:
        level.sky = SKY_CLOUDLESS;
        trap_SetConfigstring( CS_ATMOSEFFECT, "T=NONE,B=0 0,C=0,G=0 0,BV=0 0,GV=0 0,W=0 2,D=0" );
        break;
    case 4:
        level.sky = SKY_LIGHTNING;
        break;
    case 5:
        level.sky = SKY_CLOUDY;
        break;
    case 6:
        level.sky = SKY_RAINING;
        trap_SetConfigstring( CS_ATMOSEFFECT, "T=RAIN,B=5 10,C=0.5,G=0.5 2,BV=50 50,GV=200 200,W=1 2,D=2000" );
        break;
    default:
        break;
    }
}

void earthquake(void)
{
    gentity_t	*tent;
    int j;

    // send the effect to players
    for (j = 0; j < level.maxclients; j++)
    {
        if (!g_entities[j].inuse || !g_entities[j].client )
            continue;

        tent = G_TempEntity( g_entities[j].client->ps.origin, EV_EARTHQUAKE );
        tent->s.clientNum = g_entities[j].s.clientNum;
    }

    // damage players on the ground and anything else that takes damage
    for (j = 0; j < level.maxclients; j++)
    {
        if ((g_entities[j].client && g_entities[j].s.groundEntityNum) ||
                g_entities[j].takedamage)
            G_Damage (&g_entities[j], NULL, NULL, NULL, NULL, 10, 0, MOD_EARTHQUAKE);
    }

    level.nextQuake = (level.time + (rand_number(1, 10)*1000000));
}

void lightning_strike(void)
{
    int j, lightning;
    gentity_t	*tent;
    gentity_t*	ent;
    trace_t		trace;
    vec3_t start;

    if (level.sky != SKY_RAINING && level.sky != SKY_LIGHTNING)
        return;

    lightning = rand_number(0, MAX_GENTITIES/3);

    for (j = 0; j < MAX_GENTITIES; j++)
    {
        ent = &g_entities [j];

        if (!ent->inuse)
            continue;

        if ( lightning == j )
        {
            VectorCopy( ent->r.currentOrigin, start );

            // Peter: Find the sky
            while (1)
            {
                trap_Trace( &trace, ent->r.currentOrigin, NULL, NULL, start, ENTITYNUM_NONE, MASK_SOLID|MASK_WATER );

                start[2]++;

                if( trace.surfaceFlags & SURF_SKY || start[2] >= 300 )
                    break;
            }

            trap_Trace( &trace, ent->r.currentOrigin, NULL, NULL, start, ENTITYNUM_NONE, CONTENTS_SOLID );
            if( trace.contents ) return;

            if (ent->client)
            {
                Com_Printf("3-1\n");
                ent->client->lstruck_time = level.time + 1000;

                SET_BIT( ent->client->ps.eFlags, EF_LIGHTNSTRUCK );
            }

            if (ent->takedamage)
                G_Damage (ent, NULL, NULL, NULL, NULL, 15, 0, MOD_LIGHTNING);

            tent = G_TempEntity( ent->r.currentOrigin, EV_SKYLIGHTNING );

            if (ent->client)
                tent->s.clientNum = ent->client->ps.clientNum;
        }
    }

    level.nextLightning = (level.time + (rand_number(1, 10)*1000));
}
