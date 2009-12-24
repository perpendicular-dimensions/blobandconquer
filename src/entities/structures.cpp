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

#include "structures.h"

void structureFall(Entity *other);

void structureMovePush(Entity *other)
{
	Vector wantedPosition = other->position;
	wantedPosition.x += (self->velocity.x * engine->getTimeDifference(TD_LOGIC));
	wantedPosition.y += (self->velocity.y * engine->getTimeDifference(TD_LOGIC));
	
	bsp->traceBox(wantedPosition, wantedPosition, other->boundingBox.mins, other->boundingBox.maxs);
		
	if (!tracer->outputAllSolid)
	{
		other->position = wantedPosition;
	}
}

void moveStructureToStart(Structure *structure)
{
	if (structure->position == structure->startPosition)
	{
		structure->touch = NULL;
		return;
	}
	
	structure->velocity = structure->startPosition;
	structure->velocity -= structure->endPosition;
	structure->velocity.normalize();
	structure->velocity = structure->velocity * structure->speed;
	structure->destination = structure->startPosition;
	//structure->touch = structureMovePush;
}

void moveStructureToEnd(Structure *structure)
{
	if (structure->position == structure->endPosition)
	{
		structure->touch = NULL;
		return;
	}
	
	structure->velocity = structure->endPosition;
	structure->velocity -= structure->startPosition;
	structure->velocity.normalize();
	structure->velocity = structure->velocity * structure->speed;
	structure->destination = structure->endPosition;
	//structure->touch = structureMovePush;
}

void drawStructure(bool transparent)
{
	Structure *structure = (Structure*)self;
	
	glDisable(GL_LIGHTING);
	
	enableMultiTexturing();
	
	float fade;
	TextureSpec *textureSpec;
	int lightMap;
	Texture *grey = textureManager->getTexture("textures/game/grey.png");
	
	for (int i = 0 ; i < structure->model->numOfFaces ; i++)
	{
		textureSpec = bsp->textureSpec[structure->model->textureID[i]];
		
		if (textureSpec->blended != transparent)
		{
			continue;
		}
		
		lightMap = bsp->lightmapTextures[structure->model->lightmapID[i]];
		
		fade = min(textureSpec->lightLevel, (structure->health / 500));
		
		glPushMatrix();
		{
			glColor3f(1.0, 0.0, 0.0);
			
			glTranslatef(structure->position.x, structure->position.y, structure->position.z);
			
			glRotatef(structure->rotation.x, 0.0, 0.0, 1.0);
			glRotatef(structure->rotation.y, 0.0, 1.0, 0.0);
			glRotatef(structure->rotation.z, 1.0, 0.0, 0.0);
			
			glVertexPointer(3, GL_FLOAT, sizeof(Vector), &structure->model->position[i][0]);
		
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Point), &structure->model->textureCoord[i][0]);
		
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Point), &structure->model->lightmapCoord[i][0]);
		
			glActiveTextureARB(GL_TEXTURE0_ARB);
			
			if (game->totalRemaining != 0)
			{
				if (!textureSpec->blended)
				{
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
					textureSpec->activate(engine->getTimeDifference(TD_ANIMATION));
					glColor4f(1.0, graphics->redPulse, graphics->redPulse, 1.0);
				}
				else
				{
					textureSpec->activate(engine->getTimeDifference(TD_ANIMATION));
				}
			}
			else
			{
				textureSpec->activate(engine->getTimeDifference(TD_ANIMATION));
				
				if (structure->isMissionTarget)
				{
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
					glColor4f(entityManager->mtGlow, entityManager->mtGlow, entityManager->mtGlow, 1.0);
				}
			}
		
			glActiveTextureARB(GL_TEXTURE1_ARB);

			switch (self->entityType)
			{
				case ET_PUSHBLOCK:
					glBindTexture(GL_TEXTURE_2D, grey->data);
					break;
					
				case ET_WINDOW:
				case ET_BARRIER:
					glBindTexture(GL_TEXTURE_2D, bsp->whiteTexture);
					break;
					
				default:
					if (structure->isMissionTarget)
					{
						glBindTexture(GL_TEXTURE_2D, bsp->whiteTexture);
					}
					else if ((structure->model->lightmapID[i] >= 0) && (!textureSpec->bright))
					{
						glBindTexture(GL_TEXTURE_2D, lightMap);
					}
					else
					{
						glBindTexture(GL_TEXTURE_2D, bsp->whiteTexture);
					}
					break;
			}
			
			if (structure->entityType == ET_BARRIER)
			{
				glColor4f(textureSpec->lightLevel, textureSpec->lightLevel, textureSpec->lightLevel, fade);
			}
		
			glActiveTextureARB(GL_TEXTURE0_ARB);
		
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			glDrawElements(GL_TRIANGLES, structure->model->numMeshVerts[i], GL_UNSIGNED_INT, &structure->model->indices[i][0]);
			glDisable(GL_POLYGON_OFFSET_FILL);
		
			textureSpec->finish();
		}
		glPopMatrix();
	}
	
	finishMultiTexturing();
}

