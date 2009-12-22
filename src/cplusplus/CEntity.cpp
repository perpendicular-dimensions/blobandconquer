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

Entity::Entity()
{
	uniqueId = 1;
	
	entityType = ET_UNDEFINED;
	
	referenced = false;
	
	doNotSave = false;
	
	contentType = CONTENTS_SOLID;
	
	movementType = MT_NORMAL;
	
	health = maxHealth = 0;
	
	definition = NULL;
	
	currentAnimFrame = 0;
	nextAnimFrame = 0;
	animationTimer = 0;
	currentAnim = NULL;
	
	scale = 1;
	
	thinkTime = 0;
	custom = 0;
		
	action = NULL;
	die = NULL;
	walk = NULL;
	attack = NULL;
	touch = NULL;
	draw = NULL;
	
	ownerId = 0;
	owner = NULL;
	
	riding = NULL;
	
	heightAboveGround = 0;
	
	elasticity = 1.5;
	
	cluster = -1;
	
	flags = 0;
	
	// defaults
	inCollisionCandidateList = false;
	boundingBox.mins.set(-3, -3, -3);
	boundingBox.maxs.set(3, 3, 3);
	
	occupiedRect.x = 0;
	occupiedRect.y = 0;
	occupiedRect.w = 0;
	occupiedRect.h = 0;
}

const unsigned int Entity::getUniqueId()
{
	return uniqueId;
}

void Entity::setName(const char *name)
{
	this->name = name;
}

const char *Entity::getName()
{
	return name.getText();
}

const char *Entity::getEntityTypeName()
{
	return entityNames[entityType];
}

bool Entity::performNextAction(float td)
{
	Math::limit(&(thinkTime -= td), 0, RAND_MAX);
	
	return (thinkTime == 0);
}

void Entity::applyGravity(float td)
{
	if ((flags & EF_TELEPORTING) || (flags & EF_WEIGHTLESS) || (flags & EF_ONGROUND) || (flags & EF_NOCLIP))
	{
		return;
	}
	
	if (contentType & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
	{
		velocity.z -= (LIQUID_GRAVITY * td);
	}
	else
	{
		velocity.z -= (GRAVITY * td);
	}
}

void Entity::prepareToMove(float td)
{
	realVelocity = velocity;
	
	velocity = velocity * td;
}

void Entity::move(float td)
{
	position += velocity;
	
	velocity = realVelocity;
	
	if ((flags & EF_ONGROUND) && (!(flags & EF_BOUNCES)))
	{
		velocity.z = 0;
	}
}

void Entity::updateBoundingBox()
{
	boundingBox.update(position);
}

float Entity::getHealthPercent()
{
	return ((health / maxHealth) * 100);
}

void Entity::setOwner(Entity *owner)
{
	if (owner == NULL)
	{
		this->owner = NULL;
		this->ownerId = 0;
		return;
	}
	
	this->owner = owner;
	this->ownerId = owner->uniqueId;
}

void Entity::load(Properties *props)
{
	uniqueId = props->getInt("uniqueId", uniqueId);
	
	name = props->getString("name", name.getText());
	
	entityType = props->getInt("entityType", entityType);
	
	scale = props->getFloat("scale", scale);
	
	movementType = props->getInt("movementType", movementType);
	
	health = props->getFloat("health", health);
	maxHealth = props->getFloat("maxHealth", maxHealth);
	
	thinkTime = props->getFloat("thinkTime", thinkTime);
	custom = props->getFloat("custom", custom);
	
	// don't overwrite if not needed
	if (props->hasProperty("position") || props->hasProperty("origin"))
	{
		position = (props->hasProperty("position")) ? props->getVector("position", position) : props->getVector("origin");
	}
	
	velocity = props->getVector("velocity", velocity);
	destination = props->getVector("destination", destination);
	rotation = props->getVector("rotation", rotation);
	oldPosition = props->getVector("oldPosition", oldPosition);
	
	ownerId = props->getInt("ownerId", ownerId);
	ownerName = props->getString("owner", "");
	
	elasticity = props->getFloat("elasticity", elasticity);
	
	flags = props->getInt("flags", flags);
	
	q3RadiantTargetName = props->getString("targetname", q3RadiantTargetName.getText());
	q3RadiantTarget = props->getString("target", q3RadiantTarget.getText());
	
	doNotSave = props->getInt("doNotSave", false);
	
	loadAdditional(props);
}

void Entity::save(FILE *fp)
{
	if (doNotSave)
	{
		return;
	}

	Properties props;
	
	props.setName(getEntityTypeName());
	
	props.setProperty("uniqueId", uniqueId);
	props.setProperty("name", name.getText());
		
	props.setProperty("entityType", entityType);
	
	props.setProperty("movementType", movementType);
	
	props.setProperty("scale", scale);
	
	props.setProperty("health", health);
	props.setProperty("maxHealth", maxHealth);
	
	if (definition != NULL)
	{
		props.setProperty("definitionType", definition->type);
	}
	
	props.setProperty("thinkTime", thinkTime);
	props.setProperty("custom", custom);
	
	props.setProperty("velocity", velocity.toString());
	props.setProperty("position", position.toString());
	props.setProperty("destination", destination.toString());
	props.setProperty("rotation", rotation.toString());
	props.setProperty("oldPosition", oldPosition.toString());
	
	props.setProperty("ownerId", (owner != NULL) ? owner->getUniqueId() : 0);
	
	props.setProperty("elasticity", elasticity);
	
	props.setProperty("flags", flags);
	
	//props.setProperty("q3RadiantTargetName", q3RadiantTargetName.getText());
	//props.setProperty("q3RadiantTarget", q3RadiantTarget.getText());
	
	saveAdditional(&props);
	
	props.save(fp);
}
