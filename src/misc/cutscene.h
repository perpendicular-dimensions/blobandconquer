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
		
class SceneInstruction {
	
	public:
		
		String name;
		void (*function)(char *args);
		
	SceneInstruction()
	{
		function = NULL;
	}
	
	void set(const char *name, void (*function)(char *args))
	{
		this->name = name;
		this->function = function;
	}
};

SceneInstruction sceneInstruction[256];

extern void doGameObjects();
extern void drawGameObjects();
extern void resetEnemyThinkTimes();
extern void fireTriggers(const char *name, int type);
extern void handleStructureStateChange(const char *name, bool active);
extern void changeShieldState(const char *name, int state);
extern void toggleLaserTrap(const char *name);
extern void drawGameObjects();
extern void faceLocation(Unit *unit, Vector location);
extern void faceLocation(Boss *boss, Vector location);
extern void addTeleportParticles(Vector position);
extern Texture *getRequiredTexture(const char *textureFileName);
extern void renderScaledTexture(Texture *texture);
extern void addPuffParticles(Vector position, GLColor color, int amount, int minSize, int maxSize);
extern void doExplosion(Vector position, int size, Entity *owner);
extern void addDebrisEmitter(Vector position);
extern void throwExplosionDebris(Vector position);
extern Unit *addEnemy(Properties *props);
extern char *extractEnemyFromList(const char *enemyList);
extern void drawBullet(bool transparent);
extern void addDesimilatedParticles(Vector position);
extern void addBlob(Properties *props);
extern void energyBallRotate();

extern Audio *audio;
extern BoundingBoxManager *bbManager;
extern Camera *camera;
extern Engine *engine;
extern EntityManager *entityManager;
extern Game *game;
extern Graphics *graphics;
extern Mission *mission;
extern Pak *pak;
extern ParticleManager *particleManager;
extern TextureManager *textureManager;
extern Unit *player;

extern Entity *self;
