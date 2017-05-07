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

	cocos2d::Vec2	origin;
	cocos2d::Size	visibleSize;

	cocos2d::Size	mapSize;
	cocos2d::Size	layerSize;
	cocos2d::Size	tileSize;

	int						totalPage;
	cocos2d::ui::PageView	*pageView;
	cocos2d::ui::ImageView	*leftArrow;
	cocos2d::ui::ImageView	*rightArrow;

	//////////////////////////////////////
	// board initializations

	bool loadDungeonMaps();
	cocos2d::TMXTiledMap * loadDungeonMap(int page);
	bool loadArrows();
	void initKeyEventListener();

	//////////////////////////////////////
	// levels transitions
	
	void replaceMainMenuScene();
	void replaceGameScene();

	//////////////////////////////////////
	// others
	
	void refreshPageView();

	int getTotalPage();
	void onLevelSelected(cocos2d::Ref *sender, cocos2d::ui::Button::TouchEventType type);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event);
	void onPageViewEvent(cocos2d::Ref *ref, cocos2d::ui::PageView::EventType type);
};

#endif // !__LEVEL_SELECTION_BOARD_H__