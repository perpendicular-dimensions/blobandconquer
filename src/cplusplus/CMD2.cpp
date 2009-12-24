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

MD2Model::MD2Model()
{
	data = NULL;
}

MD2Model::~MD2Model()
{
	int i;
	
	if (data != NULL)
	{
		delete[] data;
	}
	
	if (frame != NULL)
	{
		for (i=0;i<frameCount;i++)
		{
			if (frame[i].verts != NULL)
			{
				free(frame[i].verts);
			}
			
			if (i == 0 && frame[i].textures != NULL)
			{
				free(frame[i].textures);
			}
			
			if (i == 0 && frame[i].faces != NULL)
			{
				free(frame[i].faces);
			}
		}
		
		free(frame);
	}
	
	if (animation != NULL)
	{
		free(animation);
	}
}
