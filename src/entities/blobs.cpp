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

#include "blobs.h"

void blobFollowOrders();
void blobDoOrderFollow();

int getAdjustedHealthChance(int in, int minimum)
{
	float healthPercent = 100;
	
	// help out the player or teeka if they're in trouble...
	for (Unit *blob = (Unit*)entityManager->blobList.getFirstElement() ; blob != NULL ; blob = (Unit*)blob->next)
	{
		healthPercent = min(blob->getHealthPercent(), healthPercent);
	}
	
	debug(("Minimum Health = %.2f\n", healthPercent));
	
	if (healthPercent >= 25)
	{
		return in;
	}
	
	in = (int)(in * (healthPercent / 15));
	
	Math::limit(&in, minimum, 100);
	
	debug(("getAdjustedHealthChance() - %d (min = %d)\n", in, minimum));
	
	return in;
}

void blobsFinishedOrders()
{
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if ((unit == player) || (unit->definition->type == BLOB_MIA))
		{
			continue;
		}
		
		if (!unit->completedOrders)
		{
			return;
		}
	}

	game->currentOrders = ORDERS_FOLLOW;
}

void blobJumping()
{
	if (self->flags & EF_ONGROUND)
	{
		self->action = blobDoOrderFollow;
	}
	
	self->thinkTime = 1;
}

bool blobJumpUp()
{
	if (!(player->flags & EF_ONGROUND))
	{
		return false;
	}
	
	Unit *unit = (Unit*)self;
	
	Vector target = (unit->currentCrumb == NULL) ? player->position : unit->currentCrumb->position;
	float zDist = (target.z - self->position.z);
	
	if ((zDist > 3) && (zDist < 50))
	{
		// are we blocked..?
		float distance = Math::getDistance(self->oldPosition, self->position);
		
		if (distance > 0.1)
		{
			return false;
		}
		
		self->velocity = target;
		self->velocity -= self->position;
		self->velocity.normalize();
		
		self->velocity.z = 4.0 / 3.0;
				
		self->action = &blobJumping;
				
		self->flags &= ~EF_ONGROUND;
		
		return true;
	}
	
	return false;
}

bool blobJumpGap()
{
	Vector start, end, velocity;
	
	bool canMakeJump = false;

	Unit *unit = (Unit*)self;
	Vector target = (unit->currentCrumb == NULL) ? player->position : unit->currentCrumb->position;
	
	if (unitIsAtEdge(unit))
	{
		if (fabs(target.z - self->position.z) > 12)
		{
			return false;
		}
		
		// attempt to jump to next ledge
		if (unit->flags & EF_ONGROUND)
		{
			velocity = target;
			velocity -= unit->position;
			velocity.normalize();
		}
		
		canMakeJump = false;
		
		start = unit->position;
		
		for (int i = 0 ; i < 150 ; i += 6)
		{
			start += (velocity * 6);
			end = start;
			end.z -= 6;
			
			if (!bsp->canSightTarget(start, end))
			{
				canMakeJump = true;
				break;
			}
			
			if (bbManager->canSightSolid(unit, start, end))
			{
				canMakeJump = true;
				break;
			}
		}
		
		if (canMakeJump)
		{
			float distance = (Math::getDistance(unit->position, start) / 85);
			
			unit->velocity = target;
			unit->velocity -= unit->position;
			unit->velocity.normalize();
			unit->velocity = unit->velocity * distance;
			unit->velocity.z = distance;
			Math::limit(&unit->velocity.z, 1.25, 2.5);
				
			unit->action = &blobJumping;
				
			unit->flags &= ~EF_ONGROUND;
		}
	}
	
	return canMakeJump;
}

void blobRushTarget()
{
	Unit *unit = (Unit*)self;
	
	unit->velocity = unit->target->position;
	
	unit->velocity.x += Math::rrand(-10, 10);
	unit->velocity.y += Math::rrand(-10, 10);
	
	unit->velocity -= unit->position;
	unit->velocity.normalize();
	
	float speed = Math::rrand(50, 100);
	speed /= 100;
	unit->velocity = unit->velocity * speed;
	
	if ((rand() % 3) == 0)
	{
		unit->velocity.x *= 0.1;
		unit->velocity.y *= 0.1;
		unit->velocity.z = Math::rrand(75, 130);
		unit->velocity.z *= 0.01;
	}
	
	faceTarget((Unit*)self);
}

void blobDodgeTarget()
{
	self->velocity.x = Math::rrand(-5, 5);
	self->velocity.y = Math::rrand(-5, 5);
	
	self->velocity.x /= Math::rrand(4, 5);
	self->velocity.y /= Math::rrand(4, 5);
	
	if ((rand() % 3) == 0)
	{
		self->velocity.x *= 0.1;
		self->velocity.y *= 0.1;
		self->velocity.z = Math::rrand(75, 130);
		self->velocity.z *= 0.01;
	}
	
	faceTarget((Unit*)self);
}

