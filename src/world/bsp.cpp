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

#include "bsp.h"

#define TESSELATION 10

float swapBytes(float f)
{
	unsigned int i;
	Convert c1, c2;
	
	c1.f = f;
	
	for (i=0;i<sizeof(float);i++)
	{
		c2.i[sizeof(float) - (i + 1)] = c1.i[i];
	}
	
	return c2.f;
}

TextureSpec *getTextureSpec(const char *basename)
{
	// ignore this texture
	if ((strcmp(basename, "textures/common/blank") == 0) || (strcmp(basename, "noshader") == 0) || (strcmp(basename, "textures/NULL") == 0) || (strcmp(basename, "textures/common/nodraw") == 0))
	{
		////printf("getTextureSpec() - Returning NULL (%s)\n", basename);
		return NULL;
	}
	
	debug(("getTextureSpec() - %s\n", basename));
	
	TextureSpec *textureSpec;
	Texture *texture;
	String filename;
	
	textureSpec = textureSpecManager->getTextureSpec(basename);
	
	if ((textureSpec != NULL) && (textureSpec->texture != 0))
	{
		debug(("getTextureSpec() - Found spec for %s\n", basename));
		return textureSpec;
	}	
	else if (textureSpec == NULL)
	{
		debug(("getTextureSpec() - Building default for %s\n", basename));
		textureSpec = textureSpecManager->createDefaultTextureSpec(basename);
	}
	
	filename.setText("%s.jpg", basename);
	
	if (pak->exists(filename.getText()))
	{
		debug(("getTextureSpec() - Loading Texture %s\n", filename.getText()));
		texture = graphics->loadTexture(filename.getText());
		textureManager->addTexture(filename.getText(), texture);
	}
	else
	{
		filename.setText("%s.tga", basename);
			
		if (pak->exists(filename.getText()))
		{
			debug(("getTextureSpec() - Loading Texture %s\n", filename.getText()));
			texture = graphics->loadTexture(filename.getText());
			textureManager->addTexture(filename.getText(), texture);
			
			textureSpec->blended = true;
			//textureSpec->blendSource = GL_SRC_ALPHA;
			//textureSpec->blendDest = GL_ONE_MINUS_SRC_ALPHA;
		}
		else
		{
			//graphics->showErrorAndExit("Could not load texture '%s' as JPG or TGA", filename.getText());
			printf("WARNING - Couldn't load texture '%s'. Using Metal instead!\n", basename);
			texture = textureManager->getTexture("Metal");
			texture = graphics->loadTexture("textures/mech/metal05.jpg");
			textureManager->addTexture("Metal", texture);
		}
	}
	
	textureSpec->texture = texture->data;
	
	return textureSpec;
}

