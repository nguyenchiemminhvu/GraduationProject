#ifndef __LEVEL_SELECTION_BOARD_H__
#define __LEVEL_SELECTION_BOARD_H__

#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

#define NUMBER_OF_LEVEL_ON_EACH_ROW 5

class LevelSelectionBoard : public cocos2d::Layer {

public:

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(LevelSelectionBoard);

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	cocos2d::Size	mapSize;
	cocos2d::Size	layerSize;
	cocos2d::Size	tileSize;

	int totalPage;

	//////////////////////////////////////
	// board initializations

	bool loadDungeonMap(int page);
	void initKeyEventListener();

	//////////////////////////////////////
	// levels transitions
	
	void replaceMainMenuScene();
	void replaceGameScene();

	//////////////////////////////////////
	// others
	
	int getTotalPage();
	void onLevelSelected(cocos2d::Ref *sender, cocos2d::ui::Button::TouchEventType type);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event);
};

#endif // !__LEVEL_SELECTION_BOARD_H__