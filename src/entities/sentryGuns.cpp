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

#include "sentryGuns.h"
		
void sentryGunDead()
{
	if (self->custom == 0)
	{
		addSmokeParticle(self->position);
		self->thinkTime = rand() % 10;
	}
	
	self->health -= engine->getTimeDifference(TD_LOGIC);
	
	if (self->health <= -350)
	{
		generalMechLargeExplosion();
	}
}
		
void drawSentryGun(bool transparent)
{
	if (transparent)
	{
		return;
	}
	
	Unit *unit = (Unit*)self;
	
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	glColor3f(1.0, 1.0, 1.0);
	
	glPushMatrix();
	{
		glTranslatef(unit->position.x, unit->position.y, unit->position.z);
		glScalef(unit->scale, unit->scale, unit->scale);
		glBindTexture(GL_TEXTURE_2D, unit->definition->texture[0]->data);
		glCallList(unit->definition->model[0]->data);
	}
	glPopMatrix();
	
	if (self->action == sentryGunDead)
	{
		return;
	}
	
	float z = unit->rotation.z;
	
	glPushMatrix();
	{
		glTranslatef(unit->position.x, unit->position.y, unit->position.z + 9);
		glRotatef(unit->rotation.x, 0.0, 0.0, 1.0);
		glRotatef(unit->rotation.y, 0.0, 1.0, 0.0);
		glRotatef(z, 1.0, 0.0, 0.0);
		glScalef(unit->scale, unit->scale, unit->scale);
		glBindTexture(GL_TEXTURE_2D, unit->definition->texture[1]->data);
		glCallList(unit->definition->model[1]->data);
	}
	glPopMatrix();
}

void sentryGunDie2()
{
	Vector v;

	v = self->position;
	v.x += Math::rrand(-3, 3);
	v.y += Math::rrand(-3, 3);
	
	addSmokeParticle(v);
	
	self->custom -= engine->getTimeDifference(TD_LOGIC);
	
	if ((int)self->rotation.z != 15)
	{
		if (self->rotation.z > 15)
		{
			self->rotation.z -= engine->getTimeDifference(TD_LOGIC) * 0.25;
		}
		else
		{
			self->rotation.z += engine->getTimeDifference(TD_LOGIC) * 0.25;
		}
	}
	
	if (self->custom <= 0)
	{
		generalMechLargeExplosion();
		
		self->flags &= ~EF_DYING;
		self->flags &= ~EF_DEAD;
		self->walk = &sentryGunDead;
		self->attack = &sentryGunDead;
		self->die = &sentryGunDead;
		self->action = &sentryGunDead;
		self->boundingBox.boxType = BOX_HOLLOW;
		
		if ((rand() % 2) == 0)
		{
			Vector velocity;
			velocity.x = Math::rrand(-10, 10);
			velocity.y = Math::rrand(-10, 10);
			velocity.z = Math::rrand(15, 25);
			velocity.x *= 0.1;
			velocity.y *= 0.1;
			velocity.z *= 0.1;
			
			Vector position = self->position;
			position.z += 9;
			
			Properties props;
			props.setProperty("classname", "NME_DEAD_SENTRYGUN");
			props.setProperty("position", position);
			props.setProperty("velocity", velocity);
			props.setProperty("helpless", 9999);
			addEnemy(&props);
		}
		
		switch (rand() % 3)
		{
			case 0:
				self->custom = 0;
				break;
			case 1:
				self->flags |= EF_ONFIRE;
				break;
		}
	}
}

void sentryGunDie1()
{
	Vector v;

	v = self->position;
	
	v.x += Math::rrand((int)self->boundingBox.mins.x, (int)self->boundingBox.maxs.x);
	v.y += Math::rrand((int)self->boundingBox.mins.y, (int)self->boundingBox.maxs.y);
	v.z += Math::rrand((int)self->boundingBox.mins.z, (int)self->boundingBox.maxs.z);
	doExplosion(v, 10, self);
	
	self->thinkTime = Math::rrand(15, 30);
	
	if (--self->custom == 0)
	{
		self->action = &sentryGunDie2;
		
		self->custom = 200;
	}
}

