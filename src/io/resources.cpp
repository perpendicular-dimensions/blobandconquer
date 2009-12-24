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

#include "resources.h"

Texture *getRequiredTexture(const char *textureFileName)
{
	if ((strcmp(textureFileName, "NULL") == 0) || (strcmp(textureFileName, "") == 0))
	{
		return NULL;
	}
	
	Texture *texture = textureManager->getTexture(textureFileName);
		
	if (texture == NULL)
	{
		texture = graphics->loadTexture(textureFileName);
		textureManager->addTexture(textureFileName, texture);
	}
	
	return texture;
}

SimpleModel *getRequiredSimpleModel(const char *modelFileName, int type)
{
	if ((strcmp(modelFileName, "NULL") == 0) || (strcmp(modelFileName, "") == 0))
	{
		return NULL;
	}
	
	SimpleModel *simpleModel = modelManager->getSimpleModel(modelFileName);
	
	if (simpleModel == NULL)
	{
		simpleModel = loadSimpleModel(modelFileName, type);
		modelManager->addSimpleModel(modelFileName, simpleModel);
	}
	
	return simpleModel;
}

MD2Model *getRequiredMD2Model(const char *modelFileName)
{
	if ((strcmp(modelFileName, "NULL") == 0) || (strcmp(modelFileName, "") == 0))
	{
		return NULL;
	}
	
	MD2Model *md2 = modelManager->getMD2Model(modelFileName);
	
	if (md2 == NULL)
	{
		md2 = loadMD2Model(modelFileName);
		modelManager->addMD2Model(modelFileName, md2);
	}
	
	return md2;
}

List *loadKeyValuePairs(const char *filename)
{
	if (!pak->unpack(filename, PAK::TEXT))
	{
		graphics->showErrorAndExit(ERR_FILE, filename);
	}
	
	List *list = new List();
	
	char *token = strtok((char*)pak->dataBuffer, "\n");
	char key[1024], value[1024];

	while (true)
	{
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

		sscanf(token, "%s %s", key, value);
		
		list->add(new Data(key, value));

		token = strtok(NULL, "\n");
	}
	
	pak->freeAll();
	
	return list;
}

