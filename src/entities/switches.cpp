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

#include "switches.h"

void switchLookForPlayer();
void switchDoNothing();

void drawSwitch(bool transparent)
{
	Vector position;
	
	if (transparent)
	{
		return;
	}
	
	Switch *swt = (Switch*)self;
	
	if (swt->texture[0] == NULL)
	{
		switch (swt->switchType)
		{
			case SWITCH_ENGINEER:
			case SWITCH_HACKER:
				swt->model = modelManager->getSimpleModel("gfx/rw2/hackerswitch.blend.raw");
				swt->texture[0] = textureManager->getTexture("textures/game/hackerSwitch1.png");
				swt->texture[1] = textureManager->getTexture("textures/game/hackerSwitch2.png");
				break;
				
			case SWT_PRESSURE:
				swt->texture[0] = textureManager->getTexture("textures/game/pressurePlate1.png");
				swt->texture[1] = textureManager->getTexture("textures/game/pressurePlate2.png");
				break;
				
			case SWITCH_DEMOLITION:
				swt->texture[0] = textureManager->getTexture("textures/game/demolitionPlate01.png");
				swt->texture[1] = textureManager->getTexture("textures/game/demolitionPlate10.png");
				break;
			
			default:
				if (!swt->altModel)
				{
					swt->model = modelManager->getSimpleModel("gfx/rw2/switch.blend.raw");
					swt->texture[0] = textureManager->getTexture("textures/game/switch1.png");
					swt->texture[1] = textureManager->getTexture("textures/game/switch2.png");
				}
				else
				{
					swt->model = modelManager->getSimpleModel("gfx/primitive/altSwitch.primitive");
					swt->texture[0] = textureManager->getTexture("textures/game/altSwitch1.png");
					swt->texture[1] = textureManager->getTexture("textures/game/altSwitch2.png");
				}
				break;
		}
	}
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3f(1.0, 1.0, 1.0);
	
	if ((swt->switchType != SWT_PRESSURE) && (swt->switchType != SWITCH_DEMOLITION))
	{
		glPushMatrix();
		{
			glTranslatef(swt->position.x, swt->position.y, swt->position.z + 8);
			glRotatef(swt->rotation.z, 0.0, 0.0, 1.0);
			
			glBindTexture(GL_TEXTURE_2D, swt->texture[swt->active]->data);
			glCallList(swt->model->data);
		}
		glPopMatrix();
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, swt->texture[swt->active]->data);
		
		drawBox(swt->position, 32, 32, 2, 1);
	}
}

bool completesSwitchPuzzle(Switch *swt)
{
	bool result = false;
	
	switch (swt->puzzleType)
	{
		case PUZZLE_TYPE_MASTERMIND:
			result = doMasterMindPuzzle(swt->puzzleSeed);
			break;
			
		case PUZZLE_TYPE_LASERGRID:
			result = doLaserGridPuzzle(swt->puzzleSeed);
			break;
	}
	
	return result;
}

bool otherIndependentSwitchActive(Switch *swt)
{
	Switch *other;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if ((entity->entityType != ET_SWITCH) || (entity->name != swt->getName()) || (swt == entity))
		{
			continue;
		}
		
		other = (Switch*)entity;
		
		if (!other->independent)
		{
			continue;
		}
		
		if (other->active)
		{
			return true;
		}
	}
	
	return false;
}

void doSwitchCutscenes(Switch *swt)
{
	CutsceneCamera *camera = getCutsceneCamera(swt->getName());
			
	if (camera == NULL)
	{
		debug(("handleSwitchUse - %s - No cameras. Firing immediately.\n", swt->getName()));
		
		activateInGameCutscene(swt->getName(), 25);
		
		if (swt->switchType == SWT_TOGGLE)
		{
			handleStructureStateChange(swt->getName(), swt->active);
		}
		else
		{
			toggleStructureState(swt->getName());
		}
		
		toggleLaserTrap(swt->getName());
	}
	else
	{
		debug(("handleSwitchUse - %s - Has camera. Firing first.\n", swt->getName()));
		
		camera->deferredSwitch = swt;
		
		activateInGameCutscene(swt->getName(), 25);
	}
	
	fireTriggers(swt->getName(), TRIGGER_TYPE_ANY);
}

