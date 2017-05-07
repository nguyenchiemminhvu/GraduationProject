#include "SoundManager.h"
#include "SimpleAudioEngine.h"
#include "GameSettings.h"

#include "cocos2d.h"

SoundManager * SoundManager::instance = nullptr;


SoundManager * SoundManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new SoundManager();
	}
	return instance;
}


SoundManager::SoundManager()
{
	loadMusic();
	currentMusicID = -1;
}


void SoundManager::loadMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/background_1.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/background_2.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/final.mp3");
}


SoundManager::~SoundManager()
{

}


void SoundManager::playStoryMusic()
{
	if (GameSettings::getInstance()->isSoundEffectEnabled())
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
		currentMusicID = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/background_1.mp3");
	}
	else
	{
		stopMusic();
	}
}


void SoundManager::playBackgroundMusic()
{
	if (GameSettings::getInstance()->isSoundEffectEnabled())
	{
		int rand = cocos2d::random(1, 2);
		cocos2d::String *file = cocos2d::String::createWithFormat("sounds/background_%d.mp3", rand);
		CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
		currentMusicID = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(file->getCString(), true);
	}
	else
	{
		stopMusic();
	}
}


void SoundManager::playEndGameMusic()
{
	if (GameSettings::getInstance()->isSoundEffectEnabled())
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
		currentMusicID = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/final.mp3");
	}
	else
	{
		stopMusic();
	}
}


void SoundManager::stopMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
	
	currentMusicID = -1;
}


void SoundManager::pauseMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
}


void SoundManager::resumeMusic()
{
	if (currentMusicID == -1)
		return;

	CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(currentMusicID);
}
