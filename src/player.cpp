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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USAf.

*/

#include "player.h"

void removeSpecialWeapons()
{
	bool remove;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		if (game->weapon[i] == NULL)
		{
			continue;
		}
		
		remove = false;
		
		switch (game->weapon[i]->definition->type)
		{
			case WEP_ICE_GUN:
			case WEP_FLAMETHROWER:
			case WEP_DESIMILATOR:
				remove = true;
				break;
				
			default:
				remove = false;
				break;
		}
		
		if (remove)
		{
			debug(("removeSpecialWeapons - Removing '%s'\n", game->weapon[i]->getName()));
			
			if (game->selectedWeapon == game->weapon[i])
			{
				game->selectedWeapon = game->weapon[0];
				game->weaponIndex = 0;
			}
			
			game->weapon[i]->setOwner(NULL);
			game->weapon[i] = NULL;
		}
	}
	
	// June 2009 fix for carrying items between levels
	for (int i = 0 ; i < 5 ; i++)
	{
		game->item[i] = NULL;
		game->itemId[i] = 0;
	}
}

WeaponDef *getRandomPlayerAmmoType()
{
	WeaponDef *rtn = NULL;
	
	if ((game->weapon[1] == NULL) && (game->weapon[2] == NULL))
	{
		return NULL;
	}
	else if ((game->weapon[1] != NULL) && (game->weapon[2] != NULL))
	{
		rtn = (WeaponDef*)game->weapon[1 + rand() % 2]->definition;
	}
	else if (game->weapon[1] == NULL)
	{
		rtn = (WeaponDef*)game->weapon[2]->definition;
	}
	else if (game->weapon[2] == NULL)
	{
		rtn = (WeaponDef*)game->weapon[1]->definition;
	}
	
	// Plasma rifles are very powerful... Don't drop them too often or it makes the game stupidly easy
	if ((rtn != NULL) && (rtn->type == WEP_PLASMA_RIFLE))
	{
		if (mission->enemyLevel < 5)
		{
			if ((rand() % 3) != 0)
			{
				rtn = NULL;
			}
		}
		else
		{
			if ((rand() % 2) == 0)
			{
				rtn = NULL;
			}
		}
	}
	
	// don't drop desimilator ammo...
	if (rtn != NULL)
	{
		switch (rtn->type)
		{
			case WEP_DESIMILATOR:
			case WEP_ICE_GUN:
			case WEP_FLAMETHROWER:
				rtn = NULL;
				break;
				
			default:
				break;
		}
	}

	return rtn;
}

void bobStunnedTouch(Entity *other)
{
	if (other == NULL)
	{
		return;
	}
	
	switch (other->entityType)
	{
		case ET_SUPPLY_CRATE:
			other->health = 0;
			break;
			
		case ET_BLOB:
		case ET_BIOMECH:
			other->velocity.x = self->velocity.x / 2;
			other->velocity.y = self->velocity.y / 2;
			other->velocity.z = self->velocity.z / 2;
			audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(other->position));
			((Unit*)other)->helpless += 75;
			break;
	}
}

void blobWalkTurn()
{
}

void resetPlayer()
{
	player->helpless = 0;
	player->position = game->checkpoint;
	player->velocity.set(0, 0, 0);
	player->rotation.set(0, 0, 0);
	player->contentType = CONTENTS_SOLID;
	player->flags &= ~EF_BOUNCES;
	game->setMissionStatus(MISSION_INPROGRESS);
}

void calculatePlayerAimDirection()
{
	Vector v, near, far, ray;

	if (game->autoLockTarget)
	{
		game->targetLock = player->position;
		
		float r = toRadians(player->rotation.x);
		
		game->targetLock.x += (sin(r) * 25);
		game->targetLock.y -= (cos(r) * 25);
		
		v = game->targetLock;
		v -= player->position;
		v.normalize();
		
		game->targetLock += (v * 150);
	}
	else
	{
		graphics->convertToWorldCoords(graphics->screenMidX, graphics->screenMidY, &near, &far, &ray);
		
		game->targetLock = near;
		game->targetLock += (ray * 900);
		
		game->targetLock = bsp->getTraceLocation(camera->position, game->targetLock);
		game->targetLock = bbManager->getTraceLocation(camera, camera->position, game->targetLock);
		
		game->aimColor.set(1.0, 1.0, 1.0, 0.2);
		
		if (bbManager->intersectedEntity != NULL)
		{
			switch (bbManager->intersectedEntity->entityType)
			{
				case ET_BLOB:
					game->aimColor.set(0.0, 1.0, 0.0, 0.75);
					break;
					
				case ET_BIOMECH:
				case ET_BOSS:
					game->aimColor.set(1.0, 0.0, 0.0, 0.75);
					break;
					
				case ET_SUPPLY_CRATE:
					game->aimColor.set(1.0, 0.0, 0.0, 0.75);
					break;
					
				case ET_WINDOW:
					game->aimColor.set(1.0, 0.0, 0.0, 0.75);
					break;
					
				case ET_STRUCTURE:
					if (!(bbManager->intersectedEntity->flags & EF_IMMORTAL))
					{
						game->aimColor.set(1.0, 0.0, 0.0, 0.75);
					}
					break;
					
				default:
					break;
			}
		}
	}
}

