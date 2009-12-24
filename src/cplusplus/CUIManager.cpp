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

UIManager::UIManager()
{
	focusedWidget = NULL;

	gridList.setName("Grid List");
	widgetList.setName("Widget List");

	modalGroup = "";
	
	mouseX = 0;
	mouseY = 0;
	
	OKDialogGrid.name = "OKDialogGrid";
	OKDialogGrid.rows = 1;
	OKDialogGrid.cols = 3;
	
	OKDialogGrid.name = "OKCancelDialogGrid";
	OKCancelDialogGrid.rows = 1;
	OKCancelDialogGrid.cols = 4;
}

UIManager *UIManager::getInstance()
{
	return &instance;
}

UIManager::~UIManager()
{
	gridList.clear();
	widgetList.clear();
	focusedWidget = NULL;
	
	delete upArrow;
	delete downArrow;
	delete minus;
	delete plus;
	delete checked;
	delete unchecked;
	delete awaitingButton;
}

Grid *UIManager::spawnGrid()
{
	Grid *grid = new Grid();
	
	gridList.add(grid);
	
	return grid;
}

Grid *UIManager::getGrid(const char *name)
{
	if (strcmp(name, "OKCancelDialogGrid") == 0)
	{
		return &OKCancelDialogGrid;
	}
	else if (strcmp(name, "OKDialogGrid") == 0)
	{
		return &OKDialogGrid;
	}
	
	for (Grid *grid = (Grid*)gridList.getFirstElement() ; grid != NULL ; grid = (Grid*)grid->next)
	{
		if (grid->name == name)
		{
			return grid;
		}
	}
	
	Graphics::getInstance()->showErrorAndExit("The requested grid '%s' does not exist", name);

	return NULL;
}

List *UIManager::getGridList()
{
	return &gridList;
}

BaseWidget *UIManager::spawnWidget(int type)
{
	BaseWidget *widget = NULL;
	
	switch (type)
	{
		case PANEL:
			widget = new Panel();
			break;
		
		case BUTTON:
			widget = new Button();
			break;

		case SLIDER:
			widget = new Slider();
			break;

		case LABEL:
			widget = new Label();
			break;

		case LISTVIEW:
			widget = new ListView();
			break;

		case CHECKBOX:
			widget = new CheckBox();
			break;
			
		case COMBOBOX:
			widget = new ComboBox();
			break;
			
		case INPUT:
			widget = new InputWidget();
			break;
			
		default:
			printf("Error - UIManager::spawnWidget() - Unknown type\n");
			exit(1);
			break;
	}

	widgetList.add(widget);

	return widget;
}

void UIManager::removeWidget(BaseWidget *widget)
{
	if (focusedWidget == widget)
	{
		focusedWidget = NULL;
	}

	widgetList.remove(widget);
}

void UIManager::removeGroup(const char *group)
{
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if (widget->groupName == group)
		{
			debug(("UIManager::removeGroup - Deleting Widget %s:%s\n", widget->getName(), widget->getGroupName()));
			widget = (BaseWidget*)widget->previous;
			widgetList.remove(widget->next);
		}
	}
	
	focusedWidget = NULL;
}

void UIManager::showGroup(const char *name, bool visible)
{
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if (widget->groupName == name)
		{
			widget->setVisible(visible);
		}
	}
	
	focusedWidget = NULL;
}

void UIManager::enableGroup(const char *group, bool enabled)
{
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if (widget->groupName == group)
		{
			widget->setEnabled(enabled);
		}
	}
	
	focusedWidget = NULL;
}

void UIManager::onlyShowGroup(const char *group)
{
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if (widget->groupName == group)
		{
			widget->setVisible(true);
		}
		else
		{
			widget->setVisible(false);
		}
	}
	
	focusedWidget = NULL;
}

