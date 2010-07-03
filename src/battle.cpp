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

#include "battle.h"

void initBattle()
{
	debug(("initBattle()\n"));

	loadWidgets("data/widgets/inGameOptions");
	loadWidgets("data/widgets/generalOptions");
	loadWidgets("data/widgets/controlOptions");;
	loadWidgets("data/widgets/status");
	loadWidgets("data/widgets/loadSave");
	loadWidgets("data/widgets/gameover");
	loadWidgets("data/widgets/cheats");
	loadWidgets("data/widgets/additional");
	loadWidgets("data/widgets/masterMind");

	loadAllBattleResources();
	loadMapAudio();

	// --------- init battledata ------------------

	game->setMissionStatus(MISSION_INPROGRESS);

	GLfloat ambient[]= {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat diffuse[]= {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat specular[]= {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat position[]= {1.0f, 1.0f, 1.0f, 0.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
	
	glClearColor(mission->fogColor.color[0], mission->fogColor.color[1], mission->fogColor.color[2], mission->fogColor.color[3]);

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glHint(GL_FOG_HINT, GL_NICEST);
	
	glFogfv(GL_FOG_COLOR, mission->fogColor.color);
	glFogf(GL_FOG_DENSITY, mission->fogDensity);
	glFogf(GL_FOG_START, mission->fogMin);
	glFogf(GL_FOG_END, mission->fogMax);
	glEnable(GL_FOG);
	
	debug(("initBattle() :: Constructing BSP Bounding Box Grid\n"));
	
	bbManager->construct(bsp->minCords, bsp->maxCords);

	debug(("initBattle() :: Done\n"));
}

void prepareBattleScene()
{
	graphics->setMode(MODE_3D);

	glColor3f(1.0, 1.0, 1.0);
	
	glFogf(GL_FOG_START, mission->fogMin);
	glFogf(GL_FOG_END, mission->fogMax);
}

void drawNormalObjects()
{
	entityManager->mtGlow += (entityManager->mtDir * 0.005 * engine->getTimeDifference(TD_LOGIC));
	
	Math::limit(&entityManager->mtGlow, 0.25, 0.75);
	
	if ((entityManager->mtGlow <= 0.25) || (entityManager->mtGlow >= 0.75))
	{
		entityManager->mtDir = -entityManager->mtDir;
	}

	drawBSP(false);
	
	calculatePlayerAimDirection();
	
	Reference *ref;
	
	for (ref = (Reference*)entityManager->drawList.getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		self = (Entity*)ref->object;
		
		if (self->draw != NULL)
		{
			self->draw(false);
		}
		else
		{
			printf("ERROR: draw() for entity %s (%s) is NULL\n", self->getEntityTypeName(), self->getName());
			exit(1);
		}
	}
	
	drawBSP(true);
	
	drawDecals();
	
	for (ref = (Reference*)entityManager->drawList.getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		self = (Entity*)ref->object;
		
		if (self->draw != NULL)
		{
			self->draw(true);
		}
		else
		{
			printf("ERROR: draw() for entity %s (%s) is NULL\n", self->getEntityTypeName(), self->getName());
			exit(1);
		}
	}
	
	#if DEV
	if ((camera->targetCamera != NULL) && (camera->targetCamera->chasing))
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glColor4f(1.0, 0.0, 0.0, 0.1);
		drawBox(camera->targetCamera->chase.position, 1, 1, 1, 1);
	}
	#endif
}

#if DEV
void drawBreadCrumbs()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);
	
	float red = crumbBox.crumbs.getSize();
	float age = 1 / red;
	
	red = 0;
	
	for (Crumb *crumb = (Crumb*)crumbBox.crumbs.getFirstElement() ; crumb != NULL ; crumb = (Crumb*)crumb->next)
	{
		red += age;
	
		glColor3f(red, 0.0, 1.0);
	
		drawBox(crumb->position, 1, 1, 1, 1);
	}
}
#endif

void drawGameObjects()
{
	if (!game->inGame)
	{
		graphics->setMode(MODE_2D);
		glColor3f(1.0, 1.0, 1.0);
		drawTitle();
		return;
	}

	prepareBattleScene();
	doCamera();
	
	textureSpecManager->resetTextureAnimation();
	
	drawSkyDome(camera->position, 500, textureManager->getTexture("SkySphere"));
	
	drawNormalObjects();
	
	#if DEV
	//drawBreadCrumbs();
	#endif
	
	drawParticles();
	
	drawShadow();
		
	drawSelectedUnit();
	
	GLColor c;
	
	if (game->whiteOutTime > 0)
	{
		graphics->setMode(MODE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		c = GLColor::white;
		c.color[3] = game->whiteOutTime;
		game->whiteOutTime -= (0.01 * engine->getTimeDifference(TD_LOGIC));
		graphics->drawRect(0, 0, graphics->screen->w, graphics->screen->h, c, true);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		graphics->setMode(MODE_3D);
	}
	
	if (camera->contentType & CONTENTS_WATER)
	{
		c = GLColor::skyBlue;
	}
	else if (camera->contentType & CONTENTS_SLIME)
	{
		c = GLColor::green;
	}
	else if (camera->contentType & CONTENTS_LAVA)
	{
		c = GLColor::red;
	}
	else
	{
		return;
	}
	
	c.color[3] *= 0.25;
	graphics->setMode(MODE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	graphics->drawRect(0, 0, graphics->screen->w, graphics->screen->h, c, true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	graphics->setMode(MODE_3D);
	
	camera->contentType = CONTENTS_SOLID;
}

void doGameObjects()
{
	entityManager->drawList.clear();
	
	doItems();
	
	doBullets();
	
	doEnemies();
	doBosses();
	doBlobs();
	
	renderBSP();
	
	doStructures();
	
	doDecoration();
	
	doTriggers();
	doFeatures();
	doParticles();
	doWeather();
	
	doEmitters();
	
	doTraps();
	
	doDecals();
	
	audio->doAmbience();
}

void doBattleCamera()
{
	if (engine->keyState[SDLK_F2])
	{
		if ((game->canSaveGame) || (game->cheatSave))
		{
			if (entityManager->bossList.getSize() == 0)
			{
				saveGame();
				engine->resetTimeDifference();
			}
			else
			{
				game->setInfoMessage(INFO_HINT, _("Can't do that during a boss battle..."));
			}
		}

		engine->keyState[SDLK_F2] = 0;
	}

	if (engine->keyState[SDLK_F5])
	{
		if ((game->canSaveGame) || (game->cheatSave))
		{
			saveGameData(false);
			engine->resetTimeDifference();
		}

		engine->keyState[SDLK_F5] = 0;
	}
	
	#if DEV
	if (engine->keyState[SDLK_F11])
	{
		engine->keyState[SDLK_F11] = 0;
		graphics->wireframe = !graphics->wireframe;
	}
	
	if (engine->keyState[SDLK_F8])
	{
		processCutscene(&mission->startCutsceneData);
	}
	
	if (engine->keyState[SDLK_F9])
	{
		processCutscene(&mission->endCutsceneData);
	}
	
	if (engine->keyState[SDLK_F12])
	{
		String exitToMission;
		exitToMission = mission->getNextMissionName();
		game->setMissionStatus(MISSION_COMPLETE);
		game->changingLevel = true;
		game->targetMission = exitToMission.getText();
		game->resetControl(CTRL_ACTION);
	}
	#endif
}

int mainBattleLoop()
{
	Vector explosionPosition;

	debug(("mainBattleLoop()\n"));
	
	game->inGame = true;
	
	initBattle();
	
	if (game->changingLevel)
	{
		game->changingLevel = false;
		saveGameData(true);
		saveRestartData();
	}
	
	game->retrying = false;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		game->infoMessage[i].life = -1;
	}

	graphics->clearScreen();
	
	/* Reset Cluster before Cutscene */
	
	player->cluster = -1;
	player->riding = NULL;
	player->inCollisionCandidateList = false;
	
	engine->slowLogic = false;
	
	processCutscene(&mission->startCutsceneData);

	uiManager->onlyShowGroup("standardPanel");

	int rtn = SECTION_BATTLE;

	mouse->setBusy(true);
	
	/* Reset Cluster after Cutscene */
	
	player->cluster = -1;
	player->riding = NULL;
	player->showHealthData = true;
	player->target = NULL;
	player->inCollisionCandidateList = false;
	player->action = NULL;
	debug(("Player start: %s\n", player->position.toString()));
	
	game->timeUpExplosionTime = Math::rrand(10, 75);
	
	float bannerTimeOut = 525;
	
	graphics->setFont(FONT_DIGITAL);
	
	Texture *missionBanner = NULL;
	
	if (entityManager->bossList.getSize() == 0)
	{
		String banner;
		banner = mission->missionName.getText();
		missionBanner = graphics->getGLString("%s", banner.getText());
		textureManager->removeTexture("MissionBanner");
		textureManager->addTexture("MissionBanner", missionBanner);
	}
	
	float autoInfoTimer = 50;

	game->changeCameraRotation(-game->cameraAdjustedRadian);

	graphics->resetFPSCount();
	
	float previousRemaining = 0;
	float spawnTime = Math::rrand(mission->minSpawnTime, mission->maxSpawnTime);
	float spawnInterval = Math::rrand(50, 150);
	int numberToSpawn = Math::rrand(min(mission->enemyLevel, mission->spawnSize), mission->spawnSize);
	
	camera->setOwner(player);
	camera->contentType = CONTENTS_SOLID;
	camera->targetCamera = NULL;
	
	audio->playSoundLoop(SND_WEATHER, CH_ENVIRONMENT);
	
	if (mission->missionNumber < 31)
	{
		audio->playMusic();
	}
	else
	{
		if (mission->startCutscene == "")
		{
			audio->playMusic();
		}
	}
	
	crumbBox.addCrumb(player->position, true);
	
	graphics->clearScreen();
	graphics->updateScreen();
	
	engine->resetTimeDifference();
	
	debug(("mainBattleLoop() - Begin\n"));

	while (rtn == SECTION_BATTLE)
	{
		engine->doTimeDifference();
		graphics->updateScreen();
		engine->getInput();
		
		if ((SDL_GetAppState() & SDL_APPINPUTFOCUS) == 0)
		{
			doInGameOptions();
		}
		
		if ((game->isControl(CTRL_PAUSE)) || (engine->keyState[SDLK_ESCAPE]))
		{
			debug(("Player = %s\n", player->position.toString()));
		
			graphics->allowMotionBlur = false;
			game->resetControl(CTRL_PAUSE);
			engine->keyState[SDLK_ESCAPE] = 0;
			doInGameOptions();
			game->resetControl(CTRL_PAUSE);
			engine->keyState[SDLK_ESCAPE] = 0;
			graphics->allowMotionBlur = true;
		}
		
		graphics->clearScreen();
		
		if (game->missionStatus != MISSION_INPROGRESS)
		{
			game->missionOverTimer -= engine->getTimeDifference(TD_LOGIC);
		}
		
		if (mission->spawnSize > 0)
		{
			if ((spawnTime -= engine->getTimeDifference(TD_LOGIC)) <= 0)
			{
				if ((spawnInterval -= engine->getTimeDifference(TD_LOGIC)) <= 0)
				{
					if (isOkayToSpawnEnemies())
					{
						spawnRandomEnemy();
						spawnInterval = Math::rrand(10, 75);
						numberToSpawn--;
					}
					else
					{
						numberToSpawn = 0;
					}
					
					if (numberToSpawn == 0)
					{
						numberToSpawn = Math::rrand(min(mission->enemyLevel, mission->spawnSize), mission->spawnSize);
						spawnTime = Math::rrand(mission->minSpawnTime, mission->maxSpawnTime);
					}
				}
			}
		}

		doGameObjects();
		drawGameObjects();

		doPlayer();
		
		previousRemaining = game->totalRemaining;

		game->incTime(0.01 * engine->getTimeDifference(TD_LOGIC));
		game->decTimeLimit(0.01 * engine->getTimeDifference(TD_LOGIC));
		game->doComboTimer(engine->getTimeDifference(TD_LOGIC));
		
		if (game->totalRemaining != 0)
		{
			graphics->redPulse += (graphics->redPulseDir * 0.0075) * engine->getTimeDifference(TD_LOGIC);
	
			if (graphics->redPulse <= 0)
			{
				graphics->redPulseDir = 1;
			}
			else if (graphics->redPulse > 0.5)
			{
				graphics->redPulseDir = -1;
			}
			
			mission->timeLimit = game->totalRemaining;
			
			if ((game->totalRemaining <= 60) && (previousRemaining > 60))
			{
				audio->playSound(SND_TIME_WARNING_1, CH_ANY, 128);
				game->setInfoMessage(INFO_BAD, _("1 Minute Left..."));
			}
			
			if ((game->totalRemaining <= 30) && (previousRemaining > 30))
			{
				audio->playSound(SND_TIME_WARNING_1, CH_ANY, 128);
				game->setInfoMessage(INFO_BAD, _("30 Seconds Remaining!"));
			}
			
			if ((game->totalRemaining <= 10) && (previousRemaining > 10))
			{
				game->setInfoMessage(INFO_BAD, _("!!! Hurry Up !!!"));
			}
			
			for (int i = 1 ; i < 11 ; i++)
			{
				if ((game->totalRemaining <= i) && (previousRemaining > i))
				{
					audio->playSound(SND_TIME_WARNING_2, CH_ANY, 128);
				}
			}
			
			if (mission->timeLimit <= 0)
			{
				disableSavePoints();
			}
		}

		graphics->setMode(MODE_2D);
		
		drawInfoPanel();

		#if DEV
		graphics->drawFPSCount(5);
		#endif
		
		glEnable(GL_BLEND);
		
		if (bannerTimeOut > 0 && missionBanner != NULL)
		{
			bannerTimeOut -= engine->getTimeDifference(TD_LOGIC);
			
			if (bannerTimeOut > 400)
			{
				glColor4f(1.0, 1.0, 1.0, (500 - bannerTimeOut) / 100);
				graphics->blit(missionBanner, graphics->screenMidX, graphics->screen->h - 180, true);
			}
			else if ((bannerTimeOut > 100) && (bannerTimeOut <= 400))
			{
				glColor3f(1.0, 1.0, 1.0);
				graphics->blit(missionBanner, graphics->screenMidX, graphics->screen->h - 180, true);
			}
			else if (bannerTimeOut > 0)
			{
				glColor4f(1.0, 1.0, 1.0, bannerTimeOut / 100);
				graphics->blit(missionBanner, graphics->screenMidX, graphics->screen->h - 180, true);
			}
		}
		
		if (mission->autoInfo != "")
		{
			autoInfoTimer -= engine->getTimeDifference(TD_LOGIC);
			
			if (autoInfoTimer <= 0)
			{
				showMessageDialog(mission->autoInfo.getText(), "OK");
				mission->autoInfo = "";
			}
		}

		if ((game->missionStatus != MISSION_INPROGRESS) && (game->missionOverTimer <= 0))
		{
			switch (game->missionStatus)
			{
				case MISSION_COMPLETE:
					processCutscene(&mission->endCutsceneData);
					
					if (mission->missionNumber != 31)
					{
						saveGameData(true);
						rtn = SECTION_CHANGE_LEVEL;
					}
					else
					{
						rtn = SECTION_CREDITS;
					}
					
					break;
					
				case MISSION_RESTARTING:
					game->targetMission = mission->restartMissionName.getText();
					rtn = SECTION_CHANGE_LEVEL;
					break;	
				
				case MISSION_PLAYER_OUT:
					graphics->clearScreen();
					graphics->updateScreen();
					graphics->delay(500, false);
					resetPlayer();
					resetEnemyThinkTimes();
					break;
				
				case MISSION_FAILED:
					rtn = doGameOver();
					break;
					
				case MISSION_LOADING:
					rtn = SECTION_LOAD;
					break;
					
				case MISSION_QUITTING:
					rtn = SECTION_TITLE;
					break;
			}
		}
		
		if ((mission->timeLimit != 0) && (game->totalRemaining <= 0))
		{
			game->timeUpExplosionTime -= engine->getTimeDifference(TD_LOGIC);
			
			if (game->timeUpExplosionTime <= 0)
			{
				explosionPosition = player->position;
				explosionPosition.x += Math::rrand(-150, 150);
				explosionPosition.y += Math::rrand(-150, 150);
				explosionPosition.z += Math::rrand(-10, 10);
				doExplosion(explosionPosition, 35, player);
				game->timeUpExplosionTime = Math::rrand(10, 35);
				if ((rand() % 3) == 0)
				{
					game->whiteOutTime = Math::rrand(0, 150);
					game->whiteOutTime /= 100;
				}
			}
		}

		doBattleCamera();
		
		if (camera->targetCamera != NULL)
		{
			doInGameCutscene();
		}
	}

	if (game->missionStatus != MISSION_FAILED)
	{
		audio->stopMusic();
		graphics->clearScreen();
		graphics->updateScreen();
	}
	
	audio->haltChannel(CH_ENVIRONMENT);
	
	removeSpecialWeapons();

	return rtn;
}
