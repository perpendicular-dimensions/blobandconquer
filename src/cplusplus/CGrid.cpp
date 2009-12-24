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

Grid::Grid()
{
	x = y = 0;
	width = height = 50;
	
	rows = cols = 0;
	cellWidth = cellHeight = 10;
	
	cellSpacing = 3;
	
	name = "";
}

Grid::~Grid()
{
}

int Grid::getCellX(int x)
{
	if (x >= cols)
	{
		printf("Warning - Grid::getCellX - %d exceeds total column number of %d\n", x, cols);
		return (cols - 1) * cellWidth;
	}
	
	int rtn = this->x;
	
	rtn += (x * cellWidth) + cellSpacing;
	
	return rtn;
}

int Grid::getCellY(int y)
{
	if (y >= rows)
	{
		printf("Warning - Grid::getCellY - %d exceeds total row number of %d\n", x, rows);
		return rows * cellHeight;
	}
	
	int rtn = this->y;
	
	rtn += (y * cellHeight) + cellSpacing;
	
	return rtn;
}

int Grid::getCellWidth()
{
	return cellWidth;
}

int Grid::getCellHeight()
{
	return cellHeight;
}

void Grid::setProperties(Properties *properties)
{
	name = properties->getString("name", name.getText());
	
	x = properties->getInt("x", x);
	y = properties->getInt("y", y);
	
	width = properties->getInt("width", width);
	height = properties->getInt("height", height);
	
	cellSpacing = properties->getInt("cellSpacing", cellSpacing);
	
	rows = properties->getInt("rows", rows);
	cols = properties->getInt("cols", cols);
	
	cellWidth = width / cols;
	cellHeight = height / rows;
	
	// top, middle, bottom, absolute
	valign = properties->getInt("valign", 3);
	
	// left, middle, right, absolute
	halign = properties->getInt("halign", 3);
	
	updatePosition();
}

void Grid::updatePosition()
{
	switch (valign)
	{
		case 0:
			y = 0;
			break;
		case 1:
			y = (Graphics::getInstance()->screen->h - height) / 2;
			break;
		case 2:
			y = (Graphics::getInstance()->screen->h - height);
			break;
	}
	
	switch (halign)
	{
		case 0:
			x = 0;
			break;
		case 1:
			x = (Graphics::getInstance()->screen->w - width) / 2;
			break;
		case 2:
			x = (Graphics::getInstance()->screen->w - width);
			break;
	}
}
