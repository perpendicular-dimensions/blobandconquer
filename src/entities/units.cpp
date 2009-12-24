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

#include "units.h"
		
void drawUnitWrapped(bool transparent)
{
	if (transparent)
	{
		return;
	}
	
	Unit *unit = (Unit*)self;
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	glPushMatrix();
	{
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(unit->position.x, unit->position.y, unit->position.z);
		glBindTexture(GL_TEXTURE_2D, textureManager->getTexture("textures/game/web.png")->data);
		glScalef(2, 2, 2);
		glCallList(modelManager->getSimpleModel("gfx/rw2/webbing.blend.raw")->data);
	}
	glPopMatrix();
}

void drawUnit(bool transparent)
{
	Unit *unit = (Unit*)self;
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	if (!transparent)
	{
		float z = unit->rotation.z;
		
		if (unit->entityType != ET_BLOB)
		{
			float limit = 360;
			
			switch (unit->definition->type)
			{
				case NME_PISTOL_DROID:
				case NME_GRENADE_DROID:
				case NME_MACHINEGUN_DROID:
				case NME_SHOTGUN_DROID:
					limit = 45;
					break;
					
				case NME_MACHINEGUN_SPIDERBLOB:
				case NME_SHOTGUN_SPIDERBLOB:
				case NME_PLASMA_SPIDERBLOB:
					limit = 10;
					break;
					
				default:
					limit = 360;
					break;
			}
			
			Math::limit(&z, -limit, limit);
		}
		
		glBindTexture(GL_TEXTURE_2D, self->definition->texture[0]->data);
		
		glPushMatrix();
		{
			glTranslatef(unit->position.x, unit->position.y, unit->position.z);
			
			if (unit->helpless)
			{
				unit->rotation.x += (3 * engine->getTimeDifference(TD_LOGIC));
				unit->rotation.y += (3 * engine->getTimeDifference(TD_LOGIC));
			}
			
			glRotatef(unit->rotation.x, 0.0, 0.0, 1.0);
			glRotatef(unit->rotation.y, 0.0, 1.0, 0.0);
			glRotatef(z, 1.0, 0.0, 0.0);
			
			glColor3f(1.0, 1.0, 1.0);
			glScalef(unit->scale, unit->scale, unit->scale);
			
			if (unit->definition->model[0] != NULL)
			{
				glCallList(unit->definition->model[0]->data);
			}
			else if (unit->definition->md2 != NULL)
			{
				drawMD2(unit);
			}
			else
			{
				printf("No model for %s\n", unit->getName());
				exit(1);
			}
		}
		glPopMatrix();
	}
	else
	{
		if (unit->shieldHit)
		{
			glPushMatrix();
			{
				glTranslatef(unit->position.x, unit->position.y, unit->position.z);
			
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
			
				GLColor c = GLColor::blue;
				c.color[3] = unit->shieldHit;
				glColor4fv(c.color);
			
				gluSphere(graphics->quadric, 4, 32, 16);
			
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
			}
			glPopMatrix();
		
			unit->shieldHit -= (0.01 * engine->getTimeDifference(TD_LOGIC));
			Math::limit(&unit->shieldHit, 0, 2.0);
		}
		
		if (unit->flags & EF_FROZEN)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			
			glBindTexture(GL_TEXTURE_2D, textureManager->getTexture("textures/game/iceBlock.jpg")->data);
			
			float size = unit->boundingBox.maxs.x * 2;
			
			glColor3f(0.5, 0.5, 0.5);
			drawBox(unit->position, size, size, size, 1);
		}
		
		if (unit->flags & EF_ASSIMILATING)
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
		
			glLineWidth(3.0);
	
			glPushMatrix();
			{
				glBegin(GL_LINES);
				{
					glColor4f(0.0, 0.0, 1.0, 1.0); glVertex3f(unit->lastTargetPosition.x, unit->lastTargetPosition.y, unit->lastTargetPosition.z);
					glColor4f(0.0, 1.0, 1.0, 0.75); glVertex3f(unit->destination.x, unit->destination.y, unit->destination.z);
				}
				glEnd();
			}
			glPopMatrix();
	
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}
	}
}

