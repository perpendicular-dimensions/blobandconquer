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

#include "decals.h"

void drawDecals()
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_FOG);
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glDepthMask(GL_FALSE);
	glPolygonOffset(-1, 1);
	
	for (Decal *decal = (Decal*)entityManager->decalList.getFirstElement() ; decal != NULL ; decal = (Decal*)decal->next)
	{
		if (decal->texture == NULL)
		{
			continue;
		}
		
		if (isClusterVisible(decal->cluster) == 0)
		{
			continue;
		}
		
		glBindTexture(GL_TEXTURE_2D, decal->texture->data);
		
		if (decal->alternativeBlend)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
		
		glPushMatrix();
		{
			glColor4fv(decal->color.color);
		
			glTranslatef(decal->position.x, decal->position.y, decal->position.z);
			
			glRotatef(decal->rotation.x, 0.0, 1.0, 0.0);
			glRotatef(decal->rotation.y, 1.0, 0.0, 0.0);
			glRotatef(decal->rotation.z, 0.0, 0.0, 1.0);
			
			glBegin(GL_QUADS);
			{
				glTexCoord2f(decal->uv1, decal->uv2); glVertex2d(decal->x1, decal->y1);
				glTexCoord2f(decal->uv3, decal->uv2); glVertex2d(decal->x2, decal->y1);
				glTexCoord2f(decal->uv3, decal->uv4); glVertex2d(decal->x2, decal->y2);
				glTexCoord2f(decal->uv1, decal->uv4); glVertex2d(decal->x1, decal->y2);
			}
			glEnd();
		}
		glPopMatrix();
	}
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	glDepthMask(GL_TRUE);
	glEnable(GL_FOG);
}

void removeTouchingDecals(Entity *ent)
{
	debug(("removeTouchingDecals() - Checking %d...\n", entityManager->decalList.getSize()));
	
	BoundingBox boundingBox = ent->boundingBox;
	boundingBox.mins.x--;
	boundingBox.mins.y--;
	boundingBox.mins.z--;
	boundingBox.maxs.x++;
	boundingBox.maxs.y++;
	boundingBox.maxs.z++;
	
	BoundingBox decalBoundingBox;
	decalBoundingBox.mins.x = -1;
	decalBoundingBox.mins.y = -1;
	decalBoundingBox.mins.z = -1;
	decalBoundingBox.maxs.x = 1;
	decalBoundingBox.maxs.y = 1;
	decalBoundingBox.maxs.z = 1;
	
	for (Decal *decal = (Decal*)entityManager->decalList.getFirstElement() ; decal != NULL ; decal = (Decal*)decal->next)
	{
		decalBoundingBox.update(decal->position);
		
		if (Collision::collision(ent->position, boundingBox, decal->position, decalBoundingBox))
		{
			decal->health = 0;
		}
	}
}

void addDecal(Properties *props)
{
	if (game->decalPolicy == DECAL_POLICY_OFF)
	{
		return;
	}
	
	Decal *decal = entityManager->spawnDecal();
	
	decal->load(props);
}