void blobWander()
{
	Unit *unit = (Unit*)self;
	
	unit->velocity.x = Math::rrand(-20, 20);
	unit->velocity.y = Math::rrand(-20, 20);
	unit->velocity.x /= 100;
	unit->velocity.y /= 100;

	float angle = atan2(unit->velocity.y, unit->velocity.x);
	unit->rotation.x = 90 + toDegrees(angle);
		
	unit->thinkTime = Math::rrand(200, 400);
	
	// always try and swim up
	if (unit->liquidLevel == 2)
	{
		unit->velocity.z += (0.01 * engine->getTimeDifference(TD_LOGIC));
		unit->thinkTime = Math::rrand(50, 75);
	}
	
	return;
}

void blobTeleport()
{
	addTeleporterParticle(self->position);
	
	if (self->custom == 0)
	{
		audio->playSound(SND_TELEPORT1, CH_ANY, camera->getSoundDistance(self->position));
	}
	
	if (self->custom < 10)
	{
		self->rotation.x += self->custom;
	}
	else
	{
		self->rotation.x += 10;
	}
	
	self->custom += (0.05 * engine->getTimeDifference(TD_LOGIC));
	
	Math::limit(&self->custom, 0, 20);
	
	if (self->custom >= 10)
	{
		self->flags |= EF_WEIGHTLESS;
		self->velocity.z = 1;
	}
		
	if (self->custom >= 15)
	{
		addTeleportParticles(self->position);
		self->flags |= EF_EXITTED;
		
		if (self->definition->type == BLOB_MIA)
		{
			game->setInfoMessage(INFO_GOOD, _("Rescued %s"), self->getName());
			fireTriggers(self->getName(), TRIGGER_TYPE_BLOB_RESCUED);
			fireTriggers("ANY_MIA", TRIGGER_TYPE_BLOB_RESCUED);
		}
	}
}

void blobPreTeleport()
{
	self->custom = 0;
	
	self->action = &blobTeleport;
}

void miaBlobThink()
{
	if (!bsp->canSightTarget(self->position, player->position))
	{
		if ((rand() % 3) == 0)
		{
			blobWander();
		}
		else
		{
			self->velocity.x = 0;
			self->velocity.y = 0;
			
			self->thinkTime = Math::rrand(200, 400);
		}
		
		return;
	}
	
	self->velocity.x = 0;
	self->velocity.y = 0;
	
	faceLocation((Unit*)self, player->position);
	
	float distance = Math::getDistance(self->position, player->position);
	
	if (distance <= 10)
	{
		int slot = game->getItemSlot(_("Teletag"));
		
		if ((slot == -1) && (!game->cheatItems))
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Teletag required"));
			return;
		}
		
		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to attach Teletag"));
		
		if (game->isControl(CTRL_ACTION))
		{
			if (!game->cheatItems)
			{
				game->item[slot]->itemPower--;
				
				if (game->item[slot]->itemPower == 0)
				{
					game->removeItem(_("Teletag"));
				}
			}
			
			String s;
			s.setText("str_item_%d", slot);
			textureManager->removeTexture(s.getText());
			
			game->resetControl(CTRL_ACTION);
			
			blobPreTeleport();
		}
	}
}

void assimiatedBlobThink()
{
	Unit *unit = (Unit*)self;
	
	unit->helpless = 1;
}

void teleportOutBlob(const char *name)
{
	debug(("teleportOutBlob() - '%s'\n", name));
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->name == name)
		{
			unit->custom = 0;
			unit->action = &blobTeleport;
			unit->thinkTime = 0;
		}
	}
}

void blobFollowCrumb(Unit *unit)
{
	float distance;

	if (unit->currentCrumb == NULL)
	{
		unit->crumbChaseTime = 0;
	
		unit->currentCrumb = crumbBox.findYoungestCrumb(unit->position);
		
		if (unit->currentCrumb == NULL)
		{
			debug(("%s cannot find crumb trail to follow\n", unit->name.getText()));
			
			// try and swim up
			if (unit->liquidLevel == 2)
			{
				unit->thinkTime = Math::rrand(1, 5);
				unit->action = blobDoOrderFollow;
				unit->velocity.z = 1.5;
				return;
			}
			
			unit->crumbLost = true;
			
			blobWander();
			return;
		}
		
		unit->crumbChaseTime = 200;
		unit->crumbsConsumed = 0;
		
		debug(("%s following first crumb\n", unit->name.getText()));
	}
	
	distance = Math::getDistance(unit->position, unit->currentCrumb->position);
	
	if (distance > (unit->currentCrumb->threshold * engine->getTimeDifference(TD_LOGIC)))
	{
		float z = unit->velocity.z;
		unit->velocity = unit->currentCrumb->position;
		unit->velocity -= unit->position;
		unit->velocity.normalize();
		unit->velocity.x /= 0.8;
		unit->velocity.y /= 0.8;
		
		if (unit->liquidLevel != 2)
		{
			unit->velocity.z = z;
		}
		
		faceLocation(unit, unit->currentCrumb->position);
		
		blobJumpUp();
		blobJumpGap();
		
		unit->crumbChaseTime -= engine->getTimeDifference(TD_LOGIC);
		
		if (unit->crumbChaseTime <= 0)
		{
			debug(("%s can't get to next crumb - Wandering\n", unit->name.getText()));
			
			unit->currentCrumb = NULL;
			unit->crumbLost = true;
			blobWander();
		}
	}
	else
	{
		unit->currentCrumb = (Crumb*)unit->currentCrumb->next;
		
		unit->crumbChaseTime = 200;
		unit->crumbsConsumed++;
		
		// after we've consumed lots of crumbs, recheck
		// that we are following the freshest path...
		if (unit->crumbsConsumed >= 25)
		{
			debug(("%s re-evaluating crumb path...\n", unit->name.getText()))
			unit->currentCrumb = NULL;
		}
	}
	
	unit->action = &blobFollowOrders;
}

