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

Sprite::Sprite()
{
	name = "Unnamed Sprite";

	image = NULL;
	frameLength = NULL;

	currentFrame = 0;
	currentTime = 0;
	maxFrames = -1;

	loops = false;

	next = NULL;
}

Sprite::~Sprite()
{
	for (int i = 0 ; i < maxFrames ; i++)
	{
		if (image[i] != NULL)
		{
			SDL_FreeSurface(image[i]);
		}
	}

	if (image != NULL)
	{
		delete[] image;
	}
	
	if (frameLength != NULL)
	{
		delete[] frameLength;
	}
}

const char *Sprite::getName()
{
	return name.getText();
}

void Sprite::setNumberOfFrames(int i)
{
	if (maxFrames != -1)
	{
		printf("WARNING: Cannot set maximum frames for sprite '%s' - Already set!\n", getName());
	}

	maxFrames = i;

	image = new SDL_Surface*[maxFrames];
	
	for (int j = 0 ; j < maxFrames ; j++)
	{
		image[j] = NULL;
	}

	frameLength = new unsigned int[maxFrames];
}

void Sprite::setFrame(int i, SDL_Surface *shape, int time)
{
	if (i >= maxFrames)
	{
		printf("WARNING: Couldn't set frame %d >= maximum %d for sprite '%s'!\n", i, maxFrames, getName());
		return;
	}

	image[i] = shape;
	frameLength[i] = time;

	currentFrame = 0;
	currentTime = frameLength[0];

	if (i > maxFrames)
	{
		maxFrames = i;
	}
}

void Sprite::animate()
{
	currentTime--;

	if (currentTime > 0)
	{
		return;
	}

	currentFrame++;

	if (currentFrame == maxFrames)
	{
		currentFrame = 0;
	}

	if (frameLength[currentFrame] == 0)
	{
		currentFrame = 0;
	}

	currentTime = frameLength[currentFrame];
}

SDL_Surface *Sprite::getFrame(int frame)
{
	if (frame >= maxFrames)
	{
		printf("WARNING : No such frame '%d' in sprite '%s'\n", frame, name.getText());
		return image[0];
	}

	return image[frame];
}

void Sprite::getNextFrame(int *frame, int *time)
{
	if (*frame >= maxFrames)
	{
		if (loops)
		{
			*frame = 0;
		}
		else
		{
			*frame = maxFrames - 1;
		}
	}

	if (frameLength[*frame] == 0)
	{
		if (loops)
		{
			*frame = 0;
		}
		else
		{
			*frame = maxFrames - 1;
		}
	}

	*time = frameLength[*frame];
}

SDL_Surface *Sprite::getCurrentFrame()
{
	return image[currentFrame];
}
