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

Boss::Boss()
{
	entityType = ET_BOSS;
	
	reactToBullet = NULL;
	reactToDamage = NULL;
	bossEffect = NULL;
	
	item = NULL;
	target = NULL;
	
	reload = 0;
	
	shieldHit = 0;
	
	dead = false;
	
	mental = 100;
	
	showHealthData = true;
	
	helpless = 0;
	
	energyCharge = 0;
	
	numberOfBeams = 0;
	
	speed = 0;
}

Boss::~Boss()
{
}

void Boss::think(float td)
{
	Math::limit(&(reload -= td), 0, RAND_MAX);
	Math::limit(&(shield += (0.0025 * td)), -2.5, maxShield);
	Math::limit(&(mental += (td * 0.025)), 0, 100);
}

void Boss::applyDamage(float amount)
{
	if ((amount == 0) || (flags & EF_IMMORTAL))
	{
		return;
	}
	
	if (shield > 0)
	{
		shield -= amount;
		
		shieldHit = 1;
		
		if (shield > 0)
		{
			return;
		}
	}
	
	shieldHit = 0;
	
	health -= amount;
	
	if (health > maxHealth)
	{
		health = maxHealth;
	}
}

void Boss::loadAdditional(Properties *p)
{
	
}

void Boss::saveAdditional(Properties *p)
{
	p->name = autoSaveToken.getText();
}
