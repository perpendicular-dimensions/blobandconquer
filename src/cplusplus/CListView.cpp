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

ListView::ListView()
{
	widgetType = LISTVIEW;

	startIndex = 0;
	selectedIndex = 0;
	endIndex = 0;

	clicked = false;
	itemSelected = false;
	transparent = false;
	border = true;
	
	numberOfPages = 1;
	currentPage = 1;
	buttonState = 0;
	
	items.setName("ListView List");
}

ListView::~ListView()
{
	items.clear();
}

void ListView::use(int x, int y)
{
}

void ListView::gotoPage(int pageNumber)
{
	startIndex = selectedIndex = 0;
	
	for (ListViewItem *item = (ListViewItem*)items.getFirstElement() ; item != NULL ; item = (ListViewItem*)item->next)
	{
		if (item->pageNumber == pageNumber)
		{
			return;
		}
		
		startIndex++;
		selectedIndex++;
	}
}

void ListView::calculateNumberOfPages()
{
	int itemHeight = 2;
	
	numberOfPages = 1;
	
	for (ListViewItem *item = (ListViewItem*)items.getFirstElement() ; item != NULL ; item = (ListViewItem*)item->next)
	{
		if (itemHeight + item->texture->ih + 2 > height)
		{
			numberOfPages++;
			itemHeight = 2;
		}
		
		item->pageNumber = numberOfPages;
		
		itemHeight += (item->texture->ih + 2);
	}
}

void ListView::addItem(ListViewItem *item)
{
	items.add(item);
	
	itemSelected = true;
	
	calculateNumberOfPages();
}

void ListView::removeItem(ListViewItem *item)
{
	items.remove(item);
	
	if (selectedIndex == items.getSize())
	{
		selectedIndex--;
	}
	
	calculateNumberOfPages();
}

ListViewItem *ListView::getSelectedItem()
{
	int i = 0;

	for (ListViewItem *item = (ListViewItem*)items.getFirstElement() ; item != NULL ; item = (ListViewItem*)item->next)
	{
		if (i == selectedIndex)
		{
			return item;
		}

		i++;
	}
	
	printf("WARNING: ListView::getSelectedItem - Returning NULL! (%s)\n", getName());
	
	// shouldn't get here!
	return NULL;
}

Linkable *ListView::getSelectedObject()
{
	int i = 0;

	for (ListViewItem *item = (ListViewItem*)items.getFirstElement() ; item != NULL ; item = (ListViewItem*)item->next)
	{
		if (i == selectedIndex)
		{
			return item->object;
		}

		i++;
	}
	
	printf("WARNING: ListView::getSelectedObject - Returning NULL! (%s)\n", getName());
	
	// shouldn't get here!
	return NULL;
}

int ListView::getNumberOfPages()
{
	return numberOfPages;
}

int ListView::getCurrentPage()
{
	return currentPage;
}

int ListView::getSelectedIndex()
{
	return selectedIndex;
}

void ListView::setSelectedIndex(int index)
{
	selectedIndex = index;
}

void ListView::setSelectedObject(Linkable *object)
{
	int i = 0;

	for (Linkable *link = items.getFirstElement() ; link != NULL ; link = link->next)
	{
		if (link == object)
		{
			selectedIndex = i;
			itemSelected = true;
		}

		i++;
	}
}

int ListView::getItemCount()
{
	return items.getSize();
}

List *ListView::getList()
{
	return &items;
}

void ListView::setProperties(Properties *properties)
{
	setBaseValues(properties);
	
	transparent = properties->getInt("transparent", 0);
	border = properties->getInt("border", 1);
	
	Grid *grid = UIManager::getInstance()->getGrid(properties->getString("grid", "unknown"));

	width = grid->getCellWidth();
	height = grid->getCellHeight();
	
	int colSpan = properties->getInt("colSpan", 1);
	int rowSpan = properties->getInt("rowSpan", 1);
	
	width *= colSpan;
	height *= rowSpan;
	
	width -= (grid->cellSpacing * 2);
	height -= (grid->cellSpacing * 2);
}

bool ListView::itemSelectionChanged()
{
	if (itemSelected)
	{
		itemSelected = false;
		return true;
	}
	
	return false;
}

void ListView::mousePressed(SDL_MouseButtonEvent mouse)
{
	if (mouse.button == SDL_BUTTON_WHEELUP)
	{
		Math::limit(&(--currentPage), 1, numberOfPages);
		gotoPage(currentPage);
		
		itemSelected = true;
	}
	else if (mouse.button == SDL_BUTTON_WHEELDOWN)
	{
		Math::limit(&(++currentPage), 1, numberOfPages);
		gotoPage(currentPage);
		
		itemSelected = true;
	}

	int index = 0;

	for (ListViewItem *item = (ListViewItem*)items.getFirstElement() ; item != NULL ; item = (ListViewItem*)item->next)
	{
		if (item->pageNumber == currentPage)
		{
			if (Collision::collision(mouse.x, mouse.y, 1, 1, item->x, item->y, item->texture->iw, item->texture->ih))
			{
				selectedIndex = index;
				itemSelected = true;
			}
		}

		index++;
	}
	
	buttonState = 1;
}

void ListView::resync()
{
	if (getItemCount() > 0)
	{
		if (getSelectedIndex() > getItemCount() - 1)
		{
			setSelectedIndex(getItemCount() - 1);
		}
	}
	
	itemSelected = true;
}

void ListView::mouseReleased(SDL_MouseButtonEvent mouse)
{
	buttonState = 0;
}

void ListView::mouseMoved(int x, int y)
{
	if (buttonState == 0)
	{
		return;
	}
	
	int index = 0;

	for (ListViewItem *item = (ListViewItem*)items.getFirstElement() ; item != NULL ; item = (ListViewItem*)item->next)
	{
		if (item->pageNumber == currentPage)
		{
			if (Collision::collision(x, y, 1, 1, item->x, item->y, item->texture->iw, item->texture->ih))
			{
				selectedIndex = index;
				itemSelected = true;
			}
		}

		index++;
	}
}
