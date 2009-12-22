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

#include "dialogs.h"

void getMouseCursorMovement(int *x, int *y)
{
	static float analogTimer = 0;
	
	analogTimer -= engine->getTimeDifference(TD_ANIMATION);
	Math::limit(&analogTimer, 0, 50);
	
	*x = *y = 0;
	
	if (analogTimer <= 0)
	{
		if (engine->axis1X >= game->joystickDeadZone)
		{
			*x = 1;
			analogTimer = 20;
		}
		
		if (engine->axis1X <= -game->joystickDeadZone)
		{
			*x = -1;
			analogTimer = 20;
		}
		
		if (engine->axis1Y >= game->joystickDeadZone)
		{
			*y = 1;
			analogTimer = 20;
		}
		
		if (engine->axis1Y <= -game->joystickDeadZone)
		{
			*y = -1;
			analogTimer = 20;
		}
	}
	
	if (game->isControl(CTRL_LEFT))
	{
		*x = -1;
	}
	
	if (game->isControl(CTRL_RIGHT))
	{
		*x = 1;
	}
	
	if (game->isControl(CTRL_UP))
	{
		*y = -1;
	}
	
	if (game->isControl(CTRL_DOWN))
	{
		*y = 1;
	}
	
	Math::limit(x, -1, 1);
	Math::limit(y, -1, 1);
	
	game->resetControl(CTRL_LEFT);
	game->resetControl(CTRL_RIGHT);
	game->resetControl(CTRL_UP);
	game->resetControl(CTRL_DOWN);
}

void moveMouseCursor()
{
	int x, y;
	
	getMouseCursorMovement(&x, &y);
	
	if ((x != 0) || (y != 0))
	{
		moveToWidget(x, y);
	}
	
	if (game->isControl(CTRL_ACTION))
	{
		if (uiManager->widgetHit(mouse->x, mouse->y))
		{
			game->resetControl(CTRL_FIRE);
			game->resetControl(CTRL_ACTION);
		}
	}
}

SDL_Surface *buildMainDialog(const char *message)
{
	SDL_Surface *scratch = graphics->scratchSurface;
	SDL_FillRect(scratch, NULL, graphics->black);
	graphics->setTransparent(scratch);
	graphics->setFont(FONT_NORMAL);
	graphics->setFontColor(255, 255, 255, 0, 0, 0);
	graphics->writeWrappedText(scratch, true, "%s", message);
	
	int width = max(graphics->textWidth + 20, 250);

	SDL_Surface *dialogBox = graphics->createSurface(width, graphics->textHeight + 50);

	graphics->createRaisedBorder(dialogBox, graphics->lightGrey, graphics->grey);

	SDL_Rect source = {0, 0, graphics->textWidth, graphics->textHeight};
	SDL_Rect dest = {10, 10, graphics->textWidth, graphics->textHeight};

	if (SDL_BlitSurface(scratch, &source, dialogBox, &dest) < 0)
	{
		graphics->showErrorAndExit("dialogs::buildMainDialog - %s", SDL_GetError());
	}

	return dialogBox;
}