int tesselate(BSPBiQuadraticPatch *quadPatch)
{
	int u, v, row, point;
	float px, py;
	BSPVertex temp[3];

	bsp->tesselation = TESSELATION;

	quadPatch->verts = (BSPVertex *)malloc((bsp->tesselation + 1) * (bsp->tesselation + 1) * sizeof(BSPVertex));

	if (quadPatch->verts == NULL)
	{
		graphics->showErrorAndExit("BSP - Failed to allocate %lu bytes for tesselation", (bsp->tesselation + 1) * (bsp->tesselation + 1) * sizeof(BSPVertex));
	}

	for (v=0;v<=bsp->tesselation;v++)
	{
		px = (float)v / bsp->tesselation;

		quadPatch->verts[v].position.x = quadPatch->controlPoints[0].position.x * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].position.x * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].position.x * (px * px);
		quadPatch->verts[v].position.y = quadPatch->controlPoints[0].position.y * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].position.y * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].position.y * (px * px);
		quadPatch->verts[v].position.z = quadPatch->controlPoints[0].position.z * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].position.z * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].position.z * (px * px);

		quadPatch->verts[v].textureCoord.x = quadPatch->controlPoints[0].textureCoord.x * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].textureCoord.x * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].textureCoord.x * (px * px);
		quadPatch->verts[v].textureCoord.y = quadPatch->controlPoints[0].textureCoord.y * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].textureCoord.y * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].textureCoord.y * (px * px);

		quadPatch->verts[v].lightmapCoord.x = quadPatch->controlPoints[0].lightmapCoord.x * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].lightmapCoord.x * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].lightmapCoord.x * (px * px);
		quadPatch->verts[v].lightmapCoord.y = quadPatch->controlPoints[0].lightmapCoord.y * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].lightmapCoord.y * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].lightmapCoord.y * (px * px);

		quadPatch->verts[v].normal.x = quadPatch->controlPoints[0].normal.x * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].normal.x * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].normal.x * (px * px);
		quadPatch->verts[v].normal.y = quadPatch->controlPoints[0].normal.y * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].normal.y * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].normal.y * (px * px);
		quadPatch->verts[v].normal.z = quadPatch->controlPoints[0].normal.z * ((1.0f - px) * (1.0f - px)) + quadPatch->controlPoints[3].normal.z * ((1.0f - px) * px * 2) + quadPatch->controlPoints[6].normal.z * (px * px);
	}

	for (u=1;u<=bsp->tesselation;u++)
	{
		py = (float)u / bsp->tesselation;

		temp[0].position.x = quadPatch->controlPoints[0].position.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].position.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].position.x * (py * py);
		temp[0].position.y = quadPatch->controlPoints[0].position.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].position.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].position.y * (py * py);
		temp[0].position.z = quadPatch->controlPoints[0].position.z * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].position.z * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].position.z * (py * py);

		temp[1].position.x = quadPatch->controlPoints[3].position.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].position.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].position.x * (py * py);
		temp[1].position.y = quadPatch->controlPoints[3].position.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].position.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].position.y * (py * py);
		temp[1].position.z = quadPatch->controlPoints[3].position.z * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].position.z * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].position.z * (py * py);

		temp[2].position.x = quadPatch->controlPoints[6].position.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].position.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].position.x * (py * py);
		temp[2].position.y = quadPatch->controlPoints[6].position.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].position.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].position.y * (py * py);
		temp[2].position.z = quadPatch->controlPoints[6].position.z * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].position.z * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].position.z * (py * py);

		temp[0].textureCoord.x = quadPatch->controlPoints[0].textureCoord.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].textureCoord.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].textureCoord.x * (py * py);
		temp[0].textureCoord.y = quadPatch->controlPoints[0].textureCoord.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].textureCoord.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].textureCoord.y * (py * py);

		temp[1].textureCoord.x = quadPatch->controlPoints[3].textureCoord.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].textureCoord.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].textureCoord.x * (py * py);
		temp[1].textureCoord.y = quadPatch->controlPoints[3].textureCoord.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].textureCoord.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].textureCoord.y * (py * py);

		temp[2].textureCoord.x = quadPatch->controlPoints[6].textureCoord.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].textureCoord.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].textureCoord.x * (py * py);
		temp[2].textureCoord.y = quadPatch->controlPoints[6].textureCoord.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].textureCoord.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].textureCoord.y * (py * py);

		temp[0].lightmapCoord.x = quadPatch->controlPoints[0].lightmapCoord.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].lightmapCoord.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].lightmapCoord.x * (py * py);
		temp[0].lightmapCoord.y = quadPatch->controlPoints[0].lightmapCoord.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].lightmapCoord.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].lightmapCoord.y * (py * py);

		temp[1].lightmapCoord.x = quadPatch->controlPoints[3].lightmapCoord.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].lightmapCoord.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].lightmapCoord.x * (py * py);
		temp[1].lightmapCoord.y = quadPatch->controlPoints[3].lightmapCoord.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].lightmapCoord.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].lightmapCoord.y * (py * py);

		temp[2].lightmapCoord.x = quadPatch->controlPoints[6].lightmapCoord.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].lightmapCoord.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].lightmapCoord.x * (py * py);
		temp[2].lightmapCoord.y = quadPatch->controlPoints[6].lightmapCoord.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].lightmapCoord.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].lightmapCoord.y * (py * py);

		temp[0].normal.x = quadPatch->controlPoints[0].normal.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].normal.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].normal.x * (py * py);
		temp[0].normal.y = quadPatch->controlPoints[0].normal.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].normal.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].normal.y * (py * py);
		temp[0].normal.z = quadPatch->controlPoints[0].normal.z * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[1].normal.z * ((1.0f - py) * py * 2) + quadPatch->controlPoints[2].normal.z * (py * py);

		temp[1].normal.x = quadPatch->controlPoints[3].normal.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].normal.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].normal.x * (py * py);
		temp[1].normal.y = quadPatch->controlPoints[3].normal.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].normal.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].normal.y * (py * py);
		temp[1].normal.z = quadPatch->controlPoints[3].normal.z * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[4].normal.z * ((1.0f - py) * py * 2) + quadPatch->controlPoints[5].normal.z * (py * py);

		temp[2].normal.x = quadPatch->controlPoints[6].normal.x * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].normal.x * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].normal.x * (py * py);
		temp[2].normal.y = quadPatch->controlPoints[6].normal.y * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].normal.y * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].normal.y * (py * py);
		temp[2].normal.z = quadPatch->controlPoints[6].normal.z * ((1.0f - py) * (1.0f - py)) + quadPatch->controlPoints[7].normal.z * ((1.0f - py) * py * 2) + quadPatch->controlPoints[8].normal.z * (py * py);

		for(v=0;v<=bsp->tesselation;v++)
		{
			px = (float)v / bsp->tesselation;

			quadPatch->verts[u * (bsp->tesselation + 1) + v].position.x = temp[0].position.x * ((1.0f - px) * (1.0f - px)) + temp[1].position.x * ((1.0f - px) * px * 2) + temp[2].position.x * (px * px);
			quadPatch->verts[u * (bsp->tesselation + 1) + v].position.y = temp[0].position.y * ((1.0f - px) * (1.0f - px)) + temp[1].position.y * ((1.0f - px) * px * 2) + temp[2].position.y * (px * px);
			quadPatch->verts[u * (bsp->tesselation + 1) + v].position.z = temp[0].position.z * ((1.0f - px) * (1.0f - px)) + temp[1].position.z * ((1.0f - px) * px * 2) + temp[2].position.z * (px * px);

			quadPatch->verts[u * (bsp->tesselation + 1) + v].textureCoord.x = temp[0].textureCoord.x * ((1.0f - px) * (1.0f - px)) + temp[1].textureCoord.x * ((1.0f - px) * px * 2) + temp[2].textureCoord.x * (px * px);
			quadPatch->verts[u * (bsp->tesselation + 1) + v].textureCoord.y = temp[0].textureCoord.y * ((1.0f - px) * (1.0f - px)) + temp[1].textureCoord.y * ((1.0f - px) * px * 2) + temp[2].textureCoord.y * (px * px);

			quadPatch->verts[u * (bsp->tesselation + 1) + v].lightmapCoord.x = temp[0].lightmapCoord.x * ((1.0f - px) * (1.0f - px)) + temp[1].lightmapCoord.x * ((1.0f - px) * px * 2) + temp[2].lightmapCoord.x * (px * px);
			quadPatch->verts[u * (bsp->tesselation + 1) + v].lightmapCoord.y = temp[0].lightmapCoord.y * ((1.0f - px) * (1.0f - px)) + temp[1].lightmapCoord.y * ((1.0f - px) * px * 2) + temp[2].lightmapCoord.y * (px * px);

			quadPatch->verts[u * (bsp->tesselation + 1) + v].normal.x = temp[0].normal.x * ((1.0f - px) * (1.0f - px)) + temp[1].normal.x * ((1.0f - px) * px * 2) + temp[2].normal.x * (px * px);
			quadPatch->verts[u * (bsp->tesselation + 1) + v].normal.y = temp[0].normal.y * ((1.0f - px) * (1.0f - px)) + temp[1].normal.y * ((1.0f - px) * px * 2) + temp[2].normal.y * (px * px);
			quadPatch->verts[u * (bsp->tesselation + 1) + v].normal.z = temp[0].normal.z * ((1.0f - px) * (1.0f - px)) + temp[1].normal.z * ((1.0f - px) * px * 2) + temp[2].normal.z * (px * px);
		}
	}

	quadPatch->indices = (unsigned int *)malloc(bsp->tesselation * (bsp->tesselation + 1) * 2 * sizeof(unsigned int));

	if (quadPatch->indices == NULL)
	{
		graphics->showErrorAndExit("BSP - Failed to allocate %lu bytes for indices", bsp->tesselation * (bsp->tesselation + 1) * 2 * sizeof(unsigned int));
	}

	for (row=0;row<bsp->tesselation;row++)
	{
		for (point=0;point<=bsp->tesselation;point++)
		{
			quadPatch->indices[(row * (bsp->tesselation + 1) + point) * 2 + 1] = row      * (bsp->tesselation + 1) + point;
			quadPatch->indices[(row * (bsp->tesselation + 1) + point) * 2]     =(row + 1) * (bsp->tesselation + 1) + point;
		}
	}

	quadPatch->trianglesPerRow = (int *)malloc(bsp->tesselation * sizeof(int));

	quadPatch->rowIndexPointers = (unsigned int **)malloc(bsp->tesselation * sizeof(unsigned int *));

	if (quadPatch->trianglesPerRow == NULL)
	{
		graphics->showErrorAndExit("BSP - Failed to allocate %lu bytes for trianglesPerRow", bsp->tesselation * sizeof(int));
	}

	if (quadPatch->rowIndexPointers == NULL)
	{
		graphics->showErrorAndExit("BSP - Failed to allocate %lu bytes for rowIndexPointers", bsp->tesselation * sizeof(unsigned int));
	}

	for (row=0;row<bsp->tesselation;row++)
	{
		quadPatch->trianglesPerRow[row]  = 2 * (bsp->tesselation + 1);
		quadPatch->rowIndexPointers[row] = &quadPatch->indices[row * 2 * (bsp->tesselation + 1)];
	}

	return 0;
}

