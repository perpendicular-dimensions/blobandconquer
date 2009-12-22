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

/* MD2 structures */

typedef unsigned char byte;

/* This stores a skin name */

typedef char tMd2Skin[64];

/* This holds the header information that is read in at the beginning of the file */

typedef struct tMd2Header
{
   int magic;                   /* This is used to identify the file */
   int version;                 /* The version number of the file (Must be 8) */
   int skinWidth;               /* The skin width in pixels */
   int skinHeight;              /* The skin height in pixels */
   int frameSize;               /* The size in bytes the frames are */
   int numSkins;                /* The number of skins associated with the model */
   int numVertices;             /* The number of vertices (constant for each frame) */
   int numTexCoords;            /* The number of texture coordinates */
   int numTriangles;            /* The number of faces (polygons) */
   int numGlCommands;           /* The number of gl commands */
   int numFrames;               /* The number of animation frames */
   int offsetSkins;             /* The offset in the file for the skin data */
   int offsetTexCoords;         /* The offset in the file for the texture data */
   int offsetTriangles;         /* The offset in the file for the face data */
   int offsetFrames;            /* The offset in the file for the frames data */
   int offsetGlCommands;        /* The offset in the file for the gl commands data */
   int offsetEnd;               /* The end of the file offset */
} tMd2Header;


/* This is used to store the vertices that are read in for the current frame */

typedef struct tMd2AliasTriangle
{
   byte vertex[3];
   byte lightNormalIndex;
} tMd2AliasTriangle;

/* This stores the normals and vertices for the frames */

typedef struct tMd2Triangle
{
   float vertex[3];
   float normal[3];
} tMd2Triangle;

/* This stores the indices into the vertex and texture coordinate arrays */

typedef struct tMd2Face
{
   short vertexIndices[3];
   short textureIndices[3];
} tMd2Face;

/* This stores UV coordinates */

typedef struct tMd2TexCoord
{
   short u, v;
} tMd2TexCoord;

/* This stores the animation scale, translation and name information for a frame, plus verts */

typedef struct tMd2AliasFrame
{
   float scale[3];
   float translate[3];
   char name[16];
   tMd2AliasTriangle aliasVertices[1];
} tMd2AliasFrame;

/* This stores the frames vertices after they have been transformed */

typedef struct tMd2Frame
{
   char strName[16];
   tMd2Triangle *pVertices;
} tMd2Frame;

/* This is the complete MD2 file format */

typedef struct MD2
{
    tMd2Header   m_Header;           /* The header data */
    tMd2Skin     *m_pSkins;          /* The skin data */
    tMd2TexCoord *m_pTexCoords;      /* The texture coordinates */
    tMd2Face     *m_pTriangles;      /* Face index information */
    tMd2Frame    *m_pFrames;         /* The frames of animation (vertices) */
} MD2;

/* Face information */

typedef struct MD2Face
{
	int vertIndex[3];
	int coordIndex[3];
} MD2Face;

/* This is a particular frame of the model */

typedef struct Object
{
	char name[255];        /* Object name */
	int vertexCount;       /* Number of vertices */
	int faceCount;         /* Number of faces */
	int textureCoordCount; /* Number of texture coordinates */
	int textureID;         /* Texture ID */
	Vector *verts;        /* Vertex information */
	Vector *normals;      /* Normals */
	Point *textures;     /* Texture coordinates */
	MD2Face *faces;           /* Faces */
} Object;

/* This is the texture information */

typedef struct Material
{
	char  name[255];	/* The texture name */
	char  file[255]; 	/* The texture file name (If this is set it's a texture map) */
	byte  color[3];     /* The color of the object (R, G, B) */
	int   id;     		/* the texture ID */
	float uTile;        /* u tiling of texture */
	float vTile;        /* v tiling of texture */
	float uOffset;      /* u offset of texture */
	float vOffset;      /* v offset of texture */
} Material;

/* Animation information */

typedef struct Animation
{
    char name[255];
    int start;
    int end;
} Animation;

// ------------------ The actual model --------------------

class MD2Model : public Linkable {

	public:
		
		int textureCount;
		int frameCount;
		int animCount;
		Material *texture;
		Object *frame;
		Animation *animation;
		
		GLuint *data;
		
	MD2Model();
	~MD2Model();

};