void blobFollowBob()
{
	Unit *unit = (Unit*)self;

	Vector sightPosition;
	sightPosition = unit->position;
	sightPosition.z += Math::rrand(0, 25);
	
	float distance = Math::getDistance(unit->position, player->position);
	
	bool canSeePlayer = bsp->canSightTarget(sightPosition, player->position);
	
	unit->crumbLost = false;
	
	if (self->definition->type != BLOB_SOLDIER)
	{
		if (distance > 512 || !canSeePlayer)
		{
			switch (self->definition->type)
			{
				case BLOB_HACKER:
				case BLOB_ENGINEER:
				case BLOB_DEMOLITION:
				case BLOB_TEEKA:
					blobFollowCrumb(unit);
					break;
					
				default:
					blobWander();
					break;
			}
			
			return;
		}
	}
	
	distance = Math::getDistance(unit->position.x, unit->position.y, player->position.x, player->position.y);
	
	unit->currentCrumb = NULL;
	
	unit->showHealthData = true;
	
	unit->action = &blobFollowOrders;
	
	faceLocation(unit, player->position);
	
	if (game->currentOrders != ORDERS_RETREAT)
	{
		if (distance < 30)
		{
			if (!blobJumpUp())
			{
				unit->velocity.x = 0;
				unit->velocity.y = 0;
			}
			
			return;
		}
	}
	
	float z = unit->velocity.z;
	
	unit->velocity = player->position;
	unit->velocity.x += Math::rrand(-6, 6);
	unit->velocity.y += Math::rrand(-6, 6);
	unit->velocity -= unit->position;
	unit->velocity.normalize();
	
	float speed = 8.0 - (distance / 10);
	
	if (game->currentOrders != ORDERS_RETREAT)
	{
		Math::limit(&speed, 0.8, 3.0);
	}
	else
	{
		Math::limit(&speed, 0.6, 1.5);
	}
	
	unit->thinkTime = Math::rrand(5, 10);
	
	unit->velocity.x /= speed;
	unit->velocity.y /= speed;
	unit->velocity.z /= speed;
	
	if (unit->liquidLevel != 2)
	{
		unit->velocity.z = z;
	}
	
	blobJumpUp();
	blobJumpGap();
}

bool blobSightTarget(Unit *unit, int range)
{
	Vector sightPosition;

	unit->target = NULL;
	
	float distance = 0;
	float targetDistance = range;
	
	sightPosition = unit->position;
	sightPosition.z += 10;
	
	for (Unit *enemy = (Unit*)entityManager->enemyList.getFirstElement() ; enemy != NULL ; enemy = (Unit*)enemy->next)
	{
		if ((!enemy->canBeDamaged()) ||( enemy->health <= 0) || (enemy->flags & EF_NOTARGET))
		{
			continue;
		}
		
		distance = Math::getDistance(unit->position, enemy->position);
		
		if (distance < targetDistance)
		{
			if (!unitCanSeeTarget(unit, enemy, sightPosition))
			{
				continue;
			}
			
			targetDistance = distance;
			unit->target = enemy;
		}
	}
	
	if (unit->target != NULL)
	{
		return true;
	}
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		if ((boss->health <= 0) || (boss->entityType == ET_NEUTRAL) || (boss->flags & EF_VANISHED))
		{
			continue;
		}
		
		distance = Math::getDistance(unit->position, boss->position);
		
		if (distance < targetDistance)
		{
			if (!unitCanSeeTarget(unit, boss, sightPosition))
			{
				continue;
			}
			
			targetDistance = distance;
			unit->target = boss;
		}
	}
	
	if (unit->target != NULL)
	{
		return true;
	}
	
	for (Structure *structure = (Structure*)entityManager->structureList.getFirstElement() ; structure != NULL ; structure = (Structure*)structure->next)
	{
		if ((!structure->isMissionTarget) || (structure->health <= 0))
		{
			continue;
		}
		
		distance = Math::getDistance(unit->position, structure->position);
		
		if (distance < targetDistance)
		{
			if (!unitCanSeeTarget(unit, structure, sightPosition))
			{
				continue;
			}
			
			targetDistance = distance;
			unit->target = structure;
		}
	}
	
	return (unit->target != NULL);
}

