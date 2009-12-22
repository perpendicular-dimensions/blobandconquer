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

#include "../attributes.h"

class Button : public BaseWidget {

	private:

		unsigned int buttonState;
		bool clicked;

	public:

		String label;
		bool selected;

	Button();
	~Button();

	bool wasClicked();
	void setEnabled(bool enabled);
	void setVisible(bool visible);
	
	void use(int x, int y);

	void setLabel(const char *label, ...) GCC_PRINTF(2,3);
	void setProperties(Properties *properties);

	void mousePressed(SDL_MouseButtonEvent mouse);
	void mouseReleased(SDL_MouseButtonEvent mouse);
	void mouseMoved(int x, int y);

};
