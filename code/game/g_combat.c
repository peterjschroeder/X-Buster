/**************************************************************************************************************************
 *                                                                                               		          *
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

#include "g_local.h"


/*
============
ScorePlum
============
*/
void ScorePlum( gentity_t *ent, vec3_t origin, int score )
{
    gentity_t *plum;

    plum = G_TempEntity( origin, EV_SCOREPLUM );
    // only send this temp entity to a single client
    plum->r.svFlags |= SVF_SINGLECLIENT;
    plum->r.singleClient = ent->s.number;
    //
    plum->s.otherEntityNum = ent->s.number;
    plum->s.time = score;
}

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, vec3_t origin, int score )
{
    if ( !ent->client )
    {
        return;
    }
    // no scoring during pre-match warmup
    if ( level.warmupTime )
    {
        return;
    }
    // show score plum
    ScorePlum(ent, origin, score);
    //
    ent->client->ps.persistant[PERS_SCORE] += score;
    if ( g_gametype.integer == GT_TEAM )
        level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
    CalculateRanks();
}

/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
void TossClientItems( gentity_t *self )
{
    gitem_t		*item;
    int			weapon;
    float		angle;
    int			i;
    gentity_t	*drop;

    // drop the weapon if not a gauntlet or machinegun
    weapon = self->s.weapon;

    if ( weapon > WP_DEFAULT )
    {
        // find the item type for this weapon
        item = BG_FindItemForWeapon( bg_playerlist[self->client->pers.playerclass].weapons[weapon].weapon );
        //item->giTag = weapon;
        //G_Printf( "weapon is %i\n", weapon );
        //G_Printf( "item->weapon is %i\n", item->giTag );
        //item->giTag = weapon;

        // spawn the item
        Drop_Item( self, item, 0 );
    }
    // drop some credits
    item = BG_FindItemForChip( HI_CHIPS );
    Drop_Item( self, item, 135 );


    // drop all the powerups if not in teamplay
    if ( g_gametype.integer != GT_TEAM )
    {
        angle = 45;
        for ( i = 1 ; i < PW_NUM_POWERUPS ; i++ )
        {
            if ( self->client->ps.powerups[ i ] > level.time )
            {
                item = BG_FindItemForPowerup( i );
                if ( !item )
                {
                    continue;
                }
                drop = Drop_Item( self, item, angle );
                // decide how many seconds it has left
                drop->count = ( self->client->ps.powerups[ i ] - level.time ) / 1000;
                if ( drop->count < 1 )
                {
                    drop->count = 1;
                }
                angle += 45;
            }
        }
    }
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker )
{
    vec3_t		dir;
    vec3_t		angles;

    if ( attacker && attacker != self )
    {
        VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
    }
    else if ( inflictor && inflictor != self )
    {
        VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
    }
    else
    {
        self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
        return;
    }

    self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );

    angles[YAW] = vectoyaw ( dir );
    angles[PITCH] = 0;
    angles[ROLL] = 0;
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer )
{
    G_AddEvent( self, EV_GIB_PLAYER, killer );
    self->takedamage = qfalse;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
    if ( self->health > GIB_HEALTH )
    {
        return;
    }
    GibEntity( self, 0 );
}


