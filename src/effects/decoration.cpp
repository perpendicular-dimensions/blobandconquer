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

#include "decoration.h"

void drawDecoration(bool transparent)
{
	Decoration *decoration = (Decoration*)self;
	
	if (decoration->type == DT_SHARD)
	{
		if (!transparent)
		{
			return;
		}
	}
	else if (transparent)
	{
		return;
	}
	
	glPushMatrix();
	{
		switch (decoration->type)
		{
			case DT_SHARD:
				
				glEnable(GL_BLEND);
				glDisable(GL_TEXTURE_2D);
				
				glColor4f(0.85, 0.85, 1.0, decoration->custom);
				
				glTranslatef(decoration->position.x, decoration->position.y, decoration->position.z);
				
				glBegin(GL_TRIANGLES);
				{
					glVertex3f(decoration->triangle.point[0].x, decoration->triangle.point[0].y, decoration->triangle.point[0].z);
					glVertex3f(decoration->triangle.point[1].x, decoration->triangle.point[1].y, decoration->triangle.point[1].z);
					glVertex3f(decoration->triangle.point[2].x, decoration->triangle.point[2].y, decoration->triangle.point[2].z);
				}
				glEnd();
				break;
				
			case DT_GENERAL:
				
				glDisable(GL_BLEND);
				glEnable(GL_TEXTURE_2D);
				
				glColor3f(1.0, 1.0, 1.0);
				
				glTranslatef(decoration->position.x, decoration->position.y, decoration->position.z);
				
				glScalef(decoration->size, decoration->size, decoration->size);
				
				glBindTexture(GL_TEXTURE_2D, decoration->texture->data);
				
				glCallList(decoration->model->data);
				
				break;
		}
	}
	glPopMatrix();
}

void addGlassShard(Triangle *triangle)
{
	Decoration *decoration = entityManager->spawnDecoration(DT_SHARD);
	
	decoration->health = Math::rrand(100, 300);
	decoration->velocity.x = Math::rrand(-3, 3);
	decoration->velocity.y = Math::rrand(-3, 3);
	decoration->velocity.z = Math::rrand(0, 3);
	decoration->size = 1;
	
	decoration->custom = Math::rrand(100, 300);
	decoration->custom /= 1000;
	
	decoration->velocity.x /= 10;
	decoration->velocity.y /= 10;
	decoration->velocity.z /= 10;
	
	decoration->flags |= EF_DYING;
	
	decoration->triangle = *triangle;
	
	decoration->draw = drawDecoration;
}

void throwNBFleshChunk(Vector position)
{
	Particle *particle;

	for (int i = 0 ; i < 75 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position = position;
		particle->life = Math::rrand(75, 150);
		particle->size = Math::rrand(1, 2);
		
		float speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 2;
		particle->velocity.z = speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 2;
		particle->velocity.x = speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 2;
		particle->velocity.y = speed;
		
		particle->color = GLColor::getRandomColor();
	}
}

void throwFleshChunk(Vector position, bool underWater)
{
	float x, y;
	
	Decoration *decoration = entityManager->spawnDecoration(DT_FLESH);
	
	decoration->position = position;
	decoration->health = 300;
	decoration->velocity.x = Math::rrand(-25, 25);
	decoration->velocity.y = Math::rrand(-25, 25);
	
	if (!underWater)
	{
		decoration->velocity.z = Math::rrand(2, 10);
		
		decoration->size = Math::rrand(25, 100);
		decoration->size /= 100;
	}
	else
	{
		x = Math::rrand(-300, 300);
		y = Math::rrand(-300, 300);
	
		x /= 100;
		y /= 100;
		
		decoration->position.x += x;
		decoration->position.y += y;
		
		decoration->size = 0.25;
		
		decoration->velocity.x = Math::rrand(-5, 5);
		decoration->velocity.y = Math::rrand(-5, 5);
	}
	
	decoration->velocity.x /= 100;
	decoration->velocity.y /= 100;
	decoration->velocity.z /= 10;
	
	decoration->flags |= EF_DYING;
	
	if (underWater)
	{
		decoration->flags |= EF_WEIGHTLESS;
	}
	
	decoration->draw = drawDecoration;
}

