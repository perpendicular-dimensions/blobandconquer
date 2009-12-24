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

Decoration::Decoration()
{
	entityType = ET_DECORATION;
	
	type = DT_FLESH;
	
	model = NULL;
	
	texture = NULL;
	
	slides = false;
	
	boundingBox.boxType = BOX_HOLLOW;
}

Decoration::~Decoration()
{

}

void Decoration::loadAdditional(Properties *props)
{
	type = props->getInt("type", type);
	size = props->getFloat("size", size);
	modelName = props->getString("modelName", modelName.getText());
	textureName = props->getString("textureName", textureName.getText());
	color = props->getColor("color");
	slides = props->getInt("slides", slides);
	
	triangle.point[0] = props->getVector("triangle1");
	triangle.point[1] = props->getVector("triangle2");
	triangle.point[2] = props->getVector("triangle3");
}

void Decoration::saveAdditional(Properties *props)
{
	props->setName("Decoration");
	
	props->setProperty("type", type);
	props->setProperty("size", size);
	props->setProperty("modelName", modelName.getText());
	props->setProperty("textureName", textureName.getText());
	props->setProperty("color", color);
	props->setProperty("slides", slides);
	
	props->setProperty("triangle1", triangle.point[0].toString());
	props->setProperty("triangle2", triangle.point[1].toString());
	props->setProperty("triangle3", triangle.point[2].toString());
}
