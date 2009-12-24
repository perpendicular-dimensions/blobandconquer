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

#include "title.h"

void drawTitle()
{
	graphics->blit(textureManager->getTexture("backdrop"), graphics->screenMidX, graphics->screenMidY, true);
		
	graphics->blit(textureManager->getTexture("title"), graphics->screenMidX, 75, true);
		
	graphics->blit(textureManager->getTexture("episode2"), graphics->screenMidX, 130, true);
		
	graphics->blit(textureManager->getTexture("copyright"), 5, graphics->screen->h - 20, false);
	
	Texture *version = textureManager->getTexture("version");
		
	graphics->blit(version, graphics->screen->w - version->iw - 5, graphics->screen->h - 20, false);
}

int doTitle()
{
	newGame();
	
	game->inGame = false;
	
	loadWidgets("data/widgets/title");
	loadWidgets("data/widgets/inGameOptions");
	loadWidgets("data/widgets/generalOptions");
	loadWidgets("data/widgets/controlOptions");;
	loadWidgets("data/widgets/status");
	loadWidgets("data/widgets/loadSave");
	loadWidgets("data/widgets/gameover");
	loadWidgets("data/widgets/cheats");
	loadWidgets("data/widgets/additional");
	loadWidgets("data/widgets/language");
	
	audio->loadMusic("music/Helmet Shake.mod");
	
	Texture *prLogo = graphics->loadTexture("gfx/game/prlogo.png");
	textureManager->addTexture("prLogo", prLogo);
	
	graphics->setFont(FONT_LARGE);
	
	Texture *presents = graphics->getGLString(_("Presents"));
	textureManager->addTexture("presents", presents);
	
	Texture *sdl = graphics->getGLString(_("An SDL-OpenGL Game"));
	textureManager->addTexture("sdl", sdl);
	
	Texture *title = graphics->loadTexture("gfx/game/title.png");
	textureManager->addTexture("title", title);
	
	Texture *backdrop = graphics->loadTexture("gfx/game/titleBack.png");
	textureManager->addTexture("backdrop", backdrop);
	
	graphics->setFont(FONT_NORMAL);
	
	Texture *copyright = graphics->getGLString(_("Copyright Parallel Realities 2008"));
	textureManager->addTexture("copyright", copyright);
	
	Texture *version = graphics->getGLString(_("Version %.2f"), VERSION);
	textureManager->addTexture("version", version);
	
	Texture *episode2 = graphics->loadTexture("gfx/game/episode2.png");
	textureManager->addTexture("episode2", episode2);
	
	Button *newGame = (Button*)uiManager->getWidget("New", "titleWidgets");
	Button *training = (Button*)uiManager->getWidget("Training", "titleWidgets");
	Button *load = (Button*)uiManager->getWidget("Load", "titleWidgets");
	Button *options = (Button*)uiManager->getWidget("Options", "titleWidgets");
	Button *quit = (Button*)uiManager->getWidget("Quit", "titleWidgets");
	
	uiManager->onlyShowGroup("titleWidgets");
	
	Grid *grid = uiManager->getGrid("TitleGrid");
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	bool finished = false;
	
	mouse->setBusy(false);
	
	graphics->clearScreen();
	
	audio->playMusic();
	
	graphics->delay(1000, false);
	graphics->allowMotionBlur = false;
	
	engine->resetTimeDifference();
	
	float logoTimer = 0;
	float fade = 0;
	float backX = graphics->screen->w + (backdrop->iw / 2);
	int rtn = SECTION_TITLE;
	
	graphics->setMode(MODE_2D);
	
	while (!finished)
	{
		engine->doTimeDifference();
		engine->getInput();
		graphics->updateScreen();
		
		graphics->clearScreen();
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		logoTimer += engine->getTimeDifference(TD_LOGIC);
		
		if (engine->keyState[SDLK_ESCAPE])
		{
			backX = graphics->screenMidX;
			logoTimer = 2800;
			fade = 1.0;
			engine->keyState[SDLK_ESCAPE] = 0;
		}
		
		if ((logoTimer > 300) && (logoTimer <= 700))
		{
			graphics->blit(prLogo, graphics->screenMidX, graphics->screenMidY, true);
		}
		else if ((logoTimer > 900) && (logoTimer <= 1300))
		{
			graphics->blit(presents, graphics->screenMidX, graphics->screenMidY, true);
		}
		else if ((logoTimer > 1500) && (logoTimer <= 1800))
		{
			graphics->blit(sdl, graphics->screenMidX, graphics->screenMidY, true);
		}
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		graphics->blit(backdrop, backX, graphics->screenMidY, true);
		backX -= 0.2 * engine->getTimeDifference(TD_LOGIC);
		Math::limit(&backX, graphics->screenMidX, graphics->screen->w + (backdrop->iw / 2));
		
		if (logoTimer < 2800)
		{
			continue;
		}
		
		glColor4f(1.0, 1.0, 1.0, fade);
		
		graphics->blit(title, graphics->screenMidX, 75, true);
		
		fade += engine->getTimeDifference(TD_LOGIC) * 0.002;
		
		if (fade < 1.0)
		{
			continue;
		}
			
		graphics->blit(episode2, graphics->screenMidX, 130, true);
			
		graphics->blit(copyright, 5, graphics->screen->h - 20, false);
		
		graphics->blit(version, graphics->screen->w - version->iw - 5, graphics->screen->h - 20, false);
		
		graphics->fade(0.10, grid->x, grid->y, grid->width, grid->height);
		graphics->outlineRect(grid->x, grid->y, grid->width, grid->height, GLColor::lightGrey);
		
		drawWidgets();
		
		if (newGame->wasClicked())
		{
			rtn = SECTION_NEWGAME;
			finished = true;
		}
		
		if (training->wasClicked())
		{
			rtn = SECTION_TRAINING;
			finished = true;
		}
		
		if (options->wasClicked())
		{
			doConfigOptions();
			mouse->setBusy(false);
			uiManager->onlyShowGroup("titleWidgets");
		}
		
		if (load->wasClicked())
		{
			if (loadGame())
			{
				rtn = SECTION_LOAD;
				finished = true;
			}
			
			mouse->setBusy(false);
			uiManager->onlyShowGroup("titleWidgets");
		}
		
		if (quit->wasClicked())
		{
			exit(0);
		}
	}
	
	audio->stopMusic();
	
	graphics->clearScreen();
	graphics->updateScreen();
	
	graphics->allowMotionBlur = true;
	
	return rtn;
}