void changeDoorState(const char *name, int state)
{
	debug(("changeDoorState - %s\n", name));
	
	Door *door;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->entityType != ET_DOOR)
		{
			continue;
		}
		
		if (entity->name != name)
		{
			continue;
		}
			
		door = (Door*)entity;
		
		door->autoClose = false;
		
		if ((state == DOOR_OPEN) && (door->state != DOOR_OPEN))
		{
			doorOpen(door);
		}
		else if ((door->state != DOOR_LOCKED) && (door->state != DOOR_CLOSED))
		{
			doorClose(door);
		}
	}
}

void toggleDoorState(const char *name)
{
	debug(("toggleDoorState - %s\n", name));
	
	Door *door;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->entityType != ET_DOOR)
		{
			continue;
		}
		
		if (entity->name != name)
		{
			continue;
		}
			
		door = (Door*)entity;
		
		door->autoClose = false;
		
		if (door->state == DOOR_OPEN)
		{
			doorClose(door);
		}
		else
		{
			doorOpen(door);
		}
	}
}

void changeShieldState(const char *name, int state)
{
	debug(("toggleShield - %s\n", name));
	
	Structure *structure;
	
	bool shutDown = false;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->entityType != ET_BARRIER)
		{
			continue;
		}
		
		if ((entity->name != name) || (entity->name == ""))
		{
			continue;
		}
		
		structure = (Structure*)entity;
		
		if (state != 2)
		{
			structure->custom = state;
		}
		else
		{
			structure->custom = !structure->custom;
		}
		
		shutDown = true;
	}
	
	if (shutDown)
	{
		audio->playSound(SND_BARRIER_SHUT_DOWN, CH_ANY, camera->getSoundDistance(structure->position));
	}
}

void toggleShield(const char *name)
{
	debug(("toggleShield - %s\n", name));
	
	changeShieldState(name, 2);
}

void toggleStructurePosition(const char *name, bool active)
{
	debug(("toggleStructurePosition - %s - %d\n", name, active));
	
	Structure *structure;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->entityType != ET_STRUCTURE)
		{
			continue;
		}
		
		if (entity->name != name)
		{
			continue;
		}
		
		structure = (Structure*)entity;
		
		if (active)
		{
			debug(("toggleStructurePosition - Moving Structure to End\n"));
			moveStructureToEnd(structure);
		}
		else
		{
			debug(("toggleStructurePosition - Moving Structure to Start\n"));
			moveStructureToStart(structure);
		}
	}
}

void toggleStructurePosition(const char *name)
{
	debug(("toggleStructurePosition - %s\n", name));
	
	Structure *structure;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if (entity->entityType != ET_STRUCTURE)
		{
			continue;
		}
		
		if (entity->name != name)
		{
			continue;
		}
		
		structure = (Structure*)entity;
		
		if (structure->position == structure->startPosition)
		{
			debug(("toggleStructurePosition - Moving Structure to End\n"));
			moveStructureToEnd(structure);
		}
		else
		{
			debug(("toggleStructurePosition - Moving Structure to Start\n"));
			moveStructureToStart(structure);
		}
	}
}

void toggleStructureState(const char *name)
{
	debug(("toggleStructureState - %s\n", name));
	
	toggleDoorState(name);
	toggleLiftPosition(name);
	toggleShield(name);
	toggleStructurePosition(name);
	toggleEmitter(name);
}

void handleStructureStateChange(const char *name, bool active)
{
	debug(("handleStructureStateChange - %s (%d)\n", name, active));
	
	if (active)
	{
		changeDoorState(name, DOOR_OPEN);
		toggleLiftPosition(name, active);
	}
	else
	{
		changeDoorState(name, DOOR_CLOSED);
	}
	
	toggleShield(name);
	toggleStructurePosition(name, active);
	enableEmitter(name, active); // emitters count as structures... it's easier that way :)
}

// ===================================== General ================================

