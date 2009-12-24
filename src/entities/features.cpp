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

#include "features.h"

void drawFeature(bool transparent)
{
	Feature *feature;
	FeatureDef *featureDef;
	GLColor c;
	
	if (!transparent)
	{
		return;
	}
	
	feature = (Feature*)self;
	featureDef = (FeatureDef*)feature->definition;
	c = featureDef->color;

	switch (feature->featureType)
	{
		case FEATURE_TELEPORTER:
			c.color[3] = (!feature->active) ? 0.25 : 0.5;
			break;
				
		case FEATURE_EXIT:
			c.color[3] = (!feature->active) ? 0.1 : 0.5;
			break;

		case FEATURE_SAVE_POINT:
			c.color[3] = (!feature->active) ? 0.25 : self->custom;
			break;

		case FEATURE_INFORMATION:
			c.color[3] = 0.5;
			break;
	}
	
	if (feature->health > 0)
	{
		drawCyclinder(feature->position, featureDef->segments, featureDef->radius, featureDef->height, c);
	}
	else
	{
		drawCyclinder(feature->position, featureDef->segments, featureDef->radius + (self->health / 10), featureDef->height, c);
	}
}

void fireFeatureTrigger(const char *name)
{
	int triggerId = -1;

	for (Feature *feature = (Feature*)entityManager->featureList.getFirstElement() ; feature != NULL ; feature = (Feature*)feature->next)
	{
		if (feature->name == name)
		{
			if (feature->triggerId != -1)
			{
				triggerId = feature->triggerId;
			}

			feature->active = false;
			feature->health = 0;
		}
	}
}

const char *extractEnemyFromList(const char *enemyList)
{
	static String enemies[25];
	char list[1024];
	char *token;
	int i = 0;
	
	if (strcmp(enemyList, "") == 0)
	{
		printf("WARNING: extractEnemyFromList() - Enemy List is blank! Returning NME_PISTOL_BLOB!\n");
		return "NME_PISTOL_BLOB";
	}
	
	strcpy(list, enemyList);
	token = strtok(list, "|");
	
	while (token != NULL)
	{
		enemies[i] = token;
		
		token = strtok(NULL, "|");
		
		i++;
	}
	
	return enemies[rand() % i].getText();
}

void spawnerSpawnEntity()
{
	//debug(("spawnerSpawnEntity()\n"));
	
	static Properties props;
	
	props.clear();
	
	Vector position;
	
	Feature *feature = (Feature*)self;

	feature->thinkTime = Math::rrand(feature->minSpawnTime, feature->maxSpawnTime);
	
	position = feature->position;
	
	props.setProperty("position", position);
	props.setProperty("name", feature->spawnedName.getText());

	if (feature->blobType == "")
	{
		if (feature->armySpawner)
		{
			// too many enemies...
			if (entityManager->enemyList.getSize() > 75)
			{
				return;
			}
		}
		
		debug(("spawnerSpawnEntity() - Spawning Enemy\n"));
		
		position.x += Math::rrand(-((rand() % 5) * 10), ((rand() % 5) * 10));
		position.y += Math::rrand(-((rand() % 5) * 10), ((rand() % 5) * 10));
		props.setProperty("classname", extractEnemyFromList(feature->enemyType.getText()));
		addEnemy(&props);
		
		if ((--feature->custom) == 0)
		{
			feature->health = 0;
			feature->thinkTime = RAND_MAX;
		}
	}
	else
	{
		if (feature->blobType == "BLOB_SOLDIER")
		{
			// too many blobs... don't bother...
			if (getBlobSoldierCount() >= 12)
			{
				return;
			}
			
			props.setProperty("name", randomNameGenerator->getRandomName());
		}
		
		if (feature->armySpawner)
		{
			if (Math::getDistance(position, player->position) > 2500)
			{
				return;
			}
		}
		else
		{
			if ((--feature->custom) == 0)
			{
				feature->health = 0;
				feature->thinkTime = RAND_MAX;
			}
		}
		
		debug(("spawnerSpawnEntity() - Spawning Blob\n"));
		
		props.setProperty("classname", feature->blobType.getText());
		addBlob(&props);
	}

	addTeleportParticles(position);

	if (!feature->silent)
	{
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(self->position));
	}
	
	debug(("spawnerSpawnEntity() - Done\n"));
}

void exitPointSearch()
{
	BoundingBox box;
	
	if (player->health < 0)
	{
		return;
	}
	
	Feature *feature = (Feature*)self;
	
	box = feature->boundingBox;
	box.mins.x -= 6;
	box.mins.y -= 6;
	box.maxs.x += 6;
	box.maxs.y += 6;

	if (Collision::collision(player->position, player->boundingBox, self->position, box))
	{
		if (!feature->active)
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Cannot exit yet - Primary objectives not completed"));
			return;
		}
		
		Unit *teeka = findTeeka();
		
		if (teeka != NULL)
		{
			if (Math::getDistance(teeka->position, player->position) > 100)
			{
				game->setGamePlayMessage(INFO_GENERAL, _("Cannot leave without Teeka"));
				return;
			}
		}
		
		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to exit mission"));

		if (game->isControl(CTRL_ACTION))
		{
			String exitToMission;
			exitToMission = mission->getNextMissionName();
			
			debug(("Going to mission '%s'\n", exitToMission.getText()));
			
			if (exitToMission == "")
			{
				exitToMission = feature->targetMission.getText();
			}
			
			if (exitToMission != "TITLE_SCREEN")
			{
				game->setMissionStatus(MISSION_COMPLETE);
				game->changingLevel = true;
				game->targetMission = exitToMission.getText();
				game->resetControl(CTRL_ACTION);
			}
			else
			{
				game->setMissionStatus(MISSION_QUITTING);
			}
		}
	}
	
	feature->active = mission->allObjectiveComplete();

	self->thinkTime = 1;
}