void doIntro()
{
	float timer, fade, x, y;
	bool skip = false;

	mouse->setBusy(false);
	
	graphics->allowMotionBlur = false;
	graphics->clearScreen();
	
	List introData;
	
	loadCutscene("data/cutscenes/scripts/intro", &introData);
	
	graphics->setFont(FONT_LARGE);
	Texture *t = graphics->getGLString(_("8 Months Later..."));
	
	timer = 800;
	fade = 1.0;
	x = graphics->screen->w - 100;
	y = graphics->screen->h / 2;
	
	graphics->setMode(MODE_2D);
	
	engine->resetTimeDifference();
	
	while (timer > 0)
	{
		engine->doTimeDifference();
		engine->getInput();
		graphics->updateScreen();
		
		graphics->clearScreen();
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		if (timer <= 100)
		{
			fade += engine->getTimeDifference(TD_LOGIC) * 0.01;
		}
		else if (timer <= 700)
		{
			fade -= engine->getTimeDifference(TD_LOGIC) * 0.01;
		}
	
		graphics->blit(t, x, y, true);
		
		graphics->fade(fade);
		Math::limit(&fade, 0, 1.0);
		
		x -= 0.65 * engine->getTimeDifference(TD_LOGIC);
		timer -= engine->getTimeDifference(TD_LOGIC);
		
		if (engine->keyState[SDLK_ESCAPE])
		{
			skip = true;
			timer = 0;
		}
	}
	
	delete t;
	
	graphics->clearScreen();
	graphics->updateScreen();
	
	if (!skip)
	{
		processStaticCutscene(&introData);
	}
	
	graphics->clearScreen();
	graphics->updateScreen();
	graphics->delay(1000, false);
	graphics->allowMotionBlur = true;
}