void goodBlobAttack()
{
	Unit *unit = (Unit*)self;
	
	if (unit->target->flags & EF_DEAD)
	{
		unit->custom = 0;
		unit->action = &blobFollowOrders;
		return;
	}
	
	faceTarget(unit);
	
	if (unit->reload > 0)
	{
		return;
	}
	
	unit->thinkTime = 0;
	unit->action = &goodBlobAttack;
	
	if (unit->currentWeapon->type == WEP_UZI)
	{
		attackTarget(unit);
	}
	else
	{
		attackTargetLeading(unit);
	}
	
	if ((--unit->custom == 0) || (unit->target->health <= 0))
	{
		unit->thinkTime = Math::rrand(50, 100);
		unit->action = &blobFollowOrders;
	}
}

void goodBlobSetFireRate()
{
	Unit *unit = (Unit*)self;
	
	switch (unit->currentWeapon->type)
	{
		case WEP_PISTOL:
			unit->custom = Math::rrand(1, 2);
			break;
			
		case WEP_UZI:
			unit->custom = Math::rrand(3 + mission->enemyLevel, 5 + mission->enemyLevel);
			break;
			
		case WEP_PLASMA_PISTOL:
			unit->custom = Math::rrand(2, mission->enemyLevel + 1);
			break;
			
		case WEP_PLASMA_RIFLE:
			unit->custom = Math::rrand(4, 8);
			break;
			
		default:
			unit->custom = Math::rrand(1, 2);
			break;
	}
}

void goodBlobPreAttack()
{
	switch (self->definition->type)
	{
		case BLOB_HACKER:
		case BLOB_ENGINEER:
		case BLOB_DEMOLITION:
			blobDodgeTarget();
			goodBlobSetFireRate();
			self->action = goodBlobAttack;
			break;
			
		case BLOB_TEEKA:
			if ((rand() % 2) > 0)
			{
				blobRushTarget();
			}
			else
			{
				blobDodgeTarget();
			}
			goodBlobSetFireRate();
			self->action = goodBlobAttack;
			break;
			
		default:
			if ((rand() % 2) > 0)
			{
				blobRushTarget();
			}
			else
			{
				blobDodgeTarget();
			}
			goodBlobSetFireRate();
			self->action = goodBlobAttack;
			break;
	}
	
	self->thinkTime = Math::rrand(25, 100);
}

bool blobSightHealth()
{
	if (self->health == self->maxHealth)
	{
		return false;
	}
	
	Item *item;
	ItemDef *itemDef;
	
	Item *targetHealth = NULL;
	
	float distance;
	float currentDistance = RAND_MAX;
	
	for (Entity *ent = (Entity*)entityManager->itemList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		if (ent->health <= 0)
		{
			continue;
		}
		
		if (fabs(self->position.z - ent->position.z) > 3)
		{
			continue;
		}
		
		item = (Item*)ent;
		itemDef = (ItemDef*)item->definition;
		
		if (itemDef->itemType == IT_MEDICAL)
		{
			distance = Math::getDistance(self->position.x, self->position.y, item->position.x, item->position.y);
			
			if (distance > 150)
			{
				continue;
			}
			
			if (unitCanSeeTarget((Unit*)self, ent, self->position))
			{
				if (distance < currentDistance)
				{
					currentDistance = distance;
					targetHealth = item;
				}
			}
		}
	}
	
	float currentPercent = self->getHealthPercent();
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{	
		if (unit->definition->type == BLOB_MIA)
		{
			continue;
		}
		
		if (unit == self)
		{
			continue;
		}
		
		if ((unit->health == unit->maxHealth) || (unit->health <= 0))
		{
			continue;
		}
		
		if ((self->definition->type != BLOB_SOLDIER) && (unit->definition->type == BLOB_SOLDIER))
		{
			continue;
		}
		
		if (unit->getHealthPercent() < currentPercent)
		{
			return false;
		}
	}
	
	if (targetHealth != NULL)
	{
		float oldZ = self->velocity.z;
					
		self->velocity = targetHealth->position;
		self->velocity -= self->position;
		self->velocity.normalize();
					
		self->velocity.x *= 0.75;
		self->velocity.y *= 0.75;
		self->velocity.z = oldZ;
					
		faceLocation((Unit*)self, item->position);
					
		debug(("%s is going for health...\n", self->getName()));
		
		return true;
	}
	
	return false;
}