// these are just for logging, the client prints its own messages
char	*modNames[] =
{
    "MOD_UNKNOWN",
    "MOD_GAUNTLET",
    "MOD_MACHINEGUN",
    "MOD_GRENADE",
    "MOD_GRENADE_SPLASH",
    "MOD_ROCKET",
    "MOD_ROCKET_SPLASH",
    "MOD_PLASMA",
    "MOD_PLASMA_SPLASH",
    "MOD_RAILGUN",
    "MOD_LIGHTNING",
    "MOD_WATER",
    "MOD_SLIME",
    "MOD_LAVA",
    "MOD_CRUSH",
    "MOD_TELEFRAG",
    "MOD_FALLING",
    "MOD_SUICIDE",
    "MOD_TARGET_LASER",
    "MOD_TRIGGER_HURT",
    //Default attacks (add charged versions)
    "MOD_XBUSTER",		//MMX's main cannon
    "MOD_XBUSTER2",		//supercharged level 3 version
    "MOD_MBUSTER",		//MM's main cannon
    "MOD_FBUSTER",		//Forte's main weapon of destruction
    "MOD_ZBUSTER",		//Zero's arm cannon
    //Flame attackes
    "MOD_FCUTTER", //charge
    "MOD_FCUTTER2", //charged
    "MOD_HWAVE",	//buster only
    "MOD_PSTRIKE",	//rapid
    //Water attacks
    "MOD_SICE",	//charge
    "MOD_SICE2",	//charged
    "MOD_FBURST",	//buster only
    "MOD_WSTRIKE",	//rapid
    //Air attacks
    "MOD_AFIST",	//charge
    "MOD_AFIST2",	//charged
    "MOD_AWAVE",	//buster only
    "MOD_TEMPEST",	//rapid
    //Earth attacks
    "MOD_BCRYSTAL",	//charge
    "MOD_BCRYSTAL2",	//charged
    "MOD_RSTONE",	//buster only
    "MOD_SBURST",	//rapid
    //Metal attacks
    "MOD_LTORPEDO",	//charge
    "MOD_LTORPEDO2",	//charged
    "MOD_BLAUNCHER",	//buster only
    "MOD_DSHOT",	//rapid
    //Nature attacks
    "MOD_TBURSTER",//charge
    "MOD_TBURSTER2",//charged
    "MOD_LSTORM",	//buster only
    "MOD_ASPLASHER",	//rapid
    //Gravity attacks
    "MOD_GBOMB",	//charge
    "MOD_GBOMB2",	//charged
    "MOD_MMINE",	//buster only
    "MOD_DMATTER",	//rapid
    //Light attacks
    "MOD_BLASER",//charge
    "MOD_BLASER2",//charged
    "MOD_SSHREDDER",	//buster only
    "MOD_NBURST",	//rapid
    //Electrical attacks
    "MOD_SFIST",	//charge
    "MOD_SFIST2",	//charged
    "MOD_TRTHUNDER",	//buster only
    "MOD_BSHIELD",	//buster only
    "MOD_BLIGHTNING",	//rapid
    "MOD_GRAPPLE"
};



/*
==================
player_die
==================
*/
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
    gentity_t	*ent;
