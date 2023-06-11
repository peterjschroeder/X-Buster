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


// NULL for everyone
void QDECL send_to_char( gentity_t *ent, const char *fmt, ... )
{
    char		msg[1024];
    va_list		argptr;
    char		*p;

    va_start (argptr,fmt);
    if (vsprintf (msg, fmt, argptr) > sizeof(msg))
    {
        G_Error ( "send_to_char overrun" );
    }
    va_end (argptr);

    // double quotes are bad
    while ((p = strchr(msg, '"')) != NULL)
        *p = '\'';

    trap_SendServerCommand ( ( (ent == NULL) ? -1 : ent-g_entities ), va("print \"%s\"", msg ));
}

void QDECL send_to_room( gentity_t *ent, const char *fmt, ... )
{
    char		msg[1024];
    va_list		argptr;
    char		*p;
    int j;

    va_start (argptr,fmt);
    if (vsprintf (msg, fmt, argptr) > sizeof(msg))
    {
        G_Error ( "send_to_room overrun" );
    }
    va_end (argptr);

    // double quotes are bad
    while ((p = strchr(msg, '"')) != NULL)
        *p = '\'';

    for (j = 0; j < level.maxclients; j++)
    {

        if (!g_entities[j].inuse)
            continue;
        if (!g_entities[j].client)
            continue;
        if (in_same_room(ent, &g_entities[j]))
            send_to_char ( &g_entities[j], msg );
    }
}


/*
==================
G_Say
==================
*/

void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message )
{
    char buf[BIG_INFO_STRING];
    char buf2[BIG_INFO_STRING];
    char	userinfo[MAX_INFO_STRING];
    char model[MAX_QPATH];
    int i;

    // check for ignore
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (ent->ignore[i] == NULL)
            break;

        if (strcmp(other->client->pers.netname, ent->ignore[i]))
            return;
    }

    if (!other || !other->inuse || !other->client ||
            other->client->pers.connected != CON_CONNECTED  ||
            (mode == SAY_TEAM  && !OnSameTeam(ent, other)) ||
            (mode == SAY_SAY && !in_same_room(ent, other)) ||
            (mode == SAY_PRIVATE && other->pchannel != ent->pchannel))
        return;

    // no chatting to players in tournements
    if ( (g_gametype.integer == GT_TOURNAMENT )
            && other->client->sess.sessionTeam == TEAM_FREE
            && ent->client->sess.sessionTeam != TEAM_FREE )
    {
        return;
    }

    switch (mode)
    {
    case SAY_SAY:
        sprintf(buf, "say");
        sprintf(buf2, "[SAY]:");
        break;
    case SAY_TELL:
        sprintf(buf, "tell");
        sprintf(buf2, "[TELL]:");
        break;
    case SAY_TEAM:
        sprintf(buf, "tchat");
        sprintf(buf2, "[TCHAT]:");
        break;
    case SAY_PRIVATE:
        sprintf(buf, "psay");
        sprintf(buf2, "[PSAY]:");
        break;
    default:
        sprintf(buf, "chat");
        sprintf(buf2, "[CHAT]:");
        break;
    }

    trap_GetUserinfo( ent->client->ps.clientNum, userinfo, sizeof( userinfo ) );
    Q_strncpyz( model, Info_ValueForKey (userinfo, "model"), sizeof( model ) );

    trap_SendServerCommand( other-g_entities, va("%s %s \"%c%c%s %c%c%s%c%c%s\n\"",
                            buf, model, Q_COLOR_ESCAPE, color, buf2,
                            Q_COLOR_ESCAPE, color, name,
                            Q_COLOR_ESCAPE, color, message));
}

#define EC		"\x19"

