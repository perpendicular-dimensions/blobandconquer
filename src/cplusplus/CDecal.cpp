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

Decal::Decal()
{
	remove = false;
	alternativeBlend = false;
	type = 0;
	health = 150;
	texture = NULL;
	cluster = -1;
	
	x1 = y1 = x2 = y2 = 0;
	
	uv1 = 0;
	uv2 = 0;
	uv3 = 1;
	uv4 = 1;
}

Decal::~Decal()
{
}

void Decal::rotate(Vector normal)
{
	rotation.x = toDegrees(asin(normal.x));
	rotation.y = toDegrees(-asin(normal.y));
	rotation.z = toDegrees(asin(normal.z));
}

void Decal::load(Properties *props)
{
	alternativeBlend = props->getInt("alternativeBlend", alternativeBlend);
	type = props->getInt("type", type);
	health = props->getFloat("health", health);
	color = props->getColor("color");
	position = props->getVector("position", position);
	rotation = props->getVector("rotation", rotation);
	cluster = props->getInt("cluster", cluster);
	x1 = props->getFloat("x1", x1);
	y1 = props->getFloat("y1", y1);
	x2 = props->getFloat("x2", x2);
	y2 = props->getFloat("y2", y2);
	uv1 = props->getFloat("uv1", uv1);
	uv2 = props->getFloat("uv2", uv2);
	uv3 = props->getFloat("uv3", uv3);
	uv4 = props->getFloat("uv4", uv4);
}

void Decal::save(FILE *fp)
{
	Properties props;
	
	props.setName("Decal");
	
	props.setProperty("type", type);
	props.setProperty("alternativeBlend", alternativeBlend);
	props.setProperty("health", health);
	props.setProperty("color", color);
	props.setProperty("position", position);
	props.setProperty("rotation", rotation);
	props.setProperty("cluster", cluster);
	props.setProperty("x1", x1);
	props.setProperty("y1", y1);
	props.setProperty("x2", x2);
	props.setProperty("y2", y2);
	props.setProperty("uv1", uv1);
	props.setProperty("uv2", uv2);
	props.setProperty("uv3", uv3);
	props.setProperty("uv4", uv4);
	
	props.save(fp);
}
