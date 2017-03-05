#ifndef __GAME_SETTINGS_H__
#define __GAME_SETTINGS_H__

#pragma once

class GameSettings
{
public:

	static GameSettings* getInstance();

	///////////////////////////////////
	// sound settings
	bool isSoundEffectEnabled();
	void enableSoundEffect();
	void disableSoundEffect();

	//////////////////////////////////
	// level settings
	int getTotalLevel();
	bool isLevelEnabled(int index);
	void enableLevel(int index);
	void disableLevel(int index);
	void selectLevel(int index);
	int getSelectedLevel();
	void save();
	void load();
	void resetAllLevel();
	bool wonTheGame();
	void onPlayerWinAllLevel();

	//////////////////////////////////
	// instruction settings
	bool isInstructionNeeded();
	void enableInstruction();
	void disableInstruction();

	~GameSettings();
	
private:

	bool winTheGame;

	bool soundEffectEnabled;

	int totalLevel;
	int selectedLevel;
	unsigned long long int levelStatus;

	bool needInstruction;

	GameSettings();
	GameSettings(const GameSettings &other) = delete;
	GameSettings& operator = (const GameSettings &other) = delete;

	static GameSettings *instance;
};

#endif // !__GAME_SETTINGS_H__


