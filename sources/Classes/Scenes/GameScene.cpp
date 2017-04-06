#include "Definition.h"
#include "GameScene.h"
#include "GameSettings.h"
#include "Scenes\LevelSelectionBoard.h"
#include "Scenes\FinishedScene.h"
#include "Scenes\MainMenuScene.h"
#include "Toast.h"
#include "Utility.h"
#include "HUD.h"
#include "Instruction.h"
#include "SoundManager.h"

#include <algorithm>


MainCharacter *mc_Instance = nullptr;

GameCache * GameCache::instance = nullptr;

///////////////////////////////////////////////////////////
// game cache

GameCache * GameCache::getInstance()
{
	if (instance == nullptr)
	{
		instance = new GameCache();
	}
	return instance;
}


bool GameCache::hasInstance()
{
	return instance != nullptr;
}


void GameCache::destroyInstance()
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}


GameCache::GameCache()
{
	path.clear();
}


GameCache::~GameCache()
{

}


void GameCache::storePath(std::vector<cocos2d::Vec2>& path)
{
	this->path = path;
}


std::vector<cocos2d::Vec2> GameCache::getData()
{
	return this->path;
}


bool GameCache::hasData()
{
	return !this->path.empty();
}


void GameCache::clearCache()
{
	this->path.clear();
}


////////////////////////////////////////////////////////////
// Game scene

cocos2d::Scene* GameScene::createScene()
{
	auto scene = cocos2d::Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0.0F, 0.0F));

#if __DEBUG_MODE__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
#endif

#endif // !__DEBUG_MODE__

	auto layer = GameScene::create();
	scene->addChild(layer, (int)ZOrderLayer::LAYER_1);

	return scene;
}


bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

#if __DEBUG_MODE__
	utils::startNewDebugSession();
#endif

	canTouch = false;

	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	origin = cocos2d::Director::getInstance()->getVisibleOrigin();

	if (!loadTiledMapProperties())
		return false;
	
	if (!findStartPos())
		return false;
	
	if (!findEndPos())
		return false;

	if (!initPhysicsBodyWalls())
		return false;

	if (!initMainCharaceter())
		return false;
	
	if (!initPath())
		return false;
	
	if (!initUI())
		return false;

	if (!initEnemies())
		return false;

	if (!initHUD())
		return false;

	if (!initInstruction())
		return false;

	if (!initTheEntranceDoor())
		return false;
	
	if (!initContactListener())
		return false;
	
	if (!GameSettings::getInstance()->isLevelReset())
	{
		SoundManager::getInstance()->playBackgroundMusic();
	}

	if (GameSettings::getInstance()->isInstructionNeeded())
	{
		if (Instruction::getInstance()->isShowFirstTime())
		{
			//wait 1 second to load scene then say hello from instructor
			//keep showing text board in 3 seconds
			//after that, jump to next instruction
			this->runAction(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(1.0F),
					cocos2d::CallFunc::create(this, callfunc_selector(GameScene::helloFromInstructor)),
					cocos2d::DelayTime::create(5.0F),
					cocos2d::CallFunc::create(this, callfunc_selector(GameScene::showInstructionAtStartPos)),
					NULL
				)
			);
		}
		else
		{
			//wait 1 second to load scene then point to start position
			this->runAction(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(1.0F),
					cocos2d::CallFunc::create(this, callfunc_selector(GameScene::showInstructionAtStartPos)),
					NULL
				)
			);
		}
	}
	else
	{
		hud->showSpeed();
	}

	this->scheduleUpdate();

	return true;
}


void GameScene::update(float dt)
{
	// handle close entrance door signal from main character
	extern bool needCloseEntranceDoor;
	if (needCloseEntranceDoor)
	{
		needCloseEntranceDoor = false;
		closeEntranceDoor();
	}
}


////////////////////////////////////////////////
// initialization

