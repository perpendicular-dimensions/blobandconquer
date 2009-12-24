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

SaveGameHeader::SaveGameHeader()
{
	inUse = false;
	
	description = "Empty";
}

SaveGameHeader::~SaveGameHeader()
{
}

void SaveGameHeader::load(Properties *props)
{
	inUse = props->getInt("inUse", inUse);
	description = props->getString("description", description.getText());
	mapName = props->getString("mapName", mapName.getText());
	time = props->getString("time", time.getText());
	date = props->getString("date", date.getText());
}

void SaveGameHeader::save(FILE *fp)
{
	Properties props;
	props.setName("Save");
	
	props.setProperty("inUse", inUse);
	props.setProperty("description", description.getText());
	props.setProperty("mapName", mapName.getText());
	props.setProperty("time", time.getText());
	props.setProperty("date", date.getText());
	
	props.save(fp);
}
