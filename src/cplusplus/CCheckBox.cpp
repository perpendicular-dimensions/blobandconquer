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

CheckBox::CheckBox()
{
	widgetType = CHECKBOX;

	checked = false;
	
	clicked = false;
}

CheckBox::~CheckBox()
{
}

void CheckBox::use(int x, int y)
{
	checked = !checked;
	
	clicked = true;
}

bool CheckBox::wasClicked()
{
	if (clicked)
	{
		clicked = false;
		Audio::getInstance()->playSound(SND_BUTTONCLICK, CH_ANY, 128);
		return true;
	}

	return false;
}

void CheckBox::setProperties(Properties *properties)
{
	setBaseValues(properties);

	checked = properties->getInt("checked", 0);
}

void CheckBox::mousePressed(SDL_MouseButtonEvent mouse)
{
	use(0, 0);
}