bool GameScene::loadTiledMapProperties()
{
	cocos2d::String *mapFile = nullptr;

	if (GameSettings::getInstance()->getLevelStatus() == 0)
	{
		mapFile = cocos2d::String::create("tiledmaps/tutorial.tmx");
	}
	else
	{
		mapFile = cocos2d::String::createWithFormat("tiledmaps/level_%d.tmx", GameSettings::getInstance()->getSelectedLevel());
	}
	
	map = cocos2d::TMXTiledMap::create(mapFile->getCString());
	if (!map)
		return false;

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


bool GameScene::initPhysicsBodyWalls()
{
	//TODO: implement Moore neighborhood algorithm
	//		to improve init wall physics body performance

	for (int row = 0; row < getLayerSize().height; row++)
	{
		for (int col = 0; col < getLayerSize().width; col++)
		{
			if (getTileDescription(cocos2d::Vec2(row, col)) == TILE_WALL_DESCRIPTON)
			{
				cocos2d::Sprite *block = backgroundLayer->getTileAt(cocos2d::Vec2(row, col));
				if (!block)
					continue;

				cocos2d::PhysicsBody *blockBody = cocos2d::PhysicsBody::createBox(getTileSize());
				blockBody->setDynamic(false);
				blockBody->setContactTestBitmask((int)ContactTestBitmast::WALL);
				blockBody->setCollisionBitmask((int)CollisionBismask::WALL);

				block->setPhysicsBody(blockBody);
			}
		}
	}

	return true;
}


bool GameScene::initMainCharaceter()
{
	mainCharacter = MainCharacter::createMainCharacter(this, startPos);
	mainCharacter->setPhysicsBody(cocos2d::PhysicsBody::createCircle((int)GameObjectSize::WIDTH / 2));
	mainCharacter->getPhysicsBody()->setDynamic(true);
	mainCharacter->getPhysicsBody()->setContactTestBitmask((int)ContactTestBitmast::MAIN_CHARACTER);
	mainCharacter->getPhysicsBody()->setCollisionBitmask((int)CollisionBismask::MAIN_CHARACTER);

	mainCharacter->setVisible(false);

	mc_Instance = mainCharacter;

	return true;
}


bool GameScene::initPath()
{
	pathArrows.clear();
	resetPath();

	// load the previous path if exist
	if (GameCache::getInstance()->hasData())
	{
		std::vector<cocos2d::Vec2> cacheData = GameCache::getInstance()->getData();
		GameCache::getInstance()->clearCache();

		// draw the path
		std::vector<cocos2d::Vec2>::iterator iter;
		for (iter = cacheData.begin(); iter != cacheData.end(); iter++)
		{
			addNodeToPath(*iter);
			cocos2d::log("%f %f", (*iter).x, (*iter).y);
		}
		isPathCompleted = true;

		/////////////////////////////////////////////
		// convert the path to game coordinate, 
		// then send the path to main character
		std::vector<cocos2d::Vec2> convertedPath = path;
		for (int i = 0; i < path.size(); i++) {
			convertedPath[i] = tileCoordinateToPoint(convertedPath[i]);
		}
		mainCharacter->setPath(convertedPath);
	}

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
			origin.x + visibleSize.width - buttonGetout->getContentSize().width / 1.8F,
			origin.y + visibleSize.height - buttonGetout->getContentSize().height / 1.3F
		)
	);
	this->addChild(buttonGetout);
	buttonGetout->addTouchEventListener(CC_CALLBACK_2(GameScene::onButtonGetoutTouched, this));


	buttonRun = cocos2d::ui::Button::create("images/UI/game_scene_ui/button_run.png");
	buttonRun->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width - buttonRun->getContentSize().width / 1.8F,
			origin.y + buttonRun->getContentSize().height / 1.3F
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
	hud->loadSpeed(speedForHud);
	this->addChild(hud);

	return true;
}


bool GameScene::initInstruction()
{
	if (!GameSettings::getInstance()->isInstructionNeeded())
		return true;

	Instruction::getInstance()->loadRequirement(hud, startPos, endPos, buttonRun->getPosition());
	
	if (GameSettings::getInstance()->getLevelStatus() == 0)
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
		// get enemy start direction
		int startDirection = 0;
		if (!properties["start_direction"].isNull())
			startDirection = properties["start_direction"].asInt();

		////////////////////////////////////////
		// get enemy speed
		float speed = 1.0F;
		if (!properties["speed"].isNull())
			speed = properties["speed"].asFloat();

		cocos2d::Node * enemy = enemyFactory->createEnemy(type, this, enemyPos, speed, startDirection);

		////////////////////////////////////////
		// information for HUD
		if (speedForHud[type] == 0)
		{
			speedForHud[type] = speed;
		}

		/////////////////////////////////////////
		// push immobilized monster into queue
		if (IS_A(enemy, ImmobilizedEnemy))
		{
			immobilizedQueue.push_back(dynamic_cast<ImmobilizedEnemy *>(enemy));
		}
	}

	return true;
}


bool GameScene::initTheEntranceDoor()
{
	// left side
	leftEntranceDoor = cocos2d::Sprite::create("images/gateway/left_entrance_door.png");
	leftEntranceDoor->setAnchorPoint(cocos2d::Vec2(0.0F, 0.5F));
	leftEntranceDoor->setPosition(
		cocos2d::Vec2(
			origin.x,
			origin.y + visibleSize.height / 2
		)
	);
		
	// right side
	rightEntranceDoor = cocos2d::Sprite::create("images/gateway/left_entrance_door.png");
	rightEntranceDoor->setAnchorPoint(cocos2d::Vec2(1.0F, 0.5F));
	rightEntranceDoor->setFlippedX(true);
	rightEntranceDoor->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width,
			origin.y + visibleSize.height / 2
		)
	);

	this->addChild(leftEntranceDoor, (int)ZOrderLayer::LAYER_10);
	this->addChild(rightEntranceDoor, (int)ZOrderLayer::LAYER_10);

	isEntranceDoorOpened = false;

	this->runAction(cocos2d::CallFunc::create(this, callfunc_selector(GameScene::openEntranceDoor)));

	return true;
}


