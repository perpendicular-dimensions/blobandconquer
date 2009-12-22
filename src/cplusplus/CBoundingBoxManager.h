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


class BoundingBoxManager {

	private:
		
		static BoundingBoxManager instance;
		
		SDL_Rect occupiedRect;
		
		int currentCandidate;
		float cellSize;
		float minX, minY;
		List cell[GRID_SIZE][GRID_SIZE];
		
		Vector stationary;
	
	BoundingBoxManager();
	
	int initCollisionCandidates();
	
	void getRequiredGridPoints(Entity *entity, Vector velocity);
	void getRequiredGridPoints(Entity *entity1, Entity *entity2);
	void getRequiredGridPoints(Vector position1, Vector position2);
		
	public:
	
		Entity *candidate[MAX_CANDIDATES];
		Entity *intersectedEntity;
		double rayPercentage;
	
	~BoundingBoxManager();
	
	static BoundingBoxManager *getInstance();
	
	void clear();
	
	void construct(Vector mins, Vector maxs);
		
	void addBox(Entity *entity);
	void removeBox(Entity *entity);
	
	int initCollisionCandidates(Entity *entity1);
	int initCollisionCandidates(Entity *entity1, Entity *entity2);
	int initCollisionCandidates(Vector position1, Vector position2);
	void clearCollisionCandidates();
	
	bool lineIntersectBox(Entity *e, Vector start, Vector end);
	
	bool isValidSightTarget(int entityType);
	bool canSightTarget(Entity *observer, Entity *target);
	bool canSightSolid(Entity *observer, Vector startPosition, Vector target);
	Vector getTraceLocation(Entity *observer, Vector startPosition, Vector target);
	
	void dump();
};
