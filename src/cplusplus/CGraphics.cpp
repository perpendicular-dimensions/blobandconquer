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

#ifdef __unix__
#include <GL/glx.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL.h>
#endif

#ifndef __unix__
extern char *strtok_r(char *s1, const char *s2, char **lasts);
#include <stdint.h>
#define drand48 rand
#endif

void SDL_SetAlpha(SDL_Surface *surface, uint8_t value) {
	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
	SDL_SetSurfaceAlphaMod(surface, value);
}

Graphics::Graphics()
{
	fontIndex = 0;
	
	acc = 0;

	screenShotNumber = 0;
	
	strcpy(tmpString, "");

	engine = Engine::getInstance();
	pak = Pak::getInstance();
	
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE);
	
	textWidth = textHeight = 0;
	
	allowMotionBlur = true;
	
	for (unsigned i = 0 ; i < MAX_RESOLUTIONS ; i++)
	{
		screenMode[i].x = screenMode[i].y = 0;
	}
	
	screenMode[0].w = 800;
	screenMode[0].h = 600;
	
	animTimer = 0;
	
	wireframe = false;
	
	lightMaps = true;
	
	currentScreenResolution = 0;
	
	redPulseDir = 1;
	redPulse = 0;
	
	motionBlur = 100;
	
	gamma = 100;
	
	for (int i = 0 ; i < 13 ; i++)
	{
		counterDigit[i] = NULL;
	}
}

Graphics::~Graphics()
{
	destroy();
}

void Graphics::calculateScreenModes() {
	SDL_DisplayMode mode;
	SDL_GetDesktopDisplayMode(0, &mode);

	fprintf(stderr, "Desktop video res: %d x %d\n", mode.w, mode.h);
	
	//Use SDL do detect available resolutions
	int rescount=SDL_GetNumDisplayModes(0);
	
	int n = 1;
	
	//Fall back to old method if SDL one fails
	if(rescount < 0)
	{
		screenMode[0].w = 800;
		screenMode[0].h = 600;
		for(int i = 10; i >= 1; i--) {
			int w = mode.w / i;
			int h = mode.h / i;

			// Skip anything smaller than 800 x 600
			if(w == 800 && h == 600)
				continue;

			if(w < 800 || h < 600)
				continue;

			// Skip resolutions that are not integer divisions
			if (w * i != mode.w || h * i != mode.h)
				continue;

			screenMode[n].w = w;
			screenMode[n].h = h;
			n++;
		}
	}
	else
	{
		n=0;
		SDL_DisplayMode prevmode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
		for(int i=rescount-1; i>=0 && n<MAX_RESOLUTIONS; --i)
		{
			SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

			if (SDL_GetDisplayMode(0, i, &mode) == 0)
			{
				//Skip resolutions lower than 800x600 and filter out duplicate resolutions (different only by refresh rate)
				if((mode.w>=800) && (mode.h>=600) && ((!n) || (prevmode.w != mode.w) || (prevmode.h != mode.h)))
				{
					screenMode[n].w=mode.w;
					screenMode[n].h=mode.h;
					prevmode=mode;
					++n;
				}
			}
		}
	}
}

Graphics *Graphics::getInstance()
{
	return &instance;
}

void Graphics::destroy()
{
	for (int i = 0 ; i < 5 ; i++)
	{
		if (font[i] != NULL)
		{
			TTF_CloseFont(font[i]);
			font[i] = NULL;
		}
	}
	
	for (int i = 0 ; i < 13 ; i++)
	{
		if (counterDigit[i] != NULL)
		{
			delete counterDigit[i];
			counterDigit[i] = NULL;
		}
	}

	if (glScreenshot != NULL)
	{
		SDL_FreeSurface(glScreenshot);
		glScreenshot = NULL;
	}
	
	if (scratchSurface != NULL)
	{
		SDL_FreeSurface(scratchSurface);
		scratchSurface = NULL;
	}
}