void destroyableStructureDie()
{
	Vector v;
	
	v = self->position;
	v.x += Math::rrand((int)self->boundingBox.mins.x, (int)self->boundingBox.maxs.x);
	v.y += Math::rrand((int)self->boundingBox.mins.y, (int)self->boundingBox.maxs.y);
	v.z += Math::rrand((int)self->boundingBox.mins.z, (int)self->boundingBox.maxs.z);
	
	doExplosion(v, Math::rrand(2, 5), self);
	
	self->health -= 10;
	
	if (self->health <= -100)
	{
		self->flags |= EF_DEAD;
		doExplosion(v, 10, self);
		removeTouchingDecals(self);
		return;
	}
	
	self->thinkTime = Math::rrand(10, 50);
}

void destroyableStructurePostDie()
{
	self->health = 0;
	
	self->thinkTime = 0;
	
	self->action = destroyableStructureDie;
	
	self->flags |= EF_DYING;
	
	fireTriggers(self->getName(), TRIGGER_TYPE_ENTITY_KILLED);
}

// =================================== Windows =================================

void splitWindow(Triangle *parent, int level)
{
	level++;
	
	if (level >= 8)
	{
		addGlassShard(parent);
		return;
	}
	
	if ((level >= 5) && ((rand() % 3) == 0))
	{
		addGlassShard(parent);
		return;
	}
	
	int vertex = rand() % 3;
	
	Vector split;
	Triangle child1, child2;
	
	switch (vertex)
	{
		case 0:
			split.x = parent->point[1].x + parent->point[2].x;
			split.y = parent->point[1].y + parent->point[2].y;
			split.z = parent->point[1].z + parent->point[2].z;
			break;
		
		case 1:
			split.x = parent->point[0].x + parent->point[2].x;
			split.y = parent->point[0].y + parent->point[2].y;
			split.z = parent->point[0].z + parent->point[2].z;
			break;
		
		case 2:
			split.x = parent->point[1].x + parent->point[0].x;
			split.y = parent->point[1].y + parent->point[0].y;
			split.z = parent->point[1].z + parent->point[0].z;
			break;
	}
	
	split.x /= 2;
	split.y /= 2;
	split.z /= 2;
	
	switch (vertex)
	{
		case 0:
			child1.point[0] = parent->point[0];
			child1.point[1] = parent->point[1];
			child1.point[2] = split;
			
			child2.point[0] = parent->point[0];
			child2.point[1] = split;
			child2.point[2] = parent->point[2];
			break;
		
		case 1:
			child1.point[0] = parent->point[0];
			child1.point[1] = parent->point[1];
			child1.point[2] = split;
			
			child2.point[0] = split;
			child2.point[1] = parent->point[1];
			child2.point[2] = parent->point[2];
			break;
		
		case 2:
			child1.point[0] = parent->point[0];
			child1.point[1] = split;
			child1.point[2] = parent->point[2];
			
			child2.point[0] = split;
			child2.point[1] = parent->point[1];
			child2.point[2] = parent->point[2];
			break;
	}
	
	splitWindow(&child1, level);
	splitWindow(&child2, level);
}

void windowShatter()
{
	debug(("windowShatter()\n"));
	
	Structure *structure = (Structure*)self;
	
	Triangle t1, t2;
	
	bool useX = (max(structure->boundingBox.maxs.x, structure->boundingBox.maxs.y) == structure->boundingBox.maxs.x);
	
	// A
	t1.point[0] = structure->position;
	(useX) ? t1.point[0].x += structure->boundingBox.mins.x : t1.point[0].y += structure->boundingBox.mins.y;
	t1.point[0].z += structure->boundingBox.maxs.z;
	
	// B
	t1.point[1] = structure->position;
	(useX) ? t1.point[1].x += structure->boundingBox.maxs.x : t1.point[1].y += structure->boundingBox.maxs.y;
	t1.point[1].z += structure->boundingBox.maxs.z;
	
	// D
	t1.point[2] = structure->position;
	(useX) ? t1.point[2].x += structure->boundingBox.mins.x : t1.point[2].y += structure->boundingBox.mins.y;
	t1.point[2].z += structure->boundingBox.mins.z;
	
	
	// B
	t2.point[0] = structure->position;
	(useX) ? t2.point[0].x += structure->boundingBox.maxs.x : t2.point[0].y += structure->boundingBox.maxs.y;
	t2.point[0].z += structure->boundingBox.maxs.z;
	
	// C
	t2.point[1] = structure->position;
	(useX) ? t2.point[1].x += structure->boundingBox.maxs.x : t2.point[1].y += structure->boundingBox.maxs.y;
	t2.point[1].z += structure->boundingBox.mins.z;
	
	// D
	t2.point[2] = structure->position;
	(useX) ? t2.point[2].x += structure->boundingBox.mins.x : t2.point[2].y += structure->boundingBox.mins.y;
	t2.point[2].z += structure->boundingBox.mins.z;
	
	engine->startDebugTimer("Shatter Window");
	splitWindow(&t1, 0);
	splitWindow(&t2, 0);
	engine->endDebugTimer("Shatter Window");
	
	self->flags |= EF_DEAD;
	
	removeTouchingDecals(self);
}

