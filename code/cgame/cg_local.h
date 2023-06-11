// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "../qcommon/q_shared.h"
#include "../renderercommon/tr_types.h"
#include "../game/bg_public.h"
#include "cg_public.h"
#include "../ui/ui_public.h"

// The entire cgame module is unloaded and reloaded on each level change,
// so there is NO persistant data between levels on the client side.
// If you absolutely need something stored, it can either be kept
// by the server in the server stored userinfos, or stashed in a cvar.


#define	POWERUP_BLINKS		5

#define	POWERUP_BLINK_TIME	1000
#define	FADE_TIME			200
#define	PULSE_TIME			200
#define	DAMAGE_DEFLECT_TIME	100
#define	DAMAGE_RETURN_TIME	400
#define DAMAGE_TIME			500
#define	LAND_DEFLECT_TIME	150
#define	LAND_RETURN_TIME	300
#define	STEP_TIME			200
#define	DUCK_TIME			100
#define	PAIN_TWITCH_TIME	200
#define	WEAPON_SELECT_TIME	1400
#define	ITEM_SCALEUP_TIME	1000
#define	ZOOM_TIME			150
#define	ITEM_BLOB_TIME		200
#define	MUZZLE_FLASH_TIME	300
#define	SINK_TIME			1000		// time for fragments to sink into ground before going away
#define	ATTACKER_HEAD_TIME	10000
#define	REWARD_TIME			3000

#define	PULSE_SCALE			2.5			// amount to scale up the icons when activating

#define	MAX_STEP_CHANGE		32

#define	MAX_VERTS_ON_POLY	10
#define	MAX_MARK_POLYS		256

#define STAT_MINUS			10	// num frame for '-' stats digit

#define	ICON_SIZE			48
#define	CHAR_WIDTH			32
#define	CHAR_HEIGHT			48
#define	TEXT_ICON_SPACE		4

#define	TEAMCHAT_WIDTH		80
#define TEAMCHAT_HEIGHT		8

// very large characters
#define	GIANT_WIDTH			32
#define	GIANT_HEIGHT		48

#define	NUM_CROSSHAIRS		10

#define TEAM_OVERLAY_MAXNAME_WIDTH	12
#define TEAM_OVERLAY_MAXLOCATION_WIDTH	16

#define	DEFAULT_MODEL			"mmx"
#define	DEFAULT_TEAM_MODEL		"mmx"
#define	DEFAULT_TEAM_HEAD		"mmx"

#define DEFAULT_REDTEAM_NAME		"Hunters"
#define DEFAULT_BLUETEAM_NAME		"Mavricks"

#define MAX_SFLASH		16

typedef enum
{
    FOOTSTEP_NORMAL,
    FOOTSTEP_BOOT,
    FOOTSTEP_FLESH,
    FOOTSTEP_MECH,
    FOOTSTEP_ENERGY,
    FOOTSTEP_METAL,
    FOOTSTEP_SPLASH,

    FOOTSTEP_TOTAL
} footstep_t;

typedef enum
{
    IMPACTSOUND_DEFAULT,
    IMPACTSOUND_METAL,
    IMPACTSOUND_FLESH
} impactSound_t;

//=================================================

// player entities need to track more information
// than any other type of entity.

// note that not every player entity is a client entity,
// because corpses after respawn are outside the normal
// client numbering range

// when changing animation, set animationTime to frameTime + lerping time
// The current lerp will finish out, then it will lerp to the new animation
typedef struct
{
    int			oldFrame;
    int			oldFrameTime;		// time when ->oldFrame was exactly on

    int			frame;
    int			frameTime;			// time when ->frame will be exactly on

    float		backlerp;

    float		yawAngle;
    qboolean	yawing;
    float		pitchAngle;
    qboolean	pitching;

    int			animationNumber;	// may include ANIM_TOGGLEBIT
    animation_t	*animation;
    int			animationTime;		// time when the first frame of the animation will be exact
} lerpFrame_t;


typedef struct
{
    lerpFrame_t		legs, torso, head, flag;
    int				painTime;
    int				painDirection;	// flip from 0 to 1
    int				lightningFiring;

    // railgun trail spawning
    vec3_t			railgunImpact;
    qboolean		railgunFlash;

    // machinegun spinning
    float			barrelAngle;
    int				barrelTime;
    qboolean		barrelSpinning;

    lerpFrame_t		hair;
    qboolean		showDashTrail;
    int				deathTime;	//the time that you actually need to be made invisible
    lerpFrame_t		weapons;
} playerEntity_t;

//=================================================


#define MAX_SOUNDS_NPC 10

typedef struct npcSounds_s
{
    int count;
    int frame[MAX_SOUNDS_NPC];
    sfxHandle_t	sound[MAX_SOUNDS_NPC];
} npcSounds_t;

typedef struct
{
    lerpFrame_t		body;
    int				sound;
} npcEntity_t;

typedef struct
{
    int				state;
    lerpFrame_t		model;
    animation_t		anim;
    int				lastCode;
    float			speed;
} md3Entity_t;

// centity_t have a direct corespondence with gentity_t in the game, but
// only the entityState_t is directly communicated to the cgame
typedef struct centity_s
{
    entityState_t	currentState;	// from cg.frame
    entityState_t	nextState;		// from cg.nextFrame, if available
    qboolean		interpolate;	// true if next is valid to interpolate to
    qboolean		currentValid;	// true if cg.frame holds this entity

    int				muzzleFlashTime;	// move to playerEntity?
    int				previousEvent;
    int				teleportFlag;

    int				trailTime;		// so missile trails can handle dropped initial packets
    int				dustTrailTime;
    int				miscTime;

    int				snapShotTime;	// last time this entity was found in a snapshot

    playerEntity_t	pe;
    npcEntity_t		ne;
    md3Entity_t		md3;

    int				errorTime;		// decay the error from this time
    vec3_t			errorOrigin;
    vec3_t			errorAngles;

    qboolean		extrapolated;	// false if origin / angles is an interpolation
    vec3_t			rawOrigin;
    vec3_t			rawAngles;

    vec3_t			beamEnd;

    // exact interpolated position of entity on this frame
    vec3_t			lerpOrigin;
    vec3_t			lerpAngles;

    int				chargelevel;
    int				hitTime;	//time to stop showing the hit sprite
} centity_t;


//======================================================================

// local entities are created as a result of events or predicted actions,
// and live independantly from all server transmitted entities

typedef struct markPoly_s
{
    struct markPoly_s	*prevMark, *nextMark;
    int			time;
    qhandle_t	markShader;
    qboolean	alphaFade;		// fade alpha instead of rgb
    float		color[4];
    poly_t		poly;
    polyVert_t	verts[MAX_VERTS_ON_POLY];
} markPoly_t;


typedef enum
{
    LE_MARK,
    LE_EXPLOSION,
    LE_SPHERE_EXPLOSION,
    LE_SPRITE_EXPLOSION,
    LE_FRAGMENT,
    LE_MOVE_SCALE_FADE,
    LE_FALL_SCALE_FADE,
    LE_FADE_RGB,
    LE_SCALE_FADE,
    LE_SCOREPLUM
} leType_t;

typedef enum
{
    LEF_PUFF_DONT_SCALE  = 0x0001,			// do not scale size over time
    LEF_TUMBLE			 = 0x0002,			// tumble over time, used for ejecting shells
    LEF_SPRITE_DELAY	 = 0x0004,			// if fadeintime > starttime, dont show this sprite yet..., only for MOVE_SCALE_FADE
    LEF_PUFF_DONT_FADE	 = 0x0008,			// don't fade
    LEF_PUFF_FADE_RGB	 = 0x0010,			// fade with RGB instead of alpha
    LEF_PUFF_SHRINK		 = 0x0020			// shrink it
} leFlag_t;

typedef enum
{
    LEMT_NONE,
    LEMT_BURN
} leMarkType_t;			// fragment local entities can leave marks on walls

typedef enum
{
    LEBS_NONE,
    LEBS_BRASS
} leBounceSoundType_t;	// fragment local entities can make sounds on impacts

typedef struct localEntity_s
{
    struct localEntity_s	*prev, *next;
    leType_t		leType;
    int				leFlags;

    int				startTime;
    int				endTime;
    int				fadeInTime;

    float			lifeRate;			// 1.0 / (endTime - startTime)

    trajectory_t	pos;
    trajectory_t	angles;

    float			bounceFactor;		// 0.0 = no bounce, 1.0 = perfect

    float			color[4];

    float			radius;

    float			light;
    vec3_t			lightColor;

    leMarkType_t		leMarkType;		// mark to leave on fragment impact
    leBounceSoundType_t	leBounceSoundType;

    refEntity_t		refEntity;
} localEntity_t;

