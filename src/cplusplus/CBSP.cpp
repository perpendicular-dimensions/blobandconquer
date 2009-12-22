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

BSP::BSP()
{
	strEntities = NULL;
	faceIndices = NULL;
	meshIndices = NULL;
	leafFaces = NULL;
	leafBrushes = NULL;
	textureSpec = NULL;
	textures = NULL;
	verts = NULL;
	faces = NULL;
	nodes = NULL;
	leafs = NULL;
	planes = NULL;
	brushes = NULL;
	brushSides = NULL;
	shaders = NULL;
	lightmapTextures = NULL;
	patches = NULL;
	
	traceLoops = 0;
}

BSP *BSP::getInstance()
{
	return &instance;
}

BSP::~BSP()
{
	clear();
}
	
void BSP::clear()
{
	if (strEntities != NULL)
	{
		free(strEntities);
	}
	
	if (faceIndices != NULL)
	{
		free(faceIndices);
	}
	
	if (meshIndices != NULL)
	{
		free(meshIndices);
	}
	
	if (leafFaces != NULL)
	{
		free(leafFaces);
	}
	
	if (leafBrushes != NULL)
	{
		free(leafBrushes);
	}
	
	if (textureSpec != NULL)
	{
		free(textureSpec);
	}
	
	if (textures != NULL)
	{
		free(textures);
	}
	
	if (verts != NULL)
	{
		free(verts);
	}
	
	if (faces != NULL)
	{
		free(faces);
	}
	
	if (nodes != NULL)
	{
		free(nodes);
	}
	
	if (leafs != NULL)
	{
		free(leafs);
	}
	
	if (planes != NULL)
	{
		free(planes);
	}
	
	if (brushes != NULL)
	{
		free(brushes);
	}
	
	if (brushSides != NULL)
	{
		free(brushSides);
	}
	
	if (shaders != NULL)
	{
		free(shaders);
	}
	
	if (clusters.pBitsets != NULL)
	{
		free(clusters.pBitsets);
	}
	
	if (facesDrawn.bits != NULL)
	{
		free(facesDrawn.bits);
	}
	
	if (lightmapTextures != NULL)
	{
		glDeleteTextures(numOfLightmaps, lightmapTextures);
		
		free(lightmapTextures);
	}
	
	glDeleteTextures(1, &whiteTexture);
	
	if (patches != NULL)
	{
		for (int i = 0 ; i < numOfPatches ; i++)
		{
			for (int j = 0 ; j < patches[i].numQuadPatches ; j++)
			{
				if (patches[i].quadPatches[j].trianglesPerRow != NULL)
				{
					free(patches[i].quadPatches[j].trianglesPerRow);
				}
				
				if (patches[i].quadPatches[j].verts != NULL)
				{
					free(patches[i].quadPatches[j].verts);
				}
				
				if (patches[i].quadPatches[j].indices != NULL)
				{
					free(patches[i].quadPatches[j].indices);
				}
				
				if (patches[i].quadPatches[j].rowIndexPointers != NULL)
				{
					free(patches[i].quadPatches[j].rowIndexPointers);
				}
			}
			
			if (patches[i].quadPatches != NULL)
			{
				free(patches[i].quadPatches);
			}
		}
		
		free(patches);
	}
	
	if (type1Faces != NULL)
	{
		for (int i=0;i<numOfFaces;i++)
		{
			if (type1Faces[i] != NULL)
			{
				free(type1Faces[i]);
			}
		}
		
		free(type1Faces);
	}
	
	if (models != NULL)
	{
		free(models);
	}
	
	strEntities = NULL;
	faceIndices = NULL;
	meshIndices = NULL;
	leafFaces = NULL;
	leafBrushes = NULL;
	textureSpec = NULL;
	textures = NULL;
	verts = NULL;
	faces = NULL;
	nodes = NULL;
	leafs = NULL;
	planes = NULL;
	brushes = NULL;
	brushSides = NULL;
	shaders = NULL;
	clusters.pBitsets = NULL;
	facesDrawn.bits = NULL;
	lightmapTextures = NULL;
	patches = NULL;
	type1Faces = NULL;
	models = NULL;
}

