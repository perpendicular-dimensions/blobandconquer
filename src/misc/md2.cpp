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

#include "md2.h"

void getAnimation(const char *name, Entity *ent)
{
	bool found;
	MD2Model *md2 = ent->definition->md2;
	
	if (md2 == NULL)
	{
		return;
	}
	
	if (ent->currentAnim != NULL)
	{
		if (strcmp(ent->currentAnim->name, name) == 0)
		{
			return;
		}
	}
	
	found = false;
	
	for (int i = 0 ; i < md2->animCount ; i++)
	{
		if (strcmp(md2->animation[i].name, name) == 0)
		{
			found = true;
			
			ent->currentAnim  = &md2->animation[i];

			/* Set the NEXT frame to the animation's start, UNLESS the current frame is already set to this */

			ent->nextAnimFrame = (ent->currentAnimFrame == ent->currentAnim->start) ? ((ent->currentAnimFrame + 1 == ent->currentAnim->end) ? ent->currentAnimFrame : ent->currentAnimFrame + 1) : ent->currentAnim->start;
			ent->animationTimer = 0;
			
			break;
		}
	}
	
	if (found == false)
	{
		printf("Failed to find animation %s\n", name);
		
		exit(0);
	}
}

void animateEntityMD2(Entity *ent)
{
	if ((ent->definition->md2 == NULL) || (ent->currentAnim == NULL))
	{
		return;
	}
	
	ent->animationTimer -= (engine->getTimeDifference(TD_LOGIC) * 0.75);

	if (ent->animationTimer <= 0)
	{
		/* Advance to the next frame */

		ent->animationTimer = 1;
		
		/* Return here if the animation is not required to loop */
		
		if ((ent->flags & EF_NO_MD2_LOOP) && ent->currentAnimFrame == ent->currentAnim->end)
		{
			return;
		}
		
		ent->currentAnimFrame++;

		if ((ent->currentAnimFrame < ent->currentAnim->start) || (ent->currentAnimFrame > ent->currentAnim->end))
		{
			/* Current frame is outside the range, this means that we're starting */
			ent->currentAnimFrame = ent->currentAnim->start;
			ent->nextAnimFrame    = ent->currentAnimFrame + 1 > ent->currentAnim->end ? ent->currentAnim->start : ent->currentAnimFrame + 1;
		}
		else if (ent->currentAnimFrame == ent->currentAnim->end)
		{
			// Loop back to start...
			
			if ((ent->flags & EF_NO_MD2_LOOP) == 0)
			{
				ent->currentAnimFrame = ent->currentAnim->start;
			}
		}
		else
		{
			ent->nextAnimFrame = (ent->currentAnimFrame + 1 > ent->currentAnim->end) ? ent->currentAnim->start : ent->currentAnimFrame + 1;
		}
	}
}

void drawMD2(Entity *entity)
{
	int i, j, vertIndex, textureIndex;
	float x, y, z;
	Object *currentFrame, *nextFrame, *firstFrame;

	firstFrame   = &entity->definition->md2->frame[0];
	currentFrame = &entity->definition->md2->frame[entity->currentAnimFrame];
	nextFrame    = &entity->definition->md2->frame[entity->nextAnimFrame];
	
	glBegin(GL_TRIANGLES);
	{
		for (i=0;i<currentFrame->faceCount;i++)
		{
			for (j=0;j<3;j++)
			{
				vertIndex    = firstFrame->faces[i].vertIndex[j];
				textureIndex = firstFrame->faces[i].coordIndex[j];

				/* Here is where the texture information will go */

                if (firstFrame->verts != NULL)
                {
                    glTexCoord2f(firstFrame->textures[textureIndex].x, firstFrame->textures[textureIndex].y);
                }

				/* Do the faces */

				x = currentFrame->verts[vertIndex].x + 0 * (nextFrame->verts[vertIndex].x - currentFrame->verts[vertIndex].x);
				y = currentFrame->verts[vertIndex].y + 0 * (nextFrame->verts[vertIndex].y - currentFrame->verts[vertIndex].y);
				z = currentFrame->verts[vertIndex].z + 0 * (nextFrame->verts[vertIndex].z - currentFrame->verts[vertIndex].z);

				glVertex3f(x, z, y);
			}
		}
	}
	glEnd();
	
	animateEntityMD2(entity);
}
