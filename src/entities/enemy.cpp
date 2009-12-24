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

#include "enemy.h"

/*
* Used to make the enemies "pause" for a moment before attacking the player again
*/
void resetEnemyThinkTimes()
{
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		unit->thinkTime = 50;
	}
}

void addEnemy(Properties *props)
{
	if (game->nomonsters)
	{
		return;
	}
	
	int type = entityManager->getEntityType(props);
	
	Unit *unit = entityManager->spawnEnemy(type, mission->enemyLevel);

	unit->name = randomNameGenerator->getRandomName();
	unit->draw = drawUnit;
	
	switch (unit->definition->type)
	{
		case NME_PISTOL_BLOB:
			pistolBlobInit(unit);
			break;

		case NME_GRENADE_BLOB:
			grenadeBlobInit(unit);
			break;

		case NME_MACHINEGUN_BLOB:
			machineGunBlobInit(unit);
			break;

		case NME_SHOTGUN_BLOB:
			shotGunBlobInit(unit);
			break;
			
		case NME_LIGHT_PLASMA_BLOB:
			plasmaPistolBlobInit(unit);
			break;
			
		case NME_AQUA_BLOB:
			pistolBlobInit(unit);
			break;

		case NME_PISTOL_DROID:
			pistolDroidInit(unit);
			break;
			
		case NME_GRENADE_DROID:
			grenadeDroidInit(unit);
			break;
			
		case NME_SHOTGUN_DROID:
			shotGunDroidInit(unit);
			break;
			
		case NME_MACHINEGUN_DROID:
			machineGunDroidInit(unit);
			break;
			
		case NME_UZI_SENTRY_GUN:
			uziSentryGunInit(unit);
			break;
			
		case NME_PLASMA_SENTRY_GUN:
			plasmaSentryGunInit(unit);
			break;
			
		case NME_ROCKET_SENTRY_GUN:
			rocketSentryGunInit(unit);
			break;
			
		case NME_MORTOR_SENTRY_GUN:
			mortorSentryGunInit(unit);
			break;
			
		case NME_DEAD_SENTRYGUN:
			deadSentryGunInit(unit);
			break;
			
		case NME_DARK_BLOB:
			
			if (props->hasProperty("classname"))
			{
				if ((rand() % 5) == 0)
				{
					unit->shield = unit->maxShield = Math::rrand(10, 15);
				}
			}
			
			darkBlobInit(unit);
			break;
			
		case NME_MACHINEGUN_SPIDERBLOB:
			machineGunSpiderBlobInit(unit);
			break;
			
		case NME_SHOTGUN_SPIDERBLOB:
			shotGunSpiderBlobInit(unit);
			break;
			
		case NME_PLASMA_SPIDERBLOB:
			plasmaSpiderBlobInit(unit);
			break;
			
		case NME_BABY_SPIDERBLOB:
			babySpiderBlobInit(unit);
			break;
			
		case NME_TANKBAY_TANK_1:
		case NME_TANKBAY_TANK_2:
			bioTankInit(unit);
			break;

		default:
			printf("ERROR - addEnemy() - Unit type %d unknown. Check init statements (enemy.cpp).\n", unit->definition->type);
			props->listProperties();
			exit(1);
			break;
	}
	
	unit->load(props);
	
	debug(("addEnemy [%s, %d - %.2f %.2f]\n", unit->getName(), unit->definition->type, unit->health, unit->shield));
}

bool isOkayToSpawnEnemies()
{
	if (player->contentType & CONTENTS_MONSTERCLIP)
	{
		return false;
	}
	
	int closeCount = 0;
	
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->spawnedIn)
		{
			continue;
		}
		
		if (Math::getDistance(player->position, unit->position) < 500)
		{
			closeCount++;
		}
	}
	
	return (closeCount <= 3);
}

void spawnRandomEnemy()
{
	debug(("spawnRandomEnemy()\n"));
	
	Properties props;
	Vector position;
	
	if (game->nomonsters)
	{
		return;
	}
	
	position = player->position;
	
	props.clear();
	
	position.x += Math::rrand(-80, 80);
	position.y += Math::rrand(-80, 80);
	position.z += Math::rrand(16, 64);
	
	props.setProperty("classname", extractEnemyFromList(mission->enemySpawnList.getText()));
	
	int type = entityManager->getEntityType(&props);
	
	if ((entityManager->enemyDef[type].flags & EF_WEIGHTLESS))
	{
		if (bsp->getContentsAtPosition(position) & (CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WATER|CONTENTS_MONSTERCLIP|CONTENTS_SOLID))
		{
			debug(("spawnRandomEnemy() - Can't spawn in content type\n"));
			return;
		}
	}
	else if (!unitCanSpawnInLocation(NULL, position))
	{
		debug(("spawnRandomEnemy() - Can't spawn at position\n"));
		return;
	}
	
	props.setProperty("position", position);
	props.setProperty("spawnedIn", 1);
	props.setProperty("thinkTime", Math::rrand(50, 100)); // don't attack instantly!
	
	addEnemy(&props);

	addTeleportParticles(position);
	
	audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(position));
	
	debug(("spawnRandomEnemy() - Done\n"));
}

