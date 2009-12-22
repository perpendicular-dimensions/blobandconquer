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

extern Unit *addEnemy(Properties *props);
extern Unit *addBlob(Properties *props);
extern void addTeleportParticles(Vector position);
extern void addTeleporterParticle(Vector position);
extern void drawCyclinder(Vector position, int segments, float size, float height, GLColor c);
extern void showMessageDialog(const char *message, const char *buttonLabel);
extern bool saveGame();
extern void teleportUnit();
extern void faceLocation(Unit *unit, Vector location);
extern bool withinViewableRange(Entity *entity);
extern int getBlobSoldierCount();
extern Unit *findTeeka();
extern void getCluster(Entity *entity);
extern Bullet *addGrapplingHook(Entity *owner, Vector destination);
extern void faceLocation(Unit *unit, Vector location);

extern Audio *audio;
extern BoundingBoxManager *bbManager;
extern Camera *camera;
extern CrumbBox crumbBox;
extern EntityManager *entityManager;
extern Engine *engine;
extern Game *game;
extern Mission *mission;
extern Pak *pak;
extern RandomNameGenerator *randomNameGenerator;
extern TextureManager *textureManager;
extern Unit *player;

extern Entity *self;
