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

#include "doors.h"

void doorTouchedNormal(Entity *other);
void doorTouchedClosing(Entity *other);

void doorClose(Door *door)
{
	door->velocity = door->startPosition;
	door->velocity -= door->position;
	door->velocity.normalize();
	door->velocity = door->velocity * door->speed;
	door->destination = door->startPosition;
	door->state = DOOR_CLOSING;
	
	door->touch = &doorTouchedClosing;
	door->autoCloseTimer = 300;
	
	audio->playSound(SND_DOOR_MOVE_START, CH_EFFECT8, camera->getSoundDistance(door->position));
}

void doorOpen(Door *door)
{
	door->velocity = door->endPosition;
	door->velocity -= door->position;
	door->velocity.normalize();
	door->velocity = door->velocity * door->speed;
	door->destination = door->endPosition;
	door->state = DOOR_OPENING;
	
	door->touch = &doorTouchedNormal;
	
	audio->playSound(SND_DOOR_MOVE_START, CH_EFFECT8, camera->getSoundDistance(door->position));
}

void doorTouchedNormal(Entity *other)
{
	Door *door = (Door*)self;
	
	if (door->state == DOOR_LOCKED)
	{
		if (other != player)
		{
			return;
		}
		
		if (!game->cheatItems)
		{
			game->setGamePlayMessage(INFO_GENERAL, 200, _("Door is Locked"));
			return;
		}
		
		door->state = DOOR_CLOSED;
	}
	
	if ((door->state != DOOR_CLOSED) && (door->state != DOOR_CLOSING))
	{
		return;
	}
	
	switch (other->entityType)
	{
		case ET_BLOB:
		case ET_BIOMECH:
			doorOpen(door);
			break;
			
		default:
			break;
	}
}

void doorTouchedClosing(Entity *other)
{
	switch (other->entityType)
	{
		case ET_BLOB:
		case ET_BIOMECH:
		case ET_CAMERA:
			break;
			
		default:
			return;
			break;
	}
	
	Door *door = (Door*)self;
	
	doorOpen(door);
	door->autoClose = true;
	return;
}

void doorThink()
{
	Door *door = (Door*)self;
	
	if ((door->state == DOOR_OPEN) && (door->autoClose))
	{
		door->autoCloseTimer -= engine->getTimeDifference(TD_LOGIC);
		
		if (door->autoCloseTimer <= 0)
		{			
			doorClose(door);
			return;
		}
	}
	
	if ((door->state != DOOR_OPENING) && (door->state != DOOR_CLOSING))
	{
		return;
	}
	
	if (Math::getDistance(door->position, door->destination) < engine->getTimeDifference(TD_LOGIC) * door->speed)
	{
		door->position = door->destination;
		door->velocity.set(0, 0, 0);
		
		if (door->position == door->endPosition)
		{
			door->state = DOOR_OPEN;
		}
		else
		{
			door->touch = &doorTouchedNormal;
			
			door->state = DOOR_CLOSED;
			
			if (door->initiallyLocked)
			{
				door->state = DOOR_LOCKED;
			}
		}
		
		audio->haltChannel(CH_EFFECT8);
		audio->playSound(SND_DOOR_MOVE_END, CH_EFFECT8, camera->getSoundDistance(door->position));
	}
}

void addDoor(Properties *props)
{
	int modelIndex;
	
	sscanf(props->getString("model", "*0"), "%*c%d", &modelIndex);
	
	MDLModel *mdlModel = modelManager->getMDLModel(props->getString("model", "NULL"));
	BSPModel *bspModel = &bsp->models[modelIndex];
	
	if ((mdlModel == NULL) || (bspModel == NULL) || (modelIndex == 0))
	{
		graphics->showErrorAndExit("createBaseStructure() - Bad BSP Model '%s'", props->getString("model", "NULL"));
	}
	
	Door *door = entityManager->spawnDoor();
	
	door->state = engine->getValueOfDefine(props->getString("state", "DOOR_CLOSED"));
	
	door->model = mdlModel;
	
	Vector v;
	
	v.x = ((mdlModel->maxs.x) - (mdlModel->mins.x)) / 2;
	v.y = ((mdlModel->maxs.y) - (mdlModel->mins.y)) / 2;
	v.z = ((mdlModel->maxs.z) - (mdlModel->mins.z)) / 2;
	
	door->boundingBox.mins -= v;
	door->boundingBox.maxs += v;
	
	door->position = mdlModel->originalPosition;
	door->startPosition = door->position;
	
	// default opening position
	door->endPosition = door->startPosition;
	door->endPosition.z += (v.z * 1.75);
	
	door->action = &doorThink;
	door->touch = &doorTouchedNormal;
	
	door->draw = drawStructure;
	
	door->load(props);
	
	if (door->initiallyLocked)
	{
		door->state = DOOR_LOCKED;
	}
	
	getCluster(door);
}

void doDoor()
{
	if (withinViewableRange(self))
	{
		entityManager->addEntityToDrawList(self);
	}
		
	self->prepareToMove(engine->getTimeDifference(TD_LOGIC));
	
	self->move(engine->getTimeDifference(TD_LOGIC));
	
	self->action();
}
