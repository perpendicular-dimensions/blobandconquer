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

ComboBox::ComboBox()
{
	widgetType = COMBOBOX;
	
	selectedItem = -1;
	
	changed = false;
}

ComboBox::~ComboBox()
{
	itemList.clear();
}

void ComboBox::use(int x, int y)
{
}

void ComboBox::setProperties(Properties *properties)
{
	setBaseValues(properties);
}

bool ComboBox::hasChanged()
{
	if (changed)
	{
		changed = false;
		return true;
	}
	
	return false;
}

void ComboBox::addItem(const char *text)
{
	String *item = new String();
	item->setText(_("%s"), text);
	
	itemList.add(item);
	
	if (itemList.getSize() == 1)
	{
		selectedItem = 0;
		currentItemText = ((String*)itemList.getElement(selectedItem))->getText();
	}
}

void ComboBox::setSelectedItem(int selectedItem)
{
	if (selectedItem > (itemList.getSize() - 1))
	{
		selectedItem = (itemList.getSize() - 1);
	}
	
	this->selectedItem = selectedItem;
	currentItemText = ((String*)itemList.getElement(selectedItem))->getText();
}

void ComboBox::clear()
{
	selectedItem = -1;
	
	itemList.clear();
}

void ComboBox::change(int x)
{
	bool okay = false;
	
	while (!okay)
	{
		Math::limit(&(selectedItem += x), 0, itemList.getSize() - 1);
		currentItemText = ((String*)itemList.getElement(selectedItem))->getText();
		
		if (currentItemText != "" && currentItemText != "NULL")
		{
			okay = true;
		}
	}
	
	changed = true;
}

void ComboBox::mousePressed(SDL_MouseButtonEvent mouse)
{
	if ((Collision::collision(mouse.x, mouse.y, 1, 1, x, y, 16, 16)) && (selectedItem != 0))
	{
		change(-1);
	}
	
	if ((Collision::collision(mouse.x, mouse.y, 1, 1, x + width - 16, y, 16, 16)) && (selectedItem < itemList.getSize() - 1))
	{
		change(1);
	}
}

void ComboBox::mouseReleased(SDL_MouseButtonEvent mouse)
{
	
}
