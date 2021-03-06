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

class Audio {

	private:

		static Audio instance;

		Engine *engine;
		Pak *pak;
		
		Mix_Chunk *sound[MAX_SOUNDS];
		Mix_Chunk *ambience[MAX_AMBIENCE];

		float ambienceTime;

		Mix_Music *music;

	Audio();

	public:
		
		int channelVolume[MAX_CHANNELS];
		
		int musicVolume;
		int soundVolume;
		int ambientVolume;
		int environmentVolume;

		bool useAudio;
		bool useSound, useMusic;
		
		int amountOfAmbience;
		int minAmbienceTime, maxAmbienceTime;

	static Audio *getInstance();
	
	void setSoundVolume(int soundVolume);
	void setMusicVolume(int musicVolume);
	void setAmbientVolume(int ambientVolume);
	void setEnvironmentVolume(int environmentVolume);
	
	bool loadSound(int i, const char *filename);
	bool loadMusic(const char *filename);
	void playSound(int snd, int channel, int volume);
	void playSoundLoop(int snd, int channel);
	void freeSound(int i);
	
	void playMusic();
	void haltChannel(int i);
	bool loadAmbience(int i, const char *filename);
	
	void doAmbience();
	void pause();
	void resume();
	void stopMusic();
	void fadeMusic();
	void free();
	void freeAmbience();
	void destroy();

};
