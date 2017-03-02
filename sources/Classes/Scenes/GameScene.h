#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

#include "Player\MainCharacter.h"
#include "Enemies\Enemy.h"

#include "HUD.h"

#include <string>
#include <vector>
#include <map>


class GameScene : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(GameScene);

	////////////////////////////////////
	// coordinates

	cocos2d::Vec2 pointToTileCoordinate(cocos2d::Vec2 point);
	cocos2d::Vec2 tileCoordinateToPoint(cocos2d::Vec2 tileCoord);

	////////////////////////////////////
	// get map properties
	std::string getTileDescription(cocos2d::Vec2 tileCoord);
	cocos2d::Size getMapSize();
	cocos2d::Size getLayerSize();
	cocos2d::Size getTileSize();

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	cocos2d::Scene *currentGameScene;

	MainCharacter *mainCharacter;
	EnemyFactory *enemyFactory;

	HUD	*hud;

	////////////////////////////////////////
	// Tiled map properties

	cocos2d::TMXTiledMap *map;
	cocos2d::TMXLayer *backgroundLayer;
	cocos2d::TMXObjectGroup *playerGroup;
	cocos2d::TMXObjectGroup *enemyGroup;
	cocos2d::Size mapSize;
	cocos2d::Size layerSize;
	cocos2d::Size tileSize;
	cocos2d::Vec2 startPos;
	cocos2d::Vec2 endPos;

	/////////////////////////////////////
	// path

	bool isTouchedOnMap;
	bool isPathCompleted;
	std::vector<cocos2d::Vec2> path;
	std::map<cocos2d::Vec2, int> pathMap;

	bool addNodeToPath(cocos2d::Vec2 nextNode);
	bool isNextNodeValid(cocos2d::Vec2 nextNode);
	void resetPath();
	void clearTheRestOfPath(cocos2d::Vec2 tilePos);
	void paintTileNode(cocos2d::Vec2 tilePos);
	void unpaintTileNode(cocos2d::Vec2 tilePos);
	void cleanPathColor();

	/////////////////////////////////////
	// initialization

	bool loadTiledMapProperties();
	bool findStartPos();
	bool findEndPos();
	bool initMainCharaceter();
	bool initPath();
	bool initUI();
	bool initHUD();

	bool initEnemies();

	bool initTouchOneByOne();
	bool initContactListener();

	////////////////////////////////////
	// events

	void onMainCharacterDead();

	void onButtonGetoutTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);
	void onButtonReadyTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);

	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);

	bool onContactBegan(cocos2d::PhysicsContact &contact);

	bool touchedOnNode(cocos2d::Touch *touch, cocos2d::Node *target);

	//////////////////////////////////////
	// level transition

	void replaceLevelSelectionBoard();
	void replaceCurrentLevel();
};

#endif // !__GAME_SCENE_H__