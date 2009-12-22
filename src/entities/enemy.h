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

extern void pistolBlobInit(Unit *unit);
extern void grenadeBlobInit(Unit *unit);
extern void machineGunBlobInit(Unit *unit);
extern void plasmaPistolBlobInit(Unit *unit);
extern void shotGunBlobInit(Unit *unit);
extern void pistolDroidInit(Unit *unit);
extern void grenadeDroidInit(Unit *unit);
extern void machineGunDroidInit(Unit *unit);
extern void shotGunDroidInit(Unit *unit);
extern void fireTriggers(const char *name, int type);
extern void keepUnitAwayFromEdges(Unit *unit);
extern void addBloodParticle(float x, float y, float z);
extern char *extractEnemyFromList(const char *enemyList);
extern void addTeleportParticles(Vector position);
extern bool unitCanSeeTarget(Entity *observer, Entity *target, Vector startPosition);
extern void moveEntity(Entity *entity);
extern bool unitCanSpawnInLocation(Unit *unit, Vector location);
extern void addGravUnitParticles(Entity *entity, GLColor c1, GLColor c2);
extern void drawUnit(bool transparent);
extern bool withinViewableRange(Entity *entity);
extern void uziSentryGunInit(Unit *unit);
extern void plasmaSentryGunInit(Unit *unit);
extern void rocketSentryGunInit(Unit *unit);
extern void mortorSentryGunInit(Unit *unit);
extern void deadSentryGunInit(Unit *unit);
extern void darkBlobInit(Unit *unit);
extern void machineGunSpiderBlobInit(Unit *unit);
extern void shotGunSpiderBlobInit(Unit *unit);
extern void plasmaSpiderBlobInit(Unit *unit);
extern void babySpiderBlobInit(Unit *unit);
extern void faceTarget(Unit *unit);
extern void generalUnitVanish(Vector targetPosition, int timeout);
extern void addOnFireParticles();
extern void bioTankInit(Unit *unit);

extern Audio *audio;
extern BSP *bsp;
extern BoundingBoxManager *bbManager;
extern Camera *camera;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern Map *map;
extern Mission *mission;
extern RandomNameGenerator *randomNameGenerator;
extern Unit *player;

extern Entity *self;
