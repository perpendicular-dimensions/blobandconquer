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

class EntityManager {

	private:

		static EntityManager instance;

		unsigned int uniqueId;

	EntityManager();

	public:
		
		BlobDef blobDef[MAX_BLOBS];
		EnemyDef enemyDef[MAX_ENEMIES];
		FeatureDef featureDef[MAX_FEATURES];
		ItemDef itemDef[MAX_ITEMS];
		WeaponDef weaponDef[MAX_WEAPONS];
		BossDef bossDef[MAX_BOSSES];
		
		List blobList;
		List enemyList;
		List bossList;
		List itemList;
		List weaponList;
		List decorationList;
		List bulletList;
		List featureList;
		List structureList;
		List cameraList;
		Hashtable Q3TargetTable; // targets are unique
		List emitterList;
		List trapList;
		List decalList;
		
		List drawList; // entities to be drawn
		
		// mission target timer...
		float mtGlow;
		float mtDir;

	~EntityManager();

	static EntityManager *getInstance();

	void clear();
	void destroy();

	Unit *spawnBlob(int type);
	Unit *spawnEnemy(int type, int enemyLevel);
	Item *spawnItem(int type);
	Weapon *spawnWeapon(int type, bool fullClip);
	Decoration *spawnDecoration(int type);
	Bullet *spawnBullet(int type);
	Feature *spawnFeature(int type);
	Door *spawnDoor();
	Switch *spawnSwitch();
	Structure *spawnStructure(int type);
	Q3Target *spawnQ3Target(const char *name);
	CutsceneCamera *spawnCutsceneCamera(const char *name);
	Emitter *spawnEmitter(int type);
	Trap *spawnTrap(int type);
	Decal *spawnDecal();
	Boss *spawnBoss();
	
	void addEntityToDrawList(Entity *entity);
	
	Entity *getOwnerById(unsigned int uniqueId);
	Entity *getOwnerByName(const char *name);
	Item *getItemById(unsigned int uniqueId);
	Weapon *getWeaponById(unsigned int uniqueId);
	
	Entity *getAnyEntityByName(const char *name);
	Vector getEntityPosition(const char *name);
	
	int getEntityType(Properties *props);
	float getEntityStrength(Entity *entity);
	
	void load(Properties *props);
	void save(FILE *fp);
	void saveCustomItems(FILE *fp);
};
