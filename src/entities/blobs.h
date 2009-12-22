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

extern void generalBlobDie();
extern void faceLocation(Unit *unit, Vector location);
extern void faceTarget(Unit *unit);
extern void attackTarget(Unit *unit);
extern void attackTargetLeading(Unit *unit);
extern void addTeleportParticles(Vector position);
extern void fireTriggers(const char *name, int type);
extern void keepUnitAwayFromEdges(Unit *unit);
extern void unitContentChanged(Unit *unit);
extern void handleSwitchUse(Switch *swt);
extern bool unitCanSeeTarget(Entity *observer, Entity *target, Vector startPosition);
extern bool unitIsAtEdge(Unit *unit);
extern void moveEntity(Entity *entity);
extern bool unitCanSpawnInLocation(Unit *unit, Vector location);
extern void addTeleporterParticle(Vector position);
extern void drawUnit(bool transparent);
extern bool withinViewableRange(Entity *entity);
extern bool completesSwitchPuzzle(Switch *swt);
extern void addOnFireParticles();
extern Item *addStickyC4(Vector position);

extern Audio *audio;
extern BSP *bsp;
extern BoundingBoxManager *bbManager;
extern Camera *camera;
extern CrumbBox crumbBox;
extern EntityManager *entityManager;
extern Engine *engine;
extern Game *game;
extern Map *map;
extern Mission *mission;
extern RandomNameGenerator *randomNameGenerator;
extern TextureManager *textureManager;
extern Unit *player;

extern Entity *self;
