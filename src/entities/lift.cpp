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

#include "lift.h"

void liftMainThink();
void liftWait();
void autoLiftChangePosition();
void liftMoveToNewPosition(Structure *structure);

void toggleLiftPosition(const char *name, bool active)
{
	Structure *structure;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if ((entity->entityType != ET_LIFT) && (entity->entityType != ET_AUTOLIFT))
		{
			continue;
		}
		
		if (entity->name != name)
		{
			continue;
		}
		
		structure = (Structure*)entity;
		
		if (entity->entityType == ET_LIFT)
		{
			liftMoveToNewPosition(structure);
		}
		else
		{
			structure->active = active;
		}
	}
}

void toggleLiftPosition(const char *name)
{
	Structure *structure;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if ((entity->entityType != ET_LIFT) && (entity->entityType != ET_AUTOLIFT))
		{
			continue;
		}
		
		if (entity->name != name)
		{
			continue;
		}
		
		structure = (Structure*)entity;
		
		if (entity->entityType == ET_LIFT)
		{
			liftMoveToNewPosition(structure);
		}
		else
		{
			structure->active = !structure->active;
		}
	}
}

void liftMoveToNewPosition(Structure *structure)
{
	if (structure->position == structure->endPosition)
	{
		moveStructureToStart(structure);
	}
	else if (structure->position == structure->startPosition)
	{
		moveStructureToEnd(structure);
	}
	
	structure->action = &liftMainThink;
	structure->thinkTime = 1;
}

void liftTouch(Entity *other)
{
	Vector wantedPosition;

	if (other == NULL)
	{
		return;
	}
	
	if (self->action == liftWait)
	{
		if (other == player)
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to use lift"));
			
			if (game->isControl(CTRL_ACTION))
			{
				liftMoveToNewPosition((Structure*)self);
			}
		}
	}
	else if (self->action != autoLiftChangePosition)
	{
		wantedPosition = other->position;
		wantedPosition.x += (self->velocity.x * engine->getTimeDifference(TD_LOGIC));
		wantedPosition.y += (self->velocity.y * engine->getTimeDifference(TD_LOGIC));
		
		if (self->velocity.z > 0)
		{
			wantedPosition.z += (self->velocity.z * engine->getTimeDifference(TD_LOGIC));
		}
		
		bsp->traceBox(wantedPosition, wantedPosition, other->boundingBox.mins, other->boundingBox.maxs);
		
		if (!tracer->outputAllSolid)
		{
			other->position = wantedPosition;
		}
	}
}

void autoLiftChangePosition()
{
	liftMoveToNewPosition((Structure*)self);
}

void liftWait()
{
	self->thinkTime = 1;
	self->touch = liftTouch;
}

void liftPause()
{
	self->thinkTime = self->custom;
	
	self->action = &autoLiftChangePosition;
}

void liftMainThink()
{
	if (Math::getDistance(self->position, self->destination) < engine->getTimeDifference(TD_LOGIC))
	{
		self->position = self->destination;
		self->velocity.set(0, 0, 0);
		
		if (self->entityType == ET_LIFT)
		{
			self->action = &liftWait;
		}
		else
		{
			self->action = &liftPause;
		}
	}
}