void Graphics::setMode(int mode)
{
	if (mode == MODE_3D)
	{
		glViewport(0, 0, screen->w, screen->h);
	
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		gluPerspective(45.0f, (GLfloat) screen->w / (GLfloat) screen->h, 0.1f, 9000.0f);
		
		// AA using the accumulation buffer

		static GLfloat dx[100] = {-0.5, 0.5, -0.5, 0.5, 0, -0.5, 0.5, 0, 0, 0}; 
		static GLfloat dy[100] = {-0.5, 0.5, 0.5, -0.5, 0, 0, 0, -0.5, 0.5, 0};

		if(!dx[9]) {
			for(int i = 9; i < 100; i++) {
				dx[i] = drand48() - 1.0;
				dy[i] = drand48() - 1.0;
			}
		}

		GLfloat m[16];
		glGetFloatv(GL_PROJECTION_MATRIX, m);
		m[8] = dx[acc % 100]/(GLfloat)screen->w;
		m[9] = dy[acc % 100]/(GLfloat)screen->h;
		glLoadMatrixf(m);
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glViewport(0, 0, screen->w, screen->h);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	
		glOrtho(0.0f, screen->w, screen->h, 0.0f, -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		glEnable(GL_BLEND);
		
		glDisable(GL_DEPTH_TEST);
	}
}

void Graphics::queryStencilSupport()
{
#ifdef __unix__
	SDL_SysWMinfo wm_info;
	
	SDL_VERSION(&wm_info.version);

	if (SDL_GetWindowWMInfo(window, &wm_info) && wm_info.subsystem == SDL_SYSWM_X11)
	{
		XVisualInfo theTemplate;  
		XVisualInfo *visuals;
		int i, numVisuals, visualCaveat, stencilSize;
		long mask = VisualScreenMask;
		Display *dpy = wm_info.info.x11.display;
		
		theTemplate.screen = 0;
		visuals = XGetVisualInfo(dpy, mask, &theTemplate, &numVisuals);
		
		hasStencils = false;
		for(i = 0; i < numVisuals; i++)
		{
			const char *ext = glXQueryExtensionsString(dpy, visuals[i].screen);
			
			glXGetConfig(dpy, &visuals[i], GLX_STENCIL_SIZE, &stencilSize);
			
			if (ext && strstr(ext, "GLX_EXT_visual_rating")) {
				glXGetConfig(dpy, &visuals[i], GLX_VISUAL_CAVEAT_EXT, &visualCaveat);
			}
			else {
				visualCaveat = -1; /* unknown */
			}
			
			if (stencilSize && (visualCaveat == GLX_NONE_EXT)) {
				hasStencils = true;
				break;
			}
		}

		debug(("Query stencil support: has stencils: %d\n", (int)hasStencils));
	}
	else
	{
		printf("Warning: Couldn't determine the availability of stencil buffers (no X11 display),\n");
		printf("  assuming they are available\n");
		hasStencils = true;
	}
#else
	/* assume stencils are available on other platforms */
	hasStencils = true;
#endif
}

void Graphics::setResolution(int i)
{
	Math::limit(&i, 0, MAX_RESOLUTIONS - 1);

	debug(("Graphics::setResolution() - %d: %d x %d\n", i, screenMode[i].w, screenMode[i].h));

	SDL_SetWindowSize(window, screenMode[i].w, screenMode[i].h);
	screen = SDL_GetWindowSurface(window);

	currentScreenResolution = i;

	screenMidX = screen->w / 2;
	screenMidY = screen->h / 2;

	debug(("Graphics::setResolution() - Done\n"));
}

static int swapframes = 0;

void Graphics::resetFPSCount()
{
	frames = 0;
	swapframes = 0;
	time = 0.0;
	fps = "N/A";
}

void Graphics::drawFPSCount(int y)
{
	if (!engine->showFPS)
	{
		//return;
	}
	
	++frames;
	
	time -= engine->getTimeDifference(TD_ANIMATION);
			
	if (time <= 0)
	{
		fps.setText("%d * %3.1f fps", swapframes, (float)frames / swapframes);
		time = 100;
		frames = 0;
		swapframes = 0;
	}
	
	drawString(screen->w - 11, y, TXT_RIGHT, GLColor::white, true, "%s", fps.getText());
}

void Graphics::showLoadingProgress(const char *status)
{
	setMode(MODE_2D);
	
	drawRect(0, 0, screen->w, screen->h, GLColor::black, false);
	
	drawString(5, screen->h - 20, TXT_LEFT, GLColor::white, true, "%s", status);
	
	updateScreen();
}

float Graphics::getAnimTimer()
{
	return animTimer;
}

void Graphics::setGamma(float value)
{
	gamma = value;
	Uint16 ramp[256];
	SDL_CalculateGammaRamp(value / 100, ramp);
	SDL_SetWindowGammaRamp(window, ramp, ramp, ramp);
}

void Graphics::mapColors()
{
	debug(("Graphics::mapColors\n"));
	
	scratchSurface = createSurface(640, 480);
	
	fontWhite.r = fontWhite.g = fontWhite.b = 0xff;
	fontBlack.r = fontBlack.g = fontBlack.b = 0xaa;
	
	red = SDL_MapRGB(scratchSurface->format, 0xff, 0x00, 0x00);
	yellow = SDL_MapRGB(scratchSurface->format, 0xff, 0xff, 0x00);
	green = SDL_MapRGB(scratchSurface->format, 0x00, 0xff, 0x00);
	darkGreen = SDL_MapRGB(scratchSurface->format, 0x00, 0x77, 0x00);
	skyBlue = SDL_MapRGB(scratchSurface->format, 0x66, 0x66, 0xff);
	blue = SDL_MapRGB(scratchSurface->format, 0x00, 0x00, 0xff);
	cyan = SDL_MapRGB(scratchSurface->format, 0x00, 0x99, 0xff);
	white = SDL_MapRGB(scratchSurface->format, 0xff, 0xff, 0xff);
	lightGrey = SDL_MapRGB(scratchSurface->format, 0xcc, 0xcc, 0xcc);
	grey = SDL_MapRGB(scratchSurface->format, 0x88, 0x88, 0x88);
	darkGrey = SDL_MapRGB(scratchSurface->format, 0x33, 0x33, 0x33);
	black = SDL_MapRGB(scratchSurface->format, 0x00, 0x00, 0x00);
}

int Graphics::getColor(int red, int green, int blue)
{
	return SDL_MapRGBA(scratchSurface->format, red, green, blue, 0xff);
}

void Graphics::clearScreen()
{
	if (hasStencils)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Graphics::updateScreenMB()
{
	static const int maxacc = 256;
	bool flush = false;
	static uint32_t lastflush = 0;
	uint32_t now = SDL_GetTicks();

	acc++;

	// Decide to flush accumulation buffer if another frame would make us miss the next vblank.
	if (now - lastflush >= 1000 / motionBlur || acc >= maxacc)
	{
		flush = true;
	}

	// Accumulate this frame unless it's the first frame and we're flushing.
	if (!flush || acc > 1)
	{
		glAccum(acc > 1 ? GL_ACCUM : GL_LOAD, 1.0f / maxacc);
	}

	if (flush)
	{
		if (acc > 1)
		{
			glAccum(GL_RETURN, (float)maxacc / (float)acc);
		}
		acc = 0;
		lastflush = now;
		swapframes++;
		SDL_GL_SwapWindow(window);
	}
	//SDL_Delay(1);

	if ((engine->keyState[SDL_SCANCODE_F10]) || ((engine->keyState[SDL_SCANCODE_RETURN]) && (engine->keyState[SDL_SCANCODE_LALT])))
	{
		fullscreen = !fullscreen;
		SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

		engine->keyState[SDL_SCANCODE_F10] = engine->keyState[SDL_SCANCODE_LALT] = engine->keyState[SDL_SCANCODE_RETURN] = 0;
		
		engine->resetTimeDifference();
	}
	
	Math::wrap(&(animTimer += engine->getTimeDifference(TD_ANIMATION)), 0, RAND_MAX);
}

void Graphics::updateScreen()
{
	if (allowMotionBlur && motionBlur < 100)
	{
		updateScreenMB();
		return;
	}
	
	SDL_GL_SwapWindow(window);
	SDL_Delay(1);

	if ((engine->keyState[SDL_SCANCODE_F10]) || ((engine->keyState[SDL_SCANCODE_RETURN]) && (engine->keyState[SDL_SCANCODE_LALT])))
	{
		fullscreen = !fullscreen;
		SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

		engine->keyState[SDL_SCANCODE_F10] = engine->keyState[SDL_SCANCODE_LALT] = engine->keyState[SDL_SCANCODE_RETURN] = 0;
		
		engine->resetTimeDifference();
	}
	
	Math::wrap(&(animTimer += engine->getTimeDifference(TD_ANIMATION)), 0, RAND_MAX);
}

void Graphics::delay(int time, bool allowSkip)
{
	unsigned long then = SDL_GetTicks();

	engine->keyState[SDL_SCANCODE_ESCAPE] = 0;
	
	while (true)
	{
		if (SDL_GetTicks() >= then + time)
		{
			break;
		}

		engine->getInput();
		
		if (allowSkip)
		{
			if (engine->keyState[SDL_SCANCODE_ESCAPE])
			{
				break;
			}
		}
	}
	
	engine->resetTimeDifference();
}

void Graphics::drawBackground(Texture *texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->data);
	
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(screen->w, 0);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(screen->w, screen->h);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0, screen->h);
	}
	glEnd();
}

