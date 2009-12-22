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

BoundingBoxManager::BoundingBoxManager()
{
	for (int i = 0 ; i < MAX_CANDIDATES ; i++)
	{
		candidate[i] = NULL;
	}
	
	stationary.set(0, 0, 0);
}

BoundingBoxManager::~BoundingBoxManager()
{
	clear();
}

void BoundingBoxManager::clear()
{
	for (int x = 0 ; x < GRID_SIZE ; x++)
	{
		for (int y = 0 ; y < GRID_SIZE ; y++)
		{
			cell[x][y].clear();
		}
	}
	
	for (int i = 0 ; i < MAX_CANDIDATES ; i++)
	{
		candidate[i] = NULL;
	}
}

BoundingBoxManager *BoundingBoxManager::getInstance()
{
	return &instance;
}

void BoundingBoxManager::construct(Vector mins, Vector maxs)
{
	int width = (int)fabs(mins.x - maxs.x);
	int height = (int)fabs(mins.y - maxs.y);
	
	minX = fabs(mins.x);
	minY = fabs(mins.y);
	
	cellSize = max(width, height);
	cellSize /= GRID_SIZE;
	
	currentCandidate = 0;
	
	for (int x = 0 ; x < GRID_SIZE ; x++)
	{
		for (int y = 0 ; y < GRID_SIZE ; y++)
		{
			cell[x][y].clear();
		}
	}
	
	debug(("BoundingBoxManager::construct() - Cell Size = %f\n", cellSize));
}

// This one is used only for entity collisions...
void BoundingBoxManager::getRequiredGridPoints(Entity *entity, Vector velocity)
{
	float x = entity->position.x + minX;
	float y = entity->position.y + minY;
	
	int cellX1 = (int)((x + entity->boundingBox.mins.x) / cellSize);
	int cellY1 = (int)((y + entity->boundingBox.mins.y) / cellSize);
	int cellX2 = (int)((x + entity->boundingBox.maxs.x) / cellSize);
	int cellY2 = (int)((y + entity->boundingBox.maxs.y) / cellSize);
	
	x = entity->position.x + minX + velocity.x;
	y = entity->position.y + minY + velocity.y;
	
	int cellX3 = (int)((x + entity->boundingBox.mins.x) / cellSize);
	int cellY3 = (int)((y + entity->boundingBox.mins.y) / cellSize);
	int cellX4 = (int)((x + entity->boundingBox.maxs.x) / cellSize);
	int cellY4 = (int)((y + entity->boundingBox.maxs.y) / cellSize);
	
	cellX1 = min(cellX1, cellX3);
	cellY1 = min(cellY1, cellY3);
	cellX2 = max(cellX2, cellX4);
	cellY2 = max(cellY2, cellY4);
	
	Math::limit(&cellX1, 0, GRID_SIZE - 1);
	Math::limit(&cellY1, 0, GRID_SIZE - 1);
	Math::limit(&cellX2, 0, GRID_SIZE - 1);
	Math::limit(&cellY2, 0, GRID_SIZE - 1);
	
	// increment the cells to ensure that the for loop works...
	occupiedRect.x = cellX1;
	occupiedRect.y = cellY1;
	occupiedRect.w = ++cellX2;
	occupiedRect.h = ++cellY2;
}

void BoundingBoxManager::getRequiredGridPoints(Vector position1, Vector position2)
{
	float x1 = position1.x + minX;
	float y1 = position1.y + minY;
	
	float x2 = position2.x + minX;
	float y2 = position2.y + minY;
	
	float minX = min(x1, x2);
	float maxX = max(x1, x2);
	float minY = min(y1, y2);
	float maxY = max(y1, y2);
	
	int cellX1 = (int)(minX / cellSize);
	int cellY1 = (int)(minY / cellSize);
	int cellX2 = (int)(maxX / cellSize);
	int cellY2 = (int)(maxY / cellSize);
	
	Math::limit(&cellX1, 0, GRID_SIZE - 1);
	Math::limit(&cellY1, 0, GRID_SIZE - 1);
	Math::limit(&cellX2, 0, GRID_SIZE - 1);
	Math::limit(&cellY2, 0, GRID_SIZE - 1);
	
	// increment the cells to ensure that the for loop works...
	occupiedRect.x = cellX1;
	occupiedRect.y = cellY1;
	occupiedRect.w = ++cellX2;
	occupiedRect.h = ++cellY2;
}

void BoundingBoxManager::getRequiredGridPoints(Entity *entity1, Entity *entity2)
{
	getRequiredGridPoints(entity1->position, entity2->position);
}

