#pragma once

#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include <string>

class SoundManager
{
public:

	static SoundManager * getInstance();

	virtual ~SoundManager();

	void playStoryMusic();
	void playBackgroundMusic();
	void playEndGameMusic();

	void stopMusic();
	void pauseMusic();
	void resumeMusic();

private:

	static SoundManager *instance;

	unsigned int currentMusicID;

	SoundManager();
	void loadMusic();

};

#endif // !__SOUND_MANAGER_H__
