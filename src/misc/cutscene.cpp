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

#include "cutscene.h"

CutsceneCamera *addCutsceneCamera(Properties *props)
{
	const char *name = props->getString("name", "unknown");
	
	CutsceneCamera *camera = entityManager->spawnCutsceneCamera(name);
	
	camera->load(props);
	
	return camera;
}

void activateInGameCutscene(const char *name, float delay)
{
	debug(("activateInGameCutscene - %s, %.2f\n", name, delay));
	
	for (CutsceneCamera *cutsceneCamera = (CutsceneCamera*)entityManager->cameraList.getFirstElement() ; cutsceneCamera != NULL ; cutsceneCamera = (CutsceneCamera*)cutsceneCamera->next)
	{
		if (cutsceneCamera->name == name)
		{
			if (cutsceneCamera->custom != 0)
			{
				continue;
			}
			
			camera->targetCamera = cutsceneCamera;
			camera->targetCamera->custom = delay;
			return;
		}
	}
}

CutsceneCamera *getCutsceneCamera(const char *name)
{
	for (CutsceneCamera *cutsceneCamera = (CutsceneCamera*)entityManager->cameraList.getFirstElement() ; cutsceneCamera != NULL ; cutsceneCamera = (CutsceneCamera*)cutsceneCamera->next)
	{
		if (cutsceneCamera->name == name)
		{
			if (cutsceneCamera->custom != 0)
			{
				continue;
			}
			
			return cutsceneCamera;
		}
	}
	
	return NULL;
}

void removeBulletsForCutscene()
{
	for (Bullet *bullet = (Bullet*)entityManager->bulletList.getFirstElement() ; bullet != NULL ; bullet = (Bullet*)bullet->next)
	{
		bbManager->removeBox(bullet);
	}

	entityManager->bulletList.clear();
}

void haltObjectsForCutscene()
{
	removeBulletsForCutscene();
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		unit->velocity.x = unit->velocity.y = 0;
	}
	
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		unit->velocity.x = unit->velocity.y = 0;
	}
}

void doInGameCutscene()
{
	engine->clearInput();
	engine->flushInput();
	
	bool doneTriggers = false;
	
	CutsceneCamera *targetCamera = camera->targetCamera;
	
	targetCamera->velocity = targetCamera->destination;
	targetCamera->velocity -= targetCamera->position;
	targetCamera->velocity.normalize();
	targetCamera->velocity = targetCamera->velocity * targetCamera->speed;
	
	while (targetCamera->custom > -targetCamera->timeout)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		doGameObjects();
		drawGameObjects();
		haltObjectsForCutscene();
		
		targetCamera->custom -= engine->getTimeDifference(TD_LOGIC);
		
		targetCamera->position += (targetCamera->velocity * engine->getTimeDifference(TD_LOGIC));
		
		if (targetCamera->custom <= 0)
		{
			game->cutsceneType = CUTSCENE_INGAME;
			
			if ((targetCamera->custom <= -25) && (!doneTriggers))
			{
				if (targetCamera->deferredSwitch != NULL)
				{
					handleStructureStateChange(targetCamera->deferredSwitch->getName(), targetCamera->deferredSwitch->active);
					toggleLaserTrap(targetCamera->deferredSwitch->getName());
					fireTriggers(targetCamera->deferredSwitch->getName(), TRIGGER_TYPE_ANY);
				}
				
				fireTriggers(targetCamera->getName(), TRIGGER_TYPE_ANY);
				
				if (mission->allObjectiveComplete())
				{
					fireTriggers("MISSION_COMPLETE", TRIGGER_TYPE_MISSION_COMPLETE);
				}
				
				doneTriggers = true;
			}
		}
	}
	
	game->cutsceneType = CUTSCENE_NONE;
	
	camera->targetCamera = NULL;
	
	engine->clearInput();
	engine->flushInput();
	
	resetEnemyThinkTimes();
}

