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

EntityManager::EntityManager()
{
	blobList.setName("BlobList");
	enemyList.setName("EnemyList");
	itemList.setName("ItemList");
	weaponList.setName("WeaponList");
	decorationList.setName("DecorationList");
	bulletList.setName("BulletList");
	featureList.setName("FeatureList");
	structureList.setName("StructureList");
	cameraList.setName("CameraTable");
	Q3TargetTable.name = "TargetTable";
	emitterList.setName("EmitterList");
	trapList.setName("TrapList");
	decalList.setName("DecalList");
	bossList.setName("BossList");

	uniqueId = 0;
	
	mtGlow = 0;
	mtDir = 1;
}

EntityManager *EntityManager::getInstance()
{
	return &instance;
}

EntityManager::~EntityManager()
{
}

void EntityManager::destroy()
{
	clear();
}

Unit *EntityManager::spawnBlob(int type)
{
	uniqueId++;
	
	BlobDef *def = &blobDef[type];

	Unit *unit = new Unit(uniqueId);
	unit->entityType = ET_BLOB;
	unit->definition = def;
	unit->health = unit->maxHealth = def->hp;
	unit->shield = unit->maxShield = def->shield;
	unit->flags = def->flags;
	
	unit->boundingBox.mins = def->bbMins;
	unit->boundingBox.maxs = def->bbMaxs;
	unit->updateBoundingBox();
	
	unit->currentWeapon = &weaponDef[def->weapon];
	
	unit->gunPosition[0] = def->gunPosition[0];
	unit->gunPosition[1] = def->gunPosition[1];
	
	blobList.add(unit);

	return unit;
}

Unit *EntityManager::spawnEnemy(int type, int enemyLevel)
{
	uniqueId++;
	
	int healthLevel = enemyLevel;
	float healthModifier = 1;
	
	Math::limit(&healthLevel, 1, 3);
	
	if (type != NME_AQUA_BLOB)
	{
		healthModifier += (0.35 * (healthLevel - 1));
	}
	
	EnemyDef *def = &enemyDef[type];

	Unit *unit = new Unit(uniqueId);
	unit->entityType = ET_BIOMECH;
	unit->definition = def;
	unit->health = unit->maxHealth = Math::rrand((int)(def->minHP * healthModifier), (int)(def->maxHP * healthModifier));
	unit->shield = unit->maxShield = def->shield;
	unit->flags = def->flags;
	
	if (enemyLevel > 3)
	{
		int shieldChance = (enemyLevel == 4) ? 7 : 5;
		
		if ((rand() % shieldChance) == 0)
		{
			unit->shield = unit->maxShield = Math::rrand(5, 10);
		}
	}
	
	unit->boundingBox.mins = def->bbMins;
	unit->boundingBox.maxs = def->bbMaxs;
	unit->updateBoundingBox();
	
	unit->currentWeapon = &weaponDef[def->weapon];
	
	unit->gunPosition[0] = def->gunPosition[0];
	unit->gunPosition[1] = def->gunPosition[1];
	
	enemyList.add(unit);
	
	return unit;
}

Item *EntityManager::spawnItem(int type)
{
	uniqueId++;
	
	ItemDef *def = &itemDef[type];

	Item *item = new Item(uniqueId);
	item->definition = def;
	item->itemPower = def->itemPower;
	item->health = 9999;
	
	item->boundingBox.mins = def->bbMins;
	item->boundingBox.maxs = def->bbMaxs;
	item->updateBoundingBox();
	
	item->flags |= EF_BOUNCES;
	
	itemList.add(item);
	
	return item;
}

Weapon *EntityManager::spawnWeapon(int type, bool fullClip)
{
	uniqueId++;
	
	WeaponDef *def = &weaponDef[type];

	Weapon *weapon = new Weapon(uniqueId);
	weapon->health = 9999;
	weapon->definition = def;
	
	weapon->boundingBox.mins = def->bbMins;
	weapon->boundingBox.maxs = def->bbMaxs;
	weapon->updateBoundingBox();
	
	if (fullClip)
	{
		weapon->currentAmmo = ((WeaponDef*)weapon->definition)->clipSize;
	}
	
	weaponList.add(weapon);

	return weapon;
}

Decoration *EntityManager::spawnDecoration(int type)
{
	Decoration *decoration = new Decoration();
	decorationList.add(decoration);
	
	decoration->type = type;

	return decoration;
}

Bullet *EntityManager::spawnBullet(int type)
{
	Bullet *bullet = new Bullet();
	bulletList.add(bullet);
	
	bullet->definition = &weaponDef[type];
	bullet->flags = bullet->definition->flags;
	bullet->scale = ((WeaponDef*)bullet->definition)->scale;
	
	return bullet;
}

