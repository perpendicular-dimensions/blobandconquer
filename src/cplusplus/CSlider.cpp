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

Slider::Slider()
{
	widgetType = SLIDER;
	
	value = 0;
	
	min = 0;
	max = 0;
	
	step = 1;
	
	dir = 0;
	
	smooth = true;
	
	changed = false;
}

Slider::~Slider()
{
}

void Slider::setProperties(Properties *properties)
{
	setBaseValues(properties);

	min = properties->getInt("min", 0);
	max = properties->getInt("max", 100);
	
	step = properties->getInt("step", 1);

	value = properties->getInt("value", 0);
	
	smooth = properties->getInt("smooth", 1);
}

void Slider::use(int mx, int my)
{
	if (Collision::collision(mx, my, 1, 1, x + 24, y, width - 48, 16))
	{
		float percent;
		percent = mx - (x + 24);
		percent /= (width - 48);
		
		value = percent * max;
		
		changed = true;
	}
}

bool Slider::hasChanged()
{
	if (changed)
	{
		changed = false;
		return true;
	}
	
	return false;
}

void Slider::update()
{
	value += (dir * step * Engine::getInstance()->getTimeDifference(TD_ANIMATION));
	Math::limit(&value, min, max);
	
	if (!smooth)
	{
		dir = 0;
	}
	
	if ((dir == -1) || (dir == 1))
	{
		changed = true;
	}
}

void Slider::mouseMoved(int mx, int my)
{
	if (!mouseHeld)
	{
		return;
	}
	
	if ((mx >= x + 24) && (mx <= x + width - 16))
	{
		float percent;
		percent = mx - (x + 24);
		percent /= (width - 48);
		
		value = percent * max;
		
		Math::limit(&value, min, max);
		
		changed = true;
	}
	
	if (mx < x)
	{
		value = 0;
		
		changed = true;
	}
	
	if (mx > x + width)
	{
		value = max;
		
		changed = true;
	}
}

void Slider::mousePressed(SDL_MouseButtonEvent mouse)
{
	if (Collision::collision(mouse.x, mouse.y, 1, 1, x, y, 16, 16))
	{
		dir = -1;
	}
	
	if (Collision::collision(mouse.x, mouse.y, 1, 1, x + width - 16, y, 16, 16))
	{
		dir = 1;
	}
	
	if (Collision::collision(mouse.x, mouse.y, 1, 1, x + 24, y, width - 48, 16))
	{
		float percent;
		percent = mouse.x - (x + 24);
		percent /= (width - 48);
		
		value = percent * max;
		
		changed = true;
	}
	
	mouseHeld = true;
}

void Slider::mouseReleased(SDL_MouseButtonEvent mouse)
{
	dir = 0;
	mouseHeld = false;
}

void Slider::change(int x)
{
	value += (x * step);
	changed = true;
	Math::limit(&value, min, max);
}
