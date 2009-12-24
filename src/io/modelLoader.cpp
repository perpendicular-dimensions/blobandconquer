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

#include "modelLoader.h"

void createDisplayListFromSimpleModel(const char *filename, SimpleModel *model)
{
	if (!pak->unpack(filename, PAK::TEXT))
	{
		graphics->showErrorAndExit(ERR_FILE, filename);
	}
	
	char *token = strtok((char*)pak->dataBuffer, "\n");
	
	int numberOfPoints = atoi(token);
	
	if ((numberOfPoints % 3) != 0)
	{
		graphics->showErrorAndExit("ERROR - loadSimpleModel() - Incorrect format! Must be divisible by 3!!");
	}
	
	float x[3], y[3], z[3], u[3], v[3];
	
	model->data = glGenLists(1);
	
	glNewList(model->data, GL_COMPILE);
	{
		for (int i = 0 ; i < numberOfPoints ; i += 3)
		{
			for (int j = 0 ; j < 3 ; j++)
			{
				token = strtok(NULL, "\n");
				
				sscanf(token, "%f %f %f %f %f", &x[j], &y[j], &z[j], &u[j], &v[j]);
			}
			
			glBegin(GL_TRIANGLES);
			{
				glTexCoord2f(u[0], v[0]); glVertex3f(x[0], y[0], z[0]);
				glTexCoord2f(u[1], v[1]); glVertex3f(x[1], y[1], z[1]);
				glTexCoord2f(u[2], v[2]); glVertex3f(x[2], y[2], z[2]);
			}
			glEnd();
		}
	}
	glEndList();
	
	pak->freeAll();
}

