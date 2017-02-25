#ifndef __FINISHED_SCENE_H__
#define __FINISHED_SCENE_H__

#pragma once

#include "cocos2d.h"

#define WAIT_FOR_SCROLL_BACKGROUND_DURATION 3.0F
#define SCROLL_BACKGROUND_DURATION			5.0F
#define SCROLL_CREDITS_DURATION				15.0F
#define MAIN_CHARACTER_RUNNING_DURATION		5.0F

class MainCharacter;

class FinishedScene : cocos2d::Layer {

public:

	static cocos2d::Scene* createScene();

	virtual ~FinishedScene();
	virtual bool init();

	CREATE_FUNC(FinishedScene);

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	cocos2d::Sprite		*background;
	cocos2d::Sequence	*backgroundSequence;
	cocos2d::Sprite		*credits;
	cocos2d::Sequence	*creditsSequence;
	MainCharacter		*mainCharacter;
	cocos2d::Sequence	*mainCharacterSequence;
	cocos2d::Sprite		*enemy;
	cocos2d::Sprite		*enemy2;
	cocos2d::Sequence	*enemySequence;
	cocos2d::Sequence	*enemy2Sequence;
	
	cocos2d::Sequence	*finishedSceneSequence;

	cocos2d::Vector<cocos2d::Sprite *> friends;

	////////////////////////////////
	// init
	void initBackground();
	void initMainCharacter();
	void initMainCharacterForChasingAction();
	void initMonstersForChasingAction();
	void initMainCharacterFriends();
	void initCredits();
	void initSequenceAction();
	void initKeyEventListener();

	////////////////////////////////
	// finished scene transitions
	void replaceMainMenu();

	////////////////////////////////
	// events
	void onButtonBackTouched(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

	////////////////////////////////
	// others
	void loadMainCharacterFriendsAnimation();
	void setupAllFriendsPosition();
	void runSequenceActions();
	void setEnemy2Visible();
	void flipMainCharacterX();
};

#endif // !__FINISHED_SCENE_H__