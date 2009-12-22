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

TextureSpec::TextureSpec()
{
	sx = sy = 1;
	
	x = y = 0;
	
	dx = dy = 0;
	
	sdx = Math::rrand(-2, 2);
	sdy = Math::rrand(-2, 2);
	
	if (sdx == 0) sdx = 1;
	if (sdy == 0) sdy = -1;
	
	sdx /= 10000;
	sdy /= 10000;
	
	texture = 0;
	
	lightType = LT_NORMAL;
	lightLevel = 1.0;
	lightMinLevel = 0;
	lightMaxLevel = 1.0;
	lightSpeed = 0;
	lightDir = 1;
	
	animated = false;
	
	warp = false;
	
	bright = false;
	
	reflective = false;

	blended = false;
	blendSource = GL_SRC_ALPHA;
	blendDest = GL_ONE_MINUS_SRC_ALPHA;
	
	mx = TSM_NORMAL;
	my = TSM_NORMAL;
	
	environmentMap = false;
	
	contents = CONTENTS_SOLID;
		
	flags = 0;
}

TextureSpec::~TextureSpec()
{
}

void TextureSpec::load(Properties *props)
{
	lightSpeed = props->getFloat("lightSpeed", 0);
	lightLevel = props->getFloat("lightLevel", 1.0);
	lightMinLevel = props->getFloat("lightMinLevel", 0.25);
	lightMaxLevel = props->getFloat("lightMaxLevel", 2.0);
			
	dx = props->getFloat("dx", 0);
	dy = props->getFloat("dy", 0);
			
	warp = props->getInt("warp", 0);
			
	animated = props->getInt("animated", 0);
	environmentMap = props->getInt("environmentMap", 0);
			
	bright = props->getInt("bright", 0);
	reflective = props->getInt("reflective", 0);
			
	blended = props->getInt("blended", 0);
	blendSource = props->getInt("blendSource", GL_SRC_ALPHA);
	blendDest = props->getInt("blendDest", GL_ONE);
			
	flags = props->getInt("flags", 0);
			
	dx *= 0.0001;
	dy *= 0.0001;
}

void TextureSpec::activate(float td)
{
	glColor4f(lightLevel, lightLevel, lightLevel, lightLevel);
	
	if (blended)
	{
		glEnable(GL_BLEND);
		glBlendFunc(blendSource, blendDest);
		glDepthMask(GL_FALSE);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	
	if (environmentMap)
	{
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	}
	else
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}
	
	if (!animated)
	{
		x += (dx * td);
		y += (dy * td);
		
		if (lightType == LT_PULSE)
		{
			lightLevel += (lightSpeed * lightDir);
			
			if (lightLevel <= lightMinLevel)
			{
				lightDir = 1;
			}
			
			if (lightLevel >= lightMaxLevel)
			{
				lightDir = -1;
			}
		}
		
		animated = true;
		
		sx += (sdx * td);
		sy += (sdy * td);
		
		if ((sx <= 0.8) || (sx >= 1.2))
		{
			sdx = -sdx;
		}
		
		if ((sy <= 0.8) || (sy >= 1.2))
		{
			sdy = -sdy;
		}
	}
	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glPushMatrix();
	{
		switch (mx)
		{
			case TSM_NORMAL:
				glTranslatef(x, 0, 0);
				break;
			
			case TSM_SIN:
				glTranslatef(sin(x), 0, 0);
				break;
				
			case TSM_COS:
				glTranslatef(cos(x), 0, 0);
				break;
		}
		
		switch (my)
		{
			case TSM_NORMAL:
				glTranslatef(0, y, 0);
				break;
			
			case TSM_SIN:
				glTranslatef(0, cos(y), 0);
				break;
				
			case TSM_COS:
				glTranslatef(0, cos(y), 0);
				break;
		}
		
		if (warp)
		{
			glScalef(sx, sy, 0.0);
		}
	}
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glMatrixMode(GL_MODELVIEW);
}

void TextureSpec::finish()
{
	if (blended)
	{
		glDepthMask(GL_TRUE);
	}
	
	glPopMatrix(); // Modelview
	
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
}
