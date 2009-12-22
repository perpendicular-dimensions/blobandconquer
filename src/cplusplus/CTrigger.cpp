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

Trigger::Trigger()
{
	active = false;
	
	type = 0;

	action = 0;
	
	objectiveId = -1;
	
	timeLimit = 0;
	
	targetValue = 1;
	
	specialAction = NULL;
}

Trigger::~Trigger()
{
}

void Trigger::addLocationBox(const char *name, Vector position, Vector v)
{
	TriggerBoxEntry *entry = new TriggerBoxEntry();
	
	entry->name = name;
	entry->position = position;
	entry->boundingBox.mins -= v;
	entry->boundingBox.maxs += v;
	
	boundingBoxList.add(entry);
	
	debug(("addLocationBox() - Added '%s'\n", name));
}

void Trigger::clearLocationBoxes()
{
	boundingBoxList.clear();
}

void Trigger::assignTriggerLocation()
{
	for (TriggerBoxEntry *entry = (TriggerBoxEntry*)boundingBoxList.getFirstElement() ; entry != NULL ; entry = (TriggerBoxEntry*)entry->next)
	{
		if (entry->name == name.getText())
		{
			debug(("Trigger::assignTriggerLocation - Assigning box to trigger '%s'\n", name.getText()));
			
			boundingBox = entry->boundingBox;
			position = entry->position;
		}
	}
}

void Trigger::load(Properties *props)
{
	name = props->getString("name", name.getText());
	targetName = props->getString("targetName", targetName.getText());

	active = props->getInt("active", active);
	
	targetValue = props->getInt("targetValue", targetValue);
	
	timeLimit = props->getInt("timeLimit", timeLimit);
	
	objectiveId = props->getInt("objectiveId", objectiveId);

	position = props->getVector("position", position);
	boundingBox.mins = props->getVector("bbMins", boundingBox.mins);
	boundingBox.maxs = props->getVector("bbMaxs", boundingBox.maxs);
}

void Trigger::save(FILE *fp)
{
	Properties props;
	
	props.setName("Trigger");
	
	props.setProperty("name", name.getText());
	props.setProperty("targetName", targetName.getText());
	
	props.setProperty("active", active);
	props.setProperty("type", type);
	
	props.setProperty("targetValue", targetValue);
	
	props.setProperty("timeLimit", timeLimit);
	
	props.setProperty("action", action);
	
	props.setProperty("objectiveId", objectiveId);
	
	props.setProperty("position", position);
	props.setProperty("bbMins", boundingBox.mins);
	props.setProperty("bbMaxs", boundingBox.maxs);
	
	props.save(fp);
}

List Trigger::boundingBoxList;
