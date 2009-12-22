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

class ModelManager {

	private:
		
		static ModelManager instance;
	
	ModelManager();
	
	public:
		
		Hashtable md2ModelTable;
		Hashtable simpleModelTable;
		Hashtable mdlModelTable;
	
	static ModelManager *getInstance();
	~ModelManager();
	
	void addSimpleModel(const char *name, SimpleModel *simpleModel);
	void removeSimpleModel(const char *name);
	SimpleModel *getSimpleModel(const char *name);
	
	void addMD2Model(const char *name, MD2Model *md2);
	void removeMD2Model(const char *name);
	MD2Model *getMD2Model(const char *name);
	
	void addMDLModel(const char *name, MDLModel *mdlModel);
	void removeMDLModel(const char *name);
	MDLModel *getMDLModel(const char *name);
	
	void clear();

};