void playerTargetEnemy()
{
	Vector sight;

	float distance = 0;
	float targetDistance = (player->target == NULL) ? PLAYER_TARGET_RANGE : Math::getDistance(player->position, player->target->position);
	
	sight = player->position;
	sight.z += 3;
	
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if ((!unit->canBeDamaged()) || (unit->health <= 0) || (!camera->frustum.containsSphere(unit->position, 6)) || (unit->flags & EF_NO_TARGET))
		{
			continue;
		}
		
		distance = Math::getDistance(player->position, unit->position);
		
		if (distance < targetDistance)
		{
			if (unitCanSeeTarget(player, unit, sight))
			{
				targetDistance = distance;
				player->target = unit;
			}
		}
	}
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		if ((boss->health <= 0) || (boss->entityType == ET_NEUTRAL) || (!camera->frustum.containsSphere(boss->position, 6)) || (boss->flags & EF_VANISHED))
		{
			continue;
		}
		
		distance = Math::getDistance(player->position, boss->position);
		
		if (distance < targetDistance)
		{
			if (unitCanSeeTarget(player, boss, sight))
			{
				targetDistance = distance;
				player->target = boss;
			}
		}
	}
}

void playerCycleTargets()
{
	float distance = 0;
	float targetDistance = PLAYER_TARGET_RANGE;
	float currentDistance = Math::getDistance(player->position, player->target->position);
	Entity *oldTarget = player->target;
	
	Vector sight = player->position;
	sight.z += 3;
	
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if ((!unit->canBeDamaged()) || (unit->health <= 0) || (!camera->frustum.containsSphere(unit->position, 6)) || (unit->flags & EF_NO_TARGET))
		{
			continue;
		}
		
		distance = Math::getDistance(player->position, unit->position);
		
		if ((distance < targetDistance) && (distance > currentDistance))
		{
			if (unitCanSeeTarget(player, unit, sight))
			{
				targetDistance = distance;
				player->target = unit;
			}
		}
	}
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		if ((boss->health <= 0) || (boss->entityType == ET_NEUTRAL) || (!camera->frustum.containsSphere(boss->position, 6)) || (boss->flags & EF_VANISHED))
		{
			continue;
		}
		
		distance = Math::getDistance(player->position, boss->position);
		
		if ((distance < targetDistance) && (distance > currentDistance))
		{
			if (unitCanSeeTarget(player, boss, sight))
			{
				targetDistance = distance;
				player->target = boss;
			}
		}
	}
	
	if (oldTarget == player->target)
	{
		playerTargetEnemy();
	}
}

void playerSelectBestWeapon()
{
	game->mustChangeWeapon = false;
	
	if ((game->weapon[1] != NULL) && (game->weapon[1]->currentAmmo > 0))
	{
		game->selectedWeapon = game->weapon[1];
		game->weaponIndex = 1;
		player->reload = 100;
		return;
	}
	else if ((game->weapon[2] != NULL)&& (game->weapon[2]->currentAmmo > 0))
	{
		game->selectedWeapon = game->weapon[2];
		game->weaponIndex = 2;
		player->reload = 100;
		return;
	}
	
	game->selectedWeapon = game->weapon[0];
	game->weaponIndex = 0;
	player->reload = 100;
}