int Graphics::getOffsetX()
{
	return (screen->w - 800) / 2;
}

int Graphics::getOffsetY()
{
	return (screen->h - 600) / 2;
}

Texture *Graphics::loadTexture(const char *filename)
{
	debug(("Graphics::loadTexture() - %s\n", filename));
	
	SDL_Surface *image = NULL;
	
	if (!pak->unpack(filename, PAK::BINARY))
	{
		showErrorAndExit(ERR_FILE, filename);
	}
	
	if (strstr(filename, ".tga"))
	{
		char type[] = "TGA";
		image = IMG_LoadTyped_RW(pak->sdlrw, true, type);
		
		return createTextureFromTGA(image);
	}
	
	image = IMG_Load_RW(pak->sdlrw, true);

	if (!image)
	{
		showErrorAndExit(ERR_FILE, filename);
	}
	
	return createTexture(image);
}

void Graphics::fade(float amount, int x, int y, int w, int h)
{
	blackFade.set(0, 0, 0, amount);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawRect(x, y, w, h, blackFade, true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void Graphics::fade(float amount)
{
	fade(amount, 0, 0, screen->w, screen->h);
}

void Graphics::loadFont(int i, const char *filename, int size)
{
	if (i >= MAX_FONTS)
	{
		printf("WARNING - Graphics::loadFont() - %d exceeds %d\n", i, MAX_FONTS);
		return;
	}
	
	debug(("Graphics::loadFont - %s\n", filename));
	
	if (font[i])
	{
		debug(("Freeing Font %d first...\n", i));
		TTF_CloseFont(font[i]);
	}
	
	if (!pak->unpack(filename, PAK::BINARY))
	{
		showErrorAndExit(ERR_FILE, filename);
	}
	
	pak->writeToTempFile();
	
	font[i] = TTF_OpenFont(pak->tmpFilename.getText(), size);

	if (!font[i])
	{
		printf("Unable to load font '%s'\n", filename);
		exit(1);
	}
	
	TTF_SetFontStyle(font[i], TTF_STYLE_NORMAL);
}

void Graphics::loadTimerDigits()
{
	String filename;
	
	for (int i = 0 ; i < 10 ; i++)
	{
		filename.setText("gfx/game/%d.png", i);
		counterDigit[i] = loadTexture(filename.getText());
	}
	
	filename.setText("gfx/game/colon.png");
	counterDigit[10] = loadTexture(filename.getText());
	
	filename.setText("gfx/game/dot.png");
	counterDigit[11] = loadTexture(filename.getText());
	
	filename.setText("gfx/game/question.png");
	counterDigit[12] = loadTexture(filename.getText());
}

void Graphics::setFont(int fontIndex)
{
	this->fontIndex = fontIndex;
	Math::limit(&this->fontIndex, 0, MAX_FONTS - 1);
}

Vector Graphics::convertToWorldCoords(GLfloat winX, GLfloat winY, Vector *near, Vector *far, Vector *dir)
{
	Vector result;
	
	GLdouble rx, ry, rz;
	GLdouble nx, ny, nz;
	GLdouble fx, fy, fz;
	
	GLfloat winZ;
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	winY = viewport[3] - winY;
	
	glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &rx, &ry, &rz);
	
	result.set(rx, ry, rz);
	
	if ((near == NULL) || (far == NULL) || (dir == NULL))
	{
		return result;
	}
		
	gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &nx, &ny, &nz);
	gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &fx, &fy, &fz);
	
	near->set(nx, ny, nz);
	far->set(fx, fy, fz);
	
	*dir = *far;
		
	*dir -= *near;
		
	dir->normalize();
	
	return result;
}

