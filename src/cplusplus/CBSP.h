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

extern Trace *tracer;

enum lumps
{
	kEntities = 0,
	kTextures,
	kPlanes,
	kNodes,
	kLeafs,
	kLeafFaces,
	kLeafBrushes,
	kModels,
	kBrushes,
	kBrushSides,
	kVertices,
	kMeshVerts,
	kShaders,
	kFaces,
	kLightmaps,
	kLightVolumes,
	kVisData,
	kMaxLumps
};

struct BSPHeader {
	
	char strID[4];
	int version;
};

struct BSPLump {
	
	int offset, length;
};

struct BSPVertex {
	
	Vector position;
	Point textureCoord;
	Point lightmapCoord;
	Vector normal;
	unsigned char color[4];
};

struct BSPFace {
	
	int textureID, effect, type;
	int startVertIndex, numOfVerts, meshVertIndex;
	int numMeshVerts, lightmapID;
	int lMapCorner[2];
	int lMapSize[2];
	Vector lMapPos;
	Vector lMapVecs[2];
	Vector vNormal;
	int size[2];
};

struct BSPTexture {
	
	char strName[64];
	int flags, contents;
};

struct BSPLightmap {
	
	unsigned char lightmapData[128*128*3];
};

struct BSPNode {
	
	int plane;
	int children[2];
	Vectori min;
	Vectori max;
};

struct BSPLeaf {
	
	int cluster, area;
	Vectori min, max;
	int leafFace, numOfLeafFaces;
	int leafBrush, numOfLeafBrushes;
};

struct BSPBrush {
	
	int brushSide, numOfBrushSides, textureID;
};

struct BSPBrushSide {
	
	int plane, textureID;
};

struct BSPShader {
	
	char strName[64];
	int brushID, unknown;
};

struct BSPVisData {
	
	int numOfClusters, bytesPerCluster;
	unsigned char *pBitsets;
};

struct Bitset {
	
	unsigned int *bits;
	int size;
};

struct BSPBiQuadraticPatch {
	
	int *trianglesPerRow;
	unsigned int **rowIndexPointers, *indices;
	BSPVertex controlPoints[9], *verts;
};

struct BSPPatch {
	
	int textureID, lightmapID;
	int width, height, numQuadPatches;
	BSPBiQuadraticPatch *quadPatches;
};

typedef struct BSPModel {
	Vector mins, maxs;
	int faceIndex, numOfFaces;
	int brushIndex, numOfBrushes;
} BSPModel;

typedef union {
	unsigned char i[sizeof(float)];
	float f;
} Convert;

class BSP {
	
	private:
		
		static BSP instance;
		
		void checkNode(int nodeIndex, float startFraction, float endFraction, Vector start, Vector end);
		void checkBrush(BSPBrush *brush, Vector inputStart, Vector inputEnd);
		
		Vector entityStart, entityEnd;
		
	BSP();
	
	public:
		
		int traceLoops;
		
		char *strEntities;
		int numOfVerts, numOfFaces, numOfTextures, numOfPatches;
		int numOfLightmaps, numOfNodes, numOfLeafs, numOfModels;
		int numOfLeafFaces, numOfPlanes, numOfEntityChars;
		int numOfBrushes, numOfLeafBrushes, numOfBrushSides;
		int numOfShaders, numOfMeshIndices, tesselation;
		int *faceIndices;
		int *meshIndices;
		int *leafFaces;
		int *leafBrushes;
		int **type1Faces;
		TextureSpec **textureSpec;
		unsigned int whiteTexture, *lightmapTextures;
	
		BSPTexture *textures;
		BSPVertex *verts;
		BSPFace *faces;
		BSPNode *nodes;
		BSPLeaf *leafs;
		Plane *planes;
		BSPBrush *brushes;
		BSPBrushSide *brushSides;
		BSPPatch *patches;
		BSPShader *shaders;
		BSPVisData clusters;
		Bitset facesDrawn;
		BSPModel *models;
		
		Vector minCords, maxCords;
		
	~BSP();
	
	static BSP *getInstance();
	
	void clear();
	
	void traceRay(Vector inputStart, Vector inputEnd);
	void traceBox(Vector inputStart, Vector inputEnd, Vector inputMins, Vector inputMaxs);
	void trace(Vector inputStart, Vector inputEnd);
	
	Vector getTraceLocation(Vector start, Vector end);
	int getContentsAtPosition(Vector position);
	
	bool canSightTarget(Vector start, Vector end);
};
