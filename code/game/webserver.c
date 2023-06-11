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

/* Mini web-server, author Rob Siemborski -- mud@towers.crusoe.net
  made for Rom Mud 2.4b Unix
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

//#include "g_local.h"

#define MAXDATA 1024

typedef struct web_descriptor WEB_DESCRIPTOR;

struct web_descriptor
{
    int fd;
    char request[MAXDATA*2];
    struct sockaddr_in their_addr;
    int sin_size;
    WEB_DESCRIPTOR *next;
    qboolean valid;
};

WEB_DESCRIPTOR *web_desc_free;

/* FUNCTION DEFS */
int send_buf(int fd, const char* buf);
void handle_web_request(WEB_DESCRIPTOR *wdesc);
void handle_web_who_request(WEB_DESCRIPTOR *wdesc);
WEB_DESCRIPTOR *new_web_desc(void);
void free_web_desc(WEB_DESCRIPTOR *desc);

/* The mark of the end of a HTTP/1.x request */
const char ENDREQUEST[5] = { 13, 10, 13, 10, 0 }; /* (CRLFCRLF) */


WEB_DESCRIPTOR *web_descs;
int sockfd;
int top_web_desc;


void init_web(int port)
{
    struct sockaddr_in my_addr;

    web_descs = NULL;

    Com_Printf( "^3Web features starting on port: %d\n", port);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        Com_Printf( "web-socket");
        return;
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htons(INADDR_ANY);
    bzero(&(my_addr.sin_zero),8);

    if((bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr))) == -1)
    {
        Com_Printf( "web-bind");
        return;
    }

    /* Only listen for 5 connects at once */
    listen(sockfd, 5);
}

struct timeval ZERO_TIME = { 0, 0 };

void handle_web(void)
{
    int max_fd;
    WEB_DESCRIPTOR *current, *prev, *next;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    /* it *will* be atleast sockfd */
    max_fd = sockfd;

    /* add in all the current web descriptors */
    for(current=web_descs; current != NULL; current = current->next)
    {
        FD_SET(current->fd, &readfds);
        if(max_fd < current->fd)
            max_fd = current->fd;
    }

    /* Wait for ONE descriptor to have activity */
    select(max_fd+1, &readfds, NULL, NULL, &ZERO_TIME);

    if(FD_ISSET(sockfd, &readfds))
    {
        /* NEW CONNECTION -- INIT & ADD TO LIST */

        current = new_web_desc();
        current->sin_size  = sizeof(struct sockaddr_in);
        current->request[0] = '\0';

        if((current->fd = accept(sockfd, (struct sockaddr *)&(current->their_addr), &(current->sin_size))) == -1)
        {
            Com_Printf( "web-accept");
            return;
        }

        current->next = web_descs;
        web_descs = current;

        /* END ADDING NEW DESC */
    }

    /* DATA IN! */
    for(current=web_descs; current != NULL; current = current->next)
    {
        if (FD_ISSET(current->fd, &readfds)) /* We Got Data! */
        {
            char buf[MAXDATA];
            int numbytes;

            if((numbytes=read(current->fd,buf,sizeof(buf))) == -1)
            {
                Com_Printf( "web-read");
                return;
            }

            buf[numbytes] = '\0';

            strcat(current->request,buf);
        }
    } /* DONE WITH DATA IN */

    /* DATA OUT */
    for(current=web_descs; current != NULL; current = next )
    {
        next = current->next;

        if(strstr(current->request, "HTTP/1.") /* 1.x request (vernum on FIRST LINE) */
                && strstr(current->request, ENDREQUEST))
            handle_web_request(current);
        else if(!strstr(current->request, "HTTP/1.")
                &&  strchr(current->request, '\n')) /* HTTP/0.9 (no ver number) */
            handle_web_request(current);
        else
        {
            continue; /* Don't have full request yet! */
        }

        close(current->fd);

        if(web_descs == current)
        {
            web_descs = current->next;
        }
        else
        {
            for(prev=web_descs; prev->next != current; prev = prev->next)
                ; /* Just ititerate through the list */
            prev->next = current->next;
        }

        free_web_desc(current);
    }   /* END DATA-OUT */
}

/* Generic Utility Function */

int send_buf(int fd, const char* buf)
{
    return send(fd, buf, strlen(buf), 0);
}

void handle_web_request(WEB_DESCRIPTOR *wdesc)
{
    /* process request */
    /* are we using HTTP/1.x? If so, write out header stuff.. */
    if(!strstr(wdesc->request, "GET"))
    {
        send_buf(wdesc->fd,"HTTP/1.0 501 Not Implemented");
        return;
    }
    else if(strstr(wdesc->request, "HTTP/1."))
    {
        send_buf(wdesc->fd,"HTTP/1.0 200 OK\n");
        send_buf(wdesc->fd,"Content-type: text/html\n\n");
    }

    /* Handle the actual request */
    if(strstr(wdesc->request, "/wholist"))
        handle_web_who_request(wdesc);
    else
        send_buf(wdesc->fd,"Sorry, X-buster's Integrated Webserver 1.0 only supports /wholist");
}

void shutdown_web (void)
{
    WEB_DESCRIPTOR *current,*next;

    /* Close All Current Connections */
    for(current=web_descs; current != NULL; current = next)
    {
        next = current->next;
        close(current->fd);
        free_web_desc(current);
    }

    /* Stop Listening */
    close(sockfd);
}

void handle_web_who_request(WEB_DESCRIPTOR *wdesc)
{
    int count = 0;
    char output[BIG_INFO_STRING];
    int j;

    send_buf(wdesc->fd,"<HTML><HEAD><TITLE>X-Buster Who List</TITLE></HEAD>\n\r");
    send_buf(wdesc->fd,"<BODY BGCOLOR=\"#FFFFFF\"><B>X-Buster Who List</B><P>\n\r");

    for (j = 0; j < level.maxclients; j++)
    {
        if (!g_entities[j].inuse)
            continue;
        if (!g_entities[j].client)
            continue;

        count++;

        Com_sprintf(output, sizeof(output), "Score: %i Name: %s<BR>", g_entities[j].client->ps.persistant[PERS_SCORE], g_entities[j].client->pers.netname);
        send_buf(wdesc->fd,output);
    }

    sprintf(output, "<P>X-Buster's Who-List [%d players found]</BODY></HTML>", count);
    send_buf(wdesc->fd,output);
}


WEB_DESCRIPTOR *new_web_desc(void)
{
    WEB_DESCRIPTOR *desc;

    if (web_desc_free == NULL)
    {
        desc = G_Alloc(sizeof(*desc));
        top_web_desc++;
    }
    else
    {
        desc = web_desc_free;
        web_desc_free = web_desc_free->next;
    }

    desc->valid = qtrue;

    return desc;
}

void free_web_desc(WEB_DESCRIPTOR *desc)
{
    if(desc != NULL && desc->valid)
        return;

    desc->valid = qfalse;
    desc->next = web_desc_free;
    web_desc_free = desc;
}