Decal *addDecal(Vector position, float size)
{
	/* Slopes and weirdly shaped brushes do crazy things with the decals, so don't display them */
	
	//if (game->decalPolicy == DECAL_POLICY_OFF || tracer->brush->numOfBrushSides > 6)
	if (game->decalPolicy == DECAL_POLICY_OFF)
	{
		return NULL;
	}
	
	BSPBrush *brush = tracer->brush;
	BSPBrushSide *brushSide;
	Plane *plane;
	
	Decal *decal = entityManager->spawnDecal();
	
	decal->rotate(*tracer->outputNormal);
	decal->position = position;
	decal->x1 = -size;
	decal->x2 = size;
	decal->y1 = -size;
	decal->y2 = size;
	
	float d[6];
	float x1 = decal->position.x - size;
	float x2 = decal->position.x + size; 
	float y1 = decal->position.y - size;
	float y2 = decal->position.y + size;
	float z1 = decal->position.z - size;
	float z2 = decal->position.z + size;
	
	for (int i = 0 ; i < 6 ; i++)
	{
		brushSide = &bsp->brushSides[brush->brushSide + i];
		plane = &bsp->planes[brushSide->plane];
		d[i] = plane->d * plane->normal.x + plane->d * plane->normal.y + plane->d * plane->normal.z;
	}
	
	Math::limit(&x1, d[0], d[1]);
	Math::limit(&x2, d[0], d[1]);
	Math::limit(&y1, d[2], d[3]);
	Math::limit(&y2, d[2], d[3]);
	Math::limit(&z1, d[4], d[5]);
	Math::limit(&z2, d[4], d[5]);
	
	x1 -= decal->position.x;
	x2 -= decal->position.x; 
	y1 -= decal->position.y;
	y2 -= decal->position.y;
	z1 -= decal->position.z;
	z2 -= decal->position.z;
	
	if (fabs(tracer->outputNormal->x) == 1.0)
	{
		decal->rotation.z += 90;
		decal->x1 = y1;
		decal->x2 = y2;
		decal->y1 = z1;
		decal->y2 = z2;
	}
	else if (fabs(tracer->outputNormal->y) == 1.0)
	{
		decal->x1 = x1;
		decal->x2 = x2;
		decal->y1 = z1;
		decal->y2 = z2;
	}
	else if (fabs(tracer->outputNormal->z) == 1.0)
	{
		decal->rotation.z += (-90 * tracer->outputNormal->z);
		decal->x1 = x1;
		decal->x2 = x2;
		decal->y1 = y1;
		decal->y2 = y2;
	}
	else if (tracer->outputNormal->x == 0.0)
	{
		decal->rotation.z -= 45;
		decal->x1 = x1;
		decal->x2 = x2;
		decal->y1 = y1;
		decal->y2 = y2;
	}
	else if (tracer->outputNormal->y == 0.0)
	{
		decal->rotation.z -= 45;
		decal->x1 = x1;
		decal->x2 = x2;
		decal->y1 = y1;
		decal->y2 = y2;
	}
	
	// finally work out the uv mapping percentages...
	float tempX = size * 2;
	float tempY = size * 2;
	
	decal->uv1 = (size + decal->x1) / tempX;
	decal->uv2 = (size + decal->y1) / tempY;
	decal->uv3 = (size + decal->x2) / tempX;
	decal->uv4 = (size + decal->y2) / tempY;
	
	return decal;
}

Decal *addBulletHole(int type, Vector position, Vector heading)
{
	if (game->decalPolicy == DECAL_POLICY_OFF)
	{
		return NULL;
	}
	
	Decal *decal = addDecal(tracer->outputEnd, 1);
	
	if (decal != NULL)
	{
		decal->type = type;
		decal->color.color[3] = 0.50;
	
		int entityLeaf = calculateEntityLeaf(decal->position);
		decal->cluster = bsp->leafs[entityLeaf].cluster;
	}
	
	return decal;
}

void addBulletMark(Vector position, Vector heading)
{
	if (game->decalPolicy == DECAL_POLICY_OFF)
	{
		return;
	}
	
	if (tracer->outputContent & CONTENTS_TRANSLUCENT)
	{
		return;
	}
	
	Decal *decal = addBulletHole(DECAL_BULLET_MARK, position, heading);
	
	if (decal != NULL)
	{
		decal->color.color[3] = 0.75;
	}
}

void addExplosionMark(Vector position, Vector end, Vector adjust, float size)
{
	bsp->getTraceLocation(position, end);
	
	if (tracer->outputFraction == 1.0)
	{
		return;
	}
	
	if (tracer->outputContent & CONTENTS_TRANSLUCENT)
	{
		return;
	}
	
	Decal *decal = addDecal(tracer->outputEnd, size);
	
	if (decal != NULL)
	{
		decal->type = DECAL_EXPLOSION_MARK;
		decal->position += adjust;
	
		decal->color.set(1.0, 1.0, 1.0, 0.75);
	
		int entityLeaf = calculateEntityLeaf(decal->position);
		decal->cluster = bsp->leafs[entityLeaf].cluster;
	}
}

void addExplosionMark(Vector position, float size)
{
	Vector end, adjust;

	if (game->decalPolicy == DECAL_POLICY_OFF)
	{
		return;
	}
	
	end = position;
	end.z -= size;
	adjust.set(0, 0, 0.1);
	addExplosionMark(position, end, adjust, size);
	
	end = position;
	end.x += size;
	adjust.set(-0.25, 0, 0);
	addExplosionMark(position, end, adjust, size);
	
	end = position;
	end.x -= size;
	adjust.set(0.25, 0, 0);
	addExplosionMark(position, end, adjust, size);
	
	end = position;
	end.y += size;
	adjust.set(0, -0.25, 0);
	addExplosionMark(position, end, adjust, size);
	
	end = position;
	end.y -= size;
	adjust.set(0, 0.25, 0);
	addExplosionMark(position, end, adjust, size);
}