void generalUnitFrozen()
{
	float size, x, y, z;
	Vector velocity;

	Unit *unit = (Unit*)self;
	
	// do nothing
	
	unit->velocity.x *= 0.5;
	unit->velocity.y *= 0.5;
	
	if (unit == player)
	{
		game->setGamePlayMessage(INFO_BAD, _("YOU'VE BEEN FROZEN! PRESS BUTTONS TO ESCAPE!!!"));
		
		if (game->isAnyControl())
		{
			unit->mental += engine->getTimeDifference(TD_LOGIC);
			unit->velocity.x = Math::rrand(-2, 2);
			unit->velocity.y = Math::rrand(-2, 2);
			unit->velocity.z = Math::rrand(0, 1);
			unit->velocity = unit->velocity * 0.05;
		}
	}
	
	if (unit->mental >= MAX_MENTAL)
	{
		unit->flags &= ~EF_FROZEN;
		unit->flags &= ~EF_PUSHABLE;
	
		for (int i = 0 ; i < 50 ; i++)
		{
			size = Math::rrand(10, 75 - i);
			size *= 0.01;
		
			x = Math::rrand(-25, 25);
			y = Math::rrand(-25, 25);
			z = Math::rrand(0, 100);
		
			x *= 0.01;
			y *= 0.01;
			z *= 0.01;
		
			velocity.set(x, y, z);
			addGeneralDebris(self->position, velocity, Math::rrand(1000, 2000), "textures/game/iceBlock.jpg", size, true);
		}
		
		unit->boundingBox.mins = unit->definition->bbMins;
		unit->boundingBox.maxs = unit->definition->bbMaxs;
		unit->updateBoundingBox();
		
		audio->playSound(SND_GLASS_SHATTER_1, CH_ANY, camera->getSoundDistance(self->position));
		
		if (unit == player)
		{
			unit->action = NULL;
			unit->health -= 5;
			unit->helpless = 150;
			unit->velocity = velocity;
		}
		else
		{
			unit->action = unit->walk;
		}
		
		unit->thinkTime = 0;
	}
}

void generalUnitWrapped()
{
	Unit *unit = (Unit*)self;
	
	unit->draw = &drawUnitWrapped;
	
	// do nothing
	unit->velocity.x = unit->velocity.y = 0;
	
	if (unit == player)
	{
		game->setGamePlayMessage(INFO_BAD, _("YOU'VE BEEN WRAPPED UP! PRESS BUTTONS TO ESCAPE!!!"));
		
		if (game->isAnyControl())
		{
			unit->mental += engine->getTimeDifference(TD_LOGIC);
			unit->velocity.x = Math::rrand(-2, 2);
			unit->velocity.y = Math::rrand(-2, 2);
			unit->velocity.z = Math::rrand(0, 1);
			unit->velocity = unit->velocity * 0.05;
			addPuffParticles(unit->position, GLColor::white, 1, 1, 2);
		}
	}
	
	if (unit->mental >= MAX_MENTAL)
	{
		unit->flags &= ~EF_WRAPPED;
		unit->flags &= ~EF_STATIC;
	
		if (unit == player)
		{
			unit->action = NULL;
		}
		else
		{
			unit->action = unit->walk;
		}
		
		unit->draw = &drawUnit;
		unit->thinkTime = 0;
		unit->velocity.x = unit->velocity.y = unit->velocity.z = 0;
		
		addPuffParticles(unit->position, GLColor::white, 25, 4, 6);
	}
}

void generalUnitReappear()
{
	audio->playSound(SND_TELEPORT2, CH_ANY, camera->getSoundDistance(self->position));
	addTeleportParticles(self->position);
	
	self->flags &= ~EF_VANISHED;
	
	if (self->entityType == ET_BOSS)
	{
		self->thinkTime = Math::rrand(25, 75);
	}
	else
	{
		self->thinkTime = Math::rrand(100, 200);
	}
	
	self->action = self->walk;
	self->contentType = CONTENTS_SOLID;
	self->rotation.set(0, 0, 0);
}

void generalUnitVanish(Vector targetPosition, int timeout)
{
	audio->playSound(SND_TELEPORT2, CH_ANY, camera->getSoundDistance(self->position));
	addTeleportParticles(self->position);
	
	self->position = targetPosition;
	self->velocity.set(0, 0, 0);
	self->thinkTime = timeout;
	self->action = generalUnitReappear;
	
	switch (self->entityType)
	{
		case ET_BIOMECH:
		case ET_BLOB:
			((Unit*)self)->liquidLevel = 0;
			((Unit*)self)->helpless = 0;
			break;
			
		case ET_BOSS:
			((Boss*)self)->helpless = 0;
			break;
	}
	
	self->flags |= EF_VANISHED;
}

void throwUnit(Unit *unit, Vector velocity)
{
	if ((unit == player) && (game->cheatStun))
	{
		return;
	}
	
	if (unit == player)
	{
		unit->action = NULL;
	}
	
	if (unit->helpless == 0)
	{
		unit->helpless = 200;
	}
	
	unit->velocity = velocity;
	unit->velocity.x /= 4;
	unit->velocity.y /= 4;
	unit->velocity.z = 0.25;
	
	unit->flags |= EF_BOUNCES;
}

