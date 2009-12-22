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

#include "weather.h"

void doRain(Vector position, int radius, float amount)
{
	Particle *particle;
	
	GLColor rain = GLColor::grey.darker();
	
	for (int i = 0 ; i < amount ; i++)
	{
		particle = particleManager->spawnParticle();
		
		particle->position = position;
		
		particle->position.x += Math::rrand(-radius, radius);
		particle->position.y += Math::rrand(-radius, radius);
		particle->position.z += 100;
		
		if (particle->position.z > bsp->maxCords.z)
		{
			particle->position.z = bsp->maxCords.z - 1;
		}
		
		particle->type = PT_SPARK;
		particle->life = Math::rrand(150, 250);
		particle->size = 1;
		
		particle->velocity.z = Math::rrand(-15, -10);
		particle->velocity.z *= 0.2;
		
		particle->color = rain;
	}
}

void doLightning()
{
	//game->nextLightning = 0;
	//lightningTime = 0;
}

void doWeather()
{
	if (mission->weather == WEATHER_NORMAL)
	{
		return;
	}
	
	switch (mission->weather)
	{
		case WEATHER_RAIN:
			doRain(player->position, 500, 7 * engine->getTimeDifference(TD_LOGIC));
			break;
			
		case WEATHER_LIGHT_RAIN:
			doRain(player->position, 700, 1 * engine->getTimeDifference(TD_LOGIC));
			break;
			
		case WEATHER_STORM:
		case WEATHER_HEAVY_RAIN:
			doRain(player->position, 500, 20 * engine->getTimeDifference(TD_LOGIC));
			break;
			
		default:
			break;
	}
}