//======================================================================


typedef struct
{
    int				client;
    int				score;
    int				ping;
    int				time;
    int				scoreFlags;
    int				powerUps;
    int				accuracy;
    int				impressiveCount;
    int				excellentCount;
    int				guantletCount;
    int				defendCount;
    int				assistCount;
    int				captures;
    qboolean	perfect;
    int				team;
} score_t;

// each client has an associated clientInfo_t
// that contains media references necessary to present the
// client model and other color coded effects
// this is regenerated each time a client's configstring changes,
// usually as a result of a userinfo (name, model, etc) change
#define	MAX_CUSTOM_SOUNDS	32

typedef struct
{
    qboolean		infoValid;

    char			name[MAX_QPATH];
    team_t			team;

    int				botSkill;		// 0 = not bot, 1-5 = bot

    int				color;
    vec3_t			color1;
    vec3_t			color2;

    int				score;			// updated by score servercmds
    int				location;		// location index for team mode
    int				health;			// you only get this info about your teammates
    int				armor;
    int				curWeapon;

    int				handicap;
    int				wins, losses;	// in tourney mode

    int				teamTask;		// task in teamplay (offence/defence)
    qboolean		teamLeader;		// true when this is a team leader

    int				powerups;		// so can display quad/flag status


    int				breathPuffTime;

    // when clientinfo is changed, the loading of models/skins/sounds
    // can be deferred until you are dead, to prevent hitches in
    // gameplay
    char			modelName[MAX_QPATH];
    char			skinName[MAX_QPATH];
    char			headModelName[MAX_QPATH];
    char			headSkinName[MAX_QPATH];
    char			redTeam[MAX_TEAMNAME];
    char			blueTeam[MAX_TEAMNAME];
    qboolean		deferred;

    qboolean		newAnims;		// true if using the new mission pack animations
    qboolean		fixedlegs;		// true if legs yaw is always the same as torso yaw
    qboolean		fixedtorso;		// true if torso never changes yaw

    vec3_t			headOffset;		// move head in icon views
    footstep_t		footsteps;
    gender_t		gender;			// from model

    qhandle_t		legsModel;
    qhandle_t		legsSkin;

    qhandle_t		torsoModel;
    qhandle_t		torsoSkin;

    qhandle_t		headModel;
    qhandle_t		headSkin;
// for Zero's hair
    qhandle_t		hairModel;
    qhandle_t		hairSkin;

    qhandle_t		modelIcon;

    animation_t		animations[MAX_TOTALANIMATIONS];

    sfxHandle_t		sounds[MAX_CUSTOM_SOUNDS];
} clientInfo_t;


// each WP_* weapon enum has an associated weaponInfo_t
// that contains media references necessary to present the
// weapon and its effects
typedef struct weaponInfo_s
{
    qboolean		registered;
    gitem_t			*item;

    qhandle_t		handsModel;			// the hands don't actually draw, they just position the weapon
    qhandle_t		weaponModel;
    qhandle_t		barrelModel;
    qhandle_t		flashModel;

    vec3_t			weaponMidpoint;		// so it will rotate centered instead of by tag

    float			flashDlight;
    vec3_t			flashDlightColor;
    sfxHandle_t		flashSound[4];		// fast firing weapons randomly choose

    qhandle_t		weaponIcon;
    qhandle_t		ammoIcon;

    qhandle_t		ammoModel;

    qhandle_t		missileModel;
    sfxHandle_t		missileSound;
    void			(*missileTrailFunc)( centity_t *, const struct weaponInfo_s *wi );
    float			missileDlight;
    vec3_t			missileDlightColor;
    int				missileRenderfx;

    void			(*ejectBrassFunc)( centity_t * );

    float			trailRadius;
    float			wiTrailTime;

    sfxHandle_t		readySound;
    sfxHandle_t		firingSound;
    qboolean		loopFireSound;
} weaponInfo_t;

//
// Each NPC in the Game has an associated npcInfo_t;
//
typedef struct
{
    qboolean		registered;
    qhandle_t		model;
    animation_t		animations[MAX_TOTALANIMATIONS];
    npcSounds_t	sounds[MAX_ANIMATIONS_NPC];
} npcInfo_t;

// each IT_* item has an associated itemInfo_t
// that constains media references necessary to present the
// item and its effects
typedef struct
{
    qboolean		registered;
    qhandle_t		models[MAX_ITEM_MODELS];
    qhandle_t		icon;
} itemInfo_t;


typedef struct
{
    int				itemNum;
} powerupInfo_t;


#define MAX_SKULLTRAIL		10

typedef struct
{
    vec3_t positions[MAX_SKULLTRAIL];
    int numpositions;
} skulltrail_t;


#define MAX_REWARDSTACK		10
#define MAX_SOUNDBUFFER		20

// HUD groups
#define ALL_GROUPS 0x0000FFFF
#define XHAIRNAME 0x00000001
#define UPPERIGHT 0x00000002
#define XCONSOLE 0x00000004
#define XWEAPONSELECT 0x00000008
#define XCENTERPRINT 0x00000010

// loadingscreen
#define MAX_CONSOLE_TEXT 8192
#define MAX_CONSOLE_LINES 32

typedef struct
{
    int time;
    int length;
    char *icon;
} consoleLine_t;

// Screen Flashes =============
typedef struct
{
    int			screenFlashID;
    int			screenFlashTime;
    qboolean	screenFlashOff;
    qhandle_t	screenFlashShader;

} screenFlash_t;

screenFlash_t		cg_screenFlash[MAX_SFLASH];


//======================================================================

// all cg.stepTime, cg.duckTime, cg.landTime, etc are set to cg.time when the action
// occurs, and they will have visible effects for #define STEP_TIME or whatever msec after

#define MAX_PREDICTED_EVENTS	16

