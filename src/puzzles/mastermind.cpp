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

#include "mastermind.h"

#define MM_GRID_HEIGHT	12

Texture *buildMasterMindInstructions()
{
	pak->unpack(_("data/locale/en/mastermind"), PAK::TEXT);
	
	for (unsigned int i = 0 ; i < strlen((char*)pak->dataBuffer) ; i++)
	{
		if ((isspace(pak->dataBuffer[i])))
		{
			pak->dataBuffer[i] = ' ';
		}
	}
	
	SDL_Surface *s = graphics->createSurface(250, 600);
	SDL_FillRect(s, NULL, graphics->black);
	graphics->setTransparent(s);
	graphics->setFont(FONT_NORMAL);
	graphics->setFontColor(255, 255, 255, 0, 0, 0);
	graphics->writeWrappedText(s, true, "%s", (char*)pak->dataBuffer);
	
	Texture *instructions = graphics->createTexture(s);
	
	textureManager->addTexture("MasterMindInstructions", instructions);
	
	return instructions;
}

bool doMasterMindPuzzle(int seed)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	graphics->clearScreen();
	graphics->delay(500, false);
	
	int grid[5][MM_GRID_HEIGHT];
	int answer[5];
	
	srand(seed);
	
	String s;
	Texture *ball[8];
	
	for (int i = 0 ; i < 8 ; i++)
	{
		s.setText("textures/game/mastermindBall%d.png", (i + 1));
		ball[i] = getRequiredTexture(s.getText());
	}
	
	Texture *instructions = textureManager->getTexture("MasterMindInstructions");
	if (instructions == NULL)
	{
		instructions = buildMasterMindInstructions();
	}
	
	Texture *arrow = getRequiredTexture("textures/game/mastermindArrow.png");
	Texture *background = getRequiredTexture("textures/game/mastermindBackground.png");
	
	graphics->setFont(FONT_DIGITAL);
	Texture *serialNumber = graphics->getGLString("%.9d", seed);
	textureManager->removeTexture("serialNumber");
	textureManager->addTexture("serialNumber", serialNumber);
	
	graphics->setFont(FONT_NORMAL);
	Texture *availableColors = graphics->getGLString("Available Colours");
	textureManager->removeTexture("availableColors");
	textureManager->addTexture("availableColors", availableColors);
	
	int maxColors = Math::rrand(2, mission->enemyLevel + 2);
	Math::limit(&maxColors, 1, 6);
	
	int gridX = Math::rrand(2, 1 + mission->enemyLevel);
	Math::limit(&gridX, 1, 5);
	
	for (int y = 0 ; y < MM_GRID_HEIGHT ; y++)
	{
		for (int x = 0 ; x < gridX ; x++)
		{
			grid[x][y] = 7;
		}
	}
	
	for (int i = 0 ; i < gridX ; i++)
	{
		answer[i] = rand() % maxColors;
		debug(("Mastermind Solution: %d: %d\n", i + 1, answer[i]));
	}
	
	int result[MM_GRID_HEIGHT];
	for (int i = 0 ; i < MM_GRID_HEIGHT ; i++)
	{
		result[i] = -1;
	}
	
	uiManager->onlyShowGroup("mastermind");
	
	int b;
	int gx, gy;
	int attempts = MM_GRID_HEIGHT - 1;
	bool valid;
	
	int puzzleState = PUZZLE_STATE_UNFINISHED;
	
	float timer = 250;
	float dy = -256;
	
	GLColor darkGrey;
	darkGrey.color[0] = 0.25;
	darkGrey.color[1] = 0.25;
	darkGrey.color[2] = 0.25;
	darkGrey.color[3] = 0.25;
	
	mouse->setBusy(false);
	
	game->doingPuzzle = true;
	
	int mx = 0;
	int tx, ty;
	int unsolvedX = 0, unsolvedY = 0;
	
	gx = 150 + (mx * 40) + graphics->getOffsetX();
	gy = 80 + (attempts * 40) + graphics->getOffsetY();

	mouse->set(gx, gy);
	
	while (timer > 0)
	{
		engine->doTimeDifference();
		
		graphics->updateScreen();
		engine->getInput();
		
		graphics->clearScreen();
		
		graphics->setMode(MODE_2D);
		
		#if DEV
		graphics->drawFPSCount(5);
		#endif
		
		dy += (engine->getTimeDifference(TD_LOGIC) * 0.1);
		
		if (dy >= 0)
		{
			dy = -256;
		}
		
		glColor3f(0.5, 0.5, 0.5);
		
		for (int x = 0 ; x < graphics->screen->w ; x += 256)
		{
			for (int y = 0 ; y < graphics->screen->h ; y += 256)
			{
				graphics->blit(background, x, y, false);
			}
		}
		
		glColor3f(0.35, 0.35, 0.35);
		
		for (int x = 0 ; x < graphics->screen->w ; x += 256)
		{
			for (int y = 0 ; y < graphics->screen->h + 256 ; y += 256)
			{
				graphics->blit(background, x + 10, (int)(y + dy), false);
			}
		}
		
		graphics->outlineRect(graphics->getOffsetX(), graphics->getOffsetY(), 800, 600, GLColor::white);
		
		graphics->fade(0.75, 125 + graphics->getOffsetX(), 60 + graphics->getOffsetY(), (gridX * 40) + 10, 495);
		graphics->fade(0.75, 145 + graphics->getOffsetX() + (gridX * 40), 60 + graphics->getOffsetY(), 30, 495);
		
		graphics->outlineRect(125 + graphics->getOffsetX(), 60 + graphics->getOffsetY(), (gridX * 40) + 10, 495, darkGrey);
		graphics->outlineRect(145 + graphics->getOffsetX() + (gridX * 40), 60 + graphics->getOffsetY(), 30, 495, darkGrey);
		
		for (int y = 0 ; y < MM_GRID_HEIGHT ; y++)
		{
			for (int x = 0 ; x < gridX ; x++)
			{
				b = grid[x][y];
				
				gx = 150 + (x * 40) + graphics->getOffsetX();
				gy = 80 + (y * 40) + graphics->getOffsetY();
				
				if (y != attempts)
				{
					glColor3f(0.5, 0.5, 0.5);
				}
				else
				{
					glColor3f(1.0, 1.0, 1.0);
					
					if (x == 0)
					{
						graphics->blit(arrow, gx - 40, gy, true);
					}
				}
				
				graphics->blit(ball[b], gx, gy, true);
				
				if (puzzleState == PUZZLE_STATE_UNFINISHED)
				{
					if (y != attempts)
					{
						continue;
					}
					
					if ((game->isControl(CTRL_ACTION)) || (game->isControl(CTRL_FIRE)) || (mouse->buttonState[SDL_BUTTON_LEFT]))
					{
						if (Collision::collision(gx - 16, gy - 16, 32, 32, mouse->x, mouse->y, 1, 1))
						{
							grid[x][y]++;
							Math::wrap(&grid[x][y], 0, maxColors); 
							game->resetControl(CTRL_FIRE);
							game->resetControl(CTRL_ACTION);
						}
					}
				}
			}
			
			if (result[y] != -1)
			{
				graphics->blit(graphics->counterDigit[result[y]], gx + 42, gy - 10, false);
			}
			else
			{
				graphics->blit(graphics->counterDigit[12], gx + 42, gy - 10, false);
				unsolvedX = gx + 40;
				unsolvedY = gy - 12;
			}
		}
		
		if (puzzleState == PUZZLE_STATE_UNFINISHED)
		{
			if ((game->isControl(CTRL_ACTION)) || (game->isControl(CTRL_FIRE)) || (mouse->buttonState[SDL_BUTTON_LEFT]))
			{
				if (Collision::collision(unsolvedX, unsolvedY, 20, 26, mouse->x, mouse->y, 1, 1))
				{
					// check the current row to see if it's valid...
					valid = true;
							
					for (int i = 0 ; i < gridX ; i++)
					{
						if (grid[i][attempts] == 7)
						{
							valid = false;
						}
					}
							
					if (valid)
					{
						result[attempts] = 0;
						
						for (int i = 0 ; i < gridX ; i++)
						{
							if (grid[i][attempts] == answer[i])
							{
								result[attempts]++;
							}
						}
						
						if (result[attempts] == gridX)
						{
							puzzleState = PUZZLE_STATE_COMPLETED;
							mouse->setBusy(true);
						}
									
						if (puzzleState != PUZZLE_STATE_COMPLETED)
						{
							attempts--;
							
							mouse->set(150 + (0 * 40) + graphics->getOffsetX(), 80 + (attempts * 40) + graphics->getOffsetY());
							
							mx = 0;
							
							if (attempts == -1)
							{
								puzzleState = PUZZLE_STATE_FAILED;
								mouse->setBusy(true);
							}
						}
					}
				}
			}
			
			if (engine->keyState[SDLK_ESCAPE])
			{
				timer = 0;
			}
		}
		else
		{
			timer -= engine->getTimeDifference(TD_LOGIC);
		}
		
		glColor3f(1.0, 1.0, 1.0);
		
		graphics->blit(instructions, 500 + graphics->getOffsetX(), 60 + graphics->getOffsetY(), false);
		
		graphics->blit(serialNumber, graphics->screenMidX + graphics->getOffsetX(), 25 + graphics->getOffsetY(), true);
		
		// draw the available colours,
		
		graphics->blit(availableColors, 540 + graphics->getOffsetX(), 450 + graphics->getOffsetY(), false);
		
		for (int i = 0 ; i < (maxColors + 1) ; i++)
		{
			if (i < 4)
			{
				graphics->blit(ball[i], 550 + (i * 45) + graphics->getOffsetX(), 495 + graphics->getOffsetY(), true);
			}
			else
			{
				graphics->blit(ball[i], 550 + ((i - 4) * 45) + graphics->getOffsetX(), 535 + graphics->getOffsetY(), true);
			}
		}
		
		getMouseCursorMovement(&tx, &ty);
		
		if (tx != 0)
		{
			Math::limit(&(mx += tx), 0, gridX);
			
			gx = 150 + (mx * 40) + graphics->getOffsetX();
			
			if (mx == gridX)
			{
				gx += 10;
			}
			
			mouse->set(gx, 80 + (attempts * 40));
		}
	}
	
	if ((engine->keyState[SDLK_ESCAPE]) && (player == NULL))
	{
		exit(0);
	}
	
	mouse->setBusy(true);
	
	graphics->clearScreen();
	graphics->delay(500, false);
	
	srand(time(NULL));
	
	glClearColor(mission->fogColor.color[0], mission->fogColor.color[1], mission->fogColor.color[2], mission->fogColor.color[3]);
	
	game->doingPuzzle = false;
	
	return (puzzleState == PUZZLE_STATE_COMPLETED);
}