//	int			anim;
    int			contents;
    int			killer;
    int			i;
    char		*killerName, *obit;

    if ( self->client->ps.pm_type == PM_DEAD )
    {
        return;
    }

    if ( level.intermissiontime )
    {
        return;
    }

    if (self->client && self->client->hook)
    {
        Weapon_HookFree(self->client->hook);
    }

    self->client->ps.pm_type = PM_DEAD;

    if ( attacker )
    {
        killer = attacker->s.number;
        if ( attacker->client )
        {
            killerName = attacker->client->pers.netname;
        }
        else
        {
            killerName = "<non-client>";
        }
    }
    else
    {
        killer = ENTITYNUM_WORLD;
        killerName = "<world>";
    }

    if ( killer < 0 || killer >= MAX_CLIENTS )
    {
        killer = ENTITYNUM_WORLD;
        killerName = "<world>";
    }

    if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) )
    {
        obit = "<bad obituary>";
    }
    else
    {
        obit = modNames[ meansOfDeath ];
    }

    G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n",
                killer, self->s.number, meansOfDeath, killerName,
                self->client->pers.netname, obit );

    // broadcast the death event to everyone
    ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
    ent->s.eventParm = meansOfDeath;
    ent->s.otherEntityNum = self->s.number;
    ent->s.otherEntityNum2 = killer;
    ent->r.svFlags = SVF_BROADCAST;	// send to everyone

    self->enemy = attacker;

    self->client->ps.persistant[PERS_KILLED]++;
    self->client->ps.persistant[PERS_CHIPS] -= 1;

    if (self->client->ps.persistant[PERS_CHIPS] < 0)
        self->client->ps.persistant[PERS_CHIPS] = 1;

    if (attacker && attacker->client)
    {
        attacker->client->lastkilled_client = self->s.number;

        if ( attacker == self || OnSameTeam (self, attacker ) )
        {
            AddScore( attacker, self->r.currentOrigin, -1 );
        }
        else
        {
            AddScore( attacker, self->r.currentOrigin, 1 );
            attacker->client->lastKillTime = level.time;

        }
    }
    else
    {
        AddScore( self, self->r.currentOrigin, -1 );
    }

    // Add team bonuses
    Team_FragBonuses(self, inflictor, attacker);

    // if I committed suicide, the flag does not fall, it returns.
    if (meansOfDeath == MOD_SUICIDE)
    {
        if ( self->client->ps.powerups[PW_NEUTRALFLAG] )  		// only happens in One Flag CTF
        {
            Team_ReturnFlag( TEAM_FREE );
            self->client->ps.powerups[PW_NEUTRALFLAG] = 0;
        }
        else if ( self->client->ps.powerups[PW_REDFLAG] )  		// only happens in standard CTF
        {
            Team_ReturnFlag( TEAM_RED );
            self->client->ps.powerups[PW_REDFLAG] = 0;
        }
        else if ( self->client->ps.powerups[PW_BLUEFLAG] )  	// only happens in standard CTF
        {
            Team_ReturnFlag( TEAM_BLUE );
            self->client->ps.powerups[PW_BLUEFLAG] = 0;
        }
    }

    // if client is in a nodrop area, don't drop anything (but return CTF flags!)
    contents = trap_PointContents( self->r.currentOrigin, -1 );
    if ( !( contents & CONTENTS_NODROP ))
    {
        TossClientItems( self );
    }
    else
    {
        if ( self->client->ps.powerups[PW_NEUTRALFLAG] )  		// only happens in One Flag CTF
        {
            Team_ReturnFlag( TEAM_FREE );
        }
        else if ( self->client->ps.powerups[PW_REDFLAG] )  		// only happens in standard CTF
        {
            Team_ReturnFlag( TEAM_RED );
        }
        else if ( self->client->ps.powerups[PW_BLUEFLAG] )  	// only happens in standard CTF
        {
            Team_ReturnFlag( TEAM_BLUE );
        }
    }

    do_score( self, NULL, 0, 0 );		// show scores
    // send updated scores to any clients that are following this one,
    // or they would get stale scoreboards
    for ( i = 0 ; i < level.maxclients ; i++ )
    {
        gclient_t	*client;

        client = &level.clients[i];
        if ( client->pers.connected != CON_CONNECTED )
        {
            continue;
        }
        if ( client->sess.sessionTeam != TEAM_SPECTATOR )
        {
            continue;
        }
        if ( client->sess.spectatorClient == self->s.number )
        {
            do_score( g_entities + i, NULL, 0, 0 );
        }
    }

    self->takedamage = qtrue;	// can still be gibbed

    self->s.weapon = WP_NONE;
    self->s.powerups = 0;

    /* Peter: Reset the pickup counters */
    for (i = 0; i<MAX_ITEMS; i++)
        self->pickup_max[i] = 0;

    self->r.contents = CONTENTS_CORPSE;

    self->s.angles[0] = 0;
    self->s.angles[2] = 0;
    LookAtKiller (self, inflictor, attacker);

    VectorCopy( self->s.angles, self->client->ps.viewangles );

    self->s.loopSound = 0;

    self->r.maxs[2] = -8;

    // don't allow respawn until the death anim is done
    // g_forcerespawn may force spawning at some later time
    self->client->respawnTime = level.time + 1700;

    // remove powerups
    memset( self->client->ps.powerups, 0, sizeof(self->client->ps.powerups) );

    GibEntity( self, killer );

    // the body can still be gibbed
    self->die = body_die;

    trap_LinkEntity (self);

}

// NIGHTZ - QUICK RESPAWN EXPLSOIVES
void doExplosiveDamage (gentity_t* targ);

void respawnExplosives (gentity_t* ent)
{
    ent->r.contents = -1;
    ent->s.eFlags &= ~EF_NODRAW;
    ent->r.svFlags &= ~SVF_NOCLIENT;
//	ent->health = ent->maxHealth;
    ent->takedamage = qtrue;
    trap_LinkEntity (ent);
}

void explosiveDie (gentity_t* self)
{
    self->health = -1;
    doExplosiveDamage (self);
}

