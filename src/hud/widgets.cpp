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

#include "widgets.h"

void drawSelectedListItem(float x, float y, float w, float h, bool isSelectedWidget)
{
	glPushMatrix();
	{
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		
		glTranslatef(x, y, 0);
		
		glBegin(GL_QUADS);
		{
			if (isSelectedWidget)
			{
				glColor3f(0, 0, 0.25); glVertex2d(0, 0);
				glColor3f(0, 0, 0.25); glVertex2d(w, 0);
				glColor3f(0, 0, 0.75); glVertex2d(w, h);
				glColor3f(0, 0, 0.75); glVertex2d(0, h);
			}
			else
			{
				glColor3f(0.15, 0.15, 0.15); glVertex2d(0, 0);
				glColor3f(0.15, 0.15, 0.15); glVertex2d(w, 0);
				glColor3f(0.25, 0.25, 0.25); glVertex2d(w, h);
				glColor3f(0.25, 0.25, 0.25); glVertex2d(0, h);
			}
		}
		glEnd();
	}
	glPopMatrix();
}

void drawListView(ListView *listView)
{
	int yLoc, index;
	
	listView->visibleItems = 0;
	yLoc = listView->y + 2;
	index = 0;
	GLColor blue, green;
	
	blue.set(0.0, 0.0, 0.45, 1.0);
	green.set(0.0, 0.45, 0, 1.0);
	
	if (listView->getNumberOfPages() > 1)
	{
		graphics->drawString(listView->x + listView->width, listView->y - 20, TXT_RIGHT, GLColor::white, true, _("Page %d / %d"), listView->getCurrentPage(), listView->getNumberOfPages());
	}
	
	if (!listView->transparent)
	{
		graphics->drawRect(listView->x, listView->y, listView->width, listView->height, GLColor::black, GLColor::white, false);
	}
	else
	{
		graphics->fade(0.75, listView->x, listView->y, listView->width, listView->height);
		
		if (listView->border)
		{
			graphics->outlineRect(listView->x, listView->y, listView->width, listView->height, GLColor::white);
		}
	}

	for (ListViewItem *item = (ListViewItem*)listView->getList()->getFirstElement() ; item != NULL ; item = (ListViewItem*)item->next)
	{
		if (index < listView->startIndex)
		{
			index++;
			continue;
		}
		
		if (!listView->transparent)
		{
			graphics->drawRect(listView->x + 2, yLoc, item->texture->iw - 4, item->texture->ih, GLColor::black, false);
		}
		
		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_BLEND);
		graphics->blit(item->texture, listView->x + 2, yLoc, false);
		
		if (index == listView->getSelectedIndex())
		{
			drawSelectedListItem(listView->x + 2, yLoc, item->texture->iw - 4, item->texture->ih, (uiManager->focusedWidget == listView));
		}

		item->x = listView->x + 2;
		item->y = yLoc;

		yLoc += (item->texture->ih + 2);

		if (yLoc + item->texture->ih + 2 > listView->height + listView->y)
		{
			break;
		}

		index++;
	}
	
	listView->endIndex = index;
	Math::limit(&listView->endIndex, 0, listView->getItemCount() - 1);
}

void drawCheckBox(CheckBox *checkBox)
{
	if (checkBox->checked)
	{
		graphics->blit(uiManager->checked, checkBox->x, checkBox->y, false);
	}
	else
	{
		graphics->blit(uiManager->unchecked, checkBox->x, checkBox->y, false);
	}
}

