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

#include "world.h"

bool checkEntityToWorld(Entity *entity)
{
	int max = 0;
	Vector start, end;
	Vector bbMins, bbMaxs, testPosition;

	if (entity->flags & (EF_NOCLIP|EF_NOWORLDCLIP))
	{
		tracer->currentPosition = entity->position;
		tracer->currentPosition += entity->velocity;
		
		tracer->outputEnd = tracer->currentPosition;
		
		return true;
	}
	
	if (fabs(entity->velocity.x) < EPSILON)
	{
		entity->velocity.x = 0;
	}
	
	if (fabs(entity->velocity.y) < EPSILON)
	{
		entity->velocity.y = 0;
	}
	
	tracer->outputFraction = 0;
	tracer->outputContent  = entity->contentType;
	tracer->stepping = 0;
	tracer->stepSize = MAX_STEP_SIZE;
	
	tracer->currentPosition = entity->position;
	
	bbMins = entity->boundingBox.mins;
	bbMaxs = entity->boundingBox.maxs;
	
	bbMins.x -= fabs(entity->velocity.x);
	bbMins.y -= fabs(entity->velocity.y);
	bbMins.z -= fabs(entity->velocity.z);
	
	bbMaxs.x += fabs(entity->velocity.x);
	bbMaxs.y += fabs(entity->velocity.y);
	bbMaxs.z += fabs(entity->velocity.z);
	
	while (tracer->outputFraction < 1 && max < 3)
	{
		start = tracer->currentPosition;

		end = start;
		end += entity->velocity;

		if (entity->velocity.x != 0)
		{
			end.x += (entity->velocity.x > 0 ? EPSILON : -EPSILON);
		}

		if (entity->velocity.y != 0)
		{
			end.y += (entity->velocity.y > 0 ? EPSILON : -EPSILON);
		}

		if (entity->velocity.z != 0)
		{
			end.z += (entity->velocity.z > 0 ? EPSILON : -EPSILON);
		}

		bsp->traceBox(start, end, entity->boundingBox.mins, entity->boundingBox.maxs);

		if (tracer->outputFraction == 1.0f || (1 - tracer->outputFraction <= EPSILON))
		{
			if (entity->velocity.x != 0)
			{
				tracer->outputEnd.x += (entity->velocity.x > 0 ? -EPSILON : EPSILON);
			}

			if (entity->velocity.y != 0)
			{
				tracer->outputEnd.y += (entity->velocity.y > 0 ? -EPSILON : EPSILON);
			}

			if (entity->velocity.z != 0)
			{
				tracer->outputEnd.z += (entity->velocity.z > 0 ? -EPSILON : EPSILON);
			}
		}
		
		if ((tracer->outputAllSolid) && (entity->contentType == CONTENTS_SOLID))
		{
			Trace oldTrace = *tracer;
			
			int contents[6];
			float size;
			
			size = max(entity->boundingBox.maxs.x, entity->boundingBox.maxs.y);
			size = max(entity->boundingBox.maxs.z, size);
			size += EPSILON;
			
			testPosition = oldTrace.currentPosition;
			testPosition.x += size;
			contents[0] = bsp->getContentsAtPosition(testPosition);
			
			testPosition = oldTrace.currentPosition;
			testPosition.x -= size;
			contents[1] = bsp->getContentsAtPosition(testPosition);
			
			testPosition = oldTrace.currentPosition;
			testPosition.y += size;
			contents[2] = bsp->getContentsAtPosition(testPosition);
			
			testPosition = oldTrace.currentPosition;
			testPosition.y -= size;
			contents[3] = bsp->getContentsAtPosition(testPosition);
			
			testPosition = oldTrace.currentPosition;
			testPosition.z += size;
			contents[4] = bsp->getContentsAtPosition(testPosition);
			
			testPosition = oldTrace.currentPosition;
			testPosition.z -= size;
			contents[5] = bsp->getContentsAtPosition(testPosition);
			
			bool stuckInSolid = true;
			
			for (int i = 0 ; i < 6 ; i++)
			{
				if (contents[i] != CONTENTS_SOLID)
				{
					stuckInSolid = false;
				}
			}
			
			if (stuckInSolid)
			{
				return false;
			}
			
			*tracer = oldTrace;
			
			tracer->outputAllSolid = 0;
		}
		
		tracer->currentPosition = tracer->outputEnd;

		if (tracer->outputFraction == 1.0f || (1 - tracer->outputFraction <= EPSILON))
		{
			return true;
		}
		
		if (tracer->outputNormal->z > 0.7 || entity->riding != NULL)
		{
			entity->flags |= EF_ONGROUND;
		}
		else if (tracer->outputNormal->z > 0.1)
		{
			Math::limit(&entity->realVelocity.z, (FALL_DAMAGE_ACCEL + 0.25), 11);
		}
		
		if ((entity != camera) && (!(entity->flags & EF_WEIGHTLESS)))
		{
			if (((entity->flags & EF_ONGROUND) && (tracer->outputNormal->z == 0)) && ((tracer->outputNormal->x != 0 || tracer->outputNormal->y != 0)))
			{
				if (entity->entityType == ET_BLOB || entity->entityType == ET_BIOMECH || entity->entityType == ET_BOSS)
				{
					if (tracer->stepping == 0)
					{
						tracer->currentPosition.z += tracer->stepSize;
						tracer->stepping = 1;
						continue;
					}
					else
					{
						tracer->currentPosition.z -= tracer->stepSize;
						tracer->stepping = 0;
					}
				}
			}
		}
		
		if ((entity->flags & EF_BOUNCES) || tracer->outputNormal->z < 0)
		{
			entity->realVelocity = Geometry::responseBounce(entity->realVelocity, *tracer->outputNormal, entity->elasticity);
			entity->velocity = entity->realVelocity;
			entity->prepareToMove(Engine::getInstance()->getTimeDifference(TD_LOGIC));
		}
		else
		{
			entity->velocity = Geometry::responseSlide(Geometry::multiplyVectorByScaler(entity->velocity, 1 - tracer->outputFraction), *tracer->outputNormal);
			
			if (tracer->outputNormal->z < 0)
			{
				entity->velocity.x = entity->velocity.y = 0;
			}
		}
		
		if (entity->touch != NULL)
		{
			entity->touch(NULL);
		}

		max++;
	}

	return false;
}

