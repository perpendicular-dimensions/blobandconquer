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

#include "emitters.h"

void addEmitter(Properties *props)
{
	int emitterType = entityManager->getEntityType(props);
	
	Emitter *emitter = entityManager->spawnEmitter(emitterType);
	
	emitter->load(props);
	
	getCluster(emitter);
	
	debug(("addEmitter() - Added Emitter [%d, %s]\n", emitter->type, emitter->position.toString()));
}

void addDebrisEmitter(Vector position)
{
	Emitter *emitter = entityManager->spawnEmitter(EMITTER_FIRE);
	
	emitter->position = position;
	emitter->flags |= EF_DYING;
	emitter->health = Math::rrand(1000, 2000);
	
	getCluster(emitter);
	
	debug(("addEmitter() - Added Emitter [%d, %s]\n", emitter->type, emitter->position.toString()));
}

void enableEmitter(const char *name, bool active)
{
	for (Emitter *emitter = (Emitter*)entityManager->emitterList.getFirstElement() ; emitter != NULL ; emitter = (Emitter*)emitter->next)
	{
		if (emitter->name != name)
		{
			continue;
		}
		
		emitter->active = active;
	}
}

void toggleEmitter(const char *name)
{
	for (Emitter *emitter = (Emitter*)entityManager->emitterList.getFirstElement() ; emitter != NULL ; emitter = (Emitter*)emitter->next)
	{
		if (emitter->name != name)
		{
			continue;
		}
		
		emitter->active = !emitter->active;
	}
}

void emitColor(Emitter *emitter)
{
	Particle *particle = particleManager->spawnParticle();
	
	particle->position = emitter->position;
	particle->position.x += Math::rrand(-3, 3);
	particle->position.y += Math::rrand(-3, 3);
	particle->position.z += Math::rrand(0, 3);
	
	particle->life = Math::rrand(25, 100);
	particle->size = Math::rrand(5, 10);
	
	float speed = Math::rrand(1, 5);
	speed /= 10;
	
	particle->velocity.set(0, 0, speed);
	
	float r = emitter->color.color[0];
	float g = emitter->color.color[1];
	float b = emitter->color.color[2];
	
	switch (rand() % 3)
	{
		case 0:
			particle->color.set(r, g, b, 1.0);
			break;
		
		case 1:
			particle->color.set(r * 2, g * 2, b * 2, 1.0);
			break;
		
		case 2:
			particle->color.set(r * 4, g * 4, b * 4, 1.0);
			break;
	}
}

void emitFire(Emitter *emitter)
{
	Particle *particle = particleManager->spawnParticle();
	
	particle->position = emitter->position;
	particle->position.x += Math::rrand(-3, 3);
	particle->position.y += Math::rrand(-3, 3);
	particle->position.z += Math::rrand(0, 3);
	
	particle->life = Math::rrand(25, 100);
	particle->size = Math::rrand(5, 10);
	
	float speed = Math::rrand(1, 5);
	speed /= 10;
	
	particle->velocity.set(0, 0, speed);
	
	switch (rand() % 3)
	{
		case 0:
			particle->color = GLColor::red;
			break;
		
		case 1:
			particle->color = GLColor::yellow;
			break;
		
		case 2:
			particle->color.set(1.0, 0.5, 0.0, 1.0);
			break;
	}
}

void emitSmoke(Emitter *emitter)
{
	Particle *particle = particleManager->spawnParticle();
	
	particle->position = emitter->position;
	particle->position.x += Math::rrand(-5, 5);
	particle->position.y += Math::rrand(-5, 5);
	particle->position.z += Math::rrand(0, 1);
	
	particle->life = Math::rrand(10, 50);
	particle->size = Math::rrand(2, 5);
	
	float speed = Math::rrand(1, 2);
	speed /= 5;
	
	particle->velocity.set(0, 0, speed);
	
	particle->color = GLColor::lightGrey;
}

void doEmitters()
{
	float distance;
	
	for (Emitter *emitter = (Emitter*)entityManager->emitterList.getFirstElement() ; emitter != NULL ; emitter = (Emitter*)emitter->next)
	{
		if (!emitter->active)
		{
			continue;
		}
		
		emitter->thinkTime -= engine->getTimeDifference(TD_LOGIC);
		
		if (emitter->flags & EF_DYING)
		{
			emitter->health -= engine->getTimeDifference(TD_LOGIC);
			
			// assuming that this is a fire emitter that's dying...
			if (emitter->health <= 500)
			{
				emitter->type = EMITTER_SMOKE;
			}
			
			if (emitter->health <= 0)
			{
				emitter = (Emitter*)emitter->previous;
				entityManager->emitterList.remove(emitter->next);
				continue;
			}
		}
		
		if (emitter->thinkTime <= 0)
		{
			if (!camera->frustum.containsEntity(emitter))
			{
				continue;
			}
			
			distance = Math::getDistance(camera->getRenderingTarget(), emitter->position);
			
			if (distance < 2000)
			{
				emitter->thinkTime = Math::rrand(1, 2);
			}
			else if (distance < 5000)
			{
				emitter->thinkTime = Math::rrand(5, 10);
			}
			else if (distance < 10000)
			{
				emitter->thinkTime = Math::rrand(10, 25);
			}
			else
			{
				emitter->thinkTime = Math::rrand(25, 50);
			}
			
			switch (emitter->type)
			{
				case EMITTER_FIRE:
					emitFire(emitter);
					break;
					
				case EMITTER_SMOKE:
					emitSmoke(emitter);
					break;
					
				case EMITTER_COLOR:
					emitColor(emitter);
					break;
			}
		}
	}
}