void GameScene::enableMainCharacter()
{
	if (mainCharacter && !mainCharacter->isVisible())
	{
		mainCharacter->setVisible(true);
	}
}


void GameScene::initTouchOneByOne()
{
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);;
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);;
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

	canTouch = true;
}


bool GameScene::initContactListener()
{
	auto contactListener = cocos2d::EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	return true;
}


void GameScene::helloFromInstructor()
{
	Instruction::getInstance()->showInstruction(Instruction::InstructionStep::SAY_HELLO);
}


void GameScene::showInstructionAtStartPos()
{
	Instruction::getInstance()->showInstruction(Instruction::InstructionStep::TOUCH_AT_START_POS);
}


void GameScene::openEntranceDoor()
{
	if (isEntranceDoorOpened)
		return;

	// entrance door action
	auto leftAction = cocos2d::RotateBy::create(1.0F, cocos2d::Vec3(0.0F, 180.0F, 0.0F));
	auto rightAction = cocos2d::RotateBy::create(1.0F, cocos2d::Vec3(0.0F, -180.0F, 0.0F));

	leftEntranceDoor->runAction(
		cocos2d::Sequence::create(
			leftAction,
			cocos2d::CallFunc::create(this, callfunc_selector(GameScene::enableMainCharacter)),
			cocos2d::CallFunc::create(this, callfunc_selector(GameScene::initTouchOneByOne)),
			NULL
		)
	);

	rightEntranceDoor->runAction(
		cocos2d::Sequence::create(
			rightAction,
			NULL
		)
	);

	isEntranceDoorOpened = true;
}


void GameScene::closeEntranceDoor()
{
	if (!isEntranceDoorOpened)
		return;

	// entrance door action
	auto leftAction = cocos2d::RotateBy::create(1.0F, cocos2d::Vec3(0.0F, -180.0F, 0.0F));
	auto rightAction = cocos2d::RotateBy::create(1.0F, cocos2d::Vec3(0.0F, 180.0F, 0.0F));

	leftEntranceDoor->runAction(
		cocos2d::Sequence::create(
			leftAction,
			NULL
		)
	);

	rightEntranceDoor->runAction(
		cocos2d::Sequence::create(
			rightAction,
			NULL
		)
	);

	isEntranceDoorOpened = true;
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

		// show instruction to notice player draw the path carefully
		if (GameSettings::getInstance()->isInstructionNeeded() && path.size() >= 2)
		{
			Instruction::getInstance()->showInstruction(Instruction::InstructionStep::DRAW_THE_PATH);

			// specify the end of the path
			if (GameSettings::getInstance()->isInstructionNeeded() && path.size() >= 5)
			{
				Instruction::getInstance()->showInstruction(Instruction::InstructionStep::SPECIFY_THE_END_POS);
			}
		}

		//////////////////////////////////////////
		// if the end node has been added into the path
		// consider that the path is completed
		if (nextNode == pointToTileCoordinate(endPos))
		{
			isPathCompleted = true;

			// after draw a completed path, tell player to use button Run
			if (GameSettings::getInstance()->isInstructionNeeded())
			{
				Instruction::getInstance()->showInstruction(Instruction::InstructionStep::USE_BUTTON_RUN);
			}
		}

		paintTileNode(nextNode);

		if (path.size() > 1)
		{
			// add an arrow on tile node to specify the direction
			auto arrowSprite = cocos2d::Sprite::create("images/instruction/path_arrow.png");
			cocos2d::Vec2 arrowPosition = tileCoordinateToPoint(nextNode);
			arrowSprite->setPosition(arrowPosition);
			pathArrows.pushBack(arrowSprite);

			if (path.size() > 2)
			{
				this->addChild(pathArrows.at(pathArrows.size() - 2), (int)ZOrderLayer::LAYER_1);
				
				// rotate the arrow based on previous tile location
				if (path.at(path.size() - 2).x == path.at(path.size() - 1).x)
				{
					// up or down
					if (path.at(path.size() - 2).y > path.at(path.size() - 1).y)
					{
						// up
						pathArrows.at(pathArrows.size() - 2)->setFlippedY(true);
					}
					else
					{
						// down
					}
				}
				else
				{
					// left or right
					if (path.at(path.size() - 2).x > path.at(path.size() - 1).x)
					{
						// right
						pathArrows.at(pathArrows.size() - 2)->setRotation(90.0F);
					}
					else
					{
						// left
						pathArrows.at(pathArrows.size() - 2)->setRotation(-90.0F);
					}
				}
			}
		}

		return true;
	}

	return false;
}


