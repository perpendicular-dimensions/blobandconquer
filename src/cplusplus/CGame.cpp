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

#include "../headers.h"

Game::Game()
{
	training = false;
	doingPuzzle = false;
	inGame = false;
	
	statistics.setName("Statistics");
	
	for (int i = 0 ; i < 3 ; i++)
	{
		weapon[i] = NULL;
		weaponId[i] = 0;
	}
	
	for (int i = 0 ; i < 5 ; i++)
	{
		item[i] = NULL;
		itemId[i] = 0;
	}
	
	for (int i = 0 ; i < 4 ; i++)
	{
		infoMessage[i].life = -1;
		infoMessage[i].redraw = true;
		infoMessage[i].message = "";
	}
	
	lastGamePlayMessage = "";
	
	for (int i = 0 ; i < CTRL_MAXIMUM ; i++)
	{
		keyboard[i] = -1;
		joystick[i] = -1;
		mouseCtrl[i] = -1;
	}
	
	decalPolicy = DECAL_POLICY_ON;
	shadowPolicy = SHADOW_POLICY_ON;
	bloodPolicy = DECAL_POLICY_ON;
	
	saveSlot = -1;
	
	joystickDeadZone = 4500;
	mouseSensitivity = 100;
	
	hours = minutes = seconds = totalTime = 0;
	
	remainingMinutes = remainingSeconds = totalRemaining = 0;
	timeUpExplosionTime = whiteOutTime = 0;
	
	mustChangeWeapon = false;
	
	desiredCameraRadius = 165;
	cameraRadius = desiredCameraRadius;
	cameraHeight = 45;
	cameraTempHeight = cameraHeight;
	cameraDirRadian = 0;
	cameraAdjustedRadian = 0;
	cameraDirDegrees = 0;
	cameraAimHeight = 0;
	
	canSaveGame = false;
	
	changingLevel = false;
	retrying = false;
	
	teekaWeapon = WEP_PISTOL;
	
	cutsceneType = CUTSCENE_NONE;
	
	autoFaceTimer = 0;
	autoLockTarget = true;
	
	currentOrders = ORDERS_FOLLOW;
	
	cheatHealth = false;
	cheatShield = false;
	cheatAmmo = false;
	cheatNPC = false;
	cheatClip = false;
	cheatSave = DEV;
	cheatOxygen = false;
	cheatBlind = false;
	cheatStun = false;
	cheatItems = false;
	cheatTime = false;
	
	nomonsters = false;
	noitems = false;
	nofeatures = false;
	allStatic = false;
	cheatsActive = DEV;
	
	engine = Engine::getInstance();
	mouse = Mouse::getInstance();
	
	setDefaultKeyboardControls();
	
	dropGrappleHooks = false;
}

Game::~Game()
{
}

Game *Game::getInstance()
{
	return &instance;
}

void Game::destroy()
{
	clear();
}

void Game::clear()
{
	statistics.clear();
	
	saveSlot = -1;
	
	if (!retrying)
	{
		hours = minutes = seconds = totalTime = 0;
	}
	
	remainingMinutes = remainingSeconds = totalRemaining = 0;
	timeUpExplosionTime = whiteOutTime = 0;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		weapon[i] = NULL;
		weaponId[i] = 0;
	}
	
	for (int i = 0 ; i < 5 ; i++)
	{
		item[i] = NULL;
		itemId[i] = 0;
	}
	
	for (int i = 0 ; i < 4 ; i++)
	{
		infoMessage[i].life = 0;
		infoMessage[i].message = "";
		infoMessage[i].redraw = true;
	}
	
	gameplayMessage.life = 0;
	gameplayMessage.message = "";
	
	changingLevel = false;
	
	training = false;
	
	cutsceneType = CUTSCENE_NONE;
	
	totalRemaining = 0;
	
	teekaWeapon = WEP_PISTOL;
}

