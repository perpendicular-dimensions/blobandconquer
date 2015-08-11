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

#ifndef __unix__
extern char *strtok_r(char *s1, const char *s2, char **lasts);
#endif

Engine::Engine()
{
	mouse = Mouse::getInstance();
	joystick = NULL;
	buttonState = NULL;
	axis1X = axis1Y = 0;
	axis2X = axis2Y = 0;

	// Timer
	time1 = time2 = 0;
	timeDifference = 0;
	slowLogic = false;
	inertiaRate = 0;
	swapSecondStickAxis = false;
	
	lastReadDataSize = 0;

	defineList.setName("Define List");

	pak = Pak::getInstance();

	debugTimerStart = 0;
	
	joystickIndex = 0;
	
	showFPS = true;
	showPosition = false;
	
	memset(lastKeyEvents, 0, 35);
}

Engine::~Engine()
{
	if (!buttonState)
	{
		delete[] buttonState;
	}
}

Engine *Engine::getInstance()
{
	return &instance;
}

void Engine::addKeyEvent(const char *key)
{
	if (strlen(key) > 1)
	{
		return;
	}

	int index = -1;

	for (int i = 0 ; i < 35 ; i++)
	{
		if (lastKeyEvents[i] == 0)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		for (int i = 0 ; i < 34 ; i++)
		{
			lastKeyEvents[i] = lastKeyEvents[i + 1];
		}

		index = 34;
	}
	
	char k = key[0];
	
	if ((k >= SDLK_a - 32) && (k <= SDLK_z - 32))
	{
		k += 32;
	}

	lastKeyEvents[index] = k;
}

void Engine::getInput()
{
	SDL_GetMouseState(&mouse->x, &mouse->y);

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouse->buttonState[event.button.button] = 1;
				fireMousePressed(event.button);
				break;

			case SDL_MOUSEBUTTONUP:
				mouse->buttonState[event.button.button] = 0;
				fireMouseReleased(event.button);
				break;

			case SDL_MOUSEWHEEL:
				mouse->wheelState = event.wheel.y;
				fireMouseWheeled(event.wheel);
				break;

			case SDL_MOUSEMOTION:
				fireMouseMoved();
				break;

			case SDL_KEYDOWN:
				keyState[event.key.keysym.scancode] = 1;
				fireKeyPressed(&event.key);
				addKeyEvent(SDL_GetScancodeName(event.key.keysym.scancode));
				break;

			case SDL_KEYUP:
				keyState[event.key.keysym.scancode] = 0;
				break;
				
			case SDL_JOYAXISMOTION:
			
				if (event.jaxis.axis == 0)
				{
					axis1X = event.jaxis.value;
				}
				else if (event.jaxis.axis == 1)
				{
					axis1Y = event.jaxis.value;
				}
				else if (event.jaxis.axis == 2)
				{
					if (swapSecondStickAxis)
					{
						axis2X = event.jaxis.value;
					}
					else
					{
						axis2Y = event.jaxis.value;
					}
				}
				else if (event.jaxis.axis == 3)
				{
					if (swapSecondStickAxis)
					{
						axis2Y = event.jaxis.value;
					}
					else
					{
						axis2X = event.jaxis.value;
					}
				}
				break;
				
			case SDL_JOYBUTTONDOWN:
				buttonState[event.jbutton.button] = 1;
				fireJoystickButtonPressed(event.jbutton.button);
				break;
				
			case SDL_JOYBUTTONUP:
				buttonState[event.jbutton.button] = 0;
				break;
		}
	}
}

void Engine::fireMousePressed(SDL_MouseButtonEvent mouse)
{
	//debug(("Engine::fireMousePressed()\n"));

	UIManager::getInstance()->mousePressed(mouse);
}

void Engine::fireMouseReleased(SDL_MouseButtonEvent mouse)
{
	//debug(("Engine::fireMouseReleased()\n"));

	UIManager::getInstance()->mouseReleased(mouse);
}

void Engine::fireMouseMoved()
{
	//debug(("Engine::fireMouseMoved()\n"));

	UIManager::getInstance()->mouseMoved(mouse->x, mouse->y);
}

void Engine::fireMouseWheeled(SDL_MouseWheelEvent mouse)
{
	//debug(("Engine::fireMouseWheeled()\n"));

	UIManager::getInstance()->mouseWheeled(mouse);
}