void loadCutscene(const char *filename, List *data)
{
	if (!pak->unpack(filename, PAK::TEXT))
	{
		graphics->showErrorAndExit(ERR_FILE, filename);
	}
	
	data->clear();
	
	int i = -1;

	char *token = strtok((char*)pak->dataBuffer, "\n");
	
	CutsceneCommand *command;
	
	bool read = true;

	while (true)
	{
		i++;
		
		//printf("%d) %s\n", i, token);
		
		if (strcmp(token, "@END@") == 0)
		{
			break;
		}
		
		// ignore blank lines
		if (strcmp(token, "") == 0)
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (strcmp(token, "/*") == 0)
		{
			read = false;
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (strcmp(token, "*/") == 0)
		{
			read = true;
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (!read)
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		// looks like a comment.
		if (token[0] == '/')
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		// ignore non command lines
		if (!strstr(token, ";"))
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		command = new CutsceneCommand();
		command->lineNumber = i;
		command->command = token;
		
		data->add(command);
		
		token = strtok(NULL, "\n");
	}
	
	pak->freeAll();
	
	debug(("Loaded Cutscene '%s'. %d lines, %d commands\n", filename, i, data->getSize()));
}

void entityDoNothing()
{
}

void cameraPositionAt(char *args)
{
	char name[128];
	
	sscanf(args, "%*c%[^\"]", name);
	
	camera->targetCamera->position = entityManager->getEntityPosition(name);
}

void cameraStop(char *args)
{
	camera->targetCamera->velocity.set(0, 0, 0);
}

void cameraLookAt(char *args)
{
	char name[128];
	
	sscanf(args, "%*c%[^\"]", name);
	
	camera->targetCamera->destination = entityManager->getEntityPosition(name);
	camera->targetCamera->follow = NULL;
	camera->targetCamera->chasing = false;
}

void cameraFollow(char *args)
{
	char name[128];
	
	sscanf(args, "%*c%[^\"]", name);
	
	if (strcmp(name, "@NONE@") == 0)
	{
		camera->targetCamera->follow = NULL;
	}
	else
	{
		camera->targetCamera->follow = entityManager->getAnyEntityByName(name);
	}
}

void cameraMoveTo(char *args)
{
	char name[128];
	float speed;
	
	sscanf(args, "%*c%[^\"]%*c %f", name, &speed);
	
	camera->targetCamera->target = entityManager->getEntityPosition(name);
	camera->targetCamera->speed = speed;
	
	camera->targetCamera->velocity = camera->targetCamera->target;
	camera->targetCamera->velocity -= camera->targetCamera->position;
	camera->targetCamera->velocity.normalize();
	
	camera->targetCamera->velocity = camera->targetCamera->velocity * speed;
	
	camera->targetCamera->chasing = false;
}

void cameraChase(char *args)
{
	float x, y, z;
	
	sscanf(args, "%f %f %f", &x, &y, &z);
	
	camera->targetCamera->target.set(x, y, z);
	camera->targetCamera->speed = camera->targetCamera->chaseSpeed;
	
	if (!camera->targetCamera->chasing)
	{
		camera->targetCamera->follow = &camera->targetCamera->chase;
		
		camera->targetCamera->velocity.set(x, y, z);
		camera->targetCamera->velocity -= camera->targetCamera->position;
		camera->targetCamera->velocity.normalize();
		camera->targetCamera->velocity = camera->targetCamera->velocity * camera->targetCamera->chaseSpeed;
		
		camera->targetCamera->chase.position = entityManager->getEntityPosition("Bob");
		camera->targetCamera->chase.target.set(x, y, z);
		camera->targetCamera->chase.velocity = camera->targetCamera->chase.target;
		camera->targetCamera->chase.velocity -= camera->targetCamera->chase.position;
		camera->targetCamera->chase.velocity.normalize();
		camera->targetCamera->chasing = true;
	}
	else
	{
		camera->targetCamera->velocity.set(x, y, z);
		camera->targetCamera->velocity -= camera->targetCamera->position;
		camera->targetCamera->velocity.normalize();
		camera->targetCamera->velocity = camera->targetCamera->velocity * camera->targetCamera->chaseSpeed;
	
		camera->targetCamera->chase.target.set(x, y, z);
		camera->targetCamera->chase.velocity = camera->targetCamera->chase.target;
		camera->targetCamera->chase.velocity -= camera->targetCamera->chase.position;
		camera->targetCamera->chase.velocity.normalize();
	}
	
	camera->targetCamera->chase.velocity = camera->targetCamera->chase.velocity * camera->targetCamera->chaseSpeed;
}

void cameraChaseEnd(char *args)
{
	camera->targetCamera->velocity = camera->targetCamera->velocity * 0.5;
	camera->targetCamera->chasing = false;
	camera->targetCamera->follow = NULL;
}

void cameraChaseSpeed(char *args)
{
	sscanf(args, "%f", &camera->targetCamera->chaseSpeed);
}

void cameraShake(char *args)
{
	int shake = 0;
	
	sscanf(args, "%d", &shake);

	camera->shake = shake;
}

void entityPositionAt(char *args)
{
	char entityName[128], target[128];
	
	sscanf(args, "%*c%[^\"]%*c%*c%*c%[^\"]", entityName, target);
	
	Entity *entity = entityManager->getAnyEntityByName(entityName);
	Vector v = entityManager->getEntityPosition(target);
	
	entity->position = v;
	entity->velocity.set(0, 0, 0);
	entity->riding = NULL;
}

void entityMoveTo(char *args)
{
	char entityName[128], target[128];
	float speed;
	
	sscanf(args, "%*c%[^\"]%*c%*c%*c%[^\"]%*c%f", entityName, target, &speed);
	
	Entity *entity = (Entity*)entityManager->getAnyEntityByName(entityName);
	Vector v = entityManager->getEntityPosition(target);
	
	if (entity->entityType == ET_BOSS)
	{
		faceLocation((Boss*)entity, v);
	}
	else
	{
		faceLocation((Unit*)entity, v);
	}
	
	entity->velocity = v;
	entity->velocity -= entity->position;
	entity->velocity.normalize();
	
	entity->velocity = entity->velocity * speed;
}

void entityTurn(char *args)
{
}

void skipCutscene(char *args)
{
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		unit->velocity.set(0, 0, 0);
		unit->realVelocity.set(0, 0, 0);
		unit->helpless = 0;
	}
}

void entityLookAt(char *args)
{
	char entityName[128], target[128];
	
	sscanf(args, "%*c%[^\"]%*c%*c%*c%[^\"]", entityName, target);
	
	Entity *ent = (Entity*)entityManager->getAnyEntityByName(entityName);
	Vector v = entityManager->getEntityPosition(target);
	
	if (ent->entityType == ET_BOSS)
	{
		faceLocation((Boss*)ent, v);
	}
	else
	{
		faceLocation((Unit*)ent, v);
	}
	
	ent->action = entityDoNothing;
	ent->thinkTime = 0;
}

void unitLookRandom()
{
	self->rotation.x = Math::rrand(0, 360);
	self->thinkTime = Math::rrand(50, 125);
}

void entityLookRandom(char *args)
{
	char entityName[128];
	
	sscanf(args, "%*c%[^\"]", entityName);
	
	Entity *entity = (Entity*)entityManager->getAnyEntityByName(entityName);
	entity->action = &unitLookRandom;
	entity->thinkTime = 0;
}

void entityWatch()
{
	if (self->entityType == ET_BOSS)
	{
		Boss *boss = (Boss*)self;
		faceLocation(boss, boss->target->position);
	}
	else
	{
		Unit *unit = (Unit*)self;
		faceLocation(unit, unit->target->position);
	}
}

void entityWatch(char *args)
{
	char entityName[128], target[128];
	
	sscanf(args, "%*c%[^\"]%*c%*c%*c%[^\"]", entityName, target);
	
	Entity *ent = (Unit*)entityManager->getAnyEntityByName(entityName);
	
	if (strcmp(target, "@NONE@") == 0)
	{
		ent->action = &entityDoNothing;
		return;
	}
	
	ent->action = &entityWatch;
	ent->thinkTime = 0;
	
	if (ent->entityType == ET_BOSS)
	{
		((Boss*)ent)->target = entityManager->getAnyEntityByName(target);
	}
	else
	{
		((Unit*)ent)->target = entityManager->getAnyEntityByName(target);
	}
}

void entityJump(char *args)
{
	char entityName[128];
	float power;
	
	sscanf(args, "%*c%[^\"]%*c%f", entityName, &power);
	
	Unit *unit = (Unit*)entityManager->getOwnerByName(entityName);
	unit->action = NULL;
	unit->velocity.z = power;
	unit->thinkTime = 0;
}

void entityRemove(char *args)
{
	char entityName[128];
	
	sscanf(args, "%*c%[^\"]", entityName);
	
	Entity *ent = (Entity*)entityManager->getAnyEntityByName(entityName);
	
	if (ent != NULL)
	{
		ent->health = 0;
		ent->flags |= EF_DEAD;
	}
}

void entityVanish(char *args)
{
	char entityName[128], target[128];
	
	sscanf(args, "%*c%[^\"]%*c%*c%*c%[^\"]", entityName, target);
	
	Entity *entity = (Entity*)entityManager->getAnyEntityByName(entityName);
	Vector v = entityManager->getEntityPosition(target);
	
	audio->playSound(SND_TELEPORT3, CH_ANY, camera->getSoundDistance(entity->position));
	addTeleportParticles(entity->position);
	
	entity->position = v;
	entity->velocity.set(0, 0, 0);
	
	audio->playSound(SND_TELEPORT3, CH_ANY, camera->getSoundDistance(entity->position));
	addTeleportParticles(entity->position);
}

void entityStop(char *args)
{
	char unitName[128];
	
	sscanf(args, "%*c%[^\"]", unitName);
	
	Entity *entity = (Entity*)entityManager->getAnyEntityByName(unitName);
	
	entity->velocity.set(0, 0, 0);
}

void entityHelpless(char *args)
{
	char unitName[128];
	float helpless;
	
	sscanf(args, "%*c%[^\"]%*c%f", unitName, &helpless);
	
	Unit *unit = (Unit*)entityManager->getOwnerByName(unitName);
	
	unit->helpless = helpless;
	
	if (helpless == 0)
	{
		unit->rotation.set(0, 0, 0);
		unit->velocity.set(0, 0, 0);
		unit->flags &= ~EF_BOUNCES;
	}
}

void entityWeightless(char *args)
{
	char unitName[128];
	
	sscanf(args, "%*c%[^\"]", unitName);
	
	Entity *entity = (Entity*)entityManager->getAnyEntityByName(unitName);
	
	entity->flags |= EF_WEIGHTLESS;
	entity->health = entity->maxHealth;
}

void structureStateChange(char *args)
{
	char name[128];
	int state;
	
	sscanf(args, "%*c%[^\"]%*c%d", name, &state);
	
	handleStructureStateChange(name, state);
}

void barrierStateChange(char *args)
{
	char name[128];
	int state;
	
	sscanf(args, "%*c%[^\"]%*c%d", name, &state);
	
	changeShieldState(name, state);
}

void blobsMoveTo(char *args)
{
	char target[128];
	float speed;
	float rSpeed;

	sscanf(args, "%*c%[^\"]%*c%f", target, &speed);
	
	Vector v = entityManager->getEntityPosition(target);

	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->name == "Bob")
		{
			continue;
		}
	
		faceLocation(unit, v);
		
		unit->velocity = v;
		unit->velocity -= unit->position;
		unit->velocity.normalize();
		
		rSpeed = Math::rrand(80, 100);
		rSpeed *= 0.01;
		
		unit->velocity = unit->velocity * (speed * rSpeed);
	}
}