void goodBlobGetItem()
{
	Unit *unit = (Unit*)self;

	if (--unit->custom == 0)
	{
		unit->action = &blobFollowOrders;
	}
	
	if (unitIsAtEdge(unit))
	{
		if (unit->flags & EF_ONGROUND)
		{
			unit->velocity.z = 4.0 / 3.0;
			self->action = &blobJumping;
			self->flags &= ~EF_ONGROUND;
		}
	}
}

void blobDoOrderFollow()
{
	Unit *unit = (Unit*)self;
	
	// Blobs never attack whilst in water...
	if (unit->liquidLevel == 2)
	{
		unit->action = &blobFollowBob;
		return;
	}
	
	if (blobSightHealth())
	{
		self->action = &goodBlobGetItem;
		self->custom = 100;
	}
	else if (blobSightTarget(unit, (entityManager->bossList.getSize() == 0) ? PLAYER_TARGET_RANGE : PLAYER_TARGET_RANGE * 2))
	{
		faceTarget(unit);
		unit->action = &goodBlobPreAttack;
	}
	else
	{
		unit->action = &blobFollowBob;
	}
}

void blobDoOrderWait()
{
	self->velocity.x = 0;
	self->velocity.y = 0;
	
	if (blobSightHealth())
	{
		self->action = &goodBlobGetItem;
		self->custom = 100;
	}
	else if (blobSightTarget((Unit*)self, PLAYER_TARGET_RANGE))
	{
		faceTarget((Unit*)self);
		self->action = &goodBlobPreAttack;
	}
	else
	{
		faceLocation((Unit*)self, player->position);
	}
	
	self->thinkTime = Math::rrand(1, 2);
}

void blobDoOrderRetreat()
{
	self->action = &blobFollowBob;
}

void blobWait()
{
	self->thinkTime = 450;
	
	self->action = &blobFollowOrders;
}

void resetBlobOrders()
{
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if ((unit == player) || (unit->definition->type == BLOB_MIA))
		{
			continue;
		}
		
		if (unit->action != blobFollowOrders)
		{
			continue;
		}
		
		if (unit->switchTarget != NULL)
		{
			unit->switchTarget->owner = NULL;
			unit->switchTarget = NULL;
		}
		
		unit->thinkTime = 0;
		unit->action = &blobFollowOrders;
		unit->completedOrders = false;
	}
}

void blobEngineerUseSwitch()
{
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom > 0)
	{
		return;
	}
	
	Unit *unit = (Unit*)self;
	
	unit->action = blobFollowOrders;
	handleSwitchUse(unit->switchTarget);
	unit->switchTarget->owner = NULL;
	unit->switchTarget = NULL;
	unit->completedOrders = true;
	blobsFinishedOrders();
	unit->thinkTime = 50;
}

void blobHackerUseSwitch()
{
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom > 0)
	{
		return;
	}
	
	Unit *unit = (Unit*)self;
	
	unit->action = blobFollowOrders;
	
	Switch *swt = unit->switchTarget;
	
	if (completesSwitchPuzzle(swt))
	{
		handleSwitchUse(swt);
	}
	
	swt->owner = NULL;
	swt = NULL;
	
	unit->completedOrders = true;
	blobsFinishedOrders();
	unit->thinkTime = 50;
}

void blobPlantBomb(Unit *unit)
{
	Vector p = unit->switchTarget->position;
	p.x += Math::rrand(-15, 15);
	p.y += Math::rrand(-15, 15);
	p.z += 2;
	
	Item *item = addStickyC4(p);
	item->rotation.set(-90, 0, Math::rrand(0, 360));
	item->scale = 0.75;
	item->flags = 0;
	item->custom = rand() % 2;
	
	audio->playSound(SND_PLANT_BOMB, CH_ANY, camera->getSoundDistance(unit->position));
}

void blobDemolitionUseSwitch2()
{
	Unit *unit = (Unit*)self;
	
	unit->action = blobFollowOrders;
	handleSwitchUse(unit->switchTarget);
	unit->switchTarget->owner = NULL;
	unit->switchTarget = NULL;
	unit->completedOrders = true;
	blobsFinishedOrders();
	unit->thinkTime = 50;
}

void blobDemolitionUseSwitch1()
{
	String texture;
	
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->custom > 0)
	{
		return;
	}
	
	Unit *unit = (Unit*)self;
	unit->switchTarget->custom++;
	unit->custom = 50;
	
	texture.setText("textures/game/demolitionPlate%.2d.png", (int)unit->switchTarget->custom);
	unit->switchTarget->texture[0] = textureManager->getTexture(texture.getText());
	
	if ((((int)unit->switchTarget->custom) % 2) == 0)
	{
		blobPlantBomb(unit);
	}
	
	if (unit->switchTarget->custom == 10)
	{
		unit->action = blobDemolitionUseSwitch2;
	}
}