Point Graphics::convertToScreenCoords(Vector position)
{
	static Point result;
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	GLdouble x, y, z;
	
	gluProject(position.x, position.y, position.z, modelview, projection, viewport, &x, &y, &z);
	
	y = screen->h - y;
	
	result.set((int)x, (int)y);
	
	return result;
}

void Graphics::setTransparent(SDL_Surface *sprite)
{
	SDL_SetColorKey(sprite, true, SDL_MapRGB(sprite->format, 0, 0, 0));
}

Texture *Graphics::createTexture(SDL_Surface *surface)
{
	int w = 1;
	int h = 1;
	
	while (w < surface->w) w <<= 1;
	while (h < surface->h) h <<= 1;
	
	SDL_Surface *tmp = createSurface(w, h);
	
	SDL_BlitSurface(surface, NULL, tmp, NULL);
		
	GLuint glTexture;
	
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_2D, glTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmp->w, tmp->h, GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
	
	Texture *texture = new Texture();
	texture->data = glTexture;
	texture->iw = surface->w;
	texture->ih = surface->h;
	texture->tw = tmp->w;
	texture->th = tmp->h;
	
	SDL_FreeSurface(tmp);
	
	SDL_FreeSurface(surface);
	
	return texture;
}

Texture *Graphics::createTextureFromTGA(SDL_Surface *surface)
{
	GLuint glTexture;
	
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_2D, glTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	Texture *texture = new Texture();
	
	texture->tw  = surface->w;
	texture->th = surface->h;
	texture->data = glTexture;
	texture->iw = texture->tw;
	texture->ih = texture->th;
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
	
	SDL_FreeSurface(surface);
	
	return texture;
}

