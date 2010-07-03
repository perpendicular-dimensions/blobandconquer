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

Mission::Mission()
{
	weather = WEATHER_NORMAL;
	
	enemyLevel = 1;
	
	itemLimit = -1;
	
	nextLightning = 0;
	lightningTime = 0;
	
	timeLimit = 0;
	
	alarmActive = false;
	
	primaryObjectives.setName("Primary Objective List");
	secondaryObjectives.setName("Secondary Objective List");
	conditions.setName("Conditions List");
	
	triggerList.setName("Trigger List");
	
	spawnSize = 0;
	minSpawnTime = 0;
	maxSpawnTime = 0;
	
	fogColor.set(0.0, 0.0, 0.0, 1.0);
	fogMin = 1000;
	fogMax = 2000;
	fogDensity = 0.35;
	
	skySphere = "textures/game/overcast.png";
	skySphereSize = 1;
	
	missionNumber = -1;
	restartMissionName = "";
	exitToMission = "AUTO";
}

Mission *Mission::getInstance()
{
	return &instance;
}

Mission::~Mission()
{
	primaryObjectives.clear();
	secondaryObjectives.clear();
	conditions.clear();
	triggerList.clear();
	
	startCutsceneData.clear();
	endCutsceneData.clear();
}

void Mission::clear()
{
	primaryObjectives.clear();
	secondaryObjectives.clear();
	conditions.clear();
	triggerList.clear();
	
	enemyLevel = 1;
	
	itemLimit = -1;
	
	timeLimit = 0;
	
	spawnSize = 0;
	minSpawnTime = 0;
	maxSpawnTime = 0;
	
	enemySpawnList = "";
	autoInfo = "";
	
	bossDefFile = "";
	
	supplyCrateList = "";
	
	fogColor.set(0.0, 0.0, 0.0, 1.0);
	fogMin = 1000;
	fogMax = 2000;
	fogDensity = 0.35;
	
	startCutsceneData.clear();
	endCutsceneData.clear();
	
	startCutscene = "";
	endCutscene = "";
	
	restartMissionName = "";
	
	ambientEffects = "";
	
	skySphere = "textures/game/overcast.png";
}

Objective *Mission::spawnObjective(int type)
{
	Objective *objective;
	
	switch (type)
	{
		case OBJ_PRIORITY_PRIMARY:
			objective = new Objective();
			objective->priority = type;
			primaryObjectives.add(objective);
			return objective;
			break;
		
		case OBJ_PRIORITY_SECONDARY:
		case OBJ_PRIORITY_HIDDEN:
			objective = new Objective();
			objective->priority = type;
			secondaryObjectives.add(objective);
			return objective;
			break;
			
		case OBJ_PRIORITY_CONDITION:
			objective = new Objective();
			objective->priority = type;
			conditions.add(objective);
			return objective;
			break;
		
		default:
			printf("ERROR: Mission::spawnObjective - Type %d not recognised\n", type);
			exit(1);
			break;
	}
}

Trigger *Mission::spawnTrigger()
{
	Trigger *trigger = new Trigger();
	
	triggerList.add(trigger);
	
	return trigger;
}

int Mission::activeObjectives(int type)
{
	List *list;
	
	switch (type)
	{
		case OBJ_PRIORITY_PRIMARY:
			list = &primaryObjectives;
			break;
			
		case OBJ_PRIORITY_SECONDARY:
			list = &secondaryObjectives;
			break;
			
		case OBJ_PRIORITY_CONDITION:
			list = &conditions;
			break;

		default:
			return 0;
	}
	
	int rtn = 0;
	
	for (Objective *objective = (Objective*)list->getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (objective->active)
		{
			rtn++;
		}
	}
	
	return rtn;
}

Objective *Mission::getObjective(int id)
{
	for (Objective *objective = (Objective*)primaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (objective->id == id)
		{
			return objective;
		}
	}
	
	for (Objective *objective = (Objective*)secondaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (objective->id == id)
		{
			return objective;
		}
	}
	
	for (Objective *objective = (Objective*)conditions.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (objective->id == id)
		{
			return objective;
		}
	}
	
	return NULL;
}

bool Mission::allObjectiveComplete()
{
	for (Objective *objective = (Objective*)primaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (!objective->active)
		{
			continue;
		}
	
		if (objective->status == OBJ_INCOMPLETE)
		{
			return false;
		}
	}
	
	return true;
}