void sentryGunPostDie()
{
	self->health = 0;
	
	self->thinkTime = 0;
	
	self->action = sentryGunDie1;
	
	self->custom = Math::rrand(3, 5);
	
	self->flags |= EF_DYING;
}

void sentryGunSearch()
{
	Unit *unit = (Unit*)self;
	
	if (unit->flags & EF_NOTARGET)
	{
		return;
	}
	
	if (unit->target != NULL)
	{
		faceTarget(unit);
	}
	
	if (enemySightTarget(unit, 650))
	{
		self->action = self->attack;
	}
	
	if (unit->health <= 0)
	{
		unit->action = &sentryGunDead;
	}
	
	self->thinkTime = Math::rrand(25, 75);
}

// ==================== Uzi sentry gun =================

void uziSentryGunAttack()
{
	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &uziSentryGunAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &sentryGunSearch;
		self->thinkTime = Math::rrand(50, 100);
	}
}

void uziSentryGunPreAttack()
{
	self->custom = Math::rrand(10, 20);
	self->thinkTime = 0;
	self->action = &uziSentryGunAttack;
}

void uziSentryGunInit(Unit *unit)
{
	unit->walk = &sentryGunSearch;
	unit->attack = &uziSentryGunPreAttack;
	unit->die = &sentryGunPostDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	unit->draw = &drawSentryGun;
}

// ==================== Plasma sentry gun =================

void plasmaSentryGunAttack()
{
	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &plasmaSentryGunAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &sentryGunSearch;
		self->thinkTime = Math::rrand(50, 100);
	}
}

void plasmaSentryGunPreAttack()
{
	self->custom = Math::rrand(5, 20);
	self->thinkTime = 0;
	self->action = &plasmaSentryGunAttack;
}

void plasmaSentryGunInit(Unit *unit)
{
	unit->walk = &sentryGunSearch;
	unit->attack = &plasmaSentryGunPreAttack;
	unit->die = &sentryGunPostDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	unit->draw = &drawSentryGun;
}

// ==================== Rocket sentry gun =================

void rocketSentryGunAttack()
{
	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &rocketSentryGunAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &sentryGunSearch;
		self->thinkTime = Math::rrand(300, 500);
	}
}

void rocketSentryGunPreAttack()
{
	self->custom = Math::rrand(1, 2);
	self->thinkTime = 0;
	self->action = &rocketSentryGunAttack;
}

void rocketSentryGunInit(Unit *unit)
{
	unit->walk = &sentryGunSearch;
	unit->attack = &rocketSentryGunPreAttack;
	unit->die = &sentryGunPostDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	unit->draw = &drawSentryGun;
}

// ==================== Mortor sentry gun =================

void mortorSentryGunAttack()
{
	Unit *unit = (Unit*)self;

	faceTarget(unit);

	if (unit->reload > 0)
	{
		return;
	}

	self->thinkTime = 0;
	self->action = &mortorSentryGunAttack;

	attackTarget(unit);

	if (--self->custom == 0)
	{
		self->action = &sentryGunSearch;
		self->thinkTime = Math::rrand(50, 100);
	}
}

void mortorSentryGunPreAttack()
{
	self->custom = Math::rrand(1, 3);
	self->thinkTime = 0;
	self->action = &mortorSentryGunAttack;
}

void mortorSentryGunInit(Unit *unit)
{
	unit->walk = &sentryGunSearch;
	unit->attack = &mortorSentryGunPreAttack;
	unit->die = &sentryGunPostDie;

	unit->thinkTime = Math::rrand(0, 50);

	unit->action = unit->walk;
	
	unit->draw = &drawSentryGun;
}

// ============= Dead Sentry Gun ============

void deadSentryGunTouch(Entity *other)
{
	if (other == self->owner)
	{
		return;
	}
	
	generalMechLargeExplosion();
	self->touch = NULL;
}

void deadSentryGunInit(Unit *unit)
{
	unit->walk = &sentryGunDead;
	unit->attack = &sentryGunDead;
	unit->die = &sentryGunDead;
	unit->touch = &deadSentryGunTouch;
	unit->boundingBox.boxType = BOX_HOLLOW;
	unit->owner = self;
}
