/* ########### widgets ############ */

#define BUTTON			0
#define SLIDER			1
#define LABEL 			2
#define LISTVIEW		3
#define CHECKBOX		4
#define PANEL			5
#define COMBOBOX		6
#define INPUT			7

/* ############## input types ########### */

#define INPUT_KEY		0
#define INPUT_JOY		1
#define INPUT_MOUSE		2

/* ############## content ############ */

#define	CONTENTS_AIR			0
#define	CONTENTS_SOLID			1
#define	CONTENTS_WINDOW			2
#define	CONTENTS_AUX			4
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_MIST			64
#define CONTENTS_AREAPORTAL    	0x8000
#define CONTENTS_PLAYERCLIP    	0x10000
#define CONTENTS_MONSTERCLIP   	0x20000
#define CONTENTS_TELEPORTER    	0x40000
#define CONTENTS_JUMPPAD       	0x80000
#define CONTENTS_CLUSTERPORTAL 	0x100000
#define CONTENTS_DONOTENTER    	0x200000
#define CONTENTS_ORIGIN        	0x1000000
#define CONTENTS_BODY          	0x2000000
#define CONTENTS_CORPSE        	0x4000000
#define CONTENTS_DETAIL        	0x8000000
#define CONTENTS_STRUCTURAL    	0x10000000
#define CONTENTS_TRANSLUCENT   	0x20000000
#define CONTENTS_TRIGGER       	0x40000000
#define CONTENTS_NODROP        	0x80000000

/* ############# missions ############ */

#define OBJ_PRIORITY_PRIMARY		0
#define OBJ_PRIORITY_SECONDARY		1
#define OBJ_PRIORITY_CONDITION		2
#define OBJ_PRIORITY_HIDDEN			3

#define OBJ_INCOMPLETE			0
#define OBJ_INPROGRESS			1
#define OBJ_COMPLETE			2
#define OBJ_FAILED				3

/* ############# triggers ############## */

#define TRIGGER_TYPE_ANY					-2
#define TRIGGER_TYPE_NONE					-1
#define TRIGGER_TYPE_LOCATION				0
#define TRIGGER_TYPE_ENTITY_KILLED			1
#define TRIGGER_TYPE_ITEM_PICKED_UP			2
#define TRIGGER_TYPE_MISSION_COMPLETE		3
#define TRIGGER_TYPE_BLOB_RESCUED			4
#define TRIGGER_TYPE_ALL_ENEMIES_KILLED		5
#define TRIGGER_TYPE_ENEMY_DESIMILATED		6

#define TRIGGER_ACTION_ENABLE_FEATURE		0
#define TRIGGER_ACTION_DISABLE_FEATURE		1
#define TRIGGER_ACTION_REMOVE_FEATURE		2
#define TRIGGER_ACTION_EXPLOSION			3
#define TRIGGER_ACTION_COMPLETE_OBJECTIVE	4
#define TRIGGER_ACTION_ENABLE_OBJECTIVE		5
#define TRIGGER_ACTION_FAIL_OBJECTIVE		6
#define TRIGGER_ACTION_UPDATE_OBJECTIVE		7
#define TRIGGER_ACTION_TOGGLE_STRUCTURE		8
#define TRIGGER_ACTION_USE_CAMERA			9
#define TRIGGER_ACTION_TELEPORT_BLOB		10
#define TRIGGER_ACTION_EXIT_MISSION			11
#define TRIGGER_ACTION_START_TIME_LIMIT		12
#define TRIGGER_ACTION_STOP_TIME_LIMIT		13
#define TRIGGER_ACTION_FUNCTION_POINTER		14
#define TRIGGER_ACTION_DESTROY_STRUCTURE	15
#define TRIGGER_ACTION_STOP_ALARM			16

/* ############# light types ########### */

#define LT_NORMAL			0
#define LT_PULSE			1
#define LT_FLICKER			2

/* ############ Doors ################# */

#define DOOR_OPEN			0
#define DOOR_CLOSED			1
#define DOOR_OPENING		2
#define DOOR_CLOSING		3
#define DOOR_LOCKED			4

/* ########### particles ########## */

