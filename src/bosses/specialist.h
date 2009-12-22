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

extern void faceLocation(Boss *boss, Vector location);
extern void faceLocation(Unit *unit, Vector location);
extern float bossSightTarget(Boss *boss, int range);
extern void addTeleportParticles(Vector position);
extern void drawBoss(bool transparent);
extern void addTeleportParticles(Vector position);
extern Unit *addEnemy(Properties *props);
extern char *extractEnemyFromList(const char *enemyList);
extern Weapon *addWeapon(Properties *props);
extern void addIceShot(Entity *owner, Vector origin, Vector destination);
extern void addFlame(Entity *owner, Vector origin, Vector destination);
extern void generalUnitVanish(Vector targetPosition, int timeout);
extern void frozenBlobDie();
extern void onFireBlobDie();
extern void throwSpawnedItem(Entity *item);
extern void addGeneralDebris(Vector position, Vector velocity, float health, const char *textureName, float size, bool slides);

extern Audio *audio;
extern Camera *camera;
extern EntityManager *entityManager;
extern Game *game;
extern TextureManager *textureManager;
extern Unit *player;

extern Entity *self;