void bioMechsMoveTo(char *args)
{
	char target[128];
	float speed;
	float rSpeed;

	sscanf(args, "%*c%[^\"]%*c%f", target, &speed);
	
	Vector v = entityManager->getEntityPosition(target);

	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		faceLocation(unit, v);
		
		unit->velocity = v;
		unit->velocity -= unit->position;
		unit->velocity.normalize();
		
		rSpeed = Math::rrand(95, 100);
		rSpeed *= 0.01;
		
		unit->velocity = unit->velocity * (speed * rSpeed);
	}
}

void blobsWatch(char *args)
{
	char target[128];
	Entity *entity = 0;

	sscanf(args, "%*c%[^\"]", target);
	
	bool watch = (strcmp(target, "@NONE@") != 0);
	
	if (watch)
	{
		entity = entityManager->getAnyEntityByName(target);
	}

	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (entity)
		{
			unit->target = entity;
			unit->action = &entityWatch;
		}
		
		unit->thinkTime = 0;
	}
}

void blobsJump(char *args)
{
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->name == "Bob")
		{
			continue;
		}
	
		unit->action = NULL;
		unit->velocity.z = 0.5;
		unit->thinkTime = 0;
	}
}

void fireBeam(char *args)
{
	char beamStart[128], beamEnd[128];
	
	sscanf(args, "%*c%[^\"]%*c%*c%*c%[^\"]", beamStart, beamEnd);
	
	Vector v, p;
	Vector start = entityManager->getEntityPosition(beamStart);
	Vector end = entityManager->getEntityPosition(beamEnd);

	v = end;
	v -= start;
	v.normalize();
	
	p = start;
	Particle *particle;
	
	int distance = (int)(Math::getDistance(start, end));
	
	for (int i = 0 ; i < distance ; i += 2)
	{
		p += (v * 2);
		
		particle = particleManager->spawnParticle();
		particle->position = p;
		particle->life = Math::rrand(50, 125);
		particle->size = Math::rrand(2, 3);
		particle->color = ((i % 2) == 0) ? GLColor::purple : GLColor::white;
	}
	
	addPuffParticles(end, GLColor::purple, 25, 2, 4);
	addPuffParticles(end, GLColor::white, 25, 2, 4);
	
	audio->playSound(SND_LASER, CH_ANY, camera->getSoundDistance(start));
}

