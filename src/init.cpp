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

#include "init.h"

void checkForLicense();

/*
Show the GNU Public License the first time the game is played. Waits 4 seconds
and then proceeds. THIS MUST NOT BE REMOVED!!!!!
*/
void showLicense()
{
	return;

	Texture *texture = graphics->loadTexture("gfx/game/licensePic.png");
	
	if (!pak->unpack(_("data/locale/en/LICENSE"), PAK::TEXT))
	{
		printf("License Agreement has been removed\n");
		exit(1);
	}
	
	mouse->setBusy(true);
	
	graphics->setMode(MODE_2D);
	
	graphics->clearScreen();
	graphics->delay(1000, false);
	
	renderScaledTexture(texture);
	
	char line[1024];
	int y = 0;
	int dy = 0;
	
	char *token = strtok((char*)pak->dataBuffer, "\n");
	
	while (token != NULL)
	{
		sscanf(token, "%d %[^\n]", &dy, line);
	
		if (dy != -1)
		{
			y += dy;
		
			graphics->drawString(graphics->screenMidX, y, TXT_CENTERED, GLColor::white, true, "%s", line);
		}
		
		token = strtok(NULL, "\n");
	}
	
	pak->freeAll();
	
	graphics->updateScreen();
	
	graphics->delay(40000, false);
	
	delete texture;
}

void showViolenceWarning()
{
	Texture *texture = graphics->loadTexture("gfx/game/violenceWarning.jpg");
	
	mouse->setBusy(true);
	
	graphics->setMode(MODE_2D);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	graphics->clearScreen();
	graphics->delay(1000, false);
	
	renderScaledTexture(texture);
	
	graphics->updateScreen();
	
	graphics->delay(4000, true);
	
	delete texture;
}

