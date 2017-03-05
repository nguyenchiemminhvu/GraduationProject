#include "Definition.h"
#include "GameScene.h"
#include "GameSettings.h"
#include "Scenes\LevelSelectionBoard.h"
#include "Scenes\FinishedScene.h"
#include "Toast.h"
#include "HUD.h"
#include "Instruction.h"

#include <algorithm>

cocos2d::Scene* GameScene::createScene()
{
	auto scene = cocos2d::Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0.0F, 0.0F));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
#endif

	auto layer = GameScene::create();
	scene->addChild(layer, (int)ZOrderLayer::LAYER_1);
	layer->currentGameScene = scene;

	return scene;
}


bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	origin = cocos2d::Director::getInstance()->getVisibleOrigin();

	if (!loadTiledMapProperties())
		return false;
	
	if (!findStartPos())
		return false;
	
	if (!findEndPos())
		return false;
	
	if (!initMainCharaceter())
		return false;
	
	if (!initPath())
		return false;
	
	if (!initUI())
		return false;

	if (!initHUD())
		return false;

	if (!initInstruction())
		return false;

	if (!initEnemies())
		return false;

	if (!initTouchOneByOne())
		return false;
	
	if (!initContactListener())
		return false;

	hud->showArrow(buttonRun->getPosition(), 60.0F, false);

	return true;
}

////////////////////////////////////////////////
// initialization

bool GameScene::loadTiledMapProperties()
{
	auto mapFile = cocos2d::String::createWithFormat("tiledmaps/level_%d.tmx", GameSettings::getInstance()->getSelectedLevel());
	map = cocos2d::TMXTiledMap::create(mapFile->getCString());
	this->addChild(map);
	
	backgroundLayer = map->getLayer("background");
	if (!backgroundLayer)
		return false;

	playerGroup = map->getObjectGroup("Player");
	if (!playerGroup)
		return false;

	enemyGroup = map->getObjectGroup("Enemies");
	if (!enemyGroup)
		return false;

	mapSize = map->getMapSize();
	layerSize = backgroundLayer->getLayerSize();
	tileSize = backgroundLayer->getMapTileSize();

	return true;
}


bool GameScene::findStartPos()
{
	////////////////////////////////////////////////////
	// detecting the entrance door position of current level
	auto startObject = playerGroup->getObject("start");
	if (startObject == cocos2d::ValueMap())
		return false;

	float x = startObject["x"].asFloat() + tileSize.width / 2;
	float y = startObject["y"].asFloat() + tileSize.height / 2;
	startPos = cocos2d::Vec2(x, y);
	
	//normalizing start position
	startPos = pointToTileCoordinate(startPos);
	startPos = tileCoordinateToPoint(startPos);
	
	auto nodeStart = cocos2d::Sprite::create("images/gateway/gate.png");
	nodeStart->setPosition(startPos);
	this->addChild(nodeStart);
	
	return true;
}


bool GameScene::findEndPos()
{
	/////////////////////////////////////////////////////
	// detecting the exit position of current level
	auto endObject = playerGroup->getObject("end");
	if (endObject == cocos2d::ValueMap())
		return false;

	float x = endObject["x"].asFloat() + tileSize.width / 2;
	float y = endObject["y"].asFloat() + tileSize.height / 2;
	endPos = cocos2d::Vec2(x, y);

	//normalizing exit position
	endPos = pointToTileCoordinate(endPos);
	endPos = tileCoordinateToPoint(endPos);

	auto nodeEnd = cocos2d::Sprite::create("images/gateway/gate.png");
	nodeEnd->setPosition(endPos);
	this->addChild(nodeEnd);

	return true;
}


bool GameScene::initMainCharaceter()
{
	mainCharacter = MainCharacter::createMainCharacter(this, startPos);
	mainCharacter->setPhysicsBody(cocos2d::PhysicsBody::createCircle((int)GameObjectSize::WIDTH / 2));
	mainCharacter->getPhysicsBody()->setDynamic(true);
	mainCharacter->getPhysicsBody()->setContactTestBitmask((int)ContactTestBitmast::MAIN_CHARACTER);
	mainCharacter->getPhysicsBody()->setCollisionBitmask((int)CollisionBismask::MAIN_CHARACTER);

	return true;
}


bool GameScene::initPath()
{
	resetPath();

	return true;
}


