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
	selectedLevel = 0;
	int status = cocos2d::UserDefault::getInstance()->getIntegerForKey("level_status");
	if (status == 0)
		status = 1;
	levelStatus = status;

	if (levelStatus == getTotalLevel())
	{
		winTheGame = true;
	}
}


void GameSettings::resetAllLevel()
{
	winTheGame = false;

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


GameSettings::~GameSettings()
{
}


GameSettings::GameSettings()
{
	didResetLevel = false;
	winTheGame = false;

	enableSoundEffect();

	totalLevel = utils::countNumberOfFileWithFormat("tiledmaps/level_%d.tmx");
	selectedLevel = 0;
	levelStatus = 1; //enable the first level

	needInstruction = true; //instruction is needed for the first level
}
