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

#include "mission.h"

void loadObjective(Properties *props)
{
	int priority = engine->getValueOfDefine(props->getString("priority", "priority"));

	Objective *objective = mission->spawnObjective(priority);

	objective->status = engine->getValueOfDefine(props->getString("status", "OBJ_INCOMPLETE"));
	
	objective->load(props);
}

void loadMission(Properties *props)
{
	mission->weather = engine->getValueOfDefine(props->getString("weather", "WEATHER_NORMAL"));
	
	mission->load(props);

	loadBSP(mission->bsp.getText());
	
	if (mission->startCutscene != "")
	{
		loadCutscene(mission->startCutscene.getText(), &mission->startCutsceneData);
	}
	
	if (mission->endCutscene != "")
	{
		loadCutscene(mission->endCutscene.getText(), &mission->endCutsceneData);
	}
	
	textureManager->addTexture("SkySphere", graphics->loadTexture(mission->skySphere.getText()));
	
	if (mission->bossDefFile != "")
	{
		loadBossDefinition(mission->bossDefFile.getText());
	}
	
	if (mission->timeLimit != 0)
	{
		game->totalRemaining = mission->timeLimit;
	}
}

void loadTarget(Properties *props)
{
	Q3Target *target;
	const char *name;
	
	if (props->hasProperty("name"))
	{
		name = props->getString("name", "NULL");
	}
	else
	{
		name = props->getString("targetname", "NULL");
	}
	
	target = entityManager->spawnQ3Target(name);
	
	target->name = name;
	target->origin = props->getVector("origin");
	
	debug(("loadTarget - Added %s: %s\n", target->name.getText(), target->origin.toString()));
}

void processAssignmentTargetList(List *list)
{
	debug(("processAssignmentTargetList()\n"));
	
	Q3Target *target;
	
	for (Entity *entity = (Entity*)list->getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->q3RadiantTarget == "")
		{
			continue;
		}
		
		target = (Q3Target*)entityManager->Q3TargetTable.get(entity->q3RadiantTarget.getText());
		
		if (target == NULL)
		{
			printf("ERROR: assignTargetsToEntities() - Couldn't find '%s'\n", entity->q3RadiantTarget.getText());
			printf("Failed Map: %s\n", mission->bsp.getText());
			printf("Failed Parent: %s, %d (%s)\n", entityNames[entity->entityType], entity->entityType, entity->getName());
			printf("Failed List: %s\n", list->name.getText());
			exit(1);
		}
		
		debug(("Assigned %s %s\n", entity->getName(), target->origin.toString()));
		
		if (list != &entityManager->structureList)
		{
			entity->destination = target->origin;
		}
		else
		{
			((Structure*)entity)->endPosition = target->origin;
		}
	}
	
	debug(("processAssignmentTargetList() - Done\n"));
}

void assignTargetsToEntities()
{
	debug(("assignTargetsToEntities()\n"));
	
	processAssignmentTargetList(&entityManager->featureList);
	processAssignmentTargetList(&entityManager->structureList);
	processAssignmentTargetList(&entityManager->cameraList);
	processAssignmentTargetList(&entityManager->trapList);
	
	debug(("assignTargetsToEntities() - Done\n"));
}

void setupEntities(List *list)
{
	debug(("setupEntities()\n"));
	
	Vector playerStart;
	bool playerStartFound = false;
	bool usePlayerStart = false;

	for (Properties *props = (Properties*)list->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		if (props->name == "Game")
		{
			game->load(props);
		}
		else if (props->name == "Statistics")
		{
			if (!game->changingLevel)
			{
				game->statistics.clear();
				game->statistics += props;
			}
		}
		else if (props->name == "Mission")
		{
			loadMission(props);
		}
		else if (props->name == "Objective")
		{
			loadObjective(props);
		}
		else if (props->name == "Trigger")
		{
			addTrigger(props);
		}
		else if (props->name == "info_player_start")
		{
			if (player == NULL)
			{
				spawnBob();
			}
			
			if (playerStartFound)
			{
				continue;
			}
			
			usePlayerStart = true;
			
			if (game->targetLocation == props->getString("name", ""))
			{
				playerStart = props->getVector("origin");
				playerStartFound = true;
			}
			else
			{
				playerStart = props->getVector("origin");
			}
		}
		else if (props->name == "player_debug_start")
		{
			playerStart = props->getVector("position");
			usePlayerStart = true;
			playerStartFound = true;
		}
		else if ((strstr(props->name.getText(), "BLOB_")) || (props->name == "Blob"))
		{
			addBlob(props);
		}
		else if ((strstr(props->name.getText(), "NME_")) || (props->name == "BioMech"))
		{
			addEnemy(props);
		}
		else if (strstr(props->name.getText(), "BOSS_"))
		{
			addBoss(props);
		}
		else if ((strstr(props->name.getText(), "WEP_")) || (props->name == "Weapon"))
		{
			if (game->noitems)
			{
				continue;
			}
			
			addWeapon(props);
		}
		else if ((strstr(props->name.getText(), "ITM_")) || (props->name == "Item"))
		{
			if (game->noitems)
			{
				continue;
			}
			
			addItem(props);
		}
		else if (props->name == "CustomItem")
		{
			addCustomItem(props);
		}
		else if (props->name == "Bullet")
		{
			addBullet(props);
		}
		else if ((strstr(props->name.getText(), "FEATURE_")) || (props->name == "Feature"))
		{
			addFeature(props);
		}
		else if ((props->name == "FUNC_DOOR") || (props->name == "Door"))
		{
			addDoor(props);
		}
		else if ((props->name == "FUNC_WINDOW") || (props->name == "Window"))
		{
			addWindow(props);
		}
		else if ((props->name == "FUNC_ENERGY_BARRIER") || (props->name == "Barrier"))
		{
			addEnergyBarrier(props);
		}
		else if ((props->name == "FUNC_LIFT") || (props->name == "Lift"))
		{
			addLift(props);
		}
		else if ((props->name == "FUNC_AUTO_LIFT") || (props->name == "AutoLift"))
		{
			addAutoLift(props);
		}
		else if ((props->name == "FUNC_PUSH_BLOCK") || (props->name == "PushBlock"))
		{
			addPushBlock(props);
		}
		else if ((props->name == "FUNC_WEAK_WALL") || (props->name == "WeakWall"))
		{
			addWeakWall(props);
		}
		else if ((props->name == "FUNC_WEAK_STRUCTURE") || (props->name == "WeakStructure"))
		{
			addWeakStructure(props);
		}
		else if ((props->name == "FUNC_STRUCTURE") || (props->name == "Structure"))
		{
			addStructure(props);
		}
		else if (props->name == "FUNC_LOCATION")
		{
			addLocation(props);
		}
		else if ((props->name == "FUNC_SUPPLY_CRATE") || (props->name == "SupplyCrate"))
		{
			addSupplyCrate(props);
		}
		else if ((props->name == "SWITCH_NORMAL") || (props->name == "Switch"))
		{
			addSwitch(props);
		}
		else if ((props->name == "SWITCH_ENGINEER") || (props->name == "SWITCH_HACKER") || (props->name == "SpecialSwitch"))
		{
			addSpecialSwitch(props);
		}
		else if ((props->name == "SWITCH_PRESSURE") || (props->name == "PressurePlate"))
		{
			addPressurePlate(props);
		}
		else if ((props->name == "SWITCH_DEMOLITION") || (props->name == "DemolitionPlate"))
		{
			addDemolitionPlate(props);
		}
		else if ((props->name == "CAMERA") || (props->name == "Camera"))
		{
			addCutsceneCamera(props);
		}
		else if ((strstr(props->name.getText(), "EMITTER_")) || (props->name == "Emitter"))
		{
			addEmitter(props);
		}
		else if ((strstr(props->name.getText(), "TRAP_")) || (props->name == "Trap"))
		{
			addTrap(props);
		}
		else if ((props->name == "TARGET") || (props->name == "Target"))
		{
			loadTarget(props);
		}
		else if (props->name == "Particle")
		{
			addParticle(props);
		}
		else if (props->name == "Decoration")
		{
			addDecoration(props);
		}
		else if (props->name == "Decal")
		{
			addDecal(props);
		}
		else if (props->name == "Crumb")
		{
			crumbBox.load(props);
		}
		else if (props->name == "EntityManager")
		{
			entityManager->load(props);
		}
		else if (props->name != "worldspawn")
		{
			printf("WARNING - setupEntities() '%s' unknown\n", props->name.getText());
			#if DEV
			printf("ERROR - setupEntities() '%s' unknown\n", props->name.getText());
			exit(1);
			#endif
			continue;
		}
	}
	
	// We only want to use this player start if we're not loading
	// a game and if the player object exists(!)
	if ((player != NULL) && (usePlayerStart))
	{
		debug(("Player start = %s\n", playerStart.toString()));
		player->position = playerStart;
	}

	delete list;
	
	if (entityManager->Q3TargetTable.getSize() > 0)
	{
		assignTargetsToEntities();
	}
	
	debug(("setupEntities() - Done\n"));
}

