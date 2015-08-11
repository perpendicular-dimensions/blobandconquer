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

#include "status.h"

const char *getBlobJob(Unit *unit)
{
	switch (unit->definition->type)
	{
		case BLOB_BOB:
			return _("Leader");
			
		case BLOB_TEEKA:
			return _("Sidekick");
			
		case BLOB_SOLDIER:
			return _("Cadet");
			
		case BLOB_DEMOLITION:
			return _("Demolition Expert");
			
		case BLOB_ENGINEER:
			return _("Engineer");
			
		case BLOB_HACKER:
			return _("Hacker");
	}
	
	return "???";
}

void drawObjectiveStatus(Objective *objective, int ax, int y)
{
	GLColor c;
	
	switch (objective->status)
	{
		case OBJ_INCOMPLETE:
			c = GLColor::white;
			break;
			
		case OBJ_COMPLETE:
			c = GLColor::green;
			break;
			
		case OBJ_FAILED:
			c = GLColor::red;
			break;
	}
	
	graphics->drawString(ax + 120, y, TXT_LEFT, GLColor::white, true, "%s", objective->description.getText());
		
	graphics->drawString(ax + 450, y, TXT_LEFT, GLColor::white, true, "%s", objective->getProgress());
		
	graphics->drawString(ax + 685, y, TXT_RIGHT, c, true, "%s", objective->getStatus());
}

void drawObjectiveTabTitle(const char *title, int ax, int ay)
{
	graphics->setFont(FONT_LARGE);
	graphics->drawString(graphics->screen-> w / 2, ay + 90, TXT_CENTERED, GLColor::white, true, "%s", title);
	graphics->drawRect(ax + 60, ay + 125, 680, 1, GLColor::white, false);
	graphics->setFont(FONT_NORMAL);
}