int loadVertices(unsigned char *buffer, BSPLump *lumps)
{
	int i;

	bsp->numOfVerts = lumps[kVertices].length / sizeof(BSPVertex);

	bsp->verts = (BSPVertex *)malloc(bsp->numOfVerts * sizeof(BSPVertex));

	if (bsp->verts == NULL)
	{
		return 1;
	}

	buffer += lumps[kVertices].offset;
	
	memcpy(bsp->verts, buffer, sizeof(BSPVertex) * bsp->numOfVerts);
	
	bsp->minCords.set(RAND_MAX, RAND_MAX, RAND_MAX);
	bsp->maxCords.set(-RAND_MAX, -RAND_MAX, -RAND_MAX);

	for (i=0;i<bsp->numOfVerts;i++)
	{
		#if SDL_BYTEORDER != SDL_LIL_ENDIAN
			bsp->verts[i].position.x = swapBytes(bsp->verts[i].position.x);
			bsp->verts[i].position.y = swapBytes(bsp->verts[i].position.y);
			bsp->verts[i].position.z = swapBytes(bsp->verts[i].position.z);
			
			bsp->verts[i].textureCoord.x = swapBytes(bsp->verts[i].textureCoord.x);
			bsp->verts[i].textureCoord.y = swapBytes(bsp->verts[i].textureCoord.y);
			
			bsp->verts[i].lightmapCoord.x = swapBytes(bsp->verts[i].lightmapCoord.x);
			bsp->verts[i].lightmapCoord.y = swapBytes(bsp->verts[i].lightmapCoord.y);
			
			bsp->verts[i].normal.x = swapBytes(bsp->verts[i].normal.x);
			bsp->verts[i].normal.y = swapBytes(bsp->verts[i].normal.y);
			bsp->verts[i].normal.z = swapBytes(bsp->verts[i].normal.z);
			
			bsp->verts[i].color[0] = SDL_SwapLE32(bsp->verts[i].color[0]);
			bsp->verts[i].color[1] = SDL_SwapLE32(bsp->verts[i].color[1]);
			bsp->verts[i].color[2] = SDL_SwapLE32(bsp->verts[i].color[2]);
			bsp->verts[i].color[3] = SDL_SwapLE32(bsp->verts[i].color[3]);
		#endif

		bsp->minCords.x = min(bsp->verts[i].position.x, bsp->minCords.x);
		bsp->minCords.y = min(bsp->verts[i].position.y, bsp->minCords.y);
		bsp->minCords.z = min(bsp->verts[i].position.z, bsp->minCords.z);
		
		bsp->maxCords.x = max(bsp->verts[i].position.x, bsp->maxCords.x);
		bsp->maxCords.y = max(bsp->verts[i].position.y, bsp->maxCords.y);
		bsp->maxCords.z = max(bsp->verts[i].position.z, bsp->maxCords.z);
	}

	return 0;
}

int loadFaces(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfFaces = lumps[kFaces].length / sizeof(BSPFace);

	bsp->faces = (BSPFace *)malloc(bsp->numOfFaces * sizeof(BSPFace));

	if (bsp->faces == NULL)
	{
		return 1;
	}

	buffer += lumps[kFaces].offset;

	memcpy(bsp->faces, buffer, sizeof(BSPFace) * bsp->numOfFaces);
	
	for (i=0;i<bsp->numOfFaces;i++)
	{
		#if SDL_BYTEORDER != SDL_LIL_ENDIAN
			bsp->faces[i].textureID      = SDL_SwapLE32(bsp->faces[i].textureID);
			bsp->faces[i].effect         = SDL_SwapLE32(bsp->faces[i].effect);
			bsp->faces[i].type           = SDL_SwapLE32(bsp->faces[i].type);
			bsp->faces[i].startVertIndex = SDL_SwapLE32(bsp->faces[i].startVertIndex);
			bsp->faces[i].numOfVerts     = SDL_SwapLE32(bsp->faces[i].numOfVerts);
			bsp->faces[i].meshVertIndex  = SDL_SwapLE32(bsp->faces[i].meshVertIndex);
			bsp->faces[i].numMeshVerts   = SDL_SwapLE32(bsp->faces[i].numMeshVerts);
			bsp->faces[i].lightmapID     = SDL_SwapLE32(bsp->faces[i].lightmapID);
			bsp->faces[i].lMapCorner[0]  = SDL_SwapLE32(bsp->faces[i].lMapCorner[0]);
			bsp->faces[i].lMapCorner[1]  = SDL_SwapLE32(bsp->faces[i].lMapCorner[1]);
			bsp->faces[i].lMapSize[0]    = SDL_SwapLE32(bsp->faces[i].lMapSize[0]);
			bsp->faces[i].lMapSize[1]    = SDL_SwapLE32(bsp->faces[i].lMapSize[1]);
			bsp->faces[i].lMapPos.x      = swapBytes(bsp->faces[i].lMapPos.x);
			bsp->faces[i].lMapPos.y      = swapBytes(bsp->faces[i].lMapPos.y);
			bsp->faces[i].lMapPos.z      = swapBytes(bsp->faces[i].lMapPos.z);
			bsp->faces[i].lMapVecs[0].x  = swapBytes(bsp->faces[i].lMapVecs[0].x);
			bsp->faces[i].lMapVecs[0].y  = swapBytes(bsp->faces[i].lMapVecs[0].y);
			bsp->faces[i].lMapVecs[0].z  = swapBytes(bsp->faces[i].lMapVecs[0].z);
			bsp->faces[i].lMapVecs[1].x  = swapBytes(bsp->faces[i].lMapVecs[1].x);
			bsp->faces[i].lMapVecs[1].y  = swapBytes(bsp->faces[i].lMapVecs[1].y);
			bsp->faces[i].lMapVecs[1].z  = swapBytes(bsp->faces[i].lMapVecs[1].z);
			bsp->faces[i].vNormal.x      = swapBytes(bsp->faces[i].vNormal.x);
			bsp->faces[i].vNormal.y      = swapBytes(bsp->faces[i].vNormal.y);
			bsp->faces[i].vNormal.z      = swapBytes(bsp->faces[i].vNormal.z);
			bsp->faces[i].size[0]        = SDL_SwapLE32(bsp->faces[i].size[0]);
			bsp->faces[i].size[1]        = SDL_SwapLE32(bsp->faces[i].size[1]);
		#endif
	}

	return 0;
}

int loadMeshes(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfMeshIndices = lumps[kMeshVerts].length / sizeof(int);

	bsp->meshIndices = (int *)malloc(bsp->numOfMeshIndices * sizeof(int));

	if (bsp->meshIndices == NULL)
	{
		return 1;
	}

	buffer += lumps[kMeshVerts].offset;

	memcpy(bsp->meshIndices, buffer, sizeof(int) * bsp->numOfMeshIndices);
	
	for (i=0;i<bsp->numOfMeshIndices;i++)
	{
		bsp->meshIndices[i] = SDL_SwapLE32(bsp->meshIndices[i]);
	}

	return 0;
}

