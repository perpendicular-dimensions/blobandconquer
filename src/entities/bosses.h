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

extern void eyeDroidCommanderInit(Boss *boss);
extern void spiderBlobCommanderInit(Boss *boss);
extern void ionCannonInit(Boss *boss);
extern void bioTank1Init(Boss *boss);
extern void bioTank2Init(Boss *boss);
extern void bioTank3Init(Boss *boss);
extern void frostInit(Boss *boss);
extern void blazeInit(Boss *boss);
extern void cryptBossInit(Boss *boss);
extern void amushBossInit(Boss *boss);
extern void finalBossInit(Boss *boss);
extern bool withinViewableRange(Entity *entity);
extern void addGravUnitParticles(Entity *entity, GLColor c1, GLColor c2);
extern void moveEntity(Entity *entity);
extern void addSmokeParticle(Vector position);
extern void fireTriggers(const char *name, int type);
extern bool unitCanSeeTarget(Entity *observer, Entity *target, Vector startPosition);
extern void addOnFireParticles();
extern void drawBox(Vector position, float width, float length, float height, float tcm);
extern void drawMD2(Entity *ent);
extern void calculateFaceLocation(Vector start, Vector end, Vector *v);
extern void faceLocation(Boss *boss, Vector position);

extern BoundingBoxManager *bbManager;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern Graphics *graphics;
extern Mission *mission;
extern ParticleManager *particleManager;
extern TextureManager *textureManager;
extern Unit *player;

extern Entity *self;