Feature *EntityManager::spawnFeature(int featureType)
{
	Feature *feature = new Feature();
	featureList.add(feature);
	
	feature->definition = &featureDef[featureType];
	feature->featureType = featureType;
	feature->health = 999;
	feature->thinkTime = 0;
	
	return feature;
}

Door *EntityManager::spawnDoor()
{
	Door *door = new Door();
	structureList.add(door);
	
	return door;
}

Switch *EntityManager::spawnSwitch()
{
	uniqueId++;

	Switch *swt = new Switch(uniqueId);
	structureList.add(swt);
	
	return swt;
}

Structure *EntityManager::spawnStructure(int type)
{
	Structure *structure = new Structure();
	structure->entityType = type;
	structureList.add(structure);
	
	return structure;
}

Emitter *EntityManager::spawnEmitter(int type)
{
	Emitter *emitter = new Emitter();
	emitter->type = type;
	emitterList.add(emitter);
	
	return emitter;
}

Trap *EntityManager::spawnTrap(int type)
{
	Trap *trap = new Trap();
	trap->trapType = type;
	trapList.add(trap);
	
	return trap;
}

Decal *EntityManager::spawnDecal()
{
	Decal *decal = new Decal();
	decalList.add(decal);
	
	return decal;
}

Boss *EntityManager::spawnBoss()
{
	Boss *boss = new Boss();
	bossList.add(boss);
	
	return boss;
}

Q3Target *EntityManager::spawnQ3Target(const char *name)
{
	Q3Target *target = new Q3Target();
	Q3TargetTable.put(name, target);
	
	return target;
}

CutsceneCamera *EntityManager::spawnCutsceneCamera(const char *name)
{
	CutsceneCamera *camera = new CutsceneCamera();
	cameraList.add(camera);
	
	return camera;
}

void EntityManager::addEntityToDrawList(Entity *entity)
{
	Reference *ref = new Reference();
	ref->object = entity;
	
	drawList.add(ref);
}

Entity *EntityManager::getOwnerById(unsigned int uniqueId)
{
	for (Unit *unit = (Unit*)enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->getUniqueId() == uniqueId)
		{
			return unit;
		}
	}
	
	for (Unit *unit = (Unit*)blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->getUniqueId() == uniqueId)
		{
			return unit;
		}
	}
	
	for (Structure *structure = (Structure*)structureList.getFirstElement() ; structure != NULL ; structure = (Structure*)structure->next)
	{
		if (structure->getUniqueId() == uniqueId)
		{
			return structure;
		}
	}
	
	return NULL;
}

Entity *EntityManager::getOwnerByName(const char *name)
{
	for (Unit *unit = (Unit*)enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->name == name)
		{
			return unit;
		}
	}
	
	for (Unit *unit = (Unit*)blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (unit->name == name)
		{
			return unit;
		}
	}
	
	for (Structure *structure = (Structure*)structureList.getFirstElement() ; structure != NULL ; structure = (Structure*)structure->next)
	{
		if (structure->name == name)
		{
			return structure;
		}
	}
	
	return NULL;
}

Item *EntityManager::getItemById(unsigned int uniqueId)
{
	for (Item *item = (Item*)itemList.getFirstElement() ; item != NULL ; item = (Item*)item->next)
	{
		if (item->getUniqueId() == uniqueId)
		{
			return item;
		}
	}
	
	return NULL;
}

Weapon *EntityManager::getWeaponById(unsigned int uniqueId)
{
	for (Weapon *weapon = (Weapon*)weaponList.getFirstElement() ; weapon != NULL ; weapon = (Weapon*)weapon->next)
	{
		if (weapon->getUniqueId() == uniqueId)
		{
			return weapon;
		}
	}
	
	return NULL;
}

int EntityManager::getEntityType(Properties *props)
{
	if (props->hasProperty("definitionType"))
	{
		return props->getInt("definitionType", -1);
	}
	
	if ((props->name == "ITM_CUSTOM") && (!props->hasProperty("definitionName")))
	{
		printf("ERROR - Cannot add custom item without a valid definitionName property!\n");
		exit(1);
	}
	
	// looks like a custom item... Try and find it...
	if ((props->hasProperty("definitionName")) && ((props->name == "ITM_CUSTOM") || (props->name == "Item")))
	{
		const char *name = props->getString("definitionName", "NULL");
		
		for (int i = ITM_CUSTOM_FIRST ; i < ITM_CUSTOM_LAST ; i++)
		{
			if (itemDef[i].name == name)
			{
				return i;
			}
		}
		
		printf("WARNING - Custom item specifies a custom definition of '%s' that was not found!\n", name);
		return -1;
	}
	
	return Engine::getInstance()->getValueOfDefine(props->getString("classname", "classname"));
}

