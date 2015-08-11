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

class BaseWidget : public Linkable {

	protected:
		
		Grid *grid;
		int gridX, gridY;

		int widgetType;
		
		bool enabled;
		bool visible;
		bool clicked;

	public:

		String name, groupName;

		int x, y, width, height;
		int fontSize;
		int fontColorRed, fontColorGreen, fontColorBlue;
		int backgroundColorRed, backgroundColorGreen, backgroundColorBlue;

		int horizontalAlignment;
		int verticleAlignment;

		Texture *normalImage, *clickedImage, *disabledImage, *currentImage;

	BaseWidget();
	~BaseWidget();

	void setName(const char *name);
	const char *getName();

	void setGroupName(const char *name);
	const char *getGroupName();

	void setVisible(bool visible);
	virtual void setEnabled(bool enabled);
	
	bool isEnabled();
	bool isVisible();

	int getWidgetType();
	
	void updatePosition();

	virtual void use(int x, int y) = 0;
	virtual void mousePressed(SDL_MouseButtonEvent mouse);
	virtual void mouseReleased(SDL_MouseButtonEvent mouse);
	virtual void mouseMoved(int x, int y);
	virtual void mouseWheeled(SDL_MouseWheelEvent mouse);
	virtual void keyPressed(int key);
	virtual void joystickButtonPressed(int key);

	void setBaseValues(Properties *properties);
	virtual void setProperties(Properties *properties) = 0;

};