int showOKCancelDialog(const char *message)
{
	glEnable(GL_BLEND);
	
	int rtn = -1;
	
	Texture *dialogBox = graphics->createTexture(buildMainDialog(message));
	
	uiManager->OKCancelDialogGrid.x = (graphics->screen->w - 350) / 2;
	uiManager->OKCancelDialogGrid.width = 350;
	uiManager->OKCancelDialogGrid.height = 26;
	
	uiManager->OKCancelDialogGrid.y = graphics->screenMidY + (dialogBox->ih / 2) - 28;
	
	uiManager->OKCancelDialogGrid.updatePosition();
	
	Properties props;

	props.setProperty("groupName", "DialogBoxWidgets");
	props.setProperty("name", "tempDialogButton");
	props.setProperty("backgroundColor", "0 150 0");
	props.setProperty("enabled", "1");
	props.setProperty("visible", "1");
	props.setProperty("y", "0");
	props.setProperty("grid", "OKCancelDialogGrid");

	props.setProperty("label", "SYS_OK");
	props.setProperty("x", "1");
	Button *ok = (Button*)createButtonWidget(&props);

	props.setProperty("label", "GUI_CANCEL");
	props.setProperty("x", "2");
	Button *cancel = (Button*)createButtonWidget(&props);

	graphics->drawRect(graphics->screenMidX - (dialogBox->iw / 2), graphics->screenMidY - (dialogBox->ih / 2), dialogBox->iw, dialogBox->ih, GLColor::black, false);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_BLEND);
	graphics->blit(dialogBox, graphics->screenMidX, graphics->screenMidY, true);
	
	uiManager->setModal("DialogBoxWidgets");

	while (rtn == -1)
	{
		engine->getInput();
		graphics->updateScreen();
		
		drawWidgets();

		if (ok->wasClicked())
		{
			rtn = 1;
		}

		if (cancel->wasClicked())
		{
			rtn = 0;
		}
	}

	delete dialogBox;

	uiManager->setModal("");
	uiManager->removeGroup("DialogBoxWidgets");

	return rtn;
}

void showMessageDialog(const char *message, const char *buttonLabel)
{
	glEnable(GL_BLEND);
	
	debug(("showMessageDialog:: - %s\n", message));

	Texture *dialogBox = textureManager->getTexture(message);
	
	if (dialogBox == NULL)
	{
		dialogBox = graphics->createTexture(buildMainDialog(message));
		textureManager->addTexture(message, dialogBox);
	}
	
	Properties props;
	
	props.setProperty("x", (graphics->screen->w - 350) / 2);
	props.setProperty("y", graphics->screenMidY - (dialogBox->ih / 2) + dialogBox->ih - 40);
	props.setProperty("width", 350);
	props.setProperty("height", 26);
	uiManager->OKDialogGrid.setProperties(&props);
	
	props.clear();
	
	props.setProperty("groupName", "DialogBoxWidgets");
	props.setProperty("name", "tempDialogButton");
	props.setProperty("label", buttonLabel);
	props.setProperty("backgroundColor", "0 150 0");
	props.setProperty("enabled", "1");
	props.setProperty("visible", "1");
	props.setProperty("x", "1");
	props.setProperty("y", "0");
	props.setProperty("grid", "OKDialogGrid");
	
	Button *ok = (Button*)createButtonWidget(&props);

	graphics->setMode(MODE_2D);
	graphics->fade(0.85, graphics->screenMidX - (dialogBox->iw / 2), graphics->screenMidY - (dialogBox->ih / 2), dialogBox->iw, dialogBox->ih);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	graphics->blit(dialogBox, graphics->screenMidX, graphics->screenMidY, true);
	
	uiManager->setModal("DialogBoxWidgets");
	
	mouse->setBusy(false);
	
	bool doneReading = false;
	
	game->changeCameraRotation(0);
	
	drawGameObjects();

	while (!doneReading)
	{
		engine->getInput();
		graphics->clearScreen();
		graphics->updateScreen();
		
		engine->doTimeDifference();
		
		game->changeCameraRotation(0);
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		graphics->fade(0.85, graphics->screenMidX - (dialogBox->iw / 2), graphics->screenMidY - (dialogBox->ih / 2), dialogBox->iw, dialogBox->ih);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		graphics->blit(dialogBox, graphics->screenMidX, graphics->screenMidY, true);
		
		drawWidgets();
		
		if ((ok->wasClicked()) || (engine->userAccepts()) || (game->isControl(CTRL_ACTION)))
		{
			doneReading = true;
		}
	}
	
	engine->resetTimeDifference();
	
	mouse->setBusy(true);

	uiManager->setModal("");
	uiManager->removeGroup("DialogBoxWidgets");
	
	textureManager->removeTexture("normalImage-tempDialogButton-DialogBoxWidgets");
	textureManager->removeTexture("clickedImage-tempDialogButton-DialogBoxWidgets");
	textureManager->removeTexture("disabledImage-tempDialogButton-DialogBoxWidgets");
	
	game->resetControl(CTRL_ACTION);
}