void drawSlider(Slider *slider)
{
	float percentage = slider->value;
	percentage /= slider->max;
	percentage *= (slider->width - 48);
	percentage = (int)percentage;
	
	GLColor c;
	c.set(0.0, 0.25, 0.0, 1.0);
	float b = 50;
	b /= slider->height;
	b *= 0.01;
	
	for (int i = 0 ; i < slider->height ; i++)
	{
		graphics->drawRect(slider->x + 24, slider->y + i, (int)percentage, 1, c, false);
		c.color[1] += b;
	}
	
	if ((slider->value != 0) && (slider->value != slider->max))
	{
		graphics->drawRect(slider->x + 25 + (int)percentage - 1, slider->y, 1, slider->height, GLColor::green, false);
	}
	
	graphics->outlineRect(slider->x + 24, slider->y, slider->width - 48, slider->height, GLColor::white);
	
	glEnable(GL_BLEND);
	
	(percentage == 0) ? glColor4f(1.0, 1.0, 1.0, 0.25) : glColor3f(1.0, 1.0, 1.0);
	
	graphics->blit(uiManager->minus, slider->x, slider->y - 1, false);
	
	(slider->value == slider->max) ? glColor4f(1.0, 1.0, 1.0, 0.25) : glColor3f(1.0, 1.0, 1.0);
	
	graphics->blit(uiManager->plus, (slider->x + slider->width - 16), slider->y - 1, false);
	
	slider->update();
}

void drawComboBox(ComboBox *comboBox)
{
	graphics->setFont(FONT_NORMAL);
	
	graphics->drawString((comboBox->x + comboBox->width / 2), comboBox->y, TXT_CENTERED, GLColor::white, true, "%s", comboBox->currentItemText.getText());
	
	(comboBox->selectedItem != 0) ? glColor3f(1.0, 1.0, 1.0) : glColor4f(1.0, 1.0, 1.0, 0.25);
	
	graphics->blit(uiManager->minus, comboBox->x, comboBox->y - 1, false);
	
	(comboBox->selectedItem != comboBox->itemList.getSize() - 1) ? glColor3f(1.0, 1.0, 1.0) : glColor4f(1.0, 1.0, 1.0, 0.25);
	
	graphics->blit(uiManager->plus, (comboBox->x + comboBox->width - 16), comboBox->y - 1, false);
}

void drawInputWidget(InputWidget *inputWidget)
{
	static bool useOr;
	static String renderString;
	static String keyString, joyString, mouseString;
	
	useOr = false;
	
	if (inputWidget->redraw)
	{
		renderString = _("N/A");
		keyString = "";
		joyString = "";
		mouseString = "";
		
		inputWidget->redraw = false;
		
		if (inputWidget->value[INPUT_KEY] != -1)
		{
			String keyName;
			keyName = SDL_GetKeyName((SDL_Keycode)inputWidget->value[INPUT_KEY]);
			
			if (keyName != "unknown key")
			{
				keyName.captialise();
				keyString = _(keyName.getText());
				useOr = true;
			}
		}
		
		if (inputWidget->value[INPUT_JOY] != -1)
		{
			if (useOr)
			{
				joyString.setText(_(" or Joy %d"), inputWidget->value[INPUT_JOY]);
			}
			else
			{
				joyString.setText(_("Joy %d"), inputWidget->value[INPUT_JOY]);
			}
			
			useOr = true;
		}
		
		if (inputWidget->value[INPUT_MOUSE] != -1)
		{
			if (useOr)
			{
				mouseString.setText(_(" or Mouse %s"), mouse->translateMouseButton(inputWidget->value[INPUT_MOUSE]));
			}
			else
			{
				mouseString.setText(_("Mouse %s"), mouse->translateMouseButton(inputWidget->value[INPUT_MOUSE]));
			}
			
			useOr = true;
		}
		
		if ((keyString != "") || (joyString != "") || (mouseString != ""))
		{
			renderString.setText("%s%s%s", keyString.getText(), joyString.getText(), mouseString.getText());
		}
		
		if (inputWidget->normalImage != NULL)
		{
			delete inputWidget->normalImage;
		}
		
		inputWidget->normalImage = graphics->getGLString("%s", renderString.getText());
		
		glEnable(GL_BLEND);
		
		inputWidget->currentImage = inputWidget->normalImage;
		
		inputWidget->width = inputWidget->normalImage->iw;
		inputWidget->height = inputWidget->normalImage->ih;
	}
	
	if (!inputWidget->awaitingInput)
	{
		glColor3f(0.0, 1.0, 0.0);
		graphics->blit(inputWidget->currentImage, inputWidget->x, inputWidget->y - 5, false);
	}
	else
	{
		glColor3f(0.5, 0.5, 0.5);
		graphics->blit(uiManager->awaitingButton, inputWidget->x, inputWidget->y - 5, false);
	}
}