void Game::setDefaultKeyboardControls()
{
	keyboard[CTRL_UP] 				= SDLK_UP;
	keyboard[CTRL_DOWN] 			= SDLK_DOWN;
	keyboard[CTRL_LEFT] 			= SDLK_LEFT;
	keyboard[CTRL_RIGHT] 			= SDLK_RIGHT;
	keyboard[CTRL_ACTION] 			= SDLK_RETURN;
	keyboard[CTRL_PAUSE] 			= SDLK_p;
	keyboard[CTRL_STATUS] 			= SDLK_TAB;
	keyboard[CTRL_QUICK_GRENADE] 	= SDLK_RSHIFT;
	
	mouseCtrl[CTRL_FIRE]			= SDL_BUTTON_LEFT;
	mouseCtrl[CTRL_JUMP]			= SDL_BUTTON_RIGHT;
	mouseCtrl[CTRL_CYCLE_TARGET]	= SDL_BUTTON_MIDDLE;
}

bool Game::isControl(int control)
{
	int result = 0;
	
	if (keyboard[control] != -1)
	{
		result = max(result, engine->keyState[keyboard[control]]);
	}
	
	if (engine->buttonState != NULL)
	{
		if (joystick[control] != -1)
		{
			result = max(result, engine->buttonState[joystick[control]]);
		}
	}
	
	if (mouseCtrl[control] != -1)
	{
		result = max(result, mouse->buttonState[mouseCtrl[control]]);
	}
	
	return result;
}

bool Game::isAnyControl()
{
	int result = 0;
	
	for (int i = 0 ; i < CTRL_MAXIMUM ; i++)
	{
		if (keyboard[i] != -1)
		{
			result = max(result, engine->keyState[keyboard[i]]);
		}
		
		if (engine->buttonState != NULL)
		{
			if (joystick[i] != -1)
			{
				result = max(result, engine->buttonState[joystick[i]]);
			}
		}
		
		if (mouseCtrl[i] != -1)
		{
			result = max(result, mouse->buttonState[mouseCtrl[i]]);
		}
	}
	
	return result;
}

void Game::resetControl(int control)
{
	if (keyboard[control] != -1)
	{
		engine->keyState[keyboard[control]] = 0;
	}
	
	if (engine->buttonState != NULL)
	{
		if (joystick[control] != -1)
		{
			engine->buttonState[joystick[control]] = 0;
		}
	}
	
	if (mouseCtrl[control] != -1)
	{
		mouse->buttonState[mouseCtrl[control]] = 0;
	}
}

void Game::setInfoMessage(int type, const char *message, ...)
{
	char tmpString[1024];
	
	va_list argp;
	va_start(argp, message);
	vsnprintf(tmpString, sizeof(tmpString), message, argp);
	va_end(argp);
	
	debug(("%s\n", tmpString));
	
	for (int i = 3 ; i > 0 ; i--)
	{
		infoMessage[i].message = infoMessage[i - 1].message.getText();
		infoMessage[i].life = infoMessage[i - 1].life;
		infoMessage[i].type = infoMessage[i - 1].type;
		infoMessage[i].redraw = true;
	}
	
	infoMessage[0].message = tmpString;
	infoMessage[0].life = 350;
	infoMessage[0].type = type;
	infoMessage[0].redraw = true;
}

void Game::setGamePlayMessage(int type, const char *message, ...)
{
	char tmpString[1024];
	
	va_list argp;
	va_start(argp, message);
	vsnprintf(tmpString, sizeof(tmpString), message, argp);
	va_end(argp);
	
	gameplayMessage.message.setText(_("Hint: %s"), tmpString);
	gameplayMessage.type = type;
	gameplayMessage.life = 10;
	
	if (lastGamePlayMessage != tmpString)
	{
		gameplayMessage.life = 0;
		lastGamePlayMessage = tmpString;
	}
}

void Game::setGamePlayMessage(int type, int time, const char *message, ...)
{
	char tmpString[1024];
	
	va_list argp;
	va_start(argp, message);
	vsnprintf(tmpString, sizeof(tmpString), message, argp);
	va_end(argp);
	
	gameplayMessage.message.setText(_("Hint: %s"), tmpString);
	gameplayMessage.type = type;
	gameplayMessage.life = time;
	
	if (lastGamePlayMessage != tmpString)
	{
		gameplayMessage.life = 0;
		lastGamePlayMessage = tmpString;
	}
}

