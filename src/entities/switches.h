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

extern void handleStructureStateChange(const char *name, bool active);
extern void toggleStructureState(const char *name);
extern void drawBox(Vector position, float width, float length, float height, float tcm);
extern void fireTriggers(const char *name, int type);
extern bool withinViewableRange(Entity *entity);
extern void resetStructures(const char *name);
extern void toggleLaserTrap(const char *name);
extern CutsceneCamera *getCutsceneCamera(const char *name);
extern void activateInGameCutscene(const char *name, float delay);
extern void getCluster(Entity *entity);
extern int doMasterMindPuzzle(int seed);
extern bool doLaserGridPuzzle(int seed);

extern Audio *audio;
extern BoundingBoxManager *bbManager;
extern Camera *camera;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern ModelManager *modelManager;
extern TextureManager *textureManager;

extern Entity *self;
extern Unit *player;
