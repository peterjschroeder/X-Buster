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

//#include "g_local.h"



void do_upgrade(gentity_t *ent, char *name, int cmd, int subcmd)
{
    int i;
    int upgrade = 0, price = 0;

    if( !(ent->client->ps.stats[STAT_FLAGS] & SF_CAPSULE) && !g_cheats.integer)
    {
        return;
    }

    if (Q_stricmp( name, "all") == 0)
    {
        for (i = 0; i < 3; i++)
        {
            SET_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[i]));
            SET_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[i]));
            SET_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[i]));
            SET_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[i]));
            SET_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[i]));
        }
    }
    else if (Q_stricmp( name, "exit") == 0)
    {
        ent->client->menuTime = level.time + 1000;
        ent->client->ps.stats[STAT_FLAGS] &= ~SF_CAPSULE;
        ent->client->ps.powerups[PW_CLOAK] = 0;
        ent->client->noclip = qfalse;
        return;
    }

    // Peter: This could probably be cleaned up some rather than all these else if's
    else if (Q_stricmp( name, "head1") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[0];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[0]].price;
    }
    else if (Q_stricmp( name, "head2") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[1];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[1]].price;
    }
    else if (Q_stricmp( name, "head3") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[2];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[2]].price;
    }
    else if (Q_stricmp( name, "back1") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[0];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[0]].price;
    }
    else if (Q_stricmp( name, "back2") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[1];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[1]].price;
    }
    else if (Q_stricmp( name, "back3") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[2];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[2]].price;
    }
    else if (Q_stricmp( name, "body1") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[0];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[0]].price;
    }
    else if (Q_stricmp( name, "body2") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[1];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[1]].price;
    }
    else if (Q_stricmp( name, "body3") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[2];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[2]].price;
    }
    else if (Q_stricmp( name, "buster1") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[0];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[0]].price;
    }
    else if (Q_stricmp( name, "buster2") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[1];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[1]].price;
    }
    else if (Q_stricmp( name, "buster3") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[2];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[2]].price;
    }
    else if (Q_stricmp( name, "legs1") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[0];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[0]].price;
    }
    else if (Q_stricmp( name, "legs2") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[1];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[1]].price;
    }
    else if (Q_stricmp( name, "legs3") == 0)
    {
        upgrade = bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[2];
        price = upgrade_table[bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[2]].price;
    }

    //if ( ent->client->ps.persistant[PERS_CHIPS] >= price )
    {
        SET_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << upgrade));
        ent->client->ps.persistant[PERS_CHIPS] -= price;
        ent->client->menuTime = level.time + 1000;
        ent->client->ps.stats[STAT_FLAGS] &= ~SF_CAPSULE;
        ent->client->ps.powerups[PW_CLOAK] = 0;
        ent->client->noclip = qfalse;
    }
}

void do_downgrade(gentity_t *ent, char *name, int cmd, int subcmd)
{
    if (Q_stricmp( name, "all") == 0)
    {
        ent->client->ps.persistant[PERS_UPGRADES] = 0;
        G_Printf( "You downgraded everything!\n");
    }
    // Peter: This could probably be cleaned up some rather than all these else if's
    else if (Q_stricmp( name, "head1") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[0]));
    else if (Q_stricmp( name, "head2") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[1]));
    else if (Q_stricmp( name, "head3") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[2]));
    else if (Q_stricmp( name, "head4") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_head[3]));
    else if (Q_stricmp( name, "back1") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[0]));
    else if (Q_stricmp( name, "back2") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[1]));
    else if (Q_stricmp( name, "back3") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[2]));
    else if (Q_stricmp( name, "back4") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_back[3]));
    else if (Q_stricmp( name, "body1") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[0]));
    else if (Q_stricmp( name, "body2") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[1]));
    else if (Q_stricmp( name, "body3") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[2]));
    else if (Q_stricmp( name, "body4") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_body[3]));
    else if (Q_stricmp( name, "buster1") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[0]));
    else if (Q_stricmp( name, "buster2") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[1]));
    else if (Q_stricmp( name, "buster3") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[2]));
    else if (Q_stricmp( name, "buster4") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_buster[3]));
    else if (Q_stricmp( name, "legs1") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[0]));
    else if (Q_stricmp( name, "legs2") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[1]));
    else if (Q_stricmp( name, "legs3") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[2]));
    else if (Q_stricmp( name, "legs4") == 0)
        REMOVE_BIT(ent->client->ps.persistant[PERS_UPGRADES], (1 << bg_playerlist[ent->client->ps.persistant[PLAYERCLASS]].ug_legs[3]));
    else
    {
        G_Printf( "incorrect choice! valid downgrades are:\n");
        G_Printf( "all\n");
        G_Printf( "helmet\n");
        G_Printf( "chest\n");
        G_Printf( "back\n");
        G_Printf( "legs\n");
        G_Printf( "internal\n");
        G_Printf( "buster\n");
    }
}

void do_cloak(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if( !IS_SET(ent->client->ps.persistant[PERS_UPGRADES], (1 << UG_CLOAK)) )
        return;

    if (ent->client->ps.powerups[PW_CLOAK])
        ent->client->ps.powerups[PW_CLOAK] = 0;
    else
        ent->client->ps.powerups[PW_CLOAK] = INT_MAX;
}

void do_metamorph( gentity_t *ent )
{
}

void do_summon( gentity_t *ent )
{
}

void do_track( gentity_t *ent )
{
}