void populateSavesList(ListView *listView)
{
	listView->getList()->clear();
	
	String filename;
	
	FILE *fp;
	
	for (int i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		// check to see if this save is valid first...
		
		filename.setText("%s/"SAVE_FORMAT, engine->userHomeDirectory.getText(), i);
	
		fp = fopen(filename.getText(), "rb");
	
		if (fp)
		{
			fclose(fp);
		}
		else
		{
			engine->saveSlot[i].inUse = false;
		}
		
		addSaveHeaderToList(listView, &engine->saveSlot[i], i);
	}
}

int doLoadSaveGameDialog(const char *type)
{
	uiManager->onlyShowGroup("LoadSave");
	
	ListView *savesList = (ListView*)uiManager->getWidget("SaveList", "LoadSave");
	Button *load = (Button*)uiManager->getWidget("Load", "LoadSave");
	Button *save = (Button*)uiManager->getWidget("Save", "LoadSave");
	Button *deleteGame = (Button*)uiManager->getWidget("Delete", "LoadSave");
	Button *cancel = (Button*)uiManager->getWidget("Cancel", "LoadSave");
	
	Grid *grid = uiManager->getGrid("LoadSaveGrid");
	
	uiManager->focusedWidget = savesList;
	
	load->setVisible(false);
	save->setVisible(false);
	uiManager->getWidget(type, "LoadSave")->setVisible(true);
	
	bool done = false;
	bool cancelled = false;
	
	populateSavesList(savesList);
	
	ListViewItem *selectedItem = NULL;
	SaveGameHeader *header = NULL;
	
	header = (SaveGameHeader*)savesList->getSelectedObject();
	deleteGame->setEnabled(header->inUse);
	load->setEnabled(header->inUse);
	
	int rtn = -1;
	
	mouse->setBusy(false);
	
	engine->resetTimeDifference();
	engine->clearInput();
	engine->flushInput();
	
	game->changeCameraRotation(0);
	
	drawGameObjects();
	
	while (!done)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		/*
		if ((player != NULL) && (player->health > 0))
		{
			game->changeCameraRotation(0);
			
			drawGameObjects();
		}
		*/
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		
		graphics->fade(0.5);
		
		graphics->drawRect(grid->x, grid->y, grid->width, grid->height, GLColor::black, GLColor::white, false);
		
		drawWidgets();
		
		if (savesList->itemSelectionChanged())
		{
			header = (SaveGameHeader*)savesList->getSelectedObject();
			selectedItem = savesList->getSelectedItem();
			deleteGame->setEnabled(header->inUse);
			load->setEnabled(header->inUse);
		}
		
		if ((load->wasClicked()) || (save->wasClicked()) || (engine->keyState[SDLK_RETURN]))
		{
			break;
		}
		
		if (deleteGame->wasClicked())
		{
			//if (showOKCancelDialog("LOADSAVE_DELETE_CONFIRM"))
			{
				removeSaveFromList(savesList->getSelectedIndex());
				header = (SaveGameHeader*)savesList->getSelectedObject();
				selectedItem = savesList->getSelectedItem();
				deleteGame->setEnabled(header->inUse);
				load->setEnabled(header->inUse);
				
				populateSavesList(savesList);
			}
		}
		
		if (cancel->wasClicked())
		{
			cancelled = true;
			done = true;
		}
	}
	
	if (header != NULL)
	{
		rtn = savesList->getSelectedIndex();
	}
	
	savesList->getList()->clear();
	
	graphics->clearScreen();
	
	game->changeCameraRotation(0);
	
	drawGameObjects();
	
	mouse->setBusy(true);
	
	game->resetControl(CTRL_ACTION);
	
	return (!cancelled) ? rtn : -1;
}
