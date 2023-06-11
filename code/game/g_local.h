// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_local.h -- local definitions for game module

#include "../qcommon/q_shared.h"
#include "bg_public.h"
#include "g_public.h"
//#include "../renderergl2/tr_local.h"

//==================================================================

// the "gameversion" client command will print this plus compile date
#define	GAMEVERSION	"xb"

#define BODY_QUEUE_SIZE		8


#define	FRAMETIME			100					// msec
#define	CARNAGE_REWARD_TIME	3000
#define REWARD_SPRITE_TIME	2000

#define	INTERMISSION_DELAY_TIME	1000
#define	SP_INTERMISSION_DELAY_TIME	5000

// gentity->flags
#define	FL_GODMODE				0x00000010
#define	FL_NOTARGET				0x00000020
#define	FL_TEAMSLAVE			0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK			0x00000800
#define FL_DROPPED_ITEM			0x00001000
#define FL_NO_BOTS				0x00002000	// spawn point not for bot use
#define FL_NO_HUMANS			0x00004000	// spawn point just for bots
#define FL_FORCE_GESTURE		0x00008000	// force gesture on client

// movers are things like doors, plats, buttons, etc
typedef enum
{
    MOVER_POS1,
    MOVER_POS2,
    MOVER_1TO2,
    MOVER_2TO1
} moverState_t;

#define SP_PODIUM_MODEL		"models/mapobjects/podium/podium4.md3"

//============================================================================

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

typedef enum
{
    NPC_ST_INACTIVE,
    NPC_ST_ACTIVE,
    NPC_ST_DEAD
} npcState_t;

typedef enum
{
    NPC_COM_NONE,
    NPC_COM_WAIT,
    NPC_COM_GOTO,
    NPC_COM_ATTACK,
    NPC_COM_SEARCH
} npcCommand_t;

typedef struct
{
    playerState_t	ps;
    vec3_t			initpos;
    vec3_t			obj;
    vec3_t			real_obj;
    gentity_t		*enemy;
    npcCommand_t	command;
    npcState_t		state;
    int				flyingAlt;
    int				painAcum;
    int				dontMoveTime;
    float			ideal_angle;
    int				attackTime;
    int				toFire;
    int				fireTime;
    int				meleeTime;
    int				canStand;
    int				checkTime;
    float			shot_factor;
    int				inFlight;
    int				altitude;
    int				goingBack;
    int				fireCount;
} npcData_t;

typedef struct
{
    vec3_t		forward, right, up;
    float		frametime;
    int			msec;
    int			can_do_back_move;
    qboolean	walking;
    qboolean	groundPlane;
    trace_t		groundTrace;
    float		impactSpeed;
    vec3_t		previous_origin;
    vec3_t		previous_velocity;
    int			previous_waterlevel;
} npml_t;

typedef struct
{
    // state (in / out)
    playerState_t	*ps;
    gnpc_t		*npc;
    npcData_t	*ns;
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
    void		(*trace)( trace_t *results, const vec3_t start, const vec3_t mins,
                          const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
    int			(*pointcontents)( const vec3_t point, int passEntityNum );
} npmove_t;

struct gentity_s
{
    entityState_t	s;				// communicated by server to clients
    entityShared_t	r;				// shared by both the server system and game

    // DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
    // EXPECTS THE FIELDS IN THAT ORDER!
    //================================

    struct gclient_s	*client;			// NULL if not a client

    qboolean	inuse;

    char		*classname;			// set in QuakeEd
    int			spawnflags;			// set in QuakeEd

    qboolean	neverFree;			// if true, FreeEntity will only unlink
    // bodyque uses this

    int			flags;				// FL_* variables

    char		*model;
    char		*model2;
    int			freetime;			// level.time when the object was freed

    int			eventTime;			// events will be cleared EVENT_VALID_MSEC after set
    qboolean	freeAfterEvent;
    qboolean	unlinkAfterEvent;

    qboolean	physicsObject;		// if true, it can be pushed by movers and fall off edges
    // all game items are physicsObjects,
    float		physicsBounce;		// 1.0 = continuous bounce, 0.0 = no bounce
    int			clipmask;			// brushes with this content value will be collided against
    // when moving.  items and corpses do not collide against
    // players, for instance

    // movers
    moverState_t moverState;
    int			soundPos1;
    int			sound1to2;
    int			sound2to1;
    int			soundPos2;
    int			soundLoop;
    gentity_t	*parent;
    gentity_t	*nextTrain;
    gentity_t	*prevTrain;
    vec3_t		pos1, pos2;

    char		*message;

    int			timestamp;		// body queue sinking, etc

    float		angle;			// set in editor, -1 = up, -2 = down
    char		*target;
    char		*targetname;
    char		*team;
    char		*targetShaderName;
    char		*targetShaderNewName;
    gentity_t	*target_ent;

    float		speed;
    vec3_t		movedir;

    int			nextthink;
    void		(*think)(gentity_t *self);
    void		(*reached)(gentity_t *self);	// movers call this when hitting endpoint
    void		(*blocked)(gentity_t *self, gentity_t *other);
    void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace);
    void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator);
    void		(*pain)(gentity_t *self, gentity_t *attacker, int damage);
    void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);

    int			pain_debounce_time;
    int			fly_sound_debounce_time;	// wind tunnel
    int			last_move_time;

    int			health;

    qboolean	takedamage;

    int			damage;
    int			splashDamage;	// quad will increase this without increasing radius
    int			splashRadius;
    int			methodOfDeath;
    int			splashMethodOfDeath;

    int			count;

    gentity_t	*chain;
    gentity_t	*enemy;
    gentity_t	*activator;
    gentity_t	*teamchain;		// next entity in team
    gentity_t	*teammaster;	// master of the team

    int			watertype;
    int			waterlevel;

    int			noise_index;

    // timing variables
    float		wait;
    float		random;

    gitem_t		*item;			// for bonus items

    gnpc_t		*npc;
    npcData_t	ns;
    int			slow_event;
    int			stop_event;

    int			pickup_max[MAX_ITEMS];		// max amount of times you can pick up item

    char		*poofin;
    char		*poofout;
    char		*ignore[MAX_CLIENTS];
    int			pchannel;                 /* Private channel */
    gentity_t	*snoop;

    // Peter: currently only used for sp_func_explosion
    int			radius;
    int			respawn;
};