bool allSwitchesActive(const char *name, int *numberInGroup, int *numberInactive)
{
	Switch *swt = (Switch*)self;
	
	*numberInGroup = 0;
	*numberInactive = 0;
	
	if (strcmp(name, "") == 0)
	{
		printf("WARNING: allSwitchesActive() - Switches MUST have names!\n");
		return false;
	}
	
	bool rtn = true;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if ((entity->entityType != ET_SWITCH) || (entity->name != name))
		{
			continue;
		}
		
		swt = (Switch*)entity;
		
		if ((swt->switchType == SWT_TOGGLE) || (swt->switchType == SWT_RESET))
		{
			continue;
		}
		
		(*numberInGroup)++;
		
		if (!swt->active)
		{
			(*numberInactive)++;
			
			debug(("allSwitchesActive - '%s' - FALSE\n", swt->getName()));
			
			rtn = false;
		}
	}
	
	if (rtn)
	{
		debug(("allSwitchesActive - '%s' - TRUE (%d)\n", swt->getName(), *numberInGroup));
	}
	
	return rtn;
}

void pressurePlateTouch(Entity *other)
{
	if (other == NULL)
	{
		return;
	}
	
	self->riding = other;
	
	switch (other->entityType)
	{
		case ET_BULLET:
		case ET_ITEM:
		case ET_STRUCTURE:
			return;
			break;
			
		case ET_BIOMECH:
			if (!(other->flags & EF_FROZEN))
			{
				return;
			}
			break;
			
		case ET_PUSHBLOCK:
			break;
			
		default:
			break;
	}
	
	int numberInGroup = 0;
	int numberInactive = 0;
	
	Switch *swt = (Switch*)self;
	
	if (swt->switchType == SWT_PRESSURE)
	{
		if (!swt->active)
		{
			audio->playSound(SND_SWITCH_CLICK_3, CH_ANY, camera->getSoundDistance(swt->position));
			
			swt->active = true;
			
			if ((swt->requires == "") || ((swt->requires != "") && (swt->requires == other->getName())))
			{
				if (!swt->independent)
				{
					bool enable = allSwitchesActive(swt->getName(), &numberInGroup, &numberInactive);
					
					if (enable)
					{
						doSwitchCutscenes(swt);
					}
					else
					{
						handleStructureStateChange(swt->getName(), enable);
					}
				}
				else
				{
					if (!otherIndependentSwitchActive(swt))
					{
						doSwitchCutscenes(swt);
					}
					else
					{
						return;
					}
				}
			}
		}
		
		if (other->entityType == ET_PUSHBLOCK)
		{
			other->flags &= ~EF_PUSHABLE;
			//other->flags |= EF_WEIGHTLESS;
			swt->action = &switchDoNothing;
			other->position.x = self->position.x;
			other->position.y = self->position.y;
			other->name = "";
		}
		
		swt->thinkTime = 5 * engine->getTimeDifference(TD_LOGIC);
	}
	else if ((other == player) && (!swt->active))
	{
		game->setGamePlayMessage(INFO_GENERAL, _("Bombs can only be planted by a Blob Demolition expert"));
	}
}

void pressurePlateDeactivate()
{
	Switch *swt = (Switch*)self;
	
	if (swt->active)
	{
		swt->active = false;
		
		if (swt->independent)
		{
			if (otherIndependentSwitchActive(swt))
			{
				return;
			}
		}
		
		handleStructureStateChange(swt->getName(), false);
	}
}

void switchDoNothing()
{
	self->thinkTime = RAND_MAX;
}

void cancelSwitches(const char *name)
{
	Switch *swt;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if ((entity->entityType != ET_SWITCH) || (entity->name != name))
		{
			continue;
		}
		
		swt = (Switch*)entity;
		
		swt->switchType = SWT_STATIC;
		swt->action = switchDoNothing;
		
		debug(("cancelSwitches() - %s\n", name));
	}
}