bool GameScene::initUI()
{
	auto blankPartBackground = cocos2d::Sprite::create("images/UI/game_scene_ui/gamescene_background.png");
	blankPartBackground->setAnchorPoint(cocos2d::Vec2(1.0F, 0.0F));
	blankPartBackground->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width,
			origin.y
		)
	);
	this->addChild(blankPartBackground);

	auto buttonGetout = cocos2d::ui::Button::create("images/UI/game_scene_ui/button_give_up.png");
	buttonGetout->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width - buttonGetout->getContentSize().width / 1.5F,
			origin.y + visibleSize.height - buttonGetout->getContentSize().height
		)
	);
	this->addChild(buttonGetout);
	buttonGetout->addTouchEventListener(CC_CALLBACK_2(GameScene::onButtonGetoutTouched, this));


	buttonRun = cocos2d::ui::Button::create("images/UI/game_scene_ui/button_run.png");
	buttonRun->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width - buttonRun->getContentSize().width / 1.5F,
			origin.y + buttonRun->getContentSize().height
		)
	);
	this->addChild(buttonRun);
	buttonRun->addTouchEventListener(CC_CALLBACK_2(GameScene::onButtonReadyTouched, this));

	return true;
}


bool GameScene::initHUD()
{
	// create hud cause game scene freeze on android device???
	hud = HUD::createLayer();
	this->addChild(hud);

	return true;
}


bool GameScene::initInstruction()
{
	instructor = new Instruction(hud, startPos, endPos, buttonRun->getPosition());
	
	if (GameSettings::getInstance()->getSelectedLevel() == 1)
	{
		GameSettings::getInstance()->enableInstruction();
	}
	else
	{
		GameSettings::getInstance()->disableInstruction();
	}

	return true;
}


bool GameScene::initEnemies()
{
	enemyFactory = new EnemyFactory();
	auto enemies = enemyGroup->getObjects();

	for (int i = 0; i < enemies.size(); i++) {

		auto properties = enemies[i].asValueMap();

		//////////////////////////////////////////
		// get enemy type
		int type = properties["type"].asInt();
		
		/////////////////////////////////////////
		// get enemy start position
		float x = properties["x"].asFloat();
		float y = properties["y"].asFloat();
		cocos2d::Vec2 enemyPos(x, y);
		enemyPos = pointToTileCoordinate(enemyPos);
		enemyPos = tileCoordinateToPoint(enemyPos);

		///////////////////////////////////////
		//get enemy start direction
		int startDirection = 0;
		if (!properties["start_direction"].isNull())
			startDirection = properties["start_direction"].asInt();

		////////////////////////////////////////
		// get enemy speed
		float speed = 1.0F;
		if (!properties["speed"].isNull())
			speed = properties["speed"].asFloat();

		enemyFactory->createEnemy(type, this, enemyPos, startDirection, speed);
	}

	return true;
}


bool GameScene::initTouchOneByOne()
{
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);;
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);;
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}


bool GameScene::initContactListener()
{
	auto contactListener = cocos2d::EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	return true;
}

////////////////////////////////////////////////////////
// path

bool GameScene::addNodeToPath(cocos2d::Vec2 nextNode)
{
	//////////////////////////////////////////////
	// if the nextNode is already in the path, 
	// just return true and waiting for player 
	// touch on other node
	if (pathMap[nextNode])
		return true;

	/////////////////////////////////////////////
	// add the nextNode to the path if it's valid
	if (isNextNodeValid(nextNode)) {

		path.push_back(nextNode);
		pathMap[nextNode]++;

		//////////////////////////////////////////
		// if the end node has been added into the path
		// consider that the path is completed
		if (nextNode == pointToTileCoordinate(endPos))
			isPathCompleted = true;

		paintTileNode(nextNode);

		return true;
	}

	return false;
}


bool GameScene::isNextNodeValid(cocos2d::Vec2 nextNode)
{
	///////////////////////////////////////////////
	// if the nextNode is a wall
	if (getTileDescription(nextNode) == "wall")
		return false;

	///////////////////////////////////////////////
	// if the path is empty, check whether the nextNode is
	// at the start node or not
	if (path.empty()) {

		if (nextNode != pointToTileCoordinate(startPos))
			return false;
		else
			return true;
	}

	///////////////////////////////////////////////
	// in this case, the path has at least one node,
	// check whether player choose the correct nextNode
	cocos2d::Vec2 sub = nextNode - path.back();
	if (sub.length() != 1.0F)
		return false;

	return true;
}


void GameScene::resetPath()
{
	cleanPathColor();

	isTouchedOnMap = false;
	isPathCompleted = false;
	path.clear();
	pathMap.clear();

	mainCharacter->resetPath();
}


void GameScene::clearTheRestOfPath(cocos2d::Vec2 tilePos)
{
	isPathCompleted = false;

	std::vector<cocos2d::Vec2>::iterator iter;
	iter = std::find_if(path.begin(), path.end(), [=](cocos2d::Vec2 pos) { return pos == tilePos; });
	if (iter != path.end()) {

		iter++;
		for ( ; iter != path.end(); ) {

			unpaintTileNode((*iter));
			pathMap.erase((*iter));
			iter = path.erase(iter);
		}
	}
}