void Graphics::blit(Texture *texture, bool centered)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->data);
	
	if (centered)
	{
		glTranslatef(-(texture->iw / 2), -(texture->ih / 2), 0);
	}
	
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(texture->tw, 0);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(texture->tw, texture->th);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0, texture->th);
	}
	glEnd();
}

void Graphics::blit(Texture *texture, int x, int y, bool centered)
{
	glPushMatrix();
	{
		glTranslatef(x, y, 0);
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->data);
		
		if (centered)
		{
			glTranslatef(-(texture->iw / 2), -(texture->ih / 2), 0);
		}
		
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(texture->tw, 0);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(texture->tw, texture->th);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0, texture->th);
		}
		glEnd();
	}
	glPopMatrix();
}

Texture *Graphics::getGLString(const char *in, ...)
{
	va_list argp;
	va_start(argp, in);
	vsnprintf(tmpString, sizeof(tmpString), in, argp);
	va_end(argp);
	
	SDL_Surface *text = TTF_RenderUTF8_Blended(font[fontIndex], tmpString, fontWhite);

	if (!text)
	{
		text = TTF_RenderUTF8_Blended(font[fontIndex], "FONT_ERROR", fontWhite);
	}
	
	//SDL_SetAlpha(text, 0);
	
	Texture *texture = createTexture(text);
	
	return texture;
}

void Graphics::drawString(int x, int y, int alignment, GLColor color, bool transparent, const char *in, ...)
{
	if (strcmp(in, "") == 0)
	{
		return;
	}
	
	va_list argp;
	va_start(argp, in);
	vsnprintf(tmpString, sizeof(tmpString), in, argp);
	va_end(argp);
	
	SDL_Surface *text = TTF_RenderUTF8_Blended(font[fontIndex], tmpString, fontWhite);

	if (!text)
	{
		text = TTF_RenderUTF8_Blended(font[fontIndex], "FONT_ERROR", fontWhite);
	}
	
	if (alignment == TXT_RIGHT)
	{
		x -= text->w;
	}
	else if (alignment == TXT_CENTERED)
	{
		x -= (text->w / 2);
	}
	
	Texture *texture = createTexture(text);
	
	(transparent) ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->data);
	glColor4fv(color.color);
	
	glPushMatrix();
	{
		glTranslatef(x, y, 0);
		
		blit(texture, false);
	}
	glPopMatrix();
	
	glFinish();
	
	delete texture;
}

void Graphics::drawString(int x, int y, int alignment, SDL_Surface *dest, const char *in, ...)
{
	if (strcmp(in, "") == 0)
	{
		return;
	}
	
	va_list argp;
	va_start(argp, in);
	vsnprintf(tmpString, sizeof(tmpString), in, argp);
	va_end(argp);
	
	SDL_Surface *text = TTF_RenderUTF8_Blended(font[fontIndex], tmpString, fontForeground);

	if (!text)
	{
		text = TTF_RenderUTF8_Blended(font[fontIndex], "FONT_ERROR", fontForeground);
	}
	
	if (alignment == TXT_RIGHT)
	{
		x -= text->w;
	}
	else if (alignment == TXT_CENTERED)
	{
		x -= (text->w / 2);
	}
	
	blit(text, x, y, dest, false);
	
	SDL_FreeSurface(text);
}

void Graphics::writeWrappedText(SDL_Surface *surface, bool transparent, const char *text, ...)
{
	// we need to special one here
	char tmpString[4096];
	char *store = NULL;
	
	va_list argp;
	va_start(argp, text);
	vsnprintf(tmpString, sizeof(tmpString), text, argp);
	va_end(argp);
	
	textWidth = 0;
	textHeight = 0;

	char *word = strtok_r(tmpString, " ", &store);

	int x = 0;
	int y = 0;

	SDL_Surface *wordSurface;
	
	int wordSurfaceHeight = 0;

	while (word)
	{
		wordSurface = getString(transparent, "%s ", word);

		if (x + wordSurface->w > surface->w)
		{
			y += (int)(wordSurface->h + 3);
			x = 0;
		}

		blit(wordSurface, x, y, surface, false);

		x += wordSurface->w;
		textWidth = max(x, textWidth);

		word = strtok_r(NULL, " ", &store);
		
		wordSurfaceHeight = wordSurface->h;
		
		SDL_FreeSurface(wordSurface);
	}

	textHeight = y + wordSurfaceHeight;
}