void doExplosiveDamage (gentity_t* targ)
{
    if (targ->health <= 0)
    {
        gentity_t* effect;
        gentity_t* temp;
        int 		i, damage;
        float 	distance;
        vec3_t	throw;
        vec3_t	center;
        vec3_t	size;

        if (targ->model2)
            VectorCopy (targ->r.currentOrigin, center);
        else
        {
            VectorSubtract (targ->r.maxs, targ->r.mins, size);
            VectorScale (size, 0.5, size);
            VectorAdd (targ->r.mins, size, center);
        }

        effect = G_TempEntity (center, EV_EXPLOSIVE);
        targ->r.svFlags |= SVF_NOCLIENT;
        targ->s.eFlags |= EF_NODRAW;
        targ->r.contents = 0;
        targ->nextthink = level.time + targ->respawn;
        targ->think = respawnExplosives;
        targ->takedamage = qfalse;

        // Apply the damage
        for (i = 0; i < MAX_GENTITIES; i++)
        {
            vec3_t tempOrigin;
            temp = &g_entities[i];

            if (!temp->takedamage)
                continue;

            if ((temp->model2)||(temp->client))
                VectorCopy (temp->r.currentOrigin, tempOrigin);
            else
            {
                vec3_t tsize;

                VectorSubtract (temp->r.maxs, temp->r.mins, tsize);
                VectorScale (tsize, 0.5, tsize);
                VectorAdd (temp->r.mins, tsize, tempOrigin);
            }

            distance = Distance(center, tempOrigin);

            if (distance >= targ->radius)
                continue;

            VectorSubtract (center, tempOrigin, throw);
            VectorNormalize (throw);
            VectorNegate (throw, throw);
            VectorScale (throw, 1400, throw);

            damage = targ->damage/(distance/10);

            if (temp->client)
                VectorAdd (throw, temp->client->ps.velocity, temp->client->ps.velocity);

            if (temp->s.eType == ET_EXPLOSIVE)
            {
                temp->think = explosiveDie;
                temp->nextthink = level.time + 300;
            }
            else G_Damage (temp, NULL, targ, NULL, NULL, damage, DAMAGE_NO_PROTECTION, MOD_EXPLOSIVE);
        }
    }
}
// END NIGHTZ


/*
================
CheckArmor
================
*/
int CheckArmor (gentity_t *ent, int damage, int dflags)
{
    gclient_t	*client;
    int			save;
    int			count;

    if (!damage)
        return 0;

    client = ent->client;

    if (!client)
        return 0;

    if (dflags & DAMAGE_NO_ARMOR)
        return 0;

    // armor
    count = client->ps.stats[STAT_ARMOR];
    save = ceil( damage * ARMOR_PROTECTION );
    if (save >= count)
        save = count;

    if (!save)
        return 0;

    client->ps.stats[STAT_ARMOR] -= save;

    return save;
}


/*
================
RaySphereIntersections
================
*/
int RaySphereIntersections( vec3_t origin, float radius, vec3_t point, vec3_t dir, vec3_t intersections[2] )
{
    float b, c, d, t;

    //	| origin - (point + t * dir) | = radius
    //	a = dir[0]^2 + dir[1]^2 + dir[2]^2;
    //	b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
    //	c = (point[0] - origin[0])^2 + (point[1] - origin[1])^2 + (point[2] - origin[2])^2 - radius^2;

    // normalize dir so a = 1
    VectorNormalize(dir);
    b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
    c = (point[0] - origin[0]) * (point[0] - origin[0]) +
        (point[1] - origin[1]) * (point[1] - origin[1]) +
        (point[2] - origin[2]) * (point[2] - origin[2]) -
        radius * radius;

    d = b * b - 4 * c;
    if (d > 0)
    {
        t = (- b + sqrt(d)) / 2;
        VectorMA(point, t, dir, intersections[0]);
        t = (- b - sqrt(d)) / 2;
        VectorMA(point, t, dir, intersections[1]);
        return 2;
    }
    else if (d == 0)
    {
        t = (- b ) / 2;
        VectorMA(point, t, dir, intersections[0]);
        return 1;
    }
    return 0;
}


