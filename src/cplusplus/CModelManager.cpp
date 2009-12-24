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

ModelManager::ModelManager()
{
	simpleModelTable.name = "Model Table";
	mdlModelTable.name = "MDL Table";
}

ModelManager *ModelManager::getInstance()
{
	return &instance;
}

ModelManager::~ModelManager()
{
}

void ModelManager::addSimpleModel(const char *name, SimpleModel *simpleModel)
{
	debug(("ModelManager::addSimpleModel() - %s\n", name));
	
	simpleModelTable.put(name, simpleModel);
}

void ModelManager::removeSimpleModel(const char *name)
{
	simpleModelTable.remove(name);
}

SimpleModel *ModelManager::getSimpleModel(const char *name)
{
	return (SimpleModel*)simpleModelTable.get(name);
}

void ModelManager::addMD2Model(const char *name, MD2Model *md2Model)
{
	debug(("ModelManager::addMD2Model() - %s\n", name));
	
	md2ModelTable.put(name, md2Model);
}

void ModelManager::removeMD2Model(const char *name)
{
	md2ModelTable.remove(name);
}

MD2Model *ModelManager::getMD2Model(const char *name)
{
	return (MD2Model*)md2ModelTable.get(name);
}

void ModelManager::addMDLModel(const char *name, MDLModel *mdlModel)
{
	debug(("ModelManager::addMDLModel() - %s\n", name));
	
	mdlModelTable.put(name, mdlModel);
}

void ModelManager::removeMDLModel(const char *name)
{
	mdlModelTable.remove(name);
}

MDLModel *ModelManager::getMDLModel(const char *name)
{
	return (MDLModel*)mdlModelTable.get(name);
}

void ModelManager::clear()
{
	simpleModelTable.clear();
	mdlModelTable.clear();
}

ModelManager ModelManager::instance;
