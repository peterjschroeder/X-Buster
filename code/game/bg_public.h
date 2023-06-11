// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_public.h -- definitions shared by both the server game and client game modules

// because games can change separately from the main system version, we need a
// second version that must match between game and cgame

#define	GAME_VERSION		"baseq3-1"

#define	DEFAULT_GRAVITY		800
#define	GIB_HEALTH			0		// changed from -40 to 0, we want every death to be a "gib"
#define	ARMOR_PROTECTION	0.66

#define	MAX_ITEMS			256

#define	RANK_TIED_FLAG		0x4000


#define	ITEM_RADIUS			15		// item sizes are needed for client side pickup detection

#define	LIGHTNING_RANGE		768

#define	SCORE_NOT_PRESENT	-9999	// for the CS_SCORES[12] when only one player is present

#define	VOTE_TIME			30000	// 30 seconds before vote times out

#define	MINS_Z				-24
#define	DEFAULT_VIEWHEIGHT	26
#define CROUCH_VIEWHEIGHT	12
#define	DEAD_VIEWHEIGHT		-16

//
// config strings are a general means of communicating variable length strings
// from the server to all connected clients.
//

// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
#define	CS_MUSIC				2
#define	CS_MESSAGE				3		// from the map worldspawn's message field
#define	CS_MOTD					4		// g_motd string for server message of the day
#define	CS_WARMUP				5		// server time when the match will be restarted
#define	CS_SCORES1				6
#define	CS_SCORES2				7
#define CS_VOTE_TIME			8
#define CS_VOTE_STRING			9
#define	CS_VOTE_YES				10
#define	CS_VOTE_NO				11

#define CS_TEAMVOTE_TIME		12
#define CS_TEAMVOTE_STRING		14
#define	CS_TEAMVOTE_YES			16
#define	CS_TEAMVOTE_NO			18

#define	CS_GAME_VERSION			20
#define	CS_LEVEL_START_TIME		21		// so the timer only shows the current level
#define	CS_INTERMISSION			22		// when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS			23		// string indicating flag status in CTF
#define CS_SHADERSTATE			24
#define CS_BOTINFO				25

#define	CS_ITEMS				27		// string of 0's and 1's that tell which items are present
#define CS_ATMOSEFFECT  	  	28
#define CS_NPCS					29

#define	CS_MODELS				32
#define	CS_SOUNDS				(CS_MODELS+MAX_MODELS)
#define	CS_PLAYERS				(CS_SOUNDS+MAX_SOUNDS)
#define CS_LOCATIONS			(CS_PLAYERS+MAX_CLIENTS)
#define CS_PARTICLES			(CS_LOCATIONS+MAX_LOCATIONS)

#define CS_MAX					(CS_PARTICLES+MAX_LOCATIONS)

#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

typedef enum
{
    GT_FFA,				// free for all
    GT_TOURNAMENT,		// one on one tournament
    GT_SINGLE_PLAYER,	// single player ffa

    //-- team games go after this --

    GT_TEAM,			// team deathmatch
    GT_CTF,				// capture the flag
    GT_MAX_GAME_TYPE
} gametype_t;

typedef enum { GENDER_MALE, GENDER_FEMALE, GENDER_NEUTER } gender_t;

/*
===================================================================================

PMOVE MODULE

The pmove code takes a player_state_t and a usercmd_t and generates a new player_state_t
and some other output data.  Used for local prediction on the client game and true
movement on the server game.
===================================================================================
*/

typedef enum
{
    PM_NORMAL,		// can accelerate and turn
    PM_NOCLIP,		// noclip movement
    PM_SPECTATOR,	// still run into walls
    PM_DEAD,		// no acceleration or turning, but free falling
    PM_FREEZE,		// stuck in place with no control
    PM_INTERMISSION,	// no movement or status bar
    PM_SPINTERMISSION	// no movement or status bar
} pmtype_t;

typedef enum
{
    WEAPON_READY,
    WEAPON_RAISING,
    WEAPON_DROPPING,
    WEAPON_FIRING,
    WEAPON_OHFIRING,
    WEAPON_CHARGING,
    WEAPON_OHCHARGING,
    WEAPON_TECHUPCUT,
    WEAPON_TECHSPIKE,
    WEAPON_TECHSPIN,
} weaponstate_t;

// pmove->pm_flags
#define	PMF_GRAVWELL		1
#define	PMF_JUMP_HELD		2
#define	PMF_AIR_JUMP1		4
#define	PMF_BACKWARDS_JUMP	8		// go into backwards land
#define	PMF_BACKWARDS_RUN	16		// coast down to backwards run
#define	PMF_TIME_LAND		32		// pm_time is time before rejump
#define	PMF_TIME_KNOCKBACK	64		// pm_time is an air-accelerate only time
#define	PMF_WALL_HANG		128
#define	PMF_TIME_WATERJUMP	256		// pm_time is waterjump
#define	PMF_RESPAWNED		512		// clear after attack and jump buttons come up
#define	PMF_USE_ITEM_HELD	1024
#define PMF_GRAPPLE_PULL	2048	// pull towards grapple location
#define PMF_FOLLOW			4096	// spectate following another player
#define PMF_SCOREBOARD		8192	// spectate as a scoreboard
#define PMF_DASH_HELD		16384	// are you holding dash?
#define	PMF_AIRDASHING		32768
#define	PMF_CAN_AIRDASH		65536
#define PMF_WALLWALKING      131072
#define PMF_WALLWALKINGCEILING      262144

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK)

