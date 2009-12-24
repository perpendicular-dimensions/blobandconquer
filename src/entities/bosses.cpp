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

#include "bosses.h"
		
void generalBossEffect()
{
	float percent = self->getHealthPercent();
			
	if (percent <= 10)
	{
		addSmokeParticle(self->position);
	}
	else if ((percent <= 25) && ((rand() % 5) == 0))
	{
		addSmokeParticle(self->position);
	}
	else if ((percent <= 50) && ((rand() % 10) == 0))
	{
		addSmokeParticle(self->position);
	}
	else if ((percent <= 75) && ((rand() % 15) == 0))
	{
		addSmokeParticle(self->position);
	}
	
	Boss *boss = (Boss*)self;
	
	if (boss->flags & EF_WEIGHTLESS)
	{
		addGravUnitParticles(boss, GLColor::skyBlue, GLColor::green);
	}
}

void spiderBlobCommanderEffect()
{
	Boss *boss = (Boss*)self;
		
	if (boss->shield <= 0)
	{
		boss->shield = -9999;
		
		for (int i = 0 ; i < 5 ; i++)
		{
			addSmokeParticle(self->position);
		}
	}
}

void frostEffect()
{
	if (self->flags & EF_ONFIRE)
	{
		addOnFireParticles();
		return;
	}
	
	Particle *particle = particleManager->spawnParticle();

	particle->position = self->position;
	particle->position.x += Math::rrand(-2, 2);
	particle->position.y += Math::rrand(-2, 2);
	particle->position.z += Math::rrand(0, 3);

	particle->life = Math::rrand(15, 65);
	particle->size = Math::rrand(1, 2);

	float speed = Math::rrand(1, 2);
	speed /= 25;

	particle->velocity.set(0, 0, speed);

	float r = 0.5;
	float g = 0.5;
	float b = 1.0;

	switch (rand() % 3)
	{
		case 0:
			particle->color.set(r, g, b, 0.1);
			break;
	
		case 1:
			particle->color.set(r * 2, g * 2, b * 2, 0.1);
			break;
	
		case 2:
			particle->color.set(r * 4, g * 4, b * 4, 0.1);
			break;
	}
}

void galdovChargeEffect(Boss *boss, int amount)
{
	if (boss->energyCharge > 0)
	{
		Particle *particle = particleManager->spawnParticle();
		particle->position = boss->position;
		particle->position.z += Math::rrand(0, 3);
		particle->life = Math::rrand(100, 200);
		particle->size = Math::rrand(1, amount);
		particle->color.color[0] = rand() % 100;
		particle->color.color[1] = rand() % 100;
		particle->color.color[2] = rand() % 100;
		particle->color.color[0] /= 100;
		particle->color.color[1] /= 100;
		particle->color.color[2] /= 100;
		
		particle->velocity.set(Math::rrand(-1, 1), Math::rrand(-1, 1), Math::rrand(-1, 1));
		particle->velocity.x /= 10;
		particle->velocity.y /= 10;
		particle->velocity.z /= 10;
	}
}

void cryptBossEffect()
{
	Boss *boss = (Boss*)self;
	
	if (!(boss->flags & EF_WEIGHTLESS))
	{
		addSmokeParticle(self->position);
	}
	
	galdovChargeEffect(boss, (int)boss->energyCharge);
}

//
// Does nothing on purpose...
//
void amushBossEffect()
{
}

void finalBossEffect()
{
	Boss *boss = (Boss*)self;
	
	if ((!(boss->flags & EF_WEIGHTLESS)) || (boss->health <= 1))
	{
		addOnFireParticles();
	}
	
	galdovChargeEffect(boss, (int)boss->energyCharge / 4);
}

void addBoss(Properties *props)
{
	Boss *boss = entityManager->spawnBoss();
	
	boss->position = props->getVector("origin");
	boss->autoSaveToken = props->name.getText();
	boss->bossEffect = &generalBossEffect;
	
	if (props->name == "BOSS_EYEDROID_COMMANDER")
	{
		eyeDroidCommanderInit(boss);
	}
	else if (props->name == "BOSS_ION_CANNON")
	{
		ionCannonInit(boss);
	}
	else if (props->name == "BOSS_BIOTANK_1")
	{
		bioTank1Init(boss);
	}
	else if (props->name == "BOSS_BIOTANK_2")
	{
		bioTank2Init(boss);
	}
	else if (props->name == "BOSS_BIOTANK_3")
	{
		bioTank3Init(boss);
	}
	else if (props->name == "BOSS_FROST")
	{
		frostInit(boss);
		boss->bossEffect = &frostEffect;
	}
	else if (props->name == "BOSS_BLAZE")
	{
		blazeInit(boss);
		boss->bossEffect = &addOnFireParticles;
	}
	else if (props->name == "BOSS_SPIDER_COMMANDER")
	{
		spiderBlobCommanderInit(boss);
		boss->bossEffect = &spiderBlobCommanderEffect;
	}
	else if (props->name == "BOSS_CRYPT")
	{
		cryptBossInit(boss);
		boss->bossEffect = &cryptBossEffect;
	}
	else if (props->name == "BOSS_AMBUSH")
	{
		amushBossInit(boss);
		boss->bossEffect = &amushBossEffect;
	}
	else if (props->name == "BOSS_FINAL")
	{
		finalBossInit(boss);
		boss->bossEffect = &finalBossEffect;
	}
	else
	{
		printf("ERROR: addBoss - '%s' not a recognised boss type\n", props->name.getText());
		exit(1);
	}
	
	boss->load(props);
}

