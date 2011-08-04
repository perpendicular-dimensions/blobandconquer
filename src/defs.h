/*
Copyright (C) 2006 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/* ####### get text ############ */

#define _(string) gettext(string)

/* ########### main misc stuff ######## */

#define PI 3.14159265359

#define EPSILON 0.03125

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define dotProd(v1, v2) ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z))

#define toDegrees(x) ((x * 180) / PI)
#define toRadians(x) ((x * PI) / 180)

#define ERR_FILE 		"A required file (%s) was not found in PAK file"

#define SAVE_FORMAT		"%.5d"

#define MODE_2D		0
#define MODE_3D		1

#define MIN_CAMERA_DISTANCE 100

#define SECTION_NEWGAME			0
#define SECTION_TITLE			1
#define SECTION_CHANGE_LEVEL	2
#define SECTION_BATTLE			3
#define SECTION_TRAINING		4
#define SECTION_GAMEOVER		5
#define SECTION_LOAD			6
#define SECTION_END_GAME		7
#define SECTION_CREDITS			8
#define SECTION_RESTART			9

#define TD_ANIMATION	0
#define TD_LOGIC		1

#define BB_GRID_SIZE	1

#define BOX_SOLID		0
#define BOX_HOLLOW		1
		
#define MAX_STEP_SIZE	3

#define POINT_ON_PLANE 				0
#define POINT_IN_FRONT_OF_PLANE 	1
#define POINT_BEHIND_PLANE 			2

#define RAY 		0
#define SPHERE 		1
#define BOX 		2

#define MDL_RAW			0
#define MDL_PRIMITIVE	1

#define GRAVITY			0.025
#define LIQUID_GRAVITY	0.00025

#define MAX_OXYGEN		1500
#define MAX_OXYGEN_IND	1800

#define FALL_DAMAGE_ACCEL	-3

#define BULLET_MOVE_SPEED 3.5

#define SHADOW_LENGTH	80

#define PLAYER_TARGET_RANGE 451

#define GRID_SIZE	50
    
// if there are anywhere near this number of entities to check against then we've got a major problem...
#define MAX_CANDIDATES	1024

#define MAX_RESOLUTIONS 	15
		
#define CUTSCENE_NONE 		0
#define CUTSCENE_INGAME		1
#define CUTSCENE_MOVIE		2

/* ########### maximums ########### */

#define MAX_SAVE_SLOTS			24
#define MAX_CHANNELS			16

#define MAX_ITEMS				50
#define MAX_WEAPONS 			50
#define MAX_DECORATIONS			35
#define MAX_BLOBS				10
#define MAX_FEATURES			10
#define MAX_ENEMIES				35
#define MAX_ENEMY_GROUPS		25
#define MAX_SOUNDS				100
#define MAX_AMBIENCE			15
#define MAX_FONTS				10
#define MAX_BOSSES				10

#define MAX_MENTAL				300
#define MAX_CRUMBS				512

const char entityNames[][25] = {
	"Blob",
	"BioMech",
	"Neutral",
	"Weapon",
	"Item",
	"Bullet",
	"Feature",
	"Decoration",
	"Door",
	"Window",
	"Switch",
	"Barrier",
	"Structure",
	"WeakWall",
	"Lift",
	"PushBlock",
	"AutoLift",
	"Emitter",
	"WeakStructure",
	"Trap",
	"SupplyCrate",
	"Boss",
	"Camera"
};

/* ########## 3D ################# */

enum frustrumSides
{
	LEFT = 0,
	RIGHT,
	TOP,
	BOTTOM,
	FRONT,
	BACK
};

/* ########### battle ############ */

#define LIGHT_DAWN			0
#define LIGHT_DAY			1
#define LIGHT_DUSK			2
#define LIGHT_NIGHT			3
#define LIGHT_DARKNIGHT		4

#define ITEM_LEAVE			0
#define ITEM_CONSUME		1
#define ITEM_PICKUP			2

#define UNIT_SPECIAL_NONE		0
#define UNIT_SPECIAL_USE		1
#define UNIT_SPECIAL_EXIT		2

/* ########### controls ############ */

enum {

	CTRL_UP = 0,
	CTRL_DOWN,
	CTRL_LEFT,
	CTRL_RIGHT,
	CTRL_JUMP,
	CTRL_FIRE,
	CTRL_CYCLE_WEAPON,
	CTRL_LOCK_TARGET,
	CTRL_CYCLE_TARGET,
	CTRL_ACTION,
	CTRL_ROTATE_LEFT,
	CTRL_ROTATE_RIGHT,
	CTRL_PAUSE,
	CTRL_STATUS,
	CTRL_QUICK_GRENADE,
	CTRL_MAXIMUM // just used to help with loops, etc

};