typedef struct
{
    int			clientFrame;		// incremented each frame

    int			clientNum;

    qboolean	demoPlayback;
    qboolean	levelShot;			// taking a level menu screenshot
    int			deferredPlayerLoading;
    qboolean	loading;			// don't defer players at initial startup
    qboolean	intermissionStarted;	// don't play voice rewards, because game will end shortly

    // there are only one or two snapshot_t that are relevent at a time
    int			latestSnapshotNum;	// the number of snapshots the client system has received
    int			latestSnapshotTime;	// the time from latestSnapshotNum, so we don't need to read the snapshot yet

    snapshot_t	*snap;				// cg.snap->serverTime <= cg.time
    snapshot_t	*nextSnap;			// cg.nextSnap->serverTime > cg.time, or NULL
    snapshot_t	activeSnapshots[2];

    float		frameInterpolation;	// (float)( cg.time - cg.frame->serverTime ) / (cg.nextFrame->serverTime - cg.frame->serverTime)

    qboolean	thisFrameTeleport;
    qboolean	nextFrameTeleport;

    int			frametime;		// cg.time - cg.oldTime

    int			time;			// this is the time value that the client
    // is rendering at.
    int			oldTime;		// time at last frame, used for missile trails and prediction checking

    int			physicsTime;	// either cg.snap->time or cg.nextSnap->time

    int			timelimitWarnings;	// 5 min, 1 min, overtime
    int			fraglimitWarnings;

    qboolean	mapRestart;			// set on a map restart to set back the weapon

    qboolean	renderingThirdPerson;		// during deaths, chasecams, etc

    // prediction state
    qboolean	hyperspace;				// true if prediction has hit a trigger_teleport
    playerState_t	predictedPlayerState;
    centity_t		predictedPlayerEntity;
    qboolean	validPPS;				// clear until the first call to CG_PredictPlayerState
    int			predictedErrorTime;
    vec3_t		predictedError;

    int			eventSequence;
    int			predictableEvents[MAX_PREDICTED_EVENTS];

    float		stepChange;				// for stair up smoothing
    int			stepTime;

    float		duckChange;				// for duck viewheight smoothing
    int			duckTime;

    float		landChange;				// for landing hard
    int			landTime;

    // input state sent to server
    int			weaponSelect;

    // auto rotating items
    vec3_t		autoAngles;
    vec3_t		autoAxis[3];
    vec3_t		autoAnglesFast;
    vec3_t		autoAxisFast[3];

    // view rendering
    refdef_t	refdef;
    vec3_t		refdefViewAngles;		// will be converted to refdef.viewaxis

    // zoom key
    qboolean	zoomed;
    int			zoomTime;
    float		zoomSensitivity;

    // information screen text during loading
    char		infoScreenText[MAX_STRING_CHARS];

    // scoreboard
    int			scoresRequestTime;
    int			numScores;
    int			selectedScore;
    int			teamScores[2];
    score_t		scores[MAX_CLIENTS];
    qboolean	showScores;
    qboolean	scoreBoardShowing;
    int			scoreFadeTime;
    char		killerName[MAX_NAME_LENGTH];
    char			spectatorList[MAX_STRING_CHARS];		// list of names
    int				spectatorLen;												// length of list
    float			spectatorWidth;											// width in device units
    int				spectatorTime;											// next time to offset
    int				spectatorPaintX;										// current paint x
    int				spectatorPaintX2;										// current paint x
    int				spectatorOffset;										// current offset from start
    int				spectatorPaintLen; 									// current offset from start

    // skull trails
    skulltrail_t	skulltrails[MAX_CLIENTS];

    // centerprinting
    int			centerPrintTime;
    int			centerPrintCharWidth;
    int			centerPrintY;
    char		centerPrint[1024];
    int			centerPrintLines;

    // low ammo warning state
    int			lowAmmoWarning;		// 1 = low, 2 = empty

    // kill timers for carnage reward
    int			lastKillTime;

    // crosshair client ID
    int			crosshairClientNum;
    int			crosshairClientTime;

    // powerup active flashing
    int			powerupActive;
    int			powerupTime;

    // attacking player
    int			attackerTime;
    int			voiceTime;

    // reward medals
    int			rewardStack;
    int			rewardTime;
    int			rewardCount[MAX_REWARDSTACK];
    qhandle_t	rewardShader[MAX_REWARDSTACK];
    qhandle_t	rewardSound[MAX_REWARDSTACK];

    // sound buffer mainly for announcer sounds
    int			soundBufferIn;
    int			soundBufferOut;
    int			soundTime;
    qhandle_t	soundBuffer[MAX_SOUNDBUFFER];

    // warmup countdown
    int			warmup;
    int			warmupCount;

    //==========================

    int			itemPickup;
    int			itemPickupTime;
    int			itemPickupBlendTime;	// the pulse around the crosshair is timed seperately

    int			weaponSelectTime;
    int			weaponAnimation;
    int			weaponAnimationTime;

    // blend blobs
    float		damageTime;
    float		damageX, damageY, damageValue;

    // status bar head
    float		headYaw;
    float		headEndPitch;
    float		headEndYaw;
    int			headEndTime;
    float		headStartPitch;
    float		headStartYaw;
    int			headStartTime;

    // view movement
    float		v_dmg_time;
    float		v_dmg_pitch;
    float		v_dmg_roll;

    vec3_t		kick_angles;	// weapon kicks
    vec3_t		kick_origin;

    // temp working variables for player view
    float		bobfracsin;
    int			bobcycle;
    float		xyspeed;
    int     nextOrbitTime;

    //qboolean cameraMode;		// if rendering from a loaded camera


    // development tool
    refEntity_t		testModelEntity;
    char			testModelName[MAX_QPATH];
    qboolean		testGun;

    float         mediaFraction;
    float         soundFraction;
    float         graphicFraction;
    char          consoleText[ MAX_CONSOLE_TEXT ];
    consoleLine_t consoleLines[ MAX_CONSOLE_LINES ];
    int           numConsoleLines;
    qboolean      consoleValid;

    // NIGHTZ - CAMERA POSITION
    vec3_t		cameraPos;

    // DonX - Screen Flash
    screenFlash_t	screenFlash;

    // NIGHTZ - SCREEN SHAKE
    int			shakeFactor;

    // END NIGHTZ

} cg_t;