void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText )
{
    int			j;
    gentity_t	*other;
    int			color;
    char		name[64];
    // don't let text be too long for malicious reasons
    char		text[MAX_SAY_TEXT];
    char		location[64];

    if ( g_gametype.integer < GT_TEAM && mode == SAY_TEAM )
    {
        mode = SAY_CHAT;
    }

    switch ( mode )
    {
    default:
    case SAY_CHAT:
        Com_sprintf (name, sizeof(name), "%s%c%c"EC" says, '", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_YELLOW );
        color = COLOR_YELLOW;
        break;
    case SAY_TEAM:
        if (Team_GetLocationMsg(ent, location, sizeof(location)))
            Com_sprintf (name, sizeof(name), EC"(%s%c%c"EC") (%s)"EC" says, '",
                         ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_GREEN, location);
        else
            Com_sprintf (name, sizeof(name), EC"(%s%c%c"EC")"EC" says, '",
                         ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_GREEN );
        color = COLOR_GREEN;
        break;
    case SAY_TELL:
        if (target && g_gametype.integer >= GT_TEAM &&
                target->client->sess.sessionTeam == ent->client->sess.sessionTeam &&
                Team_GetLocationMsg(ent, location, sizeof(location)))
            Com_sprintf (name, sizeof(name), EC"%s%c%c"EC" (%s)"EC" says, '", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_RED, location );
        else
            Com_sprintf (name, sizeof(name), EC"%s%c%c"EC""EC" says, '", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_RED );
        color = COLOR_RED;
        break;
    case SAY_SAY:
        Com_sprintf (name, sizeof(name), "%s%c%c"EC" says, '", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_GREEN );
        color = COLOR_GREEN;
        break;
    case SAY_PRIVATE:
        Com_sprintf (name, sizeof(name), "%s%c%c"EC" says, '", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_BLUE );
        color = COLOR_BLUE;
        break;
    }

    Q_strncpyz( text, chatText, sizeof(text) );

    // Check for snoop
    for (j = 0; j < level.maxclients; j++)
    {
        if (!g_entities[j].inuse)
            continue;
        if (!g_entities[j].client)
            continue;
        if (!in_same_room(ent, &g_entities[j]))
            continue;

        if (g_entities[j].snoop && g_entities[j].snoop == ent)
            send_to_char( g_entities[j].snoop, "[SNOOP]: %s says, '%s'\n", ent->client->pers.netname, text );
    }

    if ( target )
    {
        G_SayTo( ent, target, mode, color, name, va("%s'", text) );
        return;
    }

    // send it to all the apropriate clients
    for (j = 0; j < level.maxclients; j++)
    {
        other = &g_entities[j];
        G_SayTo( ent, other, mode, color, name, va("%s'", text) );
    }


}


void do_say(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    if ( !*argument )
    {
        return;
    }

    G_Say( ent, NULL, subcmd, argument );
}


