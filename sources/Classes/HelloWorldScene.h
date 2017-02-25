#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

#include "Player\MainCharacter.h"

#include <string>
#include <vector>
#include <map>

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    CREATE_FUNC(HelloWorld);

	////////////////////////////////////
	// coordinates

	cocos2d::Vec2 pointToTileCoordinate(cocos2d::Vec2 point);
	cocos2d::Vec2 tileCoordinateToPoint(cocos2d::Vec2 tileCoord);

private:
	
	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	MainCharacter *mainCharacter;

	////////////////////////////////////////
	// Tiled map properties

	cocos2d::TMXTiledMap *map;
	cocos2d::TMXLayer *backgroundLayer;
	cocos2d::TMXObjectGroup *playerGroup;
	cocos2d::Size mapSize;
	cocos2d::Size layerSize;
	cocos2d::Size tileSize;
	cocos2d::Vec2 startPos;
	cocos2d::Sprite *nodeStart;
	cocos2d::Vec2 endPos;
	cocos2d::Sprite *nodeEnd;

	/////////////////////////////////////
	// path

	bool isTouchedOnMap;
	bool isPathCompleted;
	std::vector<cocos2d::Vec2> path;
	std::map<cocos2d::Vec2, int> pathMap;

	bool addNodeToPath(cocos2d::Vec2 nextNode);
	bool isNextNodeValid(cocos2d::Vec2 nextNode);
	void resetPath();
	void paintTileNode(cocos2d::Vec2 tilePos);
	void unpaintTileNode(cocos2d::Vec2 tilePos);
	void cleanPathColor();

	/////////////////////////////////////
	// initialization

	void loadTiledMapProperties();
	void findStartPos();
	void findEndPos();
	void initMainCharaceter();
	void initPath();
	void initUI();

	void initTouchOneByOne();
	void initContactListener();

	////////////////////////////////////
	// events

	void onButtonGetoutTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);
	void onButtonReadyTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);

	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);

	bool onContactBegan(cocos2d::PhysicsContact &contact);

	bool touchedOnNode(cocos2d::Touch *touch, cocos2d::Node *target);

	////////////////////////////////////
	// others

	std::string getTileDescription(cocos2d::Vec2 tileCoord);
};

#endif // __HELLOWORLD_SCENE_H__