void playerIssueOrders()
{
	if (engine->keyState[SDLK_7] || engine->keyState[SDLK_KP7])
	{
		game->setInfoMessage(INFO_HINT, _("Orders : Follow"));
		game->currentOrders = ORDERS_FOLLOW;
		engine->keyState[SDLK_7] = 0;
		engine->keyState[SDLK_KP7] = 0;
		resetBlobOrders();
	}
	
	if (engine->keyState[SDLK_8] || engine->keyState[SDLK_KP8])
	{
		game->setInfoMessage(INFO_HINT, _("Orders : Wait"));
		game->currentOrders = ORDERS_WAIT;
		engine->keyState[SDLK_8] = 0;
		engine->keyState[SDLK_KP8] = 0;
		resetBlobOrders();
	}
	
	if (engine->keyState[SDLK_9] || engine->keyState[SDLK_KP9])
	{
		game->setInfoMessage(INFO_HINT, _("Orders : Use"));
		game->currentOrders = ORDERS_USE;
		engine->keyState[SDLK_9] = 0;
		engine->keyState[SDLK_KP9] = 0;
		resetBlobOrders();
	}
	
	if (engine->keyState[SDLK_0] || engine->keyState[SDLK_KP0])
	{
		game->setInfoMessage(INFO_HINT, _("Orders : Retreat"));
		game->currentOrders = ORDERS_RETREAT;
		engine->keyState[SDLK_0] = 0;
		engine->keyState[SDLK_KP0] = 0;
		resetBlobOrders();
	}
}

void playerFireWeapon(Weapon *weapon)
{
	Vector v;
	String s;

	if ((weapon->currentAmmo > 0) || (game->cheatAmmo))
	{
		// can't fire underwater :)
		if ((player->liquidLevel > 1) && ((WeaponDef*)weapon->definition->type != WEP_PISTOL))
		{
			return;
		}
		
		if (player->target != NULL)
		{
			faceTarget(player);
			
			v = player->target->position;
			
			switch (weapon->definition->type)
			{
				case WEP_GRENADE:
				case WEP_GRENADE_LAUNCHER:
				case WEP_CLUSTER_LAUNCHER:
					v = getLeadingTargetPosition(player, player->target);
					break;
			}
			
			fireWeapon(player, (WeaponDef*)weapon->definition, player->position, v);
		}
		else
		{
			v = player->position;
			
			//calculatePlayerAimDirection();
			
			fireWeapon(player, (WeaponDef*)weapon->definition, v, game->targetLock);
		}
		
		WeaponDef *weaponDef = (WeaponDef*)weapon->definition;
			
		if (!weaponDef->unlimited)
		{
			if (!game->cheatAmmo)
			{
				weapon->currentAmmo--;
			}
			textureManager->removeTexture("str_current_ammo");
			
			s.setText("str_weapon_%d", game->weaponIndex);
			textureManager->removeTexture(s.getText());
				
			if (weapon->currentAmmo == 0)
			{
				game->mustChangeWeapon = true;
				
				if (weaponDef->type == WEP_DESIMILATOR)
				{
					weapon->currentAmmo = -100;
				}
			}
		}
		
		if (weaponDef->type == WEP_SHOTGUN)
		{
			if ((weapon->currentAmmo % 2) != 0)
			{
				player->reload *= 0.25;
			}
		}
	}
	else
	{
		audio->playSound(SND_WEAPON_EMPTY, CH_PLAYER, 128);
		player->reload = 8;
	}
}

void doPlayerCycleWeapon(int dir)
{
	Math::wrap(&(game->weaponIndex += dir), 0, 3);
		
	while (game->weapon[game->weaponIndex] == NULL)
	{
		Math::wrap(&(game->weaponIndex += dir), 0, 3);
	}
		
	game->selectedWeapon = game->weapon[game->weaponIndex];
		
	game->resetControl(CTRL_CYCLE_WEAPON);
		
	textureManager->removeTexture("str_current_ammo");
}

