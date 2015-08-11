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

#include "attributes.h"
#include "main.h"

void GCC_NORETURN showHelp()
{
	SDL_Init(SDL_INIT_JOYSTICK|SDL_INIT_VIDEO);
	
	printf("\n");
	printf("Blob Wars, Episode II - Blob And Conquer (Version %.2f, Release %d)\n", VERSION, RELEASE);
	printf("Copyright (C) 2006 Parallel Realities\n");
	printf("Licensed under the GNU General Public License (GPL)\n\n");

	printf("The Blob And Conquer gameplay manual can be found in,\n");
	printf("\t%s\n\n", GAMEPLAYMANUAL);
	
	printf("\t-fullscreen         Start the game in fullscreen mode (default)\n");
	printf("\t-window             Start the game in windowed mode\n");
	
	graphics->calculateScreenModes();

	for (unsigned i = 0 ; i < MAX_RESOLUTIONS ; i++)
	{
		if (graphics->screenMode[i].w == 0)
		{
			continue;
		}
		
		if (i == 0)
		{
			printf("\t-screensize         %d - %d x %d\n", i, graphics->screenMode[i].w, graphics->screenMode[i].h);
		}
		else
		{
			printf("\t                    %d - %d x %d\n", i, graphics->screenMode[i].w, graphics->screenMode[i].h);
		}
	}
	
#if 0
	// TODO: fix Joystick support
	for (int i = 0 ; i < SDL_NumJoysticks() ; i++)
	{
		if (i == 0)
		{
			printf("\t-joystick 0         Enable joystick '%s' (default)\n", SDL_JoystickName(i));
		}
		else
		{
			printf("\t-joystick %d         Enable joystick '%s'\n", i, SDL_JoystickName(i));
		}
	}
#endif
	
	printf("\t-mono               Use mono sound\n");
	printf("\t-stereo             Use stereo sound (default)\n");
	printf("\t-noaudio            Disables audio\n");
	printf("\t-version            Display version number\n");
	printf("\t--help              This help\n\n");

	exit(0);
}

void GCC_NORETURN showVersion()
{
	printf("\n");
	printf("Blob Wars, Episode II - Blob And Conquer (Version %.2f, Release %d)\n", VERSION, RELEASE);
	printf("Copyright (C) 2006 Parallel Realities\n");
	printf("Licensed under the GNU General Public License (GPL)\n\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	#if RELEASE
	if (chdir(PAKLOCATION))
	{
		perror("Could not chdir to " PAKLOCATION ":");
		return 1;
	}
	#endif

	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC, "C");
	textdomain("blobAndConquer");
	bindtextdomain("blobAndConquer", LOCALEDIR);

	atexit(cleanup);
	
	game->saveSlot = -1;
	String missionName;
	int requiredSection = SECTION_TITLE;
	int puzzleType = -1;
	
	for (int i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-fullscreen") == 0) graphics->fullscreen = true;
		else if (strcmp(argv[i], "-window") == 0) graphics->fullscreen = false;
		else if (strcmp(argv[i], "-screensize") == 0) graphics->currentScreenResolution = atoi(argv[++i]);
		else if (strcmp(argv[i], "-noaudio") == 0) audio->useAudio = 0;
		else if (strcmp(argv[i], "-mono") == 0) audio->useAudio = 1;
		else if (strcmp(argv[i], "-version") == 0) showVersion();
		else if (strcmp(argv[i], "--help") == 0) showHelp();
		else if (strcmp(argv[i], "-joystick") == 0) engine->joystickIndex = atoi(argv[++i]);
		else if (strcmp(argv[i], "-mission") == 0) missionName = argv[++i];
		else if (strcmp(argv[i], "-load") == 0) game->saveSlot = atoi(argv[++i]);
		else if (strcmp(argv[i], "-nomonsters") == 0) game->nomonsters = true;
		else if (strcmp(argv[i], "-noitems") == 0) game->noitems = true;
		else if (strcmp(argv[i], "-allstatic") == 0) game->allStatic = true;
		else if (strcmp(argv[i], "-mastermind") == 0) puzzleType = PUZZLE_TYPE_MASTERMIND;
		else if (strcmp(argv[i], "-lasergrid") == 0) puzzleType = PUZZLE_TYPE_LASERGRID;
		else if (strcmp(argv[i], "-credits") == 0) requiredSection = SECTION_CREDITS;
	}
	
	initSystem();
	
	initOpenGL();
	
	initCommonResources();
	
	switch (puzzleType)
	{
		case PUZZLE_TYPE_MASTERMIND:
			loadWidgets("data/widgets/masterMind");
			while (true)
			{
				mission->enemyLevel = Math::rrand(1, 5);
				doMasterMindPuzzle(rand() % 9999999);
			}
			break;
			
		case PUZZLE_TYPE_LASERGRID:
			while (true)
			{
				mission->enemyLevel = Math::rrand(1, 5);
				doLaserGridPuzzle(rand() % 9999999);
			}
			break;
			
		default:
			break;
	}
	
	loadAllDefinitions();
	
	initCutsceneInstructions();
	
	removeUnneededResources();
	
	if (game->saveSlot != -1)
	{
		requiredSection = SECTION_LOAD;
	}
	else if (missionName != "")
	{
		game->targetLocation = "default";
		game->mission = missionName.getText();
		loadMission(game->mission.getText());
		requiredSection = SECTION_BATTLE;
	}
	#if !DEV
	else
	{
		doIntro();
	}
	#endif
	
	while (true)
	{
		if ((requiredSection != SECTION_BATTLE) && (requiredSection != SECTION_GAMEOVER))
		{
			removeUnneededResources();
		}
	
		switch (requiredSection)
		{
			case SECTION_TITLE:
				requiredSection = doTitle();
				break;
				
			case SECTION_NEWGAME:
				game->mission = "mission01";
				game->targetLocation = "default";
				loadMission(game->mission.getText());
				requiredSection = SECTION_BATTLE;
				break;
				
			case SECTION_TRAINING:
				game->training = true;
				loadMission("training");
				requiredSection = SECTION_BATTLE;
				break;
				
			case SECTION_BATTLE:
				requiredSection = mainBattleLoop();
				break;
				
			case SECTION_CHANGE_LEVEL:
				game->mission = game->targetMission.getText();
				loadMission(game->mission.getText());
				requiredSection = SECTION_BATTLE;
				break;
				
			case SECTION_GAMEOVER:
				requiredSection = doGameOver();
				break;
				
			case SECTION_LOAD:
				game->targetLocation = "@none@";
				loadGameData(game->saveSlot, false);
				requiredSection = SECTION_BATTLE;
				break;
				
			case SECTION_RESTART:
				game->targetLocation = "@none@";
				loadGameData(game->saveSlot, true);
				requiredSection = SECTION_BATTLE;
				break;
				
			case SECTION_CREDITS:
				doCredits();
				requiredSection = SECTION_TITLE;
				break;
		}
	}
	
	debug(("main() Exiting\n"));
	
	return 0;
}
