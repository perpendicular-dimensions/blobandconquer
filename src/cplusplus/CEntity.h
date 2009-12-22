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

class Entity : public Linkable {

	protected:

		unsigned int uniqueId;
		
		// can't be called directly
		virtual void loadAdditional(Properties *p) = 0;
		virtual void saveAdditional(Properties *p) = 0;

	public:

		String name;
		
		int entityType;
		
		int contentType;
		
		float health, maxHealth;
		
		int movementType;
		
		int currentAnimFrame;
		int nextAnimFrame;
		float animationTimer;
		Animation *currentAnim;
		
		float scale;
		
		Vector velocity;
		Vector position;
		Vector destination;
		Vector rotation;
		
		Vector oldPosition;
		Vector realVelocity;
		
		bool inCollisionCandidateList;
		BoundingBox boundingBox;
		
		// definitions
		EntityDef *definition;
		
		unsigned int ownerId;
		Entity *owner;
		String ownerName; // temporary storage
		
		// what the entity is standing on
		Entity *riding;
		
		float thinkTime;
		float custom;
		
		float elasticity;
		
		GLColor color;
		float heightAboveGround;
		
		bool referenced;
		
		long flags;
		
		String q3RadiantTargetName;
		String q3RadiantTarget;
		
		SDL_Rect occupiedRect;
		
		int cluster;
		
		Vector grapplingRopeVelocity;
		Vector grapplingRopePosition;
		
		void setOwner(Entity *owner);
		
		// Jan 09 break fix
		bool doNotSave;
		
		void (*action)(void);
		void (*die)(void);
		void (*walk)(void);
		void (*attack)(void);
		void (*touch)(Entity *other);
		void (*draw)(bool transparent);

	Entity();

	const unsigned int getUniqueId();
	void setName(const char *name);
	const char *getName();
	const char *getEntityTypeName();
	
	float getHealthPercent();
	
	bool performNextAction(float td);
	
	void applyGravity(float td);
	
	void prepareToMove(float td);
	void move(float td);
	
	void updateBoundingBox();
	
	void load(Properties *props);
	void save(FILE *fp);

};