void createWidgetColors(BaseWidget *widget, Properties *props)
{
	int red, green, blue;

	props->getInt("width", 0);

	sscanf(props->getString("fontColor", "255 255 255"), "%d %d %d", &red, &green, &blue);
	widget->fontColorRed = red;
	widget->fontColorGreen = green;
	widget->fontColorBlue = blue;

	sscanf(props->getString("backgroundColor", "180 180 180"), "%d %d %d", &red, &green, &blue);
	widget->backgroundColorRed = red;
	widget->backgroundColorGreen = green;
	widget->backgroundColorBlue = blue;
}

BaseWidget *createPanelWidget(Properties *props)
{
	//debug(("createPanelWidget()\n"));

	Panel *panel = (Panel*)uiManager->spawnWidget(PANEL);

	panel->setProperties(props);
	
	return panel;
}

BaseWidget *createButtonWidget(Properties *props)
{
	//debug(("createButtonWidget()\n"));

	Button *button = (Button*)uiManager->spawnWidget(BUTTON);

	button->setProperties(props);
	createWidgetColors(button, props);

	int disabledRed, disabledGreen, disabledBlue;
	
	disabledRed = button->backgroundColorRed / 2;
	disabledGreen = button->backgroundColorGreen / 2;
	disabledBlue = button->backgroundColorBlue / 2;
	
	Math::limit(&disabledRed, 0, 255);
	Math::limit(&disabledGreen, 0, 255);
	Math::limit(&disabledBlue, 0, 255);

	graphics->setFontColor(0xff, 0xff, 0xaa, 0x00, 0x00, 0x00);
	graphics->setFont(button->fontSize);
	SDL_Surface *labelImage = graphics->getString(true, "%s", button->label.getText());
	
	graphics->setFontColor(0x88, 0x88, 0x88, 0x00, 0x00, 0x00);
	graphics->setFont(button->fontSize);
	SDL_Surface *disabledLabelImage = graphics->getString(true, "%s", button->label.getText());

	int colSpan = props->getInt("colSpan", 1);

	colSpan = max(colSpan, 1);
	
	Grid *grid = uiManager->getGrid(props->getString("grid", "unknown"));
	
	int width = (grid->getCellWidth() * colSpan) - (grid->cellSpacing * 2);

	int height = labelImage->h + 5;
	
	SDL_Surface *normalImage, *clickedImage, *disabledImage;

	// create the unclicked image for the button
	normalImage = graphics->createSurface(width, height);
	graphics->drawGradient(normalImage, button->backgroundColorRed, button->backgroundColorGreen, button->backgroundColorBlue, 0, 0, 0);
	graphics->blit(labelImage, width / 2, height / 2, normalImage, true);
	graphics->createRaisedBorder(normalImage, graphics->white, graphics->darkGrey);

	// create the clicked image for the button
	clickedImage = graphics->createSurface(width, height);
	graphics->drawGradient(clickedImage, button->backgroundColorRed, button->backgroundColorGreen, button->backgroundColorBlue, 0, 0, 0);
	graphics->blit(labelImage, (width / 2) + 1, (height / 2) + 1, clickedImage, true);
	graphics->createLoweredBorder(clickedImage, graphics->white, graphics->darkGrey);
	
	// create the disabled image for the button
	disabledImage = graphics->createSurface(width, height);
	graphics->drawGradient(disabledImage, disabledRed, disabledGreen, disabledBlue, 0, 0, 0);
	graphics->blit(disabledLabelImage, width / 2, height / 2, disabledImage, true);
	
	button->normalImage = graphics->createTexture(normalImage);
	button->clickedImage = graphics->createTexture(clickedImage);
	button->disabledImage = graphics->createTexture(disabledImage);
	
	String s;
	
	s.setText("normalImage-%s-%s", button->getName(), button->getGroupName());
	textureManager->addTexture(s.getText(), button->normalImage);
	
	s.setText("clickedImage-%s-%s", button->getName(), button->getGroupName());
	textureManager->addTexture(s.getText(), button->clickedImage);
	
	s.setText("disabledImage-%s-%s", button->getName(), button->getGroupName());
	textureManager->addTexture(s.getText(), button->disabledImage);

	button->currentImage = button->normalImage;
	button->width = button->currentImage->iw;
	button->height = button->currentImage->ih;
	
	SDL_FreeSurface(labelImage);
	SDL_FreeSurface(disabledLabelImage);

	return button;
}

