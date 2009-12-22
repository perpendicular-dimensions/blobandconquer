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

class ComboBox : public BaseWidget {

	public:
	
		bool changed;
		
		String currentItemText;
		
		List itemList;
		
		int selectedItem;

	ComboBox();
	~ComboBox();

	void setProperties(Properties *properties);
	
	void addItem(const char *item);
	void clear();
	
	void setSelectedItem(int selectedItem);

	bool hasChanged();
	
	void use(int x, int y);
	
	void change(int x);

	void mousePressed(SDL_MouseButtonEvent mouse);
	void mouseReleased(SDL_MouseButtonEvent mouse);

};