bool checkEntityToOthers(Entity *entity)
{
	int j, startsOut, endsOut, planeIndex = 0, max, misses;
	float x1, y1, z1, h1, w1, l1, x2, y2, z2, h2, w2, l2;
	float startFraction, endFraction, startDistance, endDistance, fraction;
	Vector inputStart, inputEnd, offset, v;
	int sanity, c;
	bool rtn = true;
	Entity *box;
	
	Entity *oldSelf;
	Trace currentTracer;
	
	tracer->stepSize = MAX_STEP_SIZE;
	
	x1 = tracer->currentPosition.x + entity->boundingBox.mins.x;
	y1 = tracer->currentPosition.y + entity->boundingBox.mins.y;
	z1 = tracer->currentPosition.z + entity->boundingBox.mins.z;
	w1 = fabs(entity->boundingBox.mins.x) + fabs(entity->boundingBox.maxs.x);
	h1 = fabs(entity->boundingBox.mins.y) + fabs(entity->boundingBox.maxs.y);
	l1 = fabs(entity->boundingBox.mins.z) + fabs(entity->boundingBox.maxs.z);
	
	if ((entity->flags & EF_ONGROUND) && (entity->entityType == ET_BLOB || entity->entityType == ET_BIOMECH || entity->entityType == ET_BOSS))
	{
		z1 = tracer->currentPosition.z + entity->boundingBox.mins.z + tracer->stepSize;
	}
	
	c = bbManager->initCollisionCandidates(entity);
	sanity = 0;
	
	for (int i = 0 ; i < c ; i++)
	{
		box = bbManager->candidate[i];
		
		if (sanity++ >= 1024)
		{
			printf("WARNING: '%s' - bbManager->getNext() in infinite loop!\n", entity->getName());
			bbManager->clearCollisionCandidates();
			return false;
		}
		
		#if DEV
		if (box->name.getText() == NULL)
		{
			printf("ERROR: checkEntityToOthers: memory corruption detection\n");
			abort();
		}
		#endif
		
		if ((entity == box) || (box->boundingBox.boxType == BOX_HOLLOW) || (box->flags & EF_VANISHED))
		{
			continue;
		}
		
		if ((entity->owner == box) || (box->owner == entity))
		{
			continue;
		}
		
		x2 = box->position.x + box->boundingBox.mins.x;
		y2 = box->position.y + box->boundingBox.mins.y;
		z2 = box->position.z + box->boundingBox.mins.z;
		w2 = fabs(box->boundingBox.mins.x) + fabs(box->boundingBox.maxs.x);
		h2 = fabs(box->boundingBox.mins.y) + fabs(box->boundingBox.maxs.y);
		l2 = fabs(box->boundingBox.mins.z) + fabs(box->boundingBox.maxs.z);

		max = 0;

		while (Collision::collision(x1, y1, z1, w1, h1, l1, x2, y2, z2, w2, h2, l2) == true && max < 10)
		{
			rtn = true;
			
			if (max == 0)
			{
				if (entity->touch != NULL)
				{
					entity->touch(box);
				}
				
				if (box->touch != NULL)
				{
					oldSelf = self;
					self = box;
					box->touch(entity);
					self = oldSelf;
				}
			}
			
			if (entity->flags & EF_NOCLIP)
			{
				break;
			}
			
			inputStart = entity->position;
			inputEnd   = tracer->currentPosition;
			
			if (entity->velocity.x != 0)
			{
				inputEnd.x += (entity->velocity.x > 0 ? EPSILON : -EPSILON);
			}

			if (entity->velocity.y != 0)
			{
				inputEnd.y += (entity->velocity.y > 0 ? EPSILON : -EPSILON);
			}

			if (entity->velocity.z != 0)
			{
				inputEnd.z += (entity->velocity.z > 0 ? EPSILON : -EPSILON);
			}

			tracer->traceType      = BOX;
			tracer->traceMins      = entity->boundingBox.mins;
			tracer->traceMaxs      = entity->boundingBox.maxs;
			tracer->traceExtents.x = -tracer->traceMins.x > tracer->traceMaxs.x ? -tracer->traceMins.x : tracer->traceMaxs.x;
			tracer->traceExtents.y = -tracer->traceMins.y > tracer->traceMaxs.y ? -tracer->traceMins.y : tracer->traceMaxs.y;
			tracer->traceExtents.z = -tracer->traceMins.z > tracer->traceMaxs.z ? -tracer->traceMins.z : tracer->traceMaxs.z;

			tracer->outputStartsOut = 1;
			tracer->outputAllSolid  = 0;
			tracer->outputFraction  = 1.0f;

			startFraction = -1.0f;
			endFraction   = 1.0f;
			startsOut     = 0;
			endsOut       = 0;

			misses = 0;
			
			for (j=0;j<6;j++)
			{
				offset.x = box->boundingBox.plane[j].normal.x < 0 ? tracer->traceMaxs.x : tracer->traceMins.x;
				offset.y = box->boundingBox.plane[j].normal.y < 0 ? tracer->traceMaxs.y : tracer->traceMins.y;
				offset.z = box->boundingBox.plane[j].normal.z < 0 ? tracer->traceMaxs.z : tracer->traceMins.z;

				startDistance  = (inputStart.x + offset.x) * box->boundingBox.plane[j].normal.x;
				startDistance += (inputStart.y + offset.y) * box->boundingBox.plane[j].normal.y;
				startDistance += (inputStart.z + offset.z) * box->boundingBox.plane[j].normal.z;
				startDistance += box->boundingBox.plane[j].d;

				endDistance    = (inputEnd.x   + offset.x) * box->boundingBox.plane[j].normal.x;
				endDistance   += (inputEnd.y   + offset.y) * box->boundingBox.plane[j].normal.y;
				endDistance   += (inputEnd.z   + offset.z) * box->boundingBox.plane[j].normal.z;
				endDistance   += box->boundingBox.plane[j].d;

				if (startDistance > 0)
				{
					startsOut = 1;
				}

				if (endDistance > 0)
				{
					endsOut = 1;
				}

				/* Make sure the trace isn't completely on one side of the brush */

				if (startDistance > 0 && endDistance > 0) /* Both are in front of the plane, its outside of this brush */
				{
					misses++;
					continue;
				}

				if (startDistance <= 0 && endDistance <= 0) /* Both are behind this plane, it will get clipped by another one */
				{
					misses++;
					continue;
				}

				if (startDistance > endDistance) /* Line is entering into the brush */
				{
					fraction = (startDistance - EPSILON) / (startDistance - endDistance);

					if (fraction > startFraction)
					{
						planeIndex    = j;
						startFraction = fraction;

						v.x = inputStart.x + startFraction * (inputEnd.x - inputStart.x);
						v.y = inputStart.y + startFraction * (inputEnd.y - inputStart.y);
						v.z = inputStart.z + startFraction * (inputEnd.z - inputStart.z);
					}
				}

				else /* Line is leaving the brush */
				{
					fraction = (startDistance + EPSILON) / (startDistance - endDistance);

					if (fraction < endFraction)
					{
						endFraction = fraction;
					}
				}
			}

			if (misses == 6)
			{
				//debug(("CBoundingBoxManager: Did not collide - %s -> %s\n", entity->getName(), box->getName()));
				break;
			}

			if (startsOut == 0)
			{
				if (endsOut == 0)
				{
					tracer->outputAllSolid = 1;
				}

				break;
			}

			if (startFraction < endFraction)
			{
				if (startFraction > -1 && startFraction < tracer->outputFraction)
				{
					if (startFraction < 0)
					{
						startFraction = 0;
					}

					tracer->outputFraction = startFraction;
					tracer->outputNormal   = &box->boundingBox.plane[planeIndex].normal;
				}
			}

			tracer->outputEnd.x = inputStart.x + tracer->outputFraction * (inputEnd.x - inputStart.x);
			tracer->outputEnd.y = inputStart.y + tracer->outputFraction * (inputEnd.y - inputStart.y);
			tracer->outputEnd.z = inputStart.z + tracer->outputFraction * (inputEnd.z - inputStart.z);
			
			if (tracer->outputFraction == 1.0f)
			{
				if (entity->velocity.x != 0)
				{
					tracer->outputEnd.x += (entity->velocity.x > 0 ? -EPSILON : EPSILON);
				}

				if (entity->velocity.y != 0)
				{
					tracer->outputEnd.y += (entity->velocity.y > 0 ? -EPSILON : EPSILON);
				}

				if (entity->velocity.z != 0)
				{
					tracer->outputEnd.z += (entity->velocity.z > 0 ? -EPSILON : EPSILON);
				}
			}

			tracer->currentPosition = tracer->outputEnd;
			
			if (tracer->outputNormal->z > 0)
			{
				entity->flags |= EF_ONGROUND;
				
				entity->riding = box;
			}
			
			if (box->flags & EF_PUSHABLE)
			{
				oldSelf = self;
				
				currentTracer = *tracer;
				
				self = box;
					
				self->velocity = *tracer->outputNormal;
				
				self->velocity = self->velocity * -1;
					
				self->velocity.x *= fabs(oldSelf->realVelocity.x);
				self->velocity.y *= fabs(oldSelf->realVelocity.y);
				//self->velocity.z *= fabs(oldSelf->realVelocity.z);
				
				self->velocity = self->velocity * entityManager->getEntityStrength(entity);
				
				checkEntityToWorld(self);
					
				*tracer = currentTracer;
					
				self = oldSelf;
			}
			
			if (entity->flags & EF_BOUNCES)
			{
				entity->realVelocity = Geometry::responseBounce(entity->realVelocity, *tracer->outputNormal, entity->elasticity);
				entity->velocity = entity->realVelocity;
				entity->prepareToMove(Engine::getInstance()->getTimeDifference(TD_LOGIC));
			}
			else
			{
				entity->velocity = Geometry::responseSlide(Geometry::multiplyVectorByScaler(entity->velocity, 1 - tracer->outputFraction), *tracer->outputNormal);
			}
			
			max++;
			
			tracer->currentPosition += entity->velocity;

			x1 = tracer->currentPosition.x + entity->boundingBox.mins.x;
			y1 = tracer->currentPosition.y + entity->boundingBox.mins.y;
			z1 = tracer->currentPosition.z + entity->boundingBox.mins.z;
			w1 = fabs(entity->boundingBox.mins.x) + fabs(entity->boundingBox.maxs.x);
			h1 = fabs(entity->boundingBox.mins.y) + fabs(entity->boundingBox.maxs.y);
			l1 = fabs(entity->boundingBox.mins.z) + fabs(entity->boundingBox.maxs.z);

			entity->updateBoundingBox();

			i = 0;
		}

		if (max >= 10)
		{
			printf("WARNING: %s Recursed too much\n", entity->getName());
			bbManager->clearCollisionCandidates();
			return false;
		}
	}
	
	bbManager->clearCollisionCandidates();
	
	return rtn;
}

