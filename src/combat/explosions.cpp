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

#include "explosions.h"

void explosionHarmUnit(Unit *unit, int radius, int distance, Vector position, Entity *owner)
{
	Vector push;

	float damage = radius;
	
	if (distance > (damage / 10))
	{
		for (int i = radius ; i < distance ; i++)
		{
			damage *= 0.95;
		}
	}
	
	if (damage < 1)
	{
		return;
	}
	
	float power = (damage * 3) / 10;
	
	Math::limit(&power, 0.5, 3.0);
	
	bool wasAlive = (unit->health > 0);
	
	debug(("%s took %.2f damage from explosion\n", unit->getName(), damage));
	unit->applyDamage(damage, DAMAGE_EXPLOSION);
	
	if (unit == player)
	{
		unit->action = NULL;
		game->alterStat(_("Total Damage Taken"), damage);
	}
	
	if (unit->flags & EF_STATIC)
	{
		return;
	}
	
	if ((unit == player) && (game->cheatStun))
	{
		return;
	}
	
	if (owner != NULL)
	{
		if ((wasAlive) && (unit->health <= 0) && (unit->entityType == ET_BIOMECH))
		{
			switch (owner->entityType)
			{
				case ET_BLOB:
					if (owner == player)
					{
						game->alterStat(_("Enemies Killed"), 1);
					}
					else
					{
						game->alterStat(_("Enemies Killed By Teeka"), 1);
					}
					break;
				
				case ET_BIOMECH:
					game->alterStat(_("Enemies Killed By Friendly Fire"), 1);
					break;
			}
		}
	}
		
	push = unit->position;
	push -= position;
	push.normalize();
	
	push.x *= (damage / 10);
	push.y *= (damage / 10);
	push.z *= (damage / 10);
	
	unit->velocity = push;
	
	throwUnit(unit, push);
	unit->helpless = 100 + (power * 10);
	unit->flags |= EF_BOUNCES;
	
	/*
	Math::limit(&unit->velocity.x, -2, 2);
	Math::limit(&unit->velocity.y, -2, 2);
	Math::limit(&unit->velocity.z, -2, 2);
	*/
}

void doExplosion(Vector position, int size, Entity *owner)
{
	audio->playSound(SND_GRENADE_EXPLODE, CH_EXPLODE, camera->getSoundDistance(position));
	
	addExplosionParticles(size, position);
	addExplosionMark(position, size * 2);
	
	if (game->cutsceneType != CUTSCENE_NONE)
	{
		return;
	}
	
	float distance;
	
	for (Unit *unit = (Unit*)entityManager->enemyList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (!unit->canBeDamaged())
		{
			continue;
		}
		
		distance = Math::getDistance(unit->position, position);
		
		if (distance > (size * 4))
		{
			continue;
		}
		
		explosionHarmUnit(unit, size, (int)distance, position, owner);
	}
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if (!unit->canBeDamaged())
		{
			continue;
		}
		
		distance = Math::getDistance(unit->position, position);
		
		if (distance > (size * 4))
		{
			continue;
		}
		
		explosionHarmUnit(unit, size, (int)distance, position, owner);
	}
	
	float damage;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (!(entity->flags & EF_IMMORTAL))
		{
			distance = Math::getDistance(entity->position, position);
			
			damage = (size * 4);
			
			if (distance > (size * 4))
			{
				continue;
			}
			
			if (distance > (damage / 10))
			{
				for (int i = (size * 4) ; i < distance ; i++)
				{
					damage *= 0.95;
				}
			}
			
			if (damage < 1)
			{
				continue;
			}
			
			entity->health -= damage;
			
			debug(("Structure (%s) took %.2f damage from explosion\n", (entity->name != "" ? entity->getName() : "unnamed"), damage));
		}
	}
	
	Entity *oldSelf;
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		if (boss == owner)
		{
			continue;
		}
		
		if (boss->flags & (EF_IMMORTAL|EF_VANISHED))
		{
			continue;
		}
		
		distance = Math::getDistance(boss->position, position);
		
		damage = (size * 4);
		
		if (distance > (size * 4))
		{
			continue;
		}
		
		if (distance > (damage / 10))
		{
			for (int i = (size * 4) ; i < distance ; i++)
			{
				damage *= 0.95;
			}
		}
		
		if (damage < 1)
		{
			continue;
		}
		
		oldSelf = self;
		self = boss;
		boss->reactToDamage(owner, damage);
		self = oldSelf;
		
		debug(("%s took %.2f damage from explosion\n", boss->getName(), damage));
	}
}
