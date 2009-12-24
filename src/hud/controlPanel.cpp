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

#include "controlPanel.h"

void drawSlotItem(EntityDef *def, int x, int y, float scale)
{
	Vector near, far, ray;
	
	if (def->model == NULL)
	{
		return;
	}
	
	graphics->setMode(MODE_3D);
	
	glColor3f(1.0, 1.0, 1.0);
	
	graphics->convertToWorldCoords(x, y, &near, &far, &ray);
	
	float adjust = (-2 - near.z) / ray.z;
	
	near.x += (ray.x * adjust);
	near.y += (ray.y * adjust);
	near.z += -2;
	
	scale /= graphics->screen->w;
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
	glPushMatrix();
	{
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		
		glBindTexture(GL_TEXTURE_2D, def->texture[0]->data);
		
		glTranslatef(near.x, near.y, near.z);
		glRotatef(graphics->getAnimTimer() * 0.5, 1.0, 1.0, 0.0);
		
		glScalef(scale, scale, scale);
		
		glCallList(def->model[0]->data);
	}
	glPopMatrix();
}

void drawBlobHealthShieldInfo()
{
	float healthPercent;
	float healthBar, shieldBar, oxygenBar;
	int fade;
	int y, bossX;
	String s;
	GLColor c;
	Texture *t;
	
	static GLColor darkRed = GLColor::red.darker();
	static GLColor darkYellow = GLColor::yellow.darker();
	static GLColor darkCyan = GLColor::cyan.darker();
	static GLColor darkGreen = GLColor::green.darker();
	
	y = 20;
	
	bool flashBar = ((((int)graphics->getAnimTimer()) % 100) < 50);
	bool showDanger;
	
	for (Unit *unit = (Unit*)entityManager->blobList.getFirstElement() ; unit != NULL ; unit = (Unit*)unit->next)
	{
		if ((unit->health <= -100) || (unit->definition->type == BLOB_MIA) || (!unit->showHealthData))
		{
			continue;
		}
		
		switch (unit->definition->type)
		{
			case BLOB_MIA:
			case BLOB_ASSIMILATING:
			case BLOB_SOLDIER:
				continue;
				break;
					
			default:
				break;
		}
		
		showDanger = false;
		
		fade = 35;
		
		if (unit->maxShield > 0)
		{
			fade = 40;
		}
		
		if (unit->oxygen < MAX_OXYGEN_IND)
		{
			fade = 45;
		}
		
		graphics->fade(0.5, 30, y - 5, 210, fade);
		
		s.setText("str_%s", unit->getName());
		
		if ((t = textureManager->getTexture(s.getText())) == NULL)
		{
			t = graphics->getGLString("%s", unit->getName());
			textureManager->addTexture(s.getText(), t);
		}
		
		glColor3f(1.0, 1.0, 1.0);
		graphics->blit(t, 35, y, false);
		
		healthPercent = unit->getHealthPercent();
		
		healthBar = 200;
		shieldBar = 200;
		oxygenBar = 200;
		
		healthBar /= unit->maxHealth;
		shieldBar /= unit->maxShield;
		oxygenBar /= MAX_OXYGEN;
		
		healthBar *= unit->health;
		shieldBar *= unit->shield;
		oxygenBar *= unit->oxygen;
		
		y += 16;
		
		if (healthPercent >= 75)
		{
			c = darkGreen;
		}
		else if (healthPercent >= 25)
		{
			c = darkYellow;
		}
		else
		{
			c = darkRed;
		}
		
		graphics->drawRect(35, y, 200, 8, c, true);
		
		if (unit == player)
		{
			glColor4f(1.0, 1.0, 1.0, (!game->autoLockTarget) ? 1.0 : 0.15);
			graphics->blit(textureManager->getTexture("textures/game/noTarget.png"), 120, y - 18, false);
		}
		else
		{
			glColor4f(1.0, 1.0, 1.0, 1.0);
		
			if (unit->currentCrumb != NULL)
			{
				graphics->blit(textureManager->getTexture("gfx/game/crumbFollowing.png"), 120, y - 18, false);
			}
		
			if (unit->crumbLost)
			{
				graphics->blit(textureManager->getTexture("gfx/game/crumbLost.png"), 136, y - 18, false);
			}
		}
		
		if ((unit->health > 0) && (healthPercent < 25))
		{
			if (flashBar)
			{
				graphics->drawRect(35, y, (int)healthBar, 8, c, true);
			}
			
			showDanger = true;
		}
		else if (unit->health > 0)
		{
			graphics->drawRect(35, y, (int)healthBar, 8, c, true);
		}
		
		if ((unit->flags & EF_WRAPPED) || (unit->flags & EF_ONFIRE) || (unit->flags & EF_FROZEN))
		{
			showDanger = true;
		}
		
		if (showDanger)
		{
			glColor4f(1.0, 1.0, 1.0, (flashBar) ? 1.0 : 0.5);
			graphics->blit(textureManager->getTexture("textures/game/danger.png"), 185, y - 18, false);
		}
		
		y += 10;
		
		if (unit->maxShield > 0)
		{
			graphics->drawRect(35, y, 200, 4, darkCyan, true);
			
			if (unit->shield > 0)
			{
				graphics->drawRect(35, y, (int)shieldBar, 4, GLColor::cyan, true);
			}
		}
		
		y += 6;
		
		if (unit->oxygen < MAX_OXYGEN)
		{
			graphics->drawRect(35, y, 200, 4, darkYellow, true);
			
			if (unit->oxygen > 0)
			{
				graphics->drawRect(35, y, (int)oxygenBar, 4, GLColor::yellow, true);
			}
		}
		else if (unit->oxygen != MAX_OXYGEN_IND)
		{
			graphics->drawRect(35, y, 200, 4, GLColor::yellow, true);
		}
		
		if ((unit != player) && (unit->flags & EF_VANISHED))
		{
			graphics->fade(0.75, 30, y - 37, 210, fade);
		}
		
		y += 20;
	}
	
	y = 20;
	
	for (Boss *boss = (Boss*)entityManager->bossList.getFirstElement() ; boss != NULL ; boss = (Boss*)boss->next)
	{
		if ((boss->health <= 0) || (!boss->showHealthData))
		{
			continue;
		}
		
		bossX = graphics->screen->w - 240;
		
		graphics->fade(0.5, bossX, y - 5, 210, 40);
		
		s.setText("str_%s", boss->getName());
		
		if ((t = textureManager->getTexture(s.getText())) == NULL)
		{
			t = graphics->getGLString("%s", boss->getName());
			textureManager->addTexture(s.getText(), t);
		}
		
		glColor3f(1.0, 1.0, 1.0);
		graphics->blit(t, bossX + 5, y, false);
		
		healthPercent = boss->getHealthPercent();
		
		healthBar = 200;
		shieldBar = 200;
		
		healthBar /= boss->maxHealth;
		shieldBar /= boss->maxShield;
		
		healthBar *= boss->health;
		shieldBar *= boss->shield;
		
		y += 16;
		
		if (healthPercent >= 75)
		{
			c = darkGreen;
		}
		else if (healthPercent >= 25)
		{
			c = darkYellow;
		}
		else
		{
			c = darkRed;
		}
		
		graphics->drawRect(bossX + 5, y, 200, 8, c, true);
		
		if (boss->health > 0)
		{
			graphics->drawRect(bossX + 5, y, (int)healthBar, 8, c, true);
		}
		
		y += 10;
		
		if (boss->maxShield > 0)
		{
			graphics->drawRect(bossX + 5, y, 200, 4, darkCyan, true);
			
			if (boss->shield > 0)
			{
				graphics->drawRect(bossX + 5, y, (int)shieldBar, 4, GLColor::cyan, true);
			}
		}
		
		y += 26;
	}
}