void doPlayerWeapons()
{
	if ((player->target == NULL) && (game->autoLockTarget))
	{
		playerTargetEnemy();
	}
	
	if (game->isControl(CTRL_LOCK_TARGET))
	{
		player->target = NULL;
		
		game->autoLockTarget = !game->autoLockTarget;
		game->cameraAimHeight = 100;
		
		game->resetControl(CTRL_LOCK_TARGET);
	}
	
	if (game->isControl(CTRL_CYCLE_TARGET))
	{
		if (game->autoLockTarget)
		{
			if (player->target != NULL)
			{
				playerCycleTargets();
			}
			else
			{
				playerTargetEnemy();
			}
		}
		
		game->resetControl(CTRL_CYCLE_TARGET);
	}
	
	if (game->isControl(CTRL_CYCLE_WEAPON))
	{
		doPlayerCycleWeapon(1);
	}
	
	if (engine->keyState[SDLK_1])
	{
		game->weaponIndex = 0;
		game->selectedWeapon = game->weapon[game->weaponIndex];
		engine->keyState[SDLK_1] = 0;
	}
	
	if (engine->keyState[SDLK_2])
	{
		if (game->weapon[1] != NULL)
		{
			game->weaponIndex = 1;
			game->selectedWeapon = game->weapon[1];
		}
		engine->keyState[SDLK_2] = 0;
	}
	
	if (engine->keyState[SDLK_3])
	{
		if (game->weapon[2] != NULL)
		{
			game->weaponIndex = 2;
			game->selectedWeapon = game->weapon[2];
		}
		engine->keyState[SDLK_3] = 0;
	}
	
	if (engine->keyState[SDLK_4])
	{
		game->weaponIndex = 3;
		
		if (game->weapon[game->weaponIndex]->currentAmmo > 0)
		{
			game->selectedWeapon = game->weapon[3];
		}
		engine->keyState[SDLK_4] = 0;
	}
	
	if (mouse->buttonState[SDL_BUTTON_WHEELUP])
	{
		doPlayerCycleWeapon(-1);
		mouse->buttonState[SDL_BUTTON_WHEELUP] = 0;
	}
	
	if (mouse->buttonState[SDL_BUTTON_WHEELDOWN])
	{
		doPlayerCycleWeapon(1);
		mouse->buttonState[SDL_BUTTON_WHEELDOWN] = 0;
	}
	
	if (game->isControl(CTRL_FIRE))
	{
		if (player->target != NULL)
		{
			game->autoFaceTimer = 100;
		}
		
		if (player->reload == 0)
		{
			playerFireWeapon(game->selectedWeapon);
		}
	}
	else if (game->isControl(CTRL_QUICK_GRENADE))
	{
		if (game->weapon[3] != NULL)
		{
			if (player->target != NULL)
			{
				game->autoFaceTimer = 100;
			}
			
			if (player->reload == 0)
			{
				playerFireWeapon(game->weapon[3]);
				textureManager->removeTexture("str_weapon_3");
			}
		}
	}
	
	if ((game->mustChangeWeapon) && (!game->isControl(CTRL_FIRE)))
	{
		playerSelectBestWeapon();
	}
	
	if (game->autoFaceTimer > 0)
	{
		Math::limit(&(--game->autoFaceTimer), 0, 200);
		
		faceTarget(player);
	}
}

void movePlayerCamera(float turnSpeed)
{
	if (((!game->autoLockTarget) && (game->cameraAimHeight == 100)) || (game->autoLockTarget))
	{
		game->cameraHeight += (turnSpeed * engine->getTimeDifference(TD_LOGIC));
		Math::limit(&game->cameraHeight, 10, 115);
	}
		
	if (((!game->autoLockTarget) && (game->cameraHeight == 10)) || (game->autoLockTarget))
	{
		float dir = (turnSpeed < 0) ? 5 : -5;
		game->cameraAimHeight += (dir * engine->getTimeDifference(TD_LOGIC));
		Math::limit(&game->cameraAimHeight, 100, 500);
	}
}

void movePlayerAnalog(float *x, float *y)
{
	if (engine->joystick == NULL)
	{
		return;
	}
	
	if (engine->axis1X <= -game->joystickDeadZone)
	{
		*x  = engine->axis1X;
 		*x /= ANALOG_MOVE;
	}
	
	if (engine->axis1X >= game->joystickDeadZone)
	{
		*x  = engine->axis1X;
		*x /= ANALOG_MOVE;
	}
	
	if (engine->axis1Y >= game->joystickDeadZone)
	{
		*y  = -engine->axis1Y;
		*y /= ANALOG_MOVE;
	}
	
	if (engine->axis1Y <= -game->joystickDeadZone)
	{
		*y  = -engine->axis1Y;
		*y /= ANALOG_MOVE;
	}
	
	if (*x > 1)
	{
		*x = 1;
	}
	
	else if (*x < -1)
	{
		*x = -1;
	}
	
	if (*y > 1)
	{
		*y = 1;
	}
	
	else if (*y < -1)
	{
		*y = -1;
	}
}

void movePlayerKeys(float *x, float *y)
{
	if (game->isControl(CTRL_LEFT))
	{
		*x = -1;
	}
	
	if (game->isControl(CTRL_RIGHT))
	{
		*x = 1;
	}
	
	if (game->isControl(CTRL_UP))
	{
		*y = 1;
	}
	
	if (game->isControl(CTRL_DOWN))
	{
		*y = -1;
	}
}