void GameScene::paintTileNode(cocos2d::Vec2 tilePos)
{
	auto tileSprite = backgroundLayer->getTileAt(tilePos);
	tileSprite->setColor(cocos2d::Color3B::GRAY);
}


void GameScene::unpaintTileNode(cocos2d::Vec2 tilePos)
{
	auto tileSprite = backgroundLayer->getTileAt(tilePos);
	tileSprite->setColor(cocos2d::Color3B::WHITE);
}


void GameScene::cleanPathColor()
{
	for (int i = 0; i < path.size(); i++) {

		unpaintTileNode(path[i]);
	}
}


//////////////////////////////////////////////////////
// events


void GameScene::onMainCharacterDead()
{
	// load a new game scene with current selected level
	// TODO: reset the current level to the origin state (avoid dynamic allocate memory)

	this->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(2.0F),
			cocos2d::CallFunc::create(this, callfunc_selector(GameScene::replaceCurrentLevel)),
			NULL
		)
	);
}


void GameScene::onButtonGetoutTouched(cocos2d::Ref * ref, cocos2d::ui::Button::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;

	case cocos2d::ui::Widget::TouchEventType::ENDED:
		replaceLevelSelectionBoard();
		break;

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;

	default:
		break;
	}
}


void GameScene::onButtonReadyTouched(cocos2d::Ref * ref, cocos2d::ui::Button::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;

	case cocos2d::ui::Widget::TouchEventType::ENDED:

		///////////////////////////////////////////////
		// If player want to start the ball's journey, 
		// - first condition  : the path is ready
		// - second condition : the ball is not started yet
		if (mainCharacter->ready() && !mainCharacter->isStarted())
			mainCharacter->move();

		break;

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;

	default:
		break;
	}
}


bool GameScene::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	//////////////////////////////////////////////////////
	// When the main character is not exist 
	// or the main character is moving on, 
	// the touch listener stop receiving touch events
	// to prevent player draw another path
	if (!mainCharacter || mainCharacter->isStarted())
		return false;

	/////////////////////////////////////////////////////
	// Get the first touch event
	cocos2d::Vec2 touchPosOnTileCoord = pointToTileCoordinate(touch->getLocation());
	isTouchedOnMap = (touchPosOnTileCoord.x >= 0 && touchPosOnTileCoord.x < mapSize.width) &&
					 (touchPosOnTileCoord.y >= 0 && touchPosOnTileCoord.y < mapSize.height);
	
	////////////////////////////////////////////////////
	// The touch listener will not work
	// if player touch outside the tiled map
	if (!isTouchedOnMap)
		return false;

	/////////////////////////////////////////////////////
	// if player touched on start node, let player draw the new path
	if (touchPosOnTileCoord == pointToTileCoordinate(startPos))
		resetPath();

	/////////////////////////////////////////////////////
	// if player touched on a node that already in the path, 
	// remove all the rest of the path
	if (pathMap[touchPosOnTileCoord])	{
		clearTheRestOfPath(touchPosOnTileCoord);
		return true;
	}

	/////////////////////////////////////////////////////
	// the path reach to the end but player want to change the path
	if (isPathCompleted) {
		resetPath();
		isTouchedOnMap = true;
	}

	/////////////////////////////////////////////////////
	// if the node is not near to the last node in the path
	// or the node is not at the start position (in case the path is empty)
	// reject the touch event
	if (!isNextNodeValid(touchPosOnTileCoord))
		return false;

	/////////////////////////////////////////////////////
	// If player want to start to create the path
	// - first condition  : the first touch location is on the start node 
	// - second condition : player must touch inside map
	// - third condition  : the path is not completed yet
	// - fourth condition : player touched on a valid tile node
	// If one in four conditions is false, reset the path
	if (isTouchedOnMap && !isPathCompleted && !addNodeToPath(pointToTileCoordinate(touch->getLocation()))) {
		
	}

	return true;
}


void GameScene::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	cocos2d::Vec2 touchPosOnTileCoord = pointToTileCoordinate(touch->getLocation());
	isTouchedOnMap = (touchPosOnTileCoord.x >= 0 && touchPosOnTileCoord.x < mapSize.width) 
				  && (touchPosOnTileCoord.y >= 0 && touchPosOnTileCoord.y < mapSize.height);

	/////////////////////////////////////////////////////
	// If player want to add a node into the path
	// - first condition  : player must touch inside the map
	// - second condition : the path is not completed yet
	// - third condition  : player touched on a valid tile node
	// If one in three conditions is false, reset the path
	if (isTouchedOnMap && !isPathCompleted && !addNodeToPath(pointToTileCoordinate(touch->getLocation()))) {
		
	}

}