void BSP::traceRay(Vector inputStart, Vector inputEnd)
{
	tracer->traceType = RAY;
	
	tracer->traceExtents.set(0, 0, 0);

	trace(inputStart, inputEnd);
}

void BSP::traceBox(Vector inputStart, Vector inputEnd, Vector inputMins, Vector inputMaxs)
{
	if (inputMins.x == 0 && inputMins.y == 0 && inputMins.z == 0 && inputMaxs.x == 0 && inputMaxs.y == 0 && inputMaxs.z == 0)
	{
		traceRay(inputStart, inputEnd);
	}
	else
	{
		tracer->traceType      = BOX;
		tracer->traceMins      = inputMins;
		tracer->traceMaxs      = inputMaxs;
		tracer->traceExtents.x = -tracer->traceMins.x > tracer->traceMaxs.x ? -tracer->traceMins.x : tracer->traceMaxs.x;
		tracer->traceExtents.y = -tracer->traceMins.y > tracer->traceMaxs.y ? -tracer->traceMins.y : tracer->traceMaxs.y;
		tracer->traceExtents.z = -tracer->traceMins.z > tracer->traceMaxs.z ? -tracer->traceMins.z : tracer->traceMaxs.z;
		tracer->inputStart     = inputStart;

		trace(inputStart, inputEnd);
	}
}

void BSP::trace(Vector inputStart, Vector inputEnd)
{
	tracer->outputStartsOut = 1;
	tracer->outputAllSolid  = 0;
	tracer->outputFraction  = 1.0f;

	/* Walk the tree */

	checkNode(0, 0.0f, 1.0f, inputStart, inputEnd);

	tracer->outputEnd.x = inputStart.x + tracer->outputFraction * (inputEnd.x - inputStart.x);
	tracer->outputEnd.y = inputStart.y + tracer->outputFraction * (inputEnd.y - inputStart.y);
	tracer->outputEnd.z = inputStart.z + tracer->outputFraction * (inputEnd.z - inputStart.z);
}

