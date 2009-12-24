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

Panel::Panel()
{
	widgetType = PANEL;
	normalImage = NULL;
	clickedImage = NULL;
	currentImage = NULL;
}

void Panel::use(int x, int y)
{
}

void Panel::setProperties(Properties *properties)
{	
	setBaseValues(properties);
	
	borderColor = properties->getInt("borderColor", 0);
	int colSpan = properties->getInt("colSpan", 1);
	int rowSpan = properties->getInt("rowSpan", 1);
	
	Grid *grid = UIManager::getInstance()->getGrid(properties->getString("grid", "unknown"));
	
	width = grid->getCellWidth();
	height = grid->getCellHeight();
	
	width *= colSpan;
	height *= rowSpan;
	
	width -= (grid->cellSpacing * 2);
	height -= (grid->cellSpacing * 2);
	
	if (width <= 0) width = 1;
	if (height <= 0) height = 1;
}

Panel::~Panel()
{
}