// all of the model, shader, and sound references that are
// loaded at gamestate time are stored in cgMedia_t
// Other media that can be tied to clients, weapons, or items are
// stored in the clientInfo_t, itemInfo_t, weaponInfo_t, and powerupInfo_t
typedef struct
{
    qhandle_t	charsetShader;
    qhandle_t	charsetProp;
    qhandle_t	charsetPropGlow;
    qhandle_t	charsetPropB;
    qhandle_t	whiteShader;

    qhandle_t	redFlagModel;
    qhandle_t	blueFlagModel;
    qhandle_t	neutralFlagModel;
    qhandle_t	redFlagShader[3];
    qhandle_t	blueFlagShader[3];
    qhandle_t	flagShader[4];

    qhandle_t	flagPoleModel;
    qhandle_t	flagFlapModel;

    qhandle_t	redFlagFlapSkin;
    qhandle_t	blueFlagFlapSkin;
    qhandle_t	neutralFlagFlapSkin;

    qhandle_t	redFlagBaseModel;
    qhandle_t	blueFlagBaseModel;
    qhandle_t	neutralFlagBaseModel;

    qhandle_t	armorModel;
    qhandle_t	armorIcon;

    qhandle_t	teamStatusBar;

    qhandle_t	deferShader;

    qhandle_t	railRingsShader;
    qhandle_t	railCoreShader;

    qhandle_t	lightningShader;
    qhandle_t	grappleShader;

    qhandle_t	friendShader;

    qhandle_t	balloonShader;
    qhandle_t	connectionShader;

    qhandle_t	selectShader;
    qhandle_t	viewBloodShader;
    qhandle_t	tracerShader;
    qhandle_t	crosshairShader[NUM_CROSSHAIRS];
    qhandle_t	lagometerShader;
    qhandle_t	backTileShader;
    qhandle_t	noammoShader;

    qhandle_t	smokePuffShader;
    qhandle_t	smokePuffRageProShader;
    qhandle_t	plasmaBallShader;
    qhandle_t	waterBubbleShader;

    qhandle_t	numberShaders[11];

    qhandle_t	shadowMarkShader;

    qhandle_t	botSkillShaders[5];

    // wall mark shaders
    qhandle_t	wakeMarkShader;
    qhandle_t	bulletMarkShader;
    qhandle_t	burnMarkShader;
    qhandle_t	holeMarkShader;
    qhandle_t	energyMarkShader;

    // Peter: Radar shaders
    qhandle_t	radarShader;
    qhandle_t	rd_up;
    qhandle_t	rd_down;
    qhandle_t	rd_level;

    // Peter: Player effect shaders
    qhandle_t	pfx_frozen;
    qhandle_t	pfx_shocked;
    qhandle_t	pfx_hit;
    qhandle_t	pfx_poison;
    qhandle_t	pfx_burned;
    qhandle_t	pfx_spun;

    // powerup shaders
    qhandle_t	quadShader;
    qhandle_t	redQuadShader;
    qhandle_t	quadWeaponShader;
    qhandle_t	invisShader;
    qhandle_t	regenShader;
    qhandle_t	battleSuitShader;
    qhandle_t	battleWeaponShader;
    qhandle_t	hastePuffShader;
    qhandle_t	jetSmokeShader;

    // weapon effect models
    qhandle_t	lightningExplosionModel;

    qhandle_t	ringFlashModel;
    qhandle_t	xshotFlashModel;
    qhandle_t	xshot2FlashModel;
    qhandle_t	bassFlashModel;
    qhandle_t	mshotFlashModel;
    qhandle_t	zshotFlashModel;

    qhandle_t	gravwellModel;
    qhandle_t	protoshieldModel;

    // weapon effect shaders
    qhandle_t	railExplosionShader;
    qhandle_t	plasmaExplosionShader;

    // special effects models
    qhandle_t	heartShader;

    // breakable glass
    qhandle_t	breakglass01;
    qhandle_t	breakglass02;
    qhandle_t	breakglass03;
    qhandle_t	breakbrick01;
    qhandle_t	breakbrick02;
    qhandle_t	breakbrick03;

    // sounds
    sfxHandle_t	tracerSound;
    sfxHandle_t	selectSound;
    sfxHandle_t	useNothingSound;
    sfxHandle_t	wearOffSound;
    sfxHandle_t	footsteps[FOOTSTEP_TOTAL][4];
    sfxHandle_t	sfx_lghit1;
    sfxHandle_t	sfx_lghit2;
    sfxHandle_t	sfx_lghit3;
    // Explosion Hit SFX
    sfxHandle_t	sfx_rockexp;
    sfxHandle_t	sfx_plasmaexp;
    sfxHandle_t sfx_Earthexp;
    sfxHandle_t	sfx_Bang1exp;
    sfxHandle_t	sfx_Bang2exp;
    sfxHandle_t	sfx_Bang3exp;
    sfxHandle_t	sfx_Bang4exp;
    sfxHandle_t	sfx_Tempest1exp;
    sfxHandle_t	sfx_Crystal1exp;
    sfxHandle_t	sfx_Acid2exp;

    sfxHandle_t	winnerSound;
    sfxHandle_t	loserSound;
    sfxHandle_t	teleInSound;
    sfxHandle_t	teleOutSound;
    sfxHandle_t	noAmmoSound;
    sfxHandle_t	respawnSound;
    sfxHandle_t	lightningStrike;
    sfxHandle_t talkSound;
    sfxHandle_t landSound;
    sfxHandle_t fallSound;
    sfxHandle_t jumpPadSound;

    sfxHandle_t oneMinuteSound;
    sfxHandle_t fiveMinuteSound;
    sfxHandle_t suddenDeathSound;

    sfxHandle_t threeFragSound;
    sfxHandle_t twoFragSound;
    sfxHandle_t oneFragSound;

    sfxHandle_t hitSound;
    sfxHandle_t hitSoundHighArmor;
    sfxHandle_t hitSoundLowArmor;
    sfxHandle_t hitTeamSound;
    sfxHandle_t firstImpressiveSound;
    sfxHandle_t firstExcellentSound;
    sfxHandle_t firstHumiliationSound;

    sfxHandle_t takenLeadSound;
    sfxHandle_t tiedLeadSound;
    sfxHandle_t lostLeadSound;

    sfxHandle_t voteNow;
    sfxHandle_t votePassed;
    sfxHandle_t voteFailed;

    sfxHandle_t watrInSound;
    sfxHandle_t watrOutSound;
    sfxHandle_t watrUnSound;

    sfxHandle_t flightSound;
    sfxHandle_t subtankSound;

    sfxHandle_t weaponHoverSound;

    sfxHandle_t	glassbreakSound;

    // teamplay sounds
    sfxHandle_t redScoredSound;
    sfxHandle_t blueScoredSound;
    sfxHandle_t redLeadsSound;
    sfxHandle_t blueLeadsSound;
    sfxHandle_t teamsTiedSound;

    sfxHandle_t	captureYourTeamSound;
    sfxHandle_t	captureOpponentSound;
    sfxHandle_t	returnYourTeamSound;
    sfxHandle_t	returnOpponentSound;
    sfxHandle_t	takenYourTeamSound;
    sfxHandle_t	takenOpponentSound;

    sfxHandle_t redFlagReturnedSound;
    sfxHandle_t blueFlagReturnedSound;
    sfxHandle_t neutralFlagReturnedSound;
    sfxHandle_t	enemyTookYourFlagSound;
    sfxHandle_t	enemyTookTheFlagSound;
    sfxHandle_t yourTeamTookEnemyFlagSound;
    sfxHandle_t yourTeamTookTheFlagSound;
    sfxHandle_t	youHaveFlagSound;

    // tournament sounds
    sfxHandle_t	count3Sound;
    sfxHandle_t	count2Sound;
    sfxHandle_t	count1Sound;
    sfxHandle_t	countFightSound;
    sfxHandle_t	countPrepareSound;
    qhandle_t patrolShader;
    qhandle_t assaultShader;
    qhandle_t campShader;
    qhandle_t followShader;
    qhandle_t defendShader;
    qhandle_t teamLeaderShader;
    qhandle_t retrieveShader;
    qhandle_t escortShader;
    qhandle_t flagShaders[3];

    qhandle_t cursor;
    qhandle_t selectCursor;
    qhandle_t sizeCursor;

    sfxHandle_t	n_healthSound;
    sfxHandle_t	hgrenb1aSound;
    sfxHandle_t	hgrenb2aSound;
    sfxHandle_t	wstbimplSound;
    sfxHandle_t	wstbimpmSound;
    sfxHandle_t	wstbimpdSound;
    sfxHandle_t	wstbactvSound;
    sfxHandle_t rainSound;
    sfxHandle_t earthquakeSound;
    //weapon shaders
    //general
    qhandle_t	xbSpriteModel;
    qhandle_t	xShot;
    qhandle_t	xShot4;
    qhandle_t	bassShot;
    qhandle_t	xShotShader;	//when the x-buster hits the wall, uncharged
    qhandle_t	xShot1Shader;	//when the x-buster hits the wall, level 1
    qhandle_t	xShot2Shader;	//when the x-buster hits the wall, level 2
    qhandle_t	xShot3Shader;	//when the x-buster hits the wall, level 3
    qhandle_t	bassShotShader;	//when bass' cannon shots hits the wall
    qhandle_t	xbSaberModel;	//zero's saber
    qhandle_t	xbSaberhiltShader;	//zero's saber
    qhandle_t	xbSaberbladeModel;	//zero's saber blade
    qhandle_t	xbSaberbladeShader;	//zero's saber blade
    //charge
    qhandle_t	chargeFireShot1;
    qhandle_t	chargeFireShot2;
    qhandle_t	chargeFireHit;
    qhandle_t	chargeFireHit2;
    qhandle_t	chargeWaterShot1;
    qhandle_t	chargeWaterShot2;
    qhandle_t	chargeWaterShot3;
    qhandle_t	chargeWaterHit;
    qhandle_t	chargeWaterHit2;
    qhandle_t	chargeWaterHit3;
    qhandle_t	chargeWindShot1;
    qhandle_t	chargeWindShot2;
    qhandle_t	chargeWindHit;
    qhandle_t	chargeEarthShot1;
    qhandle_t	chargeEarthShot2;
    qhandle_t	chargeEarthHit;
    qhandle_t	chargeNatureShot1;
    qhandle_t	chargeNatureShot2;
    qhandle_t	chargeNatureHit;
    qhandle_t	chargeTechHit;
    qhandle_t	chargeGravityShot1;
    qhandle_t	chargeGravityShot2;
    qhandle_t	chargeGravityHit;
    qhandle_t	chargeLightShot1;
    qhandle_t	chargeLightShot2;
    qhandle_t	chargeLightHit;
    qhandle_t	chargeLightHitM;
    qhandle_t	chargeElectricShot1;
    qhandle_t	chargeElectricShot2;
    qhandle_t	chargeElectricHit;
    //buster
    qhandle_t	busterFireShot;
    qhandle_t	busterFireHit;
    qhandle_t	busterWaterShot;
    qhandle_t	busterWaterHit;
    qhandle_t	busterWindShot;
    qhandle_t	busterWindHit;
    qhandle_t	busterEarthHit;
    qhandle_t	busterNatureShot;
    qhandle_t	busterNatureHit;
    qhandle_t	busterTechHit;
    qhandle_t	busterGravityShot;
    qhandle_t	busterGravityHit;
    qhandle_t	busterLightHit;
    //rapid
    qhandle_t	rapidFireShot;
    qhandle_t	rapidFireHit;
    qhandle_t	rapidWaterShot;
    qhandle_t	rapidWaterHit;
    qhandle_t	rapidWindShot;
    qhandle_t	rapidWindHit;
    qhandle_t	rapidEarthHit;
    qhandle_t	rapidNatureShot;
    qhandle_t	rapidNatureShot2;
    qhandle_t	rapidNatureHit;
    qhandle_t	rapidTechHit;
    qhandle_t	rapidGravityShot;
    qhandle_t	rapidGravityHit;
    qhandle_t	rapidLightShot;
    qhandle_t	rapidLightHit;
    qhandle_t	rapidElectricShot;
    qhandle_t	rapidElectricHit;
    //melee
    qhandle_t	meleeFireShot;
    qhandle_t	meleeFireHit;
    qhandle_t	meleeWaterShot;
    qhandle_t	meleeWaterHit;
    qhandle_t	meleeWindShot;
    qhandle_t	meleeWindHit;
    qhandle_t	meleeEarthShot;
    qhandle_t	meleeEarthHit;
    qhandle_t	meleeNatureShot;
    qhandle_t	meleeNatureHit;
    qhandle_t	meleeTechShot;
    qhandle_t	meleeTechHit;
    qhandle_t	meleeGravityShot;
    qhandle_t	meleeGravityHit;
    qhandle_t	meleeLightShot;
    qhandle_t	meleeLightHit;
    qhandle_t	meleeElectricShot;
    qhandle_t	meleeElectricHit;
    //Sheild
    qhandle_t	sheildFireShot;
    qhandle_t	sheildFireHit;
    qhandle_t	sheildWaterShot;
    qhandle_t	sheildWaterHit;
    qhandle_t	sheildWindShot;
    qhandle_t	sheildWindHit;
    qhandle_t	sheildEarthShot;
    qhandle_t	sheildEarthHit;
    qhandle_t	sheildNatureShot;
    qhandle_t	sheildNatureHit;
    qhandle_t	sheildTechShot;
    qhandle_t	sheildTechHit;
    qhandle_t	sheildGravityHit;
    qhandle_t	sheildLightHit;
    qhandle_t	sheildElectricShot;
    qhandle_t	sheildElectricHit;
    //misc crap
    qhandle_t	blackDashEffectShader;
    qhandle_t	redDashEffectShader;
    qhandle_t	greenDashEffectShader;
    qhandle_t	yellowDashEffectShader;
    qhandle_t	blueDashEffectShader;
    qhandle_t	cyanDashEffectShader;
    qhandle_t	purpleDashEffectShader;
    qhandle_t	whiteDashEffectShader;
    qhandle_t	lightningboltShader;
    qhandle_t	celShader;
    qhandle_t	gotHitShader;
    qhandle_t	chargeGlowShader;
    qhandle_t	chargeSphereShader;
    qhandle_t	chargeSphere2Shader;
    qhandle_t	chargeSphere3Shader;
    qhandle_t	charge1Shader;
    qhandle_t	charge2Shader;
    qhandle_t	charge3Shader;
    qhandle_t	charge4Shader;
    qhandle_t	xbSmokeShader;
    qhandle_t	xbWallDustShader;
    qhandle_t	xbSparkShader;
    qhandle_t	xbBlueSparkShader;
    qhandle_t	xbDeathShader;
    qhandle_t	xbBrownDeathShader;
    qhandle_t	boostModel;		//when we want booster effects for whatever reason
    qhandle_t	boosterShader;	//when we want booster effects for whatever reason
    qhandle_t	hitSparkShader; //added for particle effects
    qhandle_t	crackMarkShader; //added for cracked walls
    // Teleport In Out
    qhandle_t	blackSpawnShader;
    qhandle_t	redSpawnShader;
    qhandle_t	greenSpawnShader;
    qhandle_t	yellowSpawnShader;
    qhandle_t	blueSpawnShader;
    qhandle_t	cyanSpawnShader;
    qhandle_t	purpleSpawnShader;
    qhandle_t	whiteSpawnShader;
    qhandle_t	blackSplashShader;
    qhandle_t	redSplashShader;
    qhandle_t	greenSplashShader;
    qhandle_t	yellowSplashShader;
    qhandle_t	blueSplashShader;
    qhandle_t	cyanSplashShader;
    qhandle_t	purpleSplashShader;
    qhandle_t	whiteSplashShader;
    // Trail Shaders
    qhandle_t	xbTrail_Circle;	//generic faded circle shader, for things like weapon trails
    qhandle_t	xbTrail_Fire;	//ice trail
    qhandle_t	xbTrail_Wind;	//wind trail
    qhandle_t	xbTrail_Plant;
    qhandle_t	xbTrail_Water;
    qhandle_t	busterTrail1Shader;
    qhandle_t	busterTrail2Shader;
    qhandle_t	busterTrail3Shader;
    // Other
    qhandle_t	WeaponModel;
    qhandle_t	xbIceCrystal;
    // Explosion Shaders
    qhandle_t	xbExplosionSphere;
    qhandle_t	xbExplosionShader1;
    qhandle_t	xbExplosionWhiteShader;
    qhandle_t	xbExplosionBlueShader;
    qhandle_t	xbExplosionGreenShader;
    qhandle_t	xbExplosionYellowShader;
    qhandle_t	xbExplosionPurpleShader;
    qhandle_t	xbExplosionRedShader;
    qhandle_t	xbExplosionBlackShader;
    qhandle_t	xbExplosionGreyShader;
    // Particles
    qhandle_t	FireParticle;
    qhandle_t	WaterParticle;
    qhandle_t	WindParticle;
    qhandle_t	EarthParticle;
    qhandle_t	NatureParticle;
    qhandle_t	TechParticle;
    qhandle_t	GravityParticle;
    qhandle_t	EnergyParticle;
    qhandle_t	ElectricParticle;
    // Ground FX
    qhandle_t	GroundFXplayerSelected1;
    qhandle_t	GroundFXplayerSelected2;
    qhandle_t	GroundFXplayerSelected3;
    qhandle_t	GroundFXplayerSelected4;
    qhandle_t	GroundFXplayerTeamBlue;
    qhandle_t	GroundFXplayerTeamRed;
    qhandle_t	GroundFXplayerUpgrade;
    qhandle_t	GroundFXplayerHoming;
    qhandle_t	GroundFXplayerLightningStruck;
    qhandle_t	GroundFXplayerTalk;
    qhandle_t	GroundFXplayerCharge;
    qhandle_t	GroundFXplayerItem;
    //ScreenFlash Shaders
    qhandle_t	ScreenFlashFX_Lightning1;
    qhandle_t	ScreenFlashFX_Red;
    qhandle_t	ScreenFlashFX_Green;
    qhandle_t	ScreenFlashFX_Tracking;
    qhandle_t	ScreenFlashFX_Pain;
    qhandle_t	ScreenFlashFX_Rage;
    qhandle_t	ScreenFlashFX_Cloak;
    qhandle_t	ScreenFlashFX_Burn;
    qhandle_t	ScreenFlashFX_Virus;
    qhandle_t	ScreenFlashFX_Poison;
    qhandle_t	ScreenFlashFX_Flash;
    qhandle_t	ScreenFlashFX_Shocked;
    qhandle_t	ScreenFlashFX_Frozen;
    qhandle_t	ScreenFlashFX_Sense;
    qhandle_t	ScreenFlashFX_Dmg;
    qhandle_t	ScreenFlashFX_Gravity;
    /* Peter FIXME: Were going to replace these with model attachments
    qhandle_t	XhelmetUpgradeShader;	//to show the helmet upgrade
    qhandle_t	XbusterUpgradeShader;	//to show the buster upgrade
    qhandle_t	XarmorUpgradeShader;	//to show the chest upgrade
    qhandle_t	XlegsUpgradeShader;		//to show the legs upgrade
    qhandle_t	XbackUpgradeShader;		//to show the back upgrade
    qhandle_t	FhelmetUpgradeShader;	//to show the helmet upgrade
    qhandle_t	FbusterUpgradeShader;	//to show the buster upgrade
    qhandle_t	FarmorUpgradeShader;	//to show the chest upgrade
    qhandle_t	FlegsUpgradeShader;		//to show the legs upgrade
    qhandle_t	FbackUpgradeShader;		//to show the back upgrade
    */
    //sounds
    qhandle_t	chargeStartSound;
    qhandle_t	chargeLoopSound;

} cgMedia_t;


