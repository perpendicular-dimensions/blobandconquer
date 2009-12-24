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

void channelDone(int channel)
{
	static Audio *audio = Audio::getInstance();
	
	audio->channelVolume[channel] = -1;
}

Audio::Audio()
{
	soundVolume = 50;
	musicVolume = 128;
	ambientVolume = 80;
	environmentVolume = 50;
	
	useAudio = 2;
	useSound = true;
	useMusic = true;
	
	amountOfAmbience = 0;
	minAmbienceTime = 5;
	maxAmbienceTime = 30;
	ambienceTime = Math::rrand(minAmbienceTime, maxAmbienceTime) * 100;

	for (int i = 0 ; i < MAX_SOUNDS ; i++)
	{
		sound[i] = NULL;
	}
	
	for (int i = 0 ; i < MAX_CHANNELS ; i++)
	{
		channelVolume[i] = -1;
	}

	engine = Engine::getInstance();
	
	pak = Pak::getInstance();

	music = NULL;
	
	Mix_ChannelFinished(channelDone);
}

Audio *Audio::getInstance()
{
	return &instance;
}

void Audio::setSoundVolume(int soundVolume)
{
	this->soundVolume = soundVolume;
	
	if (useAudio)
	{
		Mix_Volume(-1, soundVolume);
		Mix_Volume(CH_AMBIENT, ambientVolume);
		Mix_Volume(CH_ENVIRONMENT, environmentVolume);
	}
}

void Audio::setMusicVolume(int musicVolume)
{
	this->musicVolume = musicVolume;
	
	if (useAudio)
	{
		Mix_VolumeMusic(musicVolume);
	}
}

void Audio::setAmbientVolume(int ambientVolume)
{
	this->ambientVolume = ambientVolume;
	
	if (useAudio)
	{
		Mix_Volume(CH_AMBIENT, ambientVolume);
	}
}

void Audio::setEnvironmentVolume(int environmentVolume)
{
	this->environmentVolume = environmentVolume;
	
	if (useAudio)
	{
		Mix_Volume(CH_ENVIRONMENT, environmentVolume);
	}
}

bool Audio::loadSound(int i, const char *filename)
{
	if (!useAudio)
	{
		return true;
	}

	if (i >= MAX_SOUNDS)
	{
		printf("ERROR: SOUND INDEX IS HIGHER THAN MAXIMUM ALLOWED %d >= %d\n", i, MAX_SOUNDS);
		exit(1);
	}

	if (sound[i] != NULL)
	{
		Mix_FreeChunk(sound[i]);
		sound[i] = NULL;
	}
	
	if (!pak->unpack(filename, PAK::BINARY))
	{
		return true;
	}
		
	sound[i] = Mix_LoadWAV_RW(pak->sdlrw, true);

	if (!sound[i])
	{
		return false;
	}
	
	debug(("Audio::loadSound() - Loaded Sound %d - %s\n", i, filename));

	return true;
}

bool Audio::loadMusic(const char *filename)
{
	if (!useAudio)
	{
		return true;
	}

	if (music != NULL)
	{
		Mix_HaltMusic();
		SDL_Delay(5);
		Mix_FreeMusic(music);
		music = NULL;
	}
	
	if (!pak->unpack(filename, PAK::BINARY))
	{
		return true;
	}
	
	// only supported in SDL 1.2.7+
	//music = Mix_LoadMUS_RW(pak->sdlrw);
	
	pak->writeToTempFile();
	
	music = Mix_LoadMUS(pak->tmpFilename.getText());
	
	if (!music)
	{
		return false;
	}
	
	debug(("Audio::loadMusic() - Loaded '%s'\n", filename));

	return true;
}

void Audio::playSound(int snd, int channel, int volume)
{
	if (!useAudio)
	{
		return;
	}
	
	Math::limit(&volume, 0, soundVolume);
	
	if (volume <= 0)
	{
		return;
	}
	
	// play on any free unreserved channel
	if (channel == -1)
	{
		for (int i = CH_EFFECT1 ; i < MAX_CHANNELS ; i++)
		{
			if (!Mix_Playing(i))
			{
				channel = i;
				break;
			}
		}
	}
	
	// no free channels, look for the quietest one and use that
	if (channel == -1)
	{
		for (int i = CH_EFFECT1 ; i < MAX_CHANNELS ; i++)
		{
			if (channelVolume[i] < volume)
			{
				channel = i;
				break;
			}
		}
	}
	
	// Forget it, we can't play this sound
	if (channel == -1)
	{
		//debug(("Audio::playSound - No free channel\n"));
		return;
	}
	
	if (volume < soundVolume)
	{
		if ((Mix_Playing(channel)) && (volume < channelVolume[channel]))
		{
			return;
		}
	}
	
	channelVolume[channel] = volume;
	
	Mix_Volume(channel, volume);

	Mix_PlayChannel(channel, sound[snd], 0);
}

