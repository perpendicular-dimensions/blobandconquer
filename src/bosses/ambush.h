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

extern void doExplosion(Vector position, int size, Entity *owner);
extern void throwExplosionDebris(Vector position);
extern void addTeleportParticles(Vector position);
extern void addSparkParticle(Vector position);
extern void drawBoss(bool transparent);
extern void addTeleportParticles(Vector position);
extern void galdovPreMachineGunAttack();
extern void bobStunnedTouch(Entity *other);

extern Audio *audio;
extern BSP *bsp;
extern Camera *camera;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern Mission *mission;
extern ParticleManager *particleManager;
extern Trace *tracer;
extern Unit *player;

extern Entity *self;
