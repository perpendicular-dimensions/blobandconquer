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

extern void addBulletHitParticles(Bullet *bullet);
extern void addBloodParticles(Bullet *bullet);
extern void addShieldHitParticles(Vector position);
extern void addShieldDestroyedParticles(Vector position);
extern void doExplosion(Vector position, int size, Entity *owner);
extern void addSparkParticle(Vector position);
extern bool withinViewableRange(Vector position, float size);
extern void addBubbleParticle(Vector position);
extern void throwUnit(Unit *unit, Vector velocity);
extern void addTrailParticle(Bullet *bullet);
extern void moveEntity(Entity *entity);
extern bool withinViewableRange(Entity *entity);
extern void generalUnitFrozen();
extern void addBlob(Properties *props);
extern void addDesimilatedParticles(Vector position);
extern void fireTriggers(const char *name, int type);
extern void addBloodSplat(Vector position, float size);
extern void addBulletHole(Vector position, Vector heading);
extern void addPlasmaHitParticles(Vector position);
extern void addIceHitParticles(Vector position);
extern void addBulletMark(Vector position, Vector heading);
extern void generalBlobFirePanic();
extern void generalUnitWrapped();
extern void addPuffParticles(Vector position, GLColor color, int amount, int minSize, int maxSize);
extern void entityDoNothing();
extern void addDesimilatorParticles(Vector position);

extern Audio *audio;
extern BoundingBoxManager *bbManager;
extern Camera *camera;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern ModelManager *modelManager;
extern TextureManager *textureManager;
extern Unit *player;

extern Entity *self;
