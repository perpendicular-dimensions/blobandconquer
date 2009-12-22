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

extern void faceLocation(Unit *unit, Vector location);
extern void faceTarget(Unit *unit);
extern void throwFleshChunk(Vector position, bool underWater);
extern void doExplosion(Vector position, int size, Entity *owner);
extern void addHelperItems(int numberOfWeapon, WeaponDef *weaponDef, Vector position);
extern WeaponDef *getRandomPlayerAmmoType();
extern void addBloodSplat(Vector position, float size);
extern void fireWeapon(Unit *owner, WeaponDef *definition, Vector origin, Vector destination);
extern void addLauncherShot(Entity *owner, WeaponDef *weaponDef, Vector origin, Vector destination);
extern void addWeb(Entity *owner, Vector origin, Vector destination);
extern void bioMechSearch();
extern Vector getLeadingTargetPosition(Entity *attacker, Entity *target);
extern void generalMechLargeExplosion();
extern void addSmokeParticle(Vector position);
extern void throwFleshChunk(Vector position, bool underWater);
extern void throwNBFleshChunk(Vector position);
extern void generalDroidDie();
extern Bullet *addGrapplingHook(Entity *owner, Vector destination);
extern Vector getGunPosition(Unit *unit, int i);

extern Audio *audio;
extern Camera *camera;
extern EntityManager *entityManager;
extern Engine *engine;
extern Game *game;
extern Mission *mission;
extern Unit *player;

extern Entity *self;
