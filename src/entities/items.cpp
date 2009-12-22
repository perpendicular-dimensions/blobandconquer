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

#include "items.h"

void drawItem(bool transparent)
{
	if (transparent)
	{
		return;
	}
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	
	glPushMatrix();
	{
		glColor4fv(self->color.color);
				
		glTranslatef(self->position.x, self->position.y, self->position.z);
		
		glRotatef(90.0, 1.0, 0.0, 0.0);
				
		glRotatef(self->rotation.x, 1.0, 0.0, 0.0);
		glRotatef(self->rotation.y, 0.0, 1.0, 0.0);
		glRotatef(self->rotation.z, 0.0, 0.0, 1.0);
				
		if (self->definition->type != ITM_STICKY_C4)
		{
			self->rotation.y += engine->getTimeDifference(TD_ANIMATION);
		}
		
		glScalef(self->scale, self->scale, self->scale);
				
		glBindTexture(GL_TEXTURE_2D, self->definition->texture[0]->data);
		glCallList(self->definition->model[0]->data);
	}
	glPopMatrix();
}

void stickyC4Countdown()
{
	self->custom--;
	
	self->thinkTime = 23;
	
	if (self->owner != NULL)
	{
		Math::limit(&self->custom, 0, 25);
	}
	else
	{
		Math::wrap(&self->custom, 0, 25);
	}
	
	if (((int)self->custom % 2) == 0)
	{
		self->color = GLColor::grey;
	}
	else
	{
		self->color = GLColor::lightGrey;
	}
}

void throwSpawnedItem(Entity *item)
{
	item->position.z += 5;
	
	item->velocity.x = Math::rrand(-8, 8);
	item->velocity.y = Math::rrand(-8, 8);
	item->velocity.z = Math::rrand(5, 10);
	
	item->velocity = item->velocity * 0.1;
}

int handleMedicalItem(Unit *unit, Item *item, bool pickupNow)
{
	if ((unit->health == unit->maxHealth) && (!(item->flags & EF_AUTO_PICKUP)))
	{
		return ITEM_LEAVE;
	}
	
	if (!pickupNow)
	{
		return ITEM_PICKUP;
	}
				
	if (unit == player)
	{
		game->setInfoMessage(INFO_PICKUP, _("Picked up %s"), ((ItemDef*)item->definition)->getName());
		game->alterStat(_("Cherries Picked Up"), 1);
	}
				
	audio->playSound(SND_GULP, CH_ANY, 128);
				
	unit->health += item->itemPower;
	Math::limit(&unit->health, 0, unit->maxHealth);
	
	return ITEM_CONSUME;
}

int handlePowerUp(Item *item, bool pickupNow)
{
	if (!pickupNow)
	{
		return ITEM_PICKUP;
	}
	
	WeaponDef *weaponDef;
	ItemDef *itemDef = (ItemDef*)item->definition;
	Unit *teeka;
	int weaponType;
	
	switch (itemDef->type)
	{
		case ITM_SHIELD_BELT:
			player->maxShield = max(player->maxShield, item->itemPower);
			
			game->setInfoMessage(INFO_PICKUP, _("Picked up a Shield Belt (%dVGhz)"), item->itemPower);
			
			addPowerUpParticles(player->position);
			break;
			
		case ITM_TEEKA_WEAPON:
			weaponType = engine->getValueOfDefine(item->getName());
			game->teekaWeapon = weaponType;
			weaponDef = &entityManager->weaponDef[weaponType];
			
			teeka = findTeeka();
			
			if (teeka != NULL)
			{
				teeka->currentWeapon = weaponDef;
				addPowerUpParticles(teeka->position);
			}
			
			game->setInfoMessage(INFO_PICKUP, _("Picked up %s for Teeka"), weaponDef->getName());
			
			break;
	}
	
	//audio->playSound(SND_POWERUP, CH_ANY);
	audio->playSound(SND_ITEM, CH_ANY, 128);
	
	game->alterStat(_("Items Picked Up"), 1);
	
	return ITEM_CONSUME;
}

