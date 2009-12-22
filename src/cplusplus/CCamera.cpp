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

Camera::Camera()
{
	name = "Camera";
	
	entityType = ET_CAMERA;
	
	targetCamera = NULL;
	
	cluster = 0;
	
	shake = false;
	
	boundingBox.mins.set(-1, -1, -1);
	boundingBox.maxs.set(1, 1, 1);
	updateBoundingBox();
}

Camera::~Camera()
{
}

Camera *Camera::getInstance()
{
	return &instance;
}

Vector Camera::getRenderingTarget()
{
	if (targetCamera == NULL)
	{
		return position;
	}
	
	if (targetCamera->custom > 0)
	{
		return position;
	}
	
	return targetCamera->position;
}

void Camera::use()
{
	gluLookAt(
	position.x, position.y, position.z, 
	target.x, target.y, target.z, 
	0.0f, 0.0f, 1.0f);
	
	frustum.calculate();
}

int Camera::getSoundDistance(Vector position)
{
	float distance = Math::getDistance(this->position, position);
	
	distance /= 10;
	
	if (distance > 128)
	{
		return 0;
	}
	
	distance = 128 - distance;
	
	return (int)distance;
}

void Camera::loadAdditional(Properties *props){}
void Camera::saveAdditional(Properties *props){}

Camera Camera::instance;