bool allEnemyUnitsDead()
{
	Unit *unit = (Unit*)entityManager->enemyList.getFirstElement();

	while (unit != NULL)
	{
		if (unit->health > 0)
		{
			return false;
		}

		unit = (Unit*)unit->next;
	}

	return true;
}

void enemyMoveToTarget(Unit *unit)
{
	float z = unit->velocity.z;
	
	unit->velocity = unit->target->position;
	
	unit->velocity.x += Math::rrand(-10, 10);
	unit->velocity.y += Math::rrand(-10, 10);
	unit->velocity.z += Math::rrand(5, 10);
	
	unit->velocity -= unit->position;
	unit->velocity.normalize();

	float speed = Math::rrand(150, 400);
	
	if (unit->definition->type != NME_DARK_BLOB)
	{
		speed /= 600;
	}
	else
	{
		speed /= 400;
	}

	unit->velocity.x *= speed;
	unit->velocity.y *= speed;
	
	if ((!(unit->flags & EF_WEIGHTLESS)) && (!(unit->flags & EF_SWIMS)))
	{
		unit->velocity.z = z;
	}
}

bool enemySightTarget(Unit *unit, int range)
{
	Vector sight;

	unit->target = NULL;
	
	if ((game->cheatBlind) || (unit->flags & EF_BLIND))
	{
		return false;
	}
	
	float distance = 0;
	float targetDistance = range;

	for (Unit *blob = (Unit*)entityManager->blobList.getFirstElement() ; blob != NULL ; blob = (Unit*)blob->next)
	{
		if (!blob->canBeDamaged())
		{
			continue;
		}
		
		// MIAs are not targets unless they are escaping... (if there custom < 0)
		if (blob->definition->type == BLOB_MIA)
		{
			if (blob->custom == 0)
			{
				continue;
			}
		}

		distance = Math::getDistance(unit->position, blob->position);

		if (distance < targetDistance)
		{
			// if the distance is not that great then maybe stick to your old target...
			if (fabs(distance - targetDistance) <= 100)
			{
				if ((rand() % 3) == 0)
				{
					continue;
				}
			}
			
			sight = unit->position;
			sight.z += 6;
			
			if (!unitCanSeeTarget(unit, blob, sight))
			{
				continue;
			}
			
			targetDistance = distance;
			unit->target = blob;
			
			unit->lastPlayerSighting = 0;
		}
	}

	return (unit->target != NULL);
}