void do_tell(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t	*target;
    char		arg1[MAX_TOKEN_CHARS];
    char		arg2[MAX_TOKEN_CHARS];

    half_chop(argument, arg1, arg2);

    if ( !*arg1 || !*arg2 )
    {
        send_to_char( ent, "Tell who what?.\n" );
        return;
    }

    if (!(target = get_char_world(arg1)))
    {
        send_to_char( ent, "No such character.\n" );
        return;
    }

    G_LogPrintf( "tell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, arg2 );
    G_Say( ent, target, SAY_TELL, arg2 );
    // don't tell to the player self if it was already directed to this player
    // also don't send the chat back to a bot
    if ( ent != target && !(ent->r.svFlags & SVF_BOT))
    {
        G_Say( ent, ent, SAY_TELL, arg2 );
    }
}

void do_private_channel(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *vict;
    char	 buf[MAX_TOKEN_CHARS];
    char	 buf2[MAX_TOKEN_CHARS];
    int i;

    half_chop(argument, buf, buf2);

    switch (subcmd)
    {
    default:
    case PRIVATE_HELP:
        send_to_char(ent, "Private Channel (PC) commands\n");
        send_to_char(ent, "------------------------\n");
        send_to_char(ent, "popen   - opens your own private channel.\n");
        send_to_char(ent, "padd    - adds a player to your PC.\n");
        send_to_char(ent, "premove - remove a player from your PC.\n");
        send_to_char(ent, "pclose  - closes your private channel.\n");
        send_to_char(ent, "pwho    - lists all members on the current PC.\n");
        send_to_char(ent, "  NOTE: If you don't want to be added to another\n");
        send_to_char(ent, "        player's PC open your own with no players.\n");
        send_to_char(ent, "\nTo talk on the channel use -- private, psay or .\n");
        break;
    case PRIVATE_OPEN:
        if (ent->pchannel == ent->client->ps.clientNum)
        {
            send_to_char(ent, "Your Private Channel is already open.\n");
            return;
        }
        else
        {
            ent->pchannel = ent->client->ps.clientNum;
            send_to_char(ent, "You have just opened your own Private Channel.\n");
            return;
        }
        break;
        break;
    case PRIVATE_CLOSE:
        ent->pchannel  = -1;

        /* now remove all people on the private channel */
        for (i = 0; i < level.maxclients; i++)
        {
            if (!g_entities[i].inuse || !g_entities[i].client)
                continue;

            if ((g_entities[i].pchannel == ent->client->ps.clientNum) && (ent != &g_entities[i]))
            {
                g_entities[i].pchannel = -1;
                sprintf(buf, "%s has just closed their Private Channel.\n",
                        ent->client->pers.netname);
                send_to_char(&g_entities[i], buf);
            }
        }

        send_to_char(ent, "You have just CLOSED your Private Channel.\n");
        break;
    case PRIVATE_WHO:
        if (ent->pchannel == -1)
            send_to_char(ent, "You are not on a private channel\n");
        else
        {
            /* show all people on the private channel */
            send_to_char(ent, "Private Channel Members\n");
            send_to_char(ent, "-----------------------\n");

            for (i = 0; i < level.maxclients; i++)
            {
                if (!g_entities[i].inuse || !g_entities[i].client)
                    continue;

                if (g_entities[i].pchannel == ent->pchannel)
                {
                    sprintf(buf, "%s\n", g_entities[i].client->pers.netname);

                    send_to_char(ent, buf);
                }
            }
        }
        break;
    case PRIVATE_CHECK:
        /* show all people on the ALL private channels */
        send_to_char(ent, "Private Channels\n");
        send_to_char(ent, "---------------------------------------------\n");

        for (i = 0; i < level.maxclients; i++)
        {
            if (!g_entities[i].inuse || !g_entities[i].client)
                continue;

            sprintf(buf, "[%-5d]  %s\n",
                    g_entities[i].pchannel, g_entities[i].client->pers.netname);

            send_to_char(ent, buf);
        }
        break;
    case PRIVATE_REMOVE:
        if (!*buf)
            send_to_char(ent, "Who do you wish to add to you private channel?\n");
        else if (!(vict = get_char_world(buf)))
            send_to_char(ent, "No such player online.\n");
        else if (vict == ent)
            send_to_char(ent, "Use pclose to close your private channel.\n");
        else if (IS_SET(vict->r.svFlags, SVF_BOT))
            send_to_char(ent, "NPC's cannot be on private channels\n");
        else if (vict->pchannel != ent->client->ps.clientNum)
        {
            sprintf(buf, "%s is NOT on your Private Channel!\n",
                    vict->client->pers.netname);
            send_to_char(ent, buf);
        }
        else
        {
            vict->pchannel = -1;
            sprintf(buf,"You have been REMOVED from %s's Private Channel!\n",
                    ent->client->pers.netname);
            send_to_char(vict, buf);
            sprintf(buf,"%s has been REMOVED from your Private Channel!\n",
                    vict->client->pers.netname);
            send_to_char(ent, buf);
        }
        break;
    case PRIVATE_ADD:
        if (ent->pchannel != ent->client->ps.clientNum)
            send_to_char(ent, "You must open your own private channel first\n");
        else if (!*buf)
            send_to_char(ent, "Who do you wish to add to you private channel?\n");
        else if (!(vict = get_char_world(buf)))
            send_to_char(ent, "No such player online.\n");
        else if (vict == ent)
            send_to_char(ent, "You already on your private channel.\n");
        else if (IS_SET(vict->r.svFlags, SVF_BOT))
            send_to_char(ent, "NPC's cannot be added to private channels\n");
        else if (vict->pchannel != -1)
        {
            sprintf(buf,"%s is already on another private channel!\n",
                    vict->client->pers.netname);
            send_to_char(ent, buf);
        }
        else
        {
            vict->pchannel = ent->client->ps.clientNum;
            sprintf(buf,"You have been ADDED to %s's Private Channel!\n",
                    ent->client->pers.netname);
            send_to_char(vict, buf);
            sprintf(buf,"%s has been ADDED to your Private Channel!\n",
                    vict->client->pers.netname);
            send_to_char(ent, buf);
        }
        break;
    }
}

void do_ignore(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    gentity_t *victim;
    int i;
    qboolean found = qfalse;

    if (!*argument)
    {
        if (ent->ignore[0] == NULL)
        {
            send_to_char(ent, "You are not ignoring anyone.\n");
            return;
        }

        send_to_char(ent, "You are currently ignoring:\r");

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (ent->ignore[i] == NULL)
                break;

            send_to_char(ent, "    %s\n", ent->ignore[i]);
        }

        return;
    }


    for (i = 0; i < level.maxclients; i++)
    {
        if (!g_entities[i].inuse || !g_entities[i].client)
            continue;

        if (!strcmp(argument, g_entities[i].client->pers.netname))
        {
            found = qtrue;

            victim = &g_entities[i];

            if (victim == ent)
            {
                send_to_char(ent, "You ignore yourself for a moment, but it passes.\n");
                return;
            }

            if (!strcmp(argument, ent->ignore[i]))
            {
                send_to_char(ent, "You have already forgotten that person.\n");
                return;
            }

            /* make a new forget */
            ent->ignore[i]		= strdup(argument);
            send_to_char(ent, "You are now ignoring %s.\n", argument);
        }
    }

    if (!found)
    {
        send_to_char(ent, "No one by that name is playing.\n");
        return;
    }


}

void do_unignore(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    int i;
    qboolean found = qfalse;

    if (!*argument)
    {
        if (ent->ignore[0] == NULL)
        {
            send_to_char(ent, "You are not ignoring anyone.\n");
            return;
        }

        send_to_char(ent, "You are currently ignoring:\r");

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (ent->ignore[i] == NULL)
                break;

            send_to_char(ent, "    %s\n", ent->ignore[i]);
        }

        return;
    }

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (ent->ignore[i] == NULL)
            break;

        if (found)
        {
            ent->ignore[i-1]	= ent->ignore[i];
            ent->ignore[i]		= NULL;
            continue;
        }

        if(!strcmp(argument, ent->ignore[i]))
        {
            send_to_char(ent, "Ignore removed.\n");
            ent->ignore[i] = '\0';
            ent->ignore[i] = NULL;
            found = qtrue;
        }
    }

    if (!found)
        send_to_char(ent, "No one by that name is forgotten.\n");
}