void Graphics::drawRect(int x, int y, int w, int h, GLColor color, bool transparent)
{
	glPushMatrix();
	{
		(transparent) ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		
		glTranslatef(x, y, 0);
		
		glColor4fv(color.color);
		
		glBegin(GL_QUADS);
		{
			glVertex2d(0, 0);
			glVertex2d(w, 0);
			glVertex2d(w, h);
			glVertex2d(0, h);
		}
		glEnd();
	}
	glPopMatrix();
}

void Graphics::drawRect(int x, int y, int w, int h, GLColor color, GLColor border, bool transparent)
{
	glPushMatrix();
	{
		(transparent) ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		
		glTranslatef(x, y, 0);
		
		glColor4fv(border.color);
		
		glBegin(GL_QUADS);
		{
			glVertex2d(0, 0);
			glVertex2d(w, 0);
			glVertex2d(w, h);
			glVertex2d(0, h);
		}
		glEnd();
		
		glColor4fv(color.color);
		
		glBegin(GL_QUADS);
		{
			glVertex2d(1, 1);
			glVertex2d(w - 1, 1);
			glVertex2d(w - 1, h - 1);
			glVertex2d(1, h - 1);
		}
		glEnd();
	}
	glPopMatrix();
}

void Graphics::outlineRect(int x, int y, int w, int h, GLColor color)
{
	drawLine(x, y, x + w + 1, y, color);
	drawLine(x + w, y, x + w + 1, y + h + 1, color);
	drawLine(x, y + h, x + w + 1, y + h + 1, color);
	drawLine(x, y, x, y + h + 1, color);
}

void Graphics::putPixel(int x, int y, Uint32 pixel, SDL_Surface *dest)
{
	if ((x < 0) || (x > dest->w - 1) || (y < 0) || (y > dest->h - 1))
	{
		return;
	}

	int bpp = dest->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)dest->pixels + y * dest->pitch + x * bpp;

	switch(bpp)
	{
		case 1:
			*p = pixel;
			break;

		case 2:
			*(Uint16 *)p = pixel;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;

		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}

Uint32 Graphics::getPixel(SDL_Surface *surface, int x, int y)
{
	if ((x < 0) || (x > (surface->w - 1)) || (y < 0) || (y > (surface->h - 1)))
	{
		return 0;
	}

	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			return *p;

		case 2:
			return *(Uint16 *)p;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				return p[0] << 16 | p[1] << 8 | p[2];
			}
			else
			{
				return p[0] | p[1] << 8 | p[2] << 16;
			}

		case 4:
			return *(Uint32 *)p;

		default:
			return 0;       /* shouldn't happen, but avoids warnings */
	}
}

SDL_Surface *Graphics::getString(bool transparent, const char *in, ...)
{
	va_list argp;
	va_start(argp, in);
	vsnprintf(tmpString, sizeof(tmpString), in, argp);
	va_end(argp);
	
	SDL_Surface *text = TTF_RenderUTF8_Shaded(font[fontIndex], tmpString, fontForeground, fontBackground);

	if (!text)
	{
		text = TTF_RenderUTF8_Shaded(font[fontIndex], "FONT_ERROR", fontForeground, fontBackground);
	}

	if (transparent)
	{
		setTransparent(text);
	}

	return text;
}

void Graphics::drawLine(int x1, int y1, int x2, int y2, GLColor color)
{
	glPushMatrix();
	{
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		
		glColor4fv(color.color);
		
		glLineWidth(1.0);
		
		glBegin(GL_LINES);
			glVertex3f(x1, y1, 0);
			glVertex3f(x2, y2, 0);
		glEnd();
		
		glEnable(GL_BLEND);
	}
	glPopMatrix();
}

