#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

#include "Player\MainCharacter.h"
#include "Enemies\Enemy.h"

#include <string>
#include <vector>
#include <map>

class HUD;
class Instruction;

enum class PathDirection;


class GameCache
{
public:

	static GameCache * getInstance();
	bool		hasInstance();
	void		destroyInstance();

	virtual ~GameCache();

	void storePath(std::vector<cocos2d::Vec2> &path);
	std::vector<cocos2d::Vec2> getData();

	bool hasData();
	void clearCache();

private:

	static GameCache *instance;

	std::vector<cocos2d::Vec2>	path;

	GameCache();
};


class GameScene : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void update(float dt);

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

	/////////////////////////////////////
	// entrance door handler
	void onMainCharacterWinLevel();

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	MainCharacter	*mainCharacter;

	EnemyFactory					*enemyFactory;
	std::vector<ImmobilizedEnemy *> immobilizedQueue;

	HUD						*hud;
	std::map<int, float>	speedForHud;

	////////////////////////////////////////
	// Tiled map properties

	cocos2d::TMXTiledMap	*map;
	cocos2d::TMXLayer		*backgroundLayer;
	cocos2d::TMXObjectGroup *playerGroup;
	cocos2d::TMXObjectGroup *enemyGroup;
	cocos2d::Size			mapSize;
	cocos2d::Size			layerSize;
	cocos2d::Size			tileSize;
	cocos2d::Vec2			startPos;
	cocos2d::Vec2			endPos;

	//////////////////////////////////////
	// UI components
	
	cocos2d::ui::Button *buttonRun;
	cocos2d::Sprite		*leftEntranceDoor;
	cocos2d::Sprite		*rightEntranceDoor;
	bool				isEntranceDoorOpened;

	/////////////////////////////////////
	// path
	
	bool								isTouchedOnMap;
	bool								isPathCompleted;
	std::vector<cocos2d::Vec2>			path;
	cocos2d::Vector<cocos2d::Sprite *>	pathArrows;
	std::map<cocos2d::Vec2, int>		pathMap;

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
	bool initPhysicsBodyWalls();
	bool initMainCharaceter();
	bool initPath();
	bool initUI();
	bool initHUD();
	bool initInstruction();
	bool initEnemies();
	bool initTheEntranceDoor();
	bool initContactListener();

	////////////////////////////////////
	// events

	bool canTouch;
	void initTouchOneByOne();

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
	void replaceMainMenu();

	///////////////////////////////////////
	// instruction
	
	void helloFromInstructor();
	void showInstructionAtStartPos();

	////////////////////////////////////////
	// others

	void openEntranceDoor();
	void closeEntranceDoor();

};

#endif // !__GAME_SCENE_H__