void loadMission(const char *missionName)
{
	List *list;
	String filename;
	
	filename.setText("data/missions/%s", missionName);
	
	if (strstr(missionName, "mission"))
	{
		char mn[3];
		mn[0] = missionName[7];
		mn[1] = missionName[8];
		mn[2] = '\0';
		mission->missionNumber = atoi(mn);
	}
	
	graphics->showLoadingProgress(_("Loading Mission..."));

	list = loadProperties(filename.getText(), true);
	setupEntities(list);

	list = loadBSPProperties(bsp->strEntities);
	setupEntities(list);
	
	reassignOwnerShips();
	
	removeBSPModelFaces();
	
	assignTriggerLocations();
	
	localizeInfoPoints(missionName);
	
	mission->restartMissionName = missionName;
	
	loadAmbientSounds();
}

void setObjectiveComplete(Objective *objective)
{
	debug(("setObjectiveComplete() - Objective '%s'\n", objective->description.getText()));
	
	bool hidden = false;
	
	objective->status = OBJ_COMPLETE;
	
	fireTriggers(objective->completionTrigger.getText(), TRIGGER_TYPE_ANY);

	if (mission->allObjectiveComplete())
	{
		for (objective = (Objective*)mission->conditions.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
		{
			if (objective->active)
			{
				objective->status = OBJ_COMPLETE;
			}
		}
		
		fireTriggers("MISSION_COMPLETE", TRIGGER_TYPE_MISSION_COMPLETE);
		
		for (objective = (Objective*)mission->primaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
		{
			if (!objective->active)
			{
				hidden = true;
			}
		}
		
		if (!hidden)
		{
			game->setInfoMessage(INFO_GOOD, _("***** Mission Complete *****"));
		}
	}
	else
	{
		game->setInfoMessage(INFO_GOOD, _("%s - Objective Completed"), objective->description.getText());
	}
}

void setObjectiveFailed(Objective *objective)
{
	debug(("setObjectiveFailed() - Objective '%s'\n", objective->description.getText()));

	game->setInfoMessage(INFO_BAD, _("%s - Objective Failed"), objective->description.getText());

	objective->status = OBJ_FAILED;
}

void setMissionFailed()
{
	for (Objective *objective = (Objective*)mission->primaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (objective->status == OBJ_INCOMPLETE)
		{
			objective->status = OBJ_FAILED;
		}
	}

	for (Objective *objective = (Objective*)mission->secondaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (objective->status == OBJ_INCOMPLETE)
		{
			objective->status = OBJ_FAILED;
		}
	}

	game->setInfoMessage(INFO_BAD, _("*** MISSION FAILED ****"));

	game->setMissionStatus(MISSION_FAILED);
}

void updateObjectiveStatus(int id, int status)
{
	Objective *objective = mission->getObjective(id);

	if (objective == NULL)
	{
		printf("WARNING - updateObjectiveStatus() did not find a matching objective number - %d\n", id);
		return;
	}

	if (status == OBJ_COMPLETE)
	{
		setObjectiveComplete(objective);
	}
	else if (status == OBJ_FAILED)
	{
		setObjectiveFailed(objective);

		if (objective->priority == OBJ_PRIORITY_CONDITION)
		{
			setMissionFailed();
		}
	}
}

void enableObjective(int id)
{
	Objective *objective = mission->getObjective(id);

	if (objective == NULL)
	{
		printf("WARNING - enableObjective() did not find a matching objective number - %d\n", id);
		return;
	}

	game->setInfoMessage(INFO_HINT, _("New Objective - %s"), objective->description.getText());
	objective->active = true;
	return;
}

bool updateObjectiveCurrentValue(int id)
{
	Objective *objective = mission->getObjective(id);

	if (objective == NULL)
	{
		printf("WARNING - updateObjectiveTargetCount() did not find a matching objective number - %d\n", id);
		return false;
	}
	
	objective->currentValue++;
	
	debug(("updateObjectiveCurrentValue() - %s [%d -> %d]\n", objective->description.getText(), objective->currentValue, objective->targetValue));

	if (objective->currentValue == objective->targetValue)
	{
		if (objective->priority != OBJ_PRIORITY_CONDITION)
		{
			setObjectiveComplete(objective);
			return true;
		}
		else
		{
			game->setInfoMessage(INFO_BAD, _("%s - Objective Failed"), objective->description.getText());
			setMissionFailed();
			return true;
		}
	}

	return false;
}