void unitContentChanged(Unit *unit)
{
	Vector position;
	int newContentType;
	int envContents[3];
	
	envContents[0] = envContents[1] = envContents[2] = CONTENTS_SOLID;
	
	unit->liquidLevel = 0;
	
	// Check content types... going from head to feet ;)
	// Should look at this really since it's probably bloody slow... :|
	
	position = unit->position;
	position.z += unit->boundingBox.maxs.z;
	envContents[0] = bsp->getContentsAtPosition(position);
	
	position = unit->position;
	envContents[1] = bsp->getContentsAtPosition(position);
	
	position = unit->position;
	position.z += unit->boundingBox.mins.z;
	envContents[2] = bsp->getContentsAtPosition(position);
	
	newContentType = tracer->outputContent;
	
	if (envContents[1] & CONTENTS_WATER)
	{
		unit->liquidLevel = 1;
	}
	
	if (envContents[0] & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
	{
		unit->liquidLevel = 2;
		unit->oxygen = min(unit->oxygen, MAX_OXYGEN);
		unit->flags &= ~EF_ONFIRE;
	}
	
	if (newContentType == unit->contentType)
	{
		return;
	}
	
	// Unit is moving in to liquid
	if (newContentType & (CONTENTS_AIR|CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
	{
		if (unit->contentType & (CONTENTS_AIR|CONTENTS_SOLID))
		{
			Math::limit(&unit->velocity.z, -0.5, 0.5);
			
			if (tracer->outputContent & CONTENTS_WATER)
			{
				audio->playSound(SND_WATER_IN, (unit == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(unit->position));
			}
			else if (tracer->outputContent & CONTENTS_SLIME)
			{
				audio->playSound(SND_SLIME_IN, (unit == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(unit->position));
			}
			else if (tracer->outputContent & CONTENTS_LAVA)
			{
				audio->playSound(SND_LAVA_IN, (unit == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(unit->position));
			}
		}
	}
	
	// Unit is moving out of water
	if (!(unit->flags & EF_DYING))
	{
		if (newContentType & (CONTENTS_AIR|CONTENTS_SOLID))
		{
			if (unit->contentType & CONTENTS_WATER)
			{
				if ((unit->velocity.z > 0) && (unit->helpless == 0))
				{
					unit->velocity.z = 1.25;
				}
				
				audio->playSound(SND_WATER_OUT, (unit == player) ? CH_PLAYER : CH_ANY, camera->getSoundDistance(unit->position));
			}
		}
	}
	
	unit->contentType = newContentType;
}

void keepUnitAboveLiquid(Unit *unit)
{
	Vector start, end;

	if (unit->velocity.z >= 0)
	{
		return;
	}
	
	start = unit->position;
	end = start;
	end.z -= 15;
	
	bsp->traceRay(start, end);
	
	if (!(tracer->outputContent & CONTENTS_SOLID))
	{
		unit->velocity.z = 0;
	}
}

void keepUnitInLiquid(Unit *unit)
{
	Vector start, end;

	if (unit->velocity.z <= 0)
	{
		return;
	}
	
	start = unit->position;
	end = start;
	end.z += 15;
	
	bsp->traceRay(start, end);
	
	if (!(tracer->outputContent & CONTENTS_WATER))
	{
		unit->velocity.z = 0;
	}
}

bool unitCanSpawnInLocation(Unit *unit, Vector start)
{
	Vector end, mins, maxs;
	
	end = start;
	
	if (unit != NULL)
	{
		end.z -= (unit->entityType == ET_BLOB) ? 100 : 7;
	}
	else
	{
		end.z -= 100;
	}
	
	mins.x = -3;
	mins.y = -3;
	mins.z = -6;
	
	maxs.x = 3;
	maxs.y = 3;
	maxs.z = 6;
	
	/* Check start spawn point */
	
	bsp->traceBox(start, start, mins, maxs);
	
	if (tracer->outputContent & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
	{
		return false;
	}
	
	if (tracer->outputAllSolid)
	{
		return false;
	}
	
	/* Check end spawn point */
	
	bsp->traceBox(start, end, mins, maxs);
	
	if (tracer->outputContent & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
	{
		return false;
	}
	
	if (tracer->outputAllSolid)
	{
		return false;
	}
	
	if (tracer->outputFraction == 1.0)
	{
		return bbManager->canSightSolid(unit, start, end);
	}
	
	return true;
}

Vector getUnitForwardVector(Entity *unit)
{
	Vector v;
	
	float r = toRadians(unit->rotation.x);
	
	v = unit->position;
	
	v.x += (sin(r) * 25);
	v.y -= (cos(r) * 25);
	v -= unit->position;
	v.normalize();
	
	return v;
}

bool unitIsAtEdge(Unit *unit)
{
	Vector v, start, end;
	
	start = unit->position;
	start += unit->velocity;
	start -= unit->position;
	start.normalize();
	
	start = start * 8;
	start += unit->position;
	
	end = start;
	end.z += unit->boundingBox.mins.z;
	
	// don't allow Blobs to walk over edges if they
	// are in combat...
	if (unit->entityType == ET_BLOB)
	{
		if (unit->definition->type == BLOB_MIA)
		{
			end.z--;
		}
		else if (unit->target == NULL)
		{
			end.z -= (fabs(unit->position.z - player->position.z));
			end.z -= 5;
		}
		else
		{
			end.z--;
		}
	}
	else
	{
		// enemies can't walk off edges when they have no target...
		if (unit->target == NULL)
		{
			end.z--;
		}
		else
		{
			end.z -= 64;
		}
	}
	
	tracer->treatAllAsSolid = true;
	bsp->traceRay(start, end);
	tracer->treatAllAsSolid = false;
	
	// Trace reach end point in world...
	if (tracer->outputFraction == 1.0)
	{
		if ((tracer->outputContent & (CONTENTS_WATER)) && (unit->entityType == ET_BLOB))
		{
			return false;
		}
		
		return (!bbManager->canSightSolid(unit, start, end));
	}
	
	// Blob's Trace hit slime or lava... Check if it hit a solid
	
	if (unit->entityType == ET_BLOB && tracer->outputContent & (CONTENTS_SLIME|CONTENTS_LAVA))
	{
		return (!bbManager->canSightSolid(unit, start, end));
	}
	
	// Biomech's Trace hit slime, water or lava... Check if it hit a solid
	
	else if (unit->entityType == ET_BIOMECH && tracer->outputContent & (CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WATER))
	{
		return (!bbManager->canSightSolid(unit, start, end));
	}
	
	return false;
}

void keepUnitAwayFromEdges(Unit *unit)
{
	Vector end;

	//debug(("%s\n", unit->position.toString()));
	
	if ((unit->helpless > 0) || (unit->dead))
	{
		return;
	}
	
	if (unit->flags & EF_WEIGHTLESS)
	{
		keepUnitAboveLiquid(unit);
		return;
	}
	
	if (unit->flags & EF_SWIMS)
	{
		keepUnitInLiquid(unit);
		return;
	}
	
	if ((unit->velocity.x == 0) && (unit->velocity.y == 0))
	{
		return;
	}
	
	if (!(unit->flags & EF_ONGROUND))
	{
		// check we're not close enough to the ground to be considered.
		end = unit->position;
		end.z += unit->boundingBox.mins.z;
		end.z--;
	
		bsp->traceRay(unit->position, end);
	
		// can see the target so we're not on the ground
		if (tracer->outputFraction == 1.0)
		{
			return;
		}
	}
	
	if (unitIsAtEdge(unit))
	{
		unit->velocity.x = 0;
		unit->velocity.y = 0;
		unit->velocity.z = 0;
		unit->thinkTime = 0;
	}
}

bool unitCanSeeTarget(Entity *observer, Entity *target, Vector startPosition)
{
	if (!bsp->canSightTarget(startPosition, target->position))
	{
		return false;
	}
	
	if (!bbManager->canSightTarget(observer, target))
	{
		return false;
	}
	
	return true;
}

void teleportUnit()
{
	self->flags &= ~EF_TELEPORTING;
	self->position = self->destination;
	addTeleportParticles(self->position);
	
	if (self != player)
	{
		self->action = self->walk;
		camera->contentType = CONTENTS_SOLID;
		audio->playSound(SND_TELEPORT3, CH_SPAWN, camera->getSoundDistance(self->position));
	}
	else
	{
		self->action = NULL;
		audio->playSound(SND_TELEPORT3, CH_PLAYER, 128);
	}
}

void calculateFaceLocation(Vector start, Vector end, Vector *v)
{
	float dx = start.x - end.x;
	float dy = start.y - end.y;
	v->x = -90 + toDegrees(atan2(dy, dx));
	
	float dz = start.z - end.z;
	float dist = Math::getDistance(start.x, start.y, end.x, end.y);	
	v->z = toDegrees(atan2(dz, dist));
}

void faceLocation(Boss *boss, Vector location)
{
	calculateFaceLocation(boss->position, location, &boss->rotation);
}

void faceLocation(Unit *unit, Vector location)
{
	calculateFaceLocation(unit->position, location, &unit->rotation);
}

void faceTarget(Unit *unit)
{
	if (unit->target == NULL)
	{
		return;
	}
	
	faceLocation(unit, unit->target->position);
}

char *getUnitStatus(Unit *unit)
{
	float healthPercent = unit->health;
	healthPercent /= unit->maxHealth;
	healthPercent *= 100;
	
	if (healthPercent <= 0)
	{
		return _("KIA");
	}
	else if (healthPercent <= 50)
	{
		return _("Wounded");
	}
	else
	{
		return _("OK");
	}
}