void bigExplosion(char *args)
{
	char target[1024];

	sscanf(args, "%*c%[^\"]%*c", target);

	Vector position = entityManager->getEntityPosition(target);

	Vector v;

	for (int i = 0 ; i < 25 ; i++)
	{
		throwExplosionDebris(position);
	}
		
	for (int i = 0 ; i < 10 ; i++)
	{
		v = position;
		v.x += Math::rrand(-50, 50);
		v.y += Math::rrand(-50, 50);
		v.z += Math::rrand(-50, 50);
		doExplosion(v, 10, self);
	}
		
	audio->playSound(SND_LARGE_EXPLOSION, CH_ANY, camera->getSoundDistance(position));
}

void whiteOut(char *args)
{
	int whiteOutTime;
	sscanf(args, "%d", &whiteOutTime);

	game->whiteOutTime = whiteOutTime;
}

void spawnEnemies(char *args)
{
	char location[128];
	int number, radius;

	sscanf(args, "%*c%[^\"]%*c%d%d", location, &number, &radius);
	
	Properties props;
	Vector position = entityManager->getEntityPosition(location);
	Vector v;
	
	for (int i = 0 ; i < number ; i++)
	{
		v = position;
		v.x += Math::rrand(-radius, radius);
		v.y += Math::rrand(-radius, radius);
		
		props.setProperty("position", v);
		props.setProperty("classname", extractEnemyFromList("NME_PISTOL_BLOB|NME_MACHINEGUN_BLOB|NME_SHOTGUN_BLOB|NME_LIGHT_PLASMA_BLOB"));
		
		addEnemy(&props);
	}
}

void transformRayThink()
{
	Bullet *bullet = (Bullet*)self;

	Vector targetSpeed = bullet->target->position;
	targetSpeed -= bullet->position;
	targetSpeed.normalize();
	
	bullet->velocity.x += (targetSpeed.x * engine->getTimeDifference(TD_LOGIC));
	bullet->velocity.y += (targetSpeed.y * engine->getTimeDifference(TD_LOGIC));
	bullet->velocity.z += (targetSpeed.z * engine->getTimeDifference(TD_LOGIC));
	
	Math::limit(&bullet->velocity.x, -bullet->custom, bullet->custom);
	Math::limit(&bullet->velocity.y, -bullet->custom, bullet->custom);
	Math::limit(&bullet->velocity.z, -bullet->custom, bullet->custom);
	
	bullet->target->velocity = bullet->target->velocity * 0.97;
}

void transformRayTouch(Entity *other)
{
	Bullet *bullet = (Bullet*)self;

	if (other == bullet->target)
	{
		self->health = 0;
		
		Unit *unit = (Unit*)bullet->target;
		
		// create a new MIA blob based on the enemy's data
		Properties props;
		
		props.setProperty("name", unit->getName());
		props.setProperty("position", unit->position.toString());
		props.setProperty("classname", "BLOB_MIA");
		props.setProperty("health", unit->health);
		props.setProperty("maxHealth", unit->maxHealth);
		props.setProperty("shield", unit->shield);
		props.setProperty("maxShield", unit->maxShield);
		props.setProperty("helpless", 100);
		props.setProperty("velocity", "0 0 0.25");
		props.setProperty("flags", EF_BOUNCES);
		
		addBlob(&props);
		
		// now remove the old enemy...
		unit->health = 0;
		unit->flags |= EF_DEAD;
		
		addDesimilatedParticles(unit->position);
		
		audio->playSound(SND_DESIMILATED, CH_ANY, camera->getSoundDistance(self->position));
		
		self->touch = NULL;
	}
}