BaseWidget *createCheckBoxWidget(Properties *props)
{
	CheckBox *checkBox = (CheckBox*)uiManager->spawnWidget(CHECKBOX);
	checkBox->setProperties(props);
	
	checkBox->width = 16;
	checkBox->height = 16;

	return checkBox;
}

BaseWidget *createListViewWidget(Properties *props)
{
	//debug(("createListViewWidget()\n"));

	ListView *listView = (ListView*)uiManager->spawnWidget(LISTVIEW);

	listView->setProperties(props);
	createWidgetColors(listView, props);

	//drawListView(listView);

	listView->currentImage = listView->normalImage;

	return listView;
}

BaseWidget *createSliderWidget(Properties *props)
{
	//debug(("createSliderWidget()\n"));

	Slider *slider = (Slider*)uiManager->spawnWidget(SLIDER);

	slider->setProperties(props);
	createWidgetColors(slider, props);
	
	Grid *grid = uiManager->getGrid(props->getString("grid", "unknown"));
	
	int colSpan = props->getInt("colSpan", 1);
	int rowSpan = props->getInt("rowSpan", 1);
	
	slider->width = (grid->getCellWidth() * colSpan) - (grid->cellSpacing * 2);
	slider->height = (grid->getCellHeight() * rowSpan) - (grid->cellSpacing * 2);

	return slider;
}

BaseWidget *createComboBoxWidget(Properties *props)
{
	//debug(("createComboBoxWidget()\n"));

	ComboBox *comboBox = (ComboBox*)uiManager->spawnWidget(COMBOBOX);

	comboBox->setProperties(props);
	createWidgetColors(comboBox, props);
	
	Grid *grid = uiManager->getGrid(props->getString("grid", "unknown"));
	
	int colSpan = props->getInt("colSpan", 1);
	int rowSpan = props->getInt("rowSpan", 1);
	
	comboBox->width = (grid->getCellWidth() * colSpan) - (grid->cellSpacing * 2);
	comboBox->height = (grid->getCellHeight() * rowSpan) - (grid->cellSpacing * 2);

	return comboBox;
}

BaseWidget *createLabelWidget(Properties *props)
{
	//debug(("createLabelWidget()\n"));

	Label *label = (Label*)uiManager->spawnWidget(LABEL);

	label->setProperties(props);
	createWidgetColors(label, props);

	graphics->setFontColor(0xff, 0xff, 0xff, 0x00, 0x00, 0x00);

	label->currentImage = graphics->getGLString("%s", label->text.getText());

	return label;
}

BaseWidget *createInputWidget(Properties *props)
{
	//debug(("createLabelWidget()\n"));
	
	int type = engine->getValueOfDefine(props->getString("widgetType", "INPUT_KEY"));

	InputWidget *inputWidget = (InputWidget*)uiManager->spawnWidget(type);

	inputWidget->setProperties(props);
	createWidgetColors(inputWidget, props);

	return inputWidget;
}

