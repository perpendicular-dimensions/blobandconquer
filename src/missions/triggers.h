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
extern void enableObjective(int id);
extern void updateObjectiveStatus(int id, int status);
extern void enableFeature(const char *name, bool enable);
extern bool updateObjectiveCurrentValue(int id);
extern void teleportOutBlob(const char *name);
extern void activateInGameCutscene(const char *name, float delay);
extern void destroyStructure(const char *name);

extern Audio *audio;
extern BSP *bsp;
extern Engine *engine;
extern Game *game;
extern Graphics *graphics;
extern Mission *mission;
extern ModelManager *modelManager;
extern Unit *player;
