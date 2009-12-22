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

extern void addTeleportParticles(Vector position);
extern void drawBox(Vector position, float width, float length, float height, float tcm);
extern void addGeneralDebris(Vector position, Vector velocity, float health, const char *textureName, float size, bool slides);
extern void drawMD2(Entity *ent);
extern void addPuffParticles(Vector position, GLColor color, int amount, int minSize, int maxSize);

extern Audio *audio;
extern BoundingBoxManager *bbManager;
extern BSP *bsp;
extern Camera *camera;
extern Engine *engine;
extern Game *game;
extern Graphics *graphics;
extern ModelManager *modelManager;
extern TextureManager *textureManager;
extern Trace *tracer;
extern Unit *player;

extern Entity *self;
