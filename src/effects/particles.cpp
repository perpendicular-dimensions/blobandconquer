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

#include "particles.h"

void addOnFireParticles()
{
	Particle *particle = particleManager->spawnParticle();
	
	particle->position = self->position;
	particle->position.x += Math::rrand(-2, 2);
	particle->position.y += Math::rrand(-2, 2);
	particle->position.z += Math::rrand(0, 3);
	
	particle->life = Math::rrand(15, 75);
	particle->size = Math::rrand(1, 4);
	
	float speed = Math::rrand(1, 5);
	speed /= 25;
	
	particle->velocity.set(0, 0, speed);
	
	switch (rand() % 3)
	{
		case 0:
			particle->color.set(1.0, 0.0, 0.0, 1.0);
			break;
		
		case 1:
			particle->color.set(1.0, 1.0, 0.0, 1.0);
			break;
		
		case 2:
			particle->color.set(1.0, 0.5, 0.0, 1.0);
			break;
	}
}

void addPowerUpParticles(Vector position)
{
	Particle *particle;
	
	for (int i = 0 ; i < 50 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position = position;
		particle->life = Math::rrand(25, 150);
		particle->size = Math::rrand(1, 3);
		
		particle->velocity.x = Math::rrand(-20, 20);
		particle->velocity.y = Math::rrand(-20, 20);
		particle->velocity.z = Math::rrand(0, 20);
		
		particle->velocity.x /= 25;
		particle->velocity.y /= 25;
		particle->velocity.z /= 25;
		
		particle->color = GLColor::getRandomColor();
	}
}

void addTeleportParticles(Vector position)
{
	Particle *particle;
	
	for (int i = 0 ; i < 100 ; i++)
	{
		particle = particleManager->spawnParticle();
		
		particle->position = position;
		particle->position.x += Math::rrand(-5, 5);
		particle->position.y += Math::rrand(-5, 5);
		
		particle->life = Math::rrand(100, 200);
		particle->size = Math::rrand(10, 20);
		particle->size /= 10;
		
		float speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 10;
		particle->velocity.z = speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 10;
		particle->velocity.x = speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 10;
		particle->velocity.y = speed;
		
		particle->color = GLColor::getRandomColor();
	}
}

void addTeleporterParticle(Vector position)
{
	Particle *particle = particleManager->spawnParticle();
	
	particle->type = PT_SPARK;
	particle->position = position;
	particle->position.x += Math::rrand(-7, 7);
	particle->position.y += Math::rrand(-7, 7);
	
	particle->life = Math::rrand(100, 200);
	particle->size = Math::rrand(10, 20);
	particle->size /= 10;
	
	float speed = Math::rrand(1, 3);
	speed /= 10;
	particle->velocity.z = speed;
	
	particle->color = GLColor::getRandomColor();
}

void addBulletHitParticles(Bullet *bullet)
{
	Particle *particle;

	for (int i = 0 ; i < 5 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position = bullet->position;
		particle->life = Math::rrand(50, 100);
		particle->size = 2;
		
		particle->velocity.set(Math::rrand(-25, 25), Math::rrand(-25, 25), Math::rrand(-25, 25));
		particle->velocity.x *= 0.01;
		particle->velocity.y *= 0.01;
		particle->velocity.z *= 0.01;
		
		particle->color = ((WeaponDef*)bullet->definition)->hitColor;
	}
}

void addExplosionParticle(float x, float y, float z)
{
	Particle *particle = particleManager->spawnParticle();

	particle->position.set(x, y, z);
	particle->life = Math::rrand(30, 50);
	particle->size = Math::rrand(1, 10);
	
	particle->velocity.set(Math::rrand(-50, 50), Math::rrand(-50, 50), Math::rrand(0, 50));
	particle->velocity.x /= 100;
	particle->velocity.y /= 100;
	particle->velocity.z /= 100;
	
	switch (rand() % 3)
	{
		case 0:
			particle->color = GLColor::red;
			break;
		
		case 1:
			particle->color = GLColor::yellow;
			break;
		
		case 2:
			particle->color = GLColor::white;
			break;
	}
}