void doCredits()
{
	Texture *credit[1024];
	Texture *final[5];
	float cy[1024];
	int lastCredit = 0;
	int pos = 0;
	int yPos = 0;
	int y = (graphics->screen->h - yPos) + 50;
	float scrollSpeed;
	char cred[1024];
	char *token;
	
	memset(credit, 0, sizeof(Texture*) * 1024);

	Texture *texture = graphics->loadTexture("gfx/game/credits.png");
	
	mouse->setBusy(true);
	
	graphics->setMode(MODE_2D);
	
	graphics->setFont(FONT_NORMAL);
	
	audio->loadMusic("music/Autumns Dawning.s3m");
	
	final[0] = graphics->getGLString("Blob Wars : Blob And Conquer");
	final[1] = graphics->getGLString(_("Copyright (c) 2008 Parallel Realities"));
	final[2] = graphics->getGLString(_("All Rights Reserved"));
	final[3] = graphics->getGLString("http://www.parallelrealities.co.uk");
	final[4] = graphics->getGLString(_("Licensed under the GNU General Public License"));
	
	pak->unpack(_("data/locale/en/credits"), PAK::TEXT);
	
	token = strtok((char*)pak->dataBuffer, "\n");
	
	while (token != NULL)
	{
		credit[lastCredit] = NULL;
		
		sscanf(token, "%d%[^\n]", &pos, cred);
			
		y += pos;
			
		credit[lastCredit] = graphics->getGLString("%s", cred);
		cy[lastCredit] = y;
	
		token = strtok(NULL, "\n");
		
		lastCredit++;
	}
	
	pak->freeAll();
	
	graphics->clearScreen();
	
	graphics->setMode(MODE_2D);
	
	// calculate how fast we must move the credits
	// so that they finish in time with the music... ;)
	scrollSpeed = cy[lastCredit - 1] + (yPos - 25);
	scrollSpeed /= (60 * 2) + 44;
	scrollSpeed /= 100;
	
	audio->playMusic();
	
	engine->resetTimeDifference();
	
	while (cy[lastCredit - 1] > (yPos - 25))
	{
		if (engine->keyState[SDLK_ESCAPE])
		{
			break;
		}
		
		engine->doTimeDifference();
		engine->getInput();
		graphics->updateScreen();
		
		graphics->clearScreen();
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		//graphics->blit(texture, graphics->screen->w - 800, yPos, false);
		renderScaledTexture(texture);
		
		for (int i = 0 ; i < lastCredit ; i++)
		{
			cy[i] -= (engine->getTimeDifference(TD_LOGIC) * scrollSpeed);
			
			if (credit[i] == NULL)
			{
				continue;
			}
			
			if ((cy[i] > -50) && (cy[i] <= graphics->screen->h + 12))
			{
				graphics->blit(credit[i], graphics->screenMidX, (int)cy[i], true);
			}
		}
		
		graphics->fade(1.0, 0, 0, graphics->screen->w, yPos);
		graphics->fade(1.0, 0, graphics->screen->h - yPos, graphics->screen->w, yPos);
	}
	
	float f1 = 0;
	float f2 = 0;
	float timer = 600;
	
	while (timer > -500)
	{
		engine->doTimeDifference();
		engine->getInput();
		graphics->updateScreen();
		
		graphics->clearScreen();
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		renderScaledTexture(texture);
		
		f1 += engine->getTimeDifference(TD_LOGIC) * 0.005;
		
		glColor4f(1.0, 1.0, 1.0, f1);
		
		graphics->blit(final[0], graphics->screenMidX, graphics->screenMidY - 80, true);
		graphics->blit(final[1], graphics->screenMidX, graphics->screenMidY - 60, true);
		graphics->blit(final[2], graphics->screenMidX, graphics->screenMidY - 40, true);
		graphics->blit(final[3], graphics->screenMidX, graphics->screenMidY + 20, true);
		graphics->blit(final[4], graphics->screenMidX, graphics->screenMidY + 128, true);
		
		timer -= engine->getTimeDifference(TD_LOGIC);
		
		if (timer <= 200)
		{
			graphics->fade(f2);
			
			f2 += engine->getTimeDifference(TD_LOGIC) * 0.0025;
		}
	}
	
	audio->stopMusic();
	
	graphics->clearScreen();
	graphics->updateScreen();
	
	for (int i = 0 ; i < lastCredit ; i++)
	{
		if (credit[i] == NULL)
		{
			continue;
		}
	
		delete credit[i];
	}
	
	delete texture;
}