void blobUseAllocatedSwitch()
{
	Unit *unit = (Unit*)self;
	
	self->thinkTime = Math::rrand(1, 3);
	
	self->velocity.x = 0;
	self->velocity.y = 0;
	
	// walk the Blob to the feature
	
	float z = unit->velocity.z;
		
	unit->velocity = unit->switchTarget->position;
	unit->velocity -= unit->position;
	unit->velocity.normalize();
	
	unit->velocity = (unit->velocity * 0.85);
	
	unit->velocity.z = z;
	
	faceLocation(unit, unit->switchTarget->position);
	
	if (Math::getDistance(unit->switchTarget->position.x, unit->switchTarget->position.y, unit->position.x, unit->position.y) <= 8)
	{
		unit->velocity.x = unit->velocity.y = 0;
		
		switch (unit->switchTarget->switchType)
		{
			case SWITCH_ENGINEER:
				unit->custom = 250;
				unit->action = &blobEngineerUseSwitch;
				unit->thinkTime = 1;
				break;
				
			case SWITCH_HACKER:
				unit->custom = 250;
				unit->action = &blobHackerUseSwitch;
				unit->thinkTime = 1;
				break;
				
			case SWITCH_DEMOLITION:
				unit->custom = 50;
				unit->action = &blobDemolitionUseSwitch1;
				unit->thinkTime = 1;
				break;
				
			default:
				handleSwitchUse(unit->switchTarget);
				unit->switchTarget->owner = NULL;
				unit->switchTarget = NULL;
				unit->completedOrders = true;
				unit->thinkTime = 50;
				break;
		}
	}
}

void blobDoOrderUse()
{
	Unit *unit = (Unit*)self;
	
	if (unit->switchTarget != NULL)
	{
		blobUseAllocatedSwitch();
		return;
	}
	
	self->thinkTime = Math::rrand(1, 3);
	
	self->velocity.x = 0;
	self->velocity.y = 0;
	
	Switch *closest = NULL;
	float distance;
	float lastDistance = 9999;
	
	Switch *swt;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->entityType != ET_SWITCH)
		{
			continue;
		}
		
		swt = (Switch*)entity;
		
		if ((swt->owner != NULL) || (swt->requires != ""))
		{
			continue;
		}
		
		switch (swt->switchType)
		{
			case SWITCH_ENGINEER:
				if (self->definition->type != BLOB_ENGINEER)
				{
					continue;
				}
				break;
				
			case SWITCH_HACKER:
				if (self->definition->type != BLOB_HACKER)
				{
					continue;
				}
				break;
				
			case SWITCH_DEMOLITION:
				if (self->definition->type != BLOB_DEMOLITION)
				{
					continue;
				}
				
				// Bombs already set
				if (swt->custom >= 10)
				{
					continue;
				}
				break;
				
			default:
				if (swt->active)
				{
					continue;
				}
				break;
		}
		
		distance = Math::getDistance(entity->position, self->position);
		
		if (distance > 255)
		{
			continue;
		}
		
		if (distance < lastDistance)
		{
			closest = (Switch*)entity;
			lastDistance = distance;
		}
	}
	
	if (closest == NULL)
	{
		return;
	}
	
	unit->switchTarget = closest;
	unit->switchTarget->owner = unit;
}

void blobFollowOrders()
{
	Unit *unit = (Unit*)self;
	
	switch (game->currentOrders)
	{
		case ORDERS_FOLLOW:
			unit->completedOrders = true;
			blobDoOrderFollow();
			break;
			
		case ORDERS_RETREAT:
			unit->completedOrders = true;
			blobDoOrderRetreat();
			break;
			
		case ORDERS_WAIT:
			unit->completedOrders = true;
			blobDoOrderWait();
			break;
			
		case ORDERS_USE:
			if (!unit->completedOrders)
			{
				blobDoOrderUse();
			}
			else
			{
				blobDoOrderWait();
			}
			break;
	}
}

Unit *findTeeka()
{
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->definition->type == BLOB_TEEKA)
		{
			return unit;
		}
	}
	
	return NULL;
}

// Blob will disappear for about 10 seconds...
void blobVanish()
{
	debug(("blobVanish()\n"));
	
	self->custom = 1000;
	
	addTeleportParticles(self->position);
	
	self->flags |= EF_VANISHED;

	audio->playSound(SND_TELEPORT3, CH_SPAWN, camera->getSoundDistance(self->position));
	
	self->action = NULL;
}