void Graphics::drawLine(int x1, int y1, int x2, int y2, int color, SDL_Surface *dest)
{
	int deltax = abs(x2 - x1);
	int deltay = abs(y2 - y1);
	int x = x1;
	int y = y1;
	
	int xinc1, xinc2, yinc1, yinc2;
	int den, num, numadd, numpixels, curpixel;
	
	if (x2 >= x1)
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else
	{
		xinc1 = -1;
		xinc2 = -1;
	}
	
	if (y2 >= y1)
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else
	{
		yinc1 = -1;
		yinc2 = -1;
	}
	
	if (deltax >= deltay)
	{
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else
	{
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}
	
	lock(dest);
	
	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		putPixel(x, y, color, dest);
		num += numadd;
		
		if (num >= den)
		{
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		
		x += xinc2;
		y += yinc2;
	}
	
	unlock(dest);
}

void Graphics::drawCircle(int x1, int y1, int r, SDL_Surface *dest, int color)
{
	putPixel(x1, y1, color, dest);

	for (int x = 0; x <= r / 1.4; x++)
	{
		for (int y = (int)(0.5 + sqrt((r * r) - (x * x))) ; y > 0 ; --y)
		{
			putPixel(x1 + x, y1 + y, color, dest);
			putPixel(x1 + x, y1 + -1 * y, color, dest);
			putPixel(x1 + -1 * x, y1 + -1 * y, color, dest);
			putPixel(x1 + -1 * x, y1 + y, color, dest);
			putPixel(x1 + y, y1 + x, color, dest);
			putPixel(x1 + y, y1 + -1 * x, color, dest);
			putPixel(x1 + -1 * y, y1 + -1 * x, color, dest);
			putPixel(x1 + -1 * y, y1 + x, color, dest);
		}
	}
}

void Graphics::blit(SDL_Surface *image, int x, int y, SDL_Surface *dest, bool centered)
{
	if (!image)
	{
		showErrorAndExit("graphics::blit() - NULL pointer");
	}

	if ((x < -image->w) || (x > screen->w + image->w))
	{
		return;
	}

	if ((y < -image->h) || (y > screen->h + image->h))
	{
		return;
	}

	// Set up a rectangle to draw to
	gRect.x = x;
	gRect.y = y;
	if (centered)
	{
		gRect.x -= (image->w / 2);
		gRect.y -= (image->h / 2);
	}

	gRect.w = image->w;
	gRect.h = image->h;

	/* Blit onto the screen surface */
	if (SDL_BlitSurface(image, NULL, dest, &gRect) < 0)
	{
		showErrorAndExit("graphics::blit() - %s", SDL_GetError());
	}
}

void Graphics::drawRect(int x, int y, int w, int h, int color, SDL_Surface *dest)
{
	gRect.x = x;
	gRect.y = y;
	gRect.w = w;
	gRect.h = h;

	SDL_FillRect(dest, &gRect, color);
}

void Graphics::drawRect(int x, int y, int w, int h, int color, int borderColor, SDL_Surface *dest)
{
	drawRect(x, y, w, h, borderColor, dest);
	drawRect(x + 1, y + 1, w - 2, h - 2, color, dest);
}

void Graphics::createLineBorder(SDL_Surface *surface, int color)
{
	drawLine(0, 0, surface->w - 1, 0, color, surface);
	drawLine(0, 0, 0, surface->h - 1, color, surface);
	drawLine(0, surface->h - 1, surface->w - 1, surface->h - 1, color, surface);
	drawLine(surface->w - 1, 0, surface->w - 1, surface->h - 1, color, surface);
}

void Graphics::createRaisedBorder(SDL_Surface *surface, int light, int shade)
{
	drawLine(0, 0, surface->w - 1, 0, light, surface);
	drawLine(0, 0, 0, surface->h - 1, light, surface);
	drawLine(0, surface->h - 1, surface->w - 1, surface->h - 1, shade, surface);
	drawLine(surface->w - 1, 0, surface->w - 1, surface->h - 1, shade, surface);
}

void Graphics::createLoweredBorder(SDL_Surface *surface, int light, int shade)
{
	drawLine(0, 0, surface->w - 1, 0, shade, surface);
	drawLine(0, 0, 0, surface->h - 1, shade, surface);
	drawLine(0, surface->h - 1, surface->w - 1, surface->h - 1, light, surface);
	drawLine(surface->w - 1, 0, surface->w - 1, surface->h - 1, light, surface);
}

void Graphics::drawGradient(SDL_Surface *surface, float r1, float g1, float b1, float r2, float g2, float b2)
{
	float changeR = (r2 - r1) / surface->h;
	float changeG = (g2 - g1) / surface->h;
	float changeB = (b2 - b1) / surface->h;

	int color = 0;

	for (int y = 0 ; y < surface->h ; y++)
	{
		color = getColor((int)r1, (int)g1, (int)b1);

		drawLine(0, y, surface->w - 1, y, color, surface);

		r1 += changeR;
		g1 += changeG;
		b1 += changeB;
	}
}

void Graphics::setFontColor(int red, int green, int blue, int red2, int green2, int blue2)
{
	fontForeground.r = red;
	fontForeground.g = green;
	fontForeground.b = blue;

	fontBackground.r = red2;
	fontBackground.g = green2;
	fontBackground.b = blue2;
}

SDL_Surface *Graphics::createSurface(int width, int height)
{
	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	on the endianness (byte order) of the machine */
	#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
	#endif

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, 0);

	if (surface == NULL)
	{
		showErrorAndExit("CreateRGBSurface failed: %s\n", SDL_GetError());
	}
	
	return surface;
}

void Graphics::lock(SDL_Surface *surface)
{
	/* Lock the screen for direct access to the pixels */
	if (SDL_MUSTLOCK(surface))
	{
		if (SDL_LockSurface(surface) < 0 )
		{
			showErrorAndExit("Could not lock surface");
		}
	}
}

