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

ParticleManager::ParticleManager()
{
	particleList.setName("ParticleList");
}

ParticleManager *ParticleManager::getInstance()
{
	return &instance;
}

ParticleManager::~ParticleManager()
{
}

Particle *ParticleManager::spawnParticle()
{
	Particle *particle = new Particle();
	
	particleList.add(particle);

	return particle;
}

void ParticleManager::save(FILE *fp)
{
	debug(("ParticleManager::save()\n"));
	
	for (Particle *particle = (Particle*)particleList.getFirstElement() ; particle != NULL ; particle = (Particle*)particle->next)
	{
		particle->save(fp);
	}
	
	debug(("ParticleManager::save() - Done\n"));
}

ParticleManager ParticleManager::instance;
