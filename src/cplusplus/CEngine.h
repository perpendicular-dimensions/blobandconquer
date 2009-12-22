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

class Engine {

	private:

		static Engine instance;

		SDL_Event event;

		// Time Difference
		unsigned int time1;
		unsigned int time2;
		float timeDifference;
		float inertiaRate;

		Pak *pak;
		
		long debugTimerStart;

	Engine();

	public:
		
		bool slowLogic;

		char keyState[350];
		char *buttonState;
		Mouse *mouse;
		int axis1X, axis1Y, axis2X, axis2Y;
		bool swapSecondStickAxis;

		String userHomeDirectory;
		
		int joystickIndex;
		SDL_Joystick *joystick;

		List defineList;
		
		int lastReadDataSize;
		
		SaveGameHeader saveSlot[MAX_SAVE_SLOTS];
		
		// just some dev stuff
		bool showFPS;
		bool showPosition;
		
		char lastKeyEvents[35];

	static Engine *getInstance();
	~Engine();
	
	void startDebugTimer(const char *text);
	void endDebugTimer(const char *text);

	void getInput();
	void fireMousePressed(SDL_MouseButtonEvent mouse);
	void fireMouseReleased(SDL_MouseButtonEvent mouse);
	void fireKeyPressed(SDL_KeyboardEvent *key);
	void fireMouseMoved();
	void fireJoystickButtonPressed(int key);

	bool userAccepts();

	void doPause();
	void addKeyEvent(const char *key);
	void flushInput();
	void clearInput();
	
	void reportFontFailure();
	
	void doTimeDifference();
	float getTimeDifference(int tdType);
	float getInertiaRate();
	void resetTimeDifference();
	
	bool loadDefines();
	int getValueOfDefine(const char *word);
	const char *getDefineOfValue(const char *prefix, int value);
	int getValueOfFlagTokens(const char *line);
	
	void saveSaveGameHeaders();
};
