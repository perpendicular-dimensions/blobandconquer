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

TextureSpecManager::TextureSpecManager()
{
	textureSpecTable.name = "TextureSpec Table";
}

TextureSpecManager *TextureSpecManager::getInstance()
{
	return &instance;
}

TextureSpecManager::~TextureSpecManager()
{
	textureSpecTable.clear();
}

void TextureSpecManager::clear()
{
	textureSpecTable.clear();
}

void TextureSpecManager::clearTextures()
{
	TextureSpec *textureSpec;
	
	for (Reference *ref = (Reference*)textureSpecTable.toList()->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		textureSpec = (TextureSpec*)ref->object;
		
		debug(("TextureSpecManager::clearTextures() - Zeroing Texture %s\n", textureSpec->name.getText()));
		
		if (textureSpec->texture != 0)
		{
			glDeleteTextures(1, &textureSpec->texture);
		}
		
		textureSpec->texture = 0;
	}
}

void TextureSpecManager::addTextureSpec(const char *name, TextureSpec *TextureSpec)
{
	debug(("TextureSpecManager::addTextureSpec() - %s\n", name));

	textureSpecTable.put(name, TextureSpec);
}

TextureSpec *TextureSpecManager::createDefaultTextureSpec(const char *name)
{
	TextureSpec *textureSpec = new TextureSpec();
	
	textureSpec->name = name;
	
	addTextureSpec(name, textureSpec);
	
	return textureSpec;
}

TextureSpec *TextureSpecManager::getTextureSpec(const char *name)
{
	return (TextureSpec*)textureSpecTable.get(name);
}

void TextureSpecManager::resetTextureAnimation()
{
	TextureSpec *textureSpec;
	
	for (Reference *ref = (Reference*)textureSpecTable.toList()->getFirstElement() ; ref != NULL ; ref = (Reference*)ref->next)
	{
		textureSpec = (TextureSpec*)ref->object;
		
		textureSpec->animated = false;
	}
}

TextureSpecManager TextureSpecManager::instance;