int handleSpecialItem(Item *item, bool pickupNow)
{
	// Player must always pick these up themselves...
	if (!pickupNow)
	{
		return ITEM_LEAVE;
	}
	
	ItemDef *itemDef = (ItemDef*)item->definition;
	int sameItemSlot = (!itemDef->collatable) ? -1 : game->getItemSlot(itemDef->getName());
	int freeSlot = game->getFreeInventorySlot();
	
	if (itemDef->type == ITM_GRAPPLE_HOOK)
	{
		game->dropGrappleHooks = true;
	}
	
	if (sameItemSlot == -1)
	{
		if (itemDef->type < ITM_CUSTOM_FIRST)
		{
			if ((freeSlot == -1) && (!(item->flags & EF_AUTO_PICKUP)))
			{
				return ITEM_LEAVE;
			}
		}
		else if (itemDef->customCarriable)
		{
			if (freeSlot == -1)
			{
				return ITEM_LEAVE;
			}
		}
	}
	else
	{
		// can't carry more than five at a time...
		if (game->item[sameItemSlot]->definition->type != ITM_TELETAG)
		{
			if (game->item[sameItemSlot]->itemPower >= 5)
			{
				game->setGamePlayMessage(INFO_GENERAL, _("Cannot carry any more %ss"), itemDef->getName());
				return ITEM_LEAVE;
			}
		}
		
		game->item[sameItemSlot]->itemPower += item->itemPower;
		
		String s;
		s.setText("str_item_%d", sameItemSlot);
		textureManager->removeTexture(s.getText());
	}
	
	audio->playSound(SND_ITEM, CH_ANY, 128);
	
	const char *translatedName = (itemDef->type < ITM_CUSTOM_FIRST) ? itemDef->getName() : itemDef->getName();
	
	game->setInfoMessage(INFO_PICKUP, _("Picked up %s"), translatedName);
	
	game->alterStat(_("Items Picked Up"), 1);
	
	if (sameItemSlot == -1)
	{
		if ((itemDef->type < ITM_CUSTOM_FIRST) || (itemDef->customCarriable))
		{
			item->setOwner(player);
			
			game->item[freeSlot] = item;
			
			return ITEM_PICKUP;
		}
	}
	
	return ITEM_CONSUME;
}

