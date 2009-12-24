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

#include "load.h"

void assignItem(Entity *ent, Item *item)
{
	Unit *unit;
	Structure *structure;
	
	switch (ent->entityType)
	{
		case ET_BLOB:
		case ET_BIOMECH:
			unit = (Unit*)ent;
			if (unit->currentItem != NULL)
			{
				printf("ERROR: Entities may only own one item!\n");
				exit(1);
			}
			unit->currentItem = item;
			item->setOwner(ent);
			break;
			
		case ET_SUPPLY_CRATE:
			structure = (Structure*)ent;
			if (structure->currentItem != NULL)
			{
				printf("ERROR: Entities may only own one item!\n");
				exit(1);
			}
			structure->currentItem = item;
			item->setOwner(ent);
			break;
			
		case ET_SWITCH:
			item->owner = ent;
			break;
			
		default:
			printf("ERROR: Cannot assign item to entity type '%s'\n", ent->getEntityTypeName());
			exit(1);
			break;
	}
	
	debug(("assignItemToUnit() - '%s' to '%s'\n", item->getName(), ent->getName()));
}

void assignItemToUnit(Item *item)
{
	debug(("assignItemToUnit()\n"));
	
	Entity *entity;
	
	if (item->ownerName != "")
	{
		entity = entityManager->getOwnerByName(item->ownerName.getText());
		
		if (entity == NULL)
		{
			printf("WARNING: Couldn't assign item to entity. Entity does not exist\n");
			return;
		}
		
		assignItem(entity, item);
		
		return;
	}
			
	entity = entityManager->getOwnerById(item->ownerId);
	
	assignItem(entity, item);
}

void reassignItems()
{
	debug(("reassignItems()\n"));
	
	debug(("reassignItems() - Weapons: %d\n", entityManager->weaponList.getSize()));
	debug(("reassignItems() - Items: %d\n", entityManager->itemList.getSize()));
	debug(("reassignItems() - Enemies: %d\n", entityManager->enemyList.getSize()));
	
	for (Item *item = (Item*)entityManager->itemList.getFirstElement() ; item != NULL ; item = (Item*)item->next)
	{
		if ((item->ownerId != 0) && (item->ownerId != player->getUniqueId()))
		{
			assignItemToUnit(item);
			continue;
		}
		
		if (item->ownerName != "")
		{
			assignItemToUnit(item);
			continue;
		}
	}
	
	if (game->changingLevel)
	{
		return;
	}
	
	for (int i = 0 ; i < 4 ; i++)
	{
		if (game->weaponId[i] != 0)
		{
			debug(("reassignItems() - Getting weapon %d: %d\n", i, game->weaponId[i]));
			
			game->weapon[i] = entityManager->getWeaponById(game->weaponId[i]);
			game->weapon[i]->setOwner(player);
		}
	}
	
	for (int i = 0 ; i < 5 ; i++)
	{
		if (game->itemId[i] != 0)
		{
			debug(("reassignItems() - Getting item %d: %d\n", i, game->itemId[i]));
			
			game->item[i] = entityManager->getItemById(game->itemId[i]);
			game->item[i]->setOwner(player);
		}
	}
	
	game->selectedWeapon = game->weapon[game->weaponIndex];
	
	if (game->selectedWeapon == NULL)
	{
		game->selectedWeapon = game->weapon[0];
	}
	
	debug(("reassignItems() :: Done\n"));
}

void reassignBullets()
{
	debug(("reassignBullets()\n"));
	
	for (Entity *entity = (Entity*)entityManager->bulletList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		entity->setOwner(entityManager->getOwnerById(entity->ownerId));
		
		debug(("Bullet owner = %s (%d)\n", entity->owner->getName(), entity->owner->getUniqueId()));
	}
	
	debug(("reassignBullets() :: Done\n"));
}

void reassignOwnerShips()
{
	reassignItems();
	reassignBullets();
}

void decompressSaveData(int slotId, bool restart)
{
	debug(("decompressSaveData()\n"));
	
	String filename;
	
	if (!restart)
	{
		filename.setText("%s/"SAVE_FORMAT, engine->userHomeDirectory.getText(), slotId);
	}
	else
	{
		filename.setText("%s/"SAVE_FORMAT"-restart", engine->userHomeDirectory.getText(), slotId);
	}
	
	FILE *fp = fopen(filename.getText(), "rb");
	
	if (!fp)
	{
		graphics->showErrorAndExit("Save game file '%s' is unavailable", filename.getText());
	}
	
	/*
	 * Pre 1.01 version. Save games are no longer compressed.
	 */
	char hash[1024];
	fscanf(fp, "%*s %s", hash);
	if (strcmp(hash, "###############") == 0)
	{
		fclose(fp);
		pak->unpack(filename.getText(), PAK::TEXT);
		return;
	}
	
	uLongf cSize, fSize;
	
	fseek(fp, (-sizeof(uLongf)) * 2, SEEK_END);
	fread(&cSize, sizeof(uLongf), 1, fp);
	fread(&fSize, sizeof(uLongf), 1, fp);
	
	debug(("decompressSaveData() - cSize: %ld\n", cSize));
	debug(("decompressSaveData() - fSize: %ld\n", fSize));
	
	unsigned char *input = NULL;
	
	if (pak->dataBuffer != NULL)
	{
		delete[] pak->dataBuffer;
	}

	pak->dataBuffer = NULL;

	input = new unsigned char[(int)(cSize * 1.01) + 12];
	pak->dataBuffer = new unsigned char[fSize];
	
	debug(("decompressSaveData() - Opening '%s'\n", filename.getText()));

	rewind(fp);
	fread(input, 1, cSize, fp);

	fclose(fp);
	
	uncompress(pak->dataBuffer, &fSize, input, cSize);
	
	if (input != NULL)
	{
		delete[] input;
	}
	
	input = NULL;
	
	debug(("decompressSaveData() :: Done\n"));
}

bool loadGameData(int slotId, bool restart)
{
	debug(("loadGameData() - %d\n", slotId));
	
	engine->startDebugTimer("loadGameData()");
	
	if (!game->changingLevel)
	{
		game->clear();
		entityManager->clear();
	}
	
	decompressSaveData(slotId, restart);
	
	List *list = loadPropertiesInMemory();
	
	if (list == NULL)
	{
		return false;
	}
	
	graphics->showLoadingProgress(_("Loading Game..."));
	
	setupEntities(list);
	
	reassignOwnerShips();
	
	engine->endDebugTimer("loadGameData()");
	
	removeBSPModelFaces();

	return true;
}

bool loadGame()
{
	int saveSlot = doLoadSaveGameDialog("Load");
	
	// don't bother! They pressed cancel!
	if (saveSlot == -1)
	{
		return false;
	}
	
	game->saveSlot = saveSlot;
	
	uiManager->onlyShowGroup("@none@");
	
	engine->resetTimeDifference();
	
	return true;
}
