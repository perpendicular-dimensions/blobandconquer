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

#include "options.h"

void populateMouseControls()
{
	debug(("populateMouseControls()\n"));
	
	((InputWidget*)uiManager->getWidget("left", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_LEFT]);
	((InputWidget*)uiManager->getWidget("right", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_RIGHT]);
	((InputWidget*)uiManager->getWidget("forward", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_UP]);
	((InputWidget*)uiManager->getWidget("back", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_DOWN]);
	((InputWidget*)uiManager->getWidget("fire", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_FIRE]);
	((InputWidget*)uiManager->getWidget("jump", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_JUMP]);
	((InputWidget*)uiManager->getWidget("action", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_ACTION]);
	
	((InputWidget*)uiManager->getWidget("grenade", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_QUICK_GRENADE]);
	((InputWidget*)uiManager->getWidget("cycleWeapon", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_CYCLE_WEAPON]);
	((InputWidget*)uiManager->getWidget("lockTarget", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_LOCK_TARGET]);
	((InputWidget*)uiManager->getWidget("cycleTarget", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_CYCLE_TARGET]);
	((InputWidget*)uiManager->getWidget("rotateLeft", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_ROTATE_LEFT]);
	((InputWidget*)uiManager->getWidget("rotateRight", "controlConfig"))->setValue(INPUT_MOUSE, game->mouseCtrl[CTRL_ROTATE_RIGHT]);
	
	((Slider*)uiManager->getWidget("mouseSensitivity", "controlConfig"))->value = 1000 - game->mouseSensitivity;
}

void populateJoystickControls()
{
	debug(("populateJoystickControls()\n"));
	
	((InputWidget*)uiManager->getWidget("left", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_LEFT]);
	((InputWidget*)uiManager->getWidget("right", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_RIGHT]);
	((InputWidget*)uiManager->getWidget("forward", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_UP]);
	((InputWidget*)uiManager->getWidget("back", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_DOWN]);
	((InputWidget*)uiManager->getWidget("fire", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_FIRE]);
	((InputWidget*)uiManager->getWidget("jump", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_JUMP]);
	((InputWidget*)uiManager->getWidget("action", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_ACTION]);
	
	((InputWidget*)uiManager->getWidget("grenade", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_QUICK_GRENADE]);
	((InputWidget*)uiManager->getWidget("cycleWeapon", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_CYCLE_WEAPON]);
	((InputWidget*)uiManager->getWidget("lockTarget", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_LOCK_TARGET]);
	((InputWidget*)uiManager->getWidget("cycleTarget", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_CYCLE_TARGET]);
	((InputWidget*)uiManager->getWidget("rotateLeft", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_ROTATE_LEFT]);
	((InputWidget*)uiManager->getWidget("rotateRight", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_ROTATE_RIGHT]);
	
	((InputWidget*)uiManager->getWidget("status", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_STATUS]);
	((InputWidget*)uiManager->getWidget("pause", "controlConfig"))->setValue(INPUT_JOY, game->joystick[CTRL_PAUSE]);
	
	((Slider*)uiManager->getWidget("analogDeadZone", "controlConfig"))->value = game->joystickDeadZone;
}

void populateKeyboardControls()
{
	debug(("populateKeyboardControls()\n"));
	
	((InputWidget*)uiManager->getWidget("left", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_LEFT]);
	((InputWidget*)uiManager->getWidget("right", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_RIGHT]);
	((InputWidget*)uiManager->getWidget("forward", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_UP]);
	((InputWidget*)uiManager->getWidget("back", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_DOWN]);
	((InputWidget*)uiManager->getWidget("fire", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_FIRE]);
	((InputWidget*)uiManager->getWidget("jump", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_JUMP]);
	((InputWidget*)uiManager->getWidget("action", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_ACTION]);
	
	((InputWidget*)uiManager->getWidget("grenade", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_QUICK_GRENADE]);
	((InputWidget*)uiManager->getWidget("cycleWeapon", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_CYCLE_WEAPON]);
	((InputWidget*)uiManager->getWidget("lockTarget", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_LOCK_TARGET]);
	((InputWidget*)uiManager->getWidget("cycleTarget", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_CYCLE_TARGET]);
	((InputWidget*)uiManager->getWidget("rotateLeft", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_ROTATE_LEFT]);
	((InputWidget*)uiManager->getWidget("rotateRight", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_ROTATE_RIGHT]);
	
	((InputWidget*)uiManager->getWidget("status", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_STATUS]);
	((InputWidget*)uiManager->getWidget("pause", "controlConfig"))->setValue(INPUT_KEY, game->keyboard[CTRL_PAUSE]);
}