void addFireParticle(float x, float y, float z)
{
	Particle *particle = particleManager->spawnParticle();

	particle->position.set(x, y, z);
	particle->life = Math::rrand(25, 35);
	particle->size = Math::rrand(1, 3);
	
	float speed = Math::rrand(1, 10);
	speed /= 2500;
	
	particle->velocity.set(0, 0, speed);
	
	switch (rand() % 3)
	{
		case 0:
			particle->color = GLColor::red;
			break;
		
		case 1:
			particle->color = GLColor::yellow;
			break;
		
		case 2:
			particle->color = GLColor::white;
			break;
	}
}

void addSmokeParticle(Vector position)
{
	Particle *particle = particleManager->spawnParticle();
	
	particle->life = Math::rrand(100, 200);
	particle->size = Math::rrand(2, 3);
	particle->position = position;
	particle->color = GLColor::grey.darker();
	particle->velocity.x = Math::rrand(1, 25);
	particle->velocity.y = Math::rrand(1, 25);
	particle->velocity.z = Math::rrand(1, 350);
	particle->velocity = particle->velocity * 0.001;
	particle->color.color[3] = Math::rrand(1, 10);
	particle->color.color[3] /= 10;
}

void addBubbleParticle(Vector position)
{
	Particle *particle = particleManager->spawnParticle();

	particle->position = position;
	particle->life = 100;
	particle->size = 0.5;
	
	GLColor white = GLColor::white;
	white.color[3] = 0.1;
	
	particle->color = white;
}

void addTrailParticle(Bullet *bullet)
{
	WeaponDef *weaponDef = ((WeaponDef*)bullet->definition);
	
	Particle *particle = particleManager->spawnParticle();

	particle->position = bullet->position;
	
	switch (weaponDef->trailType)
	{
		case TRAIL_SMALL_PLASMA:
			particle->life = 50;
			particle->size = 1;
			particle->color = GLColor::green;
			particle->color.color[3] = Math::rrand(1, 10);
			particle->color.color[3] /= 10;
			break;
			
		case TRAIL_HEAVY_PLASMA:
			particle->life = 50;
			particle->size = 5;
			particle->color = GLColor::green;
			particle->color.color[3] = Math::rrand(1, 10);
			particle->color.color[3] /= 10;
			break;
			
		case TRAIL_SMOKE:
			particle->life = 100;
			particle->size = Math::rrand(2, 3);
			particle->color = GLColor::grey.darker();
			particle->velocity.x = Math::rrand(1, 100);
			particle->velocity.y = Math::rrand(1, 100);
			particle->velocity.z = Math::rrand(1, 100);
			particle->velocity = particle->velocity * 0.001;
			particle->color.color[3] = Math::rrand(1, 10);
			particle->color.color[3] /= 10;
			break;
			
		case TRAIL_SMALL_SMOKE:
			particle->life = 50;
			particle->size = Math::rrand(1, 2);
			particle->color = GLColor::grey.darker();
			particle->velocity.x = Math::rrand(1, 100);
			particle->velocity.y = Math::rrand(1, 100);
			particle->velocity.z = Math::rrand(1, 100);
			particle->velocity = particle->velocity * 0.001;
			particle->color.color[3] = Math::rrand(1, 10);
			particle->color.color[3] /= 10;
			break;
			
		case TRAIL_FIRE:
			particle->life = Math::rrand(50, 100);
			particle->size = Math::rrand(2, 4);
			particle->velocity.x = Math::rrand(1, 25);
			particle->velocity.y = Math::rrand(1, 25);
			particle->velocity.z = Math::rrand(1, 25);
			particle->velocity = particle->velocity * 0.001;
			particle->color.color[3] = Math::rrand(1, 10);
			switch (rand() % 3)
			{
				case 0:
					particle->color = GLColor::red;
					break;
		
				case 1:
					particle->color = GLColor::yellow;
					break;
		
				case 2:
					particle->color = GLColor::white;
					break;
			}
			break;
			
		case TRAIL_FLAMETHROWER:
			particle->life = Math::rrand(25, 100);
			particle->size = (50 - bullet->health) * 0.05;
			particle->growth = 0.1;
			particle->velocity.x = bullet->velocity.x * 0.25;
			particle->velocity.y = bullet->velocity.y * 0.25;
			particle->velocity.z = Math::rrand(1, 5);
			particle->velocity.z *= 0.01;
			switch (rand() % 3)
			{
				case 0:
					particle->color = GLColor::red;
					break;
		
				case 1:
					particle->color = GLColor::yellow;
					break;
		
				case 2:
					particle->color = GLColor::white;
					break;
			}
			break;
			
		case TRAIL_ICE:
			particle->size = Math::rrand(1, 6);
			particle->life = 80 - (particle->size * 10);
			particle->velocity.x = Math::rrand(1, 100);
			particle->velocity.y = Math::rrand(1, 100);
			particle->velocity.z = Math::rrand(1, 100);
			particle->velocity = particle->velocity * 0.0001;
			particle->color.color[3] = Math::rrand(1, 10);
			switch (rand() % 3)
			{
				case 0:
					particle->color = GLColor::cyan;
					break;
		
				case 1:
					particle->color = GLColor::cyan.lighter();
					break;
		
				case 2:
					particle->color = GLColor::white;
					break;
			}
			break;
			
		case TRAIL_ENERGY:
			particle->size = 2;
			particle->life = 200;
			particle->color = GLColor::cyan;
			break;
			
		case TRAIL_MULTI:
			particle->size = Math::rrand(2, 4);
			particle->life = 100;
			particle->color = GLColor::getRandomColor();
			break;
	}
}