bool blobReappear()
{
	Crumb *crumb = (Crumb*)crumbBox.crumbs.getTail();
	
	debug(("'%s' getting 3rd to last crumb...\n", self->name.getText()));
	
	for (int i = 0 ; i < 3 ; i++)
	{
		if (crumb != NULL)
		{
			crumb = (Crumb*)crumb->previous;
		}
	}
	
	debug(("'%s' got a crumb...\n", self->name.getText()));
	
	self->position = crumb->position;
	self->position.z += 5;
	
	if (!unitCanSpawnInLocation(NULL, self->position))
	{
		self->custom = 100;
		return false;
	}
	
	debug(("blobReappear()\n"));
	
	addTeleportParticles(self->position);
	
	self->flags &= ~EF_VANISHED;
	self->flags &= ~EF_WRAPPED;
	
	self->draw = drawUnit;

	audio->playSound(SND_TELEPORT3, CH_SPAWN, camera->getSoundDistance(self->position));
	
	self->action = blobFollowOrders;
	
	((Unit*)self)->liquidLevel = 0;
	
	self->contentType = CONTENTS_SOLID;
	
	self->velocity.set(0, 0, 0);
	
	if (self->health == 1 && self->definition->type == BLOB_TEEKA)
	{
		self->health = self->maxHealth * 0.25;
	}
	
	return true;
}

int getBlobSoldierCount()
{
	int count = 0;
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->definition->type == BLOB_SOLDIER)
		{
			count++;
		}
	}
	
	return count;
}

void addBlob(Properties *props)
{
	int type = entityManager->getEntityType(props);
	
	if ((game->changingLevel) && (type == BLOB_BOB))
	{
		player->position = props->getVector("position");
		return;
	}
	
	Unit *unit = entityManager->spawnBlob(type);
	
	unit->die = &generalBlobDie;
	unit->walk = &blobFollowOrders;
	unit->action = unit->walk;
	unit->draw = drawUnit;
	
	unit->load(props);
	
	if (unit->name == "")
	{
		unit->setName(randomNameGenerator->getRandomName());
	}
	
	switch (unit->definition->type)
	{
		case BLOB_BOB:
			unit->name = "Bob";
			player = unit;
			player->walk = NULL;
			player->action = NULL;
			debug(("Player assigned to Blob '%s', id %d\n", unit->getName(), unit->getUniqueId()));
			break;
			
		case BLOB_TEEKA:
			unit->name = "Teeka";
			unit->currentWeapon = &entityManager->weaponDef[game->teekaWeapon];
			break;
			
		case BLOB_MIA:
			unit->action = &miaBlobThink;
			unit->walk = &miaBlobThink;
			break;
			
		case BLOB_ASSIMILATING:
			unit->setName(randomNameGenerator->getRandomName());
			unit->action = &assimiatedBlobThink;
			unit->walk = &assimiatedBlobThink;
			break;
			
		case BLOB_SOLDIER:
			switch (rand() % 3)
			{
				case 0:
					unit->currentWeapon = &entityManager->weaponDef[WEP_PISTOL];
					break;
				
				case 1:
					unit->currentWeapon = &entityManager->weaponDef[WEP_UZI];
					break;
				
				case 2:
					unit->currentWeapon = &entityManager->weaponDef[WEP_PLASMA_PISTOL];
					break;
			}
			break;
			
		default:
			unit->maxHealth *= 3;
			unit->health *= 3;
			break;
	}
	
	// stop these guys from running around...
	if (mission->missionNumber == 31)
	{
		unit->walk = NULL;
		unit->action = NULL;
	}
	
	debug(("addBlob() [%s %d %.2f %.2f]\n", unit->getName(), unit->definition->type, unit->health, unit->shield));
}