#define PT_NORMAL		0
#define PT_SPARK		1
#define PT_HALO			2

/* ############# weather ################ */

#define WEATHER_NORMAL			0
#define WEATHER_RAIN			1
#define WEATHER_LIGHT_RAIN		2
#define WEATHER_HEAVY_RAIN		3
#define WEATHER_STORM			4
#define WEATHER_FOG				5
#define WEATHER_THIN_FOG		6
#define WEATHER_THICK_FOG		7

/* ############ entities ######### */

#define ET_UNDEFINED		-1
#define ET_BLOB				0
#define ET_BIOMECH			1
#define ET_NEUTRAL			2
#define ET_WEAPON			3
#define ET_ITEM				4
#define ET_BULLET			5
#define ET_FEATURE			6
#define ET_DECORATION		7
#define ET_DOOR				8
#define ET_WINDOW			9
#define ET_SWITCH			10
#define ET_BARRIER			11
#define ET_STRUCTURE		12
#define ET_WEAK_WALL		13
#define ET_LIFT				14
#define ET_PUSHBLOCK		15
#define ET_AUTOLIFT			16
#define ET_EMITTER			17
#define ET_WEAK_STRUCTURE	18
#define ET_TRAP				19
#define ET_SUPPLY_CRATE		20
#define ET_BOSS				21
#define ET_CAMERA			22

/* ############# entity flags ########### */

#define EF_BOB					1
#define EF_NOMOVE				( 2 << 0 )
#define EF_STATIC				( 2 << 1 )
#define EF_NOFIRE				( 2 << 2 )
#define EF_DYING				( 2 << 3 )
#define EF_DEAD					( 2 << 4 )
#define EF_WEIGHTLESS			( 2 << 5 )
#define EF_TELEPORTING			( 2 << 6 )
#define EF_IMMORTAL				( 2 << 7 )
#define EF_EXPLODES				( 2 << 8 )
#define EF_BOUNCES				( 2 << 9 )
#define EF_ONGROUND				( 2 << 10 )
#define EF_NOCLIP				( 2 << 11 )
#define EF_AUTO_PICKUP			( 2 << 12 )
#define EF_NO_TARGET			( 2 << 13 )
#define EF_CANNOT_BE_KILLED		( 2 << 14 )
#define EF_VANISHED				( 2 << 15 )
#define EF_NOTARGET				( 2 << 16 )
#define EF_PUSHABLE				( 2 << 17 )
#define EF_NOWORLDCLIP			( 2 << 18 )
#define EF_FROZEN				( 2 << 19 )
#define EF_SWIMS				( 2 << 20 )
#define EF_ASSIMILATING			( 2 << 21 )
#define EF_EXITTED				( 2 << 22 )
#define EF_ONFIRE				( 2 << 23 )
#define EF_WRAPPED				( 2 << 24 )
#define EF_GRAPPLING			( 2 << 25 )
#define EF_ALWAYS_FACE			( 2 << 26 )
#define EF_HAS_TURRET			( 2 << 27 )
#define EF_NO_MD2_LOOP			( 2 << 28 )
#define EF_BLIND				( 2 << 29 )

/* ############## blob jobs ########### */

#define BLOB_BOB			0
#define BLOB_TEEKA			1
#define BLOB_MIA			2
#define BLOB_SOLDIER		3
#define BLOB_DEMOLITION		4
#define BLOB_ENGINEER		5
#define BLOB_HACKER			6
#define BLOB_ASSIMILATING	7

/* ############## enemies ############# */