void doEnemies()
{
	bool helpless;
	float fallSpeed;
	
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		self = unit;
		
		//self->riding = NULL;
		
		bbManager->removeBox(unit);
		
		// final battle...
		if (unit->contentType & CONTENTS_PLAYERCLIP)
		{
			unit->health = 0;
			unit->flags |= EF_DEAD;
		}

		if (unit->health <= 0)
		{
			if (unit->flags & EF_DEAD)
			{
				if (!unit->referenced)
				{
					if (player->target == unit)
					{
						player->target = NULL;
					}
				
					unit = (Unit*)unit->previous;
					entityManager->enemyList.remove(unit->next);
					
					if (entityManager->enemyList.getSize() == 0)
					{
						fireTriggers("ANY_ENEMY", TRIGGER_TYPE_ALL_ENEMIES_KILLED);
					}
					
					continue;
				}
				else
				{
					unit->referenced = false;
				}
				
				continue;
			}
			else if (!(unit->flags & EF_DYING))
			{
				unit->die();
				
				if (!unit->dead)
				{
					fireTriggers(unit->getName(), TRIGGER_TYPE_ENTITY_KILLED);
					fireTriggers("ANY_ENEMY", TRIGGER_TYPE_ENTITY_KILLED);
					unit->dead = true;
				}
				
				unit->flags |= EF_DYING;
				
				if (!(unit->flags & EF_STATIC))
				{
					unit->flags |= EF_BOUNCES;
				}
				
				addBloodParticle(unit->position.x, unit->position.y, unit->position.z);
				
				if (unit->contentType & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
				{
					debug(("%s drowned...\n", unit->getName()));
				}
			}
		}
		
		if ((unit->spawnedIn) && (unit->lastPlayerSighting >= 6000) && (!unit->referenced))
		{
			addTeleportParticles(unit->position);
			unit = (Unit*)unit->previous;
			entityManager->enemyList.remove(unit->next);
			continue;
		}
		
		if (unit->target != NULL)
		{
			unit->target->referenced = true;
		}
		
		if (unit->owner != NULL)
		{
			unit->owner->referenced = true;
		}
		
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
		
		if (unit->flags & (EF_TELEPORTING|EF_VANISHED))
		{
			continue;
		}
		
		if (withinViewableRange(unit))
		{
			entityManager->addEntityToDrawList(unit);
			
			if (unit->flags & EF_WEIGHTLESS)
			{
				addGravUnitParticles(unit, GLColor::purple, GLColor::blue);
			}
		}
		
		if ((unit->liquidLevel > 1) && (!(unit->flags & EF_SWIMS)))
		{
			if ((unit->definition->type == NME_DARK_BLOB) && (unit->health > 0))
			{
				generalUnitVanish(player->position, Math::rrand(300, 400));
				continue;
			}
			else if (unit->health > 0)
			{
				unit->applyDamage(RAND_MAX, DAMAGE_SPECIAL);
			}
		}

		// was helpless but now isn't - Stop sliding!
		if ((helpless) && (unit->helpless <= 0))
		{
			if ((unit->flags & EF_ONGROUND) || (!(unit->contentType & CONTENTS_SOLID)) || (unit->flags & EF_WEIGHTLESS))
			{
				unit->totalCurrentDamage = 0;
				unit->rotation.set(0, 0, 0);
				unit->flags &= ~EF_BOUNCES;
			}
			else
			{
				unit->helpless = 1;
			}
		}

		unit->applyGravity(engine->getTimeDifference(TD_LOGIC));
		
		keepUnitAwayFromEdges(unit);

		fallSpeed = unit->realVelocity.z;
		
		if (fallSpeed <= FALL_DAMAGE_ACCEL)
		{
			unit->helpless = 5;
			unit->flags |= EF_BOUNCES;
		}
		
		// don't let swimming blobs move too quickly
		if ((unit->flags & EF_SWIMS) && (unit->liquidLevel == 2))
		{
			Math::limit(&unit->velocity.x, -0.1, 0.1);
			Math::limit(&unit->velocity.y, -0.1, 0.1);
			Math::limit(&unit->velocity.z, -0.1, 0.1);
		}
		
		moveEntity(unit);
		
		if ((fallSpeed <= FALL_DAMAGE_ACCEL) && (unit->velocity.z > FALL_DAMAGE_ACCEL) && (unit->contentType & CONTENTS_SOLID))
		{
			unit->velocity.z = 1.0;
			fallSpeed = fabs(fallSpeed * 1.5);
			
			if (!(unit->flags & EF_DYING))
			{
				unit->applyDamage(fallSpeed, DAMAGE_SPECIAL);
				unit->shield = -2.5;
				audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(unit->position));
				debug(("%s took %.2f damage from fall [%s]\n", unit->getName(), fallSpeed, unit->position.toString()));
			}
			else
			{
				unit->health = -100;
				unit->flags |= EF_DYING;
			}
		}
		
		if ((unit->position.z < bsp->minCords.z) && (unit->health > 0))
		{
			printf("WARNING: Enemy '%s' fell out of map at %s\n", unit->getName(), unit->position.toString());
			printf("Map Mins: %s\n", bsp->minCords.toString());
			printf("Map Maxs: %s\n", bsp->maxCords.toString());
			printf("On Ground = %ld\n", (unit->flags & EF_ONGROUND));
			printf("Velocity = %s\n", unit->velocity.toString());
			unit->health = -100;
			unit->flags |= EF_DEAD;
			continue;
			#if DEV
			exit(1);
			#endif
		}
		
		if (unit->flags & EF_ONFIRE)
		{
			addOnFireParticles();
		}
		
		bbManager->addBox(unit);
		
		if ((unit->definition->type == NME_DARK_BLOB) && (unit->health > 0))
		{
			Math::limit(&unit->helpless, 0, 25);
		}
		
		if ((unit->flags & EF_ALWAYS_FACE) && (unit->health > 0))
		{
			if ((!unit->helpless) && (unit->target != NULL))
			{
				faceTarget(unit);
			}
		}
	}
}
