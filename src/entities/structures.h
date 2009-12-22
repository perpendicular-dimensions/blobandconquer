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

extern void throwExplosionDebris(Vector position);
extern void doorOpen(Door *door);
extern void doorClose(Door *door);
extern void enableMultiTexturing();
extern void finishMultiTexturing();
extern void addGlassShard(Triangle *triangle);
extern void doExplosion(Vector position, int size, Entity *owner);
extern void fireTriggers(const char *name, int type);
extern void handleStructureStateChange(const char *name, bool active);
extern void toggleStructureState(const char *name);
extern void liftMainThink();
extern void liftTouch(Entity *other);
extern void toggleLiftPosition(const char *name, bool active);
extern void toggleLiftPosition(const char *name);
extern void moveEntity(Entity *entity);
extern void doDoor();
extern void doSwitch();
extern bool withinViewableRange(Entity *entity);
extern void enableEmitter(const char *name, bool active);
extern void toggleEmitter(const char *name);
extern void addTeleportParticles(Vector position);
extern WeaponDef *getRandomPlayerAmmoType();
extern void throwSpawnedItem(Entity *item);
extern void addHelperItems(int numberOfWeapon, WeaponDef *weaponDef, Vector position);
extern void removeTouchingDecals(Entity *ent);
extern Item *addStickyC4(Vector position);
extern void getCluster(Entity *entity);
extern void addGeneralDebris(Vector position, Vector velocity, float health, const char *textureName, float size, bool slides);

extern Audio *audio;
extern BoundingBoxManager *bbManager;
extern BSP *bsp;
extern Camera *camera;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern Graphics *graphics;
extern Mission *mission;
extern ModelManager *modelManager;
extern ParticleManager *particleManager;
extern TextureManager *textureManager;

extern Entity *self;
extern Unit *player;