int handleExplosiveWeapon(Weapon *weapon, bool pickupNow, WeaponDef *weaponDef)
{
	// Galdov can do this in the last battle!
	if (game->weapon[3] == NULL)
	{
		weapon->setOwner(player);
		game->weapon[3] = weapon;
		audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
		game->setInfoMessage(INFO_PICKUP, _("Picked up %s"), weaponDef->getName());
		return ITEM_PICKUP;
	}
	
	int rtn = ITEM_LEAVE;
	int clipSize = ((WeaponDef*)game->weapon[3]->definition)->clipSize;
	
	// we're holding grenades. Replace with this weapon...
	if ((game->weapon[3]->definition->type == WEP_GRENADE) && (weaponDef->type != WEP_GRENADE))
	{
		if (!pickupNow)
		{
			return ITEM_PICKUP;
		}
		
		weapon->setOwner(player);
		
		weapon->currentAmmo += game->weapon[3]->currentAmmo;
	
		// we need to destroy the grenades...
		entityManager->weaponList.remove(game->weapon[3]);
		
		game->weapon[3] = weapon;
		
		//game->selectedWeapon = weapon;
	
		audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
		game->setInfoMessage(INFO_PICKUP, _("Picked up %s"), weaponDef->getName());
		
		rtn = ITEM_PICKUP;
	}
	else if (weaponDef->type == WEP_GRENADE)
	{
		if (game->weapon[3]->currentAmmo < clipSize)
		{
			if (!pickupNow)
			{
				return ITEM_PICKUP;
			}
			
			game->weapon[3]->currentAmmo += weapon->currentAmmo;
			
			Math::limit(&game->weapon[3]->currentAmmo, 0, clipSize);
			
			audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
		
			game->setInfoMessage(INFO_PICKUP, _("Picked up some %s"), weaponDef->getName());
			
			rtn = ITEM_CONSUME;
		}
		else if (weapon->flags & EF_AUTO_PICKUP)
		{
			audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
			return ITEM_CONSUME;
		}
	}
	else 
	{
		if (game->weapon[3]->definition->type == weapon->definition->type)
		{
			if (game->weapon[3]->currentAmmo < clipSize)
			{
				if (!pickupNow)
				{
					return ITEM_PICKUP;
				}
			
				game->weapon[3]->currentAmmo += weapon->currentAmmo;
			
				Math::limit(&game->weapon[3]->currentAmmo, 0, clipSize);
			
				audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
		
				game->setInfoMessage(INFO_PICKUP, _("Picked up some %s"), weaponDef->getName());
			
				rtn = ITEM_CONSUME;
			}
		}
		else
		{
			if (!pickupNow)
			{
				return ITEM_LEAVE;
			}
		
			if (weapon->flags & EF_AUTO_PICKUP)
			{
				audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
				return ITEM_CONSUME;
			}
		
			game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to swap %s for %s"), game->selectedWeapon->definition->getName(), weaponDef->getName());
			
			if (game->isControl(CTRL_ACTION))
			{
				game->resetControl(CTRL_ACTION);
				
				int ammo = game->weapon[3]->currentAmmo;
				
				// put first weapon back on map...
				game->weapon[3]->position = player->position;
				game->weapon[3]->custom = 100;
				game->weapon[3]->setOwner(NULL);
				throwSpawnedItem(game->weapon[3]);
				
				// get second weapon
				game->weapon[3] = weapon;
				game->weapon[3]->setOwner(player);
				game->weapon[3]->currentAmmo = ammo;
				game->selectedWeapon = game->weapon[3];
			
				audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
				game->setInfoMessage(INFO_PICKUP, _("Picked up %s"), weaponDef->getName());
			
				String s;
				s.setText("str_weapon_%d", 3);
				textureManager->removeTexture(s.getText());
				textureManager->removeTexture("str_current_ammo");
			
				rtn = ITEM_PICKUP;
			}
		}
	}
	
	if (rtn != ITEM_LEAVE)
	{
		game->alterStat(_("Weapons Picked Up"), 1);
	
		String s;
		s.setText("str_weapon_%d", 3);
		textureManager->removeTexture(s.getText());
		textureManager->removeTexture("str_current_ammo");
	}
	
	return rtn;
}