void loadWidgets(const char *file)
{
	debug(("loadWidgets()\n"));
	
	List *widgetList = loadProperties(file, true);
	
	BaseWidget *widget;
	Grid *grid;

	int widgetType = -1;

	for (Properties *props = (Properties*)widgetList->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		if (props->name == "Grid")
		{
			grid = uiManager->spawnGrid();
			grid->setProperties(props);
			continue;
			
			debug(("Created Grid '%s'\n", grid->name.getText()));
		}
		
		if (props->name != "Widget")
		{
			printf("WARNING: Properties not named 'Widget'. Skipping.\n");
			continue;
		}

		widget = NULL;
		widgetType = engine->getValueOfDefine(props->getString("widgetType", "-1"));

		switch (widgetType)
		{
			case BUTTON:
				widget = createButtonWidget(props);
				break;
				
			case PANEL:
				widget = createPanelWidget(props);
				break;

			case SLIDER:
				widget = createSliderWidget(props);
				break;

			case LABEL:
				widget = createLabelWidget(props);
				break;

			case LISTVIEW:
				widget = createListViewWidget(props);
				break;

			case CHECKBOX:
				widget = createCheckBoxWidget(props);
				break;
				
			case COMBOBOX:
				widget = createComboBoxWidget(props);
				break;
				
			case INPUT:
				widget = createInputWidget(props);
				break;

			default:
				printf("ERROR: loadWidgets() - Widget of type '%s' unknown!\n", props->getString("name", "unnamed"));
				exit(1);
				break;
		}
		
		widget->updatePosition();
	}
	
	if (widgetList != NULL)
	{
		delete widgetList;
	}
}

void drawWidgets()
{
	Panel *panel;
	GLColor color;
	
	graphics->setMode(MODE_2D);
	
	for (BaseWidget *widget = (BaseWidget*)uiManager->getWidgetList()->getFirstElement() ; widget != NULL ; widget = (BaseWidget*) widget->next)
	{
		if (!widget->isVisible())
		{
			continue;
		}
		
		if ((uiManager->modalGroup != "") && (uiManager->modalGroup != widget->getGroupName()))
		{
			continue;
		}
		
		if (widget->currentImage == NULL)
		{
			widget->currentImage = widget->normalImage;
		}
		
		if (widget->getWidgetType() == PANEL)
		{
			panel = (Panel*)widget;
			graphics->fade(0.75, panel->x, panel->y, panel->width, panel->height);
			graphics->outlineRect(panel->x, panel->y, panel->width, panel->height, GLColor::white);
		}
		else
		{
			if (widget->getWidgetType() == BUTTON)
			{
				graphics->drawRect(widget->x, widget->y, widget->width, widget->height, GLColor::black, false);
				
				glColor4f(1.0, 1.0, 1.0, 1.0);
				
				if (!mouse->isBusy())
				{
					if ((widget->getWidgetType() == BUTTON) && (widget->isEnabled()))
					{
						if (Collision::collision(mouse->x, mouse->y, 1, 1, widget->x, widget->y, widget->width, widget->height))
						{
							glColor4f(1.0, 0.0, 1.0, 1.0);
						}
					}
				}
			}
			else
			{
				glColor4f(1.0, 1.0, 1.0, 1.0);
			}
		}
		
		switch (widget->getWidgetType())
		{
			case -1:
				printf("ERROR: %s has type -1!\n", widget->getName());
				exit(1);
				break;	
			
			case LISTVIEW:
				drawListView((ListView*)widget);
				break;
					
			case CHECKBOX:
				drawCheckBox((CheckBox*)widget);
				break;
				
			case SLIDER:
				drawSlider((Slider*)widget);
				break;
				
			case COMBOBOX:
				drawComboBox((ComboBox*)widget);
				break;
				
			case LABEL:
				glEnable(GL_BLEND);
				graphics->blit(widget->currentImage, widget->x, widget->y - 5, false);
				break;
				
			case INPUT:
				drawInputWidget((InputWidget*)widget);
				break;
					
			default:
				glEnable(GL_BLEND);
				graphics->blit(widget->currentImage, widget->x, widget->y, false);
				break;
		}
	}
	
	if (!game->doingPuzzle)
	{
		moveMouseCursor();
	}
}