void applyMouseControls()
{
	debug(("applyMouseControls()\n"));
	
	game->mouseCtrl[CTRL_LEFT] = ((InputWidget*)uiManager->getWidget("left", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_RIGHT] = ((InputWidget*)uiManager->getWidget("right", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_UP] = ((InputWidget*)uiManager->getWidget("forward", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_DOWN] = ((InputWidget*)uiManager->getWidget("back", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_FIRE] = ((InputWidget*)uiManager->getWidget("fire", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_JUMP] = ((InputWidget*)uiManager->getWidget("jump", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_ACTION] = ((InputWidget*)uiManager->getWidget("action", "controlConfig"))->value[INPUT_MOUSE];
	
	game->mouseCtrl[CTRL_QUICK_GRENADE] = ((InputWidget*)uiManager->getWidget("grenade", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_CYCLE_WEAPON] = ((InputWidget*)uiManager->getWidget("cycleWeapon", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_LOCK_TARGET] = ((InputWidget*)uiManager->getWidget("lockTarget", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_CYCLE_TARGET] = ((InputWidget*)uiManager->getWidget("cycleTarget", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_ROTATE_LEFT] = ((InputWidget*)uiManager->getWidget("rotateLeft", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_ROTATE_RIGHT] = ((InputWidget*)uiManager->getWidget("rotateRight", "controlConfig"))->value[INPUT_MOUSE];
	
	game->mouseCtrl[CTRL_STATUS] = ((InputWidget*)uiManager->getWidget("status", "controlConfig"))->value[INPUT_MOUSE];
	game->mouseCtrl[CTRL_PAUSE] = ((InputWidget*)uiManager->getWidget("pause", "controlConfig"))->value[INPUT_MOUSE];
	
	game->mouseSensitivity = 1000 - (int)((Slider*)uiManager->getWidget("mouseSensitivity", "controlConfig"))->value;
}

void applyJoystickControls()
{
	debug(("applyJoystickControls()\n"));
	
	game->joystick[CTRL_LEFT] = ((InputWidget*)uiManager->getWidget("left", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_RIGHT] = ((InputWidget*)uiManager->getWidget("right", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_UP] = ((InputWidget*)uiManager->getWidget("forward", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_DOWN] = ((InputWidget*)uiManager->getWidget("back", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_FIRE] = ((InputWidget*)uiManager->getWidget("fire", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_JUMP] = ((InputWidget*)uiManager->getWidget("jump", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_ACTION] = ((InputWidget*)uiManager->getWidget("action", "controlConfig"))->value[INPUT_JOY];
	
	game->joystick[CTRL_QUICK_GRENADE] = ((InputWidget*)uiManager->getWidget("grenade", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_CYCLE_WEAPON] = ((InputWidget*)uiManager->getWidget("cycleWeapon", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_LOCK_TARGET] = ((InputWidget*)uiManager->getWidget("lockTarget", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_CYCLE_TARGET] = ((InputWidget*)uiManager->getWidget("cycleTarget", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_ROTATE_LEFT] = ((InputWidget*)uiManager->getWidget("rotateLeft", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_ROTATE_RIGHT] = ((InputWidget*)uiManager->getWidget("rotateRight", "controlConfig"))->value[INPUT_JOY];
	
	game->joystick[CTRL_STATUS] = ((InputWidget*)uiManager->getWidget("status", "controlConfig"))->value[INPUT_JOY];
	game->joystick[CTRL_PAUSE] = ((InputWidget*)uiManager->getWidget("pause", "controlConfig"))->value[INPUT_JOY];
	
	game->joystickDeadZone = (int)((Slider*)uiManager->getWidget("analogDeadZone", "controlConfig"))->value;
}

void applyKeyboardControls()
{
	debug(("applyKeyboardControls()\n"));
	
	game->keyboard[CTRL_LEFT] = ((InputWidget*)uiManager->getWidget("left", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_RIGHT] = ((InputWidget*)uiManager->getWidget("right", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_UP] = ((InputWidget*)uiManager->getWidget("forward", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_DOWN] = ((InputWidget*)uiManager->getWidget("back", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_FIRE] = ((InputWidget*)uiManager->getWidget("fire", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_JUMP] = ((InputWidget*)uiManager->getWidget("jump", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_ACTION] = ((InputWidget*)uiManager->getWidget("action", "controlConfig"))->value[INPUT_KEY];
	
	game->keyboard[CTRL_QUICK_GRENADE] = ((InputWidget*)uiManager->getWidget("grenade", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_CYCLE_WEAPON] = ((InputWidget*)uiManager->getWidget("cycleWeapon", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_LOCK_TARGET] = ((InputWidget*)uiManager->getWidget("lockTarget", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_CYCLE_TARGET] = ((InputWidget*)uiManager->getWidget("cycleTarget", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_ROTATE_LEFT] = ((InputWidget*)uiManager->getWidget("rotateLeft", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_ROTATE_RIGHT] = ((InputWidget*)uiManager->getWidget("rotateRight", "controlConfig"))->value[INPUT_KEY];
	
	game->keyboard[CTRL_STATUS] = ((InputWidget*)uiManager->getWidget("status", "controlConfig"))->value[INPUT_KEY];
	game->keyboard[CTRL_PAUSE] = ((InputWidget*)uiManager->getWidget("pause", "controlConfig"))->value[INPUT_KEY];
}

int doGameOver()
{
	audio->fadeMusic();
	
	float fadeOut = 0;
	
	int rtn = SECTION_LOAD;
	
	uiManager->onlyShowGroup("gameOverWidgets");
	
	Grid *grid = uiManager->getGrid("GameOverGrid");
	
	bool showWidgets = false;
	bool finished = false;
	
	Texture *gameOver = textureManager->getTexture("gfx/game/missionFailed.png");
	
	Button *reload = (Button*)uiManager->getWidget("Reload", "gameOverWidgets");
	Button *load = (Button*)uiManager->getWidget("Load", "gameOverWidgets");
	Button *quit = (Button*)uiManager->getWidget("Quit", "gameOverWidgets");
	
	if (game->saveSlot == -1)
	{
		reload->setEnabled(false);
	}
	
	engine->slowLogic = true;
	
	while (!finished)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		if (!showWidgets)
		{
			doGameObjects();
			drawGameObjects();
			
			doPlayer();
			
			showWidgets = (fadeOut >= 1.25);
		}
		
		graphics->setMode(MODE_2D);
		
		if (!showWidgets)
		{
			drawInfoPanel();
		}
		
		fadeOut += engine->getTimeDifference(TD_ANIMATION) * 0.002;
		graphics->fade(fadeOut);
		
		glColor4f(1.0, 1.0, 1.0, fadeOut);
		graphics->blit(gameOver, graphics->screenMidX, grid->y - 25, true);
		
		if (showWidgets)
		{
			glClearColor(0, 0, 0, 0);
			
			mouse->setBusy(false);
			
			drawWidgets();
			
			if (reload->wasClicked())
			{
				game->setMissionStatus(MISSION_LOADING);
				game->retrying = true;
				
				finished = true;
			}
			
			if (load->wasClicked())
			{
				if (loadGame())
				{
					game->setMissionStatus(MISSION_LOADING);
					finished = true;
				}
				else
				{
					uiManager->onlyShowGroup("gameOverWidgets");
				}
			}
			
			if (quit->wasClicked())
			{
				rtn = SECTION_TITLE;
				finished = true;
				graphics->clearScreen();
				graphics->updateScreen();
			}
		}
		
		if (engine->userAccepts())
		{
			showWidgets = true;
			fadeOut = 1.25;
		}
	}
	
	return rtn;
}

void doCheatOptions()
{
	uiManager->onlyShowGroup("cheats");
	
	Grid *grid = uiManager->getGrid("CheatGrid");
	
	CheckBox *healthCheat = (CheckBox*)uiManager->getWidget("Health", "cheats");
	CheckBox *shieldCheat = (CheckBox*)uiManager->getWidget("Shield", "cheats");
	CheckBox *ammoCheat = (CheckBox*)uiManager->getWidget("Ammo", "cheats");
	CheckBox *oxygenCheat = (CheckBox*)uiManager->getWidget("Oxygen", "cheats");
	CheckBox *blindCheat = (CheckBox*)uiManager->getWidget("Blind", "cheats");
	CheckBox *NPCCheat = (CheckBox*)uiManager->getWidget("NPC", "cheats");
	CheckBox *clipCheat = (CheckBox*)uiManager->getWidget("Clip", "cheats");
	CheckBox *stunCheat = (CheckBox*)uiManager->getWidget("Stun", "cheats");
	ComboBox *teekaCheat = (ComboBox*)uiManager->getWidget("TeekaWeapon", "cheats");
	CheckBox *itemCheat = (CheckBox*)uiManager->getWidget("Items", "cheats");
	CheckBox *saveCheat = (CheckBox*)uiManager->getWidget("SaveGame", "cheats");
	CheckBox *desimilateCheat = (CheckBox*)uiManager->getWidget("Desimilate", "cheats");
	CheckBox *timeCheat = (CheckBox*)uiManager->getWidget("Time", "cheats");
	
	Button *done = (Button*)uiManager->getWidget("Done", "cheats");
	
	healthCheat->checked = game->cheatHealth;
	shieldCheat->checked = game->cheatShield;
	ammoCheat->checked = game->cheatAmmo;
	oxygenCheat->checked = game->cheatOxygen;
	blindCheat->checked = game->cheatBlind;
	NPCCheat->checked = game->cheatNPC;
	clipCheat->checked = game->cheatClip;
	stunCheat->checked = game->cheatStun;
	itemCheat->checked = game->cheatItems;
	saveCheat->checked = game->cheatSave;
	desimilateCheat->checked = game->cheatDesimilate;
	timeCheat->checked = game->cheatTime;
	
	for (int i = 0 ; i < WEP_BOMBLET ; i++)
	{
		teekaCheat->addItem(entityManager->weaponDef[i].getName());
	}
	
	teekaCheat->setSelectedItem(game->teekaWeapon);
	
	bool finished = false;
	
	engine->resetTimeDifference();
	
	while (!finished)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		graphics->setMode(MODE_3D);
		
		game->changeCameraRotation(0);
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		
		graphics->fade(0.5);
		
		graphics->fade(0.65, grid->x, grid->y, grid->width, grid->height);
		graphics->outlineRect(grid->x, grid->y, grid->width, grid->height, GLColor::lightGrey);
		
		drawWidgets();
		
		if (healthCheat->wasClicked())
		{
			game->cheatHealth = healthCheat->checked;
		}
		
		if (shieldCheat->wasClicked())
		{
			game->cheatShield = shieldCheat->checked;
		}
		
		if (ammoCheat->wasClicked())
		{
			game->cheatAmmo = ammoCheat->checked;
		}
		
		if (oxygenCheat->wasClicked())
		{
			game->cheatOxygen = oxygenCheat->checked;
		}
		
		if (blindCheat->wasClicked())
		{
			game->cheatBlind = blindCheat->checked;
		}
		
		if (NPCCheat->wasClicked())
		{
			game->cheatNPC = NPCCheat->checked;
		}
		
		if (clipCheat->wasClicked())
		{
			game->cheatClip = clipCheat->checked;
		}
		
		if (stunCheat->wasClicked())
		{
			game->cheatStun = stunCheat->checked;
		}
		
		if (itemCheat->wasClicked())
		{
			game->cheatItems = itemCheat->checked;
		}
		
		if (saveCheat->wasClicked())
		{
			game->cheatSave = saveCheat->checked;
		}
		
		if (desimilateCheat->wasClicked())
		{
			game->cheatDesimilate = desimilateCheat->checked;
		}
		
		if (clipCheat->wasClicked())
		{
			game->cheatClip = clipCheat->checked;
		}
		
		if (timeCheat->wasClicked())
		{
			game->cheatTime = timeCheat->checked;
		}
		
		if ((done->wasClicked()) || (engine->keyState[SDLK_ESCAPE]))
		{
			finished = true;
			engine->keyState[SDLK_ESCAPE] = 0;
		}
	}
	
	game->teekaWeapon = teekaCheat->selectedItem;
	WeaponDef *weaponDef = &entityManager->weaponDef[teekaCheat->selectedItem];
	
	Unit *teeka = findTeeka();
	if (teeka != NULL)
	{
		teeka->currentWeapon = weaponDef;
	}
	
	teekaCheat->clear();
	
	if (player != NULL)
	{
		(game->cheatClip) ? player->flags |= EF_NOCLIP : player->flags &= ~EF_NOCLIP;
	}
}

void doAdditionalOptions()
{
	uiManager->onlyShowGroup("additional");
	
	Grid *grid = uiManager->getGrid("AdditionalGrid");
	
	bool finished = false;
	
	ComboBox *blood = (ComboBox*)uiManager->getWidget("Blood", "additional");
	ComboBox *shadows = (ComboBox*)uiManager->getWidget("Shadows", "additional");
	ComboBox *decals = (ComboBox*)uiManager->getWidget("Decals", "additional");
	CheckBox *swap = (CheckBox*)uiManager->getWidget("SwapAxis", "additional");
	Slider *blur = (Slider*)uiManager->getWidget("Blur", "additional");
	
	Button *done = (Button*)uiManager->getWidget("Done", "additional");
	
	blood->addItem(_("Off"));
	blood->addItem(_("No Stains"));
	blood->addItem(_("On"));

	if (graphics->hasStencils)
	{	
		shadows->addItem(_("Off"));
		shadows->addItem(_("Bob Only"));
		shadows->addItem(_("Blobs and Enemies"));
		shadows->addItem(_("On"));
	}
	else
	{
		shadows->addItem(_("Not Available (no stencil support)"));
	}
	
	decals->addItem(_("Off"));
	decals->addItem(_("Low"));
	decals->addItem(_("Medium"));
	decals->addItem(_("On"));
	
	blood->setSelectedItem(game->bloodPolicy);
	decals->setSelectedItem(game->decalPolicy);
	shadows->setSelectedItem(game->shadowPolicy);
	
	swap->checked = engine->swapSecondStickAxis;
	
	blur->value = 100 - graphics->motionBlur;

	engine->resetTimeDifference();
	
	while (!finished)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		graphics->setMode(MODE_3D);
		
		game->changeCameraRotation(0);
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		
		graphics->fade(0.5);
		
		graphics->fade(0.65, grid->x, grid->y, grid->width, grid->height);
		graphics->outlineRect(grid->x, grid->y, grid->width, grid->height, GLColor::lightGrey);
		
		drawWidgets();
		
		if (done->wasClicked())
		{
			finished = true;
		}
	}
	
	game->bloodPolicy = blood->selectedItem;
	game->decalPolicy = decals->selectedItem;
	game->shadowPolicy = shadows->selectedItem;
	engine->swapSecondStickAxis = swap->checked;
	graphics->motionBlur = 100 - blur->value;
	
	blood->clear();
	decals->clear();
	shadows->clear();
}

void doControlOptions()
{
	uiManager->onlyShowGroup("controlConfig");
	
	Grid *grid = uiManager->getGrid("ControlConfigGrid");
	
	bool finished = false;
	
	engine->resetTimeDifference();
	
	populateJoystickControls();
	populateKeyboardControls();
	populateMouseControls();
	
	Button *done = (Button*)uiManager->getWidget("Done", "controlConfig");
	Button *reset = (Button*)uiManager->getWidget("Reset", "controlConfig");
	Button *cancel = (Button*)uiManager->getWidget("Cancel", "controlConfig");
	
	int lx = grid->x + 10;
	int lx2 = lx + grid->width - 20;
	int ly = grid->y + grid->height - 55;
	
	while (!finished)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		graphics->setMode(MODE_3D);
		
		game->changeCameraRotation(0);
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		
		graphics->fade(0.5);
		
		graphics->fade(0.65, grid->x, grid->y, grid->width, grid->height);
		graphics->outlineRect(grid->x, grid->y, grid->width, grid->height, GLColor::lightGrey);
		
		graphics->drawLine(lx, ly, lx2, ly, GLColor::white);
		
		drawWidgets();
		
		if (done->wasClicked())
		{
			applyJoystickControls();
			applyKeyboardControls();
			applyMouseControls();
			finished = true;
		}
		
		if (reset->wasClicked())
		{
			populateJoystickControls();
			populateKeyboardControls();
			populateMouseControls();
		}
		
		if (cancel->wasClicked())
		{
			finished = true;
		}
	}
}

void doConfigOptions()
{
	uiManager->onlyShowGroup("configPanel");
	
	Grid *grid = uiManager->getGrid("ConfigurationGrid");
	
	Button *done = (Button*)uiManager->getWidget("Done", "configPanel");
	Button *controlConfig = (Button*)uiManager->getWidget("ControlConfig", "configPanel");
	ComboBox *resolution = (ComboBox*)uiManager->getWidget("Resolution", "configPanel");
	CheckBox *fullscreen = (CheckBox*)uiManager->getWidget("FullScreen", "configPanel");
	CheckBox *grab = (CheckBox*)uiManager->getWidget("GrabMouse", "configPanel");
	Slider *soundVolume = (Slider*)uiManager->getWidget("SoundVolume", "configPanel");
	Slider *musicVolume = (Slider*)uiManager->getWidget("MusicVolume", "configPanel");
	Slider *ambientVolume = (Slider*)uiManager->getWidget("AmbientVolume", "configPanel");
	Slider *environmentVolume = (Slider*)uiManager->getWidget("EnvironmentVolume", "configPanel");
	Slider *brightness = (Slider*)uiManager->getWidget("Brightness", "configPanel");
	Button *cheats = (Button*)uiManager->getWidget("Cheats", "configPanel");
	Button *additional = (Button*)uiManager->getWidget("Extra", "configPanel");
	
	Mix_Chunk *cheatSound = NULL;
	
	cheats->setVisible(game->cheatsActive);
	
	uiManager->focusedWidget = done;
	
	bool finished = false;
	
	fullscreen->checked = graphics->fullscreen;
	soundVolume->value = audio->soundVolume;
	musicVolume->value = audio->musicVolume;
	ambientVolume->value = audio->ambientVolume;
	environmentVolume->value = audio->environmentVolume;
	brightness->value = (graphics->gamma - 50);
	grab->checked = mouse->grab;
	
	String res;
	
	for (unsigned i = 0 ; i < MAX_RESOLUTIONS ; i++)
	{
		if (graphics->screenMode[i].w == 0)
		{
			continue;
		}
		
		res.setText("%d x %d", graphics->screenMode[i].w, graphics->screenMode[i].h);
		resolution->addItem(res.getText());
	}
	
	resolution->setSelectedItem(graphics->currentScreenResolution);
	
	engine->resetTimeDifference();
	
	while (!finished)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		graphics->setMode(MODE_3D);
		
		game->changeCameraRotation(0);
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		
		graphics->fade(0.5);
		
		graphics->fade(0.65, grid->x, grid->y, grid->width, grid->height);
		graphics->outlineRect(grid->x, grid->y, grid->width, grid->height, GLColor::lightGrey);
		
		drawWidgets();
		
		if (resolution->hasChanged())
		{
			graphics->setResolution(resolution->selectedItem);
			uiManager->recalculateWidgetPositions();
		}
		
		if (fullscreen->wasClicked())
		{
			graphics->fullscreen = !graphics->fullscreen;
			SDL_WM_ToggleFullScreen(graphics->screen);
		}
		
		if (grab->wasClicked())
		{
			mouse->grab = !mouse->grab;
		}
		
		if (soundVolume->hasChanged())
		{
			audio->setSoundVolume((int)soundVolume->value);
		}
		
		if (musicVolume->hasChanged())
		{
			audio->setMusicVolume((int)musicVolume->value);
		}
		
		if (ambientVolume->hasChanged())
		{
			audio->setAmbientVolume((int)ambientVolume->value);
		}
		
		if (environmentVolume->hasChanged())
		{
			audio->setEnvironmentVolume((int)environmentVolume->value);
		}
		
		if (brightness->hasChanged())
		{
			graphics->gamma = 50 + brightness->value;
			Math::limit(&graphics->gamma, 0, 200);
			SDL_SetGamma(graphics->gamma / 100, graphics->gamma / 100, graphics->gamma / 100);
		}
		
		if (controlConfig->wasClicked())
		{
			doControlOptions();
			uiManager->onlyShowGroup("configPanel");
			cheats->setVisible(game->cheatsActive);
		}
		
		if (cheats->wasClicked())
		{
			doCheatOptions();
			uiManager->onlyShowGroup("configPanel");
			cheats->setVisible(game->cheatsActive);
		}
		
		if (additional->wasClicked())
		{
			doAdditionalOptions();
			uiManager->onlyShowGroup("configPanel");
			cheats->setVisible(game->cheatsActive);
		}
		
		if ((done->wasClicked()) || (engine->keyState[SDLK_ESCAPE]))
		{
			finished = true;
			engine->keyState[SDLK_ESCAPE] = 0;
		}
		
		fullscreen->checked = graphics->fullscreen;
		
		if (!game->cheatsActive)
		{
			if (strstr(engine->lastKeyEvents, "leavebritneyalone"))
			{
				cheats->setVisible(game->cheatsActive = true);
				String filename;
				filename = "sound/gui/cheatMode.ogg";
				
				pak->unpack(filename.getText(), PAK::BINARY);
				cheatSound = Mix_LoadWAV_RW(pak->sdlrw, true);		
				
				if (cheatSound != NULL)
				{
					Mix_Volume(0, audio->soundVolume);
					Mix_PlayChannel(0, cheatSound, 0);
				}
			}
		}
	}
	
	resolution->clear();
	
	if (cheatSound != NULL)
	{
		free(cheatSound);
	}
}

void doInGameOptions()
{
	uiManager->onlyShowGroup("optionPanel");
	
	Grid *grid = uiManager->getGrid("InGameOptionGrid");
	
	Button *load = (Button*)uiManager->getWidget("Load", "optionPanel");
	Button *save = (Button*)uiManager->getWidget("Save", "optionPanel");
	Button *options = (Button*)uiManager->getWidget("Options", "optionPanel");
	Button *resume = (Button*)uiManager->getWidget("Resume", "optionPanel");
	Button *restart = (Button*)uiManager->getWidget("Restart", "optionPanel");
	Button *quit = (Button*)uiManager->getWidget("Quit", "optionPanel");
	
	if (game->saveSlot == -1)
	{
		restart->setEnabled(false);
	}
	
	uiManager->focusedWidget = resume;
	
	save->setEnabled(game->canSaveGame);
	
	graphics->setMode(MODE_2D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	bool finished = false;
	
	mouse->setBusy(false);
	
	engine->resetTimeDifference();
	
	while (!finished)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		graphics->setMode(MODE_3D);
	
		game->changeCameraRotation(0);
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		
		graphics->fade(0.5);
		
		graphics->fade(0.65, grid->x, grid->y, grid->width, grid->height);
		graphics->outlineRect(grid->x, grid->y, grid->width, grid->height, GLColor::lightGrey);
		
		drawWidgets();
		
		if (options->wasClicked())
		{
			doConfigOptions();
			uiManager->onlyShowGroup("optionPanel");
			
			uiManager->focusedWidget = options;
		}
		
		if (load->wasClicked())
		{
			if (loadGame())
			{
				game->setMissionStatus(MISSION_LOADING);
			}
			
			finished = true;
		}
		
		if (save->wasClicked())
		{
			saveGame();
			
			uiManager->focusedWidget = save;
		}
		
		if (restart->wasClicked())
		{
			game->setMissionStatus(MISSION_RESTARTING);
			
			finished = true;
		}
		
		if ((resume->wasClicked()) || (engine->keyState[SDLK_ESCAPE]) || (game->isControl(CTRL_PAUSE)))
		{
			finished = true;
			engine->keyState[SDLK_ESCAPE] = 0;
		}
		
		if (quit->wasClicked())
		{
			game->setMissionStatus(MISSION_QUITTING);
			
			finished = true;
		}
	}
	
	mouse->setBusy(true);
}