void BSP::checkNode(int nodeIndex, float startFraction, float endFraction, Vector start, Vector end)
{
	int i, side;
	float fraction1, fraction2, middleFraction, inverseDistance;
	float startDistance, endDistance, offset;
	Vector middle;
	BSPLeaf *leaf;
	BSPBrush *brush;
	BSPNode *node;
	Plane *plane;

	if (nodeIndex < 0) /* Leaf */
	{
		leaf = &leafs[-(nodeIndex + 1)];

		for (i=0;i<leaf->numOfLeafBrushes;i++)
		{
			brush = &brushes[leafBrushes[leaf->leafBrush + i]];

			if (brush->numOfBrushSides > 0)
			{
				//if (textures[brush->textureID].contents & (CONTENTS_AIR|CONTENTS_SOLID|CONTENTS_LAVA|CONTENTS_WATER))
				{
					checkBrush(brush, start, end);
				}
			}
		}

		return;
	}

	/* Node */

	node  = &nodes[nodeIndex];
	plane = &planes[node->plane];

	startDistance = (dotProd(start, plane->normal)) - plane->d;
	endDistance   = (dotProd(end, plane->normal))   - plane->d;

	if (tracer->traceType == RAY)
	{
		offset = 0;
	}
	else if (tracer->traceType == SPHERE)
	{
		offset = tracer->traceRadius;
	}
	else if (tracer->traceType == BOX)
	{
		offset = (float)(fabs(tracer->traceExtents.x * plane->normal.x) + fabs(tracer->traceExtents.y * plane->normal.y) + fabs(tracer->traceExtents.z * plane->normal.z));
	}

	if (startDistance >= offset && endDistance >= offset) /* Check front node as both points are in front of plane */
	{
		checkNode(node->children[0], startFraction, endFraction, start, end);
	}
	else if (startDistance < -offset && endDistance < -offset) /* Check behind node as both points are in back of plane */
	{
		checkNode(node->children[1], startFraction, endFraction, start, end);
	}
	else
	{
		if (startDistance < endDistance) /* Back */
		{
			side = 1;
			inverseDistance = 1.0f / (startDistance - endDistance);
			fraction1 = (startDistance - offset + EPSILON) * inverseDistance;
			fraction2 = (startDistance + offset + EPSILON) * inverseDistance;
		}
		else if (endDistance < startDistance) /* Front */
		{
			side = 0;
			inverseDistance = 1.0f / (startDistance - endDistance);
			fraction1 = (startDistance + offset + EPSILON) * inverseDistance;
			fraction2 = (startDistance - offset - EPSILON) * inverseDistance;
		}
		else /* Just set to Front */
		{
			side = 0;
			fraction1 = 1.0f;
			fraction2 = 0.0f;
		}

		if (fraction1 < 0.0f)
		{
			fraction1 = 0.0f;
		}
		else if (fraction1 > 1.0f)
		{
			fraction1 = 1.0f;
		}

		if (fraction2 < 0.0f)
		{
			fraction2 = 0.0f;
		}
		else if (fraction2 > 1.0f)
		{
			fraction2 = 1.0f;
		}

		/* Calculate the middle point for the first side */

		middleFraction = startFraction + (endFraction - startFraction) * fraction1;

		middle.x = start.x + fraction1 * (end.x - start.x);
		middle.y = start.y + fraction1 * (end.y - start.y);
		middle.z = start.z + fraction1 * (end.z - start.z);

		/* Check the first side */

		checkNode(node->children[side], startFraction, middleFraction, start, end);

		/* Calculate the middle point for the second side */

		middleFraction = startFraction + (endFraction - startFraction) * fraction2;

		middle.x = start.x + fraction2 * (end.x - start.x);
		middle.y = start.y + fraction2 * (end.y - start.y);
		middle.z = start.z + fraction2 * (end.z - start.z);

		/* Check the second side */

		checkNode(node->children[1 - side], middleFraction, endFraction, start, end);
	}
}