void drawObjectivesTab(int ax, int ay, const char *battleResult)
{
	drawObjectiveTabTitle(battleResult, ax, ay);
	
	int y = ay + 140;
	
	graphics->drawString(ax + 60, y, TXT_LEFT, GLColor::white, true, _("Primary Objectives"));
	
	y += 30;
	
	for (Objective *objective = (Objective*)mission->primaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
	{
		if (!objective->active)
		{
			continue;
		}
		
		drawObjectiveStatus(objective, ax, y);
		y += 25;
	}
	
	y += 40;
	
	if (mission->activeObjectives(OBJ_PRIORITY_SECONDARY) > 0)
	{
		graphics->drawString(ax + 60, y, TXT_LEFT, GLColor::white, true, _("Secondary Objectives"));
		
		y += 30;
		
		for (Objective *objective = (Objective*)mission->secondaryObjectives.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
		{
			if ((!objective->active) || (objective->priority == OBJ_PRIORITY_HIDDEN))
			{
				continue;
			}
			
			drawObjectiveStatus(objective, ax, y);
			y += 25;
		}
		
		y += 40;
	}
	
	if (mission->activeObjectives(OBJ_PRIORITY_CONDITION) > 0)
	{
		graphics->drawString(ax + 60, y, TXT_LEFT, GLColor::white, true, _("Conditions"));
		
		y += 30;
		
		for (Objective *objective = (Objective*)mission->conditions.getFirstElement() ; objective != NULL ; objective = (Objective*)objective->next)
		{
			if (!objective->active)
			{
				continue;
			}
			
			drawObjectiveStatus(objective, ax, y);
			y += 25;
		}
	}
}

void drawUnitDebriefTab(int ax, int ay)
{
	drawObjectiveTabTitle(_("Unit Status"), ax, ay);
	
	int y = ay + 140;
	GLColor lineColor, statusColor;
	String string;
	
	graphics->drawString(ax + 60, y, TXT_LEFT, GLColor::white, true, _("Name"));
	graphics->drawString(ax + 200, y, TXT_LEFT, GLColor::white, true, _("Health"));
	graphics->drawString(ax + 350, y, TXT_LEFT, GLColor::white, true, _("Shield"));
	graphics->drawString(ax + 450, y, TXT_LEFT, GLColor::white, true, _("Job"));
	graphics->drawString(ax + 650, y, TXT_LEFT, GLColor::white, true, _("Weapon"));
	
	y += 35;
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if ((unit->definition->type == BLOB_MIA) || (unit->definition->type == BLOB_ASSIMILATING))
		{
			continue;
		}
	
		if (unit->health > 0)
		{
			lineColor = GLColor::white;
		}
		else
		{
			lineColor = GLColor::grey;
		}
		
		graphics->drawString(ax + 60, y, TXT_LEFT, lineColor, true, "%s", unit->getName());
		graphics->drawString(ax + 200, y, TXT_LEFT, lineColor, true, "%d / %d", (int)unit->health, (int)unit->maxHealth);
		graphics->drawString(ax + 350, y, TXT_LEFT, lineColor, true, "%d / %d", (int)unit->shield, (int)unit->maxShield);
		graphics->drawString(ax + 450, y, TXT_LEFT, lineColor, true, "%s", getBlobJob(unit));
		
		if (unit->definition->type == BLOB_BOB)
		{
			graphics->drawString(ax + 650, y, TXT_LEFT, lineColor, true, "%s", game->selectedWeapon->definition->getName());
		}
		else if (unit->definition->type == BLOB_TEEKA)
		{
			graphics->drawString(ax + 650, y, TXT_LEFT, lineColor, true, "%s", findTeeka()->currentWeapon->getName());
		}
		else
		{
			graphics->drawString(ax + 650, y, TXT_LEFT, lineColor, true, "%s", unit->currentWeapon->getName());
		}
		
		y += 25;
	}
}

void drawDebriefTable(int selectedSection, int ax, int ay)
{
	GLColor lightGreen;
	GLColor lighterGreen;
	GLColor darkGreen;
	GLColor c;
	
	darkGreen.set(0, 0.25, 0, 0.5);
	lighterGreen.set(0, 0.45, 0, 0.5);
	lightGreen.set(0, 0.5, 0, 0.5);
	
	// main table
	graphics->fade(0.8, ax + 50, ay + 50, 700, 500);
	
	c = (selectedSection == 0) ? lighterGreen : darkGreen;
	graphics->drawRect(ax + 50, ay + 50, 174, 25, c, lightGreen, false);
	graphics->drawString(ax + 60, ay + 55, TXT_LEFT, GLColor::white, true, _("Objectives"));
	
	c = (selectedSection == 1) ? lighterGreen : darkGreen;
	graphics->drawRect(ax + 225, ay + 50, 174, 25, c, lightGreen, false);
	graphics->drawString(ax + 235, ay + 55, TXT_LEFT, GLColor::white, true, _("Units"));
		
	c = (selectedSection == 2) ? lighterGreen : darkGreen;
	graphics->drawRect(ax + 400, ay + 50, 174, 25, c, lightGreen, false);
	graphics->drawString(ax + 410, ay + 55, TXT_LEFT, GLColor::white, true, _("Statistics"));
	
	graphics->drawRect(ax + 52, ay + 520, 696, 28, darkGreen, lightGreen, false);	
}

void createStatsDebriefTab()
{
	ListView *listView = (ListView*)uiManager->getWidget("StatsList", "briefing");
	listView->getList()->clear();
	
	addBlankLineToList(listView);
	addStatToList(listView, _("Total Game Time"), 250);
	
	addBlankLineToList(listView);
	addStatToList(listView, _("Enemies Killed"), 250);
	addStatToList(listView, _("Enemies Killed Teeka"), 250);
	addStatToList(listView, _("Enemies Killed Friendly"), 250);
	addStatToList(listView, _("Enemies Drowned"), 250);
	
	addBlankLineToList(listView);
	addStatToList(listView, _("Allies Killed"), 250);
	
	addBlankLineToList(listView);
	addStatToList(listView, _("Bullets Fired"), 250);
	addStatToList(listView, _("Bullets Hit"), 250);
	addStatToList(listView, _("Bullets Missed"), 250);
	
	addBlankLineToList(listView);
	addStatToList(listView, _("Cherries Picked Up"), 250);
	addStatToList(listView, _("Items Picked Up"), 250);
	addStatToList(listView, _("Weapons Picked Up"), 250);
	
	addBlankLineToList(listView);
	addStatToList(listView, _("Total Damage Taken"), 250);
	addStatToList(listView, _("Times Fallen In Slime"), 250);
	addStatToList(listView, _("Times Fallen In Lava"), 250);
	
	addBlankLineToList(listView);
	addStatToList(listView, _("Best Combo"), 250);
}

void showMissionStatusPanel()
{
	uiManager->onlyShowGroup("@none@");
	
	int ax = graphics->getOffsetX();
	int ay = graphics->getOffsetY();
	
	int selectedSection = 0;
	
	Button *finished = (Button*)uiManager->getWidget("Exit", "briefing");
	ListView *stats = (ListView*)uiManager->getWidget("StatsList", "briefing");
	
	finished->setVisible(true);
	
	mouse->setBusy(false);
	
	createStatsDebriefTab();
	
	while (true)
	{
		graphics->updateScreen();
		
		graphics->clearScreen();
		
		engine->getInput();
		
		engine->doTimeDifference();
		
		graphics->setMode(MODE_3D);
		
		game->changeCameraRotation(0);
		
		drawGameObjects();
		
		graphics->setMode(MODE_2D);
		
		drawDebriefTable(selectedSection, ax, ay);
		
		switch (selectedSection)
		{
			case 0:
				drawObjectivesTab(ax, ay, _("Objectives"));
				break;
			case 1:
				drawUnitDebriefTab(ax, ay);
				break;
			case 2:
				drawObjectiveTabTitle(_("Statistics"), ax, ay);
				break;
		}
		
		drawWidgets();
		
		if (mouse->buttonState[SDL_BUTTON_LEFT])
		{
			for (int i = 0 ; i < 3 ; i++)
			{
				if (Collision::collision(ax + 50 + (i * 175), ay + 50, 174, 25, mouse->x, mouse->y, 1, 1))
				{
					if (selectedSection != i)
					{
						selectedSection = i;
						
						audio->playSound(SND_BUTTONCLICK, CH_ANY, 128);
					}
					
					stats->setVisible(selectedSection == 2);
				}
			}
			
			mouse->buttonState[SDL_BUTTON_LEFT] = 0;
		}
		
		if (engine->keyState[SDL_SCANCODE_1])
		{
			selectedSection = 0;
			audio->playSound(SND_BUTTONCLICK, CH_ANY, 128);
			stats->setVisible(false);
			engine->keyState[SDL_SCANCODE_1] = 0;
		}
		
		if (engine->keyState[SDL_SCANCODE_2])
		{
			selectedSection = 1;
			audio->playSound(SND_BUTTONCLICK, CH_ANY, 128);
			stats->setVisible(false);
			engine->keyState[SDL_SCANCODE_2] = 0;
		}
		
		if (engine->keyState[SDL_SCANCODE_3])
		{
			selectedSection = 2;
			audio->playSound(SND_BUTTONCLICK, CH_ANY, 128);
			stats->setVisible(true);
			engine->keyState[SDL_SCANCODE_3] = 0;
		}
		
		if (game->isControl(CTRL_STATUS))
		{
			game->resetControl(CTRL_STATUS);
			break;
		}
		
		if (finished->wasClicked())
		{
			break;
		}
	}
	
	mouse->setBusy(true);
}
