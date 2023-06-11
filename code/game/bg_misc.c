// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_misc.c -- both games misc functions, all completely stateless

#include "../qcommon/q_shared.h"
#include "bg_public.h"

playerinfo2_t bg_playerlist[] =
{
//CLASS_DRN00X
    {
        "mmx",
        600,	//maximum energy
        100,	//maximum armor
        130,	//maximum health
        400,	//jump velocity
        0.80f,	//speed factor
        30,		//sensor range
        {   { WP_NONE, 0, 500, 0 },		{ WP_XBUSTER, 0, 200, 20 },		{ WP_FCUTTER, 2, 80, 10 },		{ WP_SICE, 2, 500, 20 },
            { WP_AFIST, 2, 500, 20 },	{ WP_BCRYSTAL, 2, 500, 20 },	{ WP_LTORPEDO, 2, 500, 20 },	{ WP_TBURSTER, 2, 500, 20 },
            { WP_GBOMB, 2, 500, 20 },	{ WP_BLASER, 2, 40, 40 },		{ WP_SFIST, 2, 500, 20 },		{ WP_NONE, 0, 500, 0 },
            { WP_NONE, 0, 500, 0 },		{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 }
        },
        { UG_RADAR, UG_FOCUS, UG_TRANSPERS },
        { UG_WALLJUMP, UG_SPEEDBOOST, UG_360DASH },
        { UG_REGEN, UG_CLOAK, UG_POWERCONVERTER },
        { UG_BUSTER, UG_ENERGYSAVER, UG_QUICKCHARGE },
        { UG_DASH, UG_AIRDASH, UG_AIRJUMP }
    },
//CLASS_DRN000
    {
        "proto",
        600,	//maximum energy
        80,	//maximum armor
        120,	//maximum health
        380,	//jump velocity
        .7f,	//speed factor
        30,		//sensor range
        {   { WP_NONE, 0, 500, 0 },		{ WP_PBUSTER, 0, 200, 20 },		{ WP_RISFIRE, 2, 300, 20 },		{ WP_IBURST, 2, 500, 20 },
            { WP_IMBOOMERANG, 2, 500, 20 },	{ WP_RSTONE, 2, 500, 20 },		{ WP_STRIKECHAIN, 2, 500, 20 },	{ WP_LSTORM, 2, 500, 20 },
            { WP_GRAVWELL, 2, 500, 100 },	{ WP_SSHREDDER, 2, 500, 20 },	{ WP_TRTHUNDER, 2, 500, 20 },		{ WP_NONE, 0, 500, 0 },
            { WP_NONE, 0, 500, 0 },		{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 }
        },
        { UG_SENSE, UG_RADAR, UG_SCANNER },
        { UG_WALLJUMP, UG_SPEEDBOOST, UG_WALLCLIMB },
        { UG_REGEN, UG_CLOAK, UG_RAGE },
        { UG_BUSTER, UG_QUICKCHARGE, UG_DRAIN },
        { UG_HIGHJUMP, UG_DASH, UG_MOONBOOTS }
    },
//CLASS_DRN001
    {
        "megaman",
        600,	//maximum energy
        80,	//maximum armor
        120,	//maximum health
        380,	//jump velocity
        .7f,	//speed factor
        30,		//sensor range
        {   { WP_NONE, 0, 500, 0 },		{ WP_MBUSTER, 0, 200, 20 },		{ WP_HWAVE, 2, 300, 20 },		{ WP_FBURST, 2, 500, 20 },
            { WP_AWAVE, 2, 500, 20 },	{ WP_RSTONE, 2, 500, 20 },		{ WP_BLAUNCHER, 2, 500, 20 },	{ WP_LSTORM, 2, 500, 20 },
            { WP_MMINE, 2, 500, 100 },	{ WP_SSHREDDER, 2, 500, 20 },	{ WP_BSHIELD, 2, 500, 20 },		{ WP_NONE, 0, 500, 0 },
            { WP_NONE, 0, 500, 0 },		{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 }
        },
        { UG_SENSE, UG_RADAR, UG_SCANNER },
        { UG_WALLJUMP, UG_SPEEDBOOST, UG_WALLCLIMB },
        { UG_ARMORBOOST, UG_REGEN, UG_CLOAK },
        { UG_BUSTER, UG_AMMOBOOST, UG_QUICKCHARGE },
        { UG_HIGHJUMP, UG_DASH, UG_JETBOOTS}
    },
//CLASS_SWN001
    {
        "forte",
        800,	//maximum energy
        130,	//maximum armor
        100,	//maximum health
        380,	//jump velocity
        .6f,	//speed factor
        30,		//sensor range
        {   { WP_NONE, 0, 500, 0 },		{ WP_FBUSTER, 0, 150, 20 },		{ WP_PSTRIKE, 2, 400, 20 },		{ WP_WSTRIKE, 2, 500, 20 },
            { WP_TEMPEST, 2, 500, 20 },	{ WP_SBURST, 2, 500, 20 },		{ WP_DSHOT, 2, 500, 20 },		{ WP_ASPLASHER, 2, 500, 20 },
            { WP_DMATTER, 2, 150 },		{ WP_NBURST, 2, 500, 20 },		{ WP_BLIGHTNING, 2, 500, 20 },	{ WP_NONE, 0, 500, 0 },
            { WP_NONE, 0, 500, 0 },		{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 }
        },
        { UG_RADAR, UG_FOCUS, UG_TRANSPERS },
        { UG_WALLJUMP, UG_SPEEDBOOST, UG_360DASH },
        { UG_REGEN, UG_CLOAK, UG_POWERCONVERTER },
        { UG_DOUBLE, UG_ENERGYSAVER, UG_RAGE },
        { UG_DASH, UG_AIRDASH, UG_AIRJUMP }
    },
//CLASS_DWNINFINITY
    {
        "zero",
        600,	//maximum energy
        80,		//maximum armor
        130,	//maximum health
        400,	//jump velocity
        1.0f,	//speed factor
        30,		//sensor range
        {   { WP_NONE, 0, 500, 0 },		{ WP_ZSABER, 0, 500, 20 },		{ WP_FIRE_M, 0, 500, 20 },		{ WP_WATER_M, 0, 500, 20 },
            { WP_AIR_M, 0, 500, 20 },	{ WP_EARTH_M, 0, 500, 20 },		{ WP_METAL_M, 0, 500, 20 },		{ WP_NATURE_M, 0, 500, 20 },
            { WP_GRAV_M, 0, 500, 20 },	{ WP_LIGHT_M, 0, 500, 20 },		{ WP_ELEC_M, 0, 500, 20 },		{ WP_NONE, 0, 500, 0 },
            { WP_NONE, 0, 500, 0 },		{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 },			{ WP_NONE, 0, 500, 0 }
        },
        { UG_SENSE, UG_RADAR, UG_TRANSPERS },
        { UG_WALLJUMP, UG_SPEEDBOOST, UG_360DASH },
        { UG_REGEN, UG_CLOAK, UG_POWERCONVERTER },
        { UG_BUSTER, UG_ENERGYSAVER, UG_MELEEBOOST },
        { UG_DASH, UG_AIRDASH, UG_AIRJUMP }
    },
    {0}
};

