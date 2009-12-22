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
	
MDLModel::MDLModel()
{
	numMeshVerts = 0;
	
	numMeshVerts = NULL;
	indices = NULL;
	lightmapID = NULL;
	textureID = NULL;
}

MDLModel::~MDLModel()
{
	int i;
	
	if (numMeshVerts != NULL)
	{
		free(numMeshVerts);
	}
	
	if (lightmapID != NULL)
	{
		free(lightmapID);
	}
	
	if (textureID != NULL)
	{
		free(textureID);
	}
	
	if (position != NULL)
	{
		for (i=0;i<numOfFaces;i++)
		{
			if (position[i])
			{
				free(position[i]);
			}
		}
	}
	
	if (indices != NULL)
	{
		for (i=0;i<numOfFaces;i++)
		{
			if (indices[i])
			{
				free(indices[i]);
			}
		}
		
		free(indices);
	}
	
	if (lightmapCoord != NULL)
	{
		for (i=0;i<numOfFaces;i++)
		{
			if (lightmapCoord[i])
			{
				free(lightmapCoord[i]);
			}
		}
		
		free(lightmapCoord);
	}
	
	if (textureCoord != NULL)
	{
		for (i=0;i<numOfFaces;i++)
		{
			if (textureCoord[i])
			{
				free(textureCoord[i]);
			}
		}
		
		free(textureCoord);
	}
}
