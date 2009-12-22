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

extern void addNormalBullet(Entity *owner, WeaponDef *definition, Vector origin, Vector destination);
extern void faceLocation(Boss *boss, Vector location);
extern void faceLocation(Unit *unit, Vector location);
extern void doExplosion(Vector position, int size, Entity *owner);
extern void addEnemy(Properties *props);
extern void addTeleportParticles(Vector position);
extern void addSparkParticle(Vector position);
extern void drawBoss(bool transparent);
extern void addShieldDestroyedParticles(Vector position);
extern void generalMechLargeExplosion();
extern void addTeleportParticles(Vector position);
extern char *extractEnemyFromList(const char *enemyList);
extern void entityDoNothing();
extern void blackDroidInit(Unit *unit, BossDef *bossDef);
extern Texture *getRequiredTexture(const char *textureFileName);
extern Decal *addGroundTargetMark(Vector position, Vector end);
extern void generalMechLargeExplosion();
extern void galdovPreMachineGunAttack();
extern void blackDroidInit(Unit *unit, BossDef *bossDef);
extern void assignBulletActionType(Bullet *bullet);
extern void blackDroidReleaseAndDie();
extern void addOnFireParticles();
extern void galdovCryptRemoveSupport();
extern void showMessageDialog(const char *message, const char *buttonLabel);
extern void entityDoNothing();
extern void addHalo(Vector position, float growSpeed);

extern Audio *audio;
extern Camera *camera;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern ParticleManager *particleManager;
extern TextureManager *textureManager;
extern Unit *player;

extern Entity *self;