const char *Mission::getNextMissionName()
{
	static String rtn;
	
	if (exitToMission != "AUTO")
	{
		return exitToMission.getText();
	}
	
	rtn = "";
	rtn.setText("mission%.2d", missionNumber + 1);
	
	return rtn.getText();
}

void Mission::load(Properties *props)
{
	missionName = _(props->getString("missionName", missionName.getText()));

	bsp = props->getString("bsp", bsp.getText());
	music = props->getString("music", music.getText());
	ambientEffects = props->getString("ambientEffects", ambientEffects.getText());
	autoInfo = props->getString("autoInfo", autoInfo.getText());
	
	enemyLevel = props->getInt("enemyLevel", enemyLevel);
	itemLimit = props->getInt("itemLimit", itemLimit);
	timeLimit = props->getFloat("timeLimit", timeLimit);
	alarmActive = props->getInt("alarmActive", false);
	
	bossDefFile = props->getString("bossDefFile", bossDefFile.getText());
	
	fogColor = props->getColor("fogColor");
	fogMin = props->getFloat("fogMin", fogMin);
	fogMax = props->getFloat("fogMax", fogMax);
	fogDensity = props->getFloat("fogDensity", fogDensity);
	
	skySphere = props->getString("skySphere", skySphere.getText());
	skySphereSize = props->getInt("skySphereSize", skySphereSize);
	
	enemySpawnList = props->getString("enemySpawnList", enemySpawnList.getText());
	spawnSize = props->getInt("spawnSize", 0);
	minSpawnTime = props->getInt("minSpawnTime", 6000);
	maxSpawnTime = props->getInt("maxSpawnTime", 6000);
	
	supplyCrateList = props->getString("supplyCrateList", supplyCrateList.getText());
	
	missionNumber = props->getInt("missionNumber", missionNumber);
	restartMissionName = props->getString("restartMissionName", restartMissionName.getText());
	exitToMission = props->getString("exitToMission", exitToMission.getText());
	
	startCutscene = props->getString("startCutscene", startCutscene.getText());
	endCutscene = props->getString("endCutscene", endCutscene.getText());
	
	// Good place to cap the level. At level 5 a Pistol Blob will have 22 hp at most
	// compared to 4 at most at level 1(!)
	Math::limit(&enemyLevel, 1, 5);
}

void Mission::save(FILE *fp)
{
	Properties props;
	props.setName("Mission");
	
	props.setProperty("missionName", missionName.getText());
	
	props.setProperty("bsp", bsp.getText());
	props.setProperty("music", music.getText());
	props.setProperty("ambientEffects", ambientEffects.getText());
	props.setProperty("enemyLevel", enemyLevel);
	props.setProperty("itemLimit", itemLimit);
	props.setProperty("timeLimit", timeLimit);
	props.setProperty("alarmActive", alarmActive);
	
	props.setProperty("fogColor", fogColor.toString());
	props.setProperty("fogMin", fogMin);
	props.setProperty("fogMax", fogMax);
	props.setProperty("fogDensity", fogDensity);
	
	props.setProperty("bossDefFile", bossDefFile.getText());
	
	props.setProperty("weather", weather);
	
	props.setProperty("skySphere", skySphere.getText());
	props.setProperty("skySphereSize", skySphereSize);
	
	props.setProperty("enemySpawnList", enemySpawnList.getText());
	props.setProperty("spawnSize", spawnSize);
	props.setProperty("minSpawnTime", minSpawnTime);
	props.setProperty("maxSpawnTime", maxSpawnTime);
	
	props.setProperty("supplyCrateList", supplyCrateList.getText());
	
	props.setProperty("missionNumber", missionNumber);
	props.setProperty("restartMissionName", restartMissionName.getText());
	props.setProperty("exitToMission", exitToMission.getText());
	
	if (bossDefFile != "")
	{
		props.setProperty("startCutscene", startCutscene.getText());
	}
	props.setProperty("endCutscene", endCutscene.getText());
	
	props.save(fp);
	
	Objective *objective;
	
	for (objective = (Objective*)primaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		objective->save(fp);
	}
	
	for (objective = (Objective*)secondaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		objective->save(fp);
	}
	
	for (objective = (Objective*)conditions.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		objective->save(fp);
	}
	
	for (Trigger *trigger = (Trigger*)triggerList.getFirstElement() ; trigger != NULL ; trigger = (Trigger*)trigger->next)
	{
		trigger->save(fp);
	}
	
}

Mission Mission::instance;