#define NME_PISTOL_BLOB				0
#define NME_GRENADE_BLOB			1
#define NME_MACHINEGUN_BLOB			2
#define NME_SHOTGUN_BLOB			3
#define NME_PISTOL_DROID			4
#define NME_GRENADE_DROID			5
#define NME_MACHINEGUN_DROID		6
#define NME_SHOTGUN_DROID			7
#define NME_LIGHT_PLASMA_BLOB		8
#define NME_HEAVY_PLASMA_BLOB		9
#define NME_HEAVY_PLASMA_DROID		10
#define NME_LASER_BLOB				11
#define NME_LASER_DROID				12
#define NME_MINI_MINE_DROID			13
#define NME_ROCKET_DROID			14
#define NME_ROCKET_BLOB				15
#define NME_ICEGUN_BLOB				16
#define NME_ICEGUN_DROID			17
#define NME_DARK_BLOB				18
#define NME_BLACK_DROID				19
#define NME_UZI_SENTRY_GUN			20
#define NME_PLASMA_SENTRY_GUN		21
#define NME_ROCKET_SENTRY_GUN		22
#define NME_MORTOR_SENTRY_GUN		23
#define NME_AQUA_BLOB				24
#define NME_MACHINEGUN_SPIDERBLOB	25
#define NME_SHOTGUN_SPIDERBLOB		26
#define NME_PLASMA_SPIDERBLOB		27
#define NME_BABY_SPIDERBLOB			28
#define NME_DEAD_SENTRYGUN			29
#define NME_TANKBAY_TANK_1			30
#define NME_TANKBAY_TANK_2			31

/* ############# shield types ############## */

#define SHIELD_NONE				0
#define SHIELD_NORMAL			1
#define SHIELD_CONVENTIONAL		2
#define	SHIELD_ENERGY			3

/* ########### damage types ########### */

#define DAMAGE_EXPLOSION		1
#define DAMAGE_CONVENTENIAL		2
#define DAMAGE_ENERGY			3
#define DAMAGE_SPECIAL			4

/* ############## special features ######### */

#define FEATURE_SPAWNER				0
#define FEATURE_TELEPORTER			1
#define FEATURE_EXIT				2
#define FEATURE_SAVE_POINT			3
#define FEATURE_INFORMATION			4
#define FEATURE_GRAPPLE_POINT		5

/* ############## switches ######### */

#define SWT_NORMAL				0
#define SWT_TIMED				1
#define SWT_TOGGLE				2
#define SWT_STATIC				3
#define SWT_PRESSURE			4
#define SWT_RESET				5
#define SWITCH_ENGINEER			6
#define SWITCH_HACKER			7
#define SWITCH_DEMOLITION		8

/* ############## texture spec movement ######## */

#define TSM_NORMAL		0
#define TSM_SIN			1
#define TSM_COS			2

/* ############## decoration ############## */

#define DT_FLESH		1
#define DT_FIRE			2
#define DT_SHARD		3
#define DT_GENERAL		4

/* ############### emitters ###################### */

#define EMITTER_FIRE		0
#define EMITTER_SMOKE		1
#define EMITTER_SPARKS		2
#define EMITTER_COLOR		3

/* ################ traps ####################### */

#define TRAP_LASER			0

/* ############## sounds ############## */

enum {
	