void movePlayer()
{
	if (player->target != NULL)
	{
		player->target->referenced = true;
		
		if (Math::getDistance(player->position.x, player->position.y, player->target->position.x, player->target->position.y) > PLAYER_TARGET_RANGE)
		{
			player->target = NULL;
		}
		else if (player->target->health <= 0)
		{
			player->target = NULL;
		}
		else if ((player->target->flags & EF_TELEPORTING) || (player->target->flags & EF_VANISHED))
		{
			player->target = NULL;
		}
		else if (!bsp->canSightTarget(player->position, player->target->position))
		{
			player->target = NULL;
		}
	}
	
	float sx = 0;
	float sy = 0;
	
	float x = 0;
	float y = 0;
	
	movePlayerKeys(&x, &y);
	movePlayerAnalog(&x, &y);
	
	float fx = fabs(x);
	float fy = fabs(y);
	
	if (x < 0)
	{
		sx += (fx * 1.2 * -cos(game->cameraAdjustedRadian));
		sy += (fx * 1.2 * -sin(game->cameraAdjustedRadian));
		player->rotation.x = 270 + toDegrees(game->cameraAdjustedRadian);
	}
	
	else if (x > 0)
	{
		sx += (fx * 1.2 * cos(game->cameraAdjustedRadian));
		sy += (fx * 1.2 * sin(game->cameraAdjustedRadian));
		player->rotation.x = 90 + toDegrees(game->cameraAdjustedRadian);
	}
	
	if (y > 0)
	{
		sx += (fy * 1.2 * -sin(game->cameraAdjustedRadian));
		sy += (fy * 1.2 * cos(game->cameraAdjustedRadian));
		player->rotation.x = 180 + toDegrees(game->cameraAdjustedRadian);
		player->rotation.x += (x * -45);
	}
	
	else if (y < 0)
	{
		sx += (fy * 1.2 * sin(game->cameraAdjustedRadian));
		sy += (fy * 1.2 * -cos(game->cameraAdjustedRadian));
		player->rotation.x = toDegrees(game->cameraAdjustedRadian);
		player->rotation.x += (x * 45);
	}
	
	if (sx != 0)
	{
		player->velocity.x = sx;
	}
	
	if (sy != 0)
	{
		player->velocity.y = sy;
	}
	
	if (game->isControl(CTRL_JUMP))
	{
		if ((player->liquidLevel == 0) && (player->flags & EF_ONGROUND))
		{
			player->velocity.z = 4.0 / 3.0;
			engine->keyState[SDLK_SPACE] = 0;
			player->flags &= ~EF_ONGROUND;
		}
		else if (player->liquidLevel > 0)
		{
			player->velocity.z += (0.01 * engine->getTimeDifference(TD_LOGIC));
		}
	}
	else if (player->liquidLevel > 0)
	{
		if (player->velocity.z > -0.5)
		{
			player->velocity.z -= (0.0025 * engine->getTimeDifference(TD_LOGIC));
		}
	}
	
	if (game->isControl(CTRL_STATUS))
	{
		game->resetControl(CTRL_STATUS);
		showMissionStatusPanel();
	}
	
	#if DEV
	if (engine->keyState[SDLK_f])
	{
		player->velocity.z = 0;
		player->position.z += (engine->getTimeDifference(TD_LOGIC));
	}
	
	if (engine->keyState[SDLK_v])
	{
		player->velocity.z = 0;
		player->position.z -= (engine->getTimeDifference(TD_LOGIC));
	}
	#endif
}

/*
 * Keeps the player a certain distance away from the edges by tracing down
 * in a large box around them. If it's possible to hit the target then the player
 * isn't in a safe place
*/
void assignCheckPoint()
{
	Vector position;

	if (!(player->flags & EF_ONGROUND))
	{
		return;
	}
	
	if (!(player->contentType & CONTENTS_SOLID))
	{
		return;
	}
	
	position = player->position;
	position.x -= 30;
	position.y -= 30;
	position.z -= 4;
	
	if (bsp->canSightTarget(player->position, position))
	{
		return;
	}
	
	position = player->position;
	position.x += 30;
	position.y -= 30;
	position.z -= 4;
	
	if (bsp->canSightTarget(player->position, position))
	{
		return;
	}
	
	position = player->position;
	position.x += 30;
	position.y += 30;
	position.z -= 4;
	
	if (bsp->canSightTarget(player->position, position))
	{
		return;
	}
	
	position = player->position;
	position.x -= 30;
	position.y += 30;
	position.z -= 4;
	
	if (bsp->canSightTarget(player->position, position))
	{
		return;
	}
	
	game->checkpoint = player->position;
}