void doEntityRideObject(Entity *entity)
{
	if (entity->riding == NULL)
	{
		return;
	}
	
	if ((entity->entityType == ET_ITEM) || (entity->entityType == ET_DECORATION) || (entity->entityType == ET_WEAPON))
	{
		switch (entity->riding->entityType)
		{
			case ET_BLOB:
			case ET_BIOMECH:
			case ET_NEUTRAL:
			case ET_WEAPON:
			case ET_ITEM:
			case ET_BULLET:
			case ET_BOSS:
				return;
				break;
				
			default:
				break;
		}
	}
	
	// don't hold on to the entity if it's moving up!
	if (entity->velocity.z <= 0)
	{
		entity->velocity.z = 0;
		entity->position.z = entity->riding->position.z;
		entity->position.z += entity->riding->boundingBox.maxs.z;
		entity->position.z += entity->boundingBox.maxs.z;
		entity->position.z += EPSILON * 3;
		
		/*
		if (entity->riding->velocity.z > 0)
		{
			entity->position.z += (entity->riding->velocity.z * engine->getTimeDifference(TD_LOGIC));
		}
		*/
	}
	
	entity->flags |= EF_ONGROUND;
}

void doEntityGroundTrace(Entity *entity)
{
	Vector start, end;
	
	start = entity->position;
	start.z += entity->boundingBox.mins.z;
	
	end = entity->position;
	end.z += entity->boundingBox.mins.z;
	end.z -= SHADOW_LENGTH;
	
	bsp->traceRay(start, end);
	
	entity->heightAboveGround = tracer->outputEnd.z;
	
	if (entity->flags & EF_WEIGHTLESS)
	{
		return;
	}
	
	if (entity->velocity.z > 0)
	{
		return;
	}
	
	start = entity->position;
	start.z += entity->boundingBox.mins.z;
	
	end = entity->position;
	end.z += entity->boundingBox.mins.z;
	end.z -= 0.5;
	
	if (bbManager->canSightSolid(entity, start, end))
	{
		if (bbManager->intersectedEntity != NULL && bbManager->intersectedEntity->position.z < entity->position.z)
		{
			entity->riding = bbManager->intersectedEntity;
			
			entity->heightAboveGround = SHADOW_LENGTH - (bbManager->rayPercentage * SHADOW_LENGTH);
			
			if (entity->riding->touch != NULL)
			{
				Entity *oldSelf = self;
				self = entity->riding;
				self->touch(entity);
				self = oldSelf;
				
				entity->flags |= EF_ONGROUND;
			}
		}
	}
}

