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

#include "shadows.h"

void renderShadowSurfaceVolume()
{
	float shadowSize = (self->boundingBox.getHorizontalSize() * 1.25);
	float adjustment = 0.1;
	
	glPushMatrix();
	{
		glTranslatef(self->position.x, self->position.y, self->position.z + self->boundingBox.mins.z - SHADOW_LENGTH + adjustment);
	
		gluCylinder(graphics->quadric, 0.0f, shadowSize, SHADOW_LENGTH, 12, 1);
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		glTranslatef(self->position.x, self->position.y, self->position.z + self->boundingBox.mins.z + adjustment);
		
		gluDisk(graphics->quadric, 0.0f, shadowSize, 12, 1);
	}
	glPopMatrix();
}

bool isValidShadowEntity()
{
	switch (game->shadowPolicy)
	{
		case SHADOW_POLICY_OFF:
			return false;
			break;
			
		case SHADOW_POLICY_BLOB:
			return (self == player);
			break;
			
		case SHADOW_POLICY_UNITS:
			return ((self->entityType == ET_BLOB) || (self->entityType == ET_BIOMECH));
			break;
			
		default:
			switch (self->entityType)
			{
				case ET_BLOB:
				case ET_BIOMECH:
				case ET_BOSS:
				case ET_WEAPON:
				case ET_SUPPLY_CRATE:
					return true;
					break;
					
				case ET_ITEM:
					if (self->owner != NULL)
					{
						return false;
					}
					
					if (self->definition->type != ITM_STICKY_C4)
					{
						return true;
					}
					else
					{
						return (!(self->flags & EF_WEIGHTLESS));
					}
					break;
			
				case ET_BULLET:
					return (!(self->flags & EF_WEIGHTLESS));
					break;
		
				default:
					return false;
					break;
			}
			break;
	}
}

void generateShadows()
{
	glClearStencil(0);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
		
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);
	
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	glDepthMask(GL_FALSE);
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFFL);
	
	glEnable(GL_CULL_FACE);
	
	// finally generate all the shadows...
	for (Reference *ref = (Reference*)entityManager->drawList.getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		self = (Entity*)ref->object;
		
		if (!isValidShadowEntity())
		{
			continue;
		}
		
		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		renderShadowSurfaceVolume();
		
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		renderShadowSurfaceVolume();
	}
	
	glPopAttrib();
		
	glDepthMask(GL_TRUE);
}

void drawShadow()
{
	if (game->shadowPolicy == SHADOW_POLICY_OFF)
	{
		return;
	}
	
	generateShadows();
	
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

	glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFFL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glPushMatrix();
	{
		glLoadIdentity();
		
		glBegin(GL_TRIANGLE_STRIP);
		{
			glVertex3f(-5.0f, 5.0f,-5.0f);
			glVertex3f(-5.0f,-5.0f,-5.0f);
			glVertex3f( 5.0f, 5.0f,-5.0f);
			glVertex3f( 5.0f,-5.0f,-5.0f);
		}
		glEnd();
	}
	glPopMatrix();
	
	glPopAttrib();
	
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_STENCIL_TEST);
}