void GameScene::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	isTouchedOnMap = false;

	//////////////////////////////////////////////////
	// After player stopped touching on the map, 
	// if the path is still not completed yet,
	// reset the path
	if (!isPathCompleted) {
		
	}

	else {
		/////////////////////////////////////////////
		// convert the path to game coordinate, 
		// then send the path to main character
		std::vector<cocos2d::Vec2> convertedPath = path;
		for (int i = 0; i < path.size(); i++) {
			convertedPath[i] = tileCoordinateToPoint(convertedPath[i]);
		}
		mainCharacter->setPath(convertedPath);
	}

}


bool GameScene::onContactBegan(cocos2d::PhysicsContact & contact)
{
	auto shapeA = contact.getShapeA();
	auto shapeB = contact.getShapeB();

	if (
		shapeA->getCollisionBitmask() == (int)CollisionBismask::MAIN_CHARACTER &&
		shapeB->getCollisionBitmask() == (int)CollisionBismask::OBSTACLE
		)
	{
		auto node = shapeB->getBody()->getNode();
		node->getPhysicsBody()->removeFromWorld();
		auto enemy = dynamic_cast<Enemy *>(node);
		if (enemy)
		{
			if (enemy->getPositionX() >= mainCharacter->getPositionX())
			{
				enemy->flipLeft();
			}
			else
			{
				enemy->flipRight();
			}

			enemy->stopAllActions();
			enemy->setEatingAnimation();
		}

		mainCharacter->beDestroyed();
		onMainCharacterDead();
		
		return true;
	}

	if (
		shapeA->getCollisionBitmask() == (int)CollisionBismask::OBSTACLE &&
		shapeB->getCollisionBitmask() == (int)CollisionBismask::MAIN_CHARACTER
		)
	{
		auto node = shapeA->getBody()->getNode();
		node->getPhysicsBody()->removeFromWorld();
		auto enemy = dynamic_cast<Enemy *>(node);
		if (enemy)
		{
			if (enemy->getPositionX() >= mainCharacter->getPositionX())
			{
				enemy->flipLeft();
			}
			else
			{
				enemy->flipRight();
			}

			enemy->stopAllActions();
			enemy->setEatingAnimation();
		}

		mainCharacter->beDestroyed();
		onMainCharacterDead();

		return true;
	}

	return false;
}


bool GameScene::touchedOnNode(cocos2d::Touch * touch, cocos2d::Node * target)
{
	auto runningScene = this->getScene();
	auto shapes = runningScene->getPhysicsWorld()->getShapes(touch->getLocation());

	for (int i = 0; i < shapes.size(); i++) {

		if (shapes.at(i)->getBody()->getNode() == target)
			return true;
	}

	return false;
}


void GameScene::replaceLevelSelectionBoard()
{
	cocos2d::Director::getInstance()->replaceScene(LevelSelectionBoard::createScene());
}


void GameScene::replaceCurrentLevel()
{
	auto currentLevel = GameScene::createScene();
	cocos2d::Director::getInstance()->replaceScene(currentLevel);
}


////////////////////////////////////////////////
// coordinates

cocos2d::Vec2 GameScene::pointToTileCoordinate(cocos2d::Vec2 point)
{
	auto mapSize = map->getMapSize();
	auto layerSize = backgroundLayer->getLayerSize();
	auto tileSize = backgroundLayer->getMapTileSize();

	int x = (int)(point.x / tileSize.width);
	int y = (int)((mapSize.height * tileSize.height - point.y) / tileSize.height);

	return cocos2d::Vec2(x, y);
}


cocos2d::Vec2 GameScene::tileCoordinateToPoint(cocos2d::Vec2 tileCoord)
{
	float x = tileCoord.x * tileSize.width + tileSize.width / 2;
	float y = (mapSize.height * tileSize.height) - tileCoord.y * tileSize.height - tileSize.height / 2;

	return cocos2d::Vec2(x, y);
}


///////////////////////////////////////////////////////
// get map properties

std::string GameScene::getTileDescription(cocos2d::Vec2 tileCoord)
{
	auto gid = backgroundLayer->getTileGIDAt(tileCoord);
	auto properties = map->getPropertiesForGID(gid);
	if (!properties.isNull()) {

		auto dict = properties.asValueMap();
		auto desProperty = dict["description"];
		if (!desProperty.isNull()) {

			return desProperty.asString();
		}
	}

	return std::string();
}


cocos2d::Size GameScene::getMapSize()
{
	return mapSize;
}


cocos2d::Size GameScene::getLayerSize()
{
	return layerSize;
}


cocos2d::Size GameScene::getTileSize()
{
	return tileSize;
}
