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

#include "headers.h"

#include "locale.h"

extern void initSystem();
extern void initOpenGL();
extern void initCommonResources();
extern void cleanup();

extern void loadAllDefinitions();

extern void loadGameData(int slot, bool restart);


extern void loadMission(const char *mission);

extern int mainBattleLoop();

extern void removeUnneededResources();
extern void initCutsceneInstructions();

extern int doTitle();
extern void doIntro();
extern void doCredits();
extern int doGameOver();

extern bool doMasterMindPuzzle(int seed);
extern bool doLaserGridPuzzle(int seed);
extern void loadWidgets(const char *filename);

// initialise all singletons
Audio *audio = Audio::getInstance();
BSP *bsp = BSP::getInstance();
BoundingBoxManager *bbManager = BoundingBoxManager::getInstance();
CrumbBox crumbBox;
Engine *engine = Engine::getInstance();
EntityManager *entityManager = EntityManager::getInstance();
Game *game = Game::getInstance();
Graphics *graphics = Graphics::getInstance();
Map *map = Map::getInstance();
Mission *mission = Mission::getInstance();
ModelManager *modelManager = ModelManager::getInstance();
Mouse *mouse = Mouse::getInstance();
Pak *pak = Pak::getInstance();
ParticleManager *particleManager = ParticleManager::getInstance();
RandomNameGenerator *randomNameGenerator = RandomNameGenerator::getInstance();
TextureManager *textureManager = TextureManager::getInstance();
TextureSpecManager *textureSpecManager = TextureSpecManager::getInstance();
Trace *tracer = new Trace();
UIManager *uiManager = UIManager::getInstance();

Camera *camera = Camera::getInstance();
Unit *player = NULL;
Entity *self = NULL;