void throwExplosionDebris(Vector position)
{
	Decoration *decoration = entityManager->spawnDecoration(DT_FIRE);
	
	decoration->position  = position;
	decoration->health = Math::rrand(25, 150);
	decoration->velocity.x = Math::rrand(-100, 100);
	decoration->velocity.y = Math::rrand(-100, 100);
	decoration->velocity.z = Math::rrand(2, 10);
	decoration->size = 0.25;
	
	decoration->velocity.x /= 100;
	decoration->velocity.y /= 100;
	decoration->velocity.z /= 10;
	
	decoration->flags |= EF_DYING;
	
	decoration->draw = drawDecoration;
}

void allocateDecorationData(Decoration *decoration)
{
	decoration->texture = getRequiredTexture(decoration->textureName.getText());
	decoration->model = getRequiredSimpleModel(decoration->modelName.getText(), MDL_RAW);
}

void addGeneralDebris(Vector position, Vector velocity, float health, const char *textureName, float size, bool slides)
{
	String modelName;
	modelName.setText("gfx/rw2/debris%d.blend.raw", Math::rrand(1, 5));
	
	Decoration *decoration = entityManager->spawnDecoration(DT_GENERAL);
	
	decoration->position  = position;
	decoration->health = health;
	decoration->velocity = velocity;
	decoration->size = size;
	decoration->slides = slides;
	decoration->textureName = textureName;
	decoration->modelName = modelName.getText();
	decoration->setName("_dec");
	
	if ((rand() % 15) != 0)
	{
		decoration->flags |= EF_DYING;
	}
	
	decoration->draw = drawDecoration;
	
	if (size < 1.5) size = 1.5;
	
	decoration->boundingBox.mins.set(-size, -size, -size);
	decoration->boundingBox.maxs.set(size, size, size);
}

void addDecoration(Properties *props)
{
	Decoration *decoration = entityManager->spawnDecoration(DT_FLESH);
	
	decoration->load(props);
	
	decoration->draw = drawDecoration;
}

void doDecoration()
{
	bool addToDraw = false;
	
	for (Decoration *decoration = (Decoration*)entityManager->decorationList.getFirstElement() ; decoration != NULL ; decoration = (Decoration*)decoration->next)
	{
		if (decoration->health <= -50)
		{
			decoration = (Decoration*)decoration->previous;
			entityManager->decorationList.remove(decoration->next);
			continue;
		}
		
		self = decoration;
		
		//self->riding = NULL;
		
		if (decoration->texture == NULL)
		{
			allocateDecorationData(decoration);
		}
		
		decoration->applyGravity(engine->getTimeDifference(TD_LOGIC));
		
		if (decoration->type == DT_SHARD)
		{
			addToDraw = true;
			
			decoration->position += (decoration->velocity * engine->getTimeDifference(TD_LOGIC));
		}
		else
		{
			moveEntity(decoration);
			
			if (withinViewableRange(decoration))
			{
				addToDraw = true;
			}
		}
		
		if (addToDraw)
		{
			entityManager->addEntityToDrawList(decoration);
			
			decoration->thinkTime -= engine->getTimeDifference(TD_LOGIC);
		
			if (decoration->thinkTime <= 0)
			{
				switch (decoration->type)
				{
					case DT_FLESH:
						addBloodParticle(decoration->position.x, decoration->position.y, decoration->position.z);
						break;
					
					case DT_FIRE:
						addFireParticle(decoration->position.x, decoration->position.y, decoration->position.z);
						break;
				}
			
				decoration->thinkTime = Math::rrand(1, 5);
			}
		}
	
		if (decoration->flags & EF_ONGROUND)
		{
			if (decoration->slides)
			{
				decoration->velocity.x *= 0.75;
				decoration->velocity.y *= 0.75;
			}
			else
			{
				decoration->velocity.x *= 0.25;
				decoration->velocity.y *= 0.25;
			}
			
			if (decoration->type != DT_GENERAL)
			{
				decoration->health = -50;
			}
		}
		
		if (decoration->flags & EF_DYING)
		{
			decoration->health -= engine->getTimeDifference(TD_LOGIC);
		}
	}
}
