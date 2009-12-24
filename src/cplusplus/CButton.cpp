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

Button::Button()
{
	widgetType = BUTTON;
	buttonState = 0;
	clicked = false;
	
	selected = false;
}

Button::~Button()
{
}

void Button::use(int x, int y)
{
	clicked = true;
	buttonState = 0;
}

bool Button::wasClicked()
{
	if (clicked)
	{
		clicked = false;
		Audio::getInstance()->playSound(SND_BUTTONCLICK, CH_ANY, 128);
		return true;
	}

	clicked = false;
	return false;
}

void Button::setVisible(bool visible)
{
	this->visible = visible;
}

void Button::setEnabled(bool enabled)
{
	this->enabled = enabled;
	
	if (enabled)
	{
		currentImage = normalImage;
	}
	else
	{
		currentImage = disabledImage;
	}
}

void Button::setLabel(const char *label, ...)
{
	char tmpString[256];

	va_list argp;
	va_start(argp, label);
	vsnprintf(tmpString, sizeof(tmpString), label, argp);
	va_end(argp);

	this->label = tmpString;
}

void Button::setProperties(Properties *properties)
{
	setBaseValues(properties);
	
	const char *labelProp = properties->getString("label", "NULL");

	if (strcmp(labelProp, "@NONE@") != 0)
	{
		label = _(labelProp);
	}
	else
	{
		label = " ";
	}
}

void Button::mouseReleased(SDL_MouseButtonEvent mouse)
{
	if (mouse.button != SDL_BUTTON_LEFT)
	{
		return;
	}
	
	currentImage = normalImage;

	if (buttonState == 1)
	{
		buttonState = 0;
		return;
	}

	use(0, 0);
}

void Button::mousePressed(SDL_MouseButtonEvent mouse)
{
	currentImage = clickedImage;

	buttonState = 2;
}

void Button::mouseMoved(int x, int y)
{
	if (buttonState > 0)
	{
		if (Collision::collision(x, y, 1, 1, this->x, this->y, this->currentImage->iw, this->currentImage->ih))
		{
			currentImage = clickedImage;
			buttonState = 2;
		}
		else
		{
			currentImage = normalImage;
			buttonState = 1;
		}
	}
}
