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
}


void SoundManager::loadMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/intro.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/story.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/background_1.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/background_2.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/background_3.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/final.mp3");
}


SoundManager::~SoundManager()
{

}


void SoundManager::playIntroMusic()
{
	if (GameSettings::getInstance()->isSoundEffectEnabled())
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/intro.mp3");
	}
	else
	{
		stopMusic();
	}
}


void SoundManager::playStoryMusic()
{
	if (GameSettings::getInstance()->isSoundEffectEnabled())
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/story.mp3");
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
		int rand = cocos2d::random(1, 3);
		cocos2d::String *file = cocos2d::String::createWithFormat("background_%d.png", rand);
		CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(file->getCString(), true);
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
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/final.mp3");
	}
	else
	{
		stopMusic();
	}
}


void SoundManager::stopMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
}


void SoundManager::pauseMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
}


void SoundManager::resumeMusic()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
}