int handleNormalWeapon(Weapon *weapon, bool pickupNow, WeaponDef *weaponDef)
{
	int freeSlot = -1;
	int currentWeaponSlot = -1;
	
	for (int i = 3 ; i > -1 ; i--)
	{
		if (game->weapon[i] == NULL)
		{
			freeSlot = i;
		}
		else if (game->weapon[i]->definition == weapon->definition)
		{
			currentWeaponSlot = i;
		}
	}
	
	// Does not have the weapon and has a free slot
	if ((currentWeaponSlot == -1) && (freeSlot != -1))
	{
		if (!pickupNow)
		{
			return ITEM_PICKUP;
		}
		
		weapon->setOwner(player);
		
		game->weapon[freeSlot] = weapon;
		game->weapon[freeSlot]->currentAmmo = weapon->currentAmmo;
		game->selectedWeapon = game->weapon[freeSlot];
		
		game->weaponIndex = freeSlot;
			
		audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
		game->setInfoMessage(INFO_PICKUP, _("Picked up %s"), weaponDef->getName());
		game->alterStat(_("Weapons Picked Up"), 1);
			
		return ITEM_PICKUP;
	}
	else if (currentWeaponSlot != -1)
	{
		if (game->weapon[currentWeaponSlot]->currentAmmo ==	weaponDef->clipSize)
		{
			if (weapon->flags & EF_AUTO_PICKUP)
			{
				audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
				return ITEM_CONSUME;
			}
			
			return ITEM_LEAVE;
		}
		
		if (!pickupNow)
		{
			return ITEM_PICKUP;
		}
			
		// auto switch to better weapon if not underwater
		if (!(player->contentType & CONTENTS_WATER))
		{
			if ((game->selectedWeapon == game->weapon[0]) && (game->weapon[currentWeaponSlot]->currentAmmo == 0))
			{
				game->selectedWeapon = game->weapon[currentWeaponSlot];
				game->weaponIndex = currentWeaponSlot;
			}
		}
			
		game->weapon[currentWeaponSlot]->currentAmmo += weapon->currentAmmo;
			
		Math::limit(&game->weapon[currentWeaponSlot]->currentAmmo, 0, weaponDef->clipSize);
			
		audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
		
		if (weaponDef->type != WEP_GRENADE)
		{
			game->setInfoMessage(INFO_PICKUP, _("Got some %s ammo"), weaponDef->getName());
		}
		else
		{
			game->setInfoMessage(INFO_PICKUP, _("Picked up some %s"), weaponDef->getName());
		}
		
		game->alterStat(_("Weapons Picked Up"), 1);
		
		String s;
		s.setText("str_weapon_%d", currentWeaponSlot);
		textureManager->removeTexture(s.getText());
		textureManager->removeTexture("str_current_ammo");
			
		return ITEM_CONSUME;
	}
	else if ((freeSlot == -1) && (game->selectedWeapon != game->weapon[0]) && (game->selectedWeapon != game->weapon[3]))
	{
		if (!pickupNow)
		{
			return ITEM_LEAVE;
		}
		
		if (weapon->flags & EF_AUTO_PICKUP)
		{
			audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
			return ITEM_CONSUME;
		}
		
		game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to swap %s for %s"), game->selectedWeapon->definition->getName(), weaponDef->getName());
			
		if (game->isControl(CTRL_ACTION))
		{
			game->resetControl(CTRL_ACTION);
				
			int i = game->getSelectedWeaponSlot();
			
			// put first weapon back on map...
			game->weapon[i]->position = player->position;
			game->weapon[i]->custom = 100;
			game->weapon[i]->setOwner(NULL);
			throwSpawnedItem(game->weapon[i]);
			
			/*
			if (game->weapon[i]->currentAmmo <= 0)
			{
				game->weapon[i]->health = -100;
			}
			*/
				
			// get second weapon
			game->weapon[i] = weapon;
			game->weapon[i]->setOwner(player);
			game->selectedWeapon = game->weapon[i];
			
			audio->playSound(SND_PICKUP_WEAPON, CH_PICKUP, 128);
			game->setInfoMessage(INFO_PICKUP, _("Picked up %s"), weaponDef->getName());
			
			String s;
			s.setText("str_weapon_%d", i);
			textureManager->removeTexture(s.getText());
			textureManager->removeTexture("str_current_ammo");
			
			return ITEM_PICKUP;
		}
	}
	
	return ITEM_LEAVE;
}

int handleWeaponItem(Weapon *weapon, bool pickupNow)
{
	WeaponDef *weaponDef = (WeaponDef*)weapon->definition;
		
	if (!(weaponDef->flags & EF_EXPLODES))
	{
		return handleNormalWeapon(weapon, pickupNow, weaponDef);
	}
	else
	{
		return handleExplosiveWeapon(weapon, pickupNow, weaponDef);
	}
	
	return ITEM_LEAVE;
}

int pickUpItem(Unit *unit, Entity *ent, bool pickupNow)
{
	if (ent->entityType == ET_ITEM)
	{
		Item *item = (Item*)ent;
		ItemDef *itemDef = (ItemDef*)item->definition;
		
		switch (itemDef->itemType)
		{
			case IT_MEDICAL:
				return handleMedicalItem(unit, item, pickupNow);
				break;
				
			case IT_POWER_UP:
				if (unit == player)
				{
					return handlePowerUp(item, pickupNow);
				}
				return ITEM_LEAVE;
				break;
				
			case IT_SPECIAL:
				if (unit == player)
				{
					return handleSpecialItem(item, pickupNow);
				}
				return ITEM_LEAVE;
				break;
		}
	}
	else if (unit == player)
	{
		return handleWeaponItem((Weapon*)ent, pickupNow);
	}
	
	return ITEM_LEAVE;
}