void BoundingBoxManager::addBox(Entity *entity)
{
	entity->updateBoundingBox();
	
	getRequiredGridPoints(entity, stationary);
	
	List *list;
	Reference *ref;
	
	entity->occupiedRect = occupiedRect;
	
	for (int x = occupiedRect.x ; x < occupiedRect.w ; x++)
	{
		for (int y = occupiedRect.y ; y < occupiedRect.h ; y++)
		{
			ref = new Reference();
			ref->object = entity;
			
			list = &cell[x][y];
			list->add(ref);
		}
	}
}

void BoundingBoxManager::removeBox(Entity *entity)
{
	List *list;
	Reference *ref;
	
	for (int x = entity->occupiedRect.x ; x < entity->occupiedRect.w ; x++)
	{
		for (int y = entity->occupiedRect.y ; y < entity->occupiedRect.h ; y++)
		{
			list = &cell[x][y];
			
			for (ref = (Reference*)list->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
			{
				if (ref->object == entity)
				{
					ref = (Reference*)ref->previous;
					list->remove(ref->next);
				}
			}
		}
	}
}

int BoundingBoxManager::initCollisionCandidates()
{
	currentCandidate = 0;
	int c = 0;
	
	Entity *collisionCandidate;
	Reference *ref;
	List *list;
	
	for (int x = occupiedRect.x ; x < occupiedRect.w ; x++)
	{
		for (int y = occupiedRect.y ; y < occupiedRect.h ; y++)
		{
			list = &cell[x][y];
			
			for (ref = (Reference*)list->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
			{
				collisionCandidate = (Entity*)ref->object;
				
				if (collisionCandidate->inCollisionCandidateList)
				{
					continue;
				}
				
				//printf("Adding: %d - '%s'\n", c, collisionCandidate->getName());
				
				collisionCandidate->inCollisionCandidateList = true;
				candidate[c++] = collisionCandidate;
		    
				if (c >= MAX_CANDIDATES / 4)
				{
					if (c == MAX_CANDIDATES)
					{
						printf("ERROR: Maximum number of collision candidates (%d) exceeded!!\n", MAX_CANDIDATES);
						exit(1);
					}
			
					printf("WARNING: Checking against %d candidates! (%d maximum)\n", c, MAX_CANDIDATES);
				}
			}
		}
	}
	
	return c;
}

int BoundingBoxManager::initCollisionCandidates(Entity *entity)
{
	getRequiredGridPoints(entity, entity->velocity);
	
	return initCollisionCandidates();
}

int BoundingBoxManager::initCollisionCandidates(Entity *entity1, Entity *entity2)
{
	getRequiredGridPoints(entity1, entity2);
	
	return initCollisionCandidates();
}

int BoundingBoxManager::initCollisionCandidates(Vector position1, Vector position2)
{
	getRequiredGridPoints(position1, position2);
	
	return initCollisionCandidates();
}

void BoundingBoxManager::clearCollisionCandidates()
{
	for (int i = 0 ; i < MAX_CANDIDATES ; i++)
	{
		if (candidate[i] == NULL)
		{
			// don't waste time...
			return;
		}
	    
		candidate[i]->inCollisionCandidateList = false;
		candidate[i] = NULL;
	}
}

bool BoundingBoxManager::isValidSightTarget(int entityType)
{
	switch (entityType)
	{
		case ET_WEAPON:
		case ET_ITEM:
		case ET_BULLET:
		case ET_FEATURE:
		case ET_DECORATION:
			return false;
			break;
				
		default:
			return true;
			break;
	}
	
	return false;
}

bool BoundingBoxManager::canSightTarget(Entity *observer, Entity *target)
{
	int c = initCollisionCandidates(observer, target);
	
	intersectedEntity = NULL;
	
	Entity *box;
	
	rayPercentage = 1;

	for (int i = 0 ; i < c ; i++)
	{
		box = candidate[i];
		
		if ((box == observer) || (box == target))
		{
			continue;
		}
		
		if (!isValidSightTarget(box->entityType))
		{
			continue;
		}
		
		if (lineIntersectBox(box, observer->position, target->position))
		{
			clearCollisionCandidates();
			return false;
		}
	}
	
	clearCollisionCandidates();
	
	return true;
}

bool BoundingBoxManager::canSightSolid(Entity *observer, Vector startPosition, Vector target)
{
	getRequiredGridPoints(startPosition, target);
	
	intersectedEntity = NULL;
	
	int c = initCollisionCandidates();
	
	Entity *box = NULL;
	
	rayPercentage = 1;
	
	for (int i = 0 ; i < c ; i++)
	{
		box = candidate[i];
		
		if (box == observer)
		{
			continue;
		}
		
		if (!isValidSightTarget(box->entityType))
		{
			continue;
		}
		
		if (lineIntersectBox(box, startPosition, target))
		{
			clearCollisionCandidates();
			return true;
		}
	}
	
	clearCollisionCandidates();
	
	return false;
}

Vector BoundingBoxManager::getTraceLocation(Entity *observer, Vector startPosition, Vector target)
{
	getRequiredGridPoints(startPosition, target);
	
	intersectedEntity = NULL;
	
	int c = initCollisionCandidates();
	
	Entity *box = NULL;
	
	rayPercentage = 1;
	
	for (int i = 0 ; i < c ; i++)
	{
		box = candidate[i];
		
		if (box == observer)
		{
			continue;
		}
		
		if (!isValidSightTarget(box->entityType))
		{
			continue;
		}
		
		if (lineIntersectBox(box, startPosition, target))
		{
			target -= startPosition;
			target = target * rayPercentage;
			startPosition += target;
			
			clearCollisionCandidates();
			
			return startPosition;
		}
	}
	
	clearCollisionCandidates();
	
	return target;
}

bool BoundingBoxManager::lineIntersectBox(Entity *e, Vector start, Vector end)
{
	char inside = 1, quadrant[3];
	int i, whichPlane;
	double maxT[3], candidatePlane[3], origin[3], minB[3], maxB[3], dir[3], coord[3], percentage;
	Vector v;
	
	v = end;
	v -= start;
	v.normalize();
	
	dir[0] = v.x;
	dir[1] = v.y;
	dir[2] = v.z;
	
	origin[0] = start.x;
	origin[1] = start.y;
	origin[2] = start.z;
	
	minB[0] = e->position.x + e->boundingBox.mins.x;
	minB[1] = e->position.y + e->boundingBox.mins.y;
	minB[2] = e->position.z + e->boundingBox.mins.z;
	
	maxB[0] = e->position.x + e->boundingBox.maxs.x;
	maxB[1] = e->position.y + e->boundingBox.maxs.y;
	maxB[2] = e->position.z + e->boundingBox.maxs.z;
	
	/* Find candidate planes */
	
	for (i=0;i<3;i++)
	{
		if (origin[i] < minB[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = minB[i];
			inside = 0;
		}
		
		else if (origin[i] > maxB[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = maxB[i];
			inside = 0;
		}
		
		else
		{
			quadrant[i] = 2;
		}
	}

	/* Ray origin inside bounding box */
	
	if (inside == 1)
	{
		coord[0] = origin[0];
		coord[1] = origin[1];
		coord[2] = origin[2];
		
		intersectedEntity = e;
		
		return true;
	}

	/* Calculate T distances to candidate planes */
	
	for (i = 0; i < 3; i++)
	{
		if (quadrant[i] != 2 && dir[i] != 0)
		{
			maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
		}
		
		else
		{
			maxT[i] = -1.;
		}
	}

	/* Get largest of the maxT's for final choice of intersection */
	
	whichPlane = 0;
	
	for (i=1;i<3;i++)
	{
		if (maxT[whichPlane] < maxT[i])
		{
			whichPlane = i;
		}
	}

	/* Check final candidate actually inside box */
	
	if (maxT[whichPlane] < 0)
	{
		return false;
	}
	
	for (i=0;i<3;i++)
	{
		if (whichPlane != i)
		{
			coord[i] = origin[i] + maxT[whichPlane] * dir[i];
			
			if (coord[i] < minB[i] || coord[i] > maxB[i])
			{
				return false;
			}
		}
		
		else
		{
			coord[i] = candidatePlane[i];
		}
		
		/* ray hits box */
	}
	
	if (dir[0] != 0)
	{
		percentage = fabs(coord[0] - start.x) / fabs(end.x - start.x);
		
		if (percentage > 1)
		{
			return false;
		}
		
		if (percentage < rayPercentage)
		{
			rayPercentage = percentage;
			intersectedEntity = e;
		}
		
		return true;
	}
	
	if (dir[1] != 0)
	{
		percentage = fabs(coord[1] - start.y) / fabs(end.y - start.y);
		
		if (percentage > 1)
		{
			return false;
		}
		
		if (percentage < rayPercentage)
		{
			rayPercentage = percentage;
			intersectedEntity = e;
		}
		
		return true;
	}
	
	if (dir[2] != 0)
	{
		percentage = fabs(coord[2] - start.z) / fabs(end.z - start.z);
		
		if (percentage > 1)
		{
			return false;
		}
		
		if (percentage < rayPercentage)
		{
			rayPercentage = percentage;
			intersectedEntity = e;
		}
		
		return true;
	}
	
	if (percentage < rayPercentage)
	{
		rayPercentage = percentage;
		intersectedEntity = e;
	}
	
	return true;
}

void BoundingBoxManager::dump()
{
	for (int i = 0 ; i < MAX_CANDIDATES ; i++)
	{
		if (candidate[i] != NULL)
		{
			printf("%d) %s - '%s'\n", i, entityNames[candidate[i]->entityType], candidate[i]->getName());
		}
		else
		{
			printf("%d) 'NULL'\n", i);
		}
	}
}

BoundingBoxManager BoundingBoxManager::instance;