// ================== Weak Walls ==============================

void weakWallExplode()
{
	Vector position, velocity;
	float size, x, y, z;
	
	Structure *structure = (Structure*)self;
	
	String textureName;
	textureName.setText("%s.jpg", bsp->textureSpec[structure->model->textureID[0]]->name.getText());
			
	self->flags &= ~EF_DYING;
	self->flags |= EF_DEAD;
	removeTouchingDecals(self);
	self->health = -100;
			
	for (int i = 0 ; i < 10 ; i++)
	{
		position = self->position;
		position.x += Math::rrand((int)self->boundingBox.mins.x, (int)self->boundingBox.maxs.x);
		position.y += Math::rrand((int)self->boundingBox.mins.y, (int)self->boundingBox.maxs.y);
		position.z += Math::rrand((int)self->boundingBox.mins.z, (int)self->boundingBox.maxs.z);
		doExplosion(position, 25, self);
	}
	
	bbManager->removeBox(self);
	
	audio->playSound(SND_LARGE_EXPLOSION, CH_ANY, camera->getSoundDistance(self->position));
	
	fireTriggers(self->getName(), TRIGGER_TYPE_ENTITY_KILLED);
	
	for (int i = 0 ; i < 25 ; i++)
	{
		size = Math::rrand(50, 450);
		size *= 0.01;
		
		x = Math::rrand(-25, 25);
		y = Math::rrand(-25, 25);
		z = Math::rrand(0, 50);
		
		position = self->position;
		position.x += Math::rrand((int)self->boundingBox.mins.x, (int)self->boundingBox.maxs.x);
		position.y += Math::rrand((int)self->boundingBox.mins.y, (int)self->boundingBox.maxs.y);
		position.z += Math::rrand((int)self->boundingBox.mins.z, (int)self->boundingBox.maxs.z);
		
		x *= 0.01;
		y *= 0.01;
		z *= 0.01;
		velocity.set(x, y, z);
		addGeneralDebris(position, velocity, Math::rrand(1000, 2000), textureName.getText(), size, false);
	}
}

void weakWallPreExplode()
{
	Structure *structure = (Structure*)self;
	
	if (structure->currentItem == NULL)
	{
		return;
	}
	
	if (structure->currentItem->custom == 0)
	{
		structure->currentItem->health = 0;
		self->action = &weakWallExplode;
		
		doExplosion(self->position, 10, self);
	}
}

bool calcC4Facing(float position, float normal, float mins, float maxs, float pp)
{
	double before = position + (normal == -1 ? mins : maxs);
	double after = position + (normal == -1 ? mins : maxs) + (50 * normal);
	
	if (before < pp && after > pp)
	{
		return true;
	}
	else if (before > pp && after < pp)
	{
		return true;
	}
	
	return false;
}