int loadTextures(unsigned char *buffer, BSPLump *lumps)
{
	bsp->numOfTextures = lumps[kTextures].length / sizeof(BSPTexture);

	bsp->textures = (BSPTexture *)malloc(bsp->numOfTextures * sizeof(BSPTexture));

	if (bsp->textures == NULL)
	{
		graphics->showErrorAndExit("BSP - Failed to allocate %lu bytes for textures", bsp->numOfTextures * sizeof(BSPTexture));
	}

	bsp->textureSpec = (TextureSpec **)malloc(bsp->numOfTextures * sizeof(TextureSpec *));

	if (bsp->textureSpec == NULL)
	{
		graphics->showErrorAndExit("BSP - Failed to allocate %lu bytes for texture validation", bsp->numOfTextures * sizeof(TextureSpec));
	}

	buffer += lumps[kTextures].offset;
	
	memcpy(bsp->textures, buffer, sizeof(BSPTexture) * bsp->numOfTextures);

	memset(bsp->textureSpec, 0, sizeof(TextureSpec *) * bsp->numOfTextures);

	for (int i = 0 ; i < bsp->numOfTextures ; i++)
	{
		bsp->textureSpec[i] = getTextureSpec(bsp->textures[i].strName);
		
		bsp->textures[i].flags    = SDL_SwapLE32(bsp->textures[i].flags);
		bsp->textures[i].contents = SDL_SwapLE32(bsp->textures[i].contents);
		
		if (bsp->textureSpec[i] != NULL)
		{
			//bsp->textures[i].contents = bsp->textureSpec[i]->contents;
		}
	}

	return 0;
}

int loadLightmaps(unsigned char *buffer, BSPLump *lumps)
{
	int i, j;
	float temp, gamma, r, g, b, scale, white[4];
	BSPLightmap *lightmaps;

	bsp->numOfLightmaps = lumps[kLightmaps].length / sizeof(BSPLightmap);

	debug(("BSP: %d lightmaps\n", bsp->numOfLightmaps));

	lightmaps = (BSPLightmap *)malloc(bsp->numOfLightmaps * sizeof(BSPLightmap));

	if (lightmaps == NULL)
	{
		return 1;
	}

	buffer += lumps[kLightmaps].offset;
	
	memcpy(lightmaps, buffer, sizeof(BSPLightmap) * bsp->numOfLightmaps);

	bsp->lightmapTextures = (unsigned int *)malloc(bsp->numOfLightmaps * sizeof(unsigned int));

	if (bsp->lightmapTextures == NULL)
	{
		return 1;
	}

	glGenTextures(bsp->numOfLightmaps, bsp->lightmapTextures);

	white[0] = 1.0f;
	white[1] = 1.0f;
	white[2] = 1.0f;
	white[3] = 1.0f;

	gamma = 2.5f;

	for (i=0;i<bsp->numOfLightmaps;i++)
	{
		for (j=0;j<128*128;j++)
		{
			scale = 1.0f;

			r = lightmaps[i].lightmapData[j * 3 + 0];
			g = lightmaps[i].lightmapData[j * 3 + 1];
			b = lightmaps[i].lightmapData[j * 3 + 2];

			r *= gamma / 255.0f;
			g *= gamma / 255.0f;
			b *= gamma / 255.0f;

			if (r > 1.0f && (temp = (1.0f / r)) < scale)
			{
				scale = temp;
			}

			if (g > 1.0f && (temp = (1.0f / g)) < scale)
			{
				scale = temp;
			}

			if (b > 1.0f && (temp = (1.0f / b)) < scale)
			{
				scale = temp;
			}

			scale *= 255.0f;
			r     *= scale;
			g     *= scale;
			b     *= scale;
			
			Math::limit(&r, 0, 128);
			Math::limit(&g, 0, 128);
			Math::limit(&b, 0, 128);

			lightmaps[i].lightmapData[j * 3 + 0] = (GLubyte)r;
			lightmaps[i].lightmapData[j * 3 + 1] = (GLubyte)g;
			lightmaps[i].lightmapData[j * 3 + 2] = (GLubyte)b;
		}
	}

	for (i=0;i<bsp->numOfLightmaps;i++)
	{
		glBindTexture(GL_TEXTURE_2D, bsp->lightmapTextures[i]);

		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, lightmaps[i].lightmapData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}


	glGenTextures(1, &bsp->whiteTexture);
	glBindTexture(GL_TEXTURE_2D, bsp->whiteTexture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, 1, 1, GL_RGB, GL_FLOAT, white);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	free(lightmaps);

	return 0;
}

int loadNodes(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfNodes = lumps[kNodes].length / sizeof(BSPNode);

	bsp->nodes = (BSPNode *)malloc(bsp->numOfNodes * sizeof(BSPNode));

	if (bsp->nodes == NULL)
	{
		return 1;
	}

	buffer += lumps[kNodes].offset;
	
	memcpy(bsp->nodes, buffer, sizeof(BSPNode) * bsp->numOfNodes);
	
	for (i=0;i<bsp->numOfNodes;i++)
	{
		bsp->nodes[i].plane       = SDL_SwapLE32(bsp->nodes[i].plane);
		bsp->nodes[i].children[0] = SDL_SwapLE32(bsp->nodes[i].children[0]);
		bsp->nodes[i].children[1] = SDL_SwapLE32(bsp->nodes[i].children[1]);
		bsp->nodes[i].min.x       = SDL_SwapLE32(bsp->nodes[i].min.x);
		bsp->nodes[i].min.y       = SDL_SwapLE32(bsp->nodes[i].min.y);
		bsp->nodes[i].min.z       = SDL_SwapLE32(bsp->nodes[i].min.z);
		bsp->nodes[i].max.x       = SDL_SwapLE32(bsp->nodes[i].max.x);
		bsp->nodes[i].max.y       = SDL_SwapLE32(bsp->nodes[i].max.y);
		bsp->nodes[i].max.z       = SDL_SwapLE32(bsp->nodes[i].max.z);
	}

	return 0;
}

int loadLeafs(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfLeafs = lumps[kLeafs].length / sizeof(BSPLeaf);

	bsp->leafs = (BSPLeaf *)malloc(bsp->numOfLeafs * sizeof(BSPLeaf));

	if (bsp->leafs == NULL)
	{
		return 1;
	}

	buffer += lumps[kLeafs].offset;
	
	memcpy(bsp->leafs, buffer, sizeof(BSPLeaf) * bsp->numOfLeafs);
	
	for (i=0;i<bsp->numOfLeafs;i++)
	{
		bsp->leafs[i].cluster          = SDL_SwapLE32(bsp->leafs[i].cluster);
		bsp->leafs[i].area             = SDL_SwapLE32(bsp->leafs[i].area);
		bsp->leafs[i].leafFace         = SDL_SwapLE32(bsp->leafs[i].leafFace);
		bsp->leafs[i].numOfLeafFaces   = SDL_SwapLE32(bsp->leafs[i].numOfLeafFaces);
		bsp->leafs[i].leafBrush        = SDL_SwapLE32(bsp->leafs[i].leafBrush);
		bsp->leafs[i].numOfLeafBrushes = SDL_SwapLE32(bsp->leafs[i].numOfLeafBrushes);
		bsp->leafs[i].min.x            = SDL_SwapLE32(bsp->leafs[i].min.x);
		bsp->leafs[i].min.y            = SDL_SwapLE32(bsp->leafs[i].min.y);
		bsp->leafs[i].min.z            = SDL_SwapLE32(bsp->leafs[i].min.z);
		bsp->leafs[i].max.x            = SDL_SwapLE32(bsp->leafs[i].max.x);
		bsp->leafs[i].max.y            = SDL_SwapLE32(bsp->leafs[i].max.y);
		bsp->leafs[i].max.z            = SDL_SwapLE32(bsp->leafs[i].max.z);
	}

	return 0;
}