Entity *EntityManager::getAnyEntityByName(const char *name)
{
	Entity *ent = getOwnerByName(name);
			
	if (ent != NULL)
	{
		return ent;
	}
	
	for (ent = (Entity*)bossList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		if (ent->name == name)
		{
			return ent;
		}
	}
	
	for (ent = (Entity*)itemList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		if (ent->name == name)
		{
			return ent;
		}
	}
	
	for (ent = (Entity*)weaponList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		if (ent->name == name)
		{
			return ent;
		}
	}
	
	for (ent = (Entity*)cameraList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		if (ent->name == name)
		{
			return ent;
		}
	}
	
	#if DEV
	printf("WARNING: EntityManager::getAnyEntityByName() - Nothing called '%s'\n", name);
	#endif
	
	return NULL;
}

Vector EntityManager::getEntityPosition(const char *name)
{
	Q3Target *target = (Q3Target*)Q3TargetTable.get(name);
	
	if (target != NULL)
	{
		return target->origin;
	}
	
	return getAnyEntityByName(name)->position;
}

float EntityManager::getEntityStrength(Entity *entity)
{
	switch (entity->entityType)
	{
		case ET_BLOB:
			return (entity->definition->type == BLOB_BOB) ? 0.35 : 0;
			break;
			
		case ET_BIOMECH:
			return 0.1;
			break;
			
		default:
			return 1.0;
			break;
	}
	
	// shouldn't get here...
	return 0.0;
}

void EntityManager::clear()
{
	blobList.clear();
	enemyList.clear();
	itemList.clear();
	weaponList.clear();
	decorationList.clear();
	featureList.clear();
	bulletList.clear();
	structureList.clear();
	cameraList.clear();
	Q3TargetTable.clear();
	bossList.clear();
	
	uniqueId = 0;
	
	mtGlow = 0;
	mtDir = 1;
}

void EntityManager::load(Properties *props)
{
	uniqueId = props->getInt("uniqueId", 0);
}

void EntityManager::save(FILE *fp)
{
	debug(("EntityManager::save()\n"));
	
	Unit *unit;
	Entity *ent;
	Boss *boss;
	
	debug(("EntityManager::save() - Blobs\n"));
	for (ent = (Entity*)blobList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Enemies\n"));
	for (unit = (Unit*)enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		unit->save(fp);
	}
	
	debug(("EntityManager::save() - Bosses\n"));
	for (boss = (Boss*)bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		if (boss->autoSaveToken != "")
		{
			boss->save(fp);
		}
	}
	
	debug(("EntityManager::save() - Items\n"));
	for (ent = (Entity*)itemList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Weapons\n"));
	for (ent = (Entity*)weaponList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Bullets\n"));
	for (ent = (Entity*)bulletList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Features\n"));
	for (ent = (Entity*)featureList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Decoration\n"));
	for (ent = (Entity*)decorationList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Structures\n"));
	for (ent = (Entity*)structureList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Cameras\n"));
	for (ent = (Entity*)cameraList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Emitters\n"));
	for (ent = (Entity*)emitterList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Traps\n"));
	for (ent = (Entity*)trapList.getFirstElement() ; ent != NULL ; ent = (Entity*)ent->next)
	{
		ent->save(fp);
	}
	
	debug(("EntityManager::save() - Decals\n"));
	for (Decal *decal = (Decal*)decalList.getFirstElement() ; decal != NULL ; decal = (Decal*)decal->next)
	{
		decal->save(fp);
	}
	
	debug(("EntityManager::save() - Q3TargetTable\n"));
	List *q3TargetList = Q3TargetTable.toList();
	Q3Target *target;
	for (Reference *ref = (Reference*)q3TargetList->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		target = (Q3Target*)ref->object;
		target->save(fp);
	}
	
	// Do this last to keep the Entity Manager in sync upon a reload!
	Properties props;
	props.setName("EntityManager");
	props.setProperty("uniqueId", uniqueId);
	props.save(fp);
	
	debug(("EntityManager::save() - Done\n"));
}

void EntityManager::saveCustomItems(FILE *fp)
{
	debug(("EntityManager::saveCustomItems()\n"));
	
	Properties props;
	props.setName("CustomItem");
	
	for (int i = ITM_CUSTOM_FIRST ; i < ITM_CUSTOM_LAST ; i++)
	{
		if (itemDef[i].name == "")
		{
			continue;
		}
		
		props.clear();
		
		props.setProperty("name", itemDef[i].name.getText());
		props.setProperty("model1", itemDef[i].modelName[0].getText());
		props.setProperty("model2", itemDef[i].modelName[1].getText());
		props.setProperty("texture1", itemDef[i].textureName[0].getText());
		props.setProperty("texture2", itemDef[i].textureName[1].getText());
		props.setProperty("bbMins", itemDef[i].bbMins);
		props.setProperty("bbMaxs", itemDef[i].bbMaxs);
		props.setProperty("customCarriable", itemDef[i].customCarriable);
		props.setProperty("collatable", itemDef[i].collatable);
		
		props.save(fp);
	}
}

EntityManager EntityManager::instance;