void galdovTransformBeams(char *args)
{
	Boss *boss = (Boss*)entityManager->bossList.getFirstElement();
	
	boss->energyCharge = 15;
	
	Bullet *bullet;
	
	for (int i = 0 ; i < 35 ; i++)
	{
		bullet = entityManager->spawnBullet(WEP_HOMING_ENERGY);
		bullet->position = boss->position;
		bullet->owner = boss;
		bullet->health = RAND_MAX;
		bullet->action = transformRayThink;
		bullet->touch = NULL;
		bullet->thinkTime = RAND_MAX;
		bullet->velocity.x = Math::rrand(-200, 200);
		bullet->velocity.y = Math::rrand(-200, 200);
		bullet->velocity.z = rand() % 200;
		bullet->draw = drawBullet;
		
		bullet->velocity = bullet->velocity * 0.0025;
	}
}

void transformEnemies(char *args)
{
	removeBulletsForCutscene();

	Bullet *bullet;

	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		bullet = entityManager->spawnBullet(WEP_HOMING_ENERGY);
		bullet->position = unit->position;
		bullet->position.x += Math::rrand(-256, 256);
		bullet->position.y += Math::rrand(-256, 256);
		bullet->position.z = Math::rrand(0, 128);
		bullet->health = RAND_MAX;
		bullet->action = transformRayThink;
		bullet->touch = transformRayTouch;
		bullet->thinkTime = 0;
		bullet->velocity.set(0, 0, 0);
		bullet->custom = Math::rrand(10, 25);
		bullet->custom /= 100;
		bullet->draw = drawBullet;
		bullet->target = unit;
	}
}

void removeAllMIAs(char *args)
{
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->definition->type == BLOB_MIA)
		{
			unit->flags |= EF_EXITTED;
		}
	}
}

void removeAllBlobs(char *args)
{
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->definition->type != BLOB_BOB)
		{
			unit->flags |= EF_EXITTED;
		}
	}
}

void buildCutsceneNarrative(char *filename, Texture **text, int width, int height)
{
	// we need to do this for locale purposes...
	char fullFilePath[4096];
	sprintf(fullFilePath, _("data/cutscenes/text/en/%s"), filename);

	char data[1024];
	strcpy(data, "");
	
	if (!pak->unpack(fullFilePath, PAK::TEXT))
	{
		graphics->showErrorAndExit(ERR_FILE, fullFilePath);
	}

	SDL_Surface *s;
	int i = -2;
	
	char *token = strtok((char*)pak->dataBuffer, "\n");
	char *previousToken = 0;
	
	bool done = false;
	
	while (!done)
	{
		if (strcmp(token, "[END]") == 0)
		{
			done = true;
		}
		
		if (strcmp(token, "") == 0)
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (token[0] == '/')
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (token[0] == '[')
		{
			i++;
			
			if (i >= 0)
			{
				s = graphics->createSurface(width, height);
				SDL_FillRect(s, NULL, graphics->black);
				graphics->setTransparent(s);
				graphics->setFont(FONT_NORMAL);
				graphics->setFontColor(255, 255, 255, 0, 0, 0);
				graphics->writeWrappedText(s, true, "%s", data);
				
				text[i] = graphics->createTexture(s);
				text[i]->iw = graphics->textWidth;
				text[i]->ih = graphics->textHeight;
				
				if(previousToken) {
					textureManager->removeTexture(previousToken);
					textureManager->addTexture(previousToken, text[i]);
				}
			}
			
			previousToken = token;
			
			strcpy(data, "");
			
			token = strtok(NULL, "\n");
			continue;
		}
		
		strcat(data, " ");
		strcat(data, token);
		token = strtok(NULL, "\n");
	}
	
	pak->freeAll();
}