/*
============
G_LocationDamage
============
*/
int G_LocationDamage(vec3_t point, gentity_t* targ, gentity_t* attacker, int take)
{
    vec3_t bulletPath;
    vec3_t bulletAngle;

    int clientHeight;
    int clientFeetZ;
    int clientRotation;
    int bulletHeight;
    int bulletRotation;	// Degrees rotation around client.
    // used to check Back of head vs. Face
    int impactRotation;


    // First things first.  If we're not damaging them, why are we here?
    if (!take)
        return 0;

    // Point[2] is the REAL world Z. We want Z relative to the clients feet

    // Where the feet are at [real Z]
    clientFeetZ  = targ->r.currentOrigin[2] + targ->r.mins[2];
    // How tall the client is [Relative Z]
    clientHeight = targ->r.maxs[2] - targ->r.mins[2];
    // Where the bullet struck [Relative Z]
    bulletHeight = point[2] - clientFeetZ;

    // Get a vector aiming from the client to the bullet hit
    VectorSubtract(targ->r.currentOrigin, point, bulletPath);
    // Convert it into PITCH, ROLL, YAW
    vectoangles(bulletPath, bulletAngle);

    clientRotation = targ->client->ps.viewangles[YAW];
    bulletRotation = bulletAngle[YAW];

    impactRotation = abs(clientRotation-bulletRotation);

    impactRotation += 45; // just to make it easier to work with
    impactRotation = impactRotation % 360; // Keep it in the 0-359 range

    if (impactRotation < 90)
        targ->client->lasthurt_location = LOCATION_BACK;
    else if (impactRotation < 180)
        targ->client->lasthurt_location = LOCATION_RIGHT;
    else if (impactRotation < 270)
        targ->client->lasthurt_location = LOCATION_FRONT;
    else if (impactRotation < 360)
        targ->client->lasthurt_location = LOCATION_LEFT;
    else
        targ->client->lasthurt_location = LOCATION_NONE;

    // The upper body never changes height, just distance from the feet
    if (bulletHeight > clientHeight - 2)
        targ->client->lasthurt_location |= LOCATION_HEAD;
    else if (bulletHeight > clientHeight - 8)
        targ->client->lasthurt_location |= LOCATION_FACE;
    else if (bulletHeight > clientHeight - 10)
        targ->client->lasthurt_location |= LOCATION_SHOULDER;
    else if (bulletHeight > clientHeight - 16)
        targ->client->lasthurt_location |= LOCATION_CHEST;
    else if (bulletHeight > clientHeight - 26)
        targ->client->lasthurt_location |= LOCATION_STOMACH;
    else if (bulletHeight > clientHeight - 29)
        targ->client->lasthurt_location |= LOCATION_GROIN;
    else if (bulletHeight < 4)
        targ->client->lasthurt_location |= LOCATION_FOOT;
    else
        // The leg is the only thing that changes size when you duck,
        // so we check for every other parts RELATIVE location, and
        // whats left over must be the leg.
        targ->client->lasthurt_location |= LOCATION_LEG;



    // Check the location ignoring the rotation info
    switch ( targ->client->lasthurt_location &
             ~(LOCATION_BACK | LOCATION_LEFT | LOCATION_RIGHT | LOCATION_FRONT) )
    {
    case LOCATION_HEAD:
        take *= 1.8;
        break;
    case LOCATION_FACE:
        if (targ->client->lasthurt_location & LOCATION_FRONT)
            take *= 4.0; // Faceshots REALLY suck
        else
            take *= 1.8;
        break;
    case LOCATION_SHOULDER:
        if (targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK))
            take *= 1.4; // Throat or nape of neck
        else
            take *= 1.1; // Shoulders
        break;
    case LOCATION_CHEST:
        if (targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK))
            take *= 1.3; // Belly or back
        else
            take *= 0.8; // Arms
        break;
    case LOCATION_STOMACH:
        take *= 1.2;
        break;
    case LOCATION_GROIN:
        if (targ->client->lasthurt_location & LOCATION_FRONT)
            take *= 1.3; // Groin shot
        break;
    case LOCATION_LEG:
        take *= 0.7;
        targ->client->ps.speed -= take; // slowdown if shot in legs
        break;
    case LOCATION_FOOT:
        take *= 0.5;
        targ->client->ps.speed -= take; // slowdown if shot in foot
        break;

    }
    return take;

}

/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/