int Game::getSelectedWeaponSlot()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if (selectedWeapon == weapon[i])
		{
			return i;
		}
	}
	
	// shouldn't get here!!
	return -1;
}

int Game::getFreeInventorySlot()
{
	for (int i = 0 ; i < 5 ; i++)
	{
		if (item[i] == NULL)
		{
			return i;
		}
	}
	
	return -1;
}

int Game::getItemSlot(const char *name)
{
	for (int i = 0 ; i < 5 ; i++)
	{
		if (item[i] != NULL)
		{
			if (item[i]->name == name)
			{
				return i;
			}
			
			if (item[i]->definition->name == name)
			{
				return i;
			}
		}
	}
	
	return -1;
}

bool Game::hasItem(const char *name)
{
	return (getItemSlot(name) != -1);
}

int Game::removeItem(const char *name)
{
	for (int i = 0 ; i < 5 ; i++)
	{
		if (item[i] != NULL)
		{
			if ((item[i]->name == name) || (item[i]->definition->name == name))
			{
				item[i]->owner = NULL;
				item[i]->health = -100;
				item[i] = NULL;
				itemId[i] = 0;
				return i;
			}
		}
	}
	
	printf("WARNING - Game::removeItem() didn't find '%s'\n", name);
	
	return -1;
}

void Game::calculateTime(float total, float *hours, float *minutes, float *seconds)
{
	div_t temp;
	
	temp = div((int)total, 60 * 60);
	
	*hours = temp.quot;
	
	temp = div((int)temp.rem, 60);
	
	*minutes = temp.quot;
	*seconds = temp.rem;
}

void Game::decTimeLimit(float secs)
{
	float unused = 0;
	
	if (cheatTime)
	{
		return;
	}
	
	if (totalRemaining <= 0)
	{
		return;
	}
	
	totalRemaining -= secs;
	
	calculateTime(totalRemaining, &unused, &remainingMinutes, &remainingSeconds);
}

void Game::incTime(float secs)
{
	totalTime += secs;
	
	calculateTime(totalTime, &hours, &minutes, &seconds);
}

void Game::doComboTimer(float td)
{
	if (comboTimer > 0)
	{
		comboTimer -= td;
		
		if ((comboTimer <= 0) && (comboHits > 1))
		{
			setInfoMessage(INFO_GENERAL, _("%d Hit Combo!!"), comboHits);
			
			int personalBest = statistics.getInt("BEST_COMBO", 0);
			
			statistics.setProperty("BEST_COMBO", max(personalBest, comboHits));
			
			comboTimer = 0;
			comboHits = 0;
		}
	}
}

void Game::changeCameraRotation(float r)
{
	cameraDirRadian += r;
	
	if (cameraDirRadian < -6.29) cameraDirRadian = 0;
	if (cameraDirRadian > 6.29) cameraDirRadian = 0;
	
	cameraDir.x = (sin(cameraDirRadian) * cameraRadius);
	cameraDir.y = (cos(cameraDirRadian) * cameraRadius);
	
	cameraDirDegrees = toDegrees(cameraDirRadian);
}

void Game::setMissionStatus(int status)
{
	if (status == MISSION_INPROGRESS)
	{
		missionOverTimer = 0;
		missionStatus = MISSION_INPROGRESS;
		return;
	}
	
	if ((missionStatus != MISSION_INPROGRESS) && (status != MISSION_LOADING))
	{
		return;
	}
	
	missionStatus = status;
	
	switch (missionStatus)
	{
		case MISSION_COMPLETE:
		case MISSION_LOADING:
		case MISSION_QUITTING:
			missionOverTimer = 0;
			break;
		
		case MISSION_PLAYER_OUT:
			missionOverTimer = 200;
			break;
			
		case MISSION_FAILED:
			missionOverTimer = 350;
			break;
		
		default:
			missionOverTimer = 0;
			break;
	}
}

