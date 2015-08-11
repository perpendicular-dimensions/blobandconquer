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

BaseWidget::BaseWidget()
{
	x = y = width = height = 0;
	
	widgetType = -1;

	visible = false;
	enabled = true;

	clicked = false;

	currentImage = NULL;
	normalImage = NULL;
	clickedImage = NULL;
	disabledImage = NULL;
}

BaseWidget::~BaseWidget()
{
	
}

int BaseWidget::getWidgetType()
{
	return widgetType;
}

const char *BaseWidget::getName()
{
	return name.getText();
}

const char *BaseWidget::getGroupName()
{
	return groupName.getText();
}

bool BaseWidget::isEnabled()
{
	return enabled;
}

bool BaseWidget::isVisible()
{
	return visible;
}

void BaseWidget::setVisible(bool visible)
{
	this->visible = visible;
}

void BaseWidget::setBaseValues(Properties *properties)
{
	name = properties->getString("name", "NULL");
	groupName = properties->getString("groupName", "NULL");

	enabled = properties->getInt("enabled", 0);
	visible = properties->getInt("visible", 0);
	gridX = properties->getInt("x", 0);
	gridY = properties->getInt("y", 0);

	horizontalAlignment = properties->getInt("horizontalAlignment", 0);
	verticleAlignment = properties->getInt("verticleAlignment", 0);

	fontSize = properties->getInt("fontSize", 0);
	
	grid = UIManager::getInstance()->getGrid(properties->getString("grid", "unknown"));
	
	updatePosition();
}

void BaseWidget::updatePosition()
{
	x = grid->getCellX(gridX);
	y = grid->getCellY(gridY);
	
	if (widgetType != LISTVIEW)
	{
		y += (grid->getCellHeight() - height) / 2;
	}
}

// All abstract methods follow

void BaseWidget::use(int x, int y){}
void BaseWidget::setEnabled(bool enabled){}
void BaseWidget::mousePressed(SDL_MouseButtonEvent mouse){}
void BaseWidget::mouseReleased(SDL_MouseButtonEvent mouse){}
void BaseWidget::mouseMoved(int x, int y){}
void BaseWidget::keyPressed(int key){}
void BaseWidget::joystickButtonPressed(int key){}