// NPC PLAYER LIST
gnpc_t bg_npclist[] =
{
    {
        "npc_ank",					// class
        NPC_ANK,
        100,						// health
        1.0,
        20,							// walkingSpeed
        60,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        20,							// walkingRotSpd
        75,							// runningRotSpd
        0,							// melee distance
        25,							// melee damage
        20,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,24},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {
        "npc_bat",					// class
        NPC_BAT,
        100,						// health
        1.0,
        40,
        60,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        20,							// walkingRotSpd
        75,							// runningRotSpd
        0,							// melee distance
        0,							// melee damage
        20,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,40},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {
        "npc_hulk",					// class
        NPC_HULK,
        100,						// health
        1.0,
        30,
        60,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        20,							// walkingRotSpd
        75,							// runningRotSpd
        0,							// melee distance
        50,							// melee damage
        35,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,24},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {
        "npc_metlar",					// class
        NPC_METLAR,
        100,						// health
        1.0,
        20,
        60,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        20,							// walkingRotSpd
        75,							// runningRotSpd
        0,							// melee distance
        25,							// melee damage
        20,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,40},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {
        "npc_pilot",					// class
        NPC_PILOT,
        100,						// health
        1.0,
        20,
        60,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        20,							// walkingRotSpd
        75,							// runningRotSpd
        0,							// melee distance
        0,							// melee damage
        20,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,40},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {
        "npc_sealord",					// class
        NPC_SEALORD,
        1000,						// health
        1.0,
        0,
        0,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        0,							// walkingRotSpd
        0,							// runningRotSpd
        0,							// melee distance
        25,							// melee damage
        20,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,40},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {
        "npc_soldier1",					// class
        NPC_SOLDIER1,
        100,						// health
        1.0,
        20,
        60,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        20,							// walkingRotSpd
        75,							// runningRotSpd
        0,							// melee distance
        0,							// melee damage
        20,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,40},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {
        "npc_soldier2",					// class
        NPC_SOLDIER2,
        100,						// health
        1.0,
        20,
        60,							// runningSpeed
        180,						// fov
        50,							// jumpHeight
        20,							// walkingRotSpd
        75,							// runningRotSpd
        0,							// melee distance
        0,							// melee damage
        20,							// far damage
        {0},
        {-24,-24,-24},
        {24,24,40},
        {0,0,30},
        "",							// precache
        ""							// sounds
    },
    {NULL},
};

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
"light" Dynamic light intensity (default = 100 ).
*/