void teleporterSearch()
{
	BoundingBox box;
	Vector mins, maxs;
	
	addTeleporterParticle(self->position);
	
	mins = self->position;
	mins.x -= 10;
	mins.y -= 10;
	
	maxs = self->position;
	maxs.x += 10;
	maxs.y += 10;
	
	int c = bbManager->initCollisionCandidates(mins, maxs);
	
	Entity *ent;
	
	bool doTeleportEffect;
	
	for (int i = 0 ; i < c ; i++)
	{
		ent = bbManager->candidate[i];
		
		doTeleportEffect = true;
		
		switch (ent->entityType)
		{
			case ET_BLOB:
			case ET_BIOMECH:
			case ET_WEAPON:
			case ET_ITEM:
			case ET_PUSHBLOCK:
				doTeleportEffect = true;
				break;
				
			case ET_BULLET:
			case ET_DECORATION:
				doTeleportEffect = false;
				break;
				
			default:
				continue;
				break;
		}
		
		if ((ent->health < 0) || (ent->flags & EF_TELEPORTING))
		{
			continue;
		}
		
		if (Math::getDistance(ent->position, self->position) <= (5 + ent->boundingBox.getHorizontalSize()))
		{
			if (doTeleportEffect)
			{
				addTeleportParticles(ent->position);
				audio->playSound(SND_TELEPORT3, (ent == player) ? CH_PLAYER : CH_SPAWN, camera->getSoundDistance(self->position));
			}
			
			// drop a breadcrumb on the teleporter
			// so the other Blobs can still follow
			if (ent == player)
			{
				crumbBox.addCrumb(player->position, true);
			}

			if (ent->entityType == ET_BLOB)
			{
				ent->destination = self->destination;
				ent->action = teleportUnit;
				ent->velocity.set(0, 0, 0);
				ent->thinkTime = 100;
				ent->contentType = CONTENTS_SOLID;
				((Unit*)ent)->liquidLevel = 0;
				((Unit*)ent)->currentCrumb = NULL;
	
				ent->flags |= EF_TELEPORTING;
			}
			else
			{
				ent->position = self->destination;
				
				if (doTeleportEffect)
				{
					addTeleportParticles(ent->position);
				}
			}
		}
	}
	
	bbManager->clearCollisionCandidates();
}

void savePointSearch()
{
	BoundingBox box;
	
	if ((player->health < 0) || (!(player->flags & EF_ONGROUND)))
	{
		return;
	}

	Feature *feature = (Feature*)self;

	feature->thinkTime = 1;
	
	box = feature->boundingBox;
	box.mins.x -= 6;
	box.mins.y -= 6;
	box.maxs.x += 6;
	box.maxs.y += 6;

	if (Collision::collision(player->position, player->boundingBox, self->position, box))
	{
		feature->custom += (0.01 * engine->getTimeDifference(TD_LOGIC));
		game->canSaveGame = true;

		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to Save Game"));

		if (game->isControl(CTRL_ACTION))
		{
			saveGame();
		}
	}
	else
	{
		feature->custom -= (0.01 * engine->getTimeDifference(TD_LOGIC));
	}

	Math::limit(&feature->custom, 0.3, 0.8);
}

void disableSavePoints()
{
	for (Feature *feature = (Feature*)entityManager->featureList.getFirstElement() ; feature != NULL ; feature = (Feature*)feature->next)
	{
		if (feature->featureType == FEATURE_SAVE_POINT)
		{
			feature->active = false;
		}
	}
}

void localizeInfoPoints(const char *missionName)
{
	debug(("localizeInfoPoints - %s\n", missionName));

	String filename;
	filename.setText(_("data/locale/en/%s"), missionName);
	
	if (!pak->unpack(filename.getText(), PAK::TEXT))
	{
		debug(("Couldn't load '%s'\n", filename.getText()));
		return;
	}
	
	Properties p;
	char *line = strtok((char*)pak->dataBuffer, "\n");
	char key[64], value[1024];
	
	while (line != NULL)
	{
		sscanf(line, "%s %*s %[^\n]", key, value);
		
		if (strcmp(key, "END") == 0)
		{		
			break;
		}
		
		p.setProperty(key, value);
		
		line = strtok(NULL, "\n");
	}

	for (Feature *feature = (Feature*)entityManager->featureList.getFirstElement() ; feature != NULL ; feature = (Feature*)feature->next)
	{
		if (feature->featureType != FEATURE_INFORMATION)
		{
			continue;
		}
		
		feature->name = p.getString(feature->name.getText(), "???");
	}
	
	mission->autoInfo = p.getString(mission->autoInfo.getText(), "");
	
	pak->freeAll();
}

