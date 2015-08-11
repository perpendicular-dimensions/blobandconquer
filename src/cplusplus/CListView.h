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

class ListView : public BaseWidget {

	private:

		List items;
		bool itemSelected;
		int selectedIndex;
		int numberOfPages;
		int currentPage;
		int buttonState;

	public:

		int startIndex;
		int endIndex;
		int visibleItems;
		
		bool border;
		bool transparent;

	ListView();
	~ListView();

	void calculateNumberOfPages();
	void addItem(ListViewItem *item);
	void removeItem(ListViewItem *item);

	void gotoPage(int pageNumber);
	int getItemCount();
	int getNumberOfPages();
	int getCurrentPage();

	ListViewItem *getSelectedItem();
	Linkable *getSelectedObject();
	int getSelectedIndex();

	void setSelectedIndex(int i);
	void setSelectedObject(Linkable *link);

	List *getList();

	void setProperties(Properties *properties);
	
	bool itemSelectionChanged();
	
	void resync();
	
	void use(int x, int y);

	void mousePressed(SDL_MouseButtonEvent mouse);
	void mouseReleased(SDL_MouseButtonEvent mouse);
	void mouseMoved(int x, int y);
	void mouseWheeled(SDL_MouseWheelEvent mouse);

};
