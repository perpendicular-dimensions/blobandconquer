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

extern void loadWidgets(const char *filename);
extern void saveGameData(bool autoSaving);
extern void saveGame();
extern void doBullets();
extern void doItems();
extern void doDecoration();
extern void doParticles();
extern void doEmitters();
extern void loadAllBattleResources();
extern void loadMapAudio();
extern void doPlayer();
extern void doBullets();
extern void drawInfoPanel();
extern void drawSelectedUnit();
extern void doEnemies();
extern void doBlobs();
extern void doTriggers();
extern void doFeatures();
extern void renderBSP();
extern void drawSkyDome(Vector position, int size, Texture *texture);
extern void resetPlayer();
extern void resetEnemyThinkTimes();
extern void doWeather();
extern void doStructures();
extern void doInGameOptions();
extern int doGameOver();
extern void showMessageDialog(const char *message, const char *buttonLabel);
extern void spawnRandomEnemy();
extern void doInGameCutscene();
extern void drawParticles();
extern void drawBSP(bool drawBlended);
extern void drawShadow();
extern void drawDecals();
extern bool isOkayToSpawnEnemies();
extern void doTraps();
extern void doDecals();
extern void doExplosion(Vector position, int size, Entity *owner);
extern void disableSavePoints();
extern void doBosses();
extern void removeSpecialWeapons();
extern void processCutscene(List *data);
extern void saveRestartData();
extern void drawBox(Vector position, float width, float length, float height, float tcm);
extern void doCamera();
extern void calculatePlayerAimDirection();
extern void drawTitle();

extern Audio *audio;
extern BSP *bsp;
extern BoundingBoxManager *bbManager;
extern Camera *camera;
extern CrumbBox crumbBox;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern Graphics *graphics;
extern Mission *mission;
extern Mouse *mouse;
extern TextureManager *textureManager;
extern TextureSpecManager *textureSpecManager;
extern UIManager *uiManager;
extern Unit *player;

extern Entity *self;