void loadTextureSpecs()
{
	List *list = loadKeyValuePairs("data/texturespecs/list");
	List *specList;
	TextureSpec *textureSpec;
	
	for (Data *data = (Data*)list->getFirstElement() ; data != NULL ; data = (Data*)data->next)
	{
		specList = loadProperties(data->getValue(), false);
		
		for (Properties *props = (Properties*)specList->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
		{
			textureSpec = textureSpecManager->createDefaultTextureSpec(props->getName());
			
			textureSpec->load(props);
			
			textureSpec->lightType = engine->getValueOfDefine(props->getString("lightType", "LT_NORMAL"));
			textureSpec->mx = engine->getValueOfDefine(props->getString("mx", "TSM_NORMAL"));
			textureSpec->my = engine->getValueOfDefine(props->getString("my", "TSM_NORMAL"));
			
			textureSpec->contents = engine->getValueOfDefine(props->getString("contents", "CONTENTS_SOLID"));
		}
		
		delete specList;
	}
	
	if (list != NULL)
	{
		delete list;
	}
}

void loadWeapons()
{
	List *weaponList = loadProperties("data/gameDefs/weapons", false);
	
	int weaponId;
	WeaponDef *weaponDef;
	
	for (Properties *props = (Properties*)weaponList->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		weaponId = engine->getValueOfDefine(props->getString("type", "weapon-type-error"));
		
		if (weaponId == -1)
		{
			printf("ERROR: Weapon must have an id!\n");
			exit(1);
		}
		
		weaponDef = &entityManager->weaponDef[weaponId];
		
		weaponDef->type = weaponId;
		
		weaponDef->name = _(props->getString("name", "NULL"));
		
		weaponDef->modelName[0] = props->getString("model1", "NULL");
		weaponDef->modelName[1] = props->getString("model2", "NULL");
		
		weaponDef->textureName[0] = props->getString("texture1", "textures/game/bullet.png");
		weaponDef->textureName[1] = props->getString("texture2", "textures/game/bullet.png");
		
		weaponDef->bbMins = props->getVector("bbMins");
		weaponDef->bbMaxs = props->getVector("bbMaxs");
		weaponDef->scale = props->getFloat("scale", 1);
	
		weaponDef->clipSize = props->getInt("clipSize", 6);
		weaponDef->reloadTime = props->getInt("reloadTime", 5);
		
		weaponDef->unlimited = props->getInt("unlimited", 0);
		
		weaponDef->damage = props->getInt("damage", 1);
		weaponDef->damageType = engine->getValueOfDefine(props->getString("damageType", "DAMAGE_CONVENTENIAL"));
		
		weaponDef->hitType = engine->getValueOfDefine(props->getString("hitType", "PT_SPARK"));
		weaponDef->hitColor.set(props->getString("hitColor", "1.0 1.0 1.0 1.0"));
		weaponDef->blobColor.set(props->getString("blobColor", "1.0 1.0 1.0 1.0"));
		weaponDef->bioMechColor.set(props->getString("bioMechColor", "1.0 1.0 1.0 1.0"));
		
		weaponDef->trailType = engine->getValueOfDefine(props->getString("trailType", "TRAIL_NONE"));
		
		weaponDef->firingSound = engine->getValueOfDefine(props->getString("firingSound", "SND_PISTOL"));
		weaponDef->flags = engine->getValueOfFlagTokens(props->getString("flags", 0));
	}
	
	delete weaponList;
}

void loadItems()
{
	List *itemList = loadProperties("data/gameDefs/items", false);
	
	int itemId;
	ItemDef *itemDef;
	
	for (Properties *props = (Properties*)itemList->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		itemId = engine->getValueOfDefine(props->getString("type", "item-def-error"));
		
		if (itemId == -1)
		{
			printf("ERROR: Item must have an id!\n");
			exit(1);
		}
		
		itemDef = &entityManager->itemDef[itemId];
		
		itemDef->type = itemId;
		
		itemDef->name = _(props->getString("name", "NULL"));
	
		itemDef->modelName[0] = props->getString("model1", "NULL");
		itemDef->modelName[1] = props->getString("model2", "NULL");
		itemDef->textureName[0] = props->getString("texture1", "textures/game/bullet.png");
		itemDef->textureName[1] = props->getString("texture2", "textures/game/bullet.png");
		
		itemDef->itemPower = props->getInt("itemPower", -1);
		
		itemDef->bbMins = props->getVector("bbMins");
		itemDef->bbMaxs = props->getVector("bbMaxs");
		
		itemDef->itemType = engine->getValueOfDefine(props->getString("itemType", "NULL"));
		
		itemDef->collatable = props->getInt("collatable", 0);
	}
	
	delete itemList;
}

void loadEnemies()
{
	List *enemyList = loadProperties("data/gameDefs/enemies", false);
	
	int enemyId;
	EnemyDef *enemyDef;
	
	for (Properties *props = (Properties*)enemyList->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		enemyId = engine->getValueOfDefine(props->getString("type", "enemy-def-error"));
		
		enemyDef = &entityManager->enemyDef[enemyId];
		
		enemyDef->type = enemyId;
		
		enemyDef->minHP = props->getInt("minHP", 0);
		enemyDef->maxHP = props->getInt("maxHP", 0);
		
		enemyDef->shield = props->getInt("shield", 0);
	
		enemyDef->textureName[0] = props->getString("texture1", "NULL");
		enemyDef->textureName[1] = props->getString("texture2", "NULL");
		enemyDef->modelName[0] = props->getString("model1", "NULL");
		enemyDef->modelName[1] = props->getString("model2", "NULL");
		
		enemyDef->bbMins = props->getVector("bbMins");
		enemyDef->bbMaxs = props->getVector("bbMaxs");
		
		enemyDef->gunPosition[0] = props->getVector("gunPosition1");
		enemyDef->gunPosition[1] = props->getVector("gunPosition2");
		
		enemyDef->weapon = engine->getValueOfDefine(props->getString("weapon", "enemy-weapon"));
		
		enemyDef->flags = engine->getValueOfFlagTokens(props->getString("flags", "0"));
	}
	
	delete enemyList;
}

void loadBlobs()
{
	List *blobList = loadProperties("data/gameDefs/blobs", false);
	
	int blobId;
	BlobDef *blobDef;
	
	for (Properties *props = (Properties*)blobList->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		blobId = engine->getValueOfDefine(props->getString("type", "blob-def-error"));
		
		blobDef = &entityManager->blobDef[blobId];
		
		blobDef->type = blobId;
		
		blobDef->hp = props->getInt("hp", 0);
		blobDef->shield = props->getInt("shield", 0);
		
		blobDef->textureName[0] = props->getString("texture1", "NULL");
		blobDef->textureName[1] = props->getString("texture2", "NULL");
		blobDef->modelName[0] = props->getString("model1", "NULL");
		blobDef->modelName[1] = props->getString("model2", "NULL");
		
		blobDef->bbMins = props->getVector("bbMins");
		blobDef->bbMaxs = props->getVector("bbMaxs");
		
		blobDef->weapon = engine->getValueOfDefine(props->getString("weapon", "blob-weapon"));
		
		blobDef->flags = engine->getValueOfFlagTokens(props->getString("flags", "0"));
	}
	
	delete blobList;
}

void loadFeatures()
{
	List *featureList = loadProperties("data/gameDefs/features", false);
	
	int featureId;
	FeatureDef *featureDef;
	
	for (Properties *props = (Properties*)featureList->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		featureId = engine->getValueOfDefine(props->getString("type", "feature-def-error"));
		
		featureDef = &entityManager->featureDef[featureId];
		
		featureDef->type = featureId;
		featureDef->color = props->getColor("color");
		featureDef->segments = props->getInt("segments", 10);
		featureDef->radius = props->getFloat("radius", 10);
		featureDef->height = props->getFloat("height", 10);
	}
	
	delete featureList;
}

void loadBossDefinition(const char *filename)
{
	debug(("loadBossDefinition() - %s\n", filename));
	
	List *bossData = loadProperties(filename, false);
	
	int bossId;
	BossDef *bossDef;
	
	for (Properties *props = (Properties*)bossData->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{	
		bossId = engine->getValueOfDefine(props->getString("id", 0));
			
		bossDef = &entityManager->bossDef[bossId];
		
		bossDef->type = bossId;
		bossDef->textureName[0] = props->getString("texture1", "NULL");
		bossDef->textureName[1] = props->getString("texture2", "NULL");
		bossDef->modelName[0] = props->getString("model1", "NULL");
		bossDef->modelName[1] = props->getString("model2", "NULL");
		
		bossDef->health = props->getInt("health", 0);
		bossDef->shield = props->getInt("shield", 0);
		
		bossDef->bbMins = props->getVector("bbMins");
		bossDef->bbMaxs = props->getVector("bbMaxs");
		
		bossDef->gunPosition[0] = props->getVector("gunPosition1");
		bossDef->gunPosition[1] = props->getVector("gunPosition2");
		
		bossDef->flags = engine->getValueOfFlagTokens(props->getString("flags", "0"));
	}
	
	delete bossData;
	
	debug(("loadBossDefinition() - %s - Done\n", filename));
}

void loadAmbientSounds()
{
	if (mission->ambientEffects == "")
	{
		audio->amountOfAmbience = 0;
		return;
	}
	
	int i = 0;
	char sounds[1024];
	char *token;
	
	strcpy(sounds, mission->ambientEffects.getText());
	token = strtok(sounds, "|");
	
	while (token != NULL)
	{
		debug(("loadAmbientSounds() - '%s'\n", token));
	
		audio->loadAmbience(i++, token);
		token = strtok(NULL, "|");
	}
	
	audio->amountOfAmbience = i;
}

void loadCombatSounds()
{
	List *list = loadKeyValuePairs("data/combatResources/sounds");
	
	int soundIndex;
	
	for (Data *data = (Data*)list->getFirstElement() ; data != NULL ; data = (Data*)data->next)
	{
		soundIndex = engine->getValueOfDefine(data->getKey());
		
		if (!audio->loadSound(soundIndex, data->getValue()))
		{
			graphics->showErrorAndExit(ERR_FILE, data->getValue());
		}
	}
	
	if (list != NULL)
	{
		delete list;
	}
}

void loadTextures()
{
	List *list = loadKeyValuePairs("data/combatResources/textures");
	
	for (Data *data = (Data*)list->getFirstElement() ; data != NULL ; data = (Data*)data->next)
	{
		textureManager->addTexture(data->getValue(), graphics->loadTexture(data->getValue()));
	}
	
	if (list != NULL)
	{
		delete list;
	}
}

void loadRawModels()
{
	List *list = loadKeyValuePairs("data/combatResources/models");
	
	for (Data *data = (Data*)list->getFirstElement() ; data != NULL ; data = (Data*)data->next)
	{
		if (strstr(data->getValue(), "rw2"))
		{
			modelManager->addSimpleModel(data->getValue(), loadSimpleModel(data->getValue(), MDL_RAW));
		}
		else
		{
			modelManager->addSimpleModel(data->getValue(), loadSimpleModel(data->getValue(), MDL_PRIMITIVE));
		}
	}
	
	if (list != NULL)
	{
		delete list;
	}
}

void loadAllDefinitions()
{
	loadWeapons();
	loadItems();
	loadBlobs();
	loadEnemies();
	loadFeatures();
	loadCombatSounds();
	loadTextureSpecs();
}

void loadAllDefResources(EntityDef *def)
{
	for (int i = 0 ; i < 2 ; i++)
	{
		def->texture[i] = getRequiredTexture(def->textureName[i].getText());
		
		if (strstr(def->modelName[i].getText(), "rw2"))
		{
			def->model[i] = getRequiredSimpleModel(def->modelName[i].getText(), MDL_RAW);
		}
		else if (strstr(def->modelName[i].getText(), "primitive"))
		{
			def->model[i] = getRequiredSimpleModel(def->modelName[i].getText(), MDL_PRIMITIVE);
		}
		else if (strstr(def->modelName[i].getText(), "md2"))
		{
			def->md2 = getRequiredMD2Model(def->modelName[i].getText());
		}
	}
}

void loadAllDefResources()
{
	graphics->showLoadingProgress(_("Loading Resources..."));
	
	debug(("loadAllDefResources()\n"));
	
	for (int i = 0 ; i < MAX_BLOBS ; i++)
	{
		loadAllDefResources(&entityManager->blobDef[i]);
	}
	
	for (int i = 0 ; i < MAX_ENEMIES ; i++)
	{
		loadAllDefResources(&entityManager->enemyDef[i]);
	}
	
	for (int i = 0 ; i < MAX_ITEMS ; i++)
	{
		loadAllDefResources(&entityManager->itemDef[i]);
	}
	
	for (int i = 0 ; i < MAX_WEAPONS ; i++)
	{
		loadAllDefResources(&entityManager->weaponDef[i]);
	}
	
	for (int i = 0 ; i < MAX_BOSSES ; i++)
	{
		loadAllDefResources(&entityManager->bossDef[i]);
	}
	
	debug(("loadAllDefResources() :: Done\n"));
}

void loadAllBattleResources()
{
	loadTextures();
	loadRawModels();
	loadAllDefResources();
}

void loadMapAudio()
{
	graphics->showLoadingProgress(_("Loading Music..."));
	
	audio->freeSound(SND_WEATHER);
	
	switch (mission->weather)
	{
		case WEATHER_NORMAL:
		case WEATHER_FOG:
			break;
			
		case WEATHER_RAIN:
		case WEATHER_LIGHT_RAIN:
		case WEATHER_HEAVY_RAIN:
			audio->loadSound(SND_WEATHER, "sound/ambience/general/rain.ogg");
			break;
			
		case WEATHER_STORM:
			audio->loadSound(SND_WEATHER, "sound/ambience/general/storm.ogg");
			break;
	}
	
	if (game->totalRemaining == 0)
	{
		audio->loadMusic(mission->music.getText());
	}
	else
	{
		audio->loadMusic("music/Longstabben.xm");
		audio->loadSound(SND_WEATHER, "sound/ambience/general/klaxon.ogg");
	}
	
	if (mission->alarmActive)
	{
		audio->loadSound(SND_WEATHER, "sound/ambience/general/baseAlarm.ogg");
	}
}

void removeUnneededResources()
{
	debug(("removeUnneededResources()\n"));
	
	audio->freeAmbience();
	
	particleManager->particleList.clear();
	
	bbManager->clear();
	
	modelManager->clear();
	
	entityManager->enemyList.clear();
	entityManager->bossList.clear();
	entityManager->bulletList.clear();
	entityManager->decorationList.clear();
	entityManager->featureList.clear();
	entityManager->structureList.clear();
	entityManager->Q3TargetTable.clear();
	entityManager->emitterList.clear();
	entityManager->trapList.clear();
	entityManager->decalList.clear();
	entityManager->cameraList.clear();
	
	Entity *ent;
	
	for (ent = (Entity*)entityManager->blobList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		if (ent == player)
		{
			continue;
		}
		
		ent = (Entity*)ent->previous;
		entityManager->blobList.remove(ent->next);
	}
	
	for (ent = (Entity*)entityManager->weaponList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		if (ent->owner == player)
		{
			continue;
		}
		
		ent = (Entity*)ent->previous;
		entityManager->weaponList.remove(ent->next);
	}
	
	for (ent = (Entity*)entityManager->itemList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent = (Entity*)ent->previous;
		entityManager->itemList.remove(ent->next);
	}
	
	for (int i = 0 ; i < 5 ; i++)
	{
		game->item[i] = NULL;
	}
	
	for (int i = ITM_CUSTOM_FIRST ; i < ITM_CUSTOM_LAST ; i++)
	{
		entityManager->itemDef[i].name = "";
		entityManager->itemDef[i].modelName[0] = "";
		entityManager->itemDef[i].modelName[1] = "";
		entityManager->itemDef[i].textureName[0] = "";
		entityManager->itemDef[i].textureName[1] = "";
		entityManager->itemDef[i].bbMins.set(0, 0, 0);
		entityManager->itemDef[i].bbMaxs.set(0, 0, 0);
		entityManager->itemDef[i].customCarriable = false;
		entityManager->itemDef[i].collatable = false;
	}
	
	for (int i = 0 ; i < MAX_BOSSES ; i++)
	{
		entityManager->bossDef[i].textureName[0] = "";
		entityManager->bossDef[i].textureName[0] = "";
		entityManager->bossDef[i].modelName[1] = "";
		entityManager->bossDef[i].modelName[1] = "";
	}
	
	game->dropGrappleHooks = false;
	game->totalRemaining = 0;
	
	camera->shake = false;
	
	uiManager->clearWidgetList();
	
	bsp->clear();
	
	textureManager->clear();
	
	textureSpecManager->clearTextures();
	
	mission->clear();
	
	crumbBox.clear();
	
	debug(("removeUnneededResources() - Done\n"));
}