void processAnimatedCutscene(List *data)
{
	Texture *text[64], *narrative, *speaker;
	char function[1024], args[1024], tmp[1024];
	game->cutsceneType = CUTSCENE_MOVIE;
	bool recognisedCommand = false;
	int cutsceneTextIndex = -1;
	float cutsceneTextTimer = 0;
	bool done = false;
	bool skip = false;
	bool waitForCamera = false;
	Vector explosionCenter, explosionPosition;
	int explosionRadius = 0;
	float waitTimer = 0;
	int tenPercent, y, bx = 0, by = 0;
	float explosionTimer;
	float fade = 0;
	float fadeSpeed = 0;
	float timer = 0;
	
	CutsceneCommand *cc = (CutsceneCommand*)data->getFirstElement();
	
	graphics->delay(1000, false);
	engine->resetTimeDifference();
	
	while (!done)
	{
		recognisedCommand = true;
		
		sscanf(cc->command.getText(), "%s%*c%[^;]", function, args);
		
		debug(("%d) Command = '%s' '%s'\n", cc->lineNumber, function, args));
		
		if (strcmp(function, "SKIP;") == 0)
		{
			skipCutscene(NULL);
			skip = false;
		}
		
		if (!skip)
		{
			if (strcmp(function, "WAIT") == 0)
			{
				waitTimer = atoi(args);
				waitForCamera = false;
			}
			else if (strcmp(function, "DO_EXPLOSIONS") == 0)
			{
				sscanf(args, "%*c%[^\"]%*c%d", tmp, &explosionRadius);
				explosionCenter = entityManager->getEntityPosition(tmp);
			}
			else if (strcmp(function, "STOP_EXPLOSIONS;") == 0)
			{
				explosionRadius = 0;
			}
			else if (strcmp(function, "SHOW_NARRATIVE") == 0)
			{
				cutsceneTextTimer = atoi(args);
				cutsceneTextIndex++;
				waitTimer = atoi(args);
			}
			else if (strcmp(function, "LOAD_NARRATIVE") == 0)
			{
				sscanf(args, "%s", tmp);
				buildCutsceneNarrative(tmp, text, 450, 100);
			}
			else if (strcmp(function, "LOAD_MUSIC") == 0)
			{
				sscanf(args, "%*c%[^\"]", tmp);
				
				audio->stopMusic();
				audio->loadMusic(tmp);
				audio->playMusic();
				
				timer = ((100 * 60) * 4) + (100 * 13);
			}
			else if (strcmp(function, "FADE") == 0)
			{
				fadeSpeed = atof(args);
				fade = 0;
				
				if (fadeSpeed == 0)
				{
					game->whiteOutTime = 0;
				}
			}
			else
			{
				recognisedCommand = false;
			
				for (int i = 0 ; i < 256 ; i++)
				{
					if (sceneInstruction[i].name == function)
					{
						recognisedCommand = true;
						sceneInstruction[i].function(args);
						break;
					}
				}
				
				if (strcmp(function, "CAMERA_CHASE") == 0)
				{
					waitTimer = RAND_MAX;
					waitForCamera = true;
					
					/*
					if (timer == 0)
					{
						timer = (100 * 49);
					}
					*/
				}
			}
			
			if (!recognisedCommand)
			{
				printf("ERROR: Cutscene command '%s' not recognised!\n", function);
				exit(1);
			}
			
			if (waitTimer > 0)
			{
				tenPercent = (int)(graphics->screen->h * 0.05);
				y = graphics->screen->h - tenPercent;
				explosionTimer = Math::rrand(10, 25);
				
				narrative = text[cutsceneTextIndex];
				speaker = NULL;
				
				if (cutsceneTextIndex != -1)
				{
					bx = graphics->screenMidX - (narrative->iw / 2);
					by = graphics->screen->h - 65 - (narrative->ih / 2);
					
					bx -= 5;
					by -= 5;
					
					char speakerName[1024];
					sscanf(narrative->name.getText(), "%*c%s", speakerName);
					if (strcmp(speakerName, "STORY"))
					{
						speaker = graphics->getGLString("%s", speakerName);
					}
				}
				
				while (waitTimer > 0)
				{	
					engine->doTimeDifference();
					
					graphics->updateScreen();
					engine->getInput();
					
					graphics->clearScreen();
					
					if (engine->keyState[SDL_SCANCODE_ESCAPE])
					{
						waitTimer = 0;
					}
					
					doGameObjects();
					drawGameObjects();
					
					camera->targetCamera->position += (camera->targetCamera->velocity * engine->getTimeDifference(TD_LOGIC));
					
					if (camera->targetCamera->chasing)
					{
						camera->targetCamera->chase.position += (camera->targetCamera->chase.velocity * engine->getTimeDifference(TD_LOGIC));
						waitTimer = 0;
					}
					
					if (camera->targetCamera->follow != NULL)
					{
						camera->targetCamera->destination = camera->targetCamera->follow->position;
					}
					
					waitTimer -= engine->getTimeDifference(TD_LOGIC);
					
					if (waitForCamera)
					{
						waitTimer = RAND_MAX;
						
						if (Math::getDistance(camera->targetCamera->chase.position, camera->targetCamera->chase.target) < engine->getTimeDifference(TD_LOGIC) * camera->targetCamera->speed)
						{
							waitForCamera = false;
							waitTimer = 0;
						}
					}
					
					graphics->setMode(MODE_2D);
					
					if (fadeSpeed > 0)
					{
						graphics->fade(fade);
						fade += engine->getTimeDifference(TD_LOGIC) * fadeSpeed;
					}
					
					graphics->drawRect(0, 0, graphics->screen->w, tenPercent, GLColor::black, false);
					graphics->drawRect(0, y, graphics->screen->w, tenPercent, GLColor::black, false);
					
					graphics->drawLine(0, tenPercent, graphics->screen->w, tenPercent, GLColor::grey);
					graphics->drawLine(0, y, graphics->screen->w, y, GLColor::grey);
					
					if (timer > 0)
					{
						timer -= engine->getTimeDifference(TD_LOGIC);
						
						if (timer <= 0)
						{
							audio->fadeMusic();
						}
						
						#if DEV
						graphics->drawString(graphics->screen->w, 5, TXT_RIGHT, GLColor::white, true, "%d", (int)(timer / 100));
						#endif
					}
					
					if (cutsceneTextTimer > 0)
					{
						cutsceneTextTimer -= engine->getTimeDifference(TD_LOGIC);
						
						// Main box...
						graphics->fade(0.75, bx, by, narrative->iw + 10, narrative->ih + 10);
						graphics->outlineRect(bx, by, narrative->iw + 10, narrative->ih + 10, GLColor::grey);
						
						glColor3f(1.0, 1.0, 1.0);
						graphics->blit(narrative, graphics->screenMidX, graphics->screen->h - 65, true);
						
						// Speaker box...
						if (speaker != NULL)
						{
							graphics->fade(0.75, bx, by - 20, speaker->iw + 10, 19);
							graphics->outlineRect(bx, by - 20, speaker->iw + 10, 19, GLColor::grey);
							
							glColor3f(1.0, 0.5, 0.0);
							graphics->blit(speaker, bx + 5, by - 4 - speaker->ih, false);
						}
					}
					
					if (explosionRadius != 0)
					{
						explosionTimer -= engine->getTimeDifference(TD_LOGIC);
					
						if (explosionTimer <= 0)
						{
							explosionPosition = explosionCenter;
							explosionPosition.x += Math::rrand(-explosionRadius, explosionRadius);
							explosionPosition.y += Math::rrand(-explosionRadius, explosionRadius);
							doExplosion(explosionPosition, 35, player);
							
							explosionTimer = Math::rrand(10, 50);
							
							addDebrisEmitter(explosionPosition);
						}
					}
				}
				
				if (speaker != NULL)
				{
					delete speaker;
				}
			}
		}
		
		if (engine->keyState[SDL_SCANCODE_ESCAPE])
		{
			skip = true;
			
			engine->keyState[SDL_SCANCODE_ESCAPE] = 0;
		}
		
		cc = (CutsceneCommand*)cc->next;
		
		if (cc == NULL)
		{
			done = true;
		}
	}
	
	for (Entity *ent = (Entity*)entityManager->blobList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->action = ent->walk;
		ent->thinkTime = 0;
	}
	
	for (Entity *ent = (Entity*)entityManager->enemyList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->action = ent->walk;
		ent->thinkTime = 0;
	}
	
	game->cutsceneType = CUTSCENE_NONE;
	
	entityManager->cameraList.remove(camera->targetCamera);
	camera->targetCamera = NULL;
	
	player->action = NULL;
}

