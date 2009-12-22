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

Objective::Objective()
{
	id = 0;
	
	active = true;
	
	targetValue = 1;
	
	currentValue = 0;
}

Objective::~Objective()
{
}

const char *Objective::getStatus()
{
	switch (status)
	{
		case OBJ_INCOMPLETE:
			return _("Incomplete");
			
		case OBJ_INPROGRESS:
			return _("In Progress");
			
		case OBJ_COMPLETE:
			return _("Complete");
			
		case OBJ_FAILED:
			return _("Failed");
	}
	
	return "";
}

const char *Objective::getProgress()
{
	static String s;
	
	if (targetValue == 1)
	{
		return " ";
	}
	
	s.setText("%d / %d", currentValue, targetValue);
	
	return s.getText();
}

void Objective::load(Properties *props)
{
	id = props->getInt("id", id);
	active = props->getInt("active", active);
	description = _(props->getString("description", description.getText()));
	currentValue = props->getInt("currentValue", currentValue);
	targetValue = props->getInt("targetValue", targetValue);
	completionTrigger = props->getString("completionTrigger", completionTrigger.getText());
}

void Objective::save(FILE *fp)
{
	Properties props;
	
	props.setName("Objective");
	
	props.setProperty("id", id);
	props.setProperty("active", active);
	props.setProperty("priority", priority);
	props.setProperty("status", status);
	
	props.setProperty("description", description.getText());
	
	props.setProperty("currentValue", currentValue);
	props.setProperty("targetValue", targetValue);
	
	props.setProperty("completionTrigger", completionTrigger.getText());
	
	props.save(fp);
}