// The client game static (cgs) structure hold everything
// loaded or calculated from the gamestate.  It will NOT
// be cleared when a tournement restart is done, allowing
// all clients to begin playing instantly
typedef struct
{
    gameState_t		gameState;			// gamestate from server
    glconfig_t		glconfig;			// rendering configuration
    float			screenXScale;		// derived from glconfig
    float			screenYScale;
    float			screenXBias;

    int				serverCommandSequence;	// reliable command stream counter
    int				processedSnapshotNum;// the number of snapshots cgame has requested

    qboolean		localServer;		// detected on startup by checking sv_running

    // parsed from serverinfo
    gametype_t		gametype;
    int				dmflags;
    int				teamflags;
    int				fraglimit;
    int				capturelimit;
    int				timelimit;
    int				maxclients;
    char			mapname[MAX_QPATH];
    char			redTeam[MAX_QPATH];
    char			blueTeam[MAX_QPATH];

    int				voteTime;
    int				voteYes;
    int				voteNo;
    qboolean		voteModified;			// beep whenever changed
    char			voteString[MAX_STRING_TOKENS];

    int				teamVoteTime[2];
    int				teamVoteYes[2];
    int				teamVoteNo[2];
    qboolean		teamVoteModified[2];	// beep whenever changed
    char			teamVoteString[2][MAX_STRING_TOKENS];

    int				levelStartTime;

    int				scores1, scores2;		// from configstrings
    int				redflag, blueflag;		// flag status from configstrings
    int				flagStatus;

    qboolean  newHud;

    //
    // locally derived information from gamestate
    //
    qhandle_t		gameModels[MAX_MODELS];
    sfxHandle_t		gameSounds[MAX_SOUNDS];

    int				numInlineModels;
    qhandle_t		inlineDrawModel[MAX_MODELS];
    vec3_t			inlineModelMidpoints[MAX_MODELS];

    clientInfo_t	clientinfo[MAX_CLIENTS];

    // teamchat width is *3 because of embedded color codes
    char			teamChatMsgs[TEAMCHAT_HEIGHT][TEAMCHAT_WIDTH*3+1];
    int				teamChatMsgTimes[TEAMCHAT_HEIGHT];
    int				teamChatPos;
    int				teamLastChatPos;

    int cursorX;
    int cursorY;
    qboolean eventHandling;
    qboolean mouseCaptured;
    qboolean sizingHud;
    void *capturedItem;
    qhandle_t activeCursor;

    // orders
    int currentOrder;
    qboolean orderPending;
    int orderTime;
    int currentVoiceClient;
    int acceptOrderTime;
    int acceptTask;
    int acceptLeader;
    char acceptVoice[MAX_NAME_LENGTH];

    // media
    cgMedia_t		media;

} cgs_t;