int loadEntities(unsigned char *buffer, BSPLump *lumps)
{
	bsp->numOfEntityChars = lumps[kEntities].length / sizeof(char);

	bsp->strEntities = (char *)malloc(bsp->numOfEntityChars * sizeof(char));

	if (bsp->strEntities == NULL)
	{
		return 1;
	}

	buffer += lumps[kEntities].offset;
	
	memcpy(bsp->strEntities, buffer, sizeof(char) * bsp->numOfEntityChars);

	return 0;
}

int loadLeafFaces(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfLeafFaces = lumps[kLeafFaces].length / sizeof(int);

	bsp->leafFaces = (int *)malloc(bsp->numOfLeafFaces * sizeof(int));

	if (bsp->leafFaces == NULL)
	{
		return 1;
	}

	buffer += lumps[kLeafFaces].offset;
	
	memcpy(bsp->leafFaces, buffer, sizeof(int) * bsp->numOfLeafFaces);
	
	for (i=0;i<bsp->numOfLeafFaces;i++)
	{
		bsp->leafFaces[i] = SDL_SwapLE32(bsp->leafFaces[i]);
	}

	return 0;
}

int loadPlanes(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfPlanes = lumps[kPlanes].length / sizeof(Plane);

	bsp->planes = (Plane *)malloc(bsp->numOfPlanes * sizeof(Plane));

	if (bsp->planes == NULL)
	{
		return 1;
	}

	buffer += lumps[kPlanes].offset;
	
	memcpy(bsp->planes, buffer, sizeof(Plane) * bsp->numOfPlanes);
	
	for (i=0;i<bsp->numOfPlanes;i++)
	{
		#if SDL_BYTEORDER != SDL_LIL_ENDIAN
			bsp->planes[i].normal.x = swapBytes(bsp->planes[i].normal.x);
			bsp->planes[i].normal.y = swapBytes(bsp->planes[i].normal.y);
			bsp->planes[i].normal.z = swapBytes(bsp->planes[i].normal.z);
			bsp->planes[i].d        = swapBytes(bsp->planes[i].d);
		#endif
	}

	return 0;
}

int loadShaders(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfShaders = lumps[kShaders].length / sizeof(BSPShader);

	bsp->shaders = (BSPShader *)malloc(bsp->numOfShaders * sizeof(BSPShader));

	if (bsp->shaders == NULL)
	{
		return 1;
	}

	buffer += lumps[kShaders].offset;
	
	memcpy(bsp->shaders, buffer, sizeof(BSPShader) * bsp->numOfShaders);
	
	for (i=0;i<bsp->numOfShaders;i++)
	{
		bsp->shaders[i].brushID = SDL_SwapLE32(bsp->shaders[i].brushID);
		bsp->shaders[i].unknown = SDL_SwapLE32(bsp->shaders[i].unknown);
	}

	return 0;
}

int loadVisData(unsigned char *buffer, BSPLump *lumps)
{
	int size;

	buffer += lumps[kVisData].offset;

	if (lumps[kVisData].length > 0)
	{
		memcpy(&bsp->clusters.numOfClusters, buffer, sizeof(int));
		
		buffer += sizeof(int);
		
		bsp->clusters.numOfClusters = SDL_SwapLE32(bsp->clusters.numOfClusters);

		memcpy(&bsp->clusters.bytesPerCluster, buffer, sizeof(int));
		
		buffer += sizeof(int);
		
		bsp->clusters.bytesPerCluster = SDL_SwapLE32(bsp->clusters.bytesPerCluster);

		size = bsp->clusters.numOfClusters * bsp->clusters.bytesPerCluster;

		bsp->clusters.pBitsets = (unsigned char *)malloc(size * sizeof(unsigned char));

		if (bsp->clusters.pBitsets == NULL)
		{
			return 1;
		}

		memcpy(bsp->clusters.pBitsets, buffer, sizeof(unsigned char) * size);
	}

	else
	{
		bsp->clusters.pBitsets = NULL;
	}

	return 0;
}

int loadBrushes(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfBrushes = lumps[kBrushes].length / sizeof(BSPBrush);

	bsp->brushes = (BSPBrush *)malloc(bsp->numOfBrushes * sizeof(BSPBrush));

	if (bsp->brushes == NULL)
	{
		return 1;
	}

	buffer += lumps[kBrushes].offset;
	
	memcpy(bsp->brushes, buffer, sizeof(BSPBrush) * bsp->numOfBrushes);
	
	for (i=0;i<bsp->numOfBrushes;i++)
	{
		bsp->brushes[i].brushSide       = SDL_SwapLE32(bsp->brushes[i].brushSide);
		bsp->brushes[i].numOfBrushSides = SDL_SwapLE32(bsp->brushes[i].numOfBrushSides);
		bsp->brushes[i].textureID       = SDL_SwapLE32(bsp->brushes[i].textureID);
	}

	return 0;
}

int loadLeafBrushes(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfLeafBrushes = lumps[kLeafBrushes].length / sizeof(int);

	bsp->leafBrushes = (int *)malloc(bsp->numOfLeafBrushes * sizeof(int));

	if (bsp->leafBrushes == NULL)
	{
		return 1;
	}

	buffer += lumps[kLeafBrushes].offset;
	
	memcpy(bsp->leafBrushes, buffer, sizeof(int) * bsp->numOfLeafBrushes);
	
	for (i=0;i<bsp->numOfLeafBrushes;i++)
	{
		bsp->leafBrushes[i] = SDL_SwapLE32(bsp->leafBrushes[i]);
	}

	return 0;
}

int loadBrushSides(unsigned char *buffer, BSPLump *lumps)
{
	int i;
	
	bsp->numOfBrushSides = lumps[kBrushSides].length / sizeof(BSPBrushSide);

	bsp->brushSides = (BSPBrushSide *)malloc(bsp->numOfBrushSides * sizeof(BSPBrushSide));

	if (bsp->brushSides == NULL)
	{
		return 1;
	}

	buffer += lumps[kBrushSides].offset;
	
	memcpy(bsp->brushSides, buffer, sizeof(BSPBrushSide) * bsp->numOfBrushSides);
	
	for (i=0;i<bsp->numOfBrushSides;i++)
	{
		bsp->brushSides[i].plane     = SDL_SwapLE32(bsp->brushSides[i].plane);
		bsp->brushSides[i].textureID = SDL_SwapLE32(bsp->brushSides[i].textureID);
	}

	return 0;
}

