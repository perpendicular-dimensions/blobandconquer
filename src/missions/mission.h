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

extern List *loadProperties(const char *filename, bool fromPak);
extern void addFeature(Properties *props);
extern void addBlob(Properties *props);
extern Item *addItem(Properties *props);
extern Weapon *addWeapon(Properties *props);
extern void addDoor(Properties *props);
extern void addTrigger(Properties *props);
extern void loadBSP(const char *filename);
extern List *loadBSPProperties(const char *entities);
extern void addEnemy(Properties *props);
extern void spawnBob();
extern void addStructure(Properties *props);
extern void addSwitch(Properties *props);
extern void addSpecialSwitch(Properties *props);
extern void addWindow(Properties *props);
extern void addEnergyBarrier(Properties *props);
extern void addLift(Properties *props);
extern void addWeakWall(Properties *props);
extern void addPressurePlate(Properties *props);
extern void addDemolitionPlate(Properties *props);
extern void addPushBlock(Properties *props);
extern void addAutoLift(Properties *props);
extern void addCustomItem(Properties *props);
extern void reassignOwnerShips();
extern void removeBSPModelFaces();
extern void addBullet(Properties *props);
extern void addParticle(Properties *props);
extern void addDecoration(Properties *props);
extern void fireTriggers(const char *name, int type);
extern CutsceneCamera *addCutsceneCamera(Properties *props);
extern void addEmitter(Properties *props);
extern void addWeakStructure(Properties *props);
extern void addTrap(Properties *props);
extern void addLocation(Properties *props);
extern void addDecal(Properties *props);
extern void assignTriggerLocations();
extern void addSupplyCrate(Properties *props);
extern void addBoss(Properties *props);
extern void loadCutscene(const char *filename, List *data);
extern void loadBossDefinition(const char *filename);
extern void localizeInfoPoints(const char *missionName);
extern void loadAmbientSounds();

extern BSP *bsp;
extern CrumbBox crumbBox;
extern EntityManager *entityManager;
extern Engine *engine;
extern Game *game;
extern Graphics *graphics;
extern Mission *mission;
extern TextureManager *textureManager;

extern Unit *player;
