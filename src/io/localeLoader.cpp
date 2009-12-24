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

#include "localeLoader.h"

void loadLocaleIndex()
{
	Properties *properties = &LocaleManager::getInstance()->localeIndex;
	
	if (pak->getFileLocation("data/locale/index") == FILE_NOT_FOUND)
	{
		printf("Locale data has not been installed. Blob And Conquer cannot run without it.\n");
		exit(1);
	}
	
	List *list = loadKeyValuePairs("data/locale/index");
	
	if (list == NULL)
	{
		printf("ERROR: Couldn't load Locale Index File (data/locale/index)");
		exit(1);
	}
	
	for (Data *data = (Data*)list->getFirstElement() ; data != NULL ; data = (Data*)data->next)
	{
		properties->setProperty(data->getKey(), data->getValue());
	}
	
	delete list;
}

Properties *loadLocaleData(const char *filename)
{
	debug(("Loading locale data from '%s'\n", filename));

	if (!pak->unpack(filename), PAK::TEXT)
	{
		if (strcmp(localeManager->getLanguage(), "en") == 0)
		{
			printf("ERROR - Could not load '%s'\n", filename);
			exit(1);
		}
		return NULL;
	}
	
	Properties *properties = new Properties();

	char key[1024];
	char value[1024];
	
	char *token = strtok((char*)pak->dataBuffer, "\n");

	bool done = false;
	
	while (!done)
	{
		// ignore blank lines
		if ((strcmp(token, "") == 0) || (strlen(token) == 0))
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
		
		if (strcmp(token, "@END@") == 0)
		{
			done = true;
		}
		else if (!strstr(token, ";"))
		{
			token = strtok(NULL, "\n");
			continue;
		}
		else
		{
			sscanf(token, "%s %[^;]", key, value);
			
			properties->setProperty(key, value);
		}

		token = strtok(NULL, "\n");
	}
	
	pak->freeAll();

	return properties;
}

void loadGameLocaleData(const char *filename, bool append)
{
	String path;
	
	path.setText("data/locale/en/%s", filename);
	Properties *defaultLang = loadLocaleData(path.getText());

	if (defaultLang != NULL)
	{
		if (!append)
		{
			debug(("Setting Game Properties to Locale Manager...\n"));
			LocaleManager::getInstance()->setLocaleData(defaultLang);
		}
		else
		{
			debug(("Appending Game Properties to Locale Manager...\n"));
			LocaleManager::getInstance()->appendLocaleData(defaultLang);
			delete defaultLang;
		}
	}
	
	if (strcmp(localeManager->getLanguage(), "en") == 0)
	{
		return;
	}
	
	path.setText("data/locale/%s/%s", localeManager->getLanguage(), filename);
	
	Properties *translated = loadLocaleData(path.getText());
	
	if (translated != NULL)
	{
		if (!append)
		{
			debug(("Setting Game Properties to Locale Manager...\n"));
			LocaleManager::getInstance()->setLocaleData(translated);
		}
		else
		{
			debug(("Appending Game Properties to Locale Manager...\n"));
			LocaleManager::getInstance()->appendLocaleData(translated);
			delete translated;
		}
	}
}

void loadSystemLocaleData(const char *filename)
{
	String path;
	
	path.setText("data/locale/en/%s", filename);
	Properties *defaultLang = loadLocaleData(path.getText());

	if (defaultLang != NULL)
	{
		debug(("Setting System Properties to Locale Manager...\n"));
		LocaleManager::getInstance()->setSystemLocaleData(defaultLang);
	}
	
	if (strcmp(localeManager->getLanguage(), "en") == 0)
	{
		return;
	}
	
	path.setText("data/locale/%s/%s", localeManager->getLanguage(), filename);
	
	Properties *translated = loadLocaleData(path.getText());
	
	if (translated != NULL)
	{
		debug(("Setting System Properties to Locale Manager...\n"));
		LocaleManager::getInstance()->setSystemLocaleData(translated);
	}
}
