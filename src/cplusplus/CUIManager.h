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

class UIManager {

	private:

		static UIManager instance;

		List gridList;
		List widgetList;

		int mouseX, mouseY;

		UIManager();

	public:
		
		String modalGroup;

		Texture *upArrow, *downArrow, *minus, *plus;
		Texture *checked, *unchecked, *awaitingButton;
		
		BaseWidget *focusedWidget;
		
		// special grids
		Grid OKCancelDialogGrid, OKDialogGrid;

	static UIManager *getInstance();
	~UIManager();
	
	Grid *spawnGrid();
	Grid *getGrid(const char *name);

	BaseWidget *spawnWidget(int type);
	void removeWidget(BaseWidget *widget);
	void removeGroup(const char *group);

	void showGroup(const char *name, bool hide);
	void enableGroup(const char *group, bool enable);

	void onlyShowGroup(const char *group);
	void onlyHideGroup(const char *group);

	void setModal(const char *group);

	BaseWidget *getWidget(const char *name, const char *groupName);

	List *getWidgetList();
	List *getGridList();
	
	void recalculateWidgetPositions();

	void clearWidgetList();

	bool widgetHit(int x, int y);
	void mousePressed(SDL_MouseButtonEvent mouse);
	void mouseReleased(SDL_MouseButtonEvent mouse);
	void mouseMoved(int x, int y);
	void keyPressed(int key, bool shiftHeld);
	void joystickButtonPressed(int key);

};