int loadFaceDrawing(unsigned char *buffer, BSPLump *lumps)
{
	bsp->facesDrawn.size = (bsp->numOfFaces / 32) + 1;

	bsp->facesDrawn.bits = (unsigned int *)malloc(bsp->facesDrawn.size * sizeof(unsigned int));

	if (bsp->facesDrawn.bits == NULL)
	{
		return 1;
	}

	memset(bsp->facesDrawn.bits, 0, sizeof(unsigned int) * bsp->facesDrawn.size);

	return 0;
}

int loadPatches(unsigned char *buffer, BSPLump *lumps)
{
	int i, numPatchesWide, numPatchesHigh, x, y, row, point, patch;

	bsp->numOfPatches = 0;

	for (i=0;i<bsp->numOfFaces;i++)
	{
		if (bsp->faces[i].type == 2)
		{
			bsp->numOfPatches++;
		}
	}

	if (bsp->numOfPatches > 0)
	{
		patch = 0;

		bsp->patches = (BSPPatch *)malloc(bsp->numOfPatches * sizeof(BSPPatch));

		if (bsp->patches == NULL)
		{
			printf("Failed to allocate %lu bytes for patches\n", bsp->numOfPatches * sizeof(BSPPatch));
	
			exit(1);
		}

		for (i=0;i<bsp->numOfFaces;i++)
		{
			if (bsp->faces[i].type != 2)
			{
				continue;
			}

			bsp->patches[patch].textureID  = bsp->faces[i].textureID;
			bsp->patches[patch].lightmapID = bsp->faces[i].lightmapID;
			bsp->patches[patch].width      = bsp->faces[i].size[0];
			bsp->patches[patch].height     = bsp->faces[i].size[1];

			numPatchesWide = (bsp->patches[patch].width  - 1) / 2;
			numPatchesHigh = (bsp->patches[patch].height - 1) / 2;

			bsp->patches[patch].numQuadPatches = numPatchesWide * numPatchesHigh;

			bsp->patches[patch].quadPatches = (BSPBiQuadraticPatch *)malloc(bsp->patches[patch].numQuadPatches * sizeof(BSPBiQuadraticPatch));

			if (bsp->patches[patch].quadPatches == NULL)
			{
				return 1;
			}

			for (y=0;y<numPatchesHigh;y++)
			{
				for (x=0;x<numPatchesWide;x++)
				{
					for (row=0;row<3;row++)
					{
						for (point=0;point<3;point++)
						{
							bsp->patches[patch].quadPatches[y * numPatchesWide + x].controlPoints[row * 3 + point] = bsp->verts[bsp->faces[i].startVertIndex + (y * 2 * bsp->patches[patch].width + x * 2) + row * bsp->patches[patch].width + point];
						}
					}

					/* Tesselate the patch */

					if (tesselate(&bsp->patches[patch].quadPatches[y * numPatchesWide + x]) == 1)
					{
						return 1;
					}
				}
			}

			patch++;
		}
	}

	return 0;
}

int loadModels(unsigned char *buffer, BSPLump *lumps)
{
	int i, j, k, meshIndex, vertexIndex, offset, meshCount;
	float centerX, centerY, centerZ;

	bsp->numOfModels = lumps[kModels].length / sizeof(BSPModel);

	bsp->models = (BSPModel *)malloc(bsp->numOfModels * sizeof(BSPModel));

	if (bsp->models == NULL)
	{
		return 1;
	}
	
	String mdlName;
	MDLModel *model;

	buffer += lumps[kModels].offset;
	
	memcpy(bsp->models, buffer, sizeof(BSPModel) * bsp->numOfModels);

	for (i=1;i<bsp->numOfModels;i++)
	{
		#if SDL_BYTEORDER != SDL_LIL_ENDIAN
			bsp->models[i].mins.x       = swapBytes(bsp->models[i].mins.x);
			bsp->models[i].mins.y       = swapBytes(bsp->models[i].mins.y);
			bsp->models[i].mins.z       = swapBytes(bsp->models[i].mins.z);
			bsp->models[i].maxs.x       = swapBytes(bsp->models[i].maxs.x);
			bsp->models[i].maxs.y       = swapBytes(bsp->models[i].maxs.y);
			bsp->models[i].maxs.z       = swapBytes(bsp->models[i].maxs.z);
			bsp->models[i].faceIndex    = SDL_SwapLE32(bsp->models[i].faceIndex);
			bsp->models[i].numOfFaces   = SDL_SwapLE32(bsp->models[i].numOfFaces);
			bsp->models[i].brushIndex   = SDL_SwapLE32(bsp->models[i].brushIndex);
			bsp->models[i].numOfBrushes = SDL_SwapLE32(bsp->models[i].numOfBrushes);
		#endif
		
		centerX = ((bsp->models[i].maxs.x) + (bsp->models[i].mins.x)) / 2;
		centerY = ((bsp->models[i].maxs.y) + (bsp->models[i].mins.y)) / 2;
		centerZ = ((bsp->models[i].maxs.z) + (bsp->models[i].mins.z)) / 2;
		
		model = new MDLModel();
		
		mdlName.setText("*%d", i);
		
		modelManager->addMDLModel(mdlName.getText(), model);
		
		model->originalPosition.set(centerX, centerY, centerZ);
		model->mins = bsp->models[i].mins;
		model->maxs = bsp->models[i].maxs;
		
		model->numOfFaces = bsp->models[i].numOfFaces;
		
		model->numMeshVerts = (int *)malloc(bsp->models[i].numOfFaces * sizeof(int));
		
		model->lightmapID = (int *)malloc(bsp->models[i].numOfFaces * sizeof(int));
		
		model->textureID = (int *)malloc(bsp->models[i].numOfFaces * sizeof(int));
		
		model->indices = (int **)malloc(bsp->models[i].numOfFaces * sizeof(int *));
		
		model->position = (Vector **)malloc(bsp->models[i].numOfFaces * sizeof(Vector *));
		
		model->lightmapCoord = (Point **)malloc(bsp->models[i].numOfFaces * sizeof(Point *));
		
		model->textureCoord = (Point **)malloc(bsp->models[i].numOfFaces * sizeof(Point *));
		
		for (j=0;j<bsp->models[i].numOfFaces;j++)
		{			
			meshIndex   = bsp->faces[bsp->models[i].faceIndex + j].meshVertIndex;
			meshCount   = bsp->faces[bsp->models[i].faceIndex + j].numMeshVerts;
			vertexIndex = bsp->faces[bsp->models[i].faceIndex + j].startVertIndex;
			
			model->numMeshVerts[j] = meshCount;
			
			model->textureID[j]  = bsp->faces[bsp->models[i].faceIndex + j].textureID;
			model->lightmapID[j] = bsp->faces[bsp->models[i].faceIndex + j].lightmapID;
			
			model->position[j] = (Vector *)malloc(meshCount * sizeof(Vector));
			
			model->indices[j] = (int *)malloc(meshCount * sizeof(int));
			
			model->lightmapCoord[j] = (Point *)malloc(meshCount * sizeof(Point));
			
			model->textureCoord[j] = (Point *)malloc(meshCount * sizeof(Point));
			
			for (k=0;k<meshCount;k++)
			{	
				if (meshIndex + k > bsp->numOfMeshIndices)
				{
					return 1;
				}

				offset = bsp->meshIndices[meshIndex + k] + vertexIndex;

				if (offset > bsp->numOfVerts)
				{
					return 1;
				}
				
				model->position[j][k]      = bsp->verts[offset].position;
				model->textureCoord[j][k]  = bsp->verts[offset].textureCoord;
				model->lightmapCoord[j][k] = bsp->verts[offset].lightmapCoord;
				
				model->position[j][k].x -= centerX;
				model->position[j][k].y -= centerY;
				model->position[j][k].z -= centerZ;
				
				model->indices[j][k] = k;
			}
		}
	}
	
	return 0;
}

