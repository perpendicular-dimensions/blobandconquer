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

#include <dirent.h>

extern BaseWidget *createButtonWidget(Properties *props);
extern void drawWidgets();
extern void drawGameObjects();
extern void addSaveHeaderToList(ListView *listView, SaveGameHeader *header, int i);
extern void removeSaveFromList(int id);
extern void moveToWidget(int x, int y);

extern Engine *engine;
extern Game *game;
extern Graphics *graphics;
extern Mouse *mouse;
extern TextureManager *textureManager;
extern UIManager *uiManager;
extern Unit *player;
