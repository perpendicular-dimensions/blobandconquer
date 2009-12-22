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

class Sprite : public Linkable {

	private:

		int maxFrames;

	public:

		String name;

		bool loops;

		SDL_Surface **image;
		unsigned int *frameLength;

		unsigned char currentFrame;
		float currentTime;

	Sprite();
	~Sprite();
	const char *getName();
	void setNumberOfFrames(int i);
	void setFrame(int i, SDL_Surface *shape, int time);
	void animate();
	SDL_Surface *getFrame(int frame);
	void getNextFrame(int *frame, int *time);
	SDL_Surface *getCurrentFrame();
	void destroy();

};
