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
		
// Just contains lots of misc drawing related functions...

#include "opengl.h"

bool withinViewableRange(Entity *entity)
{
	if (entity->cluster != -1)
	{
		if (isClusterVisible(entity->cluster) == 0)
		{
			return false;
		}
	}
	
	return camera->frustum.containsEntity(entity);
}

void drawCyclinder(Vector position, int segments, float size, float height, GLColor c)
{
	double f1, f2;
	Vector v1, v2;

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_FALSE);
	
	glBindTexture(GL_TEXTURE_2D, textureManager->getTexture("textures/game/fire.png")->data);
	
	for (int i = 0 ; i < segments ; i++)
	{
		f1 = (i * 2 * PI);
		f1 -= graphics->getAnimTimer() * 0.05;
		f1 /= segments;
		
		f2 = ((i + 1) * 2 * PI);
		f2 -= graphics->getAnimTimer() * 0.05;
		f2 /= segments;
		
		v1 = position;
		v1.x += (cos(f1) * size);
		v1.y += (sin(f1) * size);
		
		v2 = position;
		v2.x += (cos(f2) * size);
		v2.y += (sin(f2) * size);
		
		glColor4fv(c.color);
		
		glPushMatrix();
		{
			glBegin(GL_QUADS);
			{
				glTexCoord2f(0.0, 1.0); glVertex3f(v1.x, v1.y, v1.z);
				glTexCoord2f(1.0, 1.0); glVertex3f(v2.x, v2.y, v2.z);
				glTexCoord2f(1.0, 0.0); glVertex3f(v2.x, v2.y, v2.z + height);
				glTexCoord2f(0.0, 0.0); glVertex3f(v1.x, v1.y, v1.z + height);
			}
			glEnd();
		}
		glPopMatrix();
	}
	
	glDepthMask(GL_TRUE);
}

void drawBox(Vector position, float width, float length, float height, float tcm)
{
	width /= 2;
	length /= 2;
	height /= 2;
	
	glPushMatrix();
	{
		glTranslatef(position.x, position.y, position.z);
		
		glBegin(GL_QUADS);
		{
			glTexCoord2f(tcm, tcm); glVertex3f( width, length, -height);
			glTexCoord2f(0.0, tcm); glVertex3f(-width, length, -height);
			glTexCoord2f(0.0, 0.0); glVertex3f(-width, length, height);
			glTexCoord2f(tcm, 0.0); glVertex3f( width, length, height);
	
			glTexCoord2f(0.0, 0.0); glVertex3f( width,-length, height);
			glTexCoord2f(tcm, 0.0); glVertex3f(-width,-length, height);
			glTexCoord2f(tcm, tcm); glVertex3f(-width,-length, -height);
			glTexCoord2f(0.0, tcm); glVertex3f( width,-length, -height);
	
			// Top
			glTexCoord2f(0.0, 0.0); glVertex3f( width, length, height);
			glTexCoord2f(tcm, 0.0); glVertex3f(-width, length, height);
			glTexCoord2f(tcm, tcm); glVertex3f(-width,-length, height);
			glTexCoord2f(0.0, tcm); glVertex3f( width,-length, height);
	
			// Bottom
			glTexCoord2f(0.0, 0.0); glVertex3f( width,-length, -height);
			glTexCoord2f(tcm, 0.0); glVertex3f(-width,-length, -height);
			glTexCoord2f(tcm, tcm); glVertex3f(-width, length, -height);
			glTexCoord2f(0.0, tcm); glVertex3f( width, length, -height);
	
			glTexCoord2f(tcm, 0.0); glVertex3f(-width, length,  height);
			glTexCoord2f(tcm, tcm); glVertex3f(-width, length, -height);
			glTexCoord2f(0.0, tcm); glVertex3f(-width,-length, -height);
			glTexCoord2f(0.0, 0.0); glVertex3f(-width,-length,  height);
	
			glTexCoord2f(0.0, tcm); glVertex3f( width, length, -height);
			glTexCoord2f(0.0, 0.0); glVertex3f( width, length,  height);
			glTexCoord2f(tcm, 0.0); glVertex3f( width,-length,  height);
			glTexCoord2f(tcm, tcm); glVertex3f( width,-length, -height);
		}
		glEnd();
	}
	glPopMatrix();
}

void drawBoundingBox(Entity *entity)
{
	glColor4f(0.75, 0.75, 1.0, 0.25);
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	drawBox(entity->position, entity->boundingBox.maxs.x * 2, entity->boundingBox.maxs.y * 2, entity->boundingBox.maxs.z * 2, 1);
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

void drawSelectedUnit()
{
	Point point;
	float dir = 1;
	float c = 0.5;
	
	if ((player->target == NULL) || (game->cutsceneType != CUTSCENE_NONE))
	{
		return;
	}
	
	if (dir == 1)
	{
		c += 0.01 * engine->getTimeDifference(TD_ANIMATION);
		
		if (c >= 0.75)
		{
			dir = 0;
		}
	}
	else
	{
		c -= 0.01 * engine->getTimeDifference(TD_ANIMATION);
		
		if (c <= 0.25)
		{
			dir = 1;
		}
	}
	
	point = graphics->convertToScreenCoords(player->target->position);
	
	graphics->setMode(MODE_2D);
	
	glColor4f(0.0, 1.0, 0.0, c);
	
	if (game->autoFaceTimer > 0)
	{
		glColor4f(1.0, 0.0, 0.0, c);
	}
	
	graphics->blit(textureManager->getTexture("textures/game/targetter.png"), (int)point.x, (int)point.y, true);
	graphics->setMode(MODE_3D);
}

void drawSkyDome(Vector position, int size, Texture *texture)
{
	glDepthMask(GL_FALSE);
	
	glDisable(GL_FOG);
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	
	glBindTexture(GL_TEXTURE_2D, texture->data);
	
	drawBox(position, 24, 24, 24, mission->skySphereSize);
	
	glEnable(GL_FOG);
	
	glDepthMask(GL_TRUE);
	
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

void enableMultiTexturing()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	/* Unit 0 */

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* Unit 1 */

	glActiveTextureARB(GL_TEXTURE1_ARB);

	glEnable(GL_TEXTURE_2D);
	
	/* Set up environment to draw textures, lightmaps etc. */
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);

	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 2.0f);
	
	/* Set back to unit 0 */
	
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);
}

void finishMultiTexturing()
{
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopAttrib();
}

void renderScaledTexture(Texture *texture)
{
	glEnable(GL_TEXTURE_2D);
	
	float scaleX = graphics->screen->w;
	scaleX /= texture->iw;
	
	float scaleY = graphics->screen->h;
	scaleY /= texture->ih;
	
	glBindTexture(GL_TEXTURE_2D, texture->data);
	
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(texture->tw * scaleX, 0);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(texture->tw * scaleX, texture->th * scaleY);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0, texture->th * scaleY);
	}
	glEnd();
}