void drawWeaponsAndItems()
{
	static float itemImageSwapTime = 400;
	static int itemImage = 0;
	
	int x1 = 5;
	int x2 = (graphics->screen->w - 100) - 5;
	int y1 = graphics->screen->h - 100;
	
	int adjustX = (graphics->screen->w - 800) / 35;
	int adjustY = (graphics->screen->h - 600) / 35;
	
	graphics->fade(0.75, x1, y1, 100, 95);
	graphics->outlineRect(x1, y1, 100, 95, GLColor::grey);
	
	graphics->fade(0.75, x2, y1, 100, 95);
	graphics->outlineRect(x2, y1, 100, 95, GLColor::grey);
	
	drawSlotItem(game->selectedWeapon->definition, (x1 + 40) - adjustX, (graphics->screen->h - 40) + adjustY, 15);
	
	itemImageSwapTime -= engine->getTimeDifference(TD_ANIMATION);
	
	if (game->item[itemImage] != NULL)
	{
		drawSlotItem(game->item[itemImage]->definition, (graphics->screen->w - 40) + adjustX, (graphics->screen->h - 40) + adjustY, 15);
	}
	
	if (itemImageSwapTime <= 0)
	{
		itemImageSwapTime = 0;
		
		bool okay = false;
		int previousImage = itemImage;
		
		while (!okay)
		{
			Math::wrap(&(++itemImage), 0, 4);
			
			if (game->item[itemImage] != NULL)
			{
				itemImageSwapTime = 400;
				okay = true;
			}
			
			if (previousImage == itemImage)
			{
				okay = true;
			}
		}
	}
	
	graphics->setMode(MODE_2D);
	
	String s;
	Texture *t;
	
	WeaponDef *weaponDef;
	ItemDef *itemDef;
	
	weaponDef = (WeaponDef*)game->selectedWeapon->definition;
	
	if ((!weaponDef->unlimited) && (weaponDef->type != WEP_DESIMILATOR))
	{
		if ((t = textureManager->getTexture("str_current_ammo")) == NULL)
		{
			t = graphics->getGLString("%d", game->selectedWeapon->currentAmmo);
			textureManager->addTexture("str_current_ammo", t);
		}
		
		glColor3f(1.0, 1.0, 1.0);
		graphics->blit(t, 55, graphics->screen->h - 18, true);
	}
	
	for (int i = 0 ; i < 4 ; i++)
	{
		if (game->weapon[i] != NULL)
		{
			weaponDef = (WeaponDef*)game->weapon[i]->definition;
			
			s.setText("str_weapon_%d", i);
			
			if ((t = textureManager->getTexture(s.getText())) == NULL)
			{
				if (weaponDef->unlimited)
				{
					t = graphics->getGLString("%s", game->weapon[i]->definition->getName());
				}
				else
				{
					if (game->weapon[i]->currentAmmo >= 0)
					{
						if (weaponDef->type != WEP_DESIMILATOR)
						{
							t = graphics->getGLString("%s: %d", game->weapon[i]->definition->getName(), game->weapon[i]->currentAmmo);
						}
						else
						{
							float percentage = 0;
							percentage = game->weapon[i]->currentAmmo;
							percentage /= weaponDef->clipSize;
							percentage *= 100;
							t = graphics->getGLString("%s: %d%%", game->weapon[i]->definition->getName(), (int)percentage);
						}
					}
					else
					{
						t = graphics->getGLString(_("%s (Recharging)"), game->weapon[i]->definition->getName());
					}
				}
				
				textureManager->addTexture(s.getText(), t);
			}
			
			if (game->weapon[i]->currentAmmo >= 0)
			{
				glColor3f(1.0, 1.0, 1.0);
			}
			else
			{
				glColor3f(0.0, 1.0, 1.0);
			}
			
			if (game->weapon[i]->currentAmmo == 0)
			{
				glColor3f(0.5, 0.5, 0.5);
			}
			
			if (game->weapon[i] == game->selectedWeapon)
			{
				glColor3f(0.0, 1.0, 0.0);
			}
		
			graphics->blit(t, 110, y1, false);
			
			if (weaponDef->type == WEP_DESIMILATOR)
			{
				game->weapon[i]->custom -= engine->getTimeDifference(TD_LOGIC);
				
				int previousAmmo = game->weapon[i]->currentAmmo;
				
				if ((game->weapon[i]->custom <= 0) && (game->weapon[i]->currentAmmo < weaponDef->clipSize))
				{
					game->weapon[i]->currentAmmo += 1;
					game->weapon[i]->custom = 20;
					Math::limit(&game->weapon[i]->currentAmmo, -100, weaponDef->clipSize);
					
					if ((previousAmmo <= 0) && (game->weapon[i]->currentAmmo > 0))
					{
						textureManager->removeTexture(s.getText());
						
						game->setInfoMessage(INFO_HINT, _("Desimilator has recharged"));
					}
					else if (previousAmmo < game->weapon[i]->currentAmmo)
					{
						textureManager->removeTexture(s.getText());
					}
					
					if (game->weapon[i] == game->selectedWeapon)
					{
						textureManager->removeTexture("str_current_ammo");
					}
				}
			}
		}
		
		y1 += 25;
	}
	
	y1 = graphics->screen->h - 100;
	x2 -= 5;
	
	for (int i =  0 ; i < 5 ; i++)
	{
		if (game->item[i] != NULL)
		{
			itemDef = (ItemDef*)game->item[i]->definition;
			
			s.setText("str_item_%d", i);
			
			if ((t = textureManager->getTexture(s.getText())) == NULL)
			{
				if (itemDef->type == ITM_KEYCARD)
				{
					t = graphics->getGLString("%s (%s)", itemDef->getName(), game->item[i]->getName());
				}
				else if (itemDef->collatable)
				{
					t = graphics->getGLString("%s (%d)", itemDef->getName(), game->item[i]->itemPower);
				}
				else
				{
					const char *translatedName = (itemDef->type < ITM_CUSTOM_FIRST) ?  itemDef->getName() : itemDef->getName();
					
					t = graphics->getGLString("%s", translatedName);
				}
				
				textureManager->addTexture(s.getText(), t);
			}
			
			glColor3f(1.0, 1.0, 1.0);
			
			graphics->blit(t, x2 - t->iw, y1, false);
		}
		
		y1 += 20;
	}
}