void processStaticCutscene(List *data)
{
	Texture *scene[5];
	Texture *text[20];
	
	Texture *currentScene = NULL;
	Texture *currentText = NULL;
	
	float sceneFade = 0;
	
	for (int i = 0 ; i < 5 ; i++)
	{
		scene[i] = NULL;
	}
	
	for (int i = 0 ; i < 20 ; i++)
	{
		text[i] = NULL;
	}
	
	CutsceneCommand *cc = (CutsceneCommand*)data->getFirstElement();
	char function[1024], args[1024], tmp[1024];
	float delay = 0;
	int tx, ty;
	
	bool skip = false;
	bool done = false;
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	graphics->clearScreen();
	graphics->delay(1000, false);
	
	tx = graphics->getOffsetX() + 25;
	ty = graphics->screen->h - 90;
	
	int sceneIndex = 0;
	int textIndex = 0;
	
	engine->resetTimeDifference();
	
	while (!done)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		graphics->setMode(MODE_2D);
		
		if (engine->keyState[SDL_SCANCODE_ESCAPE])
		{
			done = true;
			skip = true;
			audio->stopMusic();
		}
		
		sceneFade += engine->getTimeDifference(TD_LOGIC) * 0.005;
		
		if (currentScene != NULL)
		{
			glColor4f(1.0, 1.0, 1.0, sceneFade);
			renderScaledTexture(currentScene);
		}
		
		if (currentText != NULL)
		{
			graphics->fade(0.85, tx - 10, ty - 10, 770, 80);
			graphics->outlineRect(tx - 10, ty - 10, 770, 80, GLColor::grey);
		
			glColor4f(1.0, 1.0, 1.0, 1.0);
			graphics->blit(currentText, tx, ty, false);
		}
		
		delay -= engine->getTimeDifference(TD_LOGIC);
		
		if (delay <= 0)
		{
			cc = (CutsceneCommand*)cc->next;
			
			if (cc == NULL)
			{
				done = true;
			}
			
			if (cc != NULL)
			{
				sscanf(cc->command.getText(), "%s%*c%[^;]", function, args);
				
				if (function[strlen(function) - 1] == ';')
				{
					function[strlen(function) - 1] = '\0';
				}
			
				debug(("%s (%s)\n", function, args));
			
				if (strcmp(function, "LOAD_IMAGE") == 0)
				{
					sscanf(args, "%s", tmp);
					scene[sceneIndex++] = getRequiredTexture(tmp);
				}
				else if (strcmp(function, "LOAD_NARRATIVE") == 0)
				{
					sscanf(args, "%s", tmp);
					buildCutsceneNarrative(tmp, text, 750, 100);
				}
				else if (strcmp(function, "PREPARE_SCENE") == 0)
				{
					sceneIndex = textIndex = 0;
				}
				else if (strcmp(function, "SHOW_IMAGE") == 0)
				{
					currentScene = scene[sceneIndex++];
				}
				else if (strcmp(function, "HIDE_IMAGE") == 0)
				{
					currentScene = NULL;
				}
				else if (strcmp(function, "SHOW_NARRATIVE") == 0)
				{
					currentText = text[textIndex++];
				}
				else if (strcmp(function, "HIDE_NARRATIVE") == 0)
				{
					currentText = NULL;
				}
				else if (strcmp(function, "WAIT") == 0)
				{
					delay = atof(args);
				}
			}
		}
	}
	
	if (!skip)
	{
		audio->fadeMusic();
		graphics->delay(3500, true);
	}
	
	graphics->clearScreen();
	graphics->delay(1000, true);
	audio->stopMusic();
	
	glClearColor(mission->fogColor.color[0], mission->fogColor.color[1], mission->fogColor.color[2], mission->fogColor.color[3]);
}