void Graphics::unlock(SDL_Surface *surface)
{
	if (SDL_MUSTLOCK(surface))
	{
		SDL_UnlockSurface(surface);
	}
}

void Graphics::showLicenseErrorAndExit()
{
	/*
	setFontSize(3); setFontColor(0xff, 0x00, 0x00, 0x00, 0x00, 0x00);
	drawString(320, 50, true, screen, false, "License Agreement Missing");

	setFontSize(1); setFontColor(0xff, 0xff, 0xff, 0x00, 0x00, 0x00);

	drawString(320, 180, true, screen, false, "The GNU General Public License was not found.");
	drawString(320, 220, true, screen, false, "It could either not be properly loaded or has been removed.");
	drawString(320, 260, true, screen, false, "Blob And Conquer will not run with the license missing.");

	drawString(320, 420, true, screen, false, "Blob and Conquer will now exit");
	drawString(320, 450, true, screen, false, "Press Escape to continue");
	*/

	engine->flushInput();

	while (true)
	{
		updateScreen();
		engine->getInput();
		if (engine->keyState[SDL_SCANCODE_ESCAPE])
		{
			exit(1);
		}
	}
}

void Graphics::showErrorAndExit(const char *error ...)
{
	debug(("Graphics::showErrorAndExit()\n"));
	
	setMode(MODE_2D);
	setFont(FONT_NORMAL);
	
	glDisable(GL_BLEND);
	
	clearScreen();
	
	int adjustX = getOffsetY();
	int adjustY = getOffsetY();
	int y = 50;
	
	drawRect(adjustX, adjustY, 800, 600, GLColor::white, false);
	drawRect(adjustX + 1, adjustY + 1, 800 - 2, 600 - 2, GLColor::black, false);
	
	char tmpString[1024];
	
	va_list argp;
	va_start(argp, error);
	vsnprintf(tmpString, sizeof(tmpString), error, argp);
	va_end(argp);
	
	glEnable(GL_BLEND);

	drawString(400 + adjustX, y + adjustY, TXT_CENTERED, GLColor::red, true, "An unforseen error has occurred");
	drawString(400 + adjustX, (y += 30) + adjustY, TXT_CENTERED, GLColor::red, true, "%s", tmpString);

	drawString(50 + adjustX, (y += 50) + adjustY, TXT_LEFT, GLColor::white, true, "You may wish to try the following,");

	drawString(75 + adjustX, (y += 40) + adjustY, TXT_LEFT, GLColor::cyan, true, "1) Try reinstalling the game.");
	drawString(75 + adjustX, (y += 30) + adjustY, TXT_LEFT, GLColor::cyan, true, "2) Ensure you have SDL 1.2.7 or greater installed.");
	drawString(75 + adjustX, (y += 30) + adjustY, TXT_LEFT, GLColor::cyan, true, "3) Ensure you have the latest versions of additional required SDL libraries.");
	drawString(75 + adjustX, (y += 30) + adjustY, TXT_LEFT, GLColor::cyan, true, "4) Ensure you have the correct OpenGL support.");
	drawString(75 + adjustX, (y += 30) + adjustY, TXT_LEFT, GLColor::cyan, true, "5) Install the latest OpenGL drivers from your vendor (%s)", glGetString(GL_VENDOR));
	drawString(75 + adjustX, (y += 30) + adjustY, TXT_LEFT, GLColor::cyan, true, "6) Install using an RPM if you originally built the game from source");
	drawString(95 + adjustX, (y += 30) + adjustY, TXT_LEFT, GLColor::cyan, true, "or try building from source if you installed using an RPM.");
	drawString(75 + adjustX, (y += 30) + adjustY, TXT_LEFT, GLColor::cyan, true, "7) Visit http://www.parallelrealities.co.uk/blobAndConquer.php and check for updates.");

	drawString(400 + adjustX, (y += 70) + adjustY, TXT_CENTERED, GLColor::white, true, "If problems persist contact Parallel Realities. Please be aware however that we will not");
	drawString(400 + adjustX, (y += 20) + adjustY, TXT_CENTERED, GLColor::white, true, "be able to assist in cases where the code or data has been modified.");

	drawString(400 + adjustX, (y += 40) + adjustY, TXT_CENTERED, GLColor::white, true, "Blob Wars : Blob and Conquer will now exit");
	drawString(400 + adjustX, (y += 30) + adjustY, TXT_CENTERED, GLColor::white, true, "Press Escape to continue");

	engine->flushInput();

	while (true)
	{
		updateScreen();
		engine->getInput();
		if (engine->keyState[SDL_SCANCODE_ESCAPE])
		{
			exit(1);
		}
	}
}

Graphics Graphics::instance;