bool GameScene::isNextNodeValid(cocos2d::Vec2 nextNode)
{
	///////////////////////////////////////////////
	// if the nextNode is a wall
	if (getTileDescription(nextNode) == TILE_WALL_DESCRIPTON)
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

	// clean path arrow
	cocos2d::Vector<cocos2d::Sprite *>::iterator iter;
	for (iter = pathArrows.begin(); iter != pathArrows.end(); )
	{
		if ((*iter))
		{
			(*iter)->removeFromParentAndCleanup(true);
			iter = pathArrows.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	pathArrows.clear();
}


void GameScene::clearTheRestOfPath(cocos2d::Vec2 tilePos)
{
	isPathCompleted = false;

	std::vector<cocos2d::Vec2>::iterator iter;
	iter = std::find_if(path.begin(), path.end(), [=](cocos2d::Vec2 pos) { return pos == tilePos; });
	if (iter != path.end()) {

		int pathArrowsIndex = iter - path.begin() - 1;
		cocos2d::Vector<cocos2d::Sprite *>::iterator pathArrowsIter = pathArrows.begin() + pathArrowsIndex;

		iter++;
		for ( ; iter != path.end(); ) {

			unpaintTileNode((*iter));
			pathMap.erase((*iter));
			iter = path.erase(iter);
		}

		// reset state of the arrow
		(*pathArrowsIter)->setFlippedX(false);
		(*pathArrowsIter)->setFlippedY(false);
		(*pathArrowsIter)->setRotation(0.0F);
		(*pathArrowsIter)->removeFromParentAndCleanup(false);

		pathArrowsIter++;
		for ( ; pathArrowsIter != pathArrows.end(); )
		{
			(*pathArrowsIter)->removeFromParentAndCleanup(true);
			pathArrowsIter = pathArrows.erase(pathArrowsIter);
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
	Instruction::getInstance()->showInstruction(Instruction::InstructionStep::MISSION_FAILED);

	// store the current path
	if (isPathCompleted)
	{
		GameCache::getInstance()->clearCache();
		GameCache::getInstance()->storePath(this->path);
	}

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
		if(canTouch)
		{
			GameCache::getInstance()->clearCache();
			SoundManager::getInstance()->stopMusic();

			if (GameSettings::getInstance()->getLevelStatus() == 0)
			{
				replaceMainMenu();
			}
			else
			{
				replaceLevelSelectionBoard();
			}
		}
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
		// - third condition  : touch listener is initialized
		if (canTouch && mainCharacter->ready() && !mainCharacter->isStarted())
		{
			mainCharacter->move();

			// after the first run, good luck for player
			if (GameSettings::getInstance()->isInstructionNeeded())
			{
				Instruction::getInstance()->showInstruction(Instruction::InstructionStep::GOOD_LUCK);
			}

			// activate all immobilized monster
			std::vector<ImmobilizedEnemy *>::iterator iter;
			for (iter = immobilizedQueue.begin(); iter != immobilizedQueue.end(); )
			{
				(*iter)->activate();

				if (IS_A(*iter, UpgradedChaser))
				{
					// keep track the upgraded chaser
					// to send update path signal
					iter++;
				}
				else
				{
					// remove the non-upgraded chaser
					iter = immobilizedQueue.erase(iter);
				}
			}

			// send all remaining immobilized monster to main character
			mainCharacter->receiveImmobilizedQueue(immobilizedQueue);
		}

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
	// If one in four conditions is false, do nothing
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
	// If one in three conditions is false, do nothing
	if (isTouchedOnMap && !isPathCompleted && !addNodeToPath(pointToTileCoordinate(touch->getLocation()))) {
		
	}

}


void GameScene::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	isTouchedOnMap = false;

	//////////////////////////////////////////////////
	// After player stopped touching on the map, 
	// if the path is still not completed yet,
	// do nothing,
	// keep the path
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
	GameSettings::getInstance()->resetCurrentLevel();

	// save the current path of player into the cache

	auto currentLevel = GameScene::createScene();
	cocos2d::Director::getInstance()->replaceScene(currentLevel);
}


void GameScene::replaceMainMenu()
{
	auto mainMenu = MainMenu::createScene();
	cocos2d::Director::getInstance()->replaceScene(mainMenu);
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


void GameScene::onMainCharacterWinLevel()
{
	closeEntranceDoor();
}