void Engine::fireKeyPressed(SDL_KeyboardEvent *key)
{
	//debug(("Engine::fireKeyPressed()\n"));

	UIManager::getInstance()->keyPressed(key->keysym.scancode);
}

void Engine::fireJoystickButtonPressed(int key)
{
	UIManager::getInstance()->joystickButtonPressed(key);
}

void Engine::startDebugTimer(const char *text)
{
	debugTimerStart = SDL_GetTicks();

	debug(("%s Started...\n", text));
}

void Engine::endDebugTimer(const char *text)
{
	long now = SDL_GetTicks();

	debug(("%s Finished. Took %d milliseconds\n", text, (int)(now - debugTimerStart)));
}

bool Engine::userAccepts()
{
	if ((keyState[SDL_SCANCODE_SPACE]) || (keyState[SDL_SCANCODE_ESCAPE]) || (keyState[SDL_SCANCODE_LCTRL]) || (keyState[SDL_SCANCODE_RCTRL]) || (keyState[SDL_SCANCODE_RETURN]))
	{
		return true;
	}

	return false;
}

void Engine::flushInput()
{
	while (SDL_PollEvent(&event)){}
}

void Engine::clearInput()
{
	for (int i = 0 ; i < SDL_NUM_SCANCODES ; i++)
	{
		keyState[i] = 0;
	}
	
	if (buttonState != NULL)
	{
		for (int i = 0 ; i < SDL_JoystickNumButtons(joystick) ; i++)
		{
			buttonState[i] = 0;
		}
	}
	
	mouse->clear();
}

void Engine::doTimeDifference()
{
	inertiaRate -= (int)inertiaRate;
	
	timeDifference = (time2 - time1) / 10.0;
	time1 = time2;
	time2 = SDL_GetTicks();
	
	inertiaRate += timeDifference;
}

float Engine::getTimeDifference(int tdType)
{
	if ((slowLogic) && (tdType == TD_LOGIC))
	{
		return timeDifference * 0.25;
	}
	
	return timeDifference;
}

float Engine::getInertiaRate()
{
	return inertiaRate;
}

void Engine::resetTimeDifference()
{
	time1 = time2 = SDL_GetTicks();
	
	inertiaRate = 0;
}