	SND_BUTTONCLICK,
	SND_BUTTONBEEP,
	SND_PISTOL,
	SND_RICO1,
	SND_RICO2,
	SND_DEATH1,
	SND_DEATH2,
	SND_DEATH3,
	SND_DEATH4,
	SND_HIT,
	SND_SPLAT,
	SND_SPLAT_UNDERWATER,
	SND_PLASMA,
	SND_PLASMA_HIT,
	SND_GRENADE_EXPLODE,
	SND_ARM1,
	SND_ARM2,
	SND_ARM3,
	SND_HEAVY_PLASMA,
	SND_ROCKET_LAUNCHER,
	SND_PISTOL_RELOAD,
	SND_SILENCED_PISTOL,
	SND_RIFLE,
	SND_UZI,
	SND_TELEPORT1,
	SND_TELEPORT2,
	SND_TELEPORT3,
	SND_PICKUP_WEAPON,
	SND_WEAPON_EMPTY,
	SND_GULP,
	SND_ITEM,
	SND_THROW,
	SND_METAL_BOUNCE,
	SND_THROW1,
	SND_THROW2,
	SND_DROID_DEATH1,
	SND_DROID_DEATH2,
	SND_DROID_DEATH3,
	SND_DROID_DEATH4,
	SND_WATER_IN,
	SND_WATER_OUT,
	SND_WATER_SWIM,
	SND_SLIME_IN,
	SND_LAVA_IN,
	SND_WEATHER,
	SND_DOOR_MOVE_START,
	SND_DOOR_MOVE_END,
	SND_GLASS_SHATTER_1,
	SND_GLASS_SHATTER_2,
	SND_GLASS_SHATTER_3,
	SND_BARRIER_SHUT_DOWN,
	SND_CLANG,
	SND_ELECTRIC_SPARK1,
	SND_ELECTRIC_SPARK2,
	SND_ELECTRIC_SPARK3,
	SND_ELECTRIC_SPARK4,
	SND_ELECTRIC_SPARK5,
	SND_BULLET_HIT_WATER,
	SND_SWITCH_BLEEP,
	SND_SWITCH_CLICK_1,
	SND_SWITCH_CLICK_2,
	SND_SWITCH_CLICK_3,
	SND_DROWNING,
	SND_GRENADE_LAUNCHER,
	SND_ICE_GUN,
	SND_LARGE_EXPLOSION,
	SND_GRAPPLE_FIRE,
	SND_GRAPPLE_HIT,
	SND_GRAPPLING,
	SND_GLASS_HIT_1,
	SND_GLASS_HIT_2,
	SND_TIME_WARNING_1,
	SND_TIME_WARNING_2,
	SND_C4_CLOCK,
	SND_SHIELD_DESTROYED,
	SND_CRATE_DESTROYED,
	SND_LASER,
	SND_FLAMETHROWER,
	SND_DESIMILATOR,
	SND_DESIMILATED,
	SND_ICE_HIT,
	SND_ICE_SHATTER,
	SND_ICE_BLOCK,
	SND_WEB_FIRE,
	SND_WEB_WRAP,
	SND_WEB_HIT,
	SND_SPIDERBLOB_DIE,
	SND_GALDOV_LAUGH,
	SND_GALDOV_ENERGY,
	SND_GALDOV_PAIN,
	SND_GALDOV_CHARGE,
	SND_GALDOV_FIRE,
	SND_PLANT_BOMB,
	SND_POP
};

/* ############### weapons ############ */

#define WEP_PISTOL				0
#define WEP_SHOTGUN				1
#define WEP_UZI					2
#define WEP_PLASMA_PISTOL		3
#define WEP_LASER_PISTOL		4
#define WEP_GRENADE				5
#define WEP_LASER_RIFLE			6
#define WEP_PLASMA_RIFLE		7
#define WEP_MINI_GRENADES		8
#define WEP_GRENADE_LAUNCHER	9
#define WEP_CLUSTER_LAUNCHER	10
#define WEP_ROCKET_LAUNCHER		11
#define WEP_BOMBLET				12
#define WEP_MINI_MINE			13
#define WEP_ICE_GUN				14
#define WEP_DESIMILATOR			15
#define WEP_MINI_ROCKET			16
#define WEP_FLAMETHROWER		17
#define WEP_WEB					18
#define WEP_GRAPPLING_HOOK		19
#define WEP_HOMING_ENERGY		20

#define TRAIL_NONE				0
#define TRAIL_SMALL_PLASMA		1
#define TRAIL_HEAVY_PLASMA		2
#define TRAIL_SMOKE				3
#define TRAIL_SMALL_SMOKE		4
#define TRAIL_FIRE				5
#define TRAIL_ICE				6
#define TRAIL_FLAMETHROWER		7
#define	TRAIL_ENERGY			8
#define	TRAIL_MULTI				9

/* ############# items  ######### */

#define IT_MEDICAL				0
#define IT_POWER_UP				1
#define IT_SPECIAL				2
#define IT_MISC					3

#define ITM_CHERRY				0
#define ITM_MEDIPACK			1
#define ITM_KEYCARD				2
#define ITM_BATTERY				3
#define ITM_IDCARD				4
#define ITM_SHIELD_BELT			5
#define ITM_TEEKA_WEAPON		6
#define ITM_C4					7
#define ITM_BOMB				8
#define ITM_TELETAG				9
#define ITM_GRAPPLE_HOOK		10
#define ITM_STICKY_C4			11

#define ITM_CUSTOM_FIRST		25
#define ITM_CUSTOM_LAST			49