#define ANALOG_MOVE 		27500
#define PLAYER_ACCEL 		0.12

#define MAX_MOUSE_BUTTONS	10

/* ############## text ############## */

#define TXT_LEFT 		0
#define TXT_CENTERED 	1
#define TXT_RIGHT 		2

#define FONT_NORMAL			0
#define FONT_LARGE			1
#define FONT_DIGITAL		2
#define FONT_LARGE_DIGITAL	3
#define FONT_NUMBER			4

#define INFO_PICKUP			0
#define INFO_BAD			1
#define INFO_GOOD			2
#define INFO_HINT			3
#define INFO_GENERAL		4

/* ########### movement type ########## */

#define MT_NORMAL			0
#define MT_SLIDE			1

/* ########### squad orders ######### */

#define ORDERS_FOLLOW		0
#define ORDERS_RETREAT		1
#define ORDERS_WAIT			2
#define ORDERS_USE			3

/* ########### missions ########## */

#define MISSION_INPROGRESS		0
#define MISSION_COMPLETE		1
#define MISSION_FAILED			2
#define MISSION_PLAYER_OUT		3
#define MISSION_LOADING			4
#define MISSION_QUITTING		5
#define MISSION_RESTARTING		6

/* ################ decal ################# */

#define DECAL_BLOOD				0
#define DECAL_BULLET_MARK		1
#define DECAL_EXPLOSION_MARK	2
#define DECAL_GLASS_IMPACT		3

/* ############# config policies ########### */

#define DECAL_POLICY_OFF		0
#define DECAL_POLICY_LOW		1
#define DECAL_POLICY_MEDIUM		2
#define DECAL_POLICY_ON			3

#define SHADOW_POLICY_OFF		0
#define SHADOW_POLICY_BLOB		1
#define SHADOW_POLICY_UNITS		2
#define SHADOW_POLICY_ON		3

#define BLOOD_POLICY_OFF			0
#define BLOOD_POLICY_NO_STAINS		1
#define BLOOD_POLICY_ON				2

/* ############# sounds ############### */

#define CH_ANY			-1
#define CH_PLAYER		0
#define CH_WEAPON		1
#define CH_PICKUP		2
#define CH_EXPLODE		3
#define CH_DEATH		4
#define CH_SPAWN		5
#define CH_ENVIRONMENT	6
#define CH_AMBIENT		7
#define CH_EFFECT1		8
#define CH_EFFECT2		9
#define CH_EFFECT3		10
#define CH_EFFECT4		11
#define CH_EFFECT5		12
#define CH_EFFECT6		13
#define CH_EFFECT7		14
#define CH_EFFECT8		15

/* ########## puzzles ################ */

#define PUZZLE_STATE_UNFINISHED		0
#define PUZZLE_STATE_COMPLETED		1
#define PUZZLE_STATE_FAILED			2

#define PUZZLE_TYPE_MASTERMIND		0
#define PUZZLE_TYPE_LASERGRID		1

/* ########### pak file stuff ######## */

#define PAK_MAX_FILENAME	60

#ifndef PAKLOCATION
	#define PAKLOCATION ""
#endif

#ifndef PAKNAME
	#define PAKNAME "blobAndConquer.pak"
#endif

#ifndef PAKFULLPATH
	#define PAKFULLPATH PAKLOCATION PAKNAME
#endif

#ifndef PATH_MAX
	#define PATH_MAX 4096
#endif

#ifndef TEMPDIR
	#define TEMPDIR ""
#endif

#ifndef GAMEPLAYMANUAL
	#define GAMEPLAYMANUAL "manual.html"
#endif

#ifndef DEV
	#define DEV 1
#endif

#if DEV
	#undef PAKFULLPATH
	#define PAKFULLPATH PAKNAME
	
	/*
	#ifdef LOCALEDIR
		#undef LOCALEDIR
	#endif
	#define LOCALEDIR "locale/"
	*/
#endif

struct PAK
{
	enum TYPE
	{
		TEXT,
		BINARY
	};
};

/* ############# debug ################## */

#if USEPAK
	#define debug(x)
#else
	#define debug(x) {printf("[DEBUG (%d)] ", SDL_GetTicks()); printf x; fflush(stdout);}
#endif

#ifndef IGNORE_FLAGTOKEN_ERRORS
	#define IGNORE_FLAGTOKEN_ERRORS 0
#endif