#ifdef __unix__
void setupUserHomeDirectory()
{
	char *userHome = getenv("HOME");

	if ((!userHome) || (userHome == NULL))
	{
		printf("Couldn't determine user home directory! Exiting.\n");
		exit(1);
	}

	String dir;

	dir.setText("%s/.parallelrealities", userHome);
	if ((mkdir(dir.getText(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0) && (errno != EEXIST))
	{
		printf("Couldn't create required directory '%s'\n", dir.getText());
		exit(1);
	}

	dir.setText("%s/.parallelrealities/blobAndConquer", userHome);
	if ((mkdir(dir.getText(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0) && (errno != EEXIST))
	{
		printf("Couldn't create required directory '%s'\n", dir.getText());
		exit(1);
	}

	engine->userHomeDirectory.setText("%s/.parallelrealities/blobAndConquer", userHome);
	
	pak->tmpFilename.setText("%s/.parallelrealities/blobAndConquer/tmpFile", userHome);
}
#endif

void loadSaveGameHeaders()
{
	String filename;
	filename.setText("%s/savedata", engine->userHomeDirectory.getText());
	
	List *list = loadProperties(filename.getText(), false);
	
	if (list == NULL)
	{
		return;
	}
	
	int slot = 0;
	
	for (Properties *props = (Properties*)list->getFirstElement() ; props != NULL ; props = (Properties*)props->next)
	{
		engine->saveSlot[slot++].load(props);
	}
	
	delete list;
}

void loadConfig()
{
	String string;
	string.setText("%s/config", engine->userHomeDirectory.getText());
	
	List *list = loadProperties(string.getText(), false);
	
	if ((list == NULL) || (list->getSize() == 0))
	{
		return;
	}
	
	Properties *props = (Properties*)list->getFirstElement();
	
	graphics->fullscreen = props->getInt("fullscreen", 0);
	audio->soundVolume = props->getInt("soundVolume", 128);
	audio->musicVolume = props->getInt("musicVolume", 90);
	audio->ambientVolume = props->getInt("ambientVolume", 65);
	audio->environmentVolume = props->getInt("environmentVolume", 50);
	graphics->gamma = props->getFloat("brightness", 100);
	
	Math::limit(&graphics->gamma, 50, 200);
	
	int w, h, c;
	sscanf(props->getString("resolution", "0 x 0"), "%d %*s %d", &w, &h);
	
	for (unsigned i = 0 ; i < MAX_RESOLUTIONS ; i++)
	{
		if ((graphics->screenMode[i].w == w) && (graphics->screenMode[i].h == h))
		{
			graphics->currentScreenResolution = i;
		}
	}
	
	for (int i = 0 ; i < CTRL_MAXIMUM ; i++)
	{
		string.setText("keyboard%d", i);
		sscanf(props->getString(string.getText(), "-1"), "%d", &c);
		if (c >= -1 && c < SDL_NUM_SCANCODES)
			game->keyboard[i] = c;
		
		string.setText("joystick%d", i);
		sscanf(props->getString(string.getText(), "-1"), "%d", &c);
		game->joystick[i] = c;
		
		string.setText("mouse%d", i);
		sscanf(props->getString(string.getText(), "-1"), "%d", &c);
		game->mouseCtrl[i] = c;
	}
	
	game->joystickDeadZone = props->getInt("joystickDeadZone", 4500);
	game->mouseSensitivity = props->getInt("mouseSensitivity", 100);
	mouse->grab = props->getInt("grabMouse", 1);
	
	game->bloodPolicy = props->getInt("bloodPolicy", BLOOD_POLICY_ON);
	game->shadowPolicy = props->getInt("shadowPolicy", SHADOW_POLICY_ON);
	game->decalPolicy = props->getInt("decalPolicy", DECAL_POLICY_ON);
	graphics->motionBlur = props->getInt("motionBlur", 100);
	
	engine->swapSecondStickAxis = props->getInt("swapSecondStickAxis", 0);
	
	delete list;
	
	return;
}

void saveConfig()
{
	if (graphics->screen == NULL)
	{
		return;
	}
	
	String savePath;
	savePath.setText("%s/config", engine->userHomeDirectory.getText());
	
	FILE *fp = fopen(savePath.getText(), "wb");
	
	if (!fp)
	{
		return;
	}
	
	Properties props;
	
	props.setName("Config");
	
	String string;
	string.setText("%d x %d", graphics->screen->w, graphics->screen->h);
	
	props.setProperty("resolution", string.getText());
	props.setProperty("fullscreen", graphics->fullscreen);
	props.setProperty("soundVolume", audio->soundVolume);
	props.setProperty("musicVolume", audio->musicVolume);
	props.setProperty("ambientVolume", audio->ambientVolume);
	props.setProperty("environmentVolume", audio->environmentVolume);
	props.setProperty("brightness", graphics->gamma);
	
	for (int i = 0 ; i < CTRL_MAXIMUM ; i++)
	{
		string.setText("keyboard%d", i);
		props.setProperty(string.getText(), game->keyboard[i]);
		
		string.setText("joystick%d", i);
		props.setProperty(string.getText(), game->joystick[i]);
		
		string.setText("mouse%d", i);
		props.setProperty(string.getText(), game->mouseCtrl[i]);
	}
	
	props.setProperty("joystickDeadZone", game->joystickDeadZone);
	props.setProperty("mouseSensitivity", game->mouseSensitivity);
	props.setProperty("grabMouse", mouse->grab);
	
	props.setProperty("bloodPolicy", game->bloodPolicy);
	props.setProperty("shadowPolicy", game->shadowPolicy);
	props.setProperty("decalPolicy", game->decalPolicy);
	props.setProperty("motionBlur", graphics->motionBlur);
	
	props.setProperty("swapSecondStickAxis", engine->swapSecondStickAxis);
	
	props.save(fp);
	
	fprintf(fp, "@END@\n");
	
	fclose(fp);
}

void initOpenGL()
{
	debug(("initOpenGL()\n"));
	String extensions;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glShadeModel(GL_SMOOTH);

    glClearDepth(1.0f);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glDepthFunc(GL_LEQUAL);
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	
	extensions = (char*)glGetString(GL_EXTENSIONS);
	
	if (!strstr(extensions.getText(), "GL_ARB_multitexture"))
	{
		printf("OpenGL multitexturing is not supported");
		exit(1);
	}
	
	if (!strstr(extensions.getText(), "GL_ARB_multitexture"))
	{
		printf("OpenGL multitexturing is not supported");
		exit(1);
	}
	
	GLint stencil;
	
	glGetIntegerv(GL_STENCIL_BITS, &stencil);
	
	/* update queried hasStencils value with the real thing */
	graphics->hasStencils = stencil;
	if (!graphics->hasStencils) {
		printf("Warning: Stencil Buffering is not supported,\n");
#ifdef __unix__
		printf("  try changing your X-windows colordepth to 32bpp,\n");
#endif
		printf("  disabling shadows!\n");
		game->shadowPolicy = SHADOW_POLICY_OFF;
	}
	
	/*
	char *ext = strtok(extensions.getText(), " ");
	
	while (ext)
	{
		debug(("initOpenGL() - Extension '%s' Available\n", ext));
		ext = strtok(NULL, " ");
	}
	*/
	
	debug(("Vendor    : %s\n", glGetString(GL_VENDOR)));
	debug(("Version   : %s\n", glGetString(GL_VERSION)));
	debug(("Renderer  : %s\n", glGetString(GL_RENDERER)));
	
	debug(("initOpenGL() :: Done\n"))
}

void initCommonResources()
{
	String f;
	f.setText("%s/config", engine->userHomeDirectory.getText());

	graphics->mapColors();
	graphics->setGamma(graphics->gamma);

	graphics->loadFont(FONT_NORMAL, "data/fonts/vera.ttf", 12);
	graphics->loadFont(FONT_LARGE, "data/fonts/vera.ttf", 32);
	graphics->loadFont(FONT_DIGITAL, "data/fonts/vera.ttf", 32);
	graphics->loadFont(FONT_LARGE_DIGITAL, "data/fonts/vera.ttf", 5);
	graphics->loadFont(FONT_NUMBER, "data/fonts/vera.ttf", 5);
	graphics->loadTimerDigits();

	uiManager->upArrow = graphics->loadTexture("gfx/gui/upArrow.png");
	uiManager->downArrow = graphics->loadTexture("gfx/gui/downArrow.png");
	uiManager->minus = graphics->loadTexture("gfx/gui/minus.png");
	uiManager->plus = graphics->loadTexture("gfx/gui/plus.png");
	uiManager->unchecked = graphics->loadTexture("gfx/gui/unchecked.png");
	uiManager->checked = graphics->loadTexture("gfx/gui/checked.png");
	uiManager->awaitingButton = graphics->getGLString("???");
	
	if (!pak->exists(f.getText()))
	{
		#if !DEV
		#endif
	}
	
	showLicense();
	
	#if !DEV
	showViolenceWarning();
	#endif
	
	graphics->clearScreen();
	graphics->updateScreen();
}

void initSystem()
{
	debug(("initSystem()\n"))

	#ifdef __unix__
	setupUserHomeDirectory();
	#endif
	
	loadSaveGameHeaders();

	/* Initialize the SDL library */
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	
	graphics->calculateScreenModes();
	graphics->queryStencilSupport();

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	if (graphics->hasStencils)
	{
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	}

	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 16);

	graphics->window = SDL_CreateWindow("Blobwars: Blob And Conquer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_SetWindowMinimumSize(graphics->window, 800, 600);

	SDL_GL_CreateContext(graphics->window);

	SDL_GL_SetSwapInterval(1);
	
	loadConfig();
	
	debug(("User Home = %s\n", engine->userHomeDirectory.getText()));
	
	SDL_SetWindowFullscreen(graphics->window, graphics->fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
	
	graphics->setResolution(graphics->currentScreenResolution);
	

	if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
		exit(1);
	}

	//audio->useAudio = false;

	if (audio->useAudio)
	{
		debug(("Opening Audio...\n"));
		
		if (Mix_OpenAudio(22050, AUDIO_S16, 1, 1024) < 0)
		{
			printf("Warning: Couldn't set 22050 Hz 16-bit audio - Reason: %s\n", Mix_GetError());
			printf("Sound and Music will be disabled\n");
			audio->useAudio = 0;
		}
		else
		{
			debug(("Audio OK...\n"));
			
			int result = 0;
			
			result = Mix_AllocateChannels(MAX_CHANNELS);
			debug(("Allocated %d sound channels\n", result));
			
			result = Mix_ReserveChannels(CH_EFFECT1);
			debug(("Reserved first %d sound channels\n", result));
		}
	}

	if (SDL_NumJoysticks() > 0)
	{
		debug(("Found a joystick...\n"));
		
		if (engine->joystickIndex >= SDL_NumJoysticks())
		{
			printf("ERROR:\n");
			printf("------\n");
			printf("Refusing to open joystick #%d when only %d stick found. Please re-run with\n\n", engine->joystickIndex, SDL_NumJoysticks());
			printf("-joystick <n>\n\n");
			printf("where n <= %d\n", SDL_NumJoysticks() - 1);
			printf("(note: joystick indexes begin from 0)\n\n");
			exit(1);
		}

		SDL_JoystickEventState(SDL_ENABLE);

		debug(("Opening Joystick %s...\n", SDL_JoystickNameForIndex(engine->joystickIndex)));
		engine->joystick = SDL_JoystickOpen(engine->joystickIndex);
		engine->buttonState = new char[SDL_JoystickNumButtons(engine->joystick)];
		
		debug(("Number of Axes: %d\n", SDL_JoystickNumAxes(engine->joystick)));
	}
	else
	{
		debug(("No joystick available.\n"));
	}

	srand(time(NULL));

	//SDL_SetWindowIcon(graphics->window, device);
	SDL_SetWindowTitle(graphics->window, "Blob Wars: Blob and Conquer");

	// force defaults or loaded
	audio->setSoundVolume(audio->soundVolume);
	audio->setMusicVolume(audio->musicVolume);
	audio->setAmbientVolume(audio->ambientVolume);
	audio->setEnvironmentVolume(audio->environmentVolume);

	RandomNameGenerator::getInstance()->loadFrequencyTable("data/misc/letterFrequencyTable");

	engine->loadDefines();

	// Always needed
	audio->loadSound(SND_BUTTONCLICK, "sound/gui/buttonClick.ogg");

	debug(("initSystem() :: Done\n"));
}

/*
Removes [hopefully] all the resources that has been
loaded and created during the game. This is called by
atexit();
*/
void cleanup()
{
	//BoundingBoxManager::getInstance()->dump();
	
	mouse->setBusy(false);
	
	debug(("Cleaning Up...\n"));
	
	debug(("Deleting Tracer...\n"));
	delete tracer;

	debug(("Freeing Audio...\n"));
	audio->destroy();

	debug(("Removing Temp File...\n"));
	remove(pak->tmpFilename.getText());

	debug(("Saving Config...\n"));
	saveConfig();

	debug(("Removing PAK File Data...\n"));
	//Pak::getInstance()->clear();

	debug(("Closing Audio...\n"));
	if (audio->useAudio)
	{
		Mix_CloseAudio();
	}

	debug(("Freeing Game Data...\n"));
	game->destroy();

	debug(("Freeing Remaining Entities...\n"));
	entityManager->destroy();

	debug(("Freeing Widgets...\n"));
	uiManager->clearWidgetList();

	debug(("Freeing Sprites and Fonts\n"));
	graphics->destroy();

	debug(("Freeing Textures...\n"));
	textureManager->clear();

	debug(("Freeing Models...\n"));
	modelManager->clear();

	debug(("Closing TTF...\n"));
	TTF_Quit();

	if (engine->joystick != NULL)
	{
		SDL_JoystickEventState(SDL_DISABLE);
		debug(("Closing Joystick %s...\n", SDL_JoystickName(0)));
		SDL_JoystickClose(engine->joystick);
	}
	
	debug(("Quitting SDL...\n"));
	
	SDL_Quit();

	debug(("All Done.\n"));
}

