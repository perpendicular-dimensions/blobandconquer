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

#include "laserGrid.h"

Texture *buildLaserGridInstructions()
{
	pak->unpack(_("data/locale/en/laserGrid"), PAK::TEXT);
	
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
	
	textureManager->addTexture("LaserGridInstructions", instructions);
	
	return instructions;
}

bool doLaserGridPuzzle(int seed)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	graphics->clearScreen();
	graphics->delay(500, false);
	
	Texture *background = getRequiredTexture("textures/game/mastermindBackground.png");
	
	srand(seed);
	
	String s;
	Texture *ball[8];
	
	for (int i = 0 ; i < 8 ; i++)
	{
		s.setText("textures/game/mastermindBall%d.png", (i + 1));
		ball[i] = getRequiredTexture(s.getText());
	}
	
	Texture *instructions = textureManager->getTexture("LaserGridInstructions");
	if (instructions == NULL)
	{
		instructions = buildLaserGridInstructions();
	}
	
	int grid[8][8][2];
	
	int pairs = Math::rrand(4, 3 + mission->enemyLevel);
	
	if ((pairs % 2) != 0)
	{
		pairs++;
	}
	
	for (int x = 0 ; x < pairs ; x++)
	{
		for (int y = 0 ; y < pairs ; y++)
		{
			grid[x][y][0] = grid[x][y][1] = 7; 
		}
	}
	
	int color = rand() % 7;
	int gx, gy;
	int assignedPairs = pairs * pairs;
	
	while (assignedPairs > 0)
	{
		gx = rand() % pairs;
		gy = rand() % pairs;
		
		if (grid[gx][gy][0] == 7)
		{
			assignedPairs--;
			
			grid[gx][gy][0] = color;
			
			if ((assignedPairs % 2) == 0)
			{
				color = rand() % 7;
			}
		}
	}
	
	assignedPairs = (pairs * pairs) / 2;
	
	float d = pairs;
	d /= 2;
	
	int midX = (int)(275 - (d * 48)) + graphics->getOffsetX();
	int midY = (int)(300 - (d * 48)) + graphics->getOffsetY();
	
	float dy = -256;
	
	int b;
	float undoTimer = 0;
	int *guess[2];
	guess[0] = guess[1] = 0;
	char timeText[15];
	
	float timer = 250;
	
	float remainingHours = 0;
	float remainingMinutes = 0;
	float remainingSeconds = 0;
	float remaining = 0;
	float totalRemaining = 60 + ((pairs - 4) * 37.5);
	float previousRemaining = totalRemaining;
	
	int puzzleState = PUZZLE_STATE_UNFINISHED;
	
	int timeX, timeY;
	
	bool startTimer = false;
	
	String banner;
	graphics->setFont(FONT_DIGITAL);
	banner.setText("%d remaining", assignedPairs);
	Texture *pairsRemaining = graphics->getGLString("%s", banner.getText());
	textureManager->removeTexture("PairsRemaining");
	textureManager->addTexture("PairsRemaining", pairsRemaining);
	graphics->setFont(FONT_NORMAL);
	
	mouse->setBusy(false);
	
	game->doingPuzzle = true;
	
	int mx = 0;
	int my = 0;
	int tx, ty;
	
	mouse->set(midX + (mx * 48) + 16, midY + (my * 48) + 16);
	
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
		
		glColor3f(0.5, 0.5, 0.5);
		
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
		
		graphics->fade(0.75, midX - 8, midY - 8, (pairs * 48), (pairs * 48));
		graphics->outlineRect(midX - 8, midY - 8, (pairs * 48), (pairs * 48), GLColor::grey);
		
		glColor3f(1.0, 1.0, 1.0);
		
		for (int x = 0 ; x < pairs ; x++)
		{
			for (int y = 0 ; y < pairs ; y++)
			{
				b = grid[x][y][1];
				
				graphics->blit(ball[b], midX + (x * 48), midY + (y * 48), false);
				
				if (grid[x][y][1] != 7)
				{
					continue;
				}
				
				if (puzzleState != PUZZLE_STATE_UNFINISHED)
				{
					continue;
				}
				
				if (undoTimer > 0)
				{
					continue;
				}
					
				if ((game->isControl(CTRL_ACTION)) || (game->isControl(CTRL_FIRE)) || (mouse->buttonState[SDL_BUTTON_LEFT]))
				{
					if (Collision::collision(midX + (x * 48), midY + (y * 48), 32, 32, mouse->x, mouse->y, 1, 1))
					{
						startTimer = true;
						
						grid[x][y][1] = grid[x][y][0];
						
						if (!guess[0])
						{
							guess[0] = &grid[x][y][1];
						}
						else
						{
							guess[1] = &grid[x][y][1];
							
							if (*guess[0] != *guess[1])
							{
								undoTimer = 75;
							}
							else
							{
								guess[0] = guess[1] = 0;
								
								assignedPairs--;
								
								graphics->setFont(FONT_DIGITAL);
								banner.setText("%d remaining", assignedPairs);
								pairsRemaining = graphics->getGLString("%s", banner.getText());
								textureManager->removeTexture("PairsRemaining");
								textureManager->addTexture("PairsRemaining", pairsRemaining);
								graphics->setFont(FONT_NORMAL);
								
								if (assignedPairs == 0)
								{
									puzzleState = PUZZLE_STATE_COMPLETED;
								}
							}
						}
						
						game->resetControl(CTRL_FIRE);
						game->resetControl(CTRL_ACTION);
					}
				}
			}
		}
		
		if (undoTimer > 0)
		{
			undoTimer -= engine->getTimeDifference(TD_LOGIC);
					
			if (undoTimer <= 0)
			{
				*guess[0] = 7;
				*guess[1] = 7;
				
				guess[0] = guess[1] = 0;
			}
		}
		
		if (startTimer)
		{
			if (puzzleState == PUZZLE_STATE_UNFINISHED)
			{
				previousRemaining = totalRemaining;
				
				totalRemaining -= (0.01 * engine->getTimeDifference(TD_LOGIC));
				
				for (int i = 1 ; i < 11 ; i++)
				{
					if ((totalRemaining <= i) && (previousRemaining > i))
					{
						audio->playSound(SND_TIME_WARNING_2, CH_ANY, 128);
					}
				}
			
				Math::limit(&totalRemaining, 0, RAND_MAX);
			
				if (totalRemaining == 0)
				{
					puzzleState = PUZZLE_STATE_FAILED;
				}
			}
			else
			{
				timer -= engine->getTimeDifference(TD_LOGIC);
			}
		}
		
		remaining = totalRemaining;
		remaining -= (int)totalRemaining;
		remaining *= 100;
		
		game->calculateTime(totalRemaining, &remainingHours, &remainingMinutes, &remainingSeconds);
		
		sprintf(timeText, "%.2d%.2d.%.2d", (int)remainingMinutes, (int)remainingSeconds, (int)remaining);
		
		timeX = graphics->getOffsetX() + 175;
		timeY = midY + (pairs * 48) + 8;
		
		glColor3f(1.0, 1.0, 1.0);
		
		if (totalRemaining <= 10)
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		else if (totalRemaining <= 30)
		{
			glColor3f(1.0, 1.0, 0.0);
		}
		
		graphics->blit(graphics->counterDigit[(int)timeText[0] - 48], timeX += 20, timeY, false);
		graphics->blit(graphics->counterDigit[(int)timeText[1] - 48], timeX += 20, timeY, false);
		graphics->blit(graphics->counterDigit[10], timeX += 26, timeY - 2, false);
		graphics->blit(graphics->counterDigit[(int)timeText[2] - 48], timeX += 15, timeY, false);
		graphics->blit(graphics->counterDigit[(int)timeText[3] - 48], timeX += 20, timeY, false);
		graphics->blit(graphics->counterDigit[11], timeX += 22, timeY, false);
		graphics->blit(graphics->counterDigit[(int)timeText[5] - 48], timeX += 13, timeY, false);
		graphics->blit(graphics->counterDigit[(int)timeText[6] - 48], timeX += 20, timeY, false);
		
		glColor3f(1.0, 1.0, 1.0);
		
		graphics->blit(pairsRemaining, midX + 65, midY - 48, false);
		
		graphics->blit(instructions, 500 + graphics->getOffsetX(), midY, false);
		
		if (engine->keyState[SDLK_ESCAPE])
		{
			timer = 0;
		}
		
		drawWidgets();
		
		getMouseCursorMovement(&tx, &ty);
		
		if ((tx != 0) || (ty != 0))
		{
			mx += tx;
			my += ty;
			
			Math::limit(&mx, 0, pairs - 1);
			Math::limit(&my, 0, pairs - 1);
		
			mouse->set(midX + (mx * 48) + 16, midY + (my * 48) + 16);
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