void weakWallLookForPlayer()
{
	BoundingBox box;
	
	if (player->health < 0)
	{
		return;
	}
	
	box = self->boundingBox;
	box.mins.x -= 12;
	box.mins.y -= 12;
	box.mins.z -= 1;
	box.maxs.x += 12;
	box.maxs.y += 12;
	box.maxs.z += 1;
	
	Structure *structure = (Structure*)self;
	
	if (Collision::collision(player->position, player->boundingBox, self->position, box))
	{
		if ((!game->hasItem(_("C4"))) && (!game->cheatItems))
		{
			if (structure->hint)
			{
				game->setGamePlayMessage(INFO_GENERAL, _("C4 required"));
			}
			return;
		}
		
		if (structure->hint)
		{
			game->setGamePlayMessage(INFO_GENERAL, _("Press ACTION to use C4"));
		}
		
		if (game->isControl(CTRL_ACTION))
		{
			int face;
			
			int i = game->removeItem(_("C4"));
			
			String s;
			s.setText("str_item_%d", i);
			textureManager->removeTexture(s.getText());
			
			self->action = &weakWallPreExplode;
			self->flags |= EF_DYING;
			
			Item *item = addStickyC4(player->position);
			
			getCluster(item);
			
			structure->currentItem = item;
			
			for (int i = 0 ; i < 6 ; i++)
			{
				if (self->boundingBox.plane[i].normal.x != 0)
				{
					/*
					before = self->position.x + (self->boundingBox.plane[i].normal.x == -1 ? self->boundingBox.mins.x : self->boundingBox.maxs.x);
					after = self->position.x + (self->boundingBox.plane[i].normal.x == -1 ? self->boundingBox.mins.x : self->boundingBox.maxs.x) + (50 * self->boundingBox.plane[i].normal.x);
					
					if (before < player->position.x && after > player->position.x) face = i;
					else if (before > player->position.x && after < player->position.x) face = i;
					*/
					
					if (calcC4Facing(self->position.x, self->boundingBox.plane[i].normal.x, self->boundingBox.mins.x, self->boundingBox.maxs.x, player->position.x))
					{
						face = i;
					}
				}
				else if (self->boundingBox.plane[i].normal.y != 0)
				{
					if (calcC4Facing(self->position.y, self->boundingBox.plane[i].normal.y, self->boundingBox.mins.y, self->boundingBox.maxs.y, player->position.y))
					{
						face = i;
					}
				}
				else
				{
					if (calcC4Facing(self->position.z, self->boundingBox.plane[i].normal.z, self->boundingBox.mins.z, self->boundingBox.maxs.z, player->position.z))
					{
						face = i;
					}
				}
			}
			
			switch (face)
			{
				case 0:
					item->rotation.y = -90;
					break;
					
				case 1:
					item->rotation.y = 90;
					break;
					
				case 2:
					item->rotation.x = 180;
					break;
					
				case 3:
					item->rotation.x = 0;
					break;
					
				case 4:
					item->rotation.x = 0;
					break;
					
				case 5:
					item->rotation.x = 90;
					item->rotation.y = 180;
					break;
			}
			
			audio->playSound(SND_C4_CLOCK, CH_ANY, 128);
			
			game->resetControl(CTRL_ACTION);
		}
	}
}

// ================== Push Blocks =============================

void resetStructures(const char *name)
{
	Vector position;

	debug(("resetStructures() - %s\n", name));
	
	bool playSound = false;
	
	for (Structure *structure = (Structure*)entityManager->structureList.getFirstElement() ; structure != NULL ; structure = (Structure*)structure->next)
	{
		if ((structure->entityType != ET_PUSHBLOCK) && (structure->entityType != ET_WEAK_STRUCTURE))
		{
			continue;
		}
		
		if (structure->name != name)
		{
			continue;
		}
		
		if (structure->position == structure->startPosition)
		{
			continue;
		}
		
		playSound = true;
		position = structure->position;
		
		addTeleportParticles(structure->position);
		
		structure->position = structure->startPosition;
		
		addTeleportParticles(structure->position);
		
		if (structure->entityType == ET_WEAK_STRUCTURE)
		{
			structure->touch = &structureFall;
			structure->action = NULL;
			structure->velocity.set(0, 0, 0);
			structure->rotation.set(0, 0, 0);
			structure->health = RAND_MAX;
			structure->flags |= EF_WEIGHTLESS;
			structure->flags |= EF_NOCLIP;
			structure->contentType = 0;
		}
	}
	
	if (playSound)
	{
		audio->playSound(SND_TELEPORT2, CH_SPAWN, camera->getSoundDistance(position));
	}
}

void addPushBlockSmoke()
{
	Particle *particle;
	float x = self->boundingBox.maxs.x;
	float y = self->boundingBox.maxs.y;
	
	x += 2;
	y += 2;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		particle = particleManager->spawnParticle();
		
		particle->position = self->position;
		particle->position.x += Math::rrand(-x, x);
		particle->position.y += Math::rrand(-y, y);
		particle->position.z += self->boundingBox.mins.z;
		
		particle->life = Math::rrand(15, 75);
		particle->size = Math::rrand(1, 4);
		
		particle->color.set(0.25, 0.25, 0.25, 1.0);
	}
}

// ================== Collaspable Structures ==================

void structureFallTouch(Entity *other)
{
	if (other == NULL)
	{
		self->health = 0;
		
		self->action = NULL;
		self->velocity.set(0, 0, 0);
		self->rotation.set(0, 0, 0);
		self->touch = NULL;
		
		audio->playSound(SND_LARGE_EXPLOSION, CH_ANY, camera->getSoundDistance(self->position));
			
		//windowShatter();
			
		for (int i = 0 ; i < 35 ; i++)
		{
			throwExplosionDebris(self->position);
		}
		
		self->flags &= ~EF_DEAD;
		self->flags &= ~EF_DYING;
		removeTouchingDecals(self);
	}
}