void infoPointSearch()
{
	BoundingBox box;
	
	if ((player->health < 0) || (!(player->flags & EF_ONGROUND)))
	{
		return;
	}

	Feature *feature = (Feature*)self;

	feature->thinkTime = 1;
	
	box = feature->boundingBox;
	box.mins.x -= 4;
	box.mins.y -= 4;
	box.maxs.x += 4;
	box.maxs.y += 4;

	if (Collision::collision(player->position, player->boundingBox, self->position, box))
	{
		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to get Information"));

		if (game->isControl(CTRL_ACTION))
		{
			game->resetControl(CTRL_ACTION);
			feature->thinkTime = 5;
			showMessageDialog(feature->name.getText(), "OK");
		}
	}
}

void grapplePointSearch()
{
	BoundingBox box;
	
	if ((player->health < 0) || (!(player->flags & EF_ONGROUND)) || (player->action != NULL))
	{
		return;
	}

	Feature *feature = (Feature*)self;
	
	box = feature->boundingBox;
	box.mins.x -= 8;
	box.mins.y -= 8;
	box.maxs.x += 8;
	box.maxs.y += 8;

	if (Math::getDistance(feature->position, player->position) <= 10)
	{
		int slot = game->getItemSlot(_("Grappling Hook"));
		
		if ((slot == -1) && (!game->cheatItems))
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Grappling Hooks required"));
			return;
		}
		
		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to use Grappling Hook"));
		
		if (game->isControl(CTRL_ACTION))
		{
			if (!game->cheatItems)
			{
				game->item[slot]->itemPower--;
				
				if (game->item[slot]->itemPower == 0)
				{
					game->removeItem(_("Grappling Hook"));
				}
			}
			
			game->resetControl(CTRL_ACTION);
			
			addGrapplingHook(player, self->destination);
			
			faceLocation(player, self->destination);
			
			String s;
			s.setText("str_item_%d", slot);
			textureManager->removeTexture(s.getText());
			
			audio->playSound(SND_GRAPPLE_FIRE, CH_PLAYER, 128);
		}
	}
}

void enableFeature(const char *name, bool enable)
{
	for (Feature *feature = (Feature*)entityManager->featureList.getFirstElement() ; feature != NULL ; feature = (Feature*)feature->next)
	{
		if (feature->name == name)
		{
			feature->active = enable;
		}
	}
}

void addFeature(Properties *props)
{
	debug(("addFeature()\n"));
	
	if (strcmp(props->name.getText(), "FEATURE_INFORMATION")== 0 && strlen(props->getString("name", "")) == 0)
	{
		printf("Could not find ID for feature: %s\n", props->name.getText());
		
		abort();
	}
	
	int type = entityManager->getEntityType(props);

	Feature *feature = entityManager->spawnFeature(type);

	switch (type)
	{
		case FEATURE_SPAWNER:
			feature->custom = props->getInt("numberToSpawn", 1);
			feature->action = &spawnerSpawnEntity;
			break;

		case FEATURE_TELEPORTER:
			feature->action = &teleporterSearch;
			break;

		case FEATURE_EXIT:
			feature->action = &exitPointSearch;
			feature->thinkTime = 1;
			break;

		case FEATURE_SAVE_POINT:
			feature->action = &savePointSearch;
			feature->custom = 0.3;
			break;

		case FEATURE_INFORMATION:
			feature->action = &infoPointSearch;
			feature->custom = 1;
			break;
			
		case FEATURE_GRAPPLE_POINT:
			feature->action = &grapplePointSearch;
			feature->custom = 1;
			break;

		default:
			printf("WARNING: addFeature() - Feature Type not recognised!\n");
			break;
	}
	
	feature->load(props);

	feature->draw = drawFeature;
	
	// Mmmmmmm, nice hack!
	feature->position.z++;
	getCluster(feature);
	feature->position.z--;
	
	debug(("addFeature() - Done\n"));
}

void doFeatures()
{
	// made true by save points
	game->canSaveGame = false;

	for (Feature *feature = (Feature*)entityManager->featureList.getFirstElement() ; feature != NULL ; feature = (Feature*)feature->next)
	{
		self = feature;
		
		if (feature->health <= 0)
		{
			feature->health -= engine->getTimeDifference(TD_LOGIC);

			if (feature->health <= -100)
			{
				feature = (Feature*)feature->previous;
				entityManager->featureList.remove(feature->next);
				continue;
			}
		}

		if ((feature->active) || (feature->featureType == FEATURE_EXIT))
		{
			if (feature->performNextAction(engine->getTimeDifference(TD_LOGIC)))
			{
				if (feature->action != NULL)
				{
					feature->action();
				}
			}
		}
		
		if (withinViewableRange(feature))
		{
			entityManager->addEntityToDrawList(feature);
		}
	}
}
