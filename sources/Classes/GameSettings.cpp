#include "cocos2d.h"
#include "GameSettings.h"
#include "Instruction.h"
#include "Utility.h"
#include "Definition.h"


GameSettings* GameSettings::instance = nullptr;

GameSettings * GameSettings::getInstance()
{
	if (instance == nullptr)
		instance = new GameSettings();

	return instance;
}


bool GameSettings::hasLoaded()
{
	return this->loaded;
}


///////////////////////////////////
// sound settings

bool GameSettings::isSoundEffectEnabled()
{
	return soundEffectEnabled;
}


void GameSettings::enableSoundEffect()
{
	soundEffectEnabled = true;
}


void GameSettings::disableSoundEffect()
{
	soundEffectEnabled = false;
}


int GameSettings::getTotalMap()
{
	return utils::countNumberOfFileWithFormat("tiledmaps/dungeon_map_%d.tmx");
}


int GameSettings::getCurrentMap()
{
	return selectedMap;
}


void GameSettings::selectMapNumber(int index)
{
	selectedMap = index;
}


//////////////////////////////////
// level settings

int GameSettings::getTotalLevel()
{
	return totalLevel;
}


bool GameSettings::isLevelEnabled(int index)
{
	return (levelStatus >> (index - 1)) & 1;
}


void GameSettings::enableLevel(int index)
{
	levelStatus |= (1 << (index - 1));
}


void GameSettings::disableLevel(int index)
{
	levelStatus &= ~(1 << (index - 1));
}


void GameSettings::selectLevel(int index)
{
	if (index <= 0 || index > totalLevel)
		return;

	selectedLevel = index;
	didResetLevel = false;
}


int GameSettings::getSelectedLevel()
{
	return selectedLevel;
}


int GameSettings::getLevelStatus()
{
	return levelStatus;
}


void GameSettings::resetCurrentLevel()
{
	didResetLevel = true;
}


bool GameSettings::isLevelReset()
{
	return didResetLevel;
}


void GameSettings::save()
{
	cocos2d::UserDefault::getInstance()->setIntegerForKey("level_status", levelStatus);
}


void GameSettings::load()
{
	bool loadFirstTime = cocos2d::UserDefault::getInstance()->getBoolForKey("first_load");
	if (!loadFirstTime)
	{
		this->loaded = true;
		cocos2d::UserDefault::getInstance()->setBoolForKey("first_load", this->loaded);

		selectedLevel = 0;
		levelStatus = 0;
		winTheGame = false;
	}
	else
	{
		selectedLevel = 0;
		int status = cocos2d::UserDefault::getInstance()->getIntegerForKey("level_status");
		levelStatus = status;

		if (levelStatus >= getTotalLevel())
		{
			winTheGame = true;
		}
	}
}


void GameSettings::resetAllLevel()
{
	winTheGame = false;

	totalMap = getTotalMap();
	selectedMap = 0;

	totalLevel = utils::countNumberOfFileWithFormat("tiledmaps/level_%d.tmx");
	selectedLevel = 0;
	levelStatus = 1; //enable the first level
	
	save();
}


bool GameSettings::wonTheGame()
{
	return winTheGame;
}


void GameSettings::onPlayerWinAllLevel()
{
	winTheGame = true;
}


bool GameSettings::isInstructionNeeded()
{
	return needInstruction;
}


void GameSettings::enableInstruction()
{
	needInstruction = true;
}


void GameSettings::disableInstruction()
{
	needInstruction = false;
}


bool GameSettings::hasShowedStory()
{
	return isStoryShowed;
}


void GameSettings::ignoreStory()
{
	isStoryShowed = true;
}


GameSettings::~GameSettings()
{
}


GameSettings::GameSettings()
{
	loaded = false;

	didResetLevel = false;
	winTheGame = false;

	enableSoundEffect();

	totalMap = getTotalMap();
	selectedMap = 0;

	totalLevel = utils::countNumberOfFileWithFormat("tiledmaps/level_%d.tmx");
	selectedLevel = 0;
	levelStatus = 0;

	needInstruction = true; // instruction is needed for the first level
	isStoryShowed = false; // story of game will be showed for the first launch
}
