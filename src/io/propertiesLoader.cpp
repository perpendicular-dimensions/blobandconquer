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

#include "propertiesLoader.h"

List *loadBSPProperties(const char *entities)
{
	debug(("loadBSPProperties() - Begin\n"));
	
	List *propsList = new List();
	propsList->setName("LoadBSPPropertiesList");
	
	Properties *properties;
	char *token = strtok((char*)entities, "\n");
	char key[1024], value[1024];
	
	while (true)
	{
		// June 2009 fix for null tokens
		if (!token || token == NULL)
		{
			break;
		}
		
		if (strstr(token, "\0") == 0)
		{
			break;
		}
		
		if (strcmp(token, "") == 0)
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (strstr(token, "{"))
		{
			properties = new Properties();
			propsList->add(properties);
			
			while (true)
			{
				token = strtok(NULL, "\n");
				
				// June 2009 fix for null tokens
				if (!token || token == NULL)
				{
					break;
				}
				
				if (strstr(token, "}"))
				{
					break;
				}
				
				sscanf(token, "%*c%[^\"]%*c%*c%*c%[^\"]", key, value);
				
				if (strcmp(key, "classname") == 0)
				{
					properties->setName(value);
				}
				
				properties->setProperty(key, value);
			}
		}
		
		token = strtok(NULL, "\n");
	}
	
	debug(("loadBSPProperties() - Finished. Loaded %d\n", propsList->getSize()));
	
	return propsList;
}

List *loadPropertiesInMemory()
{
	debug(("loadPropertiesInMemory()\n"));
	
	List *propsList = new List();
	propsList->setName("LoadPropertiesList");

	Properties *properties;

	char propTitle[1024];
	char key[1024];
	char value[1024];

	int i = 0;

	char *token = strtok((char*)pak->dataBuffer, "\n");
	
	while (true)
	{
		// ignore blank lines
		if (strcmp(token, "") == 0)
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		// looks like a comment.
		if (token[0] == '/')
		{
			token = strtok(NULL, "\n");
			continue;
		}

		sscanf(token, "%s %*s", propTitle);

		if (strcmp(propTitle, "@END@") == 0)
		{
			break;
		}

		properties = new Properties();
		propsList->add(properties);
		properties->setName(propTitle);

		//debug(("Loading Properties set %s\n", propTitle));

		i = 0;

		while (true)
		{
			i++;

			if (i == 1000)
			{
				printf("FATAL ERROR: loadProperties() count reached 1000\n");
				exit(1);
			}

			token = strtok(NULL, "\n");

			if (token[1] == '/')
			{
				continue;
			}
			else if (strstr(token, "="))
			{
				if (strstr(token, ";"))
				{
					sscanf(token, "%[^=]%*c%[^;]", key, value);
					
					if (key[0] == '/')
					{
						continue;
					}
					
					properties->setProperty(key, value);
				}
				else
				{
					printf("ERROR: Property line in '%s' does not end with a semicolumn.\n", propTitle);
					printf("ERROR: Offending line is '%s'.\n", token);
					exit(1);
				}
			}
			else if (strstr(token, "}"))
			{
				break;
			}
		}

		token = strtok(NULL, "\n");
	}

	debug(("loadPropertiesInMemory() :: Done\n"));
	
	pak->freeAll();

	return propsList;
}

List *loadProperties(const char *filename, bool required)
{
	if (!pak->unpack(filename, PAK::TEXT))
	{
		if (required)
		{
			graphics->showErrorAndExit(ERR_FILE, filename);
		}
		
		return new List();
	}
	
	return loadPropertiesInMemory();
}