void pickupItem(Entity *other)
{
	if (other == NULL)
	{
		return;
	}
	
	if (game->cutsceneType != CUTSCENE_NONE)
	{
		return;
	}
	
	if ((self->custom > 0) || (other->health <= 0) || (other->entityType != ET_BLOB))
	{
		return;
	}
	
	int result = pickUpItem((Unit*)other, self, true);
	
	if (result != ITEM_LEAVE)
	{
		if (self->name != "")
		{
			if (self->definition->type < ITM_CUSTOM_FIRST)
			{
				fireTriggers(self->getName(), TRIGGER_TYPE_ITEM_PICKED_UP);
			}
			else
			{
				fireTriggers(self->definition->getName(), TRIGGER_TYPE_ITEM_PICKED_UP);
			}
		}
		
		debug(("%s picked up %s\n", other->getName(), self->definition->getName()));
		
		// picked up an item that can be removed (eg - cherry or ammo)
		if (result == ITEM_CONSUME)
		{
			self->health = 0;
			self->touch = 0;
			
			if (other != player)
			{
				debug(("%s resuming normal function\n", other->getName()));
				
				other->action = other->walk;
				other->custom = 0;
				other->velocity.x = other->velocity.y = 0;
				other->thinkTime = 0;
			}
		}
	}
}

void addCustomItem(Properties *props)
{
	debug(("addCustomItem()\n"));
	
	if (!props->hasProperty("name"))
	{
		printf("ERROR: Custom items MUST have a name!\n");
		exit(1);
	}
	
	ItemDef *def = NULL;
	
	int i = ITM_CUSTOM_FIRST;
	
	while (def == NULL)
	{
		if (entityManager->itemDef[i].name == "")
		{
			def = &entityManager->itemDef[i];
			break;
		}
		
		i++;
		
		if (i > ITM_CUSTOM_LAST)
		{
			printf("ERROR: No more free custom item slots (Maximum of %d)\n", (ITM_CUSTOM_LAST + 1) - ITM_CUSTOM_FIRST);
			exit(1);
		}
	}
	
	def->type = i;
	def->itemPower = 1;
	def->itemType = IT_SPECIAL;
	
	def->name = props->getString("name", "???");
			
	def->modelName[0] = props->getString("model1", "NULL");
	def->modelName[1] = props->getString("model2", "NULL");
	def->textureName[0] = props->getString("texture1", "NULL");
	def->textureName[1] = props->getString("texture2", "NULL");
		
	def->bbMins = props->getVector("bbMins");
	def->bbMaxs = props->getVector("bbMaxs");
	
	def->customCarriable = props->getInt("customCarriable", 0);
	
	debug(("Added custom item '%s'\n", def->name.getText()));
}

Item *addStickyC4(Vector position)
{
	Item *item = entityManager->spawnItem(ITM_STICKY_C4);
	
	item->position = position;
	item->custom = 20;
	item->thinkTime = 25;
	item->draw = drawItem;
	item->action = &stickyC4Countdown;
	item->flags = EF_WEIGHTLESS+EF_NOCLIP;
	
	return item;
}

Item *addItem(Properties *props)
{
	#if !DEV
	if (props->hasProperty("test"))
	{
		return NULL;
	}
	#endif
	
	int type = entityManager->getEntityType(props);
	
	if (type == -1)
	{
		printf("ERROR: addItem() - Type not known!!\n");
		exit(1);
	}
	
	Item *item = entityManager->spawnItem(type);
	item->draw = drawItem;
	item->load(props);
	
	if (!game->allStatic)
	{
		item->touch = &pickupItem;
	}
	
	if (item->definition->type == ITM_STICKY_C4)
	{
		item->touch = NULL;
		item->action = &stickyC4Countdown;
		item->custom = 5;
	}
	
	debug(("addItem() - %s [%d, %s]\n", item->definition->getName(), item->getUniqueId(), item->position.toString()));
	
	return item;
}