//==============================================================================

extern	int player_stop;
extern	int black_bars;

extern	cgs_t			cgs;
extern	cg_t			cg;
extern	centity_t		cg_entities[MAX_GENTITIES];
extern	weaponInfo_t	cg_weapons[WP_NUM_WEAPONS];
extern	itemInfo_t		cg_items[MAX_ITEMS];
extern	npcInfo_t		cg_npcs[NPC_NUMNPCS];
extern	markPoly_t		cg_markPolys[MAX_MARK_POLYS];

extern	vmCvar_t		cg_centertime;
extern	vmCvar_t		cg_runpitch;
extern	vmCvar_t		cg_runroll;
extern	vmCvar_t		cg_bobup;
extern	vmCvar_t		cg_bobpitch;
extern	vmCvar_t		cg_bobroll;
extern	vmCvar_t		cg_swingSpeed;
extern	vmCvar_t		cg_shadows;
extern	vmCvar_t		cg_gibs;
extern	vmCvar_t		cg_drawTimer;
extern	vmCvar_t		cg_drawFPS;
extern	vmCvar_t		cg_drawSnapshot;
extern	vmCvar_t		cg_draw3dIcons;
extern	vmCvar_t		cg_drawIcons;
extern	vmCvar_t		cg_drawAmmoWarning;
extern	vmCvar_t		cg_drawCrosshair;
extern	vmCvar_t		cg_drawCrosshairNames;
extern	vmCvar_t		cg_drawRewards;
extern	vmCvar_t		cg_drawTeamOverlay;
extern	vmCvar_t		cg_teamOverlayUserinfo;
extern	vmCvar_t		cg_crosshairX;
extern	vmCvar_t		cg_crosshairY;
extern	vmCvar_t		cg_crosshairSize;
extern	vmCvar_t		cg_crosshairHealth;
extern	vmCvar_t		cg_drawStatus;
extern	vmCvar_t		cg_draw2D;
extern	vmCvar_t		cg_animSpeed;
extern	vmCvar_t		cg_debugAnim;
extern	vmCvar_t		cg_debugPosition;
extern	vmCvar_t		cg_debugEvents;
extern	vmCvar_t		cg_railTrailTime;
extern	vmCvar_t		cg_errorDecay;
extern	vmCvar_t		cg_nopredict;
extern	vmCvar_t		cg_noPlayerAnims;
extern	vmCvar_t		cg_showmiss;
extern	vmCvar_t		cg_footsteps;
extern	vmCvar_t		cg_addMarks;
extern	vmCvar_t		cg_brassTime;
extern	vmCvar_t		cg_gun_frame;
extern	vmCvar_t		cg_gun_x;
extern	vmCvar_t		cg_gun_y;
extern	vmCvar_t		cg_gun_z;
extern	vmCvar_t		cg_drawGun;
extern	vmCvar_t		cg_viewsize;
extern	vmCvar_t		cg_tracerChance;
extern	vmCvar_t		cg_tracerWidth;
extern	vmCvar_t		cg_tracerLength;
extern	vmCvar_t		cg_autoswitch;
extern	vmCvar_t		cg_ignore;
extern	vmCvar_t		cg_fov;
extern	vmCvar_t		cg_zoomFov;
extern	vmCvar_t		cg_thirdPersonRange;
extern	vmCvar_t		cg_thirdPersonAngle;
extern	vmCvar_t		cg_thirdPerson;
extern	vmCvar_t		cg_stereoSeparation;
extern	vmCvar_t		cg_lagometer;
extern	vmCvar_t		cg_drawAttacker;
extern	vmCvar_t		cg_synchronousClients;
extern	vmCvar_t		cg_teamChatTime;
extern	vmCvar_t		cg_teamChatHeight;
extern	vmCvar_t		cg_stats;
extern	vmCvar_t 		cg_forceModel;
extern	vmCvar_t 		cg_buildScript;
extern	vmCvar_t		cg_paused;
extern	vmCvar_t		cg_predictItems;
extern	vmCvar_t		cg_deferPlayers;
extern	vmCvar_t		cg_drawFriend;
extern	vmCvar_t		cg_teamChatsOnly;
extern  vmCvar_t		cg_scorePlum;
extern	vmCvar_t		cg_smoothClients;
extern	vmCvar_t		pmove_fixed;
extern	vmCvar_t		pmove_msec;
//extern	vmCvar_t		cg_pmove_fixed;
extern	vmCvar_t		cg_cameraOrbit;
extern	vmCvar_t		cg_cameraOrbitDelay;
extern	vmCvar_t		cg_timescaleFadeEnd;
extern	vmCvar_t		cg_timescaleFadeSpeed;
extern	vmCvar_t		cg_timescale;
extern	vmCvar_t		cg_cameraMode;
extern  vmCvar_t		cg_smallFont;
extern  vmCvar_t		cg_bigFont;
extern	vmCvar_t		cg_noTaunt;
extern	vmCvar_t		cg_noProjectileTrail;
extern	vmCvar_t		cg_oldRail;
extern	vmCvar_t		cg_oldRocket;
extern	vmCvar_t		cg_oldPlasma;
extern	vmCvar_t		cg_trueLightning;
extern	vmCvar_t		cg_redTeamName;
extern	vmCvar_t		cg_blueTeamName;
extern  vmCvar_t        cg_consoleLatency;
extern	vmCvar_t		cg_currentSelectedPlayer;
extern	vmCvar_t		cg_celShaded;
extern	vmCvar_t		cg_color;
extern  vmCvar_t  	  	cg_atmosphericEffects;
extern  vmCvar_t  	  	cg_lowEffects;

//
// cg_atmospheric.c
//
void CG_EffectParse( const char *effectstr );
void CG_AddAtmosphericEffects(void);
qboolean CG_AtmosphericKludge(void);

// NIGHTZ
void CG_DrawScreenFlash (void);
void CG_SetShake (int factor);
void CG_ShakeScreen (vec3_t point, vec3_t player);

//
// cg_main.c
//
const char *CG_ConfigString( int index );
const char *CG_Argv( int arg );

void QDECL CG_Printf( char *icon, const char *msg, ... );
void QDECL CG_Error( const char *msg, ... );

void CG_StartMusic( void );

void CG_UpdateCvars( void );

int CG_CrosshairPlayer( void );
int CG_LastAttacker( void );
void CG_LoadMenus(const char *menuFile);
void CG_KeyEvent(int key, qboolean down);
void CG_MouseEvent(int x, int y);
void CG_EventHandling(int type);
void CG_RankRunFrame( void );
void CG_SetScoreSelection(void *menu);
score_t *CG_GetSelectedScore(void);
void CG_BuildSpectatorString(void);
void CG_RemoveConsoleLine( void );
void CG_TAUIConsole( char *icon, const char *text );
void CG_Log( const char *argument, ... );

//
// cg_view.c
//
void CG_TestModel_f (void);
void CG_TestGun_f (void);
void CG_TestModelNextFrame_f (void);
void CG_TestModelPrevFrame_f (void);
void CG_TestModelNextSkin_f (void);
void CG_TestModelPrevSkin_f (void);
void CG_ZoomDown_f( void );
void CG_ZoomUp_f( void );
void CG_AddBufferedSound( sfxHandle_t sfx);

void CG_DrawActiveFrame( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback );


//
// cg_drawtools.c
//
void CG_AdjustFrom640( float *x, float *y, float *w, float *h );
void CG_FillRect( float x, float y, float width, float height, const float *color );
void CG_DrawPic( float x, float y, float width, float height, qhandle_t hShader );
void CG_DrawString( float x, float y, const char *string,
                    float charWidth, float charHeight, const float *modulate );


void CG_DrawStringExt( int x, int y, const char *string, const float *setColor,
                       qboolean forceColor, qboolean shadow, int charWidth, int charHeight, int maxChars );
void CG_DrawBigString( int x, int y, const char *s, float alpha );
void CG_DrawBigStringColor( int x, int y, const char *s, vec4_t color );
void CG_DrawSmallString( int x, int y, const char *s, float alpha );
void CG_DrawSmallStringColor( int x, int y, const char *s, vec4_t color );

int CG_DrawStrlen( const char *str );

