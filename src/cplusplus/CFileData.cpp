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

FileData::FileData()
{
	strcpy(name, "");
	fSize = 0;
	cSize = 0;
	location = 0;
}

void FileData::set(const char *name, uLongf fSize, uLongf cSize, uLongf location)
{
	strcpy(this->name, name);
	this->fSize = fSize;
	this->cSize = cSize;
	this->location = location;
}

void FileData::swapEndians()
{
	fSize = SDL_SwapLE32(fSize);
	cSize = SDL_SwapLE32(cSize);
	location = SDL_SwapLE32(location);
}