void correctAllSolid(Entity *entity)
{
	float i, j, k;
	Vector start, end;
	
	for (k=0;k<=2;k+=0.5)
	{
		start = entity->position;
		
		start.z += k;
		
		end = entity->position;
		
		tracer->currentPosition = start;
		tracer->outputFraction = 0;
		tracer->outputContent = entity->contentType;
		tracer->stepping = 0;
		
		bsp->traceBox(start, end, entity->boundingBox.mins, entity->boundingBox.maxs);
		
		if (tracer->outputAllSolid == 0)
		{
			entity->position = tracer->currentPosition;
			
			entity->flags |= EF_ONGROUND;
			
			return;
		}
	}
	
	for (j=-2;j<=2;j+=1)
	{
		start = entity->position;
		
		start.y += j;
			
		end = entity->position;
			
		tracer->currentPosition = start;
		tracer->outputFraction = 0;
		tracer->outputContent = entity->contentType;
		tracer->stepping = 0;
			
		bsp->traceBox(start, end, entity->boundingBox.mins, entity->boundingBox.maxs);
			
		if (tracer->outputAllSolid == 0)
		{
			entity->position = tracer->currentPosition;
				
			entity->flags |= EF_ONGROUND;
				
			return;
		}
	}
	
	for (i=-2;i<=2;i+=1)
	{
		start = entity->position;
		
		start.x += i;
		
		end = entity->position;
		
		tracer->currentPosition = start;
		tracer->outputFraction = 0;
		tracer->outputContent = entity->contentType;
		tracer->stepping = 0;
		
		bsp->traceBox(start, end, entity->boundingBox.mins, entity->boundingBox.maxs);
		
		if (tracer->outputAllSolid == 0)
		{
			entity->position = tracer->currentPosition;
			
			entity->flags |= EF_ONGROUND;
			
			return;
		}
	}
	
	/* If something like a bullet or rubble is stuck then just kill it */
	
	if (strcmp(entity->getName(), "_dec") == 0 || strcmp(entity->getName(), "_bul") == 0)
	{
		entity->health = -50;
	}
}