float	*CG_FadeColor( int startMsec, int totalMsec );
float *CG_TeamColor( int team );
void CG_TileClear( void );
void CG_ColorForHealth( vec4_t hcolor );
void CG_GetColorForHealth( int health, int armor, vec4_t hcolor );

void UI_DrawProportionalString( int x, int y, const char* str, int style, vec4_t color );
void CG_DrawRect( float x, float y, float width, float height, float size, const float *color );
void CG_DrawSides(float x, float y, float w, float h, float size);
void CG_DrawTopBottom(float x, float y, float w, float h, float size);


//
// cg_draw.c
//
extern	int sortedTeamPlayers[TEAM_MAXOVERLAY];
extern	int	numSortedTeamPlayers;
extern	int drawTeamOverlayModificationCount;
extern  char systemChat[256];
extern  char teamChat1[256];
extern  char teamChat2[256];

void CG_AddLagometerFrameInfo( void );
void CG_AddLagometerSnapshotInfo( snapshot_t *snap );
void CG_CenterPrint( const char *str, int y, int charWidth );
void CG_DrawHead( float x, float y, float w, float h, int clientNum, vec3_t headAngles );
void CG_DrawActive( stereoFrame_t stereoView );
void CG_DrawFlagModel( float x, float y, float w, float h, int team, qboolean force2D );
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team );
void CG_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle);
void CG_Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style);
int CG_Text_Width(const char *text, float scale, int limit);
int CG_Text_Height(const char *text, float scale, int limit);
void CG_SelectPrevPlayer(void);
void CG_SelectNextPlayer(void);
float CG_GetValue(int ownerDraw);
qboolean CG_OwnerDrawVisible(int flags);
void CG_RunMenuScript(char **args);
void CG_ShowResponseHead(void);
void CG_SetPrintString(int type, const char *p);
void CG_InitTeamChat(void);
void CG_GetTeamColor(vec4_t *color);
const char *CG_GetGameStatusText(void);
const char *CG_GetKillerText(void);
void CG_Draw3DModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles );
void CG_Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader);
void CG_CheckOrderPending(void);
const char *CG_GameTypeString(void);
qboolean CG_YourTeamHasFlag(void);
qboolean CG_OtherTeamHasFlag(void);
qhandle_t CG_StatusHandle(int task);
void CG_ScanForCrosshairEntity( void );
void  CG_DrawLoadingScreen( void );
void  CG_UpdateMediaFraction( float newFract );
void  CG_UpdateSoundFraction( float newFract );
void  CG_UpdateGraphicFraction( float newFract );
void CG_SetScreenFlash (int screenFlashID);

//
// cg_player.c
//
void CG_Player( centity_t *cent );
void CG_ResetPlayerEntity( centity_t *cent );
void CG_AddRefEntityWithPowerups( refEntity_t *ent, entityState_t *state, int team );
void CG_NewClientInfo( int clientNum );
sfxHandle_t	CG_CustomSound( int clientNum, const char *soundName );
int	 CG_ClassforPlayer( char *modelName, char *skinName );
void CG_RunLerpFrameNPC( animation_t *ai, lerpFrame_t *lf, int newAnimation, float speedScale );
void CG_ClearLerpFrameNPC( animation_t *ai, lerpFrame_t *lf, int animationNumber ) ;
void CG_GroundFX( centity_t *cent );

//
// cg_predict.c
//
void CG_BuildSolidList( void );
int	CG_PointContents( const vec3_t point, int passEntityNum );
void CG_Trace( trace_t *result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end,
               int skipNumber, int mask );
void CG_PredictPlayerState( void );
void CG_LoadDeferredPlayers( void );


//
// cg_events.c
//
void CG_CheckEvents( centity_t *cent );
const char	*CG_PlaceString( int rank );
void CG_EntityEvent( centity_t *cent, vec3_t position );
void CG_PainEvent( centity_t *cent, int health );


//
// cg_ents.c
//
void CG_SetEntitySoundPosition( centity_t *cent );
void CG_AddPacketEntities( void );
void CG_Beam( centity_t *cent );
void CG_AdjustPositionForMover( const vec3_t in, int moverNum, int fromTime, int toTime, vec3_t out );

void CG_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
                             qhandle_t parentModel, char *tagName );
void CG_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
                                    qhandle_t parentModel, char *tagName );



//
// cg_weapons.c
//
void CG_NextWeapon_f( void );
void CG_PrevWeapon_f( void );
void CG_Weapon_f( void );
void CG_ExplosionParticles( int weapon, vec3_t origin );
void CG_RegisterWeapon( int weaponNum, int chargelevel );
void CG_RegisterItemVisuals( int itemNum );

void CG_FireWeapon( centity_t *cent );
void CG_FireOffhand( centity_t *cent );
void CG_MissileHitWall( int weapon, int clientNum, vec3_t origin, vec3_t dir, impactSound_t soundType );
void CG_MissileHitPlayer( int weapon, vec3_t origin, vec3_t dir, int entityNum );
void CG_Bullet( vec3_t origin, int sourceEntityNum, vec3_t normal, qboolean flesh, int fleshEntityNum );

void CG_RailTrail( clientInfo_t *ci, vec3_t start, vec3_t end );
void CG_GrappleTrail( centity_t *ent, const weaponInfo_t *wi );
void CG_AddViewWeapon (playerState_t *ps);
void CG_AddPlayerWeapon( refEntity_t *parent, playerState_t *ps, centity_t *cent, int team );
void CG_DrawWeaponSelect( void );

void CG_OutOfAmmoChange( void );	// should this be in pmove?

//
// cg_marks.c
//
void	CG_InitMarkPolys( void );
void	CG_AddMarks( void );
void	CG_ImpactMark( qhandle_t markShader,
                       const vec3_t origin, const vec3_t dir,
                       float orientation,
                       float r, float g, float b, float a,
                       qboolean alphaFade,
                       float radius, qboolean temporary );

//
// cg_localents.c
//
void	CG_InitLocalEntities( void );
localEntity_t	*CG_AllocLocalEntity( void );
void	CG_AddLocalEntities( void );

//
// cg_effects.c
//
void CG_BlackBars(void);
localEntity_t *CG_SmokePuff( const vec3_t p, const vec3_t vel, float radius, float r, float g,
                             float b, float a, float duration, int startTime, int fadeInTime,
                             int leFlags, qhandle_t hShader );
localEntity_t *CG_GravityTrail( const vec3_t p, const vec3_t vel, float radius, float r, float g,
                                float b, float a, float duration, int startTime, int fadeInTime,
                                int leFlags, qhandle_t hShader );
void CG_BubbleTrail( vec3_t start, vec3_t end, float spacing );
void CG_SpawnEffect ( centity_t *cent, vec3_t org );
void CG_Lightning_Discharge (vec3_t origin, int msec);
void CG_ScorePlum( int client, vec3_t org, int score );


localEntity_t *CG_MakeExplosion( vec3_t origin, vec3_t dir,
                                 qhandle_t hModel, qhandle_t shader, int msec,
                                 qboolean isSprite );
void CG_Earthquake(void);
void CG_StartEarthquake(int intensity,int duration);
void CG_SkyLightning( vec3_t end, int nodes, float size, float nodes_area, float fork_length, float fadeout );
localEntity_t *CG_MakeSphericalExplosion( vec3_t origin, int radius, qhandle_t hModel, qhandle_t shader, int leFlags, int msec );
localEntity_t *CG_DrawModel( vec3_t origin, int scale, qhandle_t hModel, qhandle_t shader, int leFlags, int msec );
void CG_DeathSpheres( vec3_t origin, int playerclass );
localEntity_t* CG_GravityWell( const vec3_t p );
void CG_BreakGlass( vec3_t playerOrigin );

//
// cg_snapshot.c
//
void CG_ProcessSnapshots( void );

//
// cg_info.c
//
void CG_LoadingString( const char *s );
void CG_LoadingItem( int itemNum );
void CG_LoadingClient( int clientNum );
void CG_DrawInformation( void );


//
// cg_consolecmds.c
//
qboolean CG_ConsoleCommand( void );
void CG_InitConsoleCommands( void );

//
// cg_servercmds.c
//
void CG_ExecuteNewServerCommands( int latestSequence );
void CG_ParseServerinfo( void );
void CG_SetConfigValues( void );
void CG_ShaderStateChanged(void);

//
// cg_playerstate.c
//
void CG_Respawn( void );
void CG_TransitionPlayerState( playerState_t *ps, playerState_t *ops );
void CG_CheckChangedPredictableEvents( playerState_t *ps );

//
// cg_npcs.c
//
void CG_NPC( centity_t *cent );
void CG_RegisterNPCVisuals( int itemNum );

//===============================================