void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
               vec3_t dir, vec3_t point, int damage, int dflags, int mod )
{
    gclient_t	*client;
    int			take;
    int			save;
    int			knockback;
    int			max;

    if (!targ->takedamage)
    {
        return;
    }

    // the intermission has allready been qualified for, so don't
    // allow any extra scoring
    if ( level.intermissionQueued )
    {
        return;
    }

    if ( !inflictor )
    {
        inflictor = &g_entities[ENTITYNUM_WORLD];
    }
    if ( !attacker )
    {
        attacker = &g_entities[ENTITYNUM_WORLD];
    }

    // shootable doors / buttons don't actually have any health
    if ( targ->s.eType == ET_MOVER )
    {
        if ( targ->use && targ->moverState == MOVER_POS1 )
        {
            targ->use( targ, inflictor, attacker );
        }
        return;
    }

    // If we shot a breakable item subtract the damage from its health and try to break it
    if ( targ->s.eType == ET_BREAKABLE )
    {
        targ->health -= damage;
        G_BreakGlass( targ, point, mod );
        return;
    }

    // NIGHTZ - EXPLOSIVES
    if (targ->s.eType == ET_EXPLOSIVE)
    {
        targ->health -= damage;
        doExplosiveDamage (targ);
        return;
    }
    // END NIGHTZ

    // reduce damage by the attacker's handicap value
    // unless they are rocket jumping
    if ( attacker->client && attacker != targ )
    {
        max = attacker->client->ps.stats[STAT_MAX_HEALTH];
        damage = damage * max / 100;
    }

    client = targ->client;

    if ( client )
    {
        if ( client->noclip )
        {
            return;
        }
    }

    if ( !dir )
    {
        dflags |= DAMAGE_NO_KNOCKBACK;
    }
    else
    {
        VectorNormalize(dir);
    }

    knockback = damage;
    if ( knockback > 200 )
    {
        knockback = 200;
    }

    switch ( attacker->s.weapon )
    {
    case WP_TEMPEST:
        if( dflags & DAMAGE_RADIUS )
            knockback = 400;
        break;
    case WP_AFIST:
        knockback = 200;
        break;
    case WP_SFIST:
    case WP_SFIST2:
        knockback = 0;
        break;
    default:
        break;
    }
    if ( targ->flags & FL_NO_KNOCKBACK )
    {
        knockback = 0;
    }
    if ( dflags & DAMAGE_NO_KNOCKBACK )
    {
        knockback = 0;
    }

    // figure momentum add, even if the damage won't be taken
    if ( knockback && targ->client )
    {
        vec3_t	kvel;
        float	mass;

        mass = 200;

        VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
        VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

        // set the timer so that the other client can't cancel
        // out the movement immediately
        if ( !targ->client->ps.pm_time )
        {
            int		t;

            t = knockback * 2;
            if ( t < 50 )
            {
                t = 50;
            }
            if ( t > 200 )
            {
                t = 200;
            }
            targ->client->ps.pm_time = t;
            targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
        }
    }

    // check for completely getting out of the damage
    if ( !(dflags & DAMAGE_NO_PROTECTION) )
    {

        // if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
        // if the attacker was on the same team
        if ( targ != attacker && OnSameTeam (targ, attacker)  )
        {
            if ( !g_friendlyFire.integer )
            {
                return;
            }
        }

        // check for godmode
        if ( targ->flags & FL_GODMODE )
        {
            return;
        }
    }

    // add to the attacker's hit counter (if the target isn't a general entity like a prox mine)
    if ( attacker->client && targ != attacker && targ->health > 0
            && targ->s.eType != ET_MISSILE
            && targ->s.eType != ET_GENERAL)
    {
        if ( OnSameTeam( targ, attacker ) )
        {
            attacker->client->ps.persistant[PERS_HITS]--;
        }
        else
        {
            attacker->client->ps.persistant[PERS_HITS]++;
        }
        attacker->client->ps.persistant[PERS_ATTACKEE_ARMOR] = (targ->health<<8)|(client->ps.stats[STAT_ARMOR]);
    }

    // always give half damage if hurting self
    // calculated after knockback, so rocket jumping works
    if ( targ == attacker)
    {
        damage *= 0.5;
    }

    if ( damage < 1 )
    {
        damage = 1;
    }
    take = damage;
    save = 0;

    // check for player armor upgrades
    if( targ->client->ps.persistant[PERS_UPGRADES] & (1 << UG_ARMORBOOST) )
    {
        if ( (targ->client->pers.playerclass == CLASS_DRN00X) || (targ->client->pers.playerclass == CLASS_DWNINFINITY) )
            take = ceil(take*.75f);
        else
            take = ceil(take*.5f);
    }

    if ( g_debugDamage.integer )
    {
        G_Printf( "%i: client:%i health:%i damage:%i\n", level.time, targ->s.number,
                  targ->health, take );
    }

    // add to the damage inflicted on a player this frame
    // the total will be turned into screen blends and view angle kicks
    // at the end of the frame
    if ( client )
    {
        if ( attacker )
        {
            client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
        }
        else
        {
            client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
        }

        client->damage_blood += take;
        client->damage_knockback += knockback;
        if ( dir )
        {
            VectorCopy ( dir, client->damage_from );
            client->damage_fromWorld = qfalse;
        }
        else
        {
            VectorCopy ( targ->r.currentOrigin, client->damage_from );
            client->damage_fromWorld = qtrue;
        }
    }

    // See if it's the player hurting the emeny flag carrier
    if( g_gametype.integer == GT_CTF)
        Team_CheckHurtCarrier(targ, attacker);

    if (targ->client)
    {
        // set the last client who damaged the target
        targ->client->lasthurt_client = attacker->s.number;
        targ->client->lasthurt_mod = mod;

        // Modify the damage for location damage
        if (point && targ && targ->health > 0 && attacker && take && mod != MOD_XBUSTER2)
            take = G_LocationDamage(point, targ, attacker, take);
        else
            targ->client->lasthurt_location = LOCATION_NONE;
    }

    if (!IS_SET(targ->client->lasthurt_location, LOCATION_BACK) &&
            IS_SET(targ->client->pers.cmd.buttons, BUTTON_SHIELD))
        return;

    // do the damage
    if (take)
    {
        targ->health = targ->health - take;

        if( attacker->client && IS_SET(targ->client->ps.persistant[PERS_UPGRADES], (1 << UG_DRAIN)) )
        {
            attacker->health += take/8;
            attacker->client->ps.stats[STAT_HEALTH] = attacker->health;
        }

        if ( targ->client )
        {
            targ->client->ps.stats[STAT_HEALTH] = targ->health;
            // kick them off the wall
            targ->client->ps.pm_flags &= ~PMF_WALL_HANG;
            targ->client->ps.eFlags &= ~EF_WALLHANG;
        }

        if ( targ->health <= 0 )
        {
            if ( client )
                targ->flags |= FL_NO_KNOCKBACK;

            if (targ->health < -999)
                targ->health = -999;

            targ->enemy = attacker;
            targ->die (targ, inflictor, attacker, take, mod);
            return;
        }
        else if ( targ->pain )
        {
            targ->pain (targ, attacker, take);
        }
    }

}