void UIManager::onlyHideGroup(const char *group)
{
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if (widget->groupName == group)
		{
			widget->setVisible(false);
		}
		else
		{
			widget->setVisible(true);
		}
	}
	
	focusedWidget = NULL;
}

void UIManager::setModal(const char *group)
{
	modalGroup = group;
}

BaseWidget *UIManager::getWidget(const char *name, const char *groupName)
{
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if ((widget->name == name) && (widget->groupName == groupName))
		{
			return widget;
		}
	}
	
	Graphics::getInstance()->showErrorAndExit("The requested widget '%s' does not exist", name);

	return NULL;
}

List *UIManager::getWidgetList()
{
	return &widgetList;
}

void UIManager::recalculateWidgetPositions()
{
	for (Grid *grid = (Grid*)gridList.getFirstElement() ; grid != NULL ; grid = (Grid*)grid->next)
	{
		grid->updatePosition();
	}
	
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		widget->updatePosition();
	}
}

void UIManager::clearWidgetList()
{
	widgetList.clear();
	
	focusedWidget = NULL;
}

void UIManager::mousePressed(SDL_MouseButtonEvent mouse)
{
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if ((modalGroup != "") && (modalGroup != widget->getGroupName()))
		{
			continue;
		}
		
		if ((!widget->isEnabled()) || (!widget->isVisible()))
		{
			continue;
		}
		
		if (Collision::collision(mouse.x, mouse.y, 1, 1, widget->x, widget->y, widget->width, widget->height))
		{
			if ((focusedWidget != NULL) && (focusedWidget != widget))
			{
				if (focusedWidget->getWidgetType() == INPUT)
				{
					((InputWidget*)focusedWidget)->awaitingInput = false;
				}
			}
			
			if (mouse.button != SDL_BUTTON_LEFT)
			{
				if ((widget->getWidgetType() != INPUT) && (widget->getWidgetType() != LISTVIEW))
				{
					return;
				}
			}
			
			focusedWidget = widget;
			focusedWidget->mousePressed(mouse);
			
			return;
		}
	}
	
	focusedWidget = NULL;
}

bool UIManager::widgetHit(int x, int y)
{
	focusedWidget = NULL;
	
	for (BaseWidget *widget = (BaseWidget*)widgetList.getFirstElement() ; widget != NULL ; widget = (BaseWidget*)widget->next)
	{
		if ((modalGroup != "") && (modalGroup != widget->getGroupName()))
		{
			continue;
		}
		
		if ((!widget->isEnabled()) || (!widget->isVisible()))
		{
			continue;
		}
		
		if (Collision::collision(x, y, 1, 1, widget->x, widget->y, widget->width, widget->height))
		{
			//if ((widget->getWidgetType() == INPUT) || (widget->getWidgetType() == LISTVIEW))
			if (widget->getWidgetType() == LISTVIEW)
			{
				return false;
			}
			
			focusedWidget = widget;
			widget->use(x, y);
			
			return true;
		}
	}
	
	return false;
}

void UIManager::mouseReleased(SDL_MouseButtonEvent mouse)
{
	//debug(("UIManager::mouseReleased()\n"));
	
	if (focusedWidget == NULL)
	{
		return;
	}

	focusedWidget->mouseReleased(mouse);
}

void UIManager::mouseMoved(int x, int y)
{
	mouseX = x;
	mouseY = y;
	
	if (focusedWidget == NULL)
	{
		return;
	}

	focusedWidget->mouseMoved(x, y);
}

void UIManager::keyPressed(int key, bool shiftHeld)
{
	//debug(("UIManager::keyPressed() - %d : %c\n", key, (char)key));

	if (focusedWidget == NULL)
	{
		return;
	}

	focusedWidget->keyPressed(key, shiftHeld);
}

void UIManager::joystickButtonPressed(int key)
{
	if (focusedWidget == NULL)
	{
		return;
	}

	focusedWidget->joystickButtonPressed(key);
}

UIManager UIManager::instance;
