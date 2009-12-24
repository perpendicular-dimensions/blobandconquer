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

Particle::Particle()
{
	type = PT_NORMAL;
	
	size = 1;
	
	bright = false;
	
	growth = 0;
}

Particle::~Particle()
{
}

void Particle::load(Properties *props)
{
	type = props->getInt("type", type);
	size = props->getFloat("size", size);
	color = props->getColor("color");
	life = props->getFloat("life", life);
	position = props->getVector("position", position);
	velocity = props->getVector("velocity", velocity);
	bright = props->getInt("bright", bright);
	growth = props->getFloat("growth", growth);
}

void Particle::save(FILE *fp)
{
	Properties props;
	
	props.setName("Particle");
	
	props.setProperty("type", type);
	props.setProperty("size", size);
	props.setProperty("color", color);
	props.setProperty("life", life);
	props.setProperty("position", position);
	props.setProperty("velocity", velocity);
	props.setProperty("bright", bright);
	props.setProperty("growth", growth);
	
	props.save(fp);
}