void removeBSPModelFaces()
{
	BSPModel *bspModel;
	
	for (int modelIndex = 1 ; modelIndex < bsp->numOfModels ; modelIndex++)
	{
		bspModel = &bsp->models[modelIndex];
		
		for (int i = 0 ; i < bspModel->numOfFaces ; i++)
		{
			bsp->faces[bspModel->faceIndex + i].textureID = -1;
		}
	}
}

void drawBSP(bool drawBlended)
{
	/* Set up texture units */

	/* Unit 0 */
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	/* Unit 0 */

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* Unit 1 */

	glActiveTextureARB(GL_TEXTURE1_ARB);

	glEnable(GL_TEXTURE_2D);

	/* Set up environment to draw textures, lightmaps etc. */

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);

	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 2.0f);

	/* Set back to unit 0 */

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	/* Loop through the face list and draw the associated vertex */
	
	for (int i=0;i<bsp->numOfFaces;i++)
	{
		if (bsp->faces[i].textureID == -1)
		{
			continue;
		}
		
		if (bsp->textureSpec[bsp->faces[i].textureID] == NULL)
		{
			continue;
		}
		
		if (bsp->textureSpec[bsp->faces[i].textureID]->blended != drawBlended)
		{
			continue;
		}
		
		if (bsp->clusters.pBitsets != NULL)
		{
			if ((bsp->facesDrawn.bits[i >> 5] & (1 << (i & 31))) == 0)
			{
				continue;
			}
		}

		switch (bsp->faces[i].type)
		{
			case 1:
				glVertexPointer(3, GL_FLOAT, sizeof(BSPVertex), &bsp->verts[0].position);

				glClientActiveTextureARB(GL_TEXTURE0_ARB);
				glTexCoordPointer(2, GL_FLOAT, sizeof(BSPVertex), &bsp->verts[0].textureCoord);

				glClientActiveTextureARB(GL_TEXTURE1_ARB);
				glTexCoordPointer(2, GL_FLOAT, sizeof(BSPVertex), &bsp->verts[0].lightmapCoord);

				glActiveTextureARB(GL_TEXTURE0_ARB);
				
				if (game->totalRemaining != 0)
				{
					if (!bsp->textureSpec[bsp->faces[i].textureID]->blended)
					{
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
						bsp->textureSpec[bsp->faces[i].textureID]->activate(engine->getTimeDifference(TD_ANIMATION));
						glColor4f(1.0, graphics->redPulse, graphics->redPulse, 1.0);
					}
					else
					{
						bsp->textureSpec[bsp->faces[i].textureID]->activate(engine->getTimeDifference(TD_ANIMATION));
					}
				}
				else
				{
					bsp->textureSpec[bsp->faces[i].textureID]->activate(engine->getTimeDifference(TD_ANIMATION));
				}

				glActiveTextureARB(GL_TEXTURE1_ARB);
				
				if (graphics->lightMaps)
				{
					if ((bsp->faces[i].lightmapID >= 0) && (!bsp->textureSpec[bsp->faces[i].textureID]->bright))
					{
						glBindTexture(GL_TEXTURE_2D, bsp->lightmapTextures[bsp->faces[i].lightmapID]);
					}
					else
					{
						glBindTexture(GL_TEXTURE_2D, bsp->whiteTexture);
					}
				}

				glActiveTextureARB(GL_TEXTURE0_ARB);

				glDrawElements(graphics->wireframe ? GL_LINES : GL_TRIANGLES, bsp->faces[i].numMeshVerts, GL_UNSIGNED_INT, &bsp->type1Faces[i][0]);
				glDisable(GL_BLEND);
				break;

			case 2:
				for (int j=0;j<bsp->patches[i].numQuadPatches;j++)
				{
					glVertexPointer(3, GL_FLOAT, sizeof(BSPVertex), &bsp->patches[i].quadPatches[j].verts[0].position);

					glClientActiveTextureARB(GL_TEXTURE0_ARB);
					glTexCoordPointer(2, GL_FLOAT, sizeof(BSPVertex), &bsp->patches[i].quadPatches[j].verts[0].textureCoord);

					glClientActiveTextureARB(GL_TEXTURE1_ARB);
					glTexCoordPointer(2, GL_FLOAT, sizeof(BSPVertex), &bsp->patches[i].quadPatches[j].verts[0].lightmapCoord);

					glActiveTextureARB(GL_TEXTURE0_ARB);
					bsp->textureSpec[bsp->patches[i].textureID]->activate(engine->getTimeDifference(TD_ANIMATION));

					glActiveTextureARB(GL_TEXTURE1_ARB);

					if (graphics->lightMaps)
					{
						if ((bsp->faces[i].lightmapID >= 0) && (!bsp->textureSpec[bsp->faces[i].textureID]->bright))
						{
							glBindTexture(GL_TEXTURE_2D, bsp->lightmapTextures[bsp->patches[i].lightmapID]);
						}
						else
						{
							glBindTexture(GL_TEXTURE_2D, bsp->whiteTexture);
						}
					}

					glActiveTextureARB(GL_TEXTURE0_ARB);

					for (int row=0;row<bsp->tesselation;row++)
					{
						glDrawElements(graphics->wireframe ? GL_LINE_STRIP : GL_TRIANGLE_STRIP, 2 * (bsp->tesselation + 1), GL_UNSIGNED_INT, &bsp->patches[i].quadPatches[j].indices[row * 2 * (bsp->tesselation + 1)]);
					}
				}
				break;

			case 3:
				int offset = bsp->faces[i].startVertIndex;

				glVertexPointer(3, GL_FLOAT, sizeof(BSPVertex), &bsp->verts[offset].position);

				glClientActiveTextureARB(GL_TEXTURE0_ARB);
				glTexCoordPointer(2, GL_FLOAT, sizeof(BSPVertex), &bsp->verts[offset].textureCoord);

				glClientActiveTextureARB(GL_TEXTURE1_ARB);
				glTexCoordPointer(2, GL_FLOAT, sizeof(BSPVertex), &bsp->verts[offset].lightmapCoord);

				glActiveTextureARB(GL_TEXTURE0_ARB);
				bsp->textureSpec[bsp->faces[i].textureID]->activate(engine->getTimeDifference(TD_ANIMATION));

				glActiveTextureARB(GL_TEXTURE1_ARB);

				if (graphics->lightMaps)
				{
					if ((bsp->faces[i].lightmapID >= 0) && (!bsp->textureSpec[bsp->faces[i].textureID]->bright))
					{
						glBindTexture(GL_TEXTURE_2D, bsp->lightmapTextures[bsp->faces[i].lightmapID]);
					}
					else
					{
						glBindTexture(GL_TEXTURE_2D, bsp->whiteTexture);
					}
				}

				glActiveTextureARB(GL_TEXTURE0_ARB);

				glDrawElements(graphics->wireframe ? GL_LINES : GL_TRIANGLES, bsp->faces[i].numMeshVerts, GL_UNSIGNED_INT, &bsp->meshIndices[bsp->faces[i].meshVertIndex]);
				break;
		}
		
		bsp->textureSpec[bsp->faces[i].textureID]->finish();
	}
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glPopAttrib();
}