void addGravUnitParticles(Entity *entity, GLColor c1, GLColor c2)
{
	Particle *particle;

	for (int i = 0 ; i < 5 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->position = entity->position;
		particle->position.z -= 3;
		particle->life = Math::rrand(25, 50);
		particle->size = 1.5;
		
		float speed = Math::rrand(1, 10);
		speed *= 0.05;
		
		particle->velocity.set(0, 0, -speed);
		
		switch (rand() % 3)
		{
			case 0:
				particle->color = GLColor::white;
				break;
			
			case 1:
				particle->color = c1;
				break;
			
			case 2:
				particle->color = c2;
				break;
		}
	}
}

void addSparkParticle(Vector position)
{
	Particle *particle;

	for (int i = 0 ; i < 8 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position.set(position.x + Math::rrand(-1, 1), position.y + Math::rrand(-1, 1), position.z);
		particle->life = Math::rrand(50, 100);
		particle->size = 2;
		
		particle->velocity.set(Math::rrand(-25, 25), Math::rrand(-25, 25), Math::rrand(-25, 25));
		particle->velocity.x *= 0.01;
		particle->velocity.y *= 0.01;
		particle->velocity.z *= 0.01;
		
		switch (rand() % 3)
		{
			case 0:
				particle->color = GLColor::white;
				break;
				
			case 1:
				particle->color = GLColor::skyBlue;
				break;
				
			case 2:
				particle->color = GLColor::blue;
				break;
		}
	}
}

void addNBBloodParticle(Vector v)
{
	Particle *particle;

	for (int i = 0 ; i < 10 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position = v;
		particle->life = Math::rrand(50, 100);
		particle->size = 2;
		
		particle->velocity.set(Math::rrand(-25, 25), Math::rrand(-25, 25), Math::rrand(-25, 25));
		particle->velocity.x *= 0.01;
		particle->velocity.y *= 0.01;
		particle->velocity.z *= 0.01;
		
		particle->color = GLColor::white;
	}
}

void addBloodParticle(float x, float y, float z)
{
	if (game->bloodPolicy == BLOOD_POLICY_OFF)
	{
		return;
	}
	
	Particle *particle = particleManager->spawnParticle();

	particle->position.set(x, y, z);
	particle->life = 100;
	particle->size = 1;
	
	float speed = Math::rrand(1, 10);
	speed /= 100;
	speed = -speed;
	
	particle->velocity.set(0, 0, speed);
	
	float r = Math::rrand(20, 100);
	r /= 100;
	
	GLColor color;
	color.set(r, 0, 0, 1.0);
	
	particle->color = color;
	
	game->alterStat(_("Blood Spilt"), 0.001);
}

void addThrowParticle(Entity *item)
{
	Particle *particle;

	for (int i = 0 ; i < 2 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->position.set(item->position.x, item->position.y, item->position.z);
		particle->life = Math::rrand(25, 50);
		particle->size = 2;
		
		particle->velocity.set(0, 0, 0);
		
		particle->color.set(0.25, 0.25, 0.25, 1.0);
	}
}