/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage (gentity_t *targ, vec3_t origin)
{
    vec3_t	dest;
    trace_t	tr;
    vec3_t	midpoint;

    // use the midpoint of the bounds instead of the origin, because
    // bmodels may have their origin is 0,0,0
    VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
    VectorScale (midpoint, 0.5, midpoint);

    VectorCopy (midpoint, dest);
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0 || tr.entityNum == targ->s.number)
        return qtrue;

    // this should probably check in the plane of projection,
    // rather than in world coordinate, and also include Z
    VectorCopy (midpoint, dest);
    dest[0] += 15.0;
    dest[1] += 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] += 15.0;
    dest[1] -= 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] -= 15.0;
    dest[1] += 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] -= 15.0;
    dest[1] -= 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;


    return qfalse;
}


/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage ( vec3_t origin, gentity_t *attacker, float damage, float radius,
                          gentity_t *ignore, int mod)
{
    float		points, dist;
    gentity_t	*ent;
    int			entityList[MAX_GENTITIES];
    int			numListedEntities;
    vec3_t		mins, maxs;
    vec3_t		v;
    vec3_t		dir;
    int			i, e;
    qboolean	hitClient = qfalse;

    if ( radius < 1 )
    {
        radius = 1;
    }

    for ( i = 0 ; i < 3 ; i++ )
    {
        mins[i] = origin[i] - radius;
        maxs[i] = origin[i] + radius;
    }

    numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

    for ( e = 0 ; e < numListedEntities ; e++ )
    {
        ent = &g_entities[entityList[ e ]];

        if (ent == ignore)
            continue;
        if (!ent->takedamage)
            continue;

        // find the distance from the edge of the bounding box
        for ( i = 0 ; i < 3 ; i++ )
        {
            if ( origin[i] < ent->r.absmin[i] )
            {
                v[i] = ent->r.absmin[i] - origin[i];
            }
            else if ( origin[i] > ent->r.absmax[i] )
            {
                v[i] = origin[i] - ent->r.absmax[i];
            }
            else
            {
                v[i] = 0;
            }
        }

        dist = VectorLength( v );
        if ( dist >= radius )
        {
            continue;
        }

        points = damage * ( 1.0 - dist / radius );

        if( CanDamage (ent, origin) )
        {
            if( LogAccuracyHit( ent, attacker ) )
            {
                hitClient = qtrue;
            }
            VectorSubtract (ent->r.currentOrigin, origin, dir);
            // push the center of mass higher than the origin so players
            // get knocked into the air more
            dir[2] += 24;
            G_Damage (ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS, mod);
        }
    }

    return hitClient;
}