void processCutscene(List *data)
{
	debug(("processCutscene - %d\n", data->getSize()));

	if (data->getSize() == 0)
	{
		return;
	}
	
	audio->stopMusic();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	graphics->clearScreen();
	graphics->allowMotionBlur = false;
	
	if (mission->missionNumber < 31)
	{
		audio->loadMusic("music/Return To Napali.s3m");
	}
	
	audio->playMusic();
	
	glClearColor(mission->fogColor.color[0], mission->fogColor.color[1], mission->fogColor.color[2], mission->fogColor.color[3]);
	
	engine->resetTimeDifference();
	
	Properties props;
	props.setProperty("name", "CutsceneCamera");
	props.setProperty("position", "0 0 0");
	camera->targetCamera = addCutsceneCamera(&props);
	
	for (Entity *ent = (Entity*)entityManager->blobList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->action = entityDoNothing;
		ent->thinkTime = 0;
		ent->velocity.set(0, 0, 0);
	}
	
	for (Entity *ent = (Entity*)entityManager->enemyList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->action = entityDoNothing;
		ent->thinkTime = 0;
		ent->velocity.set(0, 0, 0);
	}
	
	for (Boss *ent = (Boss*)entityManager->bossList.getFirstElement() ; ent != NULL ; ent = (Boss*)ent->next)
	{
		if (ent->walk != energyBallRotate)
		{
			ent->action = entityDoNothing;
			ent->thinkTime = 0;
			ent->velocity.set(0, 0, 0);
		}
	}
	
	removeBulletsForCutscene();
	
	particleManager->particleList.clear();
	
	CutsceneCommand *cc = (CutsceneCommand*)data->getFirstElement();
	
	if (strstr(cc->command.getText(), "STATIC"))
	{
		processStaticCutscene(data);
	}
	else
	{
		processAnimatedCutscene(data);
	}
	
	for (Entity *ent = (Entity*)entityManager->blobList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->action = ent->walk;
		ent->thinkTime = 0;
	}
	
	for (Entity *ent = (Entity*)entityManager->enemyList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->action = ent->walk;
		ent->thinkTime = 0;
	}
	
	for (Boss *ent = (Boss*)entityManager->bossList.getFirstElement() ; ent != NULL ; ent = (Boss*)ent->next)
	{
		ent->action = ent->walk;
		ent->thinkTime = 0;
	}
	
	player->action = NULL;
	
	if (mission->missionNumber < 31)
	{
		if (game->totalRemaining == 0)
		{
			audio->loadMusic(mission->music.getText());
		}
		else
		{
			audio->loadMusic("music/Longstabben.xm");
			audio->loadSound(SND_WEATHER, "sound/ambience/general/klaxon.ogg");
		}
	}
	
	graphics->allowMotionBlur = true;
}

// ============= init statements ========================

void initCutsceneInstructions()
{
	int i = 0;
	
	// animated cut scenes...
	sceneInstruction[i++].set("CAMERA_POSITION_AT", &cameraPositionAt);
	sceneInstruction[i++].set("CAMERA_LOOK_AT", &cameraLookAt);
	sceneInstruction[i++].set("CAMERA_MOVE_TO", &cameraMoveTo);
	sceneInstruction[i++].set("CAMERA_STOP;", &cameraStop);
	sceneInstruction[i++].set("CAMERA_FOLLOW", &cameraFollow);
	sceneInstruction[i++].set("CAMERA_SHAKE", &cameraShake);
	sceneInstruction[i++].set("CAMERA_CHASE", &cameraChase);
	sceneInstruction[i++].set("CAMERA_END_CHASE;", &cameraChaseEnd);
	sceneInstruction[i++].set("CAMERA_CHASE_SPEED", &cameraChaseSpeed);
	
	sceneInstruction[i++].set("ENTITY_POSITION_AT", &entityPositionAt);
	sceneInstruction[i++].set("ENTITY_MOVE_TO", &entityMoveTo);
	sceneInstruction[i++].set("ENTITY_TURN", &entityTurn);
	sceneInstruction[i++].set("ENTITY_LOOK_AT", &entityLookAt);
	sceneInstruction[i++].set("ENTITY_LOOK_RANDOM", &entityLookRandom);
	sceneInstruction[i++].set("ENTITY_WATCH", &entityWatch);
	sceneInstruction[i++].set("ENTITY_JUMP", &entityJump);
	sceneInstruction[i++].set("ENTITY_VANISH", &entityVanish);
	sceneInstruction[i++].set("ENTITY_STOP", &entityStop);
	sceneInstruction[i++].set("ENTITY_HELPLESS", &entityHelpless);
	sceneInstruction[i++].set("ENTITY_REMOVE", &entityRemove);
	sceneInstruction[i++].set("ENTITY_WEIGHTLESS", &entityWeightless);
	
	sceneInstruction[i++].set("BEAM", &fireBeam);
	
	sceneInstruction[i++].set("BIG_EXPLOSION", &bigExplosion);
	
	sceneInstruction[i++].set("STRUCTURE_STATE", &structureStateChange);
	sceneInstruction[i++].set("BARRIER_STATE", &barrierStateChange);
	
	sceneInstruction[i++].set("BLOBS_MOVE_TO", &blobsMoveTo);
	sceneInstruction[i++].set("BLOBS_WATCH", &blobsWatch);
	sceneInstruction[i++].set("BLOBS_JUMP;", &blobsJump);
	
	sceneInstruction[i++].set("BIOMECHS_MOVE_TO", &bioMechsMoveTo);
	
	sceneInstruction[i++].set("SPAWN_ENEMIES", &spawnEnemies);
	sceneInstruction[i++].set("GALDOV_TRANSFORM_BEAMS;", &galdovTransformBeams);
	sceneInstruction[i++].set("TRANSFORM_ENEMIES;", &transformEnemies);
	sceneInstruction[i++].set("REMOVE_ALL_MIAS;", &removeAllMIAs);
	sceneInstruction[i++].set("REMOVE_ALL_BLOBS;", &removeAllBlobs);
	
	sceneInstruction[i++].set("SKIP;", &skipCutscene);
	
	sceneInstruction[i++].set("WHITE_OUT", &whiteOut);
}
