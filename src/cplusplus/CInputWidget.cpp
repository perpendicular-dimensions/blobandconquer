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

InputWidget::InputWidget()
{
	widgetType = INPUT;
	
	redraw = true;
	
	awaitingInput = false;
	
	value[0] = value[1] = value[2] = -1;
}

InputWidget::~InputWidget()
{
}

void InputWidget::use(int x, int y)
{
	if (!awaitingInput)
	{
		awaitingInput = true;
		return;
	}
}

void InputWidget::mousePressed(SDL_MouseButtonEvent mouse)
{
	if (!awaitingInput)
	{
		awaitingInput = true;
		return;
	}
	
	setValue(INPUT_MOUSE, mouse.button);
}

void InputWidget::setValue(int type, int value)
{
	awaitingInput = false;
	redraw = true;
	this->value[type] = value;
}

void InputWidget::clear()
{
	redraw = true;
	awaitingInput = false;
	value[0] = value[1] = value[2] = -1;
}

void InputWidget::keyPressed(int key, bool shiftHeld)
{
	if (!awaitingInput)
	{
		return;
	}
	
	if (key == SDLK_ESCAPE)
	{
		awaitingInput = false;
		return;
	}
	
	if (key == SDLK_BACKSPACE)
	{
		clear();
		return;
	}
	
	setValue(INPUT_KEY, key);
}

void InputWidget::joystickButtonPressed(int key)
{
	if (!awaitingInput)
	{
		return;
	}
	
	setValue(INPUT_JOY, key);
}

void InputWidget::setProperties(Properties *properties)
{
	setBaseValues(properties);
}