void structureFallRotate()
{
	switch ((int)self->custom)
	{
		case 0:
			self->rotation.z += (0.5 * engine->getTimeDifference(TD_LOGIC));
			break;
			
		case 1:
			self->rotation.z -= (0.5 * engine->getTimeDifference(TD_LOGIC));
			break;
			
		case 2:
			self->rotation.y -= (0.5 * engine->getTimeDifference(TD_LOGIC));
			self->rotation.z += (0.25 * engine->getTimeDifference(TD_LOGIC));
			break;
			
		case 3:
			self->rotation.y -= (0.5 * engine->getTimeDifference(TD_LOGIC));
			self->rotation.z -= (0.25 * engine->getTimeDifference(TD_LOGIC));
			break;
	}
}

void structureShake()
{
	Structure *structure = (Structure*)self;
	
	structure->custom -= (engine->getTimeDifference(TD_LOGIC));
	
	if (structure->custom > 0)
	{
		structure->position = structure->startPosition;
		
		float shakeAmount;
		
		shakeAmount = Math::rrand(-100, 100);
		shakeAmount /= 100;
		structure->position.x += shakeAmount;
		
		shakeAmount = Math::rrand(-100, 100);
		shakeAmount /= 100;
		structure->position.y += shakeAmount;
	}
	else
	{
		structure->custom = rand() % 4;
		structure->position = structure->startPosition;
		structure->touch = &structureFallTouch;
		structure->action = &structureFallRotate;
		structure->flags &= ~EF_WEIGHTLESS;
		structure->flags &= ~EF_NOCLIP;
	}
}

void structureFall(Entity *other)
{
	if (other != player)
	{
		return;
	}
	
	self->action = &structureShake;
	self->thinkTime = 0;
	self->custom = 50;
	self->touch = NULL;
}

// ================== Supply crates ===========================

WeaponDef *getSupplyCrateWeapon()
{
	static String weapon[25];
	int i = 0;	
	char list[1024];
	
	if (mission->supplyCrateList == "")
	{
		return getRandomPlayerAmmoType();
	}
	
	strcpy(list, mission->supplyCrateList.getText());
	char *token = strtok(list, "|");
	
	while (token != NULL)
	{
		weapon[i] = token;
		
		token = strtok(NULL, "|");
		
		i++;
	}
	
	int id = engine->getValueOfDefine(weapon[rand() % i].getText());
	
	return &entityManager->weaponDef[id];
}

void supplyCrateDie()
{
	Structure *structure = (Structure*)self;
	
	addHelperItems(rand() % 3, getSupplyCrateWeapon(), structure->position);
	
	if (structure->currentItem != NULL)
	{
		structure->currentItem->position = structure->position;
		
		structure->currentItem->setOwner(NULL);

		throwSpawnedItem(structure->currentItem);

		structure->currentItem = NULL;
	}
	
	structure->flags |= EF_DEAD;
	
	removeTouchingDecals(structure);
	
	windowShatter();
	
	audio->playSound(SND_CRATE_DESTROYED, CH_ANY, camera->getSoundDistance(structure->position));
}

// ================== General Structure Think =================

void structureThink()
{
	Structure *structure = (Structure*)self;
	
	if (Math::getDistance(structure->position, structure->destination) < engine->getTimeDifference(TD_LOGIC) * structure->speed)
	{
		structure->position = structure->destination;
		structure->velocity.set(0, 0, 0);
	}
}

Structure *createBaseStructure(Properties *props, int type)
{
	int modelIndex;
	
	sscanf(props->getString("model", "*0"), "%*c%d", &modelIndex);
	
	MDLModel *mdlModel = modelManager->getMDLModel(props->getString("model", "NULL"));
	BSPModel *bspModel = &bsp->models[modelIndex];
	
	if ((mdlModel == NULL) || (bspModel == NULL) || (modelIndex == 0))
	{
		graphics->showErrorAndExit("createBaseStructure() - Bad BSP Model '%s'", props->getString("model", "NULL"));
	}
	
	Structure *structure = entityManager->spawnStructure(type);
	
	structure->model = mdlModel;
	
	Vector v;
	
	v.x = ((mdlModel->maxs.x) - (mdlModel->mins.x)) / 2;
	v.y = ((mdlModel->maxs.y) - (mdlModel->mins.y)) / 2;
	v.z = ((mdlModel->maxs.z) - (mdlModel->mins.z)) / 2;
	
	v.x -= EPSILON;
	v.y -= EPSILON;
	v.z -= EPSILON;
	
	structure->boundingBox.mins -= v;
	structure->boundingBox.maxs += v;
	
	structure->position = mdlModel->originalPosition;
	structure->startPosition = structure->position;
	structure->endPosition = structure->position;
	
	getCluster(structure);
	
	structure->draw = drawStructure;
	
	return structure;
}

