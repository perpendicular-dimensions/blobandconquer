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

TextureManager::TextureManager()
{
	textureTable.name = "Texture Table";
}

TextureManager *TextureManager::getInstance()
{
	return &instance;
}

TextureManager::~TextureManager()
{
	textureTable.clear();
}

void TextureManager::clear()
{
	textureTable.clear();
}

void TextureManager::addTexture(const char *name, Texture *texture)
{
	//debug(("TextureManager::addTexture() - %s [%d,%d ; %d,%d]\n", name, texture->iw, texture->ih, texture->tw, texture->th));
	
	textureTable.put(name, texture);
	
	texture->name = name;
}

void TextureManager::removeTexture(const char *name)
{
	textureTable.remove(name);
}

Texture *TextureManager::getTexture(const char *name)
{
	return (Texture*)textureTable.get(name);
}

TextureManager TextureManager::instance;