gitem_t	bg_itemlist[] =
{
    {
        NULL,
        NULL,
        {
            NULL,
            NULL,
            0, 0
        } ,
        /* icon */		NULL,
        /* pickup */	NULL,
        /* hudbar */	NULL,
        0,
        0,
        0,
        /* precache */ "",
        /* sounds */ ""
    },	// leave index 0 alone

    //
    // ARMOR
    //

    /*QUAKED item_armor_shard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_armor_shard",
        "sound/misc/ar1_pkup.wav",
        {
            "models/powerups/armor/shard.md3",
            "models/powerups/armor/shard_sphere.md3",
            0, 0
        } ,
        /* icon */		"icons/iconr_yellow",
        /* pickup */	"Armor Shard",
        "",
        5,
        IT_ARMOR,
        0,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED item_armor_combat (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_armor_combat",
        "sound/misc/ar2_pkup.wav",
        {
            "models/powerups/armor/armor_yel.md3",
            0, 0, 0
        },
        /* icon */		"icons/iconr_yellow",
        /* pickup */	"Armor",
        "",
        50,
        IT_ARMOR,
        0,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED item_armor_body (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_armor_body",
        "sound/misc/ar2_pkup.wav",
        {
            "models/powerups/armor/armor_red.md3",
            0, 0, 0
        },
        /* icon */		"iconr_yellow",
        /* pickup */	"Heavy Armor",
        "",
        100,
        IT_ARMOR,
        0,
        /* precache */ "",
        /* sounds */ ""
    },

    //
    // health
    //
    /*QUAKED item_health_small (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_health_small",
        "sound/xbuster/xbitem_health1.wav",
        {
            "models/items/health/xb_health_1.md3",
            0, 0, 0
        },
        /* icon */		"icons/iconh_green",
        /* pickup */	"5 Health",
        "",
        5,
        IT_HEALTH,
        0,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED item_health (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_health",
        "sound/xbuster/xbitem_health1.wav",
        {
            "models/items/health/xb_health_2.md3",
            0, 0, 0
        },
        /* icon */		"icons/iconh_yellow",
        /* pickup */	"25 Health",
        "",
        25,
        IT_HEALTH,
        0,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED item_health_large (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_health_large",
        "sound/xbuster/xbitem_health1.wav",
        {
            "models/items/health/xb_health_3.md3",
            0, 0, 0
        },
        /* icon */		"icons/iconh_red",
        /* pickup */	"50 Health",
        "",
        50,
        IT_HEALTH,
        0,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED item_health_mega (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_health_mega",
        "sound/xbuster/xbitem_health1.wav",
        {
            "models/items/health/xb_health_3.md3",
            0, 0, 0
        },
        /* icon */		"icons/iconh_mega",
        /* pickup */	"Mega Health",
        "",
        100,
        IT_HEALTH,
        0,
        /* precache */ "",
        /* sounds */ ""
    },

    {
        "item_heartcanister",
        "sound/xbuster/xbitem_hearttank.wav",
        {
            "models/items/health/xb_heart.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_hc",
        /* pickup */	"Heart Canister",
        "",
        25,
        IT_HEALTH,
        0,
        /* precache */ "",
        /* sounds */ "",
        1
    },

    //
    // WEAPONS
    //
    //Default Weapons
    {
        "weapon_xbuster",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_xbuster",			//icon
        "X.Buster",						//item/pickup name
        "tech",					//hud bar pic
        0,								//quantity
        IT_WEAPON,						//item type
        WP_XBUSTER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_fbuster",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_xbuster",			//icon
        "F.Buster",						//item/pickup name
        "tech",					//hud bar pic
        0,								//quantity
        IT_WEAPON,						//precache
        WP_FBUSTER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_mbuster",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_xbuster",			//icon
        "M.Buster",						//item/pickup name
        "tech",					//hud bar pic
        0,								//quantity
        IT_WEAPON,						//item type
        WP_MBUSTER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_pbuster",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_xbuster",			//icon
        "P.Buster",						//item/pickup name
        "tech",					//hud bar pic
        0,								//quantity
        IT_WEAPON,						//item type
        WP_PBUSTER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_zsaber",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_saber",			//icon
        "Z.Saber",						//item/pickup name
        "tech",					//hud bar pic
        0,								//quantity
        IT_WEAPON,						//item type
        WP_ZSABER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_zbuster",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_xbuster",			//icon
        "Z.Buster",						//item/pickup name
        "tech",					//hud bar pic
        0,								//quantity
        IT_WEAPON,						//item type
        WP_ZBUSTER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Fire Weapons
    {
        "weapon_fcutter",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_fire",				//icon
        "F.Cutter",						//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_FCUTTER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_hwave",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_fire",				//icon
        "H.Wave",						//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_HWAVE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_risfire",				//classname
        "sound/misc/w_pkup.wav",								//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_fire",				//icon
        "R. Fire",						//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_RISFIRE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_pstrike",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_fire",				//icon
        "P.Strike",						//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_PSTRIKE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_fire_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_fire",				//icon
        "Fire Blade",					//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_FIRE_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Water Weapons
    {
        "weapon_sice",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_ice",				//icon
        "S. Ice",						//item/pickup name
        "water",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_SICE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_fburst",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_ice",				//icon
        "F. Burst",						//item/pickup name
        "water",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_FBURST,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_iburst",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_ice",				//icon
        "I. Burst",						//item/pickup name
        "water",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_IBURST,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_wstrike",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_ice",				//icon
        "W. Strike",					//item/pickup name
        "water",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_WSTRIKE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_water_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_ice",				//icon
        "Ice Blade",					//item/pickup name
        "water",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_WATER_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Wind Weapons
    {
        "weapon_afist",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_air",				//icon
        "A. Fist",						//item/pickup name
        "wind",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_AFIST,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_awave",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_air",				//icon
        "R. Wind",						//item/pickup name
        "wind",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_AWAVE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_imboomerang",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_air",				//icon
        "Impact Boomerang",						//item/pickup name
        "wind",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_IMBOOMERANG,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_tempest",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,//world model
            0, 0, 0
        },
        "icons/icon_air",				//icon
        "A. Crasher",					//item/pickup name
        "wind",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_TEMPEST,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_wind_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_air",				//icon
        "Thunder Blade",				//item/pickup name
        "wind",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_AIR_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Earth Weapons
    {
        "weapon_bcrystal",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,//world model
            0, 0, 0
        },
        "icons/icon_earth",				//icon
        "B. Crystal",					//item/pickup name
        "earth",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_BCRYSTAL,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_rstone",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,//world model
            0, 0, 0
        },
        "icons/icon_earth",				//icon
        "E. Crusher",					//item/pickup name
        "earth",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_RSTONE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_makeme4",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_earth",				//icon
        "Makeme4",						//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_MAKEME4,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_sburst",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,//world model
            0, 0, 0
        },
        "icons/icon_earth",				//icon
        "S. Cutter",					//item/pickup name
        "earth",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_SBURST,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_earth_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_earth",				//icon
        "Crystal Blade",				//item/pickup name
        "earth",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_EARTH_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Tech Weapons
    {
        "weapon_dshot",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_tech",				//icon
        "Tunnel Torpedo",						//item/pickup name
        "tech",							//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_DSHOT,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_blauncher",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_tech",				//icon
        "S. Blade",						//item/pickup name
        "tech",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_BLAUNCHER,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_strikechain",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_tech",				//icon
        "Strikechain",						//item/pickup name
        "tech",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_STRIKECHAIN,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_ltorpedo",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_tech",				//icon
        "L. Torpedo",					//item/pickup name
        "tech",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_LTORPEDO,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_tech_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_tech",				//icon
        "Steel Blade",					//item/pickup name
        "tech",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_METAL_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Nature Weapons
    {
        "weapon_tburster",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_plant",				//icon
        "T. Burster",					//item/pickup name
        "plant",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_TBURSTER,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_lstorm",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_plant",				//icon
        "L. Storm",						//item/pickup name
        "plant",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_LSTORM,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_makeme5",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_plant",				//icon
        "Makeme5",						//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_MAKEME5,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_asplasher",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_plant",				//icon
        "A. Splasher",					//item/pickup name
        "plant",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_ASPLASHER,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_nature_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_plant",				//icon
        "Acid Blade",					//item/pickup name
        "plant",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_NATURE_M,					//weapon num
        "",								//precache
        ""								//sounds
    },
    //Gravity Weapons
    {
        "weapon_gbomb",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_gravity",				//icon
        "G. Bomb",						//item/pickup name
        "gravity",				//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_GBOMB,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_mmine",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_gravity",				//icon
        "M. Mine",						//item/pickup name
        "gravity",				//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_MMINE,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_gravwell",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_gravity",				//icon
        "G. Well",						//item/pickup name
        "gravity",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_GRAVWELL,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_dmatter",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,			//world model
            0, 0, 0
        },
        "icons/icon_gravity",				//icon
        "D. Matter",					//item/pickup name
        "gravity",				//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_DMATTER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_grav_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_gravity",			//icon
        "Vortex Blade",					//item/pickup name
        "gravity",				//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_GRAV_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Light Weapons
    {
        "weapon_blaser",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_light",				//icon
        "B. Laser",						//item/pickup name
        "light",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_BLASER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_sshredder",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_light",				//icon
        "S. Crusher",					//item/pickup name
        "light",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_SSHREDDER,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_makeme7",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_light",				//icon
        "Makeme7",						//item/pickup name
        "fire",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_MAKEME7,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_nburst",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_light",				//icon
        "N. Burst",						////item/pickup name
        "light",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_NBURST,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_light_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_light",			//icon
        "Energy Blade",					//item/pickup name
        "light",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_LIGHT_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    //Electric Weapons
    {
        "weapon_sfist",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_energy",			//icon
        "S. Fist",						//item/pickup name
        "elec",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_SFIST,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_bshield",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_energy",			//icon
        "B. Shield",					//item/pickup name
        "elec",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_BSHIELD,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_trthunder",					//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_energy",				//icon
        "Tr. Thunder",						//item/pickup name
        "elec",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_TRTHUNDER,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_blightning",			//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,	//world model
            0, 0, 0
        },
        "icons/icon_energy",			//icon
        "B. Lightning",					//item/pickup name
        "elec",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_BLIGHTNING,					//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "weapon_elec_m",				//classname
        "sound/misc/w_pkup.wav",		//pickup sound
        {
            NULL,		//world model
            0, 0, 0
        },
        "icons/icon_energy",			//icon
        "Lightning Blade",				//item/pickup name
        "elec",					//hud bar pic
        800,							//quantity
        IT_WEAPON,						//item type
        WP_ELEC_M,						//weapon num
        "",								//precache
        ""								//sounds
    },
    {
        "chip_bolts_big",				//classname
        "sound/misc/am_pkup.wav",		//pickup sound
        {
            "models/upgrades/bigbolt/bigbolt.md3",	//world model
            0, 0, 0
        },
        "icons/icon_upgrade",		//icon
        "Large Bolt",					//item/pickup name
        "",
        100,							//quantity
        IT_CHIPS,						//item type
        HI_BOLTS,						//item num
        "",								//precache
        ""								//sounds
    },
    {
        "chip_chips_big",				//classname
        "sound/misc/am_pkup.wav",		//pickup sound
        {
            "models/upgrades/bigbolt/bigbolt.md3",	//world model
            0, 0, 0
        },
        "icons/icon_upgrade",			//icon
        "Large Chip",					//item/pickup name
        "",
        1,								//quantity
        IT_CHIPS,						//item type
        HI_CHIPS,						//item num
        "",								//precache
        ""								//sounds
    },

    /*QUAKED weapon_shotgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_shotgun",
        "sound/misc/w_pkup.wav",
        {
            NULL,
            0, 0, 0
        },
        /* icon */		"icons/icon_ice",
        /* pickup */	"Shotgun",
        "",
        10,
        IT_WEAPON,
        RWP_WATER,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED weapon_machinegun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_machinegun",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_fire",
        /* pickup */	"Machinegun",
        "",
        40,
        IT_WEAPON,
        RWP_FIRE,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED weapon_grenadelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_grenadelauncher",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_air",
        /* pickup */	"Grenade Launcher",
        "",
        10,
        IT_WEAPON,
        RWP_AIR,
        /* precache */ "",
        /* sounds */ "sound/weapons/grenade/hgrenb1a.wav sound/weapons/grenade/hgrenb2a.wav"
    },

    /*QUAKED weapon_rocketlauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_rocketlauncher",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_earth",
        /* pickup */	"Rocket Launcher",
        "",
        10,
        IT_WEAPON,
        RWP_EARTH,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED weapon_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_lightning",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_tech",
        /* pickup */	"Lightning Gun",
        "",
        100,
        IT_WEAPON,
        RWP_TECH,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED weapon_railgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_railgun",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_plant",
        /* pickup */	"Railgun",
        "",
        10,
        IT_WEAPON,
        RWP_NATURE,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED weapon_plasmagun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_plasmagun",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_gravity",
        /* pickup */	"Plasma Gun",
        "",
        50,
        IT_WEAPON,
        RWP_GRAVITY,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED weapon_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_bfg",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_light",
        /* pickup */	"BFG10K",
        "",
        20,
        IT_WEAPON,
        RWP_LIGHT,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED weapon_grapplinghook (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "weapon_grapplinghook",
        "sound/misc/w_pkup.wav",
        {
            "models/items/weapons/xb_weapon2.md3",
            0, 0, 0
        },
        /* icon */		"icons/icon_plant",
        /* pickup */	"Grappling Hook",
        "",
        0,
        IT_WEAPON,
        RWP_ELECTRICAL,
        /* precache */ "",
        /* sounds */ ""
    },


    //
    // AMMO ITEMS
    //

    /*QUAKED ammo_shells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_shells",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/shotgunam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_shotgun",
        /* pickup */	"Shells",
        "",
        10,
        IT_AMMO,
        RWP_WATER,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED ammo_bullets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_bullets",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/machinegunam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_machinegun",
        /* pickup */	"Bullets",
        "",
        50,
        IT_AMMO,
        RWP_FIRE,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED ammo_grenades (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_grenades",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/grenadeam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_grenade",
        /* pickup */	"Grenades",
        "",
        5,
        IT_AMMO,
        RWP_AIR,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED ammo_cells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_cells",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/plasmaam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_plasma",
        /* pickup */	"Cells",
        "",
        30,
        IT_AMMO,
        RWP_GRAVITY,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED ammo_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_lightning",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/lightningam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_lightning",
        /* pickup */	"Lightning",
        "",
        60,
        IT_AMMO,
        RWP_TECH,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED ammo_rockets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_rockets",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/rocketam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_rocket",
        /* pickup */	"Rockets",
        "",
        5,
        IT_AMMO,
        RWP_EARTH,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED ammo_slugs (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_slugs",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/railgunam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_railgun",
        /* pickup */	"Slugs",
        "",
        10,
        IT_AMMO,
        RWP_NATURE,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED ammo_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "ammo_bfg",
        "sound/misc/am_pkup.wav",
        {
            "models/powerups/ammo/bfgam.md3",
            0, 0, 0
        },
        /* icon */		"icons/icona_bfg",
        /* pickup */	"Bfg Ammo",
        "",
        15,
        IT_AMMO,
        RWP_LIGHT,
        /* precache */ "",
        /* sounds */ ""
    },


    //
    // HOLDABLE ITEMS
    //
    /*QUAKED item_subtank (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
    */
    {
        "item_subtank",
        "sound/items/holdable.wav",
        {
            "models/items/subtank/xb_subtank_1.md3",
            "models/items/subtank/xb_subtank_1.md3",
            0, 0
        },
        /* icon */		"icons/icon_subtank",
        /* pickup */	"Subtank",
        "",
        60,
        IT_HOLDABLE,
        HI_SUBTANK,
        /* precache */ "",
        /* sounds */ "sound/xbuster/xbitem_subtank.wav",
        2
    },

    //
    // POWERUP ITEMS
    //
    /*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
    Only in CTF games
    */
    {
        "team_CTF_redflag",
        NULL,
        {
            "models/flags/r_flag.md3",
            0, 0, 0
        },
        /* icon */		"icons/iconf_red1",
        /* pickup */	"Red Flag",
        "",
        0,
        IT_TEAM,
        PW_REDFLAG,
        /* precache */ "",
        /* sounds */ ""
    },

    /*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
    Only in CTF games
    */
    {
        "team_CTF_blueflag",
        NULL,
        {
            "models/flags/b_flag.md3",
            0, 0, 0
        },
        /* icon */		"icons/iconf_blu1",
        /* pickup */	"Blue Flag",
        "",
        0,
        IT_TEAM,
        PW_BLUEFLAG,
        /* precache */ "",
        /* sounds */ ""
    },

    // end of list marker
    {NULL}
};

int		bg_numItems = sizeof(bg_itemlist) / sizeof(bg_itemlist[0]) - 1;

upgrade_t upgrade_table[] =
{
    { "","", 0 },

    // Head
    { "Focus", "Enhances Mental Accuracy: Weapon Fire rate increases.", 3 },
    { "Radar", "This will enable a 360 HUD radar system for locating other players on the map.", 6 },
    { "Trans-perspective", "Allows player to have an overhead view of the level, (set distance).", 9 },
    { "Scanner", "Displays an enemies status onscreen.", 12 },
    { "Sense", "Allow you to sense when an enemy player is in close proximity to you with 360 audio signals or an Onscreen arrow", 12 },

    // Back
    { "Walljump", "Walljump ability", 3 },
    { "360 Dash", "Allows player to dash anywhere you point with mouse aim while in the air.", 6 },
    { "Speed Boost", "Allows the player to run faster.", 9 },
    { "Wallclimb", "Allows player to climb walls.", 12 },

    // Body
    { "Powerconverter", "Converts Power or redistributes it accordingly", 3 },
    { "Regen", "Health Enhancment: Rejuvinate health gradually. Adds +25 to your max storage.", 6  },
    { "Cloak", "Gives you the ability to turn invisable (type clock in console)", 9 },
    { "Armor Boost", "Gives player more defence to withstand attacks.", 12 },
    { "Absorb", "1/8 of the damage recieved turns into health.", 3 },
    { "Rage", "Gives player a boost to all attributes, but decreases Health", 6 },

    // Buster
    { "Buster", "Gives 4th level charge state for primary attack, and also for elemental attacks.", 9 },
    { "Quickcharge", "Quickcharges", 12 },
    { "EnergySaver", "Gives Player the ability to use energy/health better. (redirects to empty weapons).", 12 },
    { "Drain", "When touching an enemy, 1/8 of there health is drain, and is transfered to you.", 3 },
    { "Ammo Boost", "Allows more ammo/energy storage capacity.", 6 },
    { "Melee Boost", "Allows player to use secondary (air) melee techniques in addition to default (ground).", 9 },
    { "Double Buster", "Gives Rapid Buster 2 streams of weapon fire.", 6},

    // Legs
    { "Airdash", "Air Dash: Perform Dash while airborn. Allows exploration of high areas. Increases Combat and mouvment ability.", 12 },
    { "Airjump", "Airjumps", 3 },
    { "Highjump", "Jump higher", 6 },
    { "Dash", "Dash Ability", 9 },
    { "Jetboots", "Gives player jetboots which runs off of external energy which regains.", 12 },
    { "Moonboots", "Anti-gravity boots.", 12 },
    { "SuperAirJump", "Gives a super high air jump.", 12},

    // Special
    { "Diagnose", "Shows targets health.", 12 },
    { "Phase", "Randomly phase out.", 12 },
    { "Tracking", "Follow enemies trails.", 12 },

    {NULL, NULL, 0}
};

/*
==============
BG_FindItemForPowerup
==============
*/
gitem_t	*BG_FindItemForPowerup( powerup_t pw )
{
    int		i;

    for ( i = 0 ; i < bg_numItems ; i++ )
    {
        if ( (bg_itemlist[i].giType == IT_POWERUP ||
                bg_itemlist[i].giType == IT_TEAM ||
                bg_itemlist[i].giType == IT_PERSISTANT_POWERUP) &&
                bg_itemlist[i].giTag == pw )
        {
            return &bg_itemlist[i];
        }
    }

    return NULL;
}


/*
==============
BG_FindItemForHoldable
==============
*/
gitem_t	*BG_FindItemForHoldable( holdable_t pw )
{
    int		i;

    for ( i = 0 ; i < bg_numItems ; i++ )
    {
        if ( bg_itemlist[i].giType == IT_HOLDABLE && bg_itemlist[i].giTag == pw )
        {
            return &bg_itemlist[i];
        }
    }

    Com_Error( ERR_DROP, "HoldableItem not found" );

    return NULL;
}

/*
==============
BG_FindItemForChip
==============
*/
gitem_t	*BG_FindItemForChip( holdable_t pw )
{
    int		i;

    for ( i = 0 ; i < bg_numItems ; i++ )
    {
        if ( bg_itemlist[i].giType == IT_CHIPS && bg_itemlist[i].giTag == pw )
        {
            return &bg_itemlist[i];
        }
    }
    Com_Error( ERR_DROP, "ChipItem not found" );

    return NULL;
}


/*
===============
BG_FindItemForWeapon

===============
*/
gitem_t	*BG_FindItemForWeapon( weapon_t weapon )
{
    gitem_t	*it;

    for ( it = bg_itemlist + 1 ; it->classname ; it++)
    {
        if ( it->giType == IT_WEAPON && it->giTag == weapon )
        {
            return it;
        }
    }

    Com_Error( ERR_DROP, "Couldn't find item for weapon %i", weapon);
    return NULL;
}

/*
===============
BG_FindItem

===============
*/
gitem_t	*BG_FindItem( const char *pickupName )
{
    gitem_t	*it;

    for ( it = bg_itemlist + 1 ; it->classname ; it++ )
    {
        if ( !Q_stricmp( it->pickup_name, pickupName ) )
            return it;
    }

    return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
*/
qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime )
{
    vec3_t		origin;

    BG_EvaluateTrajectory( &item->pos, atTime, origin );

    // we are ignoring ducked differences here
    if ( ps->origin[0] - origin[0] > 44
            || ps->origin[0] - origin[0] < -50
            || ps->origin[1] - origin[1] > 36
            || ps->origin[1] - origin[1] < -36
            || ps->origin[2] - origin[2] > 36
            || ps->origin[2] - origin[2] < -36 )
    {
        return qfalse;
    }

    return qtrue;
}



/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.
================
*/
qboolean BG_CanItemBeGrabbed( int gametype, const entityState_t *ent, const playerState_t *ps )
{
    gitem_t	*item;
    qboolean powerconverter = (ps->persistant[PERS_UPGRADES] & (1 << UG_POWERCONVERTER));

    if ( ent->modelindex < 1 || ent->modelindex >= bg_numItems )
    {
        Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
    }

    item = &bg_itemlist[ent->modelindex];

    switch( item->giType )
    {
        // chips can always be picked up
    case IT_CHIPS:
        return qtrue;
    case IT_WEAPON:
        return qtrue;	// weapons are always picked up

    case IT_AMMO: //changed from item->giTag to ps->weapon
        // if player has ammoboost upgrade they can hold more
        if ( IS_SET(ps->persistant[PERS_UPGRADES], (1 << UG_AMMOBOOST)) &&
                ps->ammo[ ps->weapon ] <= ps->persistant[PERS_MAX_ENERGY]*1.5)
            return qtrue;
        if ( (ps->ammo[ ps->weapon ] >= ps->persistant[PERS_MAX_ENERGY]) && !powerconverter )
            return qfalse;		// can't hold any more

        return qtrue;

    case IT_ARMOR:
        if ( ps->stats[STAT_ARMOR] >= ps->persistant[PERS_MAX_ARMOR] )
        {
            return qfalse;
        }
        return qtrue;

    case IT_HEALTH:
        // small and mega healths will go over the max, otherwise
        // don't pick up if already at max
        if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] &&
                strcmp(item->classname, "item_heartcanister") &&
                bg_itemlist[ps->stats[STAT_HOLDABLE_ITEM]].giTag != HI_SUBTANK)
        {
            if(!powerconverter)
            {
                return qfalse;
            }
        }

        /* If the players subtank is full don't pick it up */
        if (bg_itemlist[ps->stats[STAT_HOLDABLE_ITEM]].giTag == HI_SUBTANK &&
                ps->persistant[PERS_SUBTANKS] >= 50 &&
                strcmp(item->classname, "item_heartcanister"))
            return qfalse;

        return qtrue;

    case IT_POWERUP:
        return qtrue;	// powerups are always picked up

    case IT_TEAM: // team items, such as flags
        if( gametype == GT_CTF )
        {
            // ent->modelindex2 is non-zero on items if they are dropped
            // we need to know this because we can pick up our dropped flag (and return it)
            // but we can't pick up our flag at base
            if (ps->persistant[PERS_TEAM] == TEAM_RED)
            {
                if (item->giTag == PW_BLUEFLAG ||
                        (item->giTag == PW_REDFLAG && ent->modelindex2) ||
                        (item->giTag == PW_REDFLAG && ps->powerups[PW_BLUEFLAG]) )
                    return qtrue;
            }
            else if (ps->persistant[PERS_TEAM] == TEAM_BLUE)
            {
                if (item->giTag == PW_REDFLAG ||
                        (item->giTag == PW_BLUEFLAG && ent->modelindex2) ||
                        (item->giTag == PW_BLUEFLAG && ps->powerups[PW_REDFLAG]) )
                    return qtrue;
            }
        }

        return qfalse;

    case IT_HOLDABLE:
        // can only hold one item at a time
        if ( ps->stats[STAT_HOLDABLE_ITEM] )
        {
            return qfalse;
        }
        return qtrue;

    case IT_BAD:
        Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD" );
    default:
#ifndef NDEBUG // bk0001204
        Com_Printf(   "BG_CanItemBeGrabbed: unknown enum %d\n", item->giType );
#endif
        break;
    }

    return qfalse;
}

//======================================================================

/*
================
BG_EvaluateTrajectory

Refrence Func: g_missle.c / RunMissle

	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

	typedef struct {
	trType_t	trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t	trBase;
	vec3_t	trDelta;			// velocity, etc
} trajectory_t;
================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result )
{
    float		deltaTime;
    float		phase;
    int			direction = 0;
    vec3_t		vel, dir, relativeX, relativeUp = {0,0,1}, relativeDown = {0,0,-1};
//	vec3_t		dpvCenter, dpvRight, dpvLeft, dpvForward;


    switch( tr->trType )
    {
    case TR_STATIONARY:
    case TR_INTERPOLATE:
        VectorCopy( tr->trBase, result );
        break;
    case TR_LINEAR:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        break;
    case TR_LINEAR_STOP:
        if ( atTime > tr->trTime + tr->trDuration )
        {
            atTime = tr->trTime + tr->trDuration;
        }
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        if ( deltaTime < 0 )
        {
            deltaTime = 0;
        }
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        break;
    case TR_SINE: // affects velocity by sin equation based on deltaTime (atTime - trTime)
        deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
        phase = sin( deltaTime * M_PI * 2 );
        VectorMA( tr->trBase, phase, tr->trDelta, result );
        break;
    case TR_CIRCLE:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        if ( VectorCompare( tr->trDelta, relativeUp ) )
        {
            VectorSet( relativeUp, 0, 1, 0 );
            direction = 1;
        }
        if ( VectorCompare( tr->trDelta, relativeDown ) )
        {
            VectorSet( relativeUp, 0, 1, 0 );
            direction = -1;
        }
        CrossProduct( tr->trDelta, relativeUp, relativeX );
        CrossProduct( tr->trDelta, relativeX, relativeUp );
        VectorNormalize( relativeX );
        VectorNormalize( relativeUp );
        VectorScale( relativeX, tr->trDuration * sin( deltaTime * 15 + (tr->trDuration*M_PI) ) * direction, relativeX ); //increase the multiplier on deltaTime to make it spin faster
        VectorScale( relativeUp, tr->trDuration * cos( deltaTime * 15 + (tr->trDuration*M_PI) ) * direction, relativeUp ); //adding to the equation offsets the particle on the circle by a certain amount
        //Com_Printf(    "relativeX = <%f, %f, %f>\n", relativeX[0], relativeX[1], relativeX[2]);
        //Com_Printf(    "relativeUp = <%f, %f, %f>\n", relativeUp[0], relativeUp[1], relativeUp[2]);
        VectorAdd( result, relativeX, result );
        VectorAdd( result, relativeUp, result );
        break;
    case TR_HELIX:
        if( atTime < ( tr->trTime + (30*(tr->trDuration%5)) ) )
        {
            VectorCopy( tr->trBase, result );
            break;
        }
        deltaTime = ( atTime - tr->trTime - ( 30*(tr->trDuration%5)) ) * 0.001;	// milliseconds to seconds
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        CrossProduct( tr->trDelta, relativeUp, relativeX );
        CrossProduct( tr->trDelta, relativeX, relativeUp );
        VectorNormalize( relativeX );
        VectorNormalize( relativeUp );
        VectorScale( relativeX, 25 * sin( deltaTime * 15 + (tr->trDuration%3)*(2*M_PI/3.0f) ), relativeX ); //increase the multiplier on deltaTime to make it spin faster
        VectorScale( relativeUp, 25 * cos( deltaTime * 15 + (tr->trDuration%3)*(2*M_PI/3.0f) ), relativeUp ); //adding to the equation offsets the particle by a certain about
        //Com_Printf(    "relativeX = <%f, %f, %f>\n", relativeX[0], relativeX[1], relativeX[2]);
        //Com_Printf(    "relativeUp = <%f, %f, %f>\n", relativeUp[0], relativeUp[1], relativeUp[2]);
        VectorAdd( result, relativeX, result );
        VectorAdd( result, relativeUp, result );
        break;
    case TR_HELIX2:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        CrossProduct( tr->trDelta, relativeUp, relativeX );
        CrossProduct( tr->trDelta, relativeX, relativeUp );
        VectorNormalize( relativeX );
        VectorNormalize( relativeUp );
        VectorScale( relativeX, 25 * sin( deltaTime * 15 + (tr->trDuration*M_PI) ), relativeX ); //increase the multiplier on deltaTime to make it spin faster
        VectorScale( relativeUp, 25 * cos( deltaTime * 15 + (tr->trDuration*M_PI) ), relativeUp ); //adding to the equation offsets the particle on the circle by a certain about
        //Com_Printf(    "relativeX = <%f, %f, %f>\n", relativeX[0], relativeX[1], relativeX[2]);
        //Com_Printf(    "relativeUp = <%f, %f, %f>\n", relativeUp[0], relativeUp[1], relativeUp[2]);
        VectorAdd( result, relativeX, result );
        VectorAdd( result, relativeUp, result );
        break;
    case TR_ACCEL:
        // time since missile fired in seconds
        deltaTime = ( atTime - tr->trTime ) * 0.001;
        // the .5*a*t^2 part. trDuration = acceleration,
        // phase gives the magnitude of the distance
        // we need to move
        if( ( deltaTime >= fabs(VectorLength(tr->trDelta)/tr->trDuration) - .01 ) && ( tr->trDuration < 0 ) )
        {
            deltaTime = fabs(VectorLength(tr->trDelta)/tr->trDuration) - .01;	//make it -.01 so that we have SOME velocity
        }
        phase = (tr->trDuration / 2) * (deltaTime * deltaTime);
        // Make dir equal to the velocity of the object
        VectorCopy (tr->trDelta, dir);
        // Sets the magnitude of vector dir to 1
        VectorNormalize (dir);
        // Move a distance "phase" in the direction "dir"
        // from our starting point
        VectorMA (tr->trBase, phase, dir, result);
        // The u*t part. Adds the velocity of the object
        // multiplied by the time to the last result.
        VectorScale (dir, tr->trDuration, dir);
        VectorMA (tr->trDelta, deltaTime, dir, vel);
        //Com_Printf(    "time since firing is %f\n", deltaTime );
        //Com_Printf(    "objects velocity is %f\n", VectorLength( vel ) );
        //Com_Printf(    "time where vel=0 is %f\n", fabs(VectorLength(tr->trDelta)/tr->trDuration) );
        VectorMA (result, deltaTime, tr->trDelta, result);
        break;
    case TR_GRAVITY:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
        break;
    case TR_RISE:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        result[2] += 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
        break;
    case TR_BOOMERANG:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds ( 250000 = 2 mins, 50 sec )
        phase = ( tr->trDuration * 0.001 );
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        if ( phase <= tr->trDuration )
        {
            result[0] -= 0.5 * DEFAULT_GRAVITY * deltaTime;	   // FIXME: local gravity...
        }
        VectorNormalize ( result );
        if ( phase >= tr->trDuration && phase <= (tr->trDuration + 1.5))
        {
            result[0] += 0.5 * DEFAULT_GRAVITY * deltaTime;
        }
        VectorNormalize ( result );
        if ( phase >= (tr->trDuration + 1.5) && phase <= (tr->trDuration + 2.5))
        {
            result[0] += 0.5 * DEFAULT_GRAVITY * deltaTime;
        }
        break;
    default:
        Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
        break;
    }
}
/*
trBase = Vector of muzzle origin, defined in _fire function of entity
(( VectorCopy( start, bolt->s.pos.trBase ); )) __ I think it calculates angle or dir too.
trDelta = Vector of dir (forward) (player view angle) scaled by ___ value, defined in _fire function of entity
(( VectorScale( dir, 700, bolt->s.pos.trDelta ); ))

entityShared_t	= (r)
entityState_t	= (s)

result[2] -= 0.5 * deltaTime;  // This does not work - freezes game */
/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result )
{
    float	deltaTime;
    float	phase;
    vec3_t	dir;

    switch( tr->trType )
    {
    case TR_STATIONARY:
    case TR_INTERPOLATE:
        VectorClear( result );
        break;
    case TR_LINEAR:
        VectorCopy( tr->trDelta, result );
        break;
    case TR_SINE:
        deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
        phase = cos( deltaTime * M_PI * 2 );	// derivative of sin = cos
        phase *= 0.5;
        VectorScale( tr->trDelta, phase, result );
        break;
    case TR_CIRCLE:
    case TR_HELIX:
    case TR_HELIX2:
        VectorCopy( tr->trDelta, result );
        break;
    case TR_ACCEL:
        // time since missile fired in seconds
        deltaTime = ( atTime - tr->trTime ) * 0.001;
        // Turn magnitude of acceleration into a vector
        VectorCopy(tr->trDelta,dir);
        VectorNormalize (dir);
        VectorScale (dir, tr->trDuration, dir);
        if( ( deltaTime >= fabs(VectorLength(tr->trDelta)/tr->trDuration) - .01 ) && ( tr->trDuration < 0 ) )
        {
            deltaTime = fabs(VectorLength(tr->trDelta)/tr->trDuration) - .01;	//make it -.01 so that we have SOME velocity
        }
        // u + t * a = v
        VectorMA (tr->trDelta, deltaTime, dir, result);
        //Com_Printf(    "objects velocity delta is %f\n", VectorLength( result ) );
        break;
    case TR_LINEAR_STOP:
        if ( atTime > tr->trTime + tr->trDuration )
        {
            VectorClear( result );
            return;
        }
        VectorCopy( tr->trDelta, result );
        break;
    case TR_GRAVITY:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        VectorCopy( tr->trDelta, result );
        result[2] -= DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
        break;
    case TR_RISE:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
        VectorCopy( tr->trDelta, result );
        result[2] += DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
        break;
    case TR_BOOMERANG:
        deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds ( 250000 = 2 mins, 50 sec )
        phase = ( tr->trDuration * 0.001 );
        VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
        if ( phase <= tr->trDuration )
        {
            result[0] -= 0.5 * DEFAULT_GRAVITY * deltaTime;	   // FIXME: local gravity...
        }
        VectorNormalize ( result );
        if ( phase >= tr->trDuration && phase <= (tr->trDuration + 1.5))
        {
            result[0] += 0.5 * DEFAULT_GRAVITY * deltaTime;
        }
        VectorNormalize ( result );
        if ( phase >= (tr->trDuration + 1.5) && phase <= (tr->trDuration + 2.5))
        {
            result[0] += 0.5 * DEFAULT_GRAVITY * deltaTime;
        }
        break;
    default:
        Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
        break;
    }
}

char *eventnames[] =
{
    "EV_NONE",

    "EV_FOOTSTEP",
    "EV_FOOTSTEP_METAL",
    "EV_FOOTSPLASH",
    "EV_FOOTWADE",
    "EV_SWIM",

    "EV_STEP_4",
    "EV_STEP_8",
    "EV_STEP_12",
    "EV_STEP_16",

    "EV_FALL_SHORT",
    "EV_FALL_MEDIUM",
    "EV_FALL_FAR",

    "EV_JUMP_PAD",			// boing sound at origin", jump sound on player

    "EV_JUMP",
    "EV_WATER_TOUCH",	// foot touches
    "EV_WATER_LEAVE",	// foot leaves
    "EV_WATER_UNDER",	// head touches
    "EV_WATER_CLEAR",	// head leaves

    "EV_ITEM_PICKUP",			// normal item pickups are predictable
    "EV_GLOBAL_ITEM_PICKUP",	// powerup / team sounds are broadcast to everyone

    "EV_NOAMMO",
    "EV_CHANGE_WEAPON",
    "EV_FIRE_WEAPON",
    "EV_FIRE_OFFHAND",

    "EV_USE_ITEM0",
    "EV_USE_ITEM1",
    "EV_USE_ITEM2",
    "EV_USE_ITEM3",
    "EV_USE_ITEM4",
    "EV_USE_ITEM5",
    "EV_USE_ITEM6",
    "EV_USE_ITEM7",
    "EV_USE_ITEM8",
    "EV_USE_ITEM9",
    "EV_USE_ITEM10",
    "EV_USE_ITEM11",
    "EV_USE_ITEM12",
    "EV_USE_ITEM13",
    "EV_USE_ITEM14",
    "EV_USE_ITEM15",

    "EV_ITEM_RESPAWN",
    "EV_ITEM_POP",
    "EV_PLAYER_TELEPORT_IN",
    "EV_PLAYER_TELEPORT_OUT",
    "EV_SKYLIGHTNING",

    "EV_GRENADE_BOUNCE",		// eventParm will be the soundindex

    "EV_GENERAL_SOUND",
    "EV_GLOBAL_SOUND",		// no attenuation
    "EV_GLOBAL_TEAM_SOUND",

    "EV_BULLET_HIT_FLESH",
    "EV_BULLET_HIT_WALL",

    "EV_MISSILE_HIT",
    "EV_MISSILE_MISS",
    "EV_MISSILE_MISS_METAL",
    "EV_BLASER_HIT",
    "EV_BLASER_MISS",
    "EV_RAILTRAIL",
    "EV_BULLET",				// otherEntity is the shooter
    "EV_LIGHTNING_DISCHARGE",

    "EV_PAIN",
    "EV_DEATH1",
    "EV_DEATH2",
    "EV_DEATH3",
    "EV_OBITUARY",

    "EV_POWERUP_QUAD",
    "EV_POWERUP_BATTLESUIT",
    "EV_POWERUP_REGEN",

    "EV_GIB_PLAYER",			// gib a previously living player
    "EV_BREAK_GLASS",
    "EV_EXPLOSIVE",

    "EV_CHARGE1",
    "EV_CHARGE2",
    "EV_CHARGE3",
    "EV_CHARGE4",
    "EV_NOCHARGE",
    "EV_CAPSULE",

    "EV_GRAVITYWELL",
    "EV_SCOREPLUM",			// score plum

    "EV_EARTHQUAKE",
    "EV_PLAYERSTOP",
    "ET_MODELANIM",
    "EV_DEBUG_LINE",
    "EV_STOPLOOPINGSOUND",
    "EV_TAUNT"

};

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/

void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps )
{

#ifdef _DEBUG
    {
        char buf[256];
        trap_Cvar_VariableStringBuffer("showevents", buf, sizeof(buf));
        if ( atof(buf) != 0 )
        {
#ifdef QAGAME
            Com_Printf(   " game event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#else
            Com_Printf(   "Cgame event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#endif
        }
    }
#endif
    ps->events[ps->eventSequence & (MAX_PS_EVENTS-1)] = newEvent;
    ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS-1)] = eventParm;
    ps->eventSequence++;
}

/*
========================
BG_TouchJumpPad
========================
*/
void BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad )
{
    qboolean	not_charging = ( ps->weaponstate != WEAPON_CHARGING ) && ( ps->weaponstate != WEAPON_OHCHARGING );
    vec3_t	angles;
    float p;
    int effectNum;

    // spectators don't use jump pads
    if ( ps->pm_type != PM_NORMAL )
    {
        return;
    }
    // if you have the jetpack active, dont hit the bounce pad
    if ( ps->powerups[PW_JETPACK] )
    {
        return;
    }

    // cancel tech attacks
    if( ps->weaponstate >= WEAPON_TECHUPCUT )
    {
        ps->weaponstate = WEAPON_READY;
    }

    // if we didn't hit this same jumppad the previous frame
    // then don't play the event sound again if we are in a fat trigger
    if ( ps->jumppad_ent != jumppad->number )
    {

        vectoangles( jumppad->origin2, angles);
        p = fabs( AngleNormalize180( angles[PITCH] ) );
        if( p < 45 )
        {
            effectNum = 0;
        }
        else
        {
            effectNum = 1;
        }
        BG_AddPredictableEventToPlayerstate( EV_JUMP_PAD, effectNum, ps );
    }
    // remember hitting this jumppad this frame
    ps->jumppad_ent = jumppad->number;
    ps->jumppad_frame = ps->pmove_framecount;
    // give the player the velocity from the jumppad
    VectorCopy( jumppad->origin2, ps->velocity );
    // give them jump animations
    if( not_charging )
    {
        ps->torsoAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )	| TORSO_JUMP;
        ps->torsoTimer = 500;
    }
    ps->legsAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT )	| LEGS_JUMP;
    ps->legsTimer = 500;
    ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
}