void handlePlayerCamera()
{
	// ======= Mouse ======
	
	int x, y;
	float turnSpeed;
	
	SDL_GetRelativeMouseState(&x, &y);
	
	turnSpeed = -x;
	turnSpeed /= game->mouseSensitivity;
	
	game->changeCameraRotation(turnSpeed * engine->getTimeDifference(TD_LOGIC));
	
	turnSpeed = y;
	turnSpeed /= (game->mouseSensitivity / 50);
	
	if (turnSpeed != 0)
	{
		movePlayerCamera(turnSpeed);
	}
	
	// === Joypad ======
	
	if ((engine->axis2X <= -game->joystickDeadZone) || (engine->axis2X >= game->joystickDeadZone))
	{
		turnSpeed = -engine->axis2X / 2000000.0;
		
		game->changeCameraRotation(turnSpeed * engine->getTimeDifference(TD_LOGIC));
	}
	
	if ((engine->axis2Y <= -game->joystickDeadZone) || (engine->axis2Y >= game->joystickDeadZone))
	{
		turnSpeed = engine->axis2Y / 35000.0;
		
		movePlayerCamera(turnSpeed);
	}
	
	// === Keyboard ======
	
	if (game->isControl(CTRL_ROTATE_LEFT))
	{
		game->changeCameraRotation(2 * PI * engine->getTimeDifference(TD_LOGIC) / 120.0);
	}
	
	if (game->isControl(CTRL_ROTATE_RIGHT))
	{
		game->changeCameraRotation(-2 * PI * engine->getTimeDifference(TD_LOGIC) / 120.0);
	}
}

void playerDropBreadCrumb()
{
	crumbBox.ageCrumbs(engine->getTimeDifference(TD_LOGIC));

	crumbBox.addCrumb(player->position, false);
}

void doPlayer()
{
	if (game->missionStatus == MISSION_INPROGRESS)
	{
		if (player->contentType & CONTENTS_SLIME)
		{
			player->applyDamage(4, DAMAGE_SPECIAL);
			player->shield = -2.5;
			player->helpless = RAND_MAX;
			game->setMissionStatus((player->health > 0) ? MISSION_PLAYER_OUT : MISSION_FAILED);
			game->alterStat(_("Times Fallen In Slime"), 1);
		}
		else if (player->contentType & CONTENTS_LAVA)
		{
			player->applyDamage(8, DAMAGE_SPECIAL);
			player->shield = -2.5;
			player->helpless = RAND_MAX;
			game->setMissionStatus((player->health > 0) ? MISSION_PLAYER_OUT : MISSION_FAILED);
			game->alterStat(_("Times Fallen In Lava"), 1);
		}
		else if (player->contentType & CONTENTS_PLAYERCLIP)
		{
			player->applyDamage(8, DAMAGE_SPECIAL);
			player->shield = -2.5;
			player->helpless = RAND_MAX;
			game->setMissionStatus((player->health > 0) ? MISSION_PLAYER_OUT : MISSION_FAILED);
		}
	}
	
	if (game->cheatHealth)
	{
		player->health = player->maxHealth;
	}
	
	if (game->cheatShield)
	{
		player->shield = player->maxShield;
	}
	
	if (game->cheatOxygen)
	{
		player->oxygen = MAX_OXYGEN_IND;
	}
	
	if (!(player->flags & (EF_DYING|EF_DEAD)))
	{
		if (player->oxygen == 0)
		{
			if ((((int)graphics->getAnimTimer()) % 45) <= 0)
			{
				audio->playSound(SND_DROWNING, CH_PLAYER, 128);
			}
		}
	}
	
	handlePlayerCamera();
	
	if ((player->health <= 0) || (player->action != NULL) || (player->helpless))
	{
		return;
	}
	
	assignCheckPoint();
	
	for (int i = 0 ; i < engine->getInertiaRate() ; i++)
	{
		player->velocity.x *= 0.95;
		player->velocity.y *= 0.95;
	}
	
	player->helpless = 0;
	
	player->rotation.z = 0;
	
	movePlayer();
	
	if (!game->autoLockTarget)
	{
		player->rotation.x = 180 + toDegrees(game->cameraAdjustedRadian);
		
		//calculatePlayerAimDirection();
	}
	
	doPlayerWeapons();
	
	playerIssueOrders();
	
	playerDropBreadCrumb();
}