void sliderComboNonMouseControl(int x)
{
	Slider *slider;
	ComboBox *comboBox;

	switch (uiManager->focusedWidget->getWidgetType())
	{
		case SLIDER:
			slider = (Slider*)uiManager->focusedWidget;
			slider->change(x);
			break;
			
		case COMBOBOX:
			comboBox = (ComboBox*)uiManager->focusedWidget;
			comboBox->change(x);
			break;
	}
}

void listViewNonMouseControl(int y)
{
	ListView *view = (ListView*)uiManager->focusedWidget;
	
	int selectedIndex = view->getSelectedIndex();

	if (y == 1)
	{
		selectedIndex++;
	}
	else if (y == -1)
	{
		selectedIndex--;
	}
	
	Math::limit(&selectedIndex, view->startIndex, view->endIndex);
	
	view->setSelectedIndex(selectedIndex);
	view->resync();
}

void moveToWidget(int x, int y)
{
	BaseWidget *closest = NULL;
	float currentDistance = RAND_MAX;
	float distance;
	int mx, my;
	int fx, fy;
	
	if ((uiManager->focusedWidget != NULL) && (uiManager->focusedWidget->isVisible()))
	{
		fx = uiManager->focusedWidget->x;
		fy = uiManager->focusedWidget->y;
	
		switch (uiManager->focusedWidget->getWidgetType())
		{
			case SLIDER:
			case COMBOBOX:
				if (x != 0)
				{
					sliderComboNonMouseControl(x);
					return;
				}
				break;
				
			case LISTVIEW:
				if (x == 0)
				{
					listViewNonMouseControl(y);
					return;
				}
				else
				{
					fx = uiManager->focusedWidget->x;
					fy = uiManager->focusedWidget->y + uiManager->focusedWidget->height;
					fx = -fx;
					fy = -fy;
					uiManager->focusedWidget = NULL;
				}
				break;
				
			default:
				break;
		}
	}
	else
	{
		fx = -mouse->x;
		fy = -mouse->y;
	}

	for (BaseWidget *widget = (BaseWidget*)uiManager->getWidgetList()->getFirstElement() ; widget != NULL ; widget = (BaseWidget*) widget->next)
	{
		if ((!widget->isVisible()) || (!widget->isEnabled()))
		{
			continue;
		}
		
		if (widget == uiManager->focusedWidget)
		{
			continue;
		}
		
		if ((uiManager->modalGroup != "") && (uiManager->modalGroup != widget->getGroupName()))
		{
			continue;
		}
		
		if ((fx < 0) && (fy < 0))
		{
			distance = Math::getDistance(widget->x, widget->y, abs(fx), abs(fy));
			
			if ((distance <= 0) || (distance > currentDistance))
			{
				continue;
			}
			
			currentDistance = distance;
			closest = widget;
		}
		
		distance = Math::getDistance(widget->x, widget->y, fx, fy);
		
		if (x == -1)
		{
			if ((widget->x >= fx) || (widget->y != fy))
			{
				continue;
			}
		
			if ((distance <= 0) || (distance > currentDistance))
			{
				continue;
			}
			
			currentDistance = distance;
			closest = widget;
		}
		
		if (x == 1)
		{
			if ((widget->x <= fx) || (widget->y != fy))
			{
				continue;
			}
		
			if ((distance <= 0) || (distance > currentDistance))
			{
				continue;
			}
			
			currentDistance = distance;
			closest = widget;
		}
		
		if (y == -1)
		{
			if (widget->y >= fy)
			{
				continue;
			}
		
			if ((distance <= 0) || (distance > currentDistance))
			{
				continue;
			}
			
			currentDistance = distance;
			closest = widget;
		}
		
		if (y == 1)
		{
			if (widget->y <= fy)
			{
				continue;
			}
		
			if ((distance <= 0) || (distance > currentDistance))
			{
				continue;
			}
			
			currentDistance = distance;
			closest = widget;
		}
	}
	
	if (closest != NULL)
	{
		mx = closest->x + (closest->width / 2);
		my = closest->y + (closest->height / 2);
		mouse->set(mx, my);
		uiManager->focusedWidget = closest;
	}
}