void createDisplayListFromProperties(const char *filename, SimpleModel *model)
{
	List *list = loadProperties(filename, true);
	
	Vector bbMins, bbMaxs;
	Vector position, scale, size;
	float radius;
	GLColor color, rotation;
	String texture;
	
	model->data = glGenLists(1);
	
	//model->name = filename;
	
	glNewList(model->data, GL_COMPILE);
	{
		for (Properties *props = (Properties*)list->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
		{
			glPushMatrix();
			{
				position = props->getVector("position");
				scale = props->getVector("scale");
				color = props->getColor("color");
				rotation = props->getColor("rotate"); // we're using a GLColor here because it can hold four values ;)
				
				if (rotation.color[0] != 1)
				{
					glRotatef(rotation.color[0], rotation.color[1], rotation.color[2], rotation.color[3]);
				}
				
				if ((scale.x != 0 ) && (scale.y != 0) && (scale.z != 0))
				{
					glScalef(scale.x, scale.y, scale.z);
				}
				
				glTranslatef(position.x, position.y, position.z);
				
				glColor4fv(color.color);
				
				if (props->name == "Sphere")
				{
					radius = props->getFloat("radius", 1);
					
					gluSphere(graphics->quadric, radius, 16, 16);
				}
				else if (props->name == "Box")
				{
					size = props->getVector("size");
					
					drawBox(position, size.x, size.y, size.z, 1);
				}
				else if (props->name == "Cone")
				{
					gluCylinder(graphics->quadric, props->getFloat("bottomRadius", 0), props->getFloat("topRadius", 1), props->getFloat("height", 0), 12, 1);
				}
			}
			glPopMatrix();
		}
	}
	glEndList();
	
	if (list != NULL)
	{
		delete list;
	}
}

SimpleModel *loadSimpleModel(const char *filename, int type)
{
	debug(("loadSimpleModel() - %s\n", filename));
	
	SimpleModel *simpleModel = new SimpleModel();
	
	switch (type)
	{
		case MDL_RAW:
			createDisplayListFromSimpleModel(filename, simpleModel);
			break;
			
		case MDL_PRIMITIVE:
			createDisplayListFromProperties(filename, simpleModel);
			break;
	}
	
	return simpleModel;
}

MD2Model *loadMD2Model(const char *filename)
{
	debug(("loadMD2Model() - %s\n", filename));
	
	char name[255], lastName[255];
	unsigned char *buffer, *bufferIndex;
	int i, j, k, frame;
	tMd2AliasFrame *frameIndex;
	Object currentFrame;
	MD2 md2;
	Animation animation;
	tMd2Header *headerIndex;
	tMd2Skin *skinIndex;
	tMd2TexCoord *textureIndex;
	tMd2Face *triangleIndex;
	
	MD2Model *model = new MD2Model();

	if (!pak->unpack(filename, PAK::BINARY))
	{
		graphics->showErrorAndExit("Failed to open MD2: %s\n", filename);
	}

	buffer = pak->binaryBuffer;
	
	bufferIndex = buffer;

	headerIndex = (tMd2Header *)buffer;

	md2.m_Header = *headerIndex;

	md2.m_Header.version          = SDL_SwapLE32(md2.m_Header.version);
	md2.m_Header.magic            = SDL_SwapLE32(md2.m_Header.magic);
	md2.m_Header.skinWidth        = SDL_SwapLE32(md2.m_Header.skinWidth);
	md2.m_Header.skinHeight       = SDL_SwapLE32(md2.m_Header.skinHeight);
	md2.m_Header.frameSize        = SDL_SwapLE32(md2.m_Header.frameSize);
	md2.m_Header.numSkins         = SDL_SwapLE32(md2.m_Header.numSkins);
	md2.m_Header.numVertices      = SDL_SwapLE32(md2.m_Header.numVertices);
	md2.m_Header.numTexCoords     = SDL_SwapLE32(md2.m_Header.numTexCoords);
	md2.m_Header.numTriangles     = SDL_SwapLE32(md2.m_Header.numTriangles);
	md2.m_Header.numGlCommands    = SDL_SwapLE32(md2.m_Header.numGlCommands);
	md2.m_Header.numFrames        = SDL_SwapLE32(md2.m_Header.numFrames);
	md2.m_Header.offsetSkins      = SDL_SwapLE32(md2.m_Header.offsetSkins);
	md2.m_Header.offsetTexCoords  = SDL_SwapLE32(md2.m_Header.offsetTexCoords);
	md2.m_Header.offsetTriangles  = SDL_SwapLE32(md2.m_Header.offsetTriangles);
	md2.m_Header.offsetFrames     = SDL_SwapLE32(md2.m_Header.offsetFrames);
	md2.m_Header.offsetGlCommands = SDL_SwapLE32(md2.m_Header.offsetGlCommands);
	md2.m_Header.offsetEnd        = SDL_SwapLE32(md2.m_Header.offsetEnd);

	if (md2.m_Header.version != 8) /* MUST be version 8 for MD2 */
	{
		graphics->showErrorAndExit("Incorrect version of MD2, expected 8, found %d\n", md2.m_Header.version);
	}

	/* Allocate memory for the md2 */

	md2.m_pSkins     = (tMd2Skin *)malloc(sizeof(tMd2Skin) * md2.m_Header.numSkins);
	md2.m_pTexCoords = (tMd2TexCoord *)malloc(sizeof(tMd2TexCoord) * md2.m_Header.numTexCoords);
	md2.m_pTriangles = (tMd2Face *)malloc(sizeof(tMd2Face) * md2.m_Header.numTriangles);
	md2.m_pFrames    = (tMd2Frame *)malloc(sizeof(tMd2Frame) * md2.m_Header.numFrames);

	if (md2.m_pSkins == NULL || md2.m_pTexCoords == NULL || md2.m_pTriangles == NULL || md2.m_pFrames == NULL)
	{
		graphics->showErrorAndExit("Ran out of memory while loading md2: %s\n", filename);
	}

	/* Seek to and read the skin. Only the first one for the moment */

	buffer = bufferIndex;

	buffer += md2.m_Header.offsetSkins;

 	skinIndex = (tMd2Skin *)buffer;

	for (i=0;i<md2.m_Header.numSkins;i++)
	{
		skinIndex++;
	}

	/* Seek to the Texture Coordinates */

	buffer = bufferIndex;

	buffer += md2.m_Header.offsetTexCoords;

	textureIndex = (tMd2TexCoord *)buffer;

	for (i=0;i<md2.m_Header.numTexCoords;i++)
	{
		md2.m_pTexCoords[i] = *textureIndex;
		textureIndex++;
	}

	/* Seek to the Triangles */

	buffer = bufferIndex;

	buffer += md2.m_Header.offsetTriangles;

 	triangleIndex = (tMd2Face *)buffer;

	for (i=0;i<md2.m_Header.numTriangles;i++)
	{
		md2.m_pTriangles[i] = *triangleIndex;
		triangleIndex++;
	}

	/* Seek to the Frames */

	buffer = bufferIndex;

	buffer += md2.m_Header.offsetFrames;

 	frameIndex = (tMd2AliasFrame *)buffer;

	for (i=0;i<md2.m_Header.numFrames;i++)
	{
		md2.m_pFrames[i].pVertices = (tMd2Triangle *)malloc(sizeof(tMd2Triangle) * md2.m_Header.numVertices);

		if (md2.m_pFrames[i].pVertices == NULL)
		{
			graphics->showErrorAndExit("Ran out of memory while creating frames for md2: %s\n", filename);
		}

		/* Copy the name of the animation to the frame */

		strcpy(md2.m_pFrames[i].strName, frameIndex->name);

		/* Assign scale and translations. MD2 swap the Y and Z, so switch them back, and negate the Z */

		for (j=0;j<md2.m_Header.numVertices;j++)
		{
			md2.m_pFrames[i].pVertices[j].vertex[0] = SDL_SwapLE32(frameIndex->aliasVertices[j].vertex[0]) * SDL_SwapLE32(frameIndex->scale[0]) + SDL_SwapLE32(frameIndex->translate[0]);
			md2.m_pFrames[i].pVertices[j].vertex[2] = -1 * SDL_SwapLE32(frameIndex->aliasVertices[j].vertex[1]) * SDL_SwapLE32(frameIndex->scale[1]) - SDL_SwapLE32(frameIndex->translate[1]);
			md2.m_pFrames[i].pVertices[j].vertex[1] = SDL_SwapLE32(frameIndex->aliasVertices[j].vertex[2]) * SDL_SwapLE32(frameIndex->scale[2]) + SDL_SwapLE32(frameIndex->translate[2]);
		}

	 	buffer     = (unsigned char *)frameIndex;
 		buffer    += md2.m_Header.frameSize;
	 	frameIndex = (tMd2AliasFrame *)buffer;
	}

	model->frameCount = md2.m_Header.numFrames;

	strcpy(lastName, "");

	k = 0;

	/* Get all the animation frame names */

	for (i=0;i<model->frameCount;i++)
	{
		strcpy(name, md2.m_pFrames[i].strName);

		/* Extract the frame number from the animation frame name */

		for (j=strlen(name)-1;j>=0;j--)
		{
			if (!isdigit(name[j]))
			{
				name[j + 1] = '\0';
				break;
			}
		}

		if (strcmp(name, lastName) != 0)
		{
			strcpy(lastName, name);
			k++;
		}
	}

	/* Allocate the required memory for the animations */

	model->animation = (Animation *)malloc(sizeof(Animation) * k);

	if (model->animation == NULL)
	{
		graphics->showErrorAndExit("Ran out of memory while creating animations for md2: %s\n", filename);
	}

	model->animCount = k;

	/* Now go back and add the frames to the relevant animations */

	strcpy(lastName, "");

	k = 0;

	for (i=0;i<model->frameCount;i++)
	{
		strcpy(name, md2.m_pFrames[i].strName);

		/* Extract the frame number from the animation frame name */

		for (j=strlen(name)-1;j>=0;j--)
		{
			if (!isdigit(name[j]))
			{
				frame       = atoi(strrchr(name, name[j + 1]));
				name[j + 1] = '\0';
				break;
			}
		}

		/* Check if this frame name is not the same as the last name or it's the very last frame */

		if (strcmp(name, lastName) != 0 || i == model->frameCount - 1)
		{
			/* If this is not the very first frame or it's the very last frame, add it to the list */

			if (strlen(lastName) != 0 || i == model->frameCount - 1)
			{
				animation.end = i - 1;

				model->animation[k++] = animation;

				/* Set the new animation's start to i */

				strcpy(animation.name, name);

				animation.start = i;
			}

			/* Otherwise, create the animation */

			else
			{
				strcpy(animation.name, name);

				animation.start = i;
			}
		}

		/* Set the last name to the current name */

		strcpy(lastName, name);
	}

	/* Now place all the information into the model */

	model->frame = (Object *)malloc(sizeof(Object) * model->frameCount);

	for (i=0;i<model->frameCount;i++)
	{
		currentFrame.vertexCount       = md2.m_Header.numVertices;
		currentFrame.textureCoordCount = md2.m_Header.numTexCoords;
		currentFrame.faceCount         = md2.m_Header.numTriangles;

		currentFrame.verts = (Vector*)malloc(sizeof(Vector) * currentFrame.vertexCount);

		if (currentFrame.verts == NULL)
		{
			graphics->showErrorAndExit("Ran out of memory while creating model for %s\n", filename);
		}

		for (j=0;j<currentFrame.vertexCount;j++)
		{
			currentFrame.verts[j].x = SDL_SwapLE32(md2.m_pFrames[i].pVertices[j].vertex[0]);
			currentFrame.verts[j].y = SDL_SwapLE32(md2.m_pFrames[i].pVertices[j].vertex[1]);
			currentFrame.verts[j].z = SDL_SwapLE32(md2.m_pFrames[i].pVertices[j].vertex[2]);
		}

		free(md2.m_pFrames[i].pVertices);

		/* Only add the UV information to the first frame */

		if (i == 0)
		{
			currentFrame.textures = (Point *)malloc(sizeof(Point) * currentFrame.textureCoordCount);
			currentFrame.faces    = (MD2Face *)malloc(sizeof(MD2Face) * currentFrame.faceCount);

			if (currentFrame.textures == NULL || currentFrame.faces == NULL)
			{
				graphics->showErrorAndExit("Ran out of memory while creating model textures for %s\n", filename);
			}

			/* Assign the uv coordinates. Scale the u and v down from 0-256 to 0-1 and invert the v */

			for (j=0;j<currentFrame.textureCoordCount;j++)
			{
				currentFrame.textures[j].x =     SDL_SwapLE32(md2.m_pTexCoords[j].u) / SDL_SwapLE32((float)md2.m_Header.skinWidth);
				currentFrame.textures[j].y = 1 - SDL_SwapLE32(md2.m_pTexCoords[j].v) / SDL_SwapLE32((float)md2.m_Header.skinHeight);
			}

			/* Assign the face data */

			for (j=0;j<currentFrame.faceCount;j++)
			{
				/* Vertex indicies */

				currentFrame.faces[j].vertIndex[0] = SDL_SwapLE32(md2.m_pTriangles[j].vertexIndices[0]);
				currentFrame.faces[j].vertIndex[1] = SDL_SwapLE32(md2.m_pTriangles[j].vertexIndices[1]);
				currentFrame.faces[j].vertIndex[2] = SDL_SwapLE32(md2.m_pTriangles[j].vertexIndices[2]);

				/* Texture coordinates */

				currentFrame.faces[j].coordIndex[0] = SDL_SwapLE32(md2.m_pTriangles[j].textureIndices[0]);
				currentFrame.faces[j].coordIndex[1] = SDL_SwapLE32(md2.m_pTriangles[j].textureIndices[1]);
				currentFrame.faces[j].coordIndex[2] = SDL_SwapLE32(md2.m_pTriangles[j].textureIndices[2]);
			}
		}

		/* Add the current frame */

		model->frame[i] = currentFrame;
	}

	/* Free all the information */

	if (md2.m_pSkins != NULL)
	{
		free(md2.m_pSkins);
	}

	if (md2.m_pTexCoords != NULL)
	{
		free(md2.m_pTexCoords);
	}

	if (md2.m_pTriangles != NULL)
	{
		free(md2.m_pTriangles);
	}

	if (md2.m_pFrames != NULL)
	{
		free(md2.m_pFrames);
	}
	
	pak->freeAll();
	
	return model;
}
