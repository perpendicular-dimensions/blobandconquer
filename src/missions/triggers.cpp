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

#include "triggers.h"

void addTrigger(Properties *props)
{
	Trigger *trigger = mission->spawnTrigger();

	trigger->type = engine->getValueOfDefine(props->getString("type", "type"));
	trigger->action = engine->getValueOfDefine(props->getString("action", "action"));
	
	trigger->load(props);
	
	debug(("Added trigger %s\n", trigger->name.getText()));
}

void assignTriggerLocations()
{
	debug(("assignTriggerLocations()\n"));
	
	for (Trigger *trigger = (Trigger*)mission->triggerList.getFirstElement() ; trigger != NULL ; trigger = (Trigger*)trigger->next)
	{
		trigger->assignTriggerLocation();
	}
}

void addLocation(Properties *props)
{
	int modelIndex;
	
	sscanf(props->getString("model", "*0"), "%*c%d", &modelIndex);
	
	MDLModel *mdlModel = modelManager->getMDLModel(props->getString("model", "NULL"));
	BSPModel *bspModel = &bsp->models[modelIndex];
	
	if ((mdlModel == NULL) || (bspModel == NULL) || (modelIndex == 0))
	{
		graphics->showErrorAndExit("createBaseStructure() - Bad BSP Model '%s'", props->getString("model", "NULL"));
	}
	
	Vector v;
	
	v.x = ((mdlModel->maxs.x) - (mdlModel->mins.x)) / 2;
	v.y = ((mdlModel->maxs.y) - (mdlModel->mins.y)) / 2;
	v.z = ((mdlModel->maxs.z) - (mdlModel->mins.z)) / 2;
	
	String name;
	name = props->getString("name", "NULL");
	
	if (name == "NULL")
	{
		printf("Locations MUST have names!!\n");
		exit(1);
	}
	
	Trigger::addLocationBox(name.getText(), mdlModel->originalPosition, v);
}

void performMainTriggerAction(Trigger *trigger)
{
	debug(("performMainTriggerAction() - ['%s', '%s']\n", trigger->name.getText(), trigger->targetName.getText()));
	
	switch (trigger->action)
	{
		case TRIGGER_ACTION_COMPLETE_OBJECTIVE:
			updateObjectiveStatus(trigger->objectiveId, OBJ_COMPLETE);
			break;
			
		case TRIGGER_ACTION_FAIL_OBJECTIVE:
			updateObjectiveStatus(trigger->objectiveId, OBJ_FAILED);
			break;
			
		case TRIGGER_ACTION_ENABLE_OBJECTIVE:
			enableObjective(trigger->objectiveId);
			break;
			
		case TRIGGER_ACTION_UPDATE_OBJECTIVE:
			if (updateObjectiveCurrentValue(trigger->objectiveId))
			{
				trigger->active = false;
			}
			break;
			
		case TRIGGER_ACTION_ENABLE_FEATURE:
			enableFeature(trigger->targetName.getText(), 1);
			break;
			
		case TRIGGER_ACTION_DISABLE_FEATURE:
			enableFeature(trigger->targetName.getText(), 0);
			break;
			
		case TRIGGER_ACTION_TOGGLE_STRUCTURE:
			//handleStructureStateChange(trigger->targetName.getText(), true);
			toggleStructureState(trigger->targetName.getText());
			break;
			
		case TRIGGER_ACTION_DESTROY_STRUCTURE:
			destroyStructure(trigger->targetName.getText());
			break;
			
		case TRIGGER_ACTION_TELEPORT_BLOB:
			teleportOutBlob(trigger->targetName.getText());
			break;
			
		case TRIGGER_ACTION_USE_CAMERA:
			activateInGameCutscene(trigger->targetName.getText(), 100);
			break;
			
		case TRIGGER_ACTION_EXIT_MISSION:
			game->setMissionStatus(MISSION_COMPLETE);
			game->changingLevel = true;
			game->targetMission = mission->getNextMissionName();
			break;
			
		case TRIGGER_ACTION_START_TIME_LIMIT:
			mission->timeLimit = trigger->timeLimit;
			game->totalRemaining = trigger->timeLimit;
			
			// load up and play the time limit music...
			audio->stopMusic();
			audio->loadMusic("music/Longstabben.xm");
			audio->freeSound(SND_WEATHER);
			audio->loadSound(SND_WEATHER, "sound/ambience/general/klaxon.ogg");
			audio->playMusic();
			audio->playSoundLoop(SND_WEATHER, CH_ENVIRONMENT);
			
			// need to do this because loading costs time...
			engine->resetTimeDifference();
			
			break;
			
		case TRIGGER_ACTION_STOP_TIME_LIMIT:
			mission->timeLimit = 0;
			game->totalRemaining = 0;
			break;
			
		case TRIGGER_ACTION_FUNCTION_POINTER:
			if (trigger->specialAction == NULL)
			{
				printf("Trigger '%s' requires a function pointer!\n", trigger->name.getText());
				exit(1);
			}
			trigger->specialAction();
			trigger->active = true; // can be activated multiple times...
			
			break;
			
		case TRIGGER_ACTION_STOP_ALARM:
			audio->haltChannel(CH_ENVIRONMENT);
			mission->alarmActive = false;
			break;
	}
}

void fireTriggers(const char *name, int type)
{
	debug(("fireTriggers() - '%s' (%d)\n", name, type));
	
	if (strcmp(name, "") == 0)
	{
		return;
	}
	
	// first check to see if a camera is part of the group... If it is then activate it first
	
	for (Trigger *trigger = (Trigger*)mission->triggerList.getFirstElement() ; trigger != NULL ; trigger = (Trigger*)trigger->next)
	{
		if (!trigger->active)
		{
			continue;
		}
		
		if ((trigger->type != type) && (type != TRIGGER_TYPE_ANY))
		{
			continue;
		}
		
		if (trigger->name == name)
		{
			if (trigger->action == TRIGGER_ACTION_USE_CAMERA)
			{
				debug(("fireTriggers() - Doing camera first...\n"));
				trigger->active = false;
				performMainTriggerAction(trigger);
				return;
			}
		}
	}
	
	debug(("fireTriggers() - No camera in group...\n"));
	
	for (Trigger *trigger = (Trigger*)mission->triggerList.getFirstElement() ; trigger != NULL ; trigger = (Trigger*)trigger->next)
	{
		if (!trigger->active)
		{
			continue;
		}
		
		if ((trigger->type != type) && (type != TRIGGER_TYPE_ANY))
		{
			continue;
		}
		
		if (trigger->name == name)
		{
			trigger->targetValue--;
			
			if (trigger->targetValue <= 0)
			{
				if ((trigger->action != TRIGGER_ACTION_UPDATE_OBJECTIVE) && (trigger->action != TRIGGER_ACTION_FAIL_OBJECTIVE))
				{
					trigger->active = false;
				}
			
				performMainTriggerAction(trigger);
			}
		}
	}
}

void triggerCheckLocation(Trigger *trigger)
{
	if (Collision::collision(player->position, player->boundingBox, trigger->position, trigger->boundingBox))
	{
		fireTriggers(trigger->name.getText(), TRIGGER_TYPE_ANY);
		trigger->active = 0;
	}
}

void doTriggers()
{
	for (Trigger *trigger = (Trigger*)mission->triggerList.getFirstElement() ; trigger != NULL ; trigger = (Trigger*)trigger->next)
	{
		if (trigger->type != TRIGGER_TYPE_LOCATION)
		{
			continue;
		}
		
		if (trigger->active)
		{
			triggerCheckLocation(trigger);
		}
	}
}
