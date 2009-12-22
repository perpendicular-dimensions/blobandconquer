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

/* "Alright, Dave?" */

class Trigger : public Linkable {
	
	private:
		
		class TriggerBoxEntry : public Linkable {
			
			public:
				
				String name;
				Vector position;
				BoundingBox boundingBox;
			
		};
		
		static List boundingBoxList;

	public:
		
		
		
		String name;
		String targetName;
		
		bool active;
		
		int timeLimit;
		
		int type;
		
		int targetValue;
		
		Vector position;
		
		int action;
		
		BoundingBox boundingBox;
		
		int objectiveId;
		
		void (*specialAction)(void);
		
	Trigger();
	~Trigger();
	
	static void assignLocationsToTriggers();
	static void addLocationBox(const char *name, Vector position, Vector v);
	static void clearLocationBoxes();
	
	void assignTriggerLocation();
	
	void load(Properties *props);
	void save(FILE *fp);
};