void BSP::checkBrush(BSPBrush *brush, Vector inputStart, Vector inputEnd)
{
	int i, startsOut, endsOut, planeIndex;
	float startFraction, endFraction, startDistance, endDistance, fraction;
	float *d, lengthX, lengthY, lengthZ;
	Vector offset;
	BSPBrushSide *brushSide;
	Plane *plane;

	startFraction = -1.0f;
	endFraction   = 1.0f;
	startsOut     = 0;
	endsOut       = 0;
	
	d = (float *)malloc(brush->numOfBrushSides * sizeof(float));
	
	if (tracer->usingCamera == true && d != NULL)
	{			
		for (i=0;i<brush->numOfBrushSides;i++)
		{
			brushSide = &brushSides[brush->brushSide + i];
			plane     = &planes[brushSide->plane];
				
			d[i] = plane->d * plane->normal.x + plane->d * plane->normal.y + plane->d * plane->normal.z;
		}
		
		/*
		Items that the camera can nove through are:
		
		Pillars : lengthX and lengthY <= 24 and lengthZ <= 400
		Thin platforms : lengthZ <= 8 and lengthX or lengthY <= 64
		Boxes : all sides are equal
		*/
		
		if (brush->numOfBrushSides == 6)
		{
			lengthX = fabs(d[1] - d[0]);
			lengthY = fabs(d[3] - d[2]);
			lengthZ = fabs(d[5] - d[4]);
			
			// a tall, thin pillar
			if (lengthX <= 24 && lengthY <= 24)
			{
				return;
			}
			
			// a long, thin beam
			if (lengthZ <= 16 && (lengthX <= 64 || lengthY <= 64))
			{
				return;
			}
			
			// a thin platform
			if (lengthX <= 64 && lengthY <= 64 && lengthZ <= 8)
			{
				return;
			}
			
			// a box
			if (lengthX == lengthY && lengthY == lengthZ)
			{
				return;
			}
		}
	}
	
	if (d != NULL)
	{
		free(d);
	}

	for (i=0;i<brush->numOfBrushSides;i++)
	{
		brushSide  = &brushSides[brush->brushSide + i];
		plane      = &planes[brushSide->plane];

		if (tracer->traceType == RAY)
		{
			startDistance = (dotProd(inputStart, plane->normal)) - plane->d;
			endDistance   = (dotProd(inputEnd, plane->normal))   - plane->d;
		}
		else if (tracer->traceType == BOX)
		{
			offset.x = plane->normal.x < 0 ? tracer->traceMaxs.x : tracer->traceMins.x;
			offset.y = plane->normal.y < 0 ? tracer->traceMaxs.y : tracer->traceMins.y;
			offset.z = plane->normal.z < 0 ? tracer->traceMaxs.z : tracer->traceMins.z;

			startDistance = (inputStart.x + offset.x) * plane->normal.x + (inputStart.y + offset.y) * plane->normal.y + (inputStart.z + offset.z) * plane->normal.z - plane->d;
			endDistance   = (inputEnd.x   + offset.x) * plane->normal.x + (inputEnd.y   + offset.y) * plane->normal.y + (inputEnd.z   + offset.z) * plane->normal.z - plane->d;
		}

		if (startDistance > 0)
		{
			startsOut = 1;
		}

		if (endDistance > 0)
		{
			endsOut = 1;
		}

		/* Make sure the trace isn't completely on one side of the brush */

		if (startDistance > 0 && endDistance > 0) /* Both are in front of the plane, its outside of this brush */
		{
			return;
		}

		if (startDistance <= 0 && endDistance <= 0) /* Both are behind this plane, it will get clipped by another one */
		{
			continue;
		}

		if (startDistance > endDistance) /* Line is entering into the brush */
		{
			fraction = (startDistance - EPSILON) / (startDistance - endDistance);

			if (fraction > startFraction)
			{
				planeIndex    = brushSide->plane;
				startFraction = fraction;
			}
		}
		else /* Line is leaving the brush */
		{
			fraction = (startDistance + EPSILON) / (startDistance - endDistance);

			if (fraction < endFraction)
			{
				endFraction = fraction;
			}
		}
	}

	// All solid - Trace is trapped within a brush
	if ((!startsOut) && (!endsOut))
	{
		tracer->outputAllSolid = 1;
		tracer->outputContent  = textures[brush->textureID].contents;
		return;
	}
	
	// Trace moved from one brush in to another
	if (startsOut != endsOut)
	{
		if (startsOut)
		{
			tracer->outputContent = textures[brush->textureID].contents;
		}
		
		if (endsOut)
		{
			tracer->outputContent = CONTENTS_SOLID;
		}
	}

	if (startFraction < endFraction)
	{
		if (startFraction > -1 && startFraction < tracer->outputFraction)
		{
			if (startFraction < 0)
			{
				startFraction = 0;
			}

			if ((textures[brush->textureID].contents & CONTENTS_SOLID) || (tracer->treatAllAsSolid))
			{
				tracer->outputFraction = startFraction;
				tracer->outputNormal   = &planes[planeIndex].normal;
				tracer->outputContent  = textures[brush->textureID].contents;
				tracer->brush = brush;
			}
		}
	}
}

bool BSP::canSightTarget(Vector start, Vector end)
{
	traceRay(start, end);
	
	return (tracer->outputFraction == 1.0);
}

Vector BSP::getTraceLocation(Vector start, Vector end)
{
	traceRay(start, end);
	
	return tracer->outputEnd;
}

int BSP::getContentsAtPosition(Vector position)
{
	tracer->outputContent = CONTENTS_SOLID;
	
	traceRay(position, position);
	
	return tracer->outputContent;
}

BSP BSP::instance;
