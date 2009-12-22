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

#include "save.h"

void showSaveGameProgess(const char *progress, bool autoSaving)
{
	graphics->setMode(MODE_2D);
	
	if (!autoSaving)
	{
		graphics->drawRect(0, (graphics->screen->h / 2) - 10, graphics->screen->w, 34, GLColor::black, false);
		graphics->drawRect(0, (graphics->screen->h / 2) - 10, graphics->screen->w, 1, GLColor::white, false);
		graphics->drawRect(0, (graphics->screen->h / 2) + 24, graphics->screen->w, 1, GLColor::white, false);
		
		graphics->drawString(graphics->screenMidX, graphics->screenMidY, TXT_CENTERED, GLColor::white, true, "%s", progress);
		
		graphics->delay(1, false);
	}
	else
	{
		graphics->showLoadingProgress(_("Saving Game State..."));
	}
}

void removeSaveFromList(int slotId)
{
	String saveSaveName;
	saveSaveName.setText("%s/"SAVE_FORMAT, engine->userHomeDirectory.getText(), slotId);
	
	debug(("removeSaveFromList() - Removing save game '%s'\n", saveSaveName.getText()));
	
	remove(saveSaveName.getText());
	
	saveSaveName.setText(""SAVE_FORMAT, slotId);
	
	engine->saveSlot[slotId].inUse = false;
	engine->saveSlot[slotId].description = "Empty";
	
	engine->saveSaveGameHeaders();
}

void createSaveHeader(int slotId)
{
	debug(("createSaveHeader - %d\n", slotId));
	
	char timeBuffer[40];
	char dateBuffer[40];
	
	time_t ltime;
	time(&ltime);
	tm *today = localtime(&ltime);

	strftime(timeBuffer, 40, "%H:%M %Z", today);
	strftime(dateBuffer, 40, "%d/%m/%Y", today);
	
	engine->saveSlot[slotId].inUse = true;
	engine->saveSlot[slotId].time = timeBuffer;
	engine->saveSlot[slotId].date = dateBuffer;
	engine->saveSlot[slotId].mapName = game->mission.getText();
	engine->saveSlot[slotId].description = mission->missionName.getText();
	
	engine->saveSaveGameHeaders();
}

void saveGameData(bool autoSaving)
{
	if ((game->saveSlot == -1) && (autoSaving))
	{
		return;
	}
	
	engine->startDebugTimer("saveGameData()");
	
	createSaveHeader(game->saveSlot);
	
	String filename;
	filename.setText("%s/"SAVE_FORMAT, engine->userHomeDirectory.getText(), game->saveSlot);

	debug(("Attempting to save game to %s\n", filename.getText()));
	
	FILE *fp = fopen(filename.getText(), "wb");

	if (!fp)
	{
		graphics->showErrorAndExit("Couldn't save game");
	}
	
	showSaveGameProgess("Saving Game - Please Wait...", autoSaving);
	
	if (!game->changingLevel)
	{
		graphics->delay(500, false);
	}
	
	showSaveGameProgess("Saving Game - Please Wait... 10%", autoSaving);
	
	fprintf(fp, "// ############### Custom Item Data ############\n\n");
	
	entityManager->saveCustomItems(fp);
	
	fprintf(fp, "// ############### Game Data ############\n\n");
	
	game->save(fp);
	
	showSaveGameProgess("Saving Game - Please Wait... 25%", autoSaving);
	
	fprintf(fp, "// ############### Mission Data ############\n\n");
	
	mission->save(fp);
	
	showSaveGameProgess("Saving Game - Please Wait... 50%", autoSaving);
	
	fprintf(fp, "// ############### All Entity Data ############\n\n");
	
	entityManager->save(fp);
	
	showSaveGameProgess("Saving Game - Please Wait... 75%", autoSaving);
	
	fprintf(fp, "// ############### Particle Data ############\n\n");
	
	particleManager->save(fp);
	
	showSaveGameProgess("Saving Game - Please Wait... 95%", autoSaving);
	
	fprintf(fp, "// ############### Crumb Data ############\n\n");
	
	crumbBox.save(fp);

	fprintf(fp, "@END@\n");

	fclose(fp);
	
	showSaveGameProgess("Saving Game - Please Wait... 100%", autoSaving);
	
	debug(("Successfully saved game to %s (slot %d)\n", filename.getText(), game->saveSlot));
	
	showSaveGameProgess("Saving Game - Done", autoSaving);
	
	engine->endDebugTimer("saveGameData()");
	
	if (!game->changingLevel)
	{
		graphics->delay(500, false);
	}
}

void saveRestartData()
{
	if (game->saveSlot == -1)
	{
		return;
	}
	
	String filename;
	
	filename.setText("%s/"SAVE_FORMAT, engine->userHomeDirectory.getText(), game->saveSlot);
	pak->unpack(filename.getText(), PAK::TEXT);
	
	filename.setText("%s/"SAVE_FORMAT"-restart", engine->userHomeDirectory.getText(), game->saveSlot);
	
	FILE *fp = fopen(filename.getText(), "wb");
	
	if (fp == NULL)
	{
		printf("WARNING: Couldn't create restart file '%s'\n", filename.getText());
		return;
	}
	
	fwrite(pak->dataBuffer, 1, pak->getLastReadDataSize(), fp);
	fclose(fp);
}

bool saveGame()
{
	int saveSlot = doLoadSaveGameDialog("Save");
	
	// don't bother! They pressed cancel!
	if (saveSlot == -1)
	{
		return false;
	}
	
	game->saveSlot = saveSlot;
	
	uiManager->onlyShowGroup("@none@");
	
	saveGameData(false);
	
	engine->resetTimeDifference();
	
	return true;
}