/*
Loads key-value defines into a linked list, comments are ignored. The defines.h file is used by the
game at compile time and run time, so everything is syncronised. This technique has the advantage of
allowing the game's data to be human readable and easy to maintain.
*/
bool Engine::loadDefines()
{
	char string[2][1024];

	if (!pak->unpack("data/gameDefs/defines.h", PAK::TEXT))
	{
		Graphics::getInstance()->showErrorAndExit(ERR_FILE, "data/gameDefs/defines.h");
	}

	char *token = strtok((char*)pak->dataBuffer, "\n");

	Data *data;
	
	bool readEnum = false;
	int value = 0;

	while (true)
	{
		if (!token)
		{
			break;
		}

		if (strstr(token, "/*"))
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (strstr(token, "enum {"))
		{
			readEnum = true;
			value = 0;
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (strstr(token, "};"))
		{
			readEnum = false;
			token = strtok(NULL, "\n");
			continue;
		}
		
		if (strlen(token) < 4)
		{
			token = strtok(NULL, "\n");
			continue;
		}
		
		// power of
		if (strstr(token, "<<"))
		{
			sscanf(token, "%*s %s %*s %*s %*s %d", string[0], &value);
			value = 2 << value;
			sprintf(string[1], "%d", value);
		}
		else
		{
			if (!readEnum)
			{
				sscanf(token, "%*s %s %[^\n]", string[0], string[1]);
			}
			else
			{
				if (strstr(token, ","))
				{
					sscanf(token, "%*c %[^,]", string[0]);
				}
				else
				{
					sscanf(token, "%*c %s", string[0]);
				}
				sprintf(string[1], "%d", value);
				value++;
			}
		}
		
		data = new Data();
		data->setKey(string[0]);
		data->setValue(string[1]);
		defineList.add(data);
		
		token = strtok(NULL, "\n");
	}
	
	pak->freeAll();

	return true;
}

/*
Returns the value of a #defined value... ACTIVE is declared as 1 so it will
traverse the list and return 1 when it encounters ACTIVE. This has two advantages.
1) It makes the game data human readable and 2) It means if I change a #define in
the code, I don't have to change all the data entries too. You probably already
thought of that though... :)
*/
int Engine::getValueOfDefine(const char *word)
{
	// might be a real number!
	int length = strlen(word);
	
	if (length > 0)
	{
		if ((isdigit(word[0])) && (isdigit(word[length - 1])))
		{
			return atoi(word);
		}
		else if ((word[0] == '-') && (isdigit(word[length - 1])))
		{
			return atoi(word);
		}
	}
	
	int rtn = 0;

	Data *data = (Data*)defineList.getFirstElement();

	while (data != NULL)
	{
		if (data->key == word)
		{
			rtn = atoi(data->getValue());
			return rtn;
		}

		data = (Data*)data->next;
	}

	printf("ERROR: getValueOfDefine() : %s is not defined!\n", word);
	exit(1);
}

/*
Does the opposite of the above(!)
*/
const char *Engine::getDefineOfValue(const char *prefix, int value)
{
	int rtn = 0;

	Data *data = (Data*)defineList.getFirstElement();

	while (data != NULL)
	{
		if (strstr(data->getKey(), prefix))
		{
			rtn = atoi(data->getValue());

			if (rtn == value)
			{
				return data->getKey();
			}
		}

		data = (Data*)data->next;
	}

	printf("ERROR: getDefineOfValue() : %s, %d is not defined!\n", prefix, value);
	exit(1);
}

/*
I like this function. It receives a list of flags declared by their #define name... like
the function above, delimited with plus signs. So ENT_FLIES+ENT_AIMS. It then works out the
flags (in a bit of a half arsed manner because of my lazy (2 << 25) declarations, adds all
the values together and then returns them... phew! Makes data files human readable though :)
*/
int Engine::getValueOfFlagTokens(const char *realLine)
{
	// might be a real number!
	int length = strlen(realLine);
	
	if (length > 0)
	{
		if ((isdigit(realLine[0])) && (isdigit(realLine[length - 1])))
		{
			return atoi(realLine);
		}
	}

	char *store = NULL;
	char line[1024];
	bool found = false;
	int value = 0;
	strcpy(line, realLine);

	int flags = 0;

	char *word = strtok_r(line, "+", &store);

	if (!word)
	{
		printf("ERROR: getValueOfFlagTokens() : NULL Pointer!\n");
		return 0;
	}

	Data *data = NULL;

	while (true)
	{
		data = (Data*)defineList.getFirstElement();
		found = false;

		while (data != NULL)
		{
			if (strcmp(data->getKey(), word) == 0)
			{
				value = -1;
				sscanf(data->getValue(), "%d", &value);
				flags += value;
				found = true;
				break;
			}

			data = (Data*)data->next;
		}

		if (!found)
		{
			printf("ERROR: getValueOfFlagTokens() : Illegal Token '%s'\n", word);
			printf("ERROR: Ensure there are no spaces in token line!!\n");
			#if IGNORE_FLAGTOKEN_ERRORS
				break;
			#else
				exit(1);
			#endif
		}

		word = strtok_r(NULL, "+", &store);

		if (!word)
		{
			break;
		}
	}

	//debug(("Engine::getValueOfFlagTokens - Returning %d\n", flags));

	return flags;
}

void Engine::saveSaveGameHeaders()
{
	String filename;
	filename.setText("%s/savedata", userHomeDirectory.getText());
	
	FILE *fp = fopen(filename.getText(), "wb");
	
	fprintf(fp, "// WARNING - DO NOT EDIT THIS FILE!!!\n");
	fprintf(fp, "// THIS FILE IS GENERATED BY BLOB AND CONQUER AND IF YOU EDIT THIS FILE\n");
	fprintf(fp, "// YOU MAY LOSE ACCESS TO YOUR SAVE GAMES! YOU HAVE BEEN WARNED!\n\n");
	
	Properties props;
	props.setName("SaveGame");
	
	for (int i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		saveSlot[i].save(fp);
	}
	
	fprintf(fp, "@END@\n");
	
	fclose(fp);
}

Engine Engine::instance;