Weapon *addWeapon(Properties *props)
{
	#if !DEV
	if (props->hasProperty("test"))
	{
		return NULL;
	}
	#endif
	
	int type = entityManager->getEntityType(props);
	
	if (type == -1)
	{
		printf("ERROR: addWeapon() - Type not known!!\n");
		exit(1);
	}
	
	Weapon *weapon = entityManager->spawnWeapon(type, false);
	weapon->draw = drawItem;
	weapon->load(props);
	
	if (!game->allStatic)
	{
		weapon->touch = &pickupItem;
	}
	
	debug(("addWeapon() - %s [%d, %s]\n", weapon->definition->getName(), weapon->getUniqueId(), weapon->position.toString()));
	
	return weapon;
}

void addHelperItems(int numberOfWeapons, WeaponDef *weaponDef, Vector position)
{
	static Properties weaponProps, itemProps;
	Weapon *weapon;
	float clipSize;
	int minAmmo, maxAmmo;
	
	weaponProps.clear();
	itemProps.clear();
	
	weaponProps.setProperty("position", position);
	itemProps.setProperty("position", position);
	
	if (weaponDef != NULL)
	{
		for (int i = 0 ; i < numberOfWeapons ; i++)
		{
			weaponProps.setProperty("classname", weaponDef->type);
			weapon = addWeapon(&weaponProps);
				
			clipSize = weaponDef->clipSize;
			clipSize *= (mission->enemyLevel * 0.65);
				
			minAmmo = (int)max(1, (clipSize * 0.10));
			maxAmmo = (int)max(2, (clipSize * 0.15));
				
			weapon->currentAmmo = Math::rrand(minAmmo, maxAmmo);
		
			throwSpawnedItem(weapon);
		}
	}
		
	if ((rand() % 8) == 0)
	{
		weaponProps.setProperty("classname", "WEP_GRENADE");
			
		weapon = addWeapon(&weaponProps);
		weapon->currentAmmo = 1;
			
		throwSpawnedItem(weapon);
	}
	
	int r = rand() % getAdjustedHealthChance(5, 1);
	
	debug(("Cherry chance: %d\n", r));

	if (r == 0)
	{
		itemProps.setProperty("classname", "ITM_CHERRY");
		
		Item *item = addItem(&itemProps);
		item->itemPower = Math::rrand(1, item->itemPower);

		throwSpawnedItem(item);
	}
	
	r = rand() % getAdjustedHealthChance(25, 2);
	
	debug(("Medipack chance: %d\n", r));
	
	if (r == 0)
	{
		itemProps.setProperty("classname", "ITM_MEDIPACK");
		
		Item *item = addItem(&itemProps);
		item->itemPower = Math::rrand(10, item->itemPower);

		throwSpawnedItem(item);
	}
	
	if (game->dropGrappleHooks)
	{
		int slot = game->getItemSlot(_("Grappling Hook"));
		int currentNumber = 0;
		
		if (slot != -1)
		{
			currentNumber = game->item[slot]->itemPower;
		}
		
		if (currentNumber != 5)
		{
			r = rand() % (2 + (2 * currentNumber));
		
			debug(("Grappling Hook chance: %d\n", r));
			
			if (r == 0)
			{
				itemProps.setProperty("classname", "ITM_GRAPPLE_HOOK");
			
				Item *item = addItem(&itemProps);
				item->itemPower = 1;
				item->position.z += 5;
	
				throwSpawnedItem(item);
			}
		}
		else
		{
			debug(("Not dropping grappling hooks - Player already has 5\n"));
		}
	}
	else
	{
		debug(("Not dropping grappling hooks\n"));
	}
}