void switchWait()
{
	self->action = &switchLookForPlayer;
	
	((Switch*)self)->active = false;
}

void switchPause()
{
	self->action = &switchLookForPlayer;
}

void switchDoTimer()
{
	debug(("switchDoTimer()\n"));
	
	Switch *swt = (Switch*)self;
	
	swt->active = false;
	
	int numberInGroup, unused;
	
	allSwitchesActive(swt->getName(), &numberInGroup, &unused);
	
	if (!swt->independent)
	{
		if (numberInGroup == 1)
		{
			doSwitchCutscenes(swt);
		}
	}
	else
	{
		if (!otherIndependentSwitchActive(swt))
		{
			doSwitchCutscenes(swt);
		}
	}
	
	self->action = &switchLookForPlayer;
}

void handleSwitchUse(Switch *swt)
{
	switch (swt->switchType)
	{
		case SWT_TOGGLE:
		case SWT_TIMED:
		case SWITCH_ENGINEER:
		case SWITCH_HACKER:
			break;
			
		default:
			if (swt->active)
			{
				return;
			}
			break;
	}
	
	swt->active = !swt->active;
	
	audio->playSound(SND_SWITCH_BLEEP, CH_ANY, camera->getSoundDistance(swt->position));
	
	bool fire = false;
	
	int numberInGroup = 0;
	int numberInactive = 0;
	
	if (swt->switchType == SWT_RESET)
	{
		resetStructures(swt->getName());
		swt->thinkTime = 100;
		swt->action = &switchWait;
		return;
	}
	
	if (!swt->independent)
	{
		switch (swt->switchType)
		{
			case SWITCH_ENGINEER:
			case SWITCH_HACKER:
			case SWITCH_DEMOLITION:
				fire = true;
				break;
				
			default:
				fire = allSwitchesActive(swt->getName(), &numberInGroup, &numberInactive);
				break;
		}
	}
	else
	{
		if (!otherIndependentSwitchActive(swt))
		{
			fire = true;
		}
		else
		{
			return;
		}
	}
	
	switch (swt->switchType)
	{
		case SWT_TOGGLE:
			swt->thinkTime = 50;
			swt->action = &switchPause;
			break;
			
		case SWITCH_ENGINEER:
		case SWITCH_HACKER:
			swt->switchType = SWT_TOGGLE;
			swt->thinkTime = 50;
			swt->action = &switchPause;
			break;
					
		case SWT_TIMED:
			swt->thinkTime = swt->custom;
			swt->action = &switchDoTimer;
			break;
			
		case SWITCH_DEMOLITION:
			swt->action = &switchDoNothing;
			break;
			
		default:
			swt->switchType = SWT_STATIC;
			swt->action = &switchDoNothing;
			break;
	}
	
	if (!fire)
	{
		game->setInfoMessage(INFO_HINT, _("%d more to go..."), numberInactive);
		return;
	}
	
	doSwitchCutscenes(swt);
	
	if ((swt->switchType == SWT_TIMED) && (numberInGroup == 1))
	{
		return;
	}
	
	switch (swt->switchType)
	{
		case SWT_NORMAL:
		case SWT_PRESSURE:
			cancelSwitches(swt->getName());
			break;
			
		case SWT_TIMED:
			if (!swt->independent)
			{
				cancelSwitches(swt->getName());
			}
			break;
			
		default:
			break;
	}
}

