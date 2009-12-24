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

Unit::Unit(long uniqueId)
{	
	this->uniqueId = uniqueId;
	
	contentType = CONTENTS_SOLID;
	
	specialAction = UNIT_SPECIAL_NONE;
	
	liquidLevel = 0;
	
	dead = false;
	
	showHealthData = false;
	
	completedOrders = true;

	shield = maxShield = 0;
	
	shieldHit = 0;

	flags = 0;
	
	helpless = 0;
	
	reload = 0;
	
	mental = MAX_MENTAL;
	
	oxygen = MAX_OXYGEN_IND;
	
	lastDamageTimer = 0;
	
	totalCurrentDamage = 0;
	
	currentWeapon = NULL;
	currentItem = NULL;
	target = NULL;
	switchTarget = NULL;
	
	spawnedIn = false;
	
	currentCrumb = NULL;
	crumbChaseTime = 0;
	crumbLost = false;
	
	lastPlayerSighting = 0;
	
	boundingBox.mins.set(-3, -3, -3);
	boundingBox.maxs.set(3, 3, 3);
}

Unit::~Unit()
{
}

void Unit::think(float td)
{
	if (lastDamageTimer == 0)
	{
		totalCurrentDamage = 0;
	}
	
	// don't allow the units to move too quickly
	if (liquidLevel <= 1)
	{
		Math::limit(&velocity.x, -6, 6);
		Math::limit(&velocity.y, -6, 6);
		Math::limit(&velocity.z, -11, 11);
		
		oxygen += (td * 2);
	}
	else
	{
		Math::limit(&velocity.x, -0.6, 0.6);
		Math::limit(&velocity.y, -0.6, 0.6);
		Math::limit(&velocity.z, -0.6, 0.6);
		
		oxygen -= td;
		
		flags &= ~EF_ONFIRE;
	}
	
	if (flags & EF_SWIMS)
	{
		oxygen = MAX_OXYGEN_IND;
	}
	
	Math::limit(&(helpless -= td), 0, RAND_MAX);
	Math::limit(&(reload -= td), 0, RAND_MAX);
	Math::limit(&(shield += (0.01 * td)), -2.5, maxShield);
	Math::limit(&(lastDamageTimer -= td), 0, RAND_MAX);
	Math::limit(&(lastPlayerSighting += td), 0, RAND_MAX);
	Math::limit(&oxygen, 0, MAX_OXYGEN_IND);
	
	if (entityType == ET_BLOB)
	{
		// Approximately 5 seconds to recover...
		Math::limit(&(mental += (0.85 * td)), 0, MAX_MENTAL);
	}
	else if (definition->type != BLOB_BOB)
	{
		// Approximately 30 seconds to recover...
		Math::limit(&(mental += (0.1 * td)), 0, MAX_MENTAL);
	}
	
	if (!(flags & (EF_DYING|EF_DEAD)))
	{
		if (oxygen == 0)
		{
			applyDamage(td / 25, DAMAGE_SPECIAL);
		}
	}
	
	if (flags & EF_ONFIRE)
	{
		if (entityType == ET_BLOB)
		{
			health -= 0.01 * td;
		}
		else
		{
			health -= 0.005 * td;
		}
	}
}

bool Unit::canBeDamaged()
{
	return ((!(flags & EF_DEAD)) && (!(flags & EF_TELEPORTING)) && (!(flags & EF_VANISHED)));
}

void Unit::applyDamage(float amount, int damageType)
{
	if ((amount == 0) || (flags & EF_IMMORTAL))
	{
		return;
	}
	
	if (flags & EF_FROZEN)
	{
		mental = MAX_MENTAL;
	}
	
	if ((shield > 0) && (damageType != DAMAGE_SPECIAL))
	{
		if (damageType == DAMAGE_EXPLOSION)
		{
			shield -= (amount * 3);
		}
		else
		{
			shield -= amount;
		}
		
		shieldHit = 1;
		
		if (shield > 0)
		{
			return;
		}
	}
	
	// shield cannot recover if the unit keeps taking damage
	if ((shield <= 0) && (maxShield > 0))
	{
		shield = -2.5;
		
		if (definition->type == BLOB_BOB)
		{
			if ((rand() % (int)(maxShield * 1.5)) == 0)
			{
				shield = maxShield = 0;
			}
		}
	}
	
	shieldHit = 0;
	
	if (flags & EF_STATIC)
	{
		health -= amount;
		return;
	}
	
	lastDamageTimer = 150;
	
	if (flags & EF_WRAPPED)
	{
		lastDamageTimer = 0;
	}
	
	if (!(flags & EF_FROZEN))
	{
		totalCurrentDamage += (int)amount;
	}
	
	if (flags & EF_CANNOT_BE_KILLED)
	{
		Math::limit(&(health -= amount), 1, RAND_MAX);
	}
	else
	{
		Math::limit(&(health -= amount), 0, RAND_MAX);
	}
	
	flags &= ~EF_DYING;
	flags &= ~EF_DEAD;
}

void Unit::loadAdditional(Properties *props)
{
	dead = props->getInt("dead", dead);
	shield = props->getFloat("shield", shield);
	maxShield = props->getFloat("maxShield", maxShield);
	helpless = props->getFloat("helpless", helpless);
	shieldHit = props->getFloat("shieldHit", shieldHit);
	reload = props->getFloat("reload", reload);
	lastDamageTimer = props->getFloat("lastDamageTimer", lastDamageTimer);
	totalCurrentDamage = props->getInt("totalCurrentDamage", totalCurrentDamage);
	spawnedIn = props->getInt("spawnedIn", spawnedIn);
	lastPlayerSighting = props->getFloat("lastPlayerSighting", lastPlayerSighting);
	completedOrders = props->getInt("completedOrders", completedOrders);
	oxygen = props->getFloat("oxygen", oxygen);
	mental = props->getFloat("mental", mental);
	
	if (props->getInt("training", 0))
	{
		flags |= EF_NOTARGET;
	}
	
	if (props->getInt("blind", 0))
	{
		flags |= EF_BLIND;
	}
}

void Unit::saveAdditional(Properties *props)
{
	props->setProperty("dead", dead);
	props->setProperty("shield", shield);
	props->setProperty("maxShield", maxShield);
	props->setProperty("helpless", helpless);
	props->setProperty("shieldHit", shieldHit);
	props->setProperty("reload", reload);
	props->setProperty("lastDamageTimer", lastDamageTimer);
	props->setProperty("totalCurrentDamage", totalCurrentDamage);
	
	props->setProperty("spawnedIn", spawnedIn);
	props->setProperty("lastPlayerSighting", lastPlayerSighting);
	props->setProperty("completedOrders", completedOrders);
	
	props->setProperty("oxygen", oxygen);
	props->setProperty("mental", mental);
	
	if (currentItem != NULL)
	{
		props->setProperty("currentItem", currentItem->getUniqueId());
	}
}