void clipToFloor(Entity *entity)
{
	Vector start, end;
	
	start = entity->position;
	
	/* Push start up a bit to stop entity getting jammed in a slope */
	
	start.z += 1;
	
	end = entity->position;
	end.z -= MAX_STEP_SIZE;
	
	bsp->traceBox(start, end, entity->boundingBox.mins, entity->boundingBox.maxs);
	
	entity->position = tracer->outputEnd;
	
	entity->position.z += EPSILON;
}

void moveEntity(Entity *entity)
{
	bool doFullCollision = true;
	
	if ((entity->velocity.x == 0) && (entity->velocity.y == 0) && (entity->velocity.z == 0))
	{
		doFullCollision = (!(entity->flags & EF_WEIGHTLESS));
	}
	
	if (doFullCollision)
	{
		tracer->treatAllAsSolid = false;
		tracer->stepping = 0;
		
		entity->flags &= ~EF_ONGROUND;
		
		entity->oldPosition = entity->position;
		
		float moveSize = engine->getTimeDifference(TD_LOGIC);
		
		while (moveSize >= 1)
		{
			entity->prepareToMove(1);
			
			if (checkEntityToWorld(entity) == true)
			{
				entity->riding = NULL;
				
				if (checkEntityToOthers(entity) == true)
				{
					entity->position = tracer->currentPosition;
					
					if (tracer->stepping)
					{
						clipToFloor(entity);
					}
					
					entity->velocity = entity->realVelocity;
					
					switch (entity->entityType)
					{
						case ET_BLOB:
						case ET_BIOMECH:
							unitContentChanged((Unit*)entity);
							break;
							
						default:
							entity->contentType = tracer->outputContent;
							break;
					}
				}
				
				else
				{
					entity->flags |= EF_ONGROUND;
				}
			}
			
			else
			{
				correctAllSolid(entity);
			}
			
			moveSize--;
		}
		
		if (moveSize > 0)
		{
			entity->prepareToMove(moveSize);
			
			if (checkEntityToWorld(entity) == true)
			{
				entity->riding = NULL;
				
				if (checkEntityToOthers(entity) == true)
				{
					entity->position = tracer->currentPosition;
					
					if (tracer->stepping)
					{
						clipToFloor(entity);
					}
					
					entity->velocity = entity->realVelocity;
					
					switch (entity->entityType)
					{
						case ET_BLOB:
						case ET_BIOMECH:
							unitContentChanged((Unit*)entity);
							break;
							
						default:
							entity->contentType = tracer->outputContent;
							break;
					}
				}
				
				else
				{
					entity->flags |= EF_ONGROUND;
				}
			}
			
			else
			{
				correctAllSolid(entity);
			}
		}
	}
	
	doEntityGroundTrace(entity);
	doEntityRideObject(entity);
		
	if (doFullCollision)
	{
		if ((entity->flags & EF_ONGROUND) && (!(entity->flags & EF_BOUNCES) && (!(entity->flags & EF_WEIGHTLESS))))
		{
			entity->velocity.z = 0;
		}
		
		switch (entity->entityType)
		{
			case ET_BLOB:
			case ET_BIOMECH:
			case ET_NEUTRAL:
			case ET_LIFT:
			case ET_PUSHBLOCK:
			case ET_AUTOLIFT:
			case ET_BOSS:
			case ET_CAMERA:
			case ET_STRUCTURE:
				getCluster(entity);
				break;
			
			case ET_DECORATION:
			case ET_ITEM:
			case ET_WEAPON:
			case ET_SUPPLY_CRATE:
				if (!(entity->flags & EF_ONGROUND))
				{
					getCluster(entity);
				}
				break;
				
			case ET_BULLET:
			case ET_WEAK_STRUCTURE:
				entity->cluster = -1;
				break;
				
			default:
				#if DEV
				printf("Don't check this entity type's cluster! - %d\n", entity->entityType);
				exit(1);
				#endif
				break;
		}
	}
}