void addBloodParticles(Bullet *bullet)
{
	if (game->bloodPolicy == BLOOD_POLICY_OFF)
	{
		addNBBloodParticle(bullet->position);
		return;
	}
	
	Particle *particle;
	float z;

	for (int i = 0 ; i < 15 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->position.set(bullet->position.x + Math::rrand(-1, 1), bullet->position.y + Math::rrand(-1, 1), bullet->position.z);
		particle->life = Math::rrand(75, 150);
		particle->size = Math::rrand(1, 2);
		
		z = Math::rrand(1, 10);
		z /= 100;
		
		particle->velocity.set(0, 0, -z);
		
		particle->color = GLColor::red;
	}
}

void addShieldHitParticles(Vector position)
{
	Particle *particle;

	for (int i = 0 ; i < 5 ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position = position;
		particle->life = Math::rrand(75, 150);
		particle->size = Math::rrand(1, 2);
		
		float speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 15;
		particle->velocity.z = speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 15;
		particle->velocity.x = speed;
		
		speed = Math::rrand(-2, 2);
		speed /= 15;
		particle->velocity.y = speed;
		
		particle->color = GLColor::skyBlue;
	}
}

void addShieldDestroyedParticles(Vector position)
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
		
		if (i < 25)
		{
			particle->color = GLColor::skyBlue;
		}
		else if (i < 50)
		{
			particle->color = GLColor::blue;
		}
		else
		{
			particle->color = GLColor::white;
		}
	}
}

void addPuffParticles(Vector position, GLColor color, int amount, int minSize, int maxSize)
{
	Particle *particle;

	for (int i = 0 ; i < amount ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->position = position;
		particle->life = Math::rrand(75, 100);
		particle->size = Math::rrand(minSize, maxSize);
		
		float speed;
		
		speed = Math::rrand(-4, 4);
		speed /= 15;
		particle->velocity.z = speed;
		
		speed = Math::rrand(-4, 4);
		speed /= 15;
		particle->velocity.x = speed;
		
		speed = Math::rrand(-4, 4);
		speed /= 15;
		particle->velocity.y = speed;
		
		particle->color = color;
	}
}

void addDesimilatedParticles(Vector position)
{
	addPuffParticles(position, GLColor::yellow, 50, 2, 4);
}

void addPlasmaHitParticles(Vector position)
{
	addPuffParticles(position, GLColor::green, 5, 1, 2);
}

void addIceHitParticles(Vector position)
{
	addPuffParticles(position, GLColor::cyan, 5, 1, 2);
}

void addDesimilatorParticles(Vector position)
{
	addPuffParticles(position, GLColor::getRandomColor(), 3, 1, 1);
}

void addExplosionParticles(int size, Vector position)
{
	int number = size * 25;
	
	Math::limit(&number, 0, 450);
	
	Math::limit(&size, 10, 15);
	
	Particle *particle;

	for (int i = 0 ; i < number ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->type = PT_SPARK;
		particle->position = position;
		particle->life = Math::rrand(25, 150);
		particle->size = Math::rrand(1, 3);
		
		particle->velocity.x = Math::rrand(-size, size);
		particle->velocity.y = Math::rrand(-size, size);
		particle->velocity.z = Math::rrand(-size, size);
		
		particle->velocity.x /= 25;
		particle->velocity.y /= 25;
		particle->velocity.z /= 25;
		
		switch (rand() % 3)
		{
			case 0:
				particle->color = GLColor::red;
				break;
				
			case 1:
				particle->color = GLColor::yellow;
				break;
				
			case 2:
				particle->color = GLColor::white;
				break;
		}
	}
	
	int s = (size / 2);
	
	for (int i = 0 ; i < size ; i++)
	{
		particle = particleManager->spawnParticle();
		particle->position = position;
		particle->position.x += Math::rrand(-s, s);
		particle->position.y += Math::rrand(-s, s);
		particle->position.z += Math::rrand(-s, s);
		particle->life = Math::rrand(75, 100);
		particle->size = Math::rrand(10, 20);
		particle->size /= 2;
		
		switch (rand() % 3)
		{
			case 0:
				particle->color = GLColor::red;
				break;
				
			case 1:
				particle->color = GLColor::yellow;
				break;
				
			case 2:
				particle->color = GLColor::white;
				break;
		}
	}
}

