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

Feature::Feature()
{
	entityType = ET_FEATURE;
	
	silent = false;
	armySpawner = false;
	active = true;
	featureType = 0;
	minSpawnTime = 100;
	maxSpawnTime = 500;
	
	triggerId = -1;
}

Feature::~Feature()
{
}

void Feature::loadAdditional(Properties *props)
{
	active = props->getInt("active", active);
	silent = props->getInt("silent", silent);
	armySpawner = props->getInt("armySpawner", armySpawner);
	featureType = props->getInt("featureType", featureType);
	blobType = props->getString("blobType", blobType.getText());
	spawnedName = props->getString("spawnedName", blobType.getText());
	enemyType = props->getString("enemyType", enemyType.getText());
	minSpawnTime = props->getInt("minSpawnTime", minSpawnTime);
	maxSpawnTime = props->getInt("maxSpawnTime", maxSpawnTime);
	targetMission = props->getString("targetMission", targetMission.getText());
	triggerId = props->getInt("triggerId", triggerId);
	
	if (maxSpawnTime == 0)
	{
		minSpawnTime = 50;
		maxSpawnTime = 100;
	}
	
	if (blobType != "")
	{
		if ((blobType == "BLOB_HACKER") || (blobType == "BLOB_ENGINEER") || (blobType == "BLOB_DEMOLITION"))
		{
			Math::limit(&custom, 0, 1);
			minSpawnTime = 1;
			maxSpawnTime = 1;
		}
	}
}

void Feature::saveAdditional(Properties *props)
{
	props->setProperty("active", active);
	props->setProperty("silent", silent);
	props->setProperty("armySpawner", armySpawner);
	props->setProperty("featureType", featureType);
	props->setProperty("enemyType", enemyType.getText());
	props->setProperty("blobType", blobType.getText());
	props->setProperty("spawnedName", spawnedName.getText());
	props->setProperty("minSpawnTime", minSpawnTime);
	props->setProperty("maxSpawnTime", maxSpawnTime);
	props->setProperty("targetMission", targetMission.getText());
	props->setProperty("triggerId", triggerId);
}