void drawMessages()
{
	static char time[9];
	static String s;
	
	Texture *t;
	
	int y1 = graphics->screen->h - 100;
	
	for (int i = 0 ; i < 4 ; i++)
	{
		if (game->infoMessage[i].life > 0)
		{
			switch (game->infoMessage[i].type)
			{
				case INFO_PICKUP:
					glColor4fv(GLColor::cyan.color);
					break;
					
				case INFO_BAD:
					glColor4fv(GLColor::red.color);
					break;
					
				case INFO_GOOD:
					glColor4fv(GLColor::green.color);
					break;
					
				case INFO_HINT:
					glColor4fv(GLColor::yellow.color);
					break;
					
				case INFO_GENERAL:
					glColor4fv(GLColor::white.color);
					break;
			}
			
			s.setText("str_message_%d", i);
			
			if (game->infoMessage[i].redraw)
			{
				textureManager->removeTexture(s.getText());
				game->infoMessage[i].redraw = false;
			}
			
			if ((t = textureManager->getTexture(s.getText())) == NULL)
			{
				t = graphics->getGLString("%s", game->infoMessage[i].message.getText());
				textureManager->addTexture(s.getText(), t);
			}
		
			graphics->blit(t, graphics->screenMidX, y1, true);
			
			y1 += 25;
			
			game->infoMessage[i].life -= engine->getTimeDifference(TD_LOGIC);
			
			if (game->infoMessage[i].life <= 0)
			{
				textureManager->removeTexture(s.getText());
			}
		}
	}
		
	if (game->gameplayMessage.life > -1)
	{
		s.setText("str_message_gameplay");
			
		if ((t = textureManager->getTexture(s.getText())) == NULL)
		{
			t = graphics->getGLString("%s", game->gameplayMessage.message.getText());
			textureManager->addTexture(s.getText(), t);
		}
		
		int x = (graphics->screen->w - t->iw) / 2;
		int y = graphics->screen->h - 135;
		int w = t->iw;
		int h = t->ih;
		
		graphics->fade(0.65, x - 10, y - 5, w + 15, h + 10);
		graphics->outlineRect(x - 10, y - 5, w + 15, h + 10, GLColor::lightGrey);
		
		if (game->gameplayMessage.type == INFO_GENERAL)
		{
			glColor3f(1.0, 1.0, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		
		glEnable(GL_BLEND);
		graphics->blit(t, x, y, false);
		
		game->gameplayMessage.life -= engine->getTimeDifference(TD_LOGIC);
			
		if (game->gameplayMessage.life <= 0)
		{
			textureManager->removeTexture(s.getText());
		}
	}
	
	if (game->totalRemaining > 0)
	{
		int x = (graphics->screen->w - 178) / 2;
		int y = 55;
		
		graphics->fade(0.65, (graphics->screen->w - 150) / 2, y - 3, 165, 28);
		graphics->outlineRect((graphics->screen->w - 150) / 2, y - 3, 165, 28, GLColor::red);
		
		glColor3f(1.0, 1.0, 1.0);
		
		if (game->totalRemaining <= 10)
		{
			glColor3f(1.0, 0.0, 0.0);
		}
		else if (game->totalRemaining <= 30)
		{
			glColor3f(1.0, 1.0, 0.0);
		}
		
		float remaining = game->totalRemaining;
		remaining -= (int)game->totalRemaining;
		remaining *= 100;
		
		sprintf(time, "%.2d%.2d.%.2d", (int)game->remainingMinutes, (int)game->remainingSeconds, (int)remaining);
		time[8] = '\0';
		
		graphics->blit(graphics->counterDigit[(int)time[0] - 48], x += 20, y, false);
		graphics->blit(graphics->counterDigit[(int)time[1] - 48], x += 20, y, false);
		graphics->blit(graphics->counterDigit[10], x += 26, y - 2, false);
		graphics->blit(graphics->counterDigit[(int)time[2] - 48], x += 15, y, false);
		graphics->blit(graphics->counterDigit[(int)time[3] - 48], x += 20, y, false);
		graphics->blit(graphics->counterDigit[11], x += 22, y, false);
		graphics->blit(graphics->counterDigit[(int)time[5] - 48], x += 13, y, false);
		graphics->blit(graphics->counterDigit[(int)time[6] - 48], x += 20, y, false);
	}
}

void drawOrders()
{
	static const int SPACER = 28;
	static const int iconY = 85;

	int iconX = (graphics->screen->w - 100);

	if ((entityManager->blobList.getSize() == 1) || (entityManager->bossList.getSize() > 0))
	{
		return;
	}
	
	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST); 
	glAlphaFunc(GL_GREATER, 0);
	
	(game->currentOrders == ORDERS_FOLLOW) ? glColor3f(1.0, 1.0, 1.0) : glColor3f(0.25, 0.25, 0.25);
	graphics->blit(textureManager->getTexture("gfx/game/orderIcon1.tga"), iconX - SPACER, iconY, true);
	
	(game->currentOrders == ORDERS_WAIT) ? glColor3f(1.0, 1.0, 1.0) : glColor3f(0.25, 0.25, 0.25);
	graphics->blit(textureManager->getTexture("gfx/game/orderIcon2.tga"), iconX + SPACER, iconY, true);
	
	(game->currentOrders == ORDERS_USE) ? glColor3f(1.0, 1.0, 1.0) : glColor3f(0.25, 0.25, 0.25);
	graphics->blit(textureManager->getTexture("gfx/game/orderIcon3.tga"), iconX, iconY - SPACER, true);
	
	(game->currentOrders == ORDERS_RETREAT) ? glColor3f(1.0, 1.0, 1.0) : glColor3f(0.25, 0.25, 0.25);
	graphics->blit(textureManager->getTexture("gfx/game/orderIcon4.tga"), iconX, iconY + SPACER, true);
	
	glDisable(GL_ALPHA_TEST);
}

void drawOnScreenTargetter()
{
	if (game->autoLockTarget)
	{
		return;
	}
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glColor4fv(game->aimColor.color);
	graphics->blit(textureManager->getTexture("textures/game/manualTargetter.png"), graphics->screenMidX, graphics->screenMidY, true);
}

void drawInfoPanel()
{
	//graphics->drawString(graphics->screen->w / 2, 5, TXT_CENTERED, GLColor::white, true, "%s", player->velocity.toString());

	graphics->setFont(FONT_NORMAL);
	
	drawBlobHealthShieldInfo();
	
	drawWeaponsAndItems();
	
	drawMessages();
	
	drawOrders();
	
	drawOnScreenTargetter();
}