void addStructure(Properties *props)
{
	Structure *structure = createBaseStructure(props, ET_STRUCTURE);
	
	structure->health = 1;
	
	structure->flags |= EF_NOCLIP;
	
	if (props->getInt("canBeHurt", 0) == 1)
	{
		structure->flags &= ~EF_IMMORTAL;
	}
	
	structure->action = &structureThink;
	
	structure->load(props);
	
	if (!(structure->flags & EF_IMMORTAL))
	{
		structure->die = destroyableStructurePostDie;
	}
}

void addWeakStructure(Properties *props)
{
	Structure *structure = createBaseStructure(props, ET_WEAK_STRUCTURE);
	
	structure->flags |= EF_NOCLIP;
	
	structure->health = RAND_MAX;
	
	structure->action = &structureThink;
	structure->touch = &structureFall;
	
	structure->load(props);
}

void addWindow(Properties *props)
{
	debug(("addWindow()\n"));
	
	Structure *window = createBaseStructure(props, ET_WINDOW);
	
	window->health = RAND_MAX;
	
	if (!props->hasProperty("toughened"))
	{
		window->flags &= ~EF_IMMORTAL;
		window->health = Math::rrand(3, 5);
	}
	
	window->load(props);
}

void addLift(Properties *props)
{
	debug(("addLift()\n"));
	
	Structure *lift = createBaseStructure(props, ET_LIFT);
	
	lift->name = "Lift";
	
	lift->action = &liftMainThink;
	lift->touch = &liftTouch;
	
	lift->destination = lift->position;
	lift->speed = 0.5;
	
	lift->load(props);
	
	lift->thinkTime = 0;
}

void addAutoLift(Properties *props)
{
	debug(("addAutoLift()\n"));
	
	Structure *lift = createBaseStructure(props, ET_AUTOLIFT);
	
	lift->name = "AutoLift";
	
	lift->custom = props->getInt("waitTime", 300);
	
	lift->action = &liftMainThink;
	lift->touch = &liftTouch;
	
	lift->destination = lift->position;
	lift->speed = 0.5;
	
	lift->load(props);
	
	lift->thinkTime = 0;
}

void addEnergyBarrier(Properties *props)
{
	Structure *barrier = createBaseStructure(props, ET_BARRIER);
	
	barrier->health = 250;
	barrier->custom = 1; // barrier is on
	
	barrier->load(props);
	
	if (barrier->custom == 0)
	{
		barrier->health = 0;
	}
}

void addPushBlock(Properties *props)
{
	Structure *pushBlock = createBaseStructure(props, ET_PUSHBLOCK);
	
	pushBlock->name = "PushBlock";
	pushBlock->health = RAND_MAX;
	
	pushBlock->flags &= ~EF_NOCLIP;
	pushBlock->flags &= ~EF_NOWORLDCLIP;
	pushBlock->flags &= ~EF_WEIGHTLESS;
	pushBlock->flags |= EF_PUSHABLE;
	
	pushBlock->load(props);
}

void addWeakWall(Properties *props)
{
	debug(("addWeakWall()\n"));
	
	Structure *wall = createBaseStructure(props, ET_WEAK_WALL);
	
	wall->health = RAND_MAX;
	wall->name = "WeakWall";
	
	wall->action = &weakWallLookForPlayer;
	wall->die = &destroyableStructureDie;
	
	wall->load(props);
	
	wall->thinkTime = 0;
}

void addSupplyCrate(Properties *props)
{
	Structure *crate = createBaseStructure(props, ET_SUPPLY_CRATE);
	
	crate->health = Math::rrand(1, 2);
	crate->name = "SupplyCrate";
	
	crate->flags &= ~EF_NOCLIP;
	crate->flags &= ~EF_NOWORLDCLIP;
	crate->flags &= ~EF_WEIGHTLESS;
	crate->flags &= ~EF_IMMORTAL;
	
	crate->die = &supplyCrateDie;
	
	crate->load(props);
	
	crate->thinkTime = 0;
}