void switchLookForPlayer()
{
	BoundingBox box;
	
	if ((player->health < 0) || (!(player->flags & EF_ONGROUND)))
	{
		return;
	}
	
	Switch *swt = (Switch*)self;
	
	box = self->boundingBox;
	box.mins.x -= 6;
	box.mins.y -= 6;
	box.maxs.x += 6;
	box.maxs.y += 6;
	
	if (Collision::collision(player->position, player->boundingBox, self->position, box))
	{
		if (swt->switchType == SWITCH_ENGINEER)
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Switch can only be activated by a Blob Engineer"));
		}
		else if (swt->switchType == SWITCH_HACKER)
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Switch can only be activated by a Blob Computer Hacker"));
		}
		else
		{
			if (swt->switchType != SWT_RESET)
			{
				game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to activate switch"));
			}
			else
			{
				game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to reset puzzle blocks"));
			}
			
			if (game->isControl(CTRL_ACTION))
			{
				game->resetControl(CTRL_ACTION);
				
				if (swt->requires != "")
				{
					if ((!game->hasItem(swt->requires.getText())) && (!game->cheatItems))
					{
						game->setInfoMessage(INFO_HINT, _("%s required"), swt->requires.getText());
						return;
					}
					else
					{
						int i = game->getItemSlot(swt->requires.getText());
						game->setInfoMessage(INFO_HINT, _("Used %s"), swt->requires.getText());
					
						if (!swt->altModel)
						{
							game->removeItem(swt->requires.getText());
						}
						else
						{
							if (i != -1)
							{
								game->item[i]->owner = swt;
								game->item[i]->position = swt->position;
								game->item[i]->position.z += 9;
								game->item[i]->position.z += game->item[i]->boundingBox.maxs.z;
								game->item[i] = NULL;
								game->itemId[i] = 0;
							}
						}
						
						swt->requires = "";
						String s;
						s.setText("str_item_%d", i);
						textureManager->removeTexture(s.getText());
					}
				}
				
				handleSwitchUse(swt);
			}
		}
	}
}

void addSwitch(Properties *props)
{
	Switch *swt = entityManager->spawnSwitch();
	
	swt->load(props);
	
	swt->switchType = engine->getValueOfDefine(props->getString("switchType", "SWT_NORMAL"));
	
	switch (swt->switchType)
	{
		case SWT_NORMAL:
			if (swt->thinkTime == 0)
			{
				swt->action = &switchLookForPlayer;
			}
			else
			{
				swt->action = &switchWait;
			}
			break;
			
		case SWT_TIMED:
			if (swt->thinkTime == 0)
			{
				swt->action = &switchLookForPlayer;
			}
			else
			{
				swt->action = &switchDoTimer;
			}
			break;
			
		default:
			swt->action = &switchLookForPlayer;
			break;
	}
	
	swt->draw = drawSwitch;
	
	debug(("addSwitch - %s\n", swt->position.toString()));
}

void addSpecialSwitch(Properties *props)
{
	Switch *swt = entityManager->spawnSwitch();
	
	swt->load(props);
	
	swt->switchType = engine->getValueOfDefine(props->getName());
	
	swt->action = &switchLookForPlayer;
	
	swt->draw = drawSwitch;
	
	debug(("addSpecialSwitch - %s\n", swt->position.toString()));
}

void addPressurePlate(Properties *props)
{
	Switch *swt = entityManager->spawnSwitch();
	
	swt->name = "PressurePlate";
	
	swt->load(props);
	
	swt->switchType = SWT_PRESSURE;
	
	swt->touch = &pressurePlateTouch;
	swt->action = &pressurePlateDeactivate;
	
	swt->boundingBox.mins.set(-16, -16, -1);
	swt->boundingBox.maxs.set(16, 16, 1);
	
	swt->draw = drawSwitch;
}

void addDemolitionPlate(Properties *props)
{
	Switch *swt = entityManager->spawnSwitch();
	
	swt->name = "SpecialPressurePlate";
	
	swt->load(props);
	
	swt->switchType = SWITCH_DEMOLITION;
	
	swt->touch = &pressurePlateTouch;
	swt->action = &pressurePlateDeactivate;
	
	swt->boundingBox.mins.set(-16, -16, -1);
	swt->boundingBox.maxs.set(16, 16, 1);
	
	swt->draw = drawSwitch;
}

void doSwitch()
{
	bbManager->removeBox(self);
	
	if (self->cluster == -1)
	{
		// Mmmmmmmm... another lovely hack
		self->position.z++;
		getCluster(self);
		self->position.z--;
	}
	
	if (withinViewableRange(self))
	{
		entityManager->addEntityToDrawList(self);
	}
	
	if (self->thinkTime == 0)
	{
		self->action();
	}
	
	bbManager->addBox(self);
}