typedef enum
{
    CON_DISCONNECTED,
    CON_CONNECTING,
    CON_CONNECTED
} clientConnected_t;

typedef enum
{
    SPECTATOR_NOT,
    SPECTATOR_FREE,
    SPECTATOR_FOLLOW,
    SPECTATOR_SCOREBOARD
} spectatorState_t;

typedef enum
{
    TEAM_BEGIN,		// Beginning a team game, spawn at base
    TEAM_ACTIVE		// Now actively playing
} playerTeamStateState_t;

typedef struct
{
    playerTeamStateState_t	state;

    int			location;

    int			captures;
    int			basedefense;
    int			carrierdefense;
    int			flagrecovery;
    int			fragcarrier;
    int			assists;

    float		lasthurtcarrier;
    float		lastreturnedflag;
    float		flagsince;
    float		lastfraggedcarrier;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define	FOLLOW_ACTIVE1	-1
#define	FOLLOW_ACTIVE2	-2

// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct
{
    team_t		sessionTeam;
    int			spectatorTime;		// for determining next-in-line to play
    spectatorState_t	spectatorState;
    int			spectatorClient;	// for chasecam and follow mode
    int			wins, losses;		// tournament stats
    qboolean	teamLeader;			// true when this client is a team leader
} clientSession_t;

//
#define MAX_NETNAME			36
#define	MAX_VOTE_COUNT		3

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct
{
    clientConnected_t	connected;
    usercmd_t	cmd;				// we would lose angles if not persistant
    qboolean	localClient;		// true if "ip" info key is "localhost"
    qboolean	initialSpawn;		// the first spawn should be at a cool location
    qboolean	predictItemPickup;	// based on cg_predictItems userinfo
    qboolean	pmoveFixed;			//
    char		netname[MAX_NETNAME];
    int			maxHealth;			// for handicapping
    int			enterTime;			// level.time the client entered the game
    playerTeamState_t teamState;	// status in teamplay games
    int			voteCount;			// to prevent people from constantly calling votes
    int			teamVoteCount;		// to prevent people from constantly calling votes
    qboolean	teamInfo;			// send team overlay updates?
    CLASS_t		playerclass;	    // The players current class
    CLASS_t		newplayerclass;	    // The class the player will become when it respawns
    int			maxEnergy;			//current maximum energy
    int			maxArmor;			//current maximum armor
} clientPersistant_t;


// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s
{
    // ps MUST be the first element, because the server expects it
    playerState_t	ps;				// communicated by server to clients

    // the rest of the structure is private to game
    clientPersistant_t	pers;
    clientSession_t		sess;

    qboolean	readyToExit;		// wishes to leave the intermission

    qboolean	noclip;

    int			lastCmdTime;		// level.time of last usercmd_t, for EF_CONNECTION
    // we can't just use pers.lastCommand.time, because
    // of the g_sycronousclients case
    int			buttons;
    int			oldbuttons;
    int			latched_buttons;

    vec3_t		oldOrigin;

    // sum up damage over an entire frame
    int			damage_armor;		// damage absorbed by armor
    int			damage_blood;		// damage taken out of health
    int			damage_knockback;	// impact damage
    vec3_t		damage_from;		// origin for vector calculation
    qboolean	damage_fromWorld;	// if true, don't use the damage_from vector

    int			accurateCount;		// for "impressive" reward sound

    int			accuracy_shots;		// total number of shots
    int			accuracy_hits;		// total number of hits

    //
    int			lastkilled_client;	// last client that this client killed
    int			lasthurt_client;	// last client that damaged this client
    int			lasthurt_mod;		// type of damage the client did
    int			lasthurt_location;	// Where the client was hit.

    // timers
    int			respawnTime;		// can respawn when time > this, force after g_forcerespwan
    int			inactivityTime;		// kick players when time > this
    qboolean	inactivityWarning;	// qtrue if the five seoond warning has been given

    int			airOutTime;

    int			lastKillTime;		// for multiple kill rewards

    int			freeze_time;		// what time the player was frozen
    int			slow_time;			// what time the player was slowed down
    int			shock_time;			// what time the player was shocked
    int			lstruck_time;		// what time the player was struck by lightning
    int			lag_time;			// what time the player was lagged
    int			phase_time;			// what time the player phases in or out

    qboolean	fireHeld;			// used for hook
    gentity_t	*hook;				// grapple hook if out

    int			switchTeamTime;		// time the player switched teams

    // timeResidual is used to handle events that happen every second
    // like health / armor countdowns and regeneration
    int			timeResidual;

    int			dashTime;			//used to handle when the next dash can occur
    qboolean	dashStop;			// should the dash_stop sfx be played?
    int			chargeStart;		//level.time you pressed the charge button
    int			menuTime;		//level.time that you can go into a capsule again (basically its just enough to make sure you get only one choice)


    char		*areabits;
};


//
// this structure is cleared as each map is entered
//
#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	4096

typedef struct
{
    struct gclient_s	*clients;		// [maxclients]

    struct gentity_s	*gentities;
    int			gentitySize;
    int			num_entities;		// current number, <= MAX_GENTITIES

    int			warmupTime;			// restart match at this time

    fileHandle_t	logFile;

    // store latched cvars here that we want to get at often
    int			maxclients;

    int			framenum;
    int			time;					// in msec
    int			previousTime;			// so movers can back up when blocked

    int			startTime;				// level.time the map was started

    int			teamScores[TEAM_NUM_TEAMS];
    int			lastTeamLocationTime;		// last time of client team location update

    qboolean	newSession;				// don't use any old session data, because
    // we changed gametype

    qboolean	restarted;				// waiting for a map_restart to fire

    int			numConnectedClients;
    int			numNonSpectatorClients;	// includes connecting clients
    int			numPlayingClients;		// connected, non-spectators
    int			sortedClients[MAX_CLIENTS];		// sorted by score
    int			follow1, follow2;		// clientNums for auto-follow spectators

    int			snd_fry;				// sound index for standing in lava

    int			warmupModificationCount;	// for detecting if g_warmup is changed

    // voting state
    char		voteString[MAX_STRING_CHARS];
    char		voteDisplayString[MAX_STRING_CHARS];
    int			voteTime;				// level.time vote was called
    int			voteExecuteTime;		// time the vote is executed
    int			voteYes;
    int			voteNo;
    int			numVotingClients;		// set by CalculateRanks

    // team voting state
    char		teamVoteString[2][MAX_STRING_CHARS];
    int			teamVoteTime[2];		// level.time vote was called
    int			teamVoteYes[2];
    int			teamVoteNo[2];
    int			numteamVotingClients[2];// set by CalculateRanks

    // spawn variables
    qboolean	spawning;				// the G_Spawn*() functions are valid
    int			numSpawnVars;
    char		*spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
    int			numSpawnVarChars;
    char		spawnVarChars[MAX_SPAWN_VARS_CHARS];

    // intermission state
    int			intermissionQueued;		// intermission was qualified, but
    // wait INTERMISSION_DELAY_TIME before
    // actually going there so the last
    // frag can be watched.  Disable future
    // kills during this delay
    int			intermissiontime;		// time the intermission was started
    char		*changemap;
    qboolean	readyToExit;			// at least one client wants to exit
    int			exitTime;
    vec3_t		intermission_origin;	// also used for spectator spawns
    vec3_t		intermission_angle;

    qboolean	locationLinked;			// target_locations get linked
    gentity_t	*locationHead;			// head of the location list
    int			bodyQueIndex;			// dead bodies
    gentity_t	*bodyQue[BODY_QUEUE_SIZE];

    // time
    int hours;
    int day;
    int month;
    int year;

    // enviromental
    int	pressure;	/* How is the pressure ( Mb ) */
    int	change;	    /* How fast and what way does it change. */
    int	sky;	    /* How is the sky. */

    int	nextQuake;
    int nextLightning;

} level_locals_t;

struct command_info
{
    const char *command;
    void	(*command_pointer)
    (gentity_t *ent, char *argument, int cmd, int subcmd);
    qboolean  cheat_command;
    int	subcmd;
};

extern const struct command_info cmd_info[];

/* Timers */
#define PULSE_WEATHER       450

/* Sky conditions for weather_data */
#define SKY_CLOUDLESS		0
#define SKY_CLOUDY			1
#define SKY_RAINING			2
#define SKY_SNOWING			3
#define SKY_LIGHTNING		4

/* private channel */
#define PRIVATE_HELP    0
#define PRIVATE_OPEN    1
#define PRIVATE_CLOSE   2
#define PRIVATE_ADD     3
#define PRIVATE_REMOVE  4
#define PRIVATE_WHO     5
#define PRIVATE_CHECK   6

/* ai_main.c */
#define MAX_FILEPATH			144
int NumBots(void);

/* channels.c */
void QDECL send_to_char( gentity_t *ent, const char *fmt, ... );
void QDECL send_to_room( gentity_t *ent, const char *fmt, ... );
void do_say(gentity_t *ent, char *argument, int cmd, int subcmd);
void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText );
void do_tell(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_private_channel(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_ignore(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_unignore(gentity_t *ent, char *argument, int cmd, int subcmd);

/* cheat.c */
void do_disconnect(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_echo(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_escape(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_force(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_freeze(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_give(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_goto(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_invis(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_lag(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_levelshot(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_noclip(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_notarget(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_poofin(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_poofout(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_recho(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_restore(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_setviewpos(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_slay(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_transfer(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_voice(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_where(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_world(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_wset(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_snoop(gentity_t *ent, char *argument, int cmd, int subcmd);

//
// g_spawn.c
//
qboolean	G_SpawnString( const char *key, const char *defaultString, char **out );
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean	G_SpawnFloat( const char *key, const char *defaultString, float *out );
qboolean	G_SpawnInt( const char *key, const char *defaultString, int *out );
qboolean	G_SpawnVector( const char *key, const char *defaultString, float *out );
void		G_SpawnEntitiesFromString( void );
char *G_NewString( const char *string );

//
// interp.c
//
void do_commands(gentity_t *ent, char *argument, int cmd, int subcmd);
void StopFollowing( gentity_t *ent );
void BroadcastTeamChange( gclient_t *client, int oldTeam );
void SetTeam( gentity_t *ent, char *s );
void Cmd_FollowCycle_f( gentity_t *ent, int dir );
void do_callvote(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_callteamvote(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_team(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_teamtask(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_teamvote(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_vote(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_score(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_follow(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_followcycle(gentity_t *ent, char *argument, int cmd, int subcmd);


//
// g_items.c
//
void G_CheckTeamItems( void );
void G_RunItem( gentity_t *ent );
void RespawnItem( gentity_t *ent );

void UseHoldableItem( gentity_t *ent );
void PrecacheItem (gitem_t *it);
gentity_t *Drop_Item( gentity_t *ent, gitem_t *item, float angle );
gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity );
void SetRespawn (gentity_t *ent, float delay);
void G_SpawnItem (gentity_t *ent, gitem_t *item);
void FinishSpawningItem( gentity_t *ent );
void Think_Weapon (gentity_t *ent);
int ArmorIndex (gentity_t *ent);
void	Add_Ammo (gentity_t *ent, int weapon, int count);
void Touch_Item (gentity_t *ent, gentity_t *other, trace_t *trace);

void ClearRegisteredItems( void );
void RegisterItem( gitem_t *item );
void SaveRegisteredItems( void );

//
// g_utils.c
//
int G_ModelIndex( char *name );
int		G_SoundIndex( char *name );
void	G_TeamCommand( team_t team, char *cmd );
void	G_KillBox (gentity_t *ent);
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match);
gentity_t *G_PickTarget (char *targetname);
void	G_UseTargets (gentity_t *ent, gentity_t *activator);
void	G_SetMovedir ( vec3_t angles, vec3_t movedir);
void	G_InitGentity( gentity_t *e );
gentity_t	*G_Spawn (void);
gentity_t *G_TempEntity( vec3_t origin, int event );
void	G_Sound( gentity_t *ent, int channel, int soundIndex );
void	G_FreeEntity( gentity_t *e );
qboolean	G_EntitiesFree( void );

void	G_TouchTriggers (gentity_t *ent);
void	G_TouchSolids (gentity_t *ent);

float	*tv (float x, float y, float z);
char	*vtos( const vec3_t v );

float vectoyaw( const vec3_t vec );

void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_SetOrigin( gentity_t *ent, vec3_t origin );
void AddRemap(const char *oldShader, const char *newShader, float timeOffset);
const char *BuildShaderStateConfig(void);
gentity_t *findradius (gentity_t *ent, vec3_t org, float rad);
qboolean visible( gentity_t *ent1, gentity_t *ent2 );
int in_same_room(gentity_t *ent, gentity_t *other);
char	*ConcatArgs( int start );
void SanitizeString( char *in, char *out );
int ClientNumberFromString( gentity_t *to, char *s );
void G_ProjectSource ( vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result );
gentity_t *get_char_world(char *name);
void G_GlobalSound( char *argument );
gentity_t *get_char_room(gentity_t *ent, char *name);

/* combat.c */
qboolean CanDamage (gentity_t *targ, vec3_t origin);
void G_Damage (gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod);
qboolean G_RadiusDamage (vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod);
int G_InvulnerabilityEffect( gentity_t *targ, vec3_t dir, vec3_t point, vec3_t impactpoint, vec3_t bouncedir );
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
void TossClientItems( gentity_t *self );
void TossClientCubes( gentity_t *self );
qboolean G_WaterRadiusDamage (vec3_t origin, gentity_t *attacker, float damage, float radius,
                              gentity_t *ignore, int mod);
void do_drag(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_shove(gentity_t *ent, char *argument, int cmd, int subcmd);
void do_kill(gentity_t *ent, char *argument, int cmd, int subcmd);

// damage flags
#define DAMAGE_RADIUS				0x00000001	// damage was indirect
#define DAMAGE_NO_ARMOR				0x00000002	// armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK			0x00000004	// do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000008  // armor, shields, invulnerability, and godmode have no effect

//
// g_missile.c
//
void G_RunMissile( gentity_t *ent );
void G_ExplodeMissile( gentity_t *ent );
void G_GuideMissile( gentity_t *ent );
void G_SplitMissile( gentity_t *ent );
void G_HomeMissile( gentity_t *self );
void G_HomeMissile2( gentity_t *self );
void G_Suck( gentity_t *self );
void G_Strike( gentity_t *self );
typedef gentity_t* (*weaponLaunch)(gentity_t*, vec3_t, vec3_t);
void G_SpreadProjectile( gentity_t *ent, weaponLaunch fireFunc );


gentity_t *fire_blauncher (gentity_t *self, vec3_t start, vec3_t aimdir);
gentity_t *fire_xbuster (gentity_t *self, vec3_t start, vec3_t aimdir);
gentity_t *fire_ltorpedo (gentity_t *self, vec3_t start, vec3_t aimdir);
gentity_t *fire_grapple (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_ank_shot (gentity_t *self, vec3_t start, vec3_t dir,gentity_t *enemy);
gentity_t *fire_bat_shot (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_sealord_big_shot (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_sealord_small_shot (gentity_t *self, vec3_t start, vec3_t dir);

void Weapon_XBuster_Fire (gentity_t *ent);
void Weapon_MBuster_Fire (gentity_t *ent);
void Weapon_PBuster_Fire (gentity_t *ent);
void Weapon_FBuster_Fire (gentity_t *ent);
void Weapon_ZBuster_Fire (gentity_t *ent);
gentity_t *fire_fbuster (gentity_t *self, vec3_t start, vec3_t dir);
void Weapon_IceBurst_Fire (gentity_t *ent);
//fire
void Weapon_FCutter_Fire (gentity_t *ent);
void Weapon_RisFire_Fire (gentity_t *ent);
void Weapon_HWave_Fire (gentity_t *ent);
void Weapon_PStrike_Fire (gentity_t *ent);
gentity_t *fire_hwave2 (gentity_t *self, vec3_t start, vec3_t dir, int number );
gentity_t *fire_pstrike (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_pstrike2 (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_risfire (gentity_t *self, vec3_t start, vec3_t dir);
//water
void Weapon_SIce_Fire (gentity_t *ent);
void Weapon_FBurst_Fire (gentity_t *ent);
void Weapon_WStrike_Fire (gentity_t *ent);
gentity_t *fire_sice2 (gentity_t *self, vec3_t start, vec3_t dir);

//gentity_t *fire_wstrike (gentity_t *self, vec3_t start, vec3_t dir);
//air
void Weapon_AFist_Fire (gentity_t *ent);
void Weapon_AWave_Fire (gentity_t *ent);
void Weapon_Tempest_Fire (gentity_t *ent);
gentity_t *fire_tempest (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_afist ( gentity_t *self, vec3_t start, vec3_t dir, float speed );
//earth
void Weapon_BCrystal_Fire (gentity_t *ent);
void Weapon_RStone_Fire (gentity_t *ent);
void Weapon_SBurst_Fire (gentity_t *ent);
gentity_t *fire_sburst (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_bcrystal3 (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_bcrystal4 (gentity_t *self, vec3_t start, vec3_t dir );
//tech
void Weapon_LTorpedo_Fire (gentity_t *ent);
void Weapon_BLauncher_Fire (gentity_t *ent);
void Weapon_DShot_Fire (gentity_t *ent);
gentity_t *fire_ltorpedo3 (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_dshot (gentity_t *self, vec3_t start, vec3_t dir, float speed );
void Weapon_IMBoomerang_Fire ( gentity_t *ent );
//nature
void Weapon_TBurster_Fire (gentity_t *ent);
void Weapon_LStorm_Fire (gentity_t *ent);
void Weapon_ASplasher_Fire (gentity_t *ent);
gentity_t *fire_tburster2 (gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_asplasher2 (gentity_t *self, vec3_t start, vec3_t dir );
//gravity
void Weapon_GBomb_Fire (gentity_t *ent);
void Weapon_GravWell_Fire (gentity_t *ent);
void Weapon_MMine_Fire (gentity_t *ent);
void Weapon_DMatter_Fire (gentity_t *ent);
gentity_t *fire_dmatter (gentity_t *self, vec3_t start, vec3_t dir );
//light
void Weapon_BLaser_Fire (gentity_t *ent);
void Weapon_SShredder_Fire (gentity_t *ent);
void Weapon_NBurst_Fire (gentity_t *ent);
gentity_t *fire_nburst (gentity_t *self, vec3_t start, vec3_t dir );
//electrical
void Weapon_SFist_Fire (gentity_t *ent);
void Weapon_BShield_Fire (gentity_t *ent);
void Weapon_BLightning_Fire (gentity_t *ent);
void Weapon_TriadThunder_Fire (gentity_t *ent);
gentity_t *fire_blightning (gentity_t *self, vec3_t start, vec3_t dir );
//tech moves
void Tech_Uppercut_Fire( gentity_t *ent );
void Tech_Spike_Fire( gentity_t *ent );
void Tech_Spin_Fire( gentity_t *ent );
//
// g_mover.c
//
void G_RunMover( gentity_t *ent );
void Touch_DoorTrigger( gentity_t *ent, gentity_t *other, trace_t *trace );

//
// g_trigger.c
//
void trigger_teleporter_touch (gentity_t *self, gentity_t *other, trace_t *trace );


//
// g_misc.c
//
void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles );
void G_BreakGlass( gentity_t *ent, vec3_t point, int mod );
void G_RunMD3Anim( gentity_t *ent ) ;


//
// g_weapon.c
//
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker );
void CalcMuzzlePoint ( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
void SnapVectorTowards( vec3_t v, vec3_t to );
qboolean CheckGauntletAttack( gentity_t *ent );
void Weapon_HookFree (gentity_t *ent);
void Weapon_HookThink (gentity_t *ent);
void NPC_FireWeapon(gentity_t *ent,vec3_t angle);

//
// g_client.c
//
team_t TeamCount( int ignoreClientNum, int team );
int TeamLeader( int team );
team_t PickTeam( int ignoreClientNum );
void SetClientViewAngle( gentity_t *ent, vec3_t angle );
gentity_t *SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles );
void CopyToBodyQue( gentity_t *ent );
void respawn (gentity_t *ent);
void BeginIntermission (void);
void InitClientPersistant (gclient_t *client);
void InitClientResp (gclient_t *client);
void InitBodyQue (void);
void ClientSpawn( gentity_t *ent );
void player_die (gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);
void AddScore( gentity_t *ent, vec3_t origin, int score );
void CalculateRanks( void );
qboolean SpotWouldTelefrag( gentity_t *spot );
int num_players(void);

//
// g_svcmds.c
//
qboolean	ConsoleCommand( void );
void G_ProcessIPBans(void);
qboolean G_FilterPacket (char *from);

//
// g_weapon.c
//
void FireWeapon( gentity_t *ent );
void FireOffhand( gentity_t *ent );

//
// p_hud.c
//
void MoveClientToIntermission (gentity_t *client);
void G_SetStats (gentity_t *ent);
void DeathmatchScoreboardMessage (gentity_t *client);

//
// g_npcs.c
//
extern int npc_skill;
void ClearRegisteredNPCs( void );
void RegisterNPC( gnpc_t *npc );
void SaveRegisteredNPCs( void );
void G_RunNPC( gentity_t *ent );
//
// g_npcmove.c
//
void NPC_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap );
void NPM_StartLegsAnim(int anim);
void NPM_ForceLegsAnim(int anim);
void NPM_ContinueLegsAnim(int anim);
void NPmove( npmove_t *pmove );
//
// g_npcthink.c
//
void NPC_ThinkMove(gentity_t *ent,usercmd_t *ucmd);
void NPC_ThinkView(gentity_t *ent,usercmd_t *ucmd);
void NPC_FindTarget(gentity_t *ent);
int NPC_RecalcLinearObj(gentity_t *ent);
qboolean NPC_InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles);
int NPC_IsVisible(gentity_t *viewer,gentity_t *ent);

//
// g_pweapon.c
//


//
// g_main.c
//
void FindIntermissionPoint( void );
void SetLeader(int team, int client);
void CheckTeamLeader( int team );
void G_RunThink (gentity_t *ent);
void QDECL G_LogPrintf( const char *fmt, ... );
void SendScoreboardMessageToAllClients( void );
void QDECL G_Printf( const char *fmt, ... );
void QDECL G_Error( const char *fmt, ... );

//
// g_client.c
//
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot );
void ClientUserinfoChanged( int clientNum );
void ClientDisconnect( int clientNum );
void ClientBegin( int clientNum );
void ClientCommand( int clientNum );

//
// g_active.c
//
void ClientThink( int clientNum );
void ClientEndFrame( gentity_t *ent );
void G_RunClient( gentity_t *ent );

//
// g_team.c
//
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );
void Team_CheckDroppedItem( gentity_t *dropped );

//
// g_mem.c
//
void *G_Alloc( int size );
void G_InitMemory( void );
void Svcmd_GameMem_f( void );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo );

void G_InitWorldSession( void );
void G_WriteSessionData( void );

//
// g_arenas.c
//
void UpdateTournamentInfo( void );
void SpawnModelsOnVictoryPads( void );
void Svcmd_AbortPodium_f( void );

//
// g_bot.c
//
void G_InitBots( qboolean restart );
char *G_GetBotInfoByNumber( int num );
char *G_GetBotInfoByName( const char *name );
void G_CheckBotSpawn( void );
void G_RemoveQueuedBotBegin( int clientNum );
qboolean G_BotConnect( int clientNum, qboolean restart );
void Svcmd_AddBot_f( void );
void Svcmd_BotList_f( void );
void BotInterbreedEndMatch( void );

//
// upgrades.c
void do_upgrade(gentity_t *ent, char *name, int cmd, int subcmd);
void do_downgrade(gentity_t *ent, char *name, int cmd, int subcmd);
void do_cloak(gentity_t *ent, char *argument, int cmd, int subcmd);

//
// weather.c
//
void weather_and_time(void);
void lightning_strike(void);
void earthquake(void);

/* webserver.c */
void init_web(int port);
void shutdown_web (void);
void handle_web(void);

//bot settings
typedef struct bot_settings_s
{
    char characterfile[MAX_FILEPATH];
    float skill;
    char team[MAX_FILEPATH];
} bot_settings_t;

int BotAISetup( int restart );
int BotAIShutdown( int restart );
int BotAILoadMap( int restart );
int BotAISetupClient(int client, struct bot_settings_s *settings, qboolean restart);
int BotAIShutdownClient( int client, qboolean restart );
int BotAIStartFrame( int time );
void BotTestAAS(vec3_t origin);

#include "g_team.h" // teamplay specific stuff


extern	level_locals_t	level;
extern	gentity_t		g_entities[MAX_GENTITIES];

#define	FOFS(x) ((int)&(((gentity_t *)0)->x))
#define NPCOFS(x) ((int)&(((gnpc_t *)0)->x))

extern	vmCvar_t	g_gametype;
extern	vmCvar_t	g_dedicated;
extern	vmCvar_t	g_autoskin;
extern	vmCvar_t	g_defaultUpgrades;
extern	vmCvar_t	g_cheats;
extern	vmCvar_t	g_maxclients;			// allow this many total, including spectators
extern	vmCvar_t	g_maxGameClients;		// allow this many active
extern	vmCvar_t	g_restarted;

extern	vmCvar_t	g_dmflags;
extern	vmCvar_t	g_fraglimit;
extern	vmCvar_t	g_timelimit;
extern	vmCvar_t	g_capturelimit;
extern	vmCvar_t	g_friendlyFire;
extern	vmCvar_t	g_password;
extern	vmCvar_t	g_needpass;
extern	vmCvar_t	g_gravity;
extern	vmCvar_t	g_speed;
extern	vmCvar_t	g_knockback;
extern	vmCvar_t	g_quadfactor;
extern	vmCvar_t	g_forcerespawn;
extern	vmCvar_t	g_inactivity;
extern	vmCvar_t	g_debugMove;
extern	vmCvar_t	g_debugAlloc;
extern	vmCvar_t	g_debugDamage;
extern	vmCvar_t	g_weaponRespawn;
extern	vmCvar_t	g_weaponTeamRespawn;
extern	vmCvar_t	g_synchronousClients;
extern	vmCvar_t	g_motd;
extern	vmCvar_t	g_warmup;
extern	vmCvar_t	g_doWarmup;
extern	vmCvar_t	g_allowVote;
extern	vmCvar_t	g_teamAutoJoin;
extern	vmCvar_t	g_teamForceBalance;
extern	vmCvar_t	g_banIPs;
extern	vmCvar_t	g_filterBan;
extern	vmCvar_t	g_redteam;
extern	vmCvar_t	g_blueteam;
extern	vmCvar_t	g_smoothClients;
extern	vmCvar_t	pmove_fixed;
extern	vmCvar_t	pmove_msec;
extern	vmCvar_t	g_rankings;
extern	vmCvar_t	g_enableDust;
extern	vmCvar_t	g_enableBreath;
extern	vmCvar_t	g_singlePlayer;
extern	vmCvar_t	g_proxMineTimeout;
extern	vmCvar_t	s_botsoff;

void	trap_Printf( const char *fmt );
void	trap_Error( const char *fmt );
int		trap_Milliseconds( void );
int		trap_Argc( void );
void	trap_Argv( int n, char *buffer, int bufferLength );
void	trap_Args( char *buffer, int bufferLength );
int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void	trap_FS_Read( void *buffer, int len, fileHandle_t f );
void	trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void	trap_FS_FCloseFile( fileHandle_t f );
int		trap_FS_GetFileList( const char *path, const char *extension, char *listbuf, int bufsize );
int		trap_FS_Seek( fileHandle_t f, long offset, int origin ); // fsOrigin_t
void	trap_SendConsoleCommand( int exec_when, const char *text );
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags );
void	trap_Cvar_Update( vmCvar_t *cvar );
void	trap_Cvar_Set( const char *var_name, const char *value );
int		trap_Cvar_VariableIntegerValue( const char *var_name );
float	trap_Cvar_VariableValue( const char *var_name );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *gameClients, int sizeofGameClient );
void	trap_DropClient( int clientNum, const char *reason );
void	trap_SendServerCommand( int clientNum, const char *text );
void	trap_SetConfigstring( int num, const char *string );
void	trap_GetConfigstring( int num, char *buffer, int bufferSize );
void	trap_GetUserinfo( int num, char *buffer, int bufferSize );
void	trap_SetUserinfo( int num, const char *buffer );
void	trap_GetServerinfo( char *buffer, int bufferSize );
void	trap_SetBrushModel( gentity_t *ent, const char *name );
void	trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
int		trap_PointContents( const vec3_t point, int passEntityNum );
qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 );
qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void	trap_AdjustAreaPortalState( gentity_t *ent, qboolean open );
qboolean trap_AreasConnected( int area1, int area2 );
void	trap_LinkEntity( gentity_t *ent );
void	trap_UnlinkEntity( gentity_t *ent );
int		trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
int		trap_BotAllocateClient( void );
void	trap_BotFreeClient( int clientNum );
void	trap_GetUsercmd( int clientNum, usercmd_t *cmd );
qboolean	trap_GetEntityToken( char *buffer, int bufferSize );

int		trap_DebugPolygonCreate(int color, int numPoints, vec3_t *points);
void	trap_DebugPolygonDelete(int id);

int		trap_BotLibSetup( void );
int		trap_BotLibShutdown( void );
int		trap_BotLibVarSet(char *var_name, char *value);
int		trap_BotLibVarGet(char *var_name, char *value, int size);
int		trap_BotLibDefine(char *string);
int		trap_BotLibStartFrame(float time);
int		trap_BotLibLoadMap(const char *mapname);
int		trap_BotLibUpdateEntity(int ent, void /* struct bot_updateentity_s */ *bue);
int		trap_BotLibTest(int parm0, char *parm1, vec3_t parm2, vec3_t parm3);

int		trap_BotGetSnapshotEntity( int clientNum, int sequence );
int		trap_BotGetServerCommand(int clientNum, char *message, int size);
void	trap_BotUserCommand(int client, usercmd_t *ucmd);

int		trap_AAS_BBoxAreas(vec3_t absmins, vec3_t absmaxs, int *areas, int maxareas);
int		trap_AAS_AreaInfo( int areanum, void /* struct aas_areainfo_s */ *info );
void	trap_AAS_EntityInfo(int entnum, void /* struct aas_entityinfo_s */ *info);

int		trap_AAS_Initialized(void);
void	trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);
float	trap_AAS_Time(void);

int		trap_AAS_PointAreaNum(vec3_t point);
int		trap_AAS_PointReachabilityAreaIndex(vec3_t point);
int		trap_AAS_TraceAreas(vec3_t start, vec3_t end, int *areas, vec3_t *points, int maxareas);

int		trap_AAS_PointContents(vec3_t point);
int		trap_AAS_NextBSPEntity(int ent);
int		trap_AAS_ValueForBSPEpairKey(int ent, char *key, char *value, int size);
int		trap_AAS_VectorForBSPEpairKey(int ent, char *key, vec3_t v);
int		trap_AAS_FloatForBSPEpairKey(int ent, char *key, float *value);
int		trap_AAS_IntForBSPEpairKey(int ent, char *key, int *value);

int		trap_AAS_AreaReachability(int areanum);

int		trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags);
int		trap_AAS_EnableRoutingArea( int areanum, int enable );
int		trap_AAS_PredictRoute(void /*struct aas_predictroute_s*/ *route, int areanum, vec3_t origin,
                              int goalareanum, int travelflags, int maxareas, int maxtime,
                              int stopevent, int stopcontents, int stoptfl, int stopareanum);

int		trap_AAS_AlternativeRouteGoals(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags,
                                       void /*struct aas_altroutegoal_s*/ *altroutegoals, int maxaltroutegoals,
                                       int type);
int		trap_AAS_Swimming(vec3_t origin);
int		trap_AAS_PredictClientMovement(void /* aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);


void	trap_EA_Say(int client, char *str);
void	trap_EA_SayTeam(int client, char *str);
void	trap_EA_Command(int client, char *command);

void	trap_EA_Action(int client, int action);
void	trap_EA_Gesture(int client);
void	trap_EA_Talk(int client);
void	trap_EA_Attack(int client);
void	trap_EA_Use(int client);
void	trap_EA_Respawn(int client);
void	trap_EA_Crouch(int client);
void	trap_EA_MoveUp(int client);
void	trap_EA_MoveDown(int client);
void	trap_EA_MoveForward(int client);
void	trap_EA_MoveBack(int client);
void	trap_EA_MoveLeft(int client);
void	trap_EA_MoveRight(int client);
void	trap_EA_SelectWeapon(int client, int weapon);
void	trap_EA_Jump(int client);
void	trap_EA_DelayedJump(int client);
void	trap_EA_Move(int client, vec3_t dir, float speed);
void	trap_EA_View(int client, vec3_t viewangles);

void	trap_EA_EndRegular(int client, float thinktime);
void	trap_EA_GetInput(int client, float thinktime, void /* struct bot_input_s */ *input);
void	trap_EA_ResetInput(int client);


int		trap_BotLoadCharacter(char *charfile, float skill);
void	trap_BotFreeCharacter(int character);
float	trap_Characteristic_Float(int character, int index);
float	trap_Characteristic_BFloat(int character, int index, float min, float max);
int		trap_Characteristic_Integer(int character, int index);
int		trap_Characteristic_BInteger(int character, int index, int min, int max);
void	trap_Characteristic_String(int character, int index, char *buf, int size);

int		trap_BotAllocChatState(void);
void	trap_BotFreeChatState(int handle);
void	trap_BotQueueConsoleMessage(int chatstate, int type, char *message);
void	trap_BotRemoveConsoleMessage(int chatstate, int handle);
int		trap_BotNextConsoleMessage(int chatstate, void /* struct bot_consolemessage_s */ *cm);
int		trap_BotNumConsoleMessages(int chatstate);
void	trap_BotInitialChat(int chatstate, char *type, int mcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int		trap_BotNumInitialChats(int chatstate, char *type);
int		trap_BotReplyChat(int chatstate, char *message, int mcontext, int vcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int		trap_BotChatLength(int chatstate);
void	trap_BotEnterChat(int chatstate, int client, int sendto);
void	trap_BotGetChatMessage(int chatstate, char *buf, int size);
int		trap_StringContains(char *str1, char *str2, int casesensitive);
int		trap_BotFindMatch(char *str, void /* struct bot_match_s */ *match, unsigned long int context);
void	trap_BotMatchVariable(void /* struct bot_match_s */ *match, int variable, char *buf, int size);
void	trap_UnifyWhiteSpaces(char *string);
void	trap_BotReplaceSynonyms(char *string, unsigned long int context);
int		trap_BotLoadChatFile(int chatstate, char *chatfile, char *chatname);
void	trap_BotSetChatGender(int chatstate, int gender);
void	trap_BotSetChatName(int chatstate, char *name, int client);
void	trap_BotResetGoalState(int goalstate);
void	trap_BotRemoveFromAvoidGoals(int goalstate, int number);
void	trap_BotResetAvoidGoals(int goalstate);
void	trap_BotPushGoal(int goalstate, void /* struct bot_goal_s */ *goal);
void	trap_BotPopGoal(int goalstate);
void	trap_BotEmptyGoalStack(int goalstate);
void	trap_BotDumpAvoidGoals(int goalstate);
void	trap_BotDumpGoalStack(int goalstate);
void	trap_BotGoalName(int number, char *name, int size);
int		trap_BotGetTopGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int		trap_BotGetSecondGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int		trap_BotChooseLTGItem(int goalstate, vec3_t origin, int *inventory, int travelflags);
int		trap_BotChooseNBGItem(int goalstate, vec3_t origin, int *inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime);
int		trap_BotTouchingGoal(vec3_t origin, void /* struct bot_goal_s */ *goal);
int		trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal);
int		trap_BotGetNextCampSpotGoal(int num, void /* struct bot_goal_s */ *goal);
int		trap_BotGetMapLocationGoal(char *name, void /* struct bot_goal_s */ *goal);
int		trap_BotGetLevelItemGoal(int index, char *classname, void /* struct bot_goal_s */ *goal);
float	trap_BotAvoidGoalTime(int goalstate, int number);
void	trap_BotSetAvoidGoalTime(int goalstate, int number, float avoidtime);
void	trap_BotInitLevelItems(void);
void	trap_BotUpdateEntityItems(void);
int		trap_BotLoadItemWeights(int goalstate, char *filename);
void	trap_BotFreeItemWeights(int goalstate);
void	trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);
void	trap_BotSaveGoalFuzzyLogic(int goalstate, char *filename);
void	trap_BotMutateGoalFuzzyLogic(int goalstate, float range);
int		trap_BotAllocGoalState(int state);
void	trap_BotFreeGoalState(int handle);

void	trap_BotResetMoveState(int movestate);
void	trap_BotMoveToGoal(void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags);
int		trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
void	trap_BotResetAvoidReach(int movestate);
void	trap_BotResetLastAvoidReach(int movestate);
int		trap_BotReachabilityArea(vec3_t origin, int testground);
int		trap_BotMovementViewTarget(int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target);
int		trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target);
int		trap_BotAllocMoveState(void);
void	trap_BotFreeMoveState(int handle);
void	trap_BotInitMoveState(int handle, void /* struct bot_initmove_s */ *initmove);
void	trap_BotAddAvoidSpot(int movestate, vec3_t origin, float radius, int type);

int		trap_BotChooseBestFightWeapon(int weaponstate, int *inventory);
void	trap_BotGetWeaponInfo(int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo);
int		trap_BotLoadWeaponWeights(int weaponstate, char *filename);
int		trap_BotAllocWeaponState(void);
void	trap_BotFreeWeaponState(int weaponstate);
void	trap_BotResetWeaponState(int weaponstate);

int		trap_GeneticParentsAndChildSelection(int numranks, float *ranks, int *parent1, int *parent2, int *child);

void	trap_SnapVector( float *v );
