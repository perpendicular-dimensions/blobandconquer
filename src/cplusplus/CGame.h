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

#include "../attributes.h"

struct InfoMessage {

	bool redraw;
	String message;
	int type;
	float life;

};

class Game {

	private:
		
		static Game instance;
		
		Engine *engine;
		Mouse *mouse;
		
	Game();

	public:
		
		bool inGame;
		bool training;
		bool doingPuzzle;
		
		int weaponIndex;
		Weapon *selectedWeapon;
		Weapon *weapon[4];
		int weaponId[4];
		
		Item *item[5];
		int itemId[5];
		
		int teekaWeapon;
				
		int currentOrders;
		
		bool dropGrappleHooks;
		
		float comboTimer;
		int comboHits;

		Properties statistics;
		
		String mission;
		String targetMission;
		String targetLocation;
		
		InfoMessage infoMessage[4];
		InfoMessage gameplayMessage;
		String lastGamePlayMessage;
		
		int keyboard[CTRL_MAXIMUM];
		int joystick[CTRL_MAXIMUM];
		int mouseCtrl[CTRL_MAXIMUM];
		
		int joystickDeadZone;
		int mouseSensitivity;
		
		float hours, minutes, seconds, totalTime;
		
		float remainingMinutes, remainingSeconds, totalRemaining;
		float timeUpExplosionTime, whiteOutTime;
		
		bool canSaveGame;
		int saveSlot;
		
		int decalPolicy;
		int shadowPolicy;
		int bloodPolicy;
		int motionBlur;
		
		bool cheatsEnabled;
		
		bool changingLevel;
		bool retrying;
		
		float cameraHeight, cameraRadius, cameraTempHeight;
		float cameraDirRadian, cameraAdjustedRadian, desiredCameraRadius;
		float cameraDirDegrees;
		float prevCameraRadius;
		Vector cameraDir;
		float cameraAimHeight;
		
		Vector checkpoint;
		bool mustChangeWeapon;
		
		float autoFaceTimer;
		bool autoLockTarget;
		Vector targetLock;
		GLColor aimColor;
		
		int missionStatus;
		float missionOverTimer;
		int cutsceneType;
		
		// -------- Cheats... Try not to abuse these, guys... -----------
		bool cheatHealth;
		bool cheatShield;
		bool cheatAmmo;
		bool cheatStun;
		bool cheatNPC;
		bool cheatClip;
		bool cheatSave;
		bool cheatBlind;
		bool cheatOxygen;
		bool cheatItems;
		bool cheatDesimilate;
		bool cheatTime;
		
		//--------- Debugging Stuff. See main.cpp for usages -----------
		bool nomonsters;
		bool noitems;
		bool nofeatures;
		bool allStatic;
		bool cheatsActive;

	~Game();
	static Game *getInstance();
	
	void clear();
	void destroy();
	
	void setDefaultKeyboardControls();

	void alterStat(const char *statName, float value);
	float getStatistic(const char *name);
	
	void setMissionStatus(int status);
	
	void setInfoMessage(int type, const char *message, ...) GCC_PRINTF(3,4);
	void setGamePlayMessage(int type, const char *message, ...) GCC_PRINTF(3,4);
	void setGamePlayMessage(int type, int time, const char *message, ...) GCC_PRINTF(4,5);
	
	int getFreeInventorySlot();
	int getItemSlot(const char *name);
	bool hasItem(const char *name);
	int removeItem(const char *name);
	
	int getSelectedWeaponSlot();
	
	void changeCameraRotation(float r);
	
	void calculateTime(float total, float *hours, float *minutes, float *seconds);
	
	void incTime(float seconds);
	void decTimeLimit(float secs);
	
	void doComboTimer(float td);
	
	bool isControl(int control);
	bool isAnyControl();
	void resetControl(int control);
	
	void load(Properties *props);
	void save(FILE *fp);

};