/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap )
{
    int		i;

    if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR || ps->powerups[PW_CLOAK] )
    {
        s->eType = ET_INVISIBLE;
    }
    else if ( ps->stats[STAT_HEALTH] <= GIB_HEALTH )
    {
        // we will take care of the invisibility
        s->eType = ET_INVISIBLE;
    }
    else
    {
        s->eType = ET_PLAYER;
    }

    s->number = ps->clientNum;

    s->pos.trType = TR_INTERPOLATE;
    VectorCopy( ps->origin, s->pos.trBase );
    if ( snap )
    {
        SnapVector( s->pos.trBase );
    }
    // set the trDelta for flag direction
    VectorCopy( ps->velocity, s->pos.trDelta );

    s->apos.trType = TR_INTERPOLATE;
    VectorCopy( ps->viewangles, s->apos.trBase );
    if ( snap )
    {
        SnapVector( s->apos.trBase );
    }

    s->angles2[YAW] = ps->movementDir;
    s->legsAnim = ps->legsAnim;
    s->torsoAnim = ps->torsoAnim;
    s->clientNum = ps->clientNum;		// ET_PLAYER looks here instead of at number
    // so corpses can also reference the proper config
    s->eFlags = ps->eFlags;
    if ( ps->stats[STAT_HEALTH] <= 0 )
    {
        s->eFlags |= EF_DEAD;
    }
    else
    {
        s->eFlags &= ~EF_DEAD;
    }

    if ( ps->externalEvent )
    {
        s->event = ps->externalEvent;
        s->eventParm = ps->externalEventParm;
    }
    else if ( ps->entityEventSequence < ps->eventSequence )
    {
        int		seq;

        if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS)
        {
            ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
        }
        seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
        s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
        s->eventParm = ps->eventParms[ seq ];
        ps->entityEventSequence++;
    }

    s->weapon = ps->weapon;
    s->groundEntityNum = ps->groundEntityNum;

    s->powerups = 0;
    for ( i = 0 ; i < MAX_POWERUPS ; i++ )
    {
        if ( ps->powerups[ i ] )
        {
            s->powerups |= 1 << i;
        }
    }

    s->loopSound = ps->loopSound;
    s->generic1 = ps->generic1;
}