void Audio::haltChannel(int channel)
{
	if (channel == -1)
	{
		printf("ERROR: Can't halt channel -1!!\n");
		exit(1);
	}
	
	Mix_HaltChannel(channel);
	channelVolume[channel] = -1;
}

void Audio::playSoundLoop(int snd, int channel)
{
	if (!useAudio)
	{
		return;
	}

	Mix_PlayChannel(channel, sound[snd], -1);
}

void Audio::freeSound(int i)
{
	if (!useAudio)
	{
		return;
	}
	
	if (sound[i] != NULL)
	{
		Mix_FreeChunk(sound[i]);
		sound[i] = NULL;
	}
}

void Audio::playMusic()
{
	if (!useAudio)
	{
		return;
	}

	Mix_PlayMusic(music, -1);

	Mix_VolumeMusic(musicVolume);
}

void Audio::doAmbience()
{
	if ((!useAudio) || (ambientVolume == 0))
	{
		return;
	}
	
	if (amountOfAmbience == 0)
	{
		return;
	}
	
	ambienceTime -= engine->getTimeDifference(false);
	
	if (ambienceTime <= 0)
	{
		int sound = rand() % amountOfAmbience;
		
		debug(("Audio::doAmbience - Playing sound %d\n", sound));
		
		Mix_Volume(CH_AMBIENT, ambientVolume);
		Mix_PlayChannel(CH_AMBIENT, ambience[sound], 0);
		
		ambienceTime = Math::rrand(minAmbienceTime, maxAmbienceTime) * 100;
	}
}

bool Audio::loadAmbience(int i, const char *filename)
{
	if (!useAudio)
	{
		return true;
	}

	if (i >= MAX_AMBIENCE)
	{
		printf("ERROR: AMBIENT INDEX IS HIGHER THAN MAXIMUM ALLOWED %d >= %d\n", i, MAX_AMBIENCE);
		exit(1);
	}

	if (ambience[i] != NULL)
	{
		Mix_FreeChunk(ambience[i]);
		ambience[i] = NULL;
	}
	
	if (!pak->unpack(filename, PAK::BINARY))
	{
		return false;
	}
	
	ambience[i] = Mix_LoadWAV_RW(pak->sdlrw, true);
	
	if (!ambience[i])
	{
		return false;
	}

	return true;
}

void Audio::pause()
{
	if (!useAudio)
	{
		return;
	}

	for (int i = 0 ; i < 8 ; i++)
	{
		Mix_Pause(i);
	}

	Mix_PauseMusic();
}

void Audio::resume()
{
	if (!useAudio)
	{
		return;
	}

	for (int i = 0 ; i < 8 ; i++)
	{
		Mix_Resume(i);
	}

	Mix_ResumeMusic();
}

void Audio::stopMusic()
{
	if (!useAudio)
	{
		return;
	}

	Mix_HaltMusic();
}

void Audio::fadeMusic()
{
	if (!useAudio)
	{
		return;
	}

	Mix_FadeOutMusic(5000);
	Mix_FadeOutChannel(-1, 5000);
}

void Audio::freeAmbience()
{
	for (int i = 0 ; i < MAX_AMBIENCE ; i++)
	{
		if (ambience[i] != NULL)
		{
			Mix_FreeChunk(ambience[i]);
		}
		
		ambience[i] = NULL;
	}
	
	if (music != NULL)
	{
		Mix_HaltMusic();
		SDL_Delay(5);
		Mix_FreeMusic(music);
	}

	music = NULL;
}

void Audio::free()
{
	for (int i = 0 ; i < MAX_SOUNDS - 3 ; i++)
	{
		if (sound[i] != NULL)
		{
			Mix_FreeChunk(sound[i]);
		}
		
		sound[i] = NULL;
	}
	
	for (int i = 0 ; i < MAX_AMBIENCE ; i++)
	{
		if (ambience[i] != NULL)
		{
			Mix_FreeChunk(ambience[i]);
		}
		
		ambience[i] = NULL;
	}

	if (music != NULL)
	{
		Mix_HaltMusic();
		SDL_Delay(5);
		Mix_FreeMusic(music);
	}

	music = NULL;
}

void Audio::destroy()
{
	free();

	for (int i = 0 ; i < MAX_SOUNDS ; i++)
	{
		if (sound[i] != NULL)
		{
			Mix_FreeChunk(sound[i]);
			sound[i] = NULL;
		}
	}
}

Audio Audio::instance;
