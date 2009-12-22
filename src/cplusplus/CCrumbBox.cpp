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

CrumbBox::CrumbBox()
{
}

CrumbBox::~CrumbBox()
{
	clear();
}

void CrumbBox::clear()
{
	lastDropPosition.set(0, 0, 0);

	crumbs.clear();
}

void CrumbBox::addCrumb(Vector position, bool force)
{
	if ((!force) && (Math::getDistance(position, lastDropPosition) < 24))
	{
		return;
	}

	Crumb *crumb = new Crumb();
	
	crumb->age = 0;
	crumb->position = position;
	crumb->threshold = (force) ? 1 : 12;
	
	crumbs.add(crumb);
	
	if (crumbs.getSize() >= MAX_CRUMBS)
	{
		crumbs.remove(crumbs.getFirstElement());
	}
	
	lastDropPosition = position;
	
	//debug(("Dropped Breadcrumb [total: %d]\n", crumbs.getSize()));
}

void CrumbBox::ageCrumbs(float td)
{
	for (Crumb *crumb = (Crumb*)crumbs.getFirstElement() ; crumb != NULL ; crumb = (Crumb*)crumb->next)
	{
		crumb->age += td;
		
		// expire after 5 minutes...
		if (crumb->age > 30000)
		{
			crumb = (Crumb*)crumb->previous;
			crumbs.remove(crumb->next);
		}
	}
}

Crumb *CrumbBox::findYoungestCrumb(Vector position)
{
	// trap the crumbs that we want to
	// consider within our radius
	Crumb *candidate[MAX_CRUMBS];
	int c = 0;
	float distance = 0;
	float z = 0;
	
	memset(candidate, 0, sizeof (Crumb*) * MAX_CRUMBS);
	
	for (Crumb *crumb = (Crumb*)crumbs.getFirstElement() ; crumb != NULL ; crumb = (Crumb*)crumb->next)
	{
		distance = Math::getDistance(position.x, position.y, crumb->position.x, crumb->position.y);
		
		if (distance >= 256)
		{
			continue;
		}
		
		z = position.z - crumb->position.z;
		
		if ((z < -32 ) || (z > 12))
		{
			continue;
		}
		
		candidate[c++] = crumb;
	}
	
	// from our list of candidates, pull out the
	// youngest one and return that
	float youngest = RAND_MAX;
	Crumb *closest = NULL;
	
	for (int i = 0 ; i < MAX_CRUMBS ; i++)
	{
		if (candidate[i] == NULL)
		{
			return closest;
		}
		
		if (candidate[i]->age < youngest)
		{
			closest = candidate[i];
			youngest = candidate[i]->age;
		}
	}

	return closest;
}

void CrumbBox::load(Properties *props)
{
	Crumb *crumb = new Crumb();
	crumbs.add(crumb);
	crumb->load(props);
}

void CrumbBox::save(FILE *fp)
{
	for (Crumb *crumb = (Crumb*)crumbs.getFirstElement() ; crumb != NULL ; crumb = (Crumb*)crumb->next)
	{
		crumb->save(fp);
	}
}