#define	MAXTOUCH	32
typedef struct
{
    // state (in / out)
    playerState_t	*ps;

    // command (in)
    usercmd_t	cmd;
    int			tracemask;			// collide against these types of surfaces
    int			debugLevel;			// if set, diagnostic output will be printed
    qboolean	noFootsteps;		// if the game is setup for no footsteps by the server
    qboolean	gauntletHit;		// true if a gauntlet attack would actually hit something

    int			framecount;

    // results (out)
    int			numtouch;
    int			touchents[MAXTOUCH];

    vec3_t		mins, maxs;			// bounding box size

    int			watertype;
    int			waterlevel;

    float		xyspeed;

    // for fixed msec Pmove
    int			pmove_fixed;
    int			pmove_msec;

    // callbacks to test the world
    // these will be different functions during game and cgame
    void		(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
    int			(*pointcontents)( const vec3_t point, int passEntityNum );
} pmove_t;

// if a full pmove isn't done on the client, you can just update the angles
void PM_UpdateViewAngles( playerState_t *ps, const usercmd_t *cmd );
void Pmove (pmove_t *pmove);

//===================================================================================


// player_state->stats[] indexes
// NOTE: may not have more than 16
typedef enum
{
    STAT_HEALTH,
    STAT_HOLDABLE_ITEM,
    STAT_WEAPONS,					// 16 bit fields
    STAT_ARMOR,
    STAT_DEAD_YAW,					// look this direction when dead (FIXME: get rid of?)
    STAT_CLIENTS_READY,				// bit mask of clients wishing to exit the intermission (FIXME: configstring?)
    STAT_MAX_HEALTH,				// health / armor limit, changable by handicap
    STAT_ENERGY,					//Energy for special (non-standard) attacks
    STAT_FUEL,						//fuel for things like the jetpack
    STAT_CHARGE,					//charge level - [0 - standard shot], [1 - slightly bigger], [2 - a little more], [3 - huge], [4 - special]
    STAT_FLAGS						//I need more control of flags, because this just aint cuttin it :
} statIndex_t;


// player_state->persistant[] indexes
// these fields are the only part of player_state that isn't
// cleared on respawn
// NOTE: may not have more than 16
typedef enum
{
    PERS_SCORE,						// !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
    PERS_HITS,						// total points damage inflicted so damage beeps can sound on change
    PERS_RANK,						// player rank or team rank
    PERS_TEAM,						// player team
    PERS_SPAWN_COUNT,				// incremented every respawn
    PERS_ATTACKER,					// clientnum of last damage inflicter
    PERS_ATTACKEE_ARMOR,			// health/armor of last person we attacked
    PERS_KILLED,					// count of the number of times you died
    PERS_MAX_ARMOR,
    PERS_SUBTANKS,					//sub tanks to hold extra health (really, it just holds a number, divide by STAT_HEALTH to get the numbers)
    PERS_MAX_ENERGY,				//Maximum energy (can change as you get upgrades)
    PERS_CAPTURES,					// Peter: Used for exp and captures
    PERS_CHIPS,						//number of chips player has (used to buy upgrades)
    PERS_UPGRADES,					//what upgrades do we have?
    PLAYERCLASS						//added for player classes
} persEnum_t;


// entityState_t->eFlags
#define	EF_DEAD				0x00000001		// don't draw a foe marker over players with EF_DEAD
#define	EF_TELEPORT_BIT		0x00000002		// toggled every time the origin abruptly changes
#define EF_PLAYER_EVENT		0x00000004
#define	EF_BOUNCE			0x00000008		// for missiles
#define	EF_BOUNCE_HALF		0x00000010		// for missiles
#define	EF_NODRAW			0x00000020		// may have an event, but no model (unspawned items)
#define	EF_FIRING			0x00000040		// for lightning gun
#define	EF_MOVER_STOP		0x00000080		// will push otherwise
#define	EF_TALK				0x00000100		// draw a talk balloon
#define	EF_CONNECTION		0x00000200		// draw a connection trouble sprite
#define	EF_VOTED			0x00000400		// already cast a vote
#define EF_TEAMVOTED		0x00000800		// already cast a team vote
#define EF_DAMAGED			0x00001000		//below 30 health
#define EF_WALLHANG			0x00002000		//hanging on the wall
#define EF_FROZEN			0x00004000		//frozen
#define EF_SLOW				0x00008000		//slowed down
#define EF_SHOCKED			0x00010000		//shocked
#define EF_LIGHTNSTRUCK		0x00020000		//struck by lightning
#define EF_LAGGED			0x00040000		//lagged
#define EF_GRAVWELL			0x00080000		//has a gravity well out
#define	EF_FORCE_END_FRAME	0x00100000


// NOTE: may not have more than 16
typedef enum
{
    PW_NONE,

    PW_REDFLAG,
    PW_BLUEFLAG,
    PW_NEUTRALFLAG,
    PW_DASH,
    PW_JETPACK,
//upgrades
    PW_FOCUS,
    PW_POWERCONVERTER,
    PW_AIRJUMP,
    PW_BUSTER,
    PW_GENERIC,	//for generic timers, like the Z-Buster
    PW_CHARGE2,	//for any secondary charge function
    PW_CLOAK,

    PW_NUM_POWERUPS

} powerup_t;

typedef enum
{
    HI_NONE,

    HI_SUBTANK,
    // Peter FIXME: Do these need to be holdable?
    // the various chips are holdables I guess...
    HI_BOLTS,
    HI_CHIPS,

    HI_NUM_HOLDABLE
} holdable_t;


typedef enum
{
    //Weapon slots
    WP_RNONE,		//0

    WP_DEFAULT,		//1
    WP_FIRE,		//2
    WP_WATER,		//3
    WP_AIR,			//4
    WP_EARTH,		//5
    WP_TECH,		//6
    WP_NATURE,		//7
    WP_GRAVITY,		//8
    WP_LIGHT,		//9
    WP_ELECTRICAL,	//10
    WP_OTHER10,		//11
    WP_OTHER11,		//12
    WP_OTHER12,		//13
    WP_OTHER13,		//14
    WP_OTHER14,		//15

    WP_NUMR_WEAPONS
} weapon_t; //replaced original weapon_t

typedef enum
{
    WP_NONE,

    // Peter FIXME: Redo weapon system, so the follow isn't needed
    RWP_DEFAULT,
    RWP_FIRE,
    RWP_WATER,
    RWP_AIR,
    RWP_EARTH,
    RWP_TECH,
    RWP_NATURE,
    RWP_GRAVITY,
    RWP_LIGHT,
    RWP_ELECTRICAL,

    //busters
    WP_XBUSTER,		//MMX's main cannon
    WP_MBUSTER,		//MM's main cannon
    WP_PBUSTER,		//Proto's main cannon
    WP_FBUSTER,		//Forte's main weapon of destruction
    WP_ZSABER,		//Zero's arm cannon
    //Flame attacks
    WP_FCUTTER, //charge
    WP_HWAVE,	//buster only
    WP_RISFIRE,	//drn000
    WP_PSTRIKE,	//rapid
    WP_FIRE_M,	//melee
    //Water attacks
    WP_SICE,	//charge
    WP_FBURST,	//buster only
    WP_IBURST,	//drn000
    WP_WSTRIKE,	//rapid
    WP_WATER_M,	//melee
    //Air attacks
    WP_AFIST,	//charge
    WP_AWAVE,	//buster only
    WP_IMBOOMERANG,	//buster only
    WP_TEMPEST,	//rapid
    WP_AIR_M,	//melee
    //Earth attacks
    WP_BCRYSTAL,	//charge
    WP_RSTONE,	//buster only
    WP_MAKEME4,	//buster only
    WP_SBURST,	//rapid
    WP_EARTH_M,	//melee
    //Metal attacks
    WP_LTORPEDO,	//charge
    WP_BLAUNCHER,	//buster only
    WP_STRIKECHAIN,	//proto
    WP_DSHOT,	//rapid
    WP_METAL_M,	//melee
    //Nature attacks
    WP_TBURSTER,//charge
    WP_LSTORM,	//buster only
    WP_MAKEME5,	//buster only
    WP_ASPLASHER,	//rapid
    WP_NATURE_M,//melee
    //Gravity attacks
    WP_GBOMB,	//charge
    WP_MMINE,	//buster only
    WP_GRAVWELL,	//drn000
    WP_DMATTER,	//rapid
    WP_GRAV_M,	//melee
    //Light attacks
    WP_BLASER,//charge
    WP_SSHREDDER,	//buster only
    WP_MAKEME7,	//buster only
    WP_NBURST,	//rapid
    WP_LIGHT_M,	//melee
    //Electrical attacks
    WP_SFIST,	//charge
    WP_BSHIELD,	//buster only
    WP_TRTHUNDER,	//buster only
    WP_BLIGHTNING,	//rapid
    WP_ELEC_M,	//melee

    WP_NUM_WEAPONS,
    //put special charged weapons here for cgame to use for the model
    WP_XBUSTER1,
    WP_XBUSTER2,
    WP_XBUSTER3,
    WP_XBUSTER4,
    WP_MBUSTER1,
    WP_MBUSTER2,
    WP_PBUSTER1,
    WP_PBUSTER2,
    WP_ZBUSTER,
    WP_ZBUSTER1,
    WP_ZBUSTER2,
    WP_FCUTTER2,
    WP_HWAVE2,
    WP_SICE2,
    WP_SICE3,
    WP_AFIST2,
    WP_BCRYSTAL2,
    WP_BCRYSTAL3,
    WP_BCRYSTAL4,
    WP_LTORPEDO2,
    WP_LTORPEDO3,
    WP_TBURSTER2,
    WP_GBOMB2,
    WP_BLASER2,
    WP_SFIST2,

    WP_NUMALLWEAPONS
//formerly weapon_t
} weaponr_t;

//classes added
typedef enum
{
    CLASS_DRN00X,
    CLASS_DRN000,
    CLASS_DRN001,
    CLASS_SWN001,
    CLASS_DWNINFINITY,

    MAX_CLASS
} CLASS_t;
typedef enum
{
    UG_NONE,

    // Head
    UG_FOCUS,
    UG_RADAR,
    UG_TRANSPERS,
    UG_SCANNER,
    UG_SENSE,

    // Back
    UG_WALLJUMP,
    UG_360DASH,
    UG_SPEEDBOOST,
    UG_WALLCLIMB,

    // Body
    UG_POWERCONVERTER,
    UG_REGEN,
    UG_CLOAK,
    UG_ARMORBOOST,
    UG_ABSORB,
    UG_RAGE,

    // Buster
    UG_BUSTER,
    UG_QUICKCHARGE,
    UG_ENERGYSAVER,
    UG_DRAIN,
    UG_AMMOBOOST,
    UG_MELEEBOOST,
    UG_DOUBLE,

    // Legs
    UG_AIRDASH,
    UG_AIRJUMP,
    UG_HIGHJUMP,
    UG_DASH,
    UG_JETBOOTS,
    UG_MOONBOOTS,
    UG_SUPERAIRJUMP,

    UG_NUM_UPGRADES
} upgrades_t;
typedef struct
{
    char *name;
    char *desc;
    int price;
} upgrade_t;

extern upgrade_t upgrade_table[];

typedef enum
{
    // ps flags that arent changed in pmove
    SF_CAPSULE		= 0x0001,			// in a capsule
    SF_BANK			= 0x0002,			// in a bank
    SF_GARAGE		= 0x0004,			// in a shop
    SF_SABER_SWING1 = 0x0008,			//first swing
    SF_SABER_SWING2 = 0x0010,			//second swing
    SF_SABER_SWING3 = 0x0020,			//third (heavy) swing
} eflags_t;


typedef struct
{
    int	weapon;		//the actual weapon integer (i.e. WP_ROCKET_LAUNCHER)
    int	energyuse;	//amount of energy used per cycle
    int	addtime;	//the time in between shots
    int	splashradius;	//the splash radius
} weaponinfo2_t;
//added for suit info
typedef struct
{
    char    *model;
    int		maxenergy;		//starting energy
    int		maxarmor;		//starting armor
    int		maxhealth;		//maximum health
    int		jumpvelocity;	//jump velocity
    float	speed;			//ground speed factor
    int		radarrange;		//sensor range
    weaponinfo2_t weapons[16];	//array of weapons for the player
    int     ug_head[3];
    int		ug_back[3];
    int		ug_body[3];
    int		ug_buster[3];
    int		ug_legs[3];
} playerinfo2_t;

//following added for more weapons, suit info
extern playerinfo2_t bg_playerlist[];



// entityState_t->event values
// entity events are for effects that take place reletive
// to an existing entities origin.  Very network efficient.

// two bits at the top of the entityState->event field
// will be incremented with each change in the event so
// that an identical event started twice in a row can
// be distinguished.  And off the value with ~EV_EVENT_BITS
// to retrieve the actual event number
#define	EV_EVENT_BIT1		0x00000100
#define	EV_EVENT_BIT2		0x00000200
#define	EV_EVENT_BITS		(EV_EVENT_BIT1|EV_EVENT_BIT2)

#define	EVENT_VALID_MSEC	300

typedef enum
{
    EV_NONE,

    EV_FOOTSTEP,
    EV_FOOTSTEP_METAL,
    EV_FOOTSPLASH,
    EV_FOOTWADE,
    EV_SWIM,

    EV_STEP_4,
    EV_STEP_8,
    EV_STEP_12,
    EV_STEP_16,

    EV_FALL_SHORT,
    EV_FALL_MEDIUM,
    EV_FALL_FAR,

    EV_JUMP_PAD,			// boing sound at origin, jump sound on player

    EV_JUMP,
    EV_WATER_TOUCH,	// foot touches
    EV_WATER_LEAVE,	// foot leaves
    EV_WATER_UNDER,	// head touches
    EV_WATER_CLEAR,	// head leaves

    EV_ITEM_PICKUP,			// normal item pickups are predictable
    EV_GLOBAL_ITEM_PICKUP,	// powerup / team sounds are broadcast to everyone

    EV_NOAMMO,
    EV_CHANGE_WEAPON,
    EV_FIRE_WEAPON,
    EV_FIRE_OFFHAND,

    EV_USE_ITEM0,
    EV_USE_ITEM1,
    EV_USE_ITEM2,
    EV_USE_ITEM3,
    EV_USE_ITEM4,
    EV_USE_ITEM5,
    EV_USE_ITEM6,
    EV_USE_ITEM7,
    EV_USE_ITEM8,
    EV_USE_ITEM9,
    EV_USE_ITEM10,
    EV_USE_ITEM11,
    EV_USE_ITEM12,
    EV_USE_ITEM13,
    EV_USE_ITEM14,
    EV_USE_ITEM15,

    EV_ITEM_RESPAWN,
    EV_ITEM_POP,
    EV_PLAYER_TELEPORT_IN,
    EV_PLAYER_TELEPORT_OUT,
    EV_SKYLIGHTNING,

    EV_GRENADE_BOUNCE,		// eventParm will be the soundindex

    EV_GENERAL_SOUND,
    EV_GLOBAL_SOUND,		// no attenuation
    EV_GLOBAL_TEAM_SOUND,

    EV_BULLET_HIT_FLESH,
    EV_BULLET_HIT_WALL,

    EV_MISSILE_HIT,
    EV_MISSILE_MISS,
    EV_MISSILE_MISS_METAL,
    EV_BLASER_HIT,
    EV_BLASER_MISS,
    EV_RAILTRAIL,
    EV_BULLET,				// otherEntity is the shooter
    EV_LIGHTNING_DISCHARGE,

    EV_PAIN,

    EV_DEATH1,
    EV_DEATH2,
    EV_DEATH3,
    EV_OBITUARY,

    EV_POWERUP_QUAD,
    EV_POWERUP_BATTLESUIT,
    EV_POWERUP_REGEN,

    EV_GIB_PLAYER,			// gib a previously living player
    EV_BREAK_GLASS,
    EV_EXPLOSIVE,

    EV_CHARGE1,
    EV_CHARGE2,
    EV_CHARGE3,
    EV_CHARGE4,
    EV_NOCHARGE,
    EV_CAPSULE,

    EV_GRAVITYWELL,

    EV_SCOREPLUM,			// score plum

    EV_EARTHQUAKE,
    EV_PLAYERSTOP,
    ET_MODELANIM,
    EV_DEBUG_LINE,
    EV_STOPLOOPINGSOUND,
    EV_TAUNT,
    EV_TAUNT_YES,
    EV_TAUNT_NO,
    EV_TAUNT_FOLLOWME,
    EV_TAUNT_GETFLAG,
    EV_TAUNT_GUARDBASE,
    EV_TAUNT_PATROL

} entity_event_t;


typedef enum
{
    GTS_RED_CAPTURE,
    GTS_BLUE_CAPTURE,
    GTS_RED_RETURN,
    GTS_BLUE_RETURN,
    GTS_RED_TAKEN,
    GTS_BLUE_TAKEN,
    GTS_REDTEAM_SCORED,
    GTS_BLUETEAM_SCORED,
    GTS_REDTEAM_TOOK_LEAD,
    GTS_BLUETEAM_TOOK_LEAD,
    GTS_TEAMS_ARE_TIED
} global_team_sound_t;

// animations
typedef	enum
{
    BOTH_DEATH,
    BOTH_DEAD,
    BOTH_GESTURE,
    BOTH_POWERUP,		//plays after selecting upgrade...10 timer
    BOTH_GIGA,
    BOTH_ENTER,			//players first animation after beaming down
    BOTH_VICTORY,
    BOTH_DEFEND,
    //----------SABER SYSTEM------------//
    BOTH_CHARGED_AIR_ROLL,
    BOTH_CHARGED_AIR_DROP,	//( Drop from air - highest point transistion)
    BOTH_CHARGED_AIR_DROP_FIN,
    BOTH_CHARGED_STAB,		//ground stab
    BOTH_CHARGED_THRUST,	//charging sequence
    BOTH_CHARGED_THRUST_FIN,//(forward jab stab)
    BOTH_CHARGED_TACKLE,	//(shoulder-hit)
    BOTH_CHARGED_THROW,
    BOTH_CHARGED_THROW_FIN,
    BOTH_CHARGED_STRIKE,
    BOTH_CHARGED_STRIKE_FIN,
    BOTH_CHARGED_SPIN,
    BOTH_CHARGED_SPIN_AIR,
    BOTH_CHARGED_TWIRL,		//( weapon twirl ) - from normal charge animation
    BOTH_CHARGED_UPPER,		//( upper cut ) - from normal charge animation
    BOTH_DASH_SPIN,			//(special spin dash attack)
    BOTH_SLASH_HEAVY,		//(3rd combo hit)
    TORSO_SLASH1,
    TORSO_SLASH1_FIN,
    TORSO_SLASH2,
    TORSO_SLASH2_FIN,
    TORSO_SLASH_LEFT,
    TORSO_SLASH_LEFT_FIN,
    TORSO_SLASH_RIGHT,
    TORSO_SLASH_RIGHT_FIN,
    TORSO_SLASH_AIR,
    //----------TORSO ONLY--------------//
    TORSO_CHARGE_RUN,
    TORSO_CHARGE_JUMP,
    TORSO_CHARGE_DASH,
    TORSO_CHARGE,		//default weapon charge
    TORSO_CHARGE_HOLD,
    TORSO_ATTACK,		//for primary weapon
    TORSO_ATTACK_FIN,
    TORSO_CHARGE2_RUN,
    TORSO_CHARGE2_JUMP,
    TORSO_CHARGE2_DASH,
    TORSO_CHARGE2,		//charge weapon with shield up
    TORSO_CHARGE2_HOLD,
    TORSO_ATTACK2,		//shield up...
    TORSO_ATTACK2_FIN,	//puts shield away
    TORSO_ATTACK_RUN,	//player attack while runing (not for saber class)
    TORSO_ATTACK2_RUN,	//shield up while runing
    TORSO_ATTACK_JUMP,	//normal shot in air
    TORSO_ATTACK2_JUMP,	//jumping with shield
    TORSO_ATTACK_DASH,	//normal shot while dashing
    TORSO_ATTACK2_DASH,	//dashing with shield up
    TORSO_SHEILD_IDLE,
    TORSO_SHEILD_UP,
    TORSO_SHEILD_DOWN,
    TORSO_DROP,			//(MUST NOT CHANGE -- hand animation is synced to this)
    TORSO_RAISE,		//(MUST NOT CHANGE -- hand animation is synced to this)
    //--------WALL SYSTEM-------------// // NOT WORKED OUT YET //
    TORSO_ATTACK_WALL,	//player attack while stuck to a wall
    TORSO_ATTACK_MR,	//MID RIGHT FORWARDS (right from behind player on wall)
    TORSO_ATTACK_R,		//FULL RIGHT
    TORSO_ATTACK_MRD,	//MID RIGHT DOWN
    TORSO_ATTACK_D,		//FULL DOWN
    TORSO_ATTACK_MD,	//MID DOWN FOWARDS
    TORSO_ATTACK_MU,	//MID UP FORWARDS
    TORSO_ATTACK_U,		//FULL UP
    TORSO_ATTACK_MRU,	//MID RIGHT UP
    TORSO_ATTACK_MLU,	//MID LEFT UP
    TORSO_ATTACK_L,		//FULL LEFT
    TORSO_ATTACK_MLD,	//MID LEFT DOWN
    TORSO_ATTACK_ML,	//MID LEFT FOWARDS
    //--------IN SYNC ANIMATIONS-----------//
    TORSO_DASH,			//1 frame transition...no loop
    TORSO_DASH_FIN,		//slides to a stop
    TORSO_DASH_B,		//Backwards dash
    TORSO_STAND,
    TORSO_IDLEHURT,
    TORSO_WALK,
    TORSO_RUN,
    TORSO_BACK,
    TORSO_JUMP,
    TORSO_FALL,
    TORSO_LAND,
    TORSO_JUMPB,
    TORSO_FALLB,
    TORSO_LANDB,
    TORSO_DJUMP,		//double jump / also used for jetpack
    TORSO_WALL,
    LEGS_DASH,
    LEGS_DASH_FIN,		//slides to a stop
    LEGS_DASH_B,		//dashes backwards
    LEGS_IDLE,
    LEGS_IDLEHURT,
    LEGS_WALK,
    LEGS_RUN,
    LEGS_BACK,
    LEGS_JUMP,
    LEGS_FALL,
    LEGS_LAND,
    LEGS_JUMPB,
    LEGS_FALLB,
    LEGS_LANDB,
    LEGS_DJUMP,		//Performs double jump from end of normal legs_jump / jetpack also
    LEGS_WALL,
    //--------HEAD ANIMATIONS---------// (used for facial animations / hair animations)
    HEAD_SLASH1,
    HEAD_SLASH1_FIN,
    HEAD_SLASH2,
    HEAD_SLASH2_FIN,
    HEAD_SLASH_LEFT,
    HEAD_SLASH_LEFT_FIN,
    HEAD_SLASH_RIGHT,
    HEAD_SLASH_RIGHT_FIN,
    HEAD_SLASH_AIR,
    HEAD_CHARGE_RUN,
    HEAD_CHARGE_JUMP,
    HEAD_CHARGE_DASH,
    HEAD_CHARGE,		//default weapon charge (buster, saber)
    HEAD_CHARGE_HOLD,
    HEAD_ATTACK,		//for primary weapon
    HEAD_ATTACK_FIN,
    HEAD_CHARGE2_RUN,
    HEAD_CHARGE2_JUMP,
    HEAD_CHARGE2_DASH,
    HEAD_CHARGE2,		//secondary weapon (shield, buster((saber))
    HEAD_CHARGE2_HOLD,
    HEAD_ATTACK2,		//shield up...
    HEAD_ATTACK2_FIN,	//puts shield away
    HEAD_ATTACK_RUN,	//player attack while runing (not for saber class)
    HEAD_ATTACK2_RUN,	//shield up while runing
    HEAD_ATTACK_JUMP,	//normal shot in air
    HEAD_ATTACK2_JUMP,	//jumping with sheild
    HEAD_ATTACK_DASH,	//normal shot while dashing
    HEAD_ATTACK2_DASH,	//dashing with sheild up
    HEAD_DROP,			// (MUST NOT CHANGE -- hand animation is synced to this)
    HEAD_RAISE,			//(MUST NOT CHANGE -- hand animation is synced to this)
    HEAD_DASH,			//1 frame transition...no loop
    HEAD_DASH_FIN,		//slides to a stop
    HEAD_DASH_B,		//Backwards dash
    HEAD_IDLE,
    HEAD_IDLEHURT,
    HEAD_WALK,
    HEAD_RUN,
    HEAD_BACK,
    HEAD_JUMP,
    HEAD_FALL,
    HEAD_LAND,
    HEAD_JUMPB,
    HEAD_FALLB,
    HEAD_LANDB,
    HEAD_DJUMP,			//double jump / also used for jetpack
    HEAD_WALL,

    MAX_ANIMATIONS,

    LEGS_BACKWALK,
    FLAG_RUN,
    FLAG_STAND,
    FLAG_STAND2RUN,

    MAX_TOTALANIMATIONS
} animNumber_t;

// NPC animations
typedef enum
{
    ANPC_DEATH,
    ANPC_TAUNT,
    ANPC_ATTACK1,
    ANPC_ATTACK_MELEE,
    ANPC_STANDING,
    ANPC_STANDING_ACTIVE,
    ANPC_WALK,
    ANPC_RUN,
    ANPC_BACKPEDAL,
    ANPC_JUMP,
    ANPC_LAND,
    ANPC_PAIN,
    MAX_ANIMATIONS_NPC
} animNumberNPC_t;

typedef struct animation_s
{
    int		firstFrame;
    int		numFrames;
    int		loopFrames;			// 0 to numFrames
    int		frameLerp;			// msec between frames
    int		initialLerp;		// msec to get to first frame
    int		reversed;			// true if animation is reversed
    int		flipflop;			// true if animation should flipflop back to base
} animation_t;


// flip the togglebit every time an animation
// changes so a restart of the same anim can be detected
#define	ANIM_TOGGLEBIT		256


typedef enum
{
    TEAM_FREE,
    TEAM_RED,
    TEAM_BLUE,
    TEAM_SPECTATOR,

    TEAM_NUM_TEAMS
} team_t;

// Time between location updates
#define TEAM_LOCATION_UPDATE_TIME		1000

// How many players on the overlay
#define TEAM_MAXOVERLAY		32

//team task
typedef enum
{
    TEAMTASK_NONE,
    TEAMTASK_OFFENSE,
    TEAMTASK_DEFENSE,
    TEAMTASK_PATROL,
    TEAMTASK_FOLLOW,
    TEAMTASK_RETRIEVE,
    TEAMTASK_ESCORT,
    TEAMTASK_CAMP
} teamtask_t;

#define LOCATION_NONE		0x00000000

// Height layers
#define LOCATION_HEAD		0x00000001 // [F,B,L,R] Top of head
#define LOCATION_FACE		0x00000002 // [F] Face [B,L,R] Head
#define LOCATION_SHOULDER	0x00000004 // [L,R] Shoulder [F] Throat, [B] Neck
#define LOCATION_CHEST		0x00000008 // [F] Chest [B] Back [L,R] Arm
#define LOCATION_STOMACH	0x00000010 // [L,R] Sides [F] Stomach [B] Lower Back
#define LOCATION_GROIN		0x00000020 // [F] Groin [B] Butt [L,R] Hip
#define LOCATION_LEG		0x00000040 // [F,B,L,R] Legs
#define LOCATION_FOOT		0x00000080 // [F,B,L,R] Bottom of Feet

// Relative direction strike came from
#define LOCATION_LEFT		0x00000100
#define LOCATION_RIGHT		0x00000200
#define LOCATION_FRONT		0x00000400
#define LOCATION_BACK		0x00000800

// means of death
typedef enum
{
    MOD_UNKNOWN,
    MOD_GAUNTLET,
    MOD_MACHINEGUN,
    MOD_GRENADE,
    MOD_GRENADE_SPLASH,
    MOD_ROCKET,
    MOD_ROCKET_SPLASH,
    MOD_PLASMA,
    MOD_PLASMA_SPLASH,
    MOD_RAILGUN,
    MOD_LIGHTNING,
    MOD_LIGHTNING_DISCHARGE,
    MOD_EARTHQUAKE,
    MOD_WATER,
    MOD_SLIME,
    MOD_LAVA,
    MOD_ICE,
    MOD_CRUSH,
    MOD_TELEFRAG,
    MOD_FALLING,
    MOD_SUICIDE,
    MOD_TARGET_LASER,
    MOD_TRIGGER_HURT,
    MOD_EXPLOSIVE,
    //Default attacks (add charged versions)
    MOD_XBUSTER,		//MMX's main cannon
    MOD_XBUSTER2,		//supercharged level 3 version
    MOD_MBUSTER,		//MM's main cannon
    MOD_PBUSTER,		//proto's main cannon
    MOD_FBUSTER,		//Forte's main weapon of destruction
    MOD_ZBUSTER,		//Zero's arm cannon
    //Flame attackes
    MOD_FCUTTER, //charge
    MOD_FCUTTER2, //charged
    MOD_RISFIRE, //drn000
    MOD_HWAVE,	//buster only
    MOD_PSTRIKE,	//rapid
    //Water attacks
    MOD_SICE,	//charge
    MOD_SICE2,	//charged
    MOD_FBURST,	//buster only
    MOD_WSTRIKE,	//rapid
    //Air attacks
    MOD_AFIST,	//charge
    MOD_AFIST2,	//charged
    MOD_AWAVE,	//buster only
    MOD_TEMPEST,	//rapid
    //Earth attacks
    MOD_BCRYSTAL,	//charge
    MOD_BCRYSTAL2,	//charged
    MOD_RSTONE,	//buster only
    MOD_SBURST,	//rapid
    //Metal attacks
    MOD_LTORPEDO,	//charge
    MOD_LTORPEDO2,	//charged
    MOD_BLAUNCHER,	//buster only
    MOD_DSHOT,	//rapid
    //Nature attacks
    MOD_TBURSTER,//charge
    MOD_TBURSTER2,//charged
    MOD_LSTORM,	//buster only
    MOD_ASPLASHER,	//rapid
    //Gravity attacks
    MOD_GBOMB,	//charge
    MOD_GBOMB2,	//charged
    MOD_GRAVWELL,	//drn000
    MOD_MMINE,	//buster only
    MOD_DMATTER,	//rapid
    //Light attacks
    MOD_BLASER,//charge
    MOD_BLASER2,//charged
    MOD_SSHREDDER,	//buster only
    MOD_NBURST,	//rapid
    //Electrical attacks
    MOD_SFIST,	//charge
    MOD_SFIST2,	//charged
    MOD_TRTHUNDER,	//buster only
    MOD_BSHIELD,	//buster only
    MOD_BLIGHTNING,	//rapid
    MOD_GRAPPLE
} meansOfDeath_t;

typedef enum
{
    NPC_ANK,
    NPC_BAT,
    NPC_HULK,
    NPC_METLAR,
    NPC_PILOT,
    NPC_SEALORD,
    NPC_SOLDIER1,
    NPC_SOLDIER2,
    NPC_NUMNPCS
} npcType_t;


typedef struct gnpc_s
{
    char *classname;
    npcType_t npcType;
    int health;
    float painFreq;
    int	walkingSpeed;
    int runningSpeed;
    int fov;
    int jumpHeight;
    int walkingRotSpd;
    int runningRotSpd;
    int melee_dist;
    int melee_damage;
    int far_damage;
    int animTimes[MAX_ANIMATIONS_NPC];
    vec3_t mins,maxs,eye;
    char *precaches;
    char *sounds;
} gnpc_t;

extern gnpc_t bg_npclist[];

//---------------------------------------------------------

// gitem_t->type
typedef enum
{
    IT_BAD,
    IT_WEAPON,				// EFX: rotate + upscale + minlight
    IT_AMMO,				// EFX: rotate
    IT_ARMOR,				// EFX: rotate + minlight
    IT_HEALTH,				// EFX: static external sphere + rotating internal
    IT_POWERUP,				// instant on, timer based
    // EFX: rotate + external ring that rotates
    IT_HOLDABLE,			// single use, holdable item
    // EFX: rotate + bob
    IT_CHIPS,				//EFX: sit there
    IT_PERSISTANT_POWERUP,
    IT_TEAM
} itemType_t;

#define MAX_ITEM_MODELS 4

typedef struct gitem_s
{
    char		*classname;	// spawning name
    char		*pickup_sound;
    char		*world_model[MAX_ITEM_MODELS];

    char		*icon;
    char		*pickup_name;	// for printing on pickup
    char		*wtype_name;

    int			quantity;		// for ammo how much, or duration of powerup
    itemType_t  giType;			// IT_* flags

    int			giTag;

    char		*precaches;		// string of all models and images this item will use
    char		*sounds;		// string of all sounds this item will use

    int			pickup_max;		// max amount of times a player can pick up the item

    float		Dlight;
    vec3_t		DlightRBG;

} gitem_t;

// included in both the game dll and the client
extern	gitem_t	bg_itemlist[];
extern	int		bg_numItems;

gitem_t	*BG_FindItem( const char *pickupName );
gitem_t	*BG_FindItemForWeapon( weapon_t weapon );
gitem_t	*BG_FindItemForPowerup( powerup_t pw );
gitem_t	*BG_FindItemForHoldable( holdable_t pw );
gitem_t	*BG_FindItemForChip( holdable_t pw );
#define	ITEM_INDEX(x) ((x)-bg_itemlist)

qboolean	BG_CanItemBeGrabbed( int gametype, const entityState_t *ent, const playerState_t *ps );


// g_dmflags->integer flags
#define	DF_NO_FALLING			8
#define DF_FIXED_FOV			16
#define	DF_NO_FOOTSTEPS			32

// content masks
#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define	MASK_DEADSOLID			(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE)


//
// entityState_t->eType
//
typedef enum
{
    ET_GENERAL,
    ET_PLAYER,
    ET_ITEM,
    ET_MISSILE,
    ET_MOVER,
    ET_BREAKABLE,
    ET_EXPLOSIVE,
    ET_BEAM,
    ET_PORTAL,
    ET_SPEAKER,
    ET_PUSH_TRIGGER,
    ET_TELEPORT_TRIGGER,
    ET_INVISIBLE,
    ET_GRAPPLE,				// grapple hooked on wall
    ET_TEAM,
    ET_NPC,

    ET_EVENTS				// any of the EV_* events can be added freestanding
    // by setting eType to ET_EVENTS + eventNum
    // this avoids having to set eFlags and eventNum
} entityType_t;



void	BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
void	BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result );

void	BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps );

void	BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad );

void	BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap );
void	BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap );

qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime );


#define ARENAS_PER_TIER		4
#define MAX_ARENAS			1024
#define	MAX_ARENAS_TEXT		8192

#define MAX_BOTS			1024
#define MAX_BOTS_TEXT		8192

//
// screenFlash_t
//
typedef enum
{
    SF_NONE,
    SF_LIGHTNINGSTRIKE,
    SF_CLOAK,
    SF_RAGE,
    SF_PAIN,
    SF_SENSE,
    SF_TRACKING,
    SF_FLASH,
    SF_FROZEN,
    SF_BURNED,
    SF_POISON,
    SF_VIRUS,
    SF_DMG,
    SF_SHOCKED,
    SF_GRAVITY,

    SF_EVENTS_MAX
} screenFlashID_t;

typedef enum
{
    SFT_QUICK,
    SFT_LONG,
    SFT_PERM
} screenFlashTime_t;
