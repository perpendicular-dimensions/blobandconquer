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

#include "misc.h"

void addStatToList(ListView *listView, const char *string, float value, int position)
{
	ListViewItem *item;
	SDL_Surface *textSurface;
	
	graphics->setFontColor(0xff, 0xff, 0xff, 0, 0, 0);
	textSurface = graphics->createSurface(listView->width, 16);
	graphics->setTransparent(textSurface);
		
	graphics->drawString(25, 0, TXT_LEFT, textSurface, "%s", string);
	graphics->drawString(listView->width - position, 0, TXT_RIGHT, textSurface, "%d", (int)value);
		
	item = new ListViewItem();
	item->texture = graphics->createTexture(textSurface);
	
	String s;
	s.setText("%s-%s", listView->getName(), string);
	textureManager->removeTexture(s.getText());
	textureManager->addTexture(s.getText(), item->texture);
		
	listView->addItem(item);
}

void addStatToList(ListView *listView, const char *statname, int position)
{
	if (strcmp(statname, _("Total Game Time")) != 0)
	{
		addStatToList(listView, statname, game->getStatistic(statname), position);
		return;
	}
	
	String time;
	time.setText("%.2d:%.2d:%.2d", (int)game->hours, (int)game->minutes, (int)game->seconds);
		
	ListViewItem *item;
	SDL_Surface *textSurface;
	
	graphics->setFontColor(0xff, 0xff, 0xff, 0, 0, 0);
	textSurface = graphics->createSurface(listView->width, 16);
	graphics->setTransparent(textSurface);
		
	graphics->drawString(25, 0, TXT_LEFT, textSurface, _("Total Game Time"));
	graphics->drawString(listView->width - position, 0, TXT_RIGHT, textSurface, "%s", time.getText());
		
	item = new ListViewItem();
	item->texture = graphics->createTexture(textSurface);
	
	String s;
	s.setText("%s-%s", listView->getName(), statname);
	textureManager->removeTexture(s.getText());
	textureManager->addTexture(s.getText(), item->texture);
		
	listView->addItem(item);
}

void addStringToList(ListView *listView, const char *string)
{
	ListViewItem *item;
	SDL_Surface *textSurface;
	
	graphics->setFontColor(0xff, 0xff, 0xff, 0, 0, 0);
	textSurface = graphics->createSurface(listView->width, 16);
	graphics->setTransparent(textSurface);
		
	graphics->drawString(25, 0, TXT_LEFT, textSurface, "%s", string);
		
	item = new ListViewItem();
	item->texture = graphics->createTexture(textSurface);
	
	String s;
	s.setText("%s-%s", listView->getName(), string);
	textureManager->removeTexture(s.getText());
	textureManager->addTexture(s.getText(), item->texture);
		
	listView->addItem(item);
}

void addBlankLineToList(ListView *listView)
{
	ListViewItem *item;
	
	String s;
	s.setText("%s-blankline", listView->getName());
	
	Texture *blankLine = textureManager->getTexture(s.getText());
	
	if (blankLine == NULL)
	{
		SDL_Surface *textSurface;
		textSurface = graphics->createSurface(listView->width, 16);
		graphics->setTransparent(textSurface);	
		blankLine = graphics->createTexture(textSurface);
		textureManager->addTexture(s.getText(), blankLine);
	}
	
	item = new ListViewItem();
	item->texture = blankLine;
		
	listView->addItem(item);
}

void addSaveHeaderToList(ListView *listView, SaveGameHeader *header, int i)
{
	ListViewItem *item;
	SDL_Surface *textSurface;
	
	textSurface = graphics->createSurface(listView->width, 16);
	graphics->setTransparent(textSurface);
	
	if (!header->inUse)
	{
		graphics->setFontColor(0x66, 0x66, 0x66, 0, 0, 0);
		graphics->drawString(5, 0, TXT_LEFT, textSurface, _("<<< Empty >>>"));
	}
	else
	{
		graphics->setFontColor(0xff, 0xff, 0xff, 0, 0, 0);
		graphics->drawString(5, 0, TXT_LEFT, textSurface, "%s", header->description.getText());
		graphics->drawString(textSurface->w - 5, 0, TXT_RIGHT, textSurface, "%s, %s", header->date.getText(), header->time.getText());
	}
		
	item = new ListViewItem();
	item->texture = graphics->createTexture(textSurface);
	item->object = header;
	
	String s;
	s.setText("%s-SaveGameHeader%.2d", listView->getName(), i);
	textureManager->removeTexture(s.getText());
	textureManager->addTexture(s.getText(), item->texture);
	
	listView->addItem(item);
}