void bringItemToPlayer(Entity *item)
{
	if ((player->health <= 0) || (player->helpless))
	{
		return;
	}
	
	/*
	 * First check that we're allowed to pick up this item.
	 * If not then just leave it there.
	*/
	if (!(item->flags & EF_AUTO_PICKUP))
	{
		if (pickUpItem(player, item, false) == ITEM_LEAVE)
		{
			return;
		}
	}
	
	item->velocity = player->position;
	item->velocity -= item->position;
	item->velocity.normalize();
	item->velocity = item->velocity * 2;
	item->thinkTime = Math::rrand(1, 3);
}

void processItemList(List *list)
{
	for (Entity *item = (Entity*)list->getFirstElement() ; item != NULL ; item = (Entity*)item->next)
	{
		self = item;
		
		//self->riding = NULL;
		
		bbManager->removeBox(item);
		
		if (item->owner != NULL)
		{
			if ((item->owner->entityType == ET_SWITCH) && (withinViewableRange(item->owner)))
			{
				entityManager->addEntityToDrawList(item);
			}
		
			continue;
		}
		
		if (mission->itemLimit > 0)
		{
			if (list->getSize() > mission->itemLimit)
			{
				item->health = 0;
			}
		}
		
		if (item->health <= 0)
		{
			debug(("Removing Item '%s'\n", item->definition->getName()));
			item = (Entity*)item->previous;
			list->remove(item->next);
			continue;
		}
		
		if (item->definition->type != ITM_STICKY_C4)
		{
			Math::limit(&(--item->custom), 0, RAND_MAX);
		}
		
		Math::limit(&(item->thinkTime -= engine->getTimeDifference(TD_LOGIC)), 0, RAND_MAX);
		
		if (item->thinkTime == 0)
		{
			if (item->action != NULL)
			{
				item->action();
			}
		}
		
		if (withinViewableRange(item))
		{
			entityManager->addEntityToDrawList(item);
		}
		
		item->applyGravity(engine->getTimeDifference(TD_LOGIC));
		
		moveEntity(item);
		
		if (item->flags & EF_ONGROUND)
		{
			item->flags &= ~EF_BOUNCES;
			
			/*
			if (!(item->flags & EF_BOUNCES))
			{
				item->velocity.z = 0;
			}
			else if ((item->riding == NULL) && (fabs(item->velocity.z) < EPSILON))
			{
				item->flags &= ~EF_BOUNCES;
			}
			*/
		}
		
		for (int i = 0 ; i < engine->getInertiaRate() ; i++)
		{
			item->velocity.x *= 0.95;
			item->velocity.y *= 0.95;
		}
		
		if (item->contentType & CONTENTS_WATER)
		{
			item->flags &= ~EF_BOUNCES;
			Math::limit(&item->velocity.z, -0.25, 0.25);
		}
		else if ((item->contentType & CONTENTS_LAVA) || (item->contentType & CONTENTS_SLIME))
		{
			item->flags |= EF_AUTO_PICKUP;
			item->flags |= EF_WEIGHTLESS;
			item->flags |= EF_NOCLIP;
			item->thinkTime = 0;
		}
		
		if ((!game->allStatic) && (game->cutsceneType == CUTSCENE_NONE))
		{
			if ((item->flags & EF_AUTO_PICKUP) && (item->thinkTime <= 0) && (game->missionStatus == MISSION_INPROGRESS))
			{
				bringItemToPlayer(item);
			}
			
			if (Math::getDistance(player->position, item->position) < 25)
			{
				bringItemToPlayer(item);
			}
		}
		
		if ((item->position.z < bsp->minCords.z) && (item->health > 0))
		{
			printf("WARNING: Item '%s' fell out of map at %s\n", item->getName(), item->position.toString());
			printf("Map Mins: %s\n", bsp->minCords.toString());
			printf("Map Maxs: %s\n", bsp->maxCords.toString());
			printf("On Ground = %ld\n", (item->flags & EF_ONGROUND));
			printf("Velocity = %s\n", item->velocity.toString());
			item->health = -100;
			item->flags |= EF_DEAD;
			continue;
			#if DEV
			exit(1);
			#endif
		}
		
		bbManager->addBox(item);
	}
}

void doItems()
{
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	processItemList(&entityManager->weaponList);
	processItemList(&entityManager->itemList);
}