/*
========================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap )
{
    int		i;

    if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR )
    {
        s->eType = ET_INVISIBLE;
    }
    else if ( ps->stats[STAT_HEALTH] <= GIB_HEALTH )
    {
        // we will take care of the invisibility
        s->eType = ET_INVISIBLE;
    }
    else
    {
        s->eType = ET_PLAYER;
    }

    s->number = ps->clientNum;

    s->pos.trType = TR_LINEAR_STOP;
    VectorCopy( ps->origin, s->pos.trBase );
    if ( snap )
    {
        SnapVector( s->pos.trBase );
    }
    // set the trDelta for flag direction and linear prediction
    VectorCopy( ps->velocity, s->pos.trDelta );
    // set the time for linear prediction
    s->pos.trTime = time;
    // set maximum extra polation time
    s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

    s->apos.trType = TR_INTERPOLATE;
    VectorCopy( ps->viewangles, s->apos.trBase );
    if ( snap )
    {
        SnapVector( s->apos.trBase );
    }

    s->angles2[YAW] = ps->movementDir;
    s->legsAnim = ps->legsAnim;
    s->torsoAnim = ps->torsoAnim;
    s->clientNum = ps->clientNum;		// ET_PLAYER looks here instead of at number
    // so corpses can also reference the proper config
    s->eFlags = ps->eFlags;
    if ( ps->stats[STAT_HEALTH] <= 0 )
    {
        s->eFlags |= EF_DEAD;
    }
    else
    {
        s->eFlags &= ~EF_DEAD;
    }

    if ( ps->externalEvent )
    {
        s->event = ps->externalEvent;
        s->eventParm = ps->externalEventParm;
    }
    else if ( ps->entityEventSequence < ps->eventSequence )
    {
        int		seq;

        if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS)
        {
            ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
        }
        seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
        s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
        s->eventParm = ps->eventParms[ seq ];
        ps->entityEventSequence++;
    }

    s->weapon = ps->weapon;
    s->groundEntityNum = ps->groundEntityNum;

    s->powerups = 0;
    for ( i = 0 ; i < MAX_POWERUPS ; i++ )
    {
        if ( ps->powerups[ i ] )
        {
            s->powerups |= 1 << i;
        }
    }

    s->loopSound = ps->loopSound;
    s->generic1 = ps->generic1;
}
