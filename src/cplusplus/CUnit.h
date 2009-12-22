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

class Unit : public Entity {

	private:
		
		void loadAdditional(Properties *p);
		void saveAdditional(Properties *p);

	public:
		
		bool dead;
		bool spawnedIn;
		bool completedOrders;
		
		bool showHealthData;
		
		int specialAction;
		
		int liquidLevel;

		float shield, maxShield;
		
		float mental;
		float oxygen;
		
		float shieldHit;
		float reload;
		
		float lastDamageTimer;
		int totalCurrentDamage;
		
		WeaponDef *currentWeapon;
		Item *currentItem;
		
		String targetName;
		Entity *target;
		
		Switch *switchTarget;
		
		Vector lastTargetPosition;
		
		float helpless;
		
		float lastPlayerSighting;
		
		Crumb *currentCrumb;
		float crumbChaseTime;
		bool crumbLost;
		int crumbsConsumed;
		
		Vector gunPosition[2];

	Unit(long uniqueId);
	~Unit();
	
	bool canBeDamaged();
	void applyDamage(float amount, int damageType);
	void think(float td);
};