void doBasicStructure()
{
	Structure *structure = (Structure*)self;
	
	//self->riding = NULL;
	
	if (withinViewableRange(structure))
	{
		entityManager->addEntityToDrawList(structure);
	}
	
	if ((structure->active) || (!(structure->flags & EF_WEIGHTLESS)))
	{
		structure->applyGravity(engine->getTimeDifference(TD_LOGIC));
		
		moveEntity(structure);
		
		if ((structure->position.z < bsp->minCords.z) && (!(structure->flags & EF_WEIGHTLESS)))
		{
			printf("ERROR: Structure '%s' fell out of map\n", self->getName());
			exit(1);
		}
	}
	
	if (structure->flags & EF_PUSHABLE)
	{
		for (int i = 0 ; i < engine->getInertiaRate() ; i++)
		{
			structure->velocity.x = structure->velocity.x * 0.9;
			structure->velocity.y = structure->velocity.y * 0.9;
		}
		
		if (game->cutsceneType != CUTSCENE_NONE)
		{
			structure->velocity.x = structure->velocity.y = 0;
		}
		
		if ((structure->velocity.x > -EPSILON) && (structure->velocity.x < EPSILON))
		{
			structure->velocity.x = 0;
		}
	
		if ((structure->velocity.y > -EPSILON) && (structure->velocity.y < EPSILON))
		{
			structure->velocity.y = 0;
		}
		
		if (tracer->outputContent & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
		{
			Math::limit(&structure->velocity.x, -2, 2);
			Math::limit(&structure->velocity.y, -2, 2);
			Math::limit(&structure->velocity.z, -2, 2);
		}
	}
	
	if (structure->thinkTime == 0)
	{
		if (structure->action != NULL)
		{
			structure->action();
		}
	}
}

void destroyStructure(const char *name)
{
	debug(("destroyStructure() - %s\n", name));
	
	Entity *oldSelf;
	
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		if ((entity->name != name) || (entity->entityType != ET_WEAK_WALL))
		{
			continue;
		}
		
		oldSelf = self;
		self = entity;
		weakWallExplode();
		self = oldSelf;
	}
}

void doStructures()
{
	for (Entity *entity = (Entity*)entityManager->structureList.getFirstElement() ; entity != NULL ; entity = (Entity*)entity->next)
	{
		self = entity;
		
		bbManager->removeBox(self);
		
		if (self->flags & EF_DEAD)
		{
			if (!self->referenced)
			{
				entity = (Entity*)entity->previous;
				entityManager->structureList.remove(entity->next);
			}
			else
			{
				self->referenced = false;
			}
			
			continue;
		}
		
		if (self->owner != NULL)
		{
			self->owner->referenced = true;
		}
		
		self->referenced = false;
		
		self->thinkTime -= engine->getTimeDifference(TD_LOGIC);
		Math::limit(&self->thinkTime, 0, RAND_MAX);
		
		switch (entity->entityType)
		{
			case ET_DOOR:
				doDoor();
				break;
				
			case ET_LIFT:
			case ET_AUTOLIFT:
				doBasicStructure();
				break;
				
			case ET_SWITCH:
				doSwitch();
				break;
			
			case ET_WINDOW:
				if (entity->health <= 0)
				{
					audio->playSound(Math::rrand(SND_GLASS_SHATTER_1, SND_GLASS_SHATTER_3), CH_ANY, camera->getSoundDistance(self->position));
					windowShatter();
				}
				doBasicStructure();
				game->alterStat("Windows Broken", 1);
				break;
				
			case ET_BARRIER:
				if (entity->custom)
				{
					entity->health += (2 * engine->getTimeDifference(TD_LOGIC));
				}
				else
				{
					entity->health -= (2 * engine->getTimeDifference(TD_LOGIC));
				}
				
				Math::limit(&entity->health, 0, 250);
				
				if (entity->health > 0)
				{
					doBasicStructure();
				}
				else
				{
					continue;
				}
				break;
				
			case ET_STRUCTURE:
			case ET_WEAK_WALL:
			case ET_SUPPLY_CRATE:
				if ((self->health <= 0) && (!(self->flags & EF_DYING)))
				{
					self->action = self->die;
					removeTouchingDecals(self);
				}
				doBasicStructure();
				break;
				
			case ET_WEAK_STRUCTURE:
				if (self->health > 0)
				{
					doBasicStructure();
				}
				break;
				
			case ET_PUSHBLOCK:
				if (!(self->flags & EF_PUSHABLE))
				{
					self->velocity.x = 0;
					self->velocity.y = 0;
				}
				
				if ((self->velocity.x != 0) || ( self->velocity.y != 0))
				{
					addPushBlockSmoke();
				}
				
				doBasicStructure();
				break;
				
			default:
				printf("ERROR - Unknown entity (%d) in structures list!\n", entity->entityType);
				exit(1);
				break;
		}
		
		bbManager->addBox(self);
	}
}