//
// this is only used by Galdov
//
void addHalo(Vector position, float growSpeed)
{
	Particle *particle = particleManager->spawnParticle();
	
	particle->type = PT_HALO;
	particle->position = position;
	particle->life = 100;
	particle->size = 1;
	particle->growth = growSpeed;
	particle->color.set(1.0, 1.0, 1.0, 1.0);
}

void drawNormalParticle(Particle *particle)
{
	glPushMatrix();
	{
		glTranslatef(particle->position.x, particle->position.y, particle->position.z);
		
		glRotatef(90, 1.0, 0.0, 0.0);
		
		if (camera->targetCamera == NULL)
		{
			glRotatef(game->cameraDirDegrees, 0.0, 1.0, 0.0);
		}
		else
		{
			float dx = camera->targetCamera->position.x - camera->targetCamera->destination.x;
			float dy = camera->targetCamera->position.y - camera->targetCamera->destination.y;
			float d = -90 + toDegrees(atan2(dy, dx));
			glRotatef(d, 0.0, 1.0, 0.0);
		}
		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
					
		glColor4fv(particle->color.color);
						
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0f, 0.0f); glVertex2d(-particle->size, -particle->size);
			glTexCoord2f(1.0f, 0.0f); glVertex2d(particle->size, -particle->size);
			glTexCoord2f(1.0f, 1.0f); glVertex2d(particle->size, particle->size);
			glTexCoord2f(0.0f, 1.0f); glVertex2d(-particle->size, particle->size);
		}
		glEnd();
	}
	glPopMatrix();
}

void drawSparkParticle(Particle *particle)
{
	Vector oldPosition;
	
	glPushMatrix();
	{
		oldPosition = particle->position;
		oldPosition -= (particle->velocity * 10);
					
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		
		glLineWidth(particle->size);
		
		glBegin(GL_LINES);
		{
			glColor4fv(particle->color.color); glVertex3f(particle->position.x, particle->position.y, particle->position.z);
			glColor4f(0.0, 0.0, 0.0, 0.0); glVertex3f(oldPosition.x, oldPosition.y, oldPosition.z);
		}
		glEnd();
	}
	glPopMatrix();
}

void addParticle(Properties *props)
{
	particleManager->spawnParticle()->load(props);
}

void drawParticles()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureManager->getTexture("textures/game/explosion.png")->data);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDisable(GL_FOG);
	
	for (Particle *particle = (Particle*)particleManager->particleList.getFirstElement() ; particle != NULL ; particle = (Particle*)particle->next)
	{
		switch (particle->type)
		{
			case PT_NORMAL:
				drawNormalParticle(particle);
				break;
				
			case PT_HALO:
				glBindTexture(GL_TEXTURE_2D, textureManager->getTexture("textures/game/halo.png")->data);
				drawNormalParticle(particle);
				glBindTexture(GL_TEXTURE_2D, textureManager->getTexture("textures/game/explosion.png")->data);
				break;
				
			case PT_SPARK:
				drawSparkParticle(particle);
				break;
				
			default:
				break;
		}
	}
	
	glDepthMask(GL_TRUE);
	glEnable(GL_FOG);
}

void doParticles()
{
	float distance;
	
	for (Particle *particle = (Particle*)particleManager->particleList.getFirstElement() ; particle != NULL ; particle = (Particle*)particle->next)
	{
		distance = Math::getDistance(camera->position, particle->position);
		
		if ((particle->life <= 0) || (distance > 2000))
		{
			particle = (Particle*)particle->previous;
			particleManager->particleList.remove(particle->next);
			continue;
		}
		
		particle->life -= engine->getTimeDifference(TD_LOGIC);
		particle->position += (particle->velocity * engine->getTimeDifference(TD_LOGIC));
			
		if (!particle->bright)
		{
			particle->color.color[3] = particle->life / 100;
		}
		
		particle->size += (particle->growth * engine->getTimeDifference(TD_LOGIC));
	}
}