/*
============
G_WaterRadiusDamage for The SARACEN's Lightning Discharge
============
*/
qboolean G_WaterRadiusDamage (vec3_t origin, gentity_t *attacker, float damage, float radius,
                              gentity_t *ignore, int mod)
{
    float		points, dist;
    gentity_t	*ent;
    int		entityList[MAX_GENTITIES];
    int		numListedEntities;
    vec3_t		mins, maxs;
    vec3_t		v;
    vec3_t		dir;
    int		i, e;
    qboolean	hitClient = qfalse;

    if (!(trap_PointContents (origin, -1) & MASK_WATER)) return qfalse;
    // if we're not underwater, forget it!

    if (radius < 1) radius = 1;

    for (i = 0 ; i < 3 ; i++)
    {
        mins[i] = origin[i] - radius;
        maxs[i] = origin[i] + radius;
    }

    numListedEntities = trap_EntitiesInBox (mins, maxs, entityList, MAX_GENTITIES);

    for (e = 0 ; e < numListedEntities ; e++)
    {
        ent = &g_entities[entityList[e]];

        if (ent == ignore)			continue;
        if (!ent->takedamage)		continue;

        // find the distance from the edge of the bounding box
        for (i = 0 ; i < 3 ; i++)
        {
            if (origin[i] < ent->r.absmin[i]) v[i] = ent->r.absmin[i] - origin[i];
            else if (origin[i] > ent->r.absmax[i]) v[i] = origin[i] - ent->r.absmax[i];
            else v[i] = 0;
        }

        dist = VectorLength(v);
        if (dist >= radius)			continue;

        points = damage * (1.0 - dist / radius);

        if (CanDamage (ent, origin) && ent->waterlevel) 	// must be in the water, somehow!
        {
            if (LogAccuracyHit (ent, attacker)) hitClient = qtrue;
            VectorSubtract (ent->r.currentOrigin, origin, dir);
            // push the center of mass higher than the origin so players
            // get knocked into the air more
            dir[2] += 24;
            G_Damage (ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS, mod);
        }
    }

    return hitClient;
}



void do_shove(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    vec3_t  start;
    vec3_t  forward;
    vec3_t  end;
    trace_t tr;
    gentity_t	*traceEnt;


    VectorCopy(ent->s.origin, start);
    start[2] += ent->client->ps.viewheight;
    AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
    VectorMA(start, 8192, forward, end);
    trap_Trace( &tr, start, vec3_origin, vec3_origin, end, ent->s.number, MASK_SHOT);
    traceEnt = &g_entities[ tr.entityNum ];

    if ( traceEnt && traceEnt->client )
    {
        VectorScale(forward, 5000, forward);
        VectorAdd(forward, traceEnt->client->ps.velocity, traceEnt->client->ps.velocity);
    }
}

void do_drag(gentity_t *ent, char *argument, int cmd, int subcmd)
{
    vec3_t  start;
    vec3_t  forward;
    vec3_t  end;
    trace_t tr;
    gentity_t	*traceEnt;


    VectorCopy(ent->s.origin, start);
    start[2] += ent->client->ps.viewheight;
    AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
    VectorMA(start, 8192, forward, end);
    trap_Trace( &tr, start, vec3_origin, vec3_origin, end, ent->s.number, MASK_SHOT);
    traceEnt = &g_entities[ tr.entityNum ];

    if ( traceEnt && traceEnt->client )
    {
        VectorScale(forward, -5000, forward);
        VectorAdd(forward, traceEnt->client->ps.velocity, traceEnt->client->ps.velocity);
    }
}