void addBloodSplat(Vector position, Vector end, Vector adjust, float size)
{
	bsp->getTraceLocation(position, end);
	
	if (tracer->outputFraction == 1.0)
	{
		return;
	}
	
	if (tracer->outputContent & CONTENTS_TRANSLUCENT)
	{
		return;
	}
	
	Decal *decal = addDecal(tracer->outputEnd, size);
	
	if (decal != NULL)
	{
		decal->type = DECAL_BLOOD;
		decal->position += adjust;
	
		float r = Math::rrand(25, 50);
		r /= 100;
		decal->color.set(1.0, 1.0, 1.0, r);
	
		int entityLeaf = calculateEntityLeaf(decal->position);
		decal->cluster = bsp->leafs[entityLeaf].cluster;
	}
}

void addBloodSplat(Vector position, float size)
{
	Vector end, adjust;

	if ((game->bloodPolicy != BLOOD_POLICY_ON) || (game->decalPolicy == DECAL_POLICY_OFF))
	{
		return;
	}
	
	end = position;
	end.z -= 32;
	adjust.set(0, 0, 0.1);
	addBloodSplat(position, end, adjust, size);
	
	end = position;
	end.x += 16;
	adjust.set(-0.25, 0, 0);
	addBloodSplat(position, end, adjust, size);
	
	end = position;
	end.x -= 16;
	adjust.set(0.25, 0, 0);
	addBloodSplat(position, end, adjust, size);
	
	end = position;
	end.y += 16;
	adjust.set(0, -0.25, 0);
	addBloodSplat(position, end, adjust, size);
	
	end = position;
	end.y -= 16;
	adjust.set(0, 0.25, 0);
	addBloodSplat(position, end, adjust, size);
}

Decal *addGroundTargetMark(Vector position, Vector end)
{
	bsp->getTraceLocation(position, end);
	
	if (tracer->outputFraction == 1.0)
	{
		return NULL;
	}
	
	if (tracer->outputContent & CONTENTS_TRANSLUCENT)
	{
		return NULL;
	}
	
	Decal *decal = addDecal(tracer->outputEnd, 16);
	
	if (decal != NULL)
	{
		decal->type = -1;
		decal->position.z += 0.1;
		decal->remove = true;
		decal->health = 200;
	
		decal->color.set(1.0, 1.0, 1.0, 1.0);
	
		int entityLeaf = calculateEntityLeaf(decal->position);
		decal->cluster = bsp->leafs[entityLeaf].cluster;
	}
	
	return decal;
}

void allocateDecalTexture(Decal *decal)
{
	static String texture;
	
	switch (decal->type)
	{
		case DECAL_BLOOD:
			texture.setText("textures/game/blood0%d.png", 1 + (rand() % 5));
			break;
			
		case DECAL_GLASS_IMPACT:
			texture.setText("textures/game/glassImpact01.png");
			break;
			
		case DECAL_BULLET_MARK:
			texture.setText("textures/game/bulletMark0%d.tga", 1 + (rand() % 3));
			decal->alternativeBlend = true;
			break;
			
		case DECAL_EXPLOSION_MARK:
			texture.setText("textures/game/explosionMark0%d.tga", 1 + (rand() % 3));
			decal->alternativeBlend = true;
			break;
	}
	
	decal->texture = textureManager->getTexture(texture.getText());
}

void doDecals()
{
	int difference = 0;
	int n = 0;
	
	int limit = (game->decalPolicy * 250);
	
	for (Decal *decal = (Decal*)entityManager->decalList.getFirstElement() ; decal != NULL ; decal = (Decal*)decal->next)
	{
		n++;
		
		if (decal->texture == NULL)
		{
			allocateDecalTexture(decal);
		}
		
		if (entityManager->decalList.getSize() > limit)
		{
			difference = entityManager->decalList.getSize() - limit;
			
			if (n < difference)
			{
				decal->remove = true;
			}
		}
		
		if (decal->remove)
		{
			decal->health -= engine->getTimeDifference(TD_LOGIC);
			decal->color.color[3] = decal->health;
			decal->color.color[3] /= 100;
		}
		
		if (decal->health <= 0)
		{
			decal = (Decal*)decal->previous;
			entityManager->decalList.remove(decal->next);
			continue;
		}
	}
}