//
// system traps
// These functions are how the cgame communicates with the main game system
//

// print message on the local console
void		trap_Print( const char *fmt );

// abort the game
void		trap_Error( const char *fmt );

// milliseconds should only be used for performance tuning, never
// for anything game related.  Get time from the CG_DrawActiveFrame parameter
int			trap_Milliseconds( void );

// console variable interaction
void		trap_Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
void		trap_Cvar_Update( vmCvar_t *vmCvar );
void		trap_Cvar_Set( const char *var_name, const char *value );
void		trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

// ServerCommand and ConsoleCommand parameter access
int			trap_Argc( void );
void		trap_Argv( int n, char *buffer, int bufferLength );
void		trap_Args( char *buffer, int bufferLength );

// filesystem access
// returns length of file
int			trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void		trap_FS_Read( void *buffer, int len, fileHandle_t f );
void		trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void		trap_FS_FCloseFile( fileHandle_t f );
int			trap_FS_Seek( fileHandle_t f, long offset, int origin ); // fsOrigin_t

// add commands to the local console as if they were typed in
// for map changing, etc.  The command is not executed immediately,
// but will be executed in order the next time console commands
// are processed
void		trap_SendConsoleCommand( const char *text );

// register a command name so the console can perform command completion.
// FIXME: replace this with a normal console command "defineCommand"?
void		trap_AddCommand( const char *cmdName );

// send a string to the server over the network
void		trap_SendClientCommand( const char *s );

// force a screen update, only used during gamestate load
void		trap_UpdateScreen( void );

// model collision
void		trap_CM_LoadMap( const char *mapname );
int			trap_CM_NumInlineModels( void );
clipHandle_t trap_CM_InlineModel( int index );		// 0 = world, 1+ = bmodels
clipHandle_t trap_CM_TempBoxModel( const vec3_t mins, const vec3_t maxs );
int			trap_CM_PointContents( const vec3_t p, clipHandle_t model );
int			trap_CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles );
void		trap_CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
                              const vec3_t mins, const vec3_t maxs,
                              clipHandle_t model, int brushmask );
void		trap_CM_TransformedBoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
        const vec3_t mins, const vec3_t maxs,
        clipHandle_t model, int brushmask,
        const vec3_t origin, const vec3_t angles );

// Returns the projection of a polygon onto the solid brushes in the world
int			trap_CM_MarkFragments( int numPoints, const vec3_t *points,
                                   const vec3_t projection,
                                   int maxPoints, vec3_t pointBuffer,
                                   int maxFragments, markFragment_t *fragmentBuffer );

// normal sounds will have their volume dynamically changed as their entity
// moves and the listener moves
void		trap_S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx );
void		trap_S_StopLoopingSound(int entnum);

// a local sound is always played full volume
void		trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum );
void		trap_S_ClearLoopingSounds( qboolean killall );
void		trap_S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx );
void		trap_S_AddRealLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx );
void		trap_S_UpdateEntityPosition( int entityNum, const vec3_t origin );

// respatialize recalculates the volumes of sound as they should be heard by the
// given entityNum and position
void		trap_S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater );
sfxHandle_t	trap_S_RegisterSound( const char *sample, qboolean compressed );		// returns buzz if not found
void		trap_S_StartBackgroundTrack( const char *intro, const char *loop );	// empty name stops music
void		trap_S_StopBackgroundTrack( void );


void		trap_R_LoadWorldMap( const char *mapname );

// all media should be registered during level startup to prevent
// hitches during gameplay
qhandle_t	trap_R_RegisterModel( const char *name );			// returns rgb axis if not found
qhandle_t	trap_R_RegisterSkin( const char *name );			// returns all white if not found
qhandle_t	trap_R_RegisterShader( const char *name );			// returns all white if not found
qhandle_t	trap_R_RegisterShaderNoMip( const char *name );			// returns all white if not found

// a scene is built up by calls to R_ClearScene and the various R_Add functions.
// Nothing is drawn until R_RenderScene is called.
void		trap_R_ClearScene( void );
void		trap_R_AddRefEntityToScene( const refEntity_t *re );

// polys are intended for simple wall marks, not really for doing
// significant construction
void		trap_R_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts );
void		trap_R_AddPolysToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts, int numPolys );
void		trap_R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b );
int			trap_R_LightForPoint( vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir );
void		trap_R_RenderScene( const refdef_t *fd );
void		trap_R_SetColor( const float *rgba );	// NULL = 1,1,1,1
void		trap_R_DrawStretchPic( float x, float y, float w, float h,
                                   float s1, float t1, float s2, float t2, qhandle_t hShader );
void		trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );
int			trap_R_LerpTag( orientation_t *tag, clipHandle_t mod, int startFrame, int endFrame,
                            float frac, const char *tagName );
void		trap_R_RemapShader( const char *oldShader, const char *newShader, const char *timeOffset );

// The glconfig_t will not change during the life of a cgame.
// If it needs to change, the entire cgame will be restarted, because
// all the qhandle_t are then invalid.
void		trap_GetGlconfig( glconfig_t *glconfig );

// the gamestate should be grabbed at startup, and whenever a
// configstring changes
void		trap_GetGameState( gameState_t *gamestate );

// cgame will poll each frame to see if a newer snapshot has arrived
// that it is interested in.  The time is returned seperately so that
// snapshot latency can be calculated.
void		trap_GetCurrentSnapshotNumber( int *snapshotNumber, int *serverTime );

// a snapshot get can fail if the snapshot (or the entties it holds) is so
// old that it has fallen out of the client system queue
qboolean	trap_GetSnapshot( int snapshotNumber, snapshot_t *snapshot );

// retrieve a text command from the server stream
// the current snapshot will hold the number of the most recent command
// qfalse can be returned if the client system handled the command
// argc() / argv() can be used to examine the parameters of the command
qboolean	trap_GetServerCommand( int serverCommandNumber );

// returns the most recent command number that can be passed to GetUserCmd
// this will always be at least one higher than the number in the current
// snapshot, and it may be quite a few higher if it is a fast computer on
// a lagged connection
int			trap_GetCurrentCmdNumber( void );

qboolean	trap_GetUserCmd( int cmdNumber, usercmd_t *ucmd );

// used for the weapon select and zoom
void		trap_SetUserCmdValue( int stateValue, float sensitivityScale );

// aids for VM testing
void		testPrintInt( char *string, int i );
void		testPrintFloat( char *string, float f );

int			trap_MemoryRemaining( void );
void		trap_R_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font);
qboolean	trap_Key_IsDown( int keynum );
int			trap_Key_GetCatcher( void );
void		trap_Key_SetCatcher( int catcher );
int			trap_Key_GetKey( const char *binding );


typedef enum
{
    SYSTEM_PRINT,
    CHAT_PRINT,
    TEAMCHAT_PRINT
} q3print_t; // bk001201 - warning: useless keyword or type name in empty declaration


int trap_CIN_PlayCinematic( const char *arg0, int xpos, int ypos, int width, int height, int bits);
e_status trap_CIN_StopCinematic(int handle);
e_status trap_CIN_RunCinematic (int handle);
void trap_CIN_DrawCinematic (int handle);
void trap_CIN_SetExtents (int handle, int x, int y, int w, int h);
void trap_CG_SetActiveMenu (uiMenuCommand_t menu);

void trap_SnapVector( float *v );

qboolean	trap_loadCamera(const char *name);
void		trap_startCamera(int time);
qboolean	trap_getCameraInfo(int time, vec3_t *origin, vec3_t *angles);

qboolean	trap_GetEntityToken( char *buffer, int bufferSize );

void	CG_ClearParticles (void);
void	CG_AddParticles (void);
void	CG_ParticleSnow (qhandle_t pshader, vec3_t origin, vec3_t origin2, int turb, float range, int snum);
void	CG_ParticleSmoke (qhandle_t pshader, centity_t *cent);
void	CG_AddParticleShrapnel (localEntity_t *le);
void	CG_ParticleSnowFlurry (qhandle_t pshader, centity_t *cent);
void	CG_ParticleBulletDebris (vec3_t	org, vec3_t vel, int duration);
void	CG_ParticleSparks (vec3_t org, vec3_t vel, int duration, float x, float y, float speed);
void	CG_ParticleDust (centity_t *cent, vec3_t origin, vec3_t dir);
void	CG_ParticleMisc (qhandle_t pshader, vec3_t origin, int size, int duration, float alpha);
void	CG_ParticleExplosion (char *animStr, vec3_t origin, vec3_t vel, int duration, int sizeStart, int sizeEnd);
extern qboolean		initparticles;
int		CG_NewParticleArea ( int num );



