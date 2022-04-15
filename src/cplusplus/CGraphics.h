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

#include "../attributes.h"

class Graphics {

	private:

		static Graphics instance;

		Engine *engine;
		Pak *pak;

		TTF_Font *font[MAX_FONTS];

		int screenShotNumber;
		int fontIndex;
		
		// used for var args in drawString like commands
		char tmpString[1024];
		
		SDL_Rect gRect;
		
		SDL_Color fontWhite, fontBlack;
		
		SDL_Color fontForeground, fontBackground;
		
		GLColor blackFade;
		
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY;
		
		int frames;
		float time;
		int acc;
		String fps;
		
		float animTimer;

	Graphics();

	public:
		
		SDL_Rect screenMode[MAX_RESOLUTIONS];
		SDL_Rect realWindowSize;

		SDL_Window *window;
		SDL_Surface *screen, *glScreenshot, *scratchSurface;
		
		Texture *counterDigit[13];
		
		int textWidth, textHeight;
		
		GLUquadricObj *quadric;
		
		int red, yellow, green, darkGreen, skyBlue, blue, cyan, white, lightGrey, grey, darkGrey, black;
		
		bool wireframe, lightMaps;
		
		int screenMidX, screenMidY;
		
		int currentScreenResolution;
		bool fullscreen;
		bool hasStencils;
		float gamma;
		unsigned int motionBlur;
		bool allowMotionBlur;
		
		int redPulseDir;
		float redPulse;

	static Graphics *getInstance();
	~Graphics();
	void destroy();
	
	void queryStencilSupport();
	void calculateScreenModes();
	void setResolution(int i);
	void resetFPSCount();
	void drawFPSCount(int y);
	
	void showLoadingProgress(const char *status);
	
	float getAnimTimer();
	
	void setMode(int mode);

	void setGamma(float gamma);
	void mapColors();
	int getColor(int red, int green, int blue);

	void clearScreen();
	void delay(int time, bool allowSkip);
	void updateScreen();
	void updateScreenMB();
	
	int getOffsetX();
	int getOffsetY();
	
	void drawBackground(Texture *texture);

	Texture *createTexture(SDL_Surface *surface);
	Texture *loadTexture(const char *filename);
	Texture *createTextureFromTGA(SDL_Surface *surface);
	
	Vector convertToWorldCoords(GLfloat winX, GLfloat winY, Vector *near, Vector *far, Vector *dir);
	Point convertToScreenCoords(Vector position);
	
	void setTransparent(SDL_Surface *sprite);

	void fade(float amount);
	void fade(float amount, int x, int y, int w, int h);
	
	void loadFont(int i, const char *filename, int size);
	void loadTimerDigits();
	void setFont(int index);

	// GL Functions
	void blit(Texture *texture, bool centered);
	void blit(Texture *texture, int x, int y, bool centered);
	void drawRect(int x, int y, int w, int h, GLColor color, bool transparent);
	void drawRect(int x, int y, int w, int h, GLColor color, GLColor borderColor, bool transparent);
	void outlineRect(int x, int y, int w, int h, GLColor color);
	void drawLine(int startX, int startY, int endX, int endY, GLColor color);
	void createLineBorder(GLColor color);
	void createRaisedBorder(GLColor light, GLColor shade);
	void createLoweredBorder(GLColor light, GLColor shade);
	void drawGradient(GLColor color1, GLColor color2, bool vertical);
	Texture *getGLString(const char *text, ...) GCC_PRINTF(2,3);
	void drawString(int x, int y, int alignment, GLColor color, bool transparent, const char *text, ...) GCC_PRINTF(7,8);
	void drawString(int x, int y, int alignment, SDL_Surface *dest, const char *text, ...) GCC_PRINTF(6,7);
	
	// SDL Functions
	void putPixel(int x, int y, Uint32 pixel, SDL_Surface *dest);
	Uint32 getPixel(SDL_Surface *surface, int x, int y);
	void drawLine(int startX, int startY, int endX, int endY, int color, SDL_Surface *dest);
	void drawCircle(int x, int y, int radius, SDL_Surface *dest, int color);
	void blit(SDL_Surface *image, int x, int y, SDL_Surface *dest, bool centered);
	SDL_Surface *getString(bool transparent, const char *text, ...) GCC_PRINTF(3,4);
	void drawRect(int x, int y, int w, int h, int color, SDL_Surface *dest);
	void drawRect(int x, int y, int w, int h, int color, int borderColor, SDL_Surface *dest);
	void createLineBorder(SDL_Surface *surface, int color);
	void createRaisedBorder(SDL_Surface *surface, int light, int shade);
	void createLoweredBorder(SDL_Surface *surface, int light, int shade);
	void drawGradient(SDL_Surface *surface, float r1, float g1, float b1, float r2, float g2, float b2);
	void lock(SDL_Surface *surface);
	void unlock(SDL_Surface *surface);
	void setFontColor(int red, int green, int blue, int red2, int green2, int blue2);
	SDL_Surface *createSurface(int width, int height);
	void writeWrappedText(SDL_Surface *dest, bool transparent, const char *text, ...) GCC_PRINTF(4,5);

	void showErrorAndExit(const char *error, ...) GCC_PRINTF(2,3) GCC_NORETURN;
	void showLicenseErrorAndExit() GCC_NORETURN;

};