int calculateEntityLeaf(Vector position)
{
	int currentNode = 0;
	
	while (currentNode >= 0)
	{
		if (camera->frustum.classifyPoint(bsp->planes[bsp->nodes[currentNode].plane], position) == POINT_IN_FRONT_OF_PLANE)
		{
			currentNode = bsp->nodes[currentNode].children[0];
		}
		else
		{
			currentNode = bsp->nodes[currentNode].children[1];
		}
	}
	
	return ~currentNode;
}

void getCluster(Entity *entity)
{
	int cluster = 0;

	int entityLeaf = calculateEntityLeaf(entity->position);
	
	cluster = bsp->leafs[entityLeaf].cluster;
	
	if (cluster != -1)
	{
		entity->cluster = cluster;
	}
}

int isClusterVisible(int cluster)
{
	int index;

	if (bsp->clusters.pBitsets == NULL || camera->cluster < 0)
	{
		return 1;
	}

	index = (camera->cluster * bsp->clusters.bytesPerCluster) + (cluster / 8);

	if (index > bsp->clusters.numOfClusters * bsp->clusters.bytesPerCluster)
	{
		printf("FATAL: Could not get BSP cluster %d. Max is %d\n", index, bsp->clusters.numOfClusters * bsp->clusters.bytesPerCluster);
		abort();
	}

	return bsp->clusters.pBitsets[index] & (1 << (cluster & 7));
}

void renderBSP()
{
	Vector position;
	int cameraLeaf, i, j, facesToDraw, faceIndex;

	memset(bsp->facesDrawn.bits, 0, sizeof(unsigned int) * bsp->facesDrawn.size);
	
	facesToDraw   = 0;
	cameraLeaf    = calculateEntityLeaf(camera->getRenderingTarget());
	camera->cluster = bsp->leafs[cameraLeaf].cluster;

	for (i=bsp->numOfLeafs - 1;i>=0;i--)
	{
		if (isClusterVisible(bsp->leafs[i].cluster) == 0)
		{
			continue;
		}
		
		position.set(bsp->leafs[i].min.x, bsp->leafs[i].min.y, bsp->leafs[i].min.z);
		position.x += bsp->leafs[i].max.x;
		position.y += bsp->leafs[i].max.y;
		position.z += bsp->leafs[i].max.z;
		
		for (j=bsp->leafs[i].numOfLeafFaces - 1;j>=0;j--)
		{
			faceIndex = bsp->leafFaces[bsp->leafs[i].leafFace + j];

			if ((bsp->facesDrawn.bits[faceIndex >> 5] & (1 << (faceIndex & 31))) == 0)
			{
				bsp->facesDrawn.bits[faceIndex >> 5] |= (1 << (faceIndex & 31));
			}
		}
	}
}

void loadBSP(const char *name)
{
	unsigned char *buffer, *bufferIndex;
	int i, j, vertexIndex, meshIndex, meshCount, offset;
	BSPHeader header;
	BSPLump lumps[kMaxLumps];
	
	debug(("loadBSP() - %s\n", name));

	if (!pak->unpack(name, PAK::BINARY))
	{
		graphics->showErrorAndExit("Failed to open BSP file %s", name);
	}
	
	buffer = new unsigned char[pak->getLastReadDataSize()];
	memcpy(buffer, pak->binaryBuffer, pak->getLastReadDataSize());
	
	bufferIndex = buffer;
	
	memcpy(&header, buffer, sizeof(BSPHeader));
	
	buffer += sizeof(BSPHeader);
	
	memcpy(&lumps, buffer, sizeof(BSPLump) * kMaxLumps);
	
	buffer = bufferIndex;
	
	header.version = SDL_SwapLE32(header.version);
	
	for (i=0;i<kMaxLumps;i++)
	{
		lumps[i].offset = SDL_SwapLE32(lumps[i].offset);
		lumps[i].length = SDL_SwapLE32(lumps[i].length);
	}

	if (loadVertices(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadFaces(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadMeshes(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadTextures(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadLightmaps(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadNodes(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadLeafs(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadEntities(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadLeafFaces(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadPlanes(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadShaders(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadVisData(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadBrushes(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadLeafBrushes(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadBrushSides(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadFaceDrawing(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	if (loadPatches(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;
	
	if (loadModels(buffer, lumps) == 1)
	{
		exit(1);
	}
	
	buffer = bufferIndex;

	/* Create a list of Type 1 faces for faster rendering */

	bsp->type1Faces = (int **)malloc(bsp->numOfFaces * sizeof(int *));

	if (bsp->type1Faces == NULL)
	{
		//printf("Failed to allocate %d bytes for Type 1 faces\n", bsp->numOfFaces * sizeof(int));

		exit(1);
	}
	
	for (i=0;i<bsp->numOfFaces;i++)
	{
		bsp->type1Faces[i] = NULL;
		
		if (bsp->faces[i].type != 1)
		{
			continue;
		}

		meshIndex   = bsp->faces[i].meshVertIndex;
		meshCount   = bsp->faces[i].numMeshVerts;
		vertexIndex = bsp->faces[i].startVertIndex;

		bsp->type1Faces[i] = (int *)malloc(meshCount * sizeof(int));

		if (bsp->type1Faces[i] == NULL)
		{
			//printf("Failed to allocate %d bytes for type1Faces[%d]\n", meshCount * sizeof(int), i);
			exit(1);
		}

		for (j=0;j<meshCount;j++)
		{
			if (meshIndex + j > bsp->numOfMeshIndices)
			{
				//printf("Could not get offset %d as max is %d\n", meshIndex + j, bsp->numOfMeshIndices);
				exit(1);
			}

			offset = bsp->meshIndices[meshIndex + j] + vertexIndex;

			if (offset > bsp->numOfVerts)
			{
				//printf("Could not draw vertex %d as max is %d\n", offset, bsp->numOfVerts);
				exit(1);
			}

			bsp->type1Faces[i][j] = offset;
		}
	}
	
	pak->freeAll();
	
	delete[] buffer;
	
	debug(("loadBSP() :: Done\n"));
}