void Game::alterStat(const char *statName, float value)
{
	statistics.alterProperty(statName, value);
}

float Game::getStatistic(const char *statName)
{
	return statistics.getFloat(statName, 0);
}

void Game::load(Properties *properties)
{
	mission = properties->getString("mission", "NULL");
	
	if (!retrying)
	{
		hours = properties->getInt("hours", 0);
		minutes = properties->getInt("minutes", 0);
		seconds = properties->getInt("seconds", 0);
		totalTime = properties->getInt("totalTime", 0);
	}
	
	remainingMinutes = properties->getInt("remainingMinutes", 0);
	remainingSeconds = properties->getInt("remainingSeconds", 0);
	totalRemaining = properties->getInt("totalRemaining", 0);
	
	saveSlot = properties->getInt("saveSlot", -1);
	
	weaponIndex = properties->getInt("weaponIndex", 0);
	
	String s;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		s.setText("weapon%d", i);
		
		weaponId[i] = properties->getInt(s.getText(), 0);
	}
		
	for (int i = 0 ; i < 5 ; i++)
	{
		s.setText("item%d", i);
		
		itemId[i] = properties->getInt(s.getText(), 0);
	}
	
	teekaWeapon = properties->getInt("teekaWeapon", WEP_PISTOL);
	
	currentOrders = properties->getInt("currentOrders", ORDERS_FOLLOW);
	dropGrappleHooks = properties->getInt("dropGrappleHooks", 0);
	
	cameraDirRadian = properties->getFloat("cameraDirRadian", cameraDirRadian);
	cameraDir.x = properties->getFloat("cameraDir.x", cameraDir.x);
	cameraDir.y = properties->getFloat("cameraDir.y", cameraDir.y);
	cameraDirDegrees = properties->getFloat("cameraDirDegrees", cameraDirDegrees);
	cameraHeight = properties->getFloat("cameraHeight", cameraHeight);
}

void Game::save(FILE *fp)
{
	debug(("Game::save()\n"));

	Properties properties;
	properties.setName("Game");
	
	properties.setProperty("mission", mission.getText());
	
	properties.setProperty("hours", hours);
	properties.setProperty("minutes", minutes);
	properties.setProperty("seconds", seconds);
	properties.setProperty("totalTime", totalTime);
	
	properties.setProperty("remainingMinutes", remainingMinutes);
	properties.setProperty("remainingSeconds", remainingSeconds);
	properties.setProperty("totalRemaining", totalRemaining);
	
	properties.setProperty("saveSlot", saveSlot);
	
	properties.setProperty("weaponIndex", weaponIndex);
	
	String s;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		s.setText("weapon%d", i);
		
		if (weapon[i] != NULL)
		{
			properties.setProperty(s.getText(), weapon[i]->getUniqueId());
		}
		else
		{
			properties.setProperty(s.getText(), 0);
		}
	}
		
	for (int i = 0 ; i < 5 ; i++)
	{
		s.setText("item%d", i);
		
		if (item[i] != NULL)
		{
			properties.setProperty(s.getText(), item[i]->getUniqueId());
		}
		else
		{
			properties.setProperty(s.getText(), 0);
		}
	}
	
	properties.setProperty("currentOrders", currentOrders);
	properties.setProperty("dropGrappleHooks", dropGrappleHooks);
	
	properties.setProperty("teekaWeapon", teekaWeapon);
	
	properties.setProperty("cameraDirRadian", cameraDirRadian);
	properties.setProperty("cameraDir.x", cameraDir.x);
	properties.setProperty("cameraDir.y", cameraDir.y);
	properties.setProperty("cameraDirDegrees", cameraDirDegrees);
	properties.setProperty("cameraHeight", cameraHeight);
	
	properties.save(fp);
	
	statistics.save(fp);

	debug(("Game::save() : Done\n"));
}

Game Game::instance;