float bossSightTarget(Boss *boss, int range)
{
	boss->target = NULL;
	
	float distance = 0;
	float targetDistance = range;
	Vector sight;

	for (Unit *blob = (Unit*)entityManager->blobList.getFirstElement() ; blob != NULL ; blob = (Unit*)blob->next)
	{
		if (!blob->canBeDamaged())
		{
			continue;
		}
		
		distance = Math::getDistance(boss->position, blob->position);
		
		if (distance < targetDistance)
		{
			sight = boss->position;
			sight.z += 6;
			
			if (!unitCanSeeTarget(boss, blob, sight))
			{
				continue;
			}
			
			targetDistance = distance;
			boss->target = blob;
		}
	}
	
	return targetDistance;
}

void drawBoss(bool transparent)
{
	Boss *boss = (Boss*)self;
	
	if (!transparent)
	{
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		
		glBindTexture(GL_TEXTURE_2D, boss->definition->texture[0]->data);
		
		glPushMatrix();
		{
			glTranslatef(boss->position.x, boss->position.y, boss->position.z);
			
			if (boss->helpless)
			{
				boss->rotation.x += (3 * engine->getTimeDifference(TD_LOGIC));
				boss->rotation.y += (3 * engine->getTimeDifference(TD_LOGIC));
			}
			
			glRotatef(boss->rotation.x, 0.0, 0.0, 1.0);
			glRotatef(boss->rotation.y, 0.0, 1.0, 0.0);
			
			glColor3f(1.0, 1.0, 1.0);
			glScalef(boss->scale, boss->scale, boss->scale);
			
			if (boss->definition->model[0] != NULL)
			{
				glCallList(boss->definition->model[0]->data);
			}
			else if (boss->definition->md2 != NULL)
			{
				drawMD2(boss);
			}
			else
			{
				printf("No model for %s\n", boss->getName());
				exit(1);
			}
		}
		glPopMatrix();
	}
	else
	{
		if (boss->shieldHit)
		{
			glPushMatrix();
			{
				glTranslatef(boss->position.x, boss->position.y, boss->position.z);
			
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
			
				GLColor c = GLColor::blue;
				c.color[3] = boss->shieldHit;
				glColor4fv(c.color);
			
				gluSphere(graphics->quadric, boss->boundingBox.getHorizontalSize(), 32, 16);
			
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
			}
			glPopMatrix();
		
			boss->shieldHit -= (0.01 * engine->getTimeDifference(TD_LOGIC));
			Math::limit(&boss->shieldHit, 0, 2.0);
		}
		
		if (boss->flags & EF_FROZEN)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			
			glBindTexture(GL_TEXTURE_2D, textureManager->getTexture("textures/game/iceBlock.jpg")->data);
			
			float size = boss->boundingBox.maxs.x * 2;
			
			glColor3f(0.5, 0.5, 0.5);
			drawBox(boss->position, size, size, size, 1);
		}
	}
}

void doBosses()
{
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		self = boss;
		
		//self->riding = NULL;
		
		bbManager->removeBox(boss);
		
		if (boss->flags & EF_DEAD)
		{
			if (!boss->dead)
			{
				fireTriggers(boss->getName(), TRIGGER_TYPE_ENTITY_KILLED);
				boss->dead = true;
			}
			
			if (!boss->referenced)
			{
				boss = (Boss*)boss->previous;
				entityManager->bossList.remove(boss->next);
			}
			else
			{
				boss->referenced = false;
			}
			
			continue;
		}
		
		if (boss->target != NULL)
		{
			boss->target->referenced = true;
		}
		
		if (boss->owner != NULL)
		{
			boss->owner->referenced = true;
		}
		
		boss->referenced = false;
		
		boss->think(engine->getTimeDifference(TD_LOGIC));
		
		if ((!game->allStatic) && (game->cutsceneType != CUTSCENE_INGAME))
		{
			if (boss->performNextAction(engine->getTimeDifference(TD_LOGIC)))
			{
				if (boss->action != NULL)
				{
					boss->action();
				}
			}
		}
		
		if (boss->flags & EF_VANISHED)
		{
			continue;
		}
		
		if (withinViewableRange(boss))
		{
			entityManager->addEntityToDrawList(boss);
			
			if (boss->bossEffect != NULL)
			{
				boss->bossEffect();
			}
		}
		
		boss->applyGravity(engine->getTimeDifference(TD_LOGIC));
		
		moveEntity(boss);
		
		if (boss->health > 0)
		{
			if (boss->contentType & CONTENTS_WATER)
			{
				boss->thinkTime = 0;
				boss->action = boss->walk;
			}
		}
		
		if (boss->flags & EF_HAS_TURRET)
		{
			if (boss->target != NULL)
			{
				calculateFaceLocation(boss->position, boss->target->position, &boss->cannonRotation);
			}
		}
		
		if (game->cutsceneType == CUTSCENE_NONE)
		{
			if ((boss->flags & EF_ALWAYS_FACE) && (boss->health > 0))
			{
				faceLocation(boss, player->position);
			}
		}
		
		bbManager->addBox(boss);
	}
}
