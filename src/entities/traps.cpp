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

#include "traps.h"

void drawLaserTrapWarmUp(bool transparent)
{
	if (!transparent)
	{
		return;
	}
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
		
	glLineWidth(1.0);
	
	float fade = ((50 - self->custom) / 50);
	
	Math::limit(&fade, 0, 1);
	
	glPushMatrix();
	{
		glBegin(GL_LINES);
		{
			glColor4f(1.0, 0.0, 0.0, fade); glVertex3f(self->position.x, self->position.y, self->position.z);
			glColor4f(1.0, 0.0, 1.0, fade); glVertex3f(self->destination.x, self->destination.y, self->destination.z);
		}
		glEnd();
	}
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void drawLaserTrapCoolDown(bool transparent)
{
	if (!transparent)
	{
		return;
	}
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
		
	glLineWidth(1.0);
	
	float fade = self->custom / 50;
	
	Math::limit(&fade, 0, 1);
	
	glPushMatrix();
	{
		glBegin(GL_LINES);
		{
			glColor4f(1.0, 0.0, 0.0, fade); glVertex3f(self->position.x, self->position.y, self->position.z);
			glColor4f(1.0, 0.0, 1.0, fade); glVertex3f(self->destination.x, self->destination.y, self->destination.z);
		}
		glEnd();
	}
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void drawLaserTrap(bool transparent)
{
	if (!transparent)
	{
		return;
	}
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
		
	glLineWidth(1.0);
	
	glPushMatrix();
	{
		glBegin(GL_LINES);
		{
			glColor4f(1.0, 0.0, 0.0, 1.0); glVertex3f(self->position.x, self->position.y, self->position.z);
			glColor4f(1.0, 0.0, 1.0, 1.0); glVertex3f(self->destination.x, self->destination.y, self->destination.z);
		}
		glEnd();
	}
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void doLaserTrapOn();
void doLaserTrapOff();

void doLaserTrapWarmUp()
{
	Trap *trap = (Trap*)self;
	
	entityManager->addEntityToDrawList(trap);
	
	trap->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (trap->custom <= 0)
	{
		trap->action = &doLaserTrapOn;
		trap->draw = &drawLaserTrap;
		trap->custom = trap->onTime;
	}
}

void doLaserTrapCoolDown()
{
	Trap *trap = (Trap*)self;
	
	trap->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (trap->custom <= 0)
	{
		trap->action = &doLaserTrapOff;
		trap->draw = &drawLaserTrap;
		trap->custom = trap->offTime;
		return;
	}
	
	entityManager->addEntityToDrawList(trap);
}

void doLaserTrapOff()
{
	Trap *trap = (Trap*)self;
	
	if (!trap->active)
	{
		return;
	}
	
	trap->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (trap->custom <= 0)
	{
		trap->action = &doLaserTrapWarmUp;
		trap->custom = 100;
		trap->draw = drawLaserTrapWarmUp;
	}
}

void doLaserTrapOn()
{
	Vector velocity;
	
	Unit *unit;
	Entity *entity;

	Trap *trap = (Trap*)self;
	
	if (!trap->active)
	{
		return;
	}
	
	entityManager->addEntityToDrawList(trap);
	
	int c = bbManager->initCollisionCandidates(trap->position, trap->destination);
	
	bool deactivate = false;

	for (int i = 0 ; i < c ; i++)
	{
		entity = bbManager->candidate[i];
		
		if (bbManager->lineIntersectBox(entity, trap->position, trap->destination))
		{
			switch (entity->entityType)
			{
				case ET_BLOB:
				case ET_BIOMECH:
					unit = (Unit*)entity;
					unit->health -= 3;
					velocity.x = Math::rrand(-150, 150);
					velocity.y = Math::rrand(-150, 150);
					velocity.z = Math::rrand(0, 150);
					unit->velocity = velocity * 0.01;
					unit->helpless = 50;
					audio->playSound(SND_HIT, CH_ANY, camera->getSoundDistance(unit->position));
					deactivate = true;
					break;
					
				case ET_BULLET:
					entity->health = 0;
					deactivate = true;
					break;
					
				case ET_PUSHBLOCK:
					self->oldPosition = self->destination;
					
					break;
					
				default:
					break;
			}
			
			if (deactivate)
			{
				addSparkParticle(entity->position);
				audio->playSound(SND_ELECTRIC_SPARK3, CH_ANY, camera->getSoundDistance(entity->position));
				trap->custom = 0;
			}
		}
	}
	
	bbManager->clearCollisionCandidates();
	
	trap->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if (trap->custom <= 0)
	{
		if (trap->offTime > 0)
		{
			trap->action = &doLaserTrapCoolDown;
			trap->draw = &drawLaserTrapCoolDown;
			trap->custom = 100;
		}
		else
		{
			trap->custom = trap->onTime;
		}
	}
}

void addTrap(Properties *props)
{
	Trap *trap = entityManager->spawnTrap(TRAP_LASER);
	
	trap->load(props);
	
	trap->draw = &drawLaserTrap;
	trap->action = &doLaserTrapOn;
}

void toggleLaserTrap(const char *name)
{
	debug(("toggleLaserTrap - %s\n", name));
	
	for (Trap *trap = (Trap*)entityManager->trapList.getFirstElement() ; trap != NULL ; trap = (Trap*)trap->next)
	{
		if (trap->name == name)
		{
			trap->active = !trap->active;
			
			if (trap->active)
			{
				trap->action = &doLaserTrapWarmUp;
				trap->draw = drawLaserTrapWarmUp;
				trap->custom = 100;
			}
			else
			{
				trap->action = &doLaserTrapCoolDown;
				trap->draw = &drawLaserTrapCoolDown;
				trap->custom = 100;
			}
		}
	}
}

void doTraps()
{
	for (Trap *trap = (Trap*)entityManager->trapList.getFirstElement() ; trap != NULL ; trap = (Trap*)trap->next)
	{
		self = trap;
		
		trap->action();
	}
}