void doBlobs()
{
	bool helpless;
	float fallSpeed;
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		self = unit;
		
		bbManager->removeBox(unit);
		
		if ((game->cheatNPC) && (unit != player))
		{
			unit->health = unit->maxHealth;
		}
		
		if (unit != player)
		{
			if (!(unit->flags & EF_VANISHED))
			{
				/* Only Teeka is immortal */
				
				if (unit->definition->type == BLOB_TEEKA)
				{
					Math::limit(&unit->health, 1, unit->maxHealth);
				
					if ((unit->totalCurrentDamage >= 15) || (unit->health <= 1))
					{
						unit->totalCurrentDamage = 0;
						blobVanish();
					}
				}
			}
			else
			{
				if (player->health > 0)
				{
					unit->custom -= engine->getTimeDifference(TD_LOGIC);
				}
				
				Math::limit(&unit->custom, 0, 1000);
				
				if (unit->custom == 0)
				{
					if (!blobReappear())
					{
						continue;
					}
				}
				else
				{
					continue;
				}
			}
		}
		
		if (unit->flags & EF_EXITTED)
		{
			if (!unit->referenced)
			{
				unit = (Unit*)unit->previous;
				entityManager->blobList.remove(unit->next);
			}
			
			unit->referenced = false;
			
			continue;
		}
		
		if (unit->health <= 0)
		{
			if (unit->flags & EF_DEAD)
			{
				game->alterStat(_("Allies Killed"), 1);
				
				if ((unit == player) || (unit->definition->type == BLOB_TEEKA))
				{
					game->setMissionStatus(MISSION_FAILED);
					unit->referenced = false;
					
					if (unit->definition->type == BLOB_TEEKA)
					{
						game->missionOverTimer = 0;
					}
				}
				else if (!unit->referenced)
				{
					unit = (Unit*)unit->previous;
					entityManager->blobList.remove(unit->next);
				}
				
				unit->referenced = false;
				
				continue;
			}
			else if (!(unit->flags & EF_DYING))
			{
				unit->die();
				
				if (!unit->dead)
				{
					game->setInfoMessage(INFO_BAD, _("%s has been killed"), unit->getName());
					fireTriggers(unit->getName(), TRIGGER_TYPE_ENTITY_KILLED);
					
					if (unit->definition->type == BLOB_MIA)
					{
						fireTriggers("ANY_MIA", TRIGGER_TYPE_ENTITY_KILLED);
					}
					
					unit->dead = true;
				}
				
				unit->flags |= EF_DYING;
				unit->flags |= EF_BOUNCES;
			}
		}
		
		if (unit->target != NULL)
		{
			unit->target->referenced = true;
		}
		
		if (unit->owner != NULL)
		{
			unit->owner->referenced = true;
		}
		
		// we can do this here because we've already checked it
		unit->referenced = false;
		
		helpless = (unit->helpless > 0);

		if ((!game->allStatic) && (game->cutsceneType != CUTSCENE_INGAME))
		{
			if ((!helpless) || (unit->health < 1))
			{
				if (unit->performNextAction(engine->getTimeDifference(TD_LOGIC)))
				{
					if (unit->action != NULL)
					{
						unit->action();
					}
				}
			}
		}

		if (game->cutsceneType != CUTSCENE_INGAME)
		{
			unit->think(engine->getTimeDifference(TD_LOGIC));
		}
		
		if (unit->flags & EF_TELEPORTING)
		{
			continue;
		}
		
		if ((unit->liquidLevel > 1) && (unit != player))
		{
			unit->oxygen += (engine->getTimeDifference(TD_LOGIC) * 0.75);
			
			if ((unit != player) && (!(unit->flags & EF_VANISHED)))
			{
				if (unit->contentType & (CONTENTS_SLIME|CONTENTS_LAVA))
				{
					blobVanish();
				}
			}
		}
		
		if (withinViewableRange(unit))
		{
			entityManager->addEntityToDrawList(unit);
		}

		// was helpless but now isn't - Stop sliding!
		if ((helpless) && (unit->helpless <= 0))
		{
			if ((unit->flags & EF_ONGROUND) || (!(unit->contentType & CONTENTS_SOLID)))
			{
				unit->totalCurrentDamage = 0;
				unit->rotation.set(0, 0, 0);
				unit->flags &= ~EF_BOUNCES;
				
				if (unit == player)
				{
					unit->touch = NULL;
				}
			}
			else
			{
				unit->helpless = 1;
			}
		}

		unit->applyGravity(engine->getTimeDifference(TD_LOGIC));
		
		if ((unit != player) && (unit->flags & EF_ONGROUND) && (game->cutsceneType != CUTSCENE_INGAME))
		{
			keepUnitAwayFromEdges(unit);
		}
		
		fallSpeed = unit->realVelocity.z;
		
		if (fallSpeed <= FALL_DAMAGE_ACCEL)
		{
			unit->helpless = max(5, unit->helpless);
			unit->flags |= EF_BOUNCES;
			unit->currentCrumb = NULL;
		}
		
		moveEntity(unit);
		
		//unitContentChanged(unit);
		
		// take damage from falling far but not if you hit liquid
		if ((fallSpeed <= FALL_DAMAGE_ACCEL) && (unit->velocity.z > FALL_DAMAGE_ACCEL) && (unit->contentType & CONTENTS_SOLID))
		{
			unit->velocity.z = 1.0;
			unit->realVelocity.z = 1.0;
			fallSpeed = fabs(fallSpeed * 1.5);
			
			if (game->cutsceneType == CUTSCENE_NONE)
			{
				unit->applyDamage(fallSpeed, DAMAGE_SPECIAL);
				unit->shield = -2.5;
				debug(("%s took %.2f damage from fall\n", unit->getName(), fallSpeed));
			}
			
			audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(unit->position));
			
			if (unit->flags & EF_FROZEN)
			{
				unit->mental = MAX_MENTAL;
			}
			
			if (unit == player)
			{
				game->alterStat(_("Total Damage Taken"), fallSpeed);
			}
		}
		
		if ((unit->position.z < bsp->minCords.z) && (unit->health > 0))
		{
			printf("WARNING: Blob '%s' fell out of map at %s\n", unit->getName(), unit->position.toString());
			unit->health = -100;
			unit->flags |= EF_DEAD;
			unit->referenced = false;
			#if DEV
			exit(1);
			#endif
			continue;
		}
		
		if (unit->flags & EF_ONFIRE)
		{
			addOnFireParticles();
		}
		
		bbManager->addBox(unit);
	}
}
