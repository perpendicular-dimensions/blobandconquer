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

#include "combat.h"

Vector getLeadingTargetPosition(Entity *attacker, Entity *target)
{
	Vector targetSpeed, targetPosition;

	float distance = Math::getDistance(attacker->position, target->position);
			
	targetSpeed = (target->velocity * (distance / BULLET_MOVE_SPEED));
			
	targetPosition = target->position;
	targetPosition += targetSpeed;
	
	return targetPosition;
}

Vector getGunPosition(Boss *boss, int i)
{
	Vector gunPosition = boss->position;
	
	if ((boss->gunPosition[i].x == 0) && (boss->gunPosition[i].y == 0) && (boss->gunPosition[i].z == 0))
	{
		return gunPosition;
	}
	
	Vector rot = (boss->flags & EF_HAS_TURRET) ? boss->cannonRotation : boss->rotation;
	
	float r = toRadians(rot.x);
	Vector v = boss->position;
	v.x += (sin(r));
	v.y -= (cos(r));
	v -= boss->position;
	v.normalize();
	v = v * -boss->gunPosition[i].y;
	
	gunPosition += v;
	gunPosition.x += boss->gunPosition[i].x;
	gunPosition.z += boss->gunPosition[i].z;
	
	return gunPosition;
}

Vector getGunPosition(Unit *unit, int i)
{
	Vector gunPosition = unit->position;
	
	if ((unit->gunPosition[i].x == 0) && (unit->gunPosition[i].y == 0) && (unit->gunPosition[i].z == 0))
	{
		return gunPosition;
	}
	
	float r = toRadians(unit->rotation.x);
	Vector v = unit->position;
	v.x += (sin(r));
	v.y -= (cos(r));
	v -= unit->position;
	v.normalize();
	v = v * -unit->gunPosition[i].y;
	
	gunPosition += v;
	gunPosition.x += unit->gunPosition[i].x;
	gunPosition.z += unit->gunPosition[i].z;
	
	return gunPosition;
}

void attackTarget(Unit *unit)
{
	if (unit->reload > 0)
	{
		return;
	}
	
	fireWeapon(unit, unit->currentWeapon, getGunPosition(unit, 0), unit->target->position);
}

// Calculates where the unit will be at X time and fires at that position
void attackTargetLeading(Unit *unit)
{
	Vector target;

	if (unit->reload > 0)
	{
		return;
	}
	
	target = getLeadingTargetPosition(unit, unit->target);
	
	Vector gunPosition = unit->position;
	gunPosition += unit->gunPosition[0];
	
	fireWeapon(unit, unit->currentWeapon, getGunPosition(unit, 0), target);
}
