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

RandomNameGenerator::RandomNameGenerator()
{
	for (int i = 0 ; i < 26 ; i++)
	{
		strcpy(letterFrequency[i], "");
	}

	tableLoaded = false;
}

RandomNameGenerator *RandomNameGenerator::getInstance()
{
	return &instance;
}

void RandomNameGenerator::loadFrequencyTable(const char *filename)
{
	if (!Pak::getInstance()->unpack(filename, PAK::TEXT))
	{
		Graphics::getInstance()->showErrorAndExit("RandomNameGenerator : Couldn't load %s", filename);
	}
	
	char *token = strtok((char*)Pak::getInstance()->dataBuffer, "\n");

	for (int i = 0 ; i < 26 ; i++)
	{
		sscanf(token, "%s", letterFrequency[i]);
		
		token = strtok(NULL, "\n");
	}

	tableLoaded = true;
}

void RandomNameGenerator::loadFilterWords()
{
}

char *RandomNameGenerator::createName()
{
	if (!tableLoaded)
	{
		Graphics::getInstance()->showErrorAndExit("RandomNameGenerator::createName - Frequency Table Not Loaded\n");
	}

	int letter = (int)(rand() % 26);
	int len = 0;
	char c;
	char i = 0;
	char length = Math::rrand(4, 8);

	char cons = 0;
	char vowels = 0;

	strcpy(name, "");

	while (true)
	{
		len = strlen(letterFrequency[letter]);
		c = (char)letterFrequency[letter][rand() % len];

		switch (c)
		{
			case 'A':
			case 'a':
			case 'E':
			case 'e':
			case 'I':
			case 'i':
			case 'O':
			case 'o':
			case 'U':
			case 'u':
				vowels++;
				cons = 0;
				break;
			default:
				vowels = 0;
				cons++;
				break;
		}

		if ((vowels > 2) || (cons > 2))
		{
			continue;
		}

		if (i > 0)
		{
			c += 32;
		}

		letter = (char)c;
		letter -= 65;
		
		if (i > 0)
		{
			letter -= 32;
		}

		sprintf(name, "%s%c", name, c);
		i++;

		if (i == length)
		{
			break;
		}
	}

	return name;
}

// DO NOT REMOVE THIS FUNCTION! IT'S FOR CENSORSHIP!
bool RandomNameGenerator::isNameOkay(const char *name)
{
	for (int i = 0 ; i < 7 ; i++)
	{
		if (strstr(name, badWord[i]))
		{
			return false;
		}
	}

	return true;
}

char *RandomNameGenerator::getRandomName()
{
	char *name = createName();

	while (!isNameOkay(name))
	{
		name = createName();
	}

	return name;
}

RandomNameGenerator RandomNameGenerator::instance;
