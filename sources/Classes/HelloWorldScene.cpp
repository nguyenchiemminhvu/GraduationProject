#include "GameSettings.h"
#include "Defination.h"
#include "HelloWorldScene.h"
#include "Scenes\LevelSelectionBoard.h"


cocos2d::Scene* HelloWorld::createScene()
{
    auto scene = cocos2d::Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0.0F, 0.0F));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
#endif

    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}


bool HelloWorld::init()
{
    if ( !cocos2d::Layer::init() )
    {
        return false;
    }
    
    visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    origin = cocos2d::Director::getInstance()->getVisibleOrigin();

	loadTiledMapProperties();
	findStartPos();
	findEndPos();
	initMainCharaceter();
	initPath();
	initUI();

	initTouchOneByOne();

    return true;
}

////////////////////////////////////////////////
// initialization

void HelloWorld::loadTiledMapProperties()
{
	auto mapFile = cocos2d::String::createWithFormat("tiledmaps/level_%d.tmx", GameSettings::getInstance()->getSelectedLevel());
	map = cocos2d::TMXTiledMap::create(mapFile->getCString());
	this->addChild(map);

	backgroundLayer = map->getLayer("background");
	playerGroup = map->getObjectGroup("Player");

	mapSize = map->getMapSize();
	layerSize = backgroundLayer->getLayerSize();
	tileSize = backgroundLayer->getMapTileSize();
}


void HelloWorld::findStartPos()
{
	auto startObject = playerGroup->getObject("start");
	float x = startObject["x"].asFloat() + tileSize.width / 2;
	float y = startObject["y"].asFloat() + tileSize.height / 2;
	startPos = cocos2d::Vec2(x, y);

	nodeStart = cocos2d::Sprite::create("images/gateway/gate.png");
	nodeStart->setPosition(startPos);
	this->addChild(nodeStart);
}


void HelloWorld::findEndPos()
{
	auto endObject = playerGroup->getObject("end");
	float x = endObject["x"].asFloat() + tileSize.width / 2;
	float y = endObject["y"].asFloat() + tileSize.height / 2;
	endPos = cocos2d::Vec2(x, y);

	nodeEnd = cocos2d::Sprite::create("images/gateway/gate.png");
	nodeEnd->setPosition(endPos);
	this->addChild(nodeEnd);
}


void HelloWorld::initMainCharaceter()
{
	mainCharacter = MainCharacter::createMainCharacter(this, startPos);
	mainCharacter->setPhysicsBody(cocos2d::PhysicsBody::createCircle(mainCharacter->getContentSize().width / 2));
	mainCharacter->getPhysicsBody()->setDynamic(true);
	mainCharacter->getPhysicsBody()->setContactTestBitmask(1);
	mainCharacter->getPhysicsBody()->setCollisionBitmask((int)CollisionBismask::MAIN_CHARACTER);
}


void HelloWorld::initPath()
{
	isTouchedOnMap = false;
	isPathCompleted = false;
	path.clear();
	pathMap.clear();
}


void HelloWorld::initUI()
{
	auto buttonGetout = cocos2d::ui::Button::create("images/UI/button_getout.png");
	buttonGetout->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width - buttonGetout->getContentSize().width / 2,
			origin.y + visibleSize.height - buttonGetout->getContentSize().height / 2
		)
	);
	this->addChild(buttonGetout);

	auto buttonReady = cocos2d::ui::Button::create("images/UI/button_ready.png");
	buttonReady->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width - buttonReady->getContentSize().width / 2,
			origin.y + buttonReady->getContentSize().height / 2
		)
	);
	this->addChild(buttonReady);

	buttonGetout->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onButtonGetoutTouched, this));
	buttonReady->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onButtonReadyTouched, this));
}


void HelloWorld::initTouchOneByOne()
{
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);;
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);;
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}


void HelloWorld::initContactListener()
{
	auto contactListener = cocos2d::EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
}

////////////////////////////////////////////////////////
// path

bool HelloWorld::addNodeToPath(cocos2d::Vec2 nextNode)
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


bool HelloWorld::isNextNodeValid(cocos2d::Vec2 nextNode)
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


void HelloWorld::resetPath()
{
	cleanPathColor();

	isTouchedOnMap = false;
	isPathCompleted = false;
	path.clear();
	pathMap.clear();

	mainCharacter->resetPath();
}


void HelloWorld::paintTileNode(cocos2d::Vec2 tilePos)
{
	auto tileSprite = backgroundLayer->getTileAt(tilePos);
	tileSprite->setColor(cocos2d::Color3B::GRAY);
}


void HelloWorld::unpaintTileNode(cocos2d::Vec2 tilePos)
{
	auto tileSprite = backgroundLayer->getTileAt(tilePos);
	tileSprite->setColor(cocos2d::Color3B::WHITE);
}


void HelloWorld::cleanPathColor()
{
	for (int i = 0; i < path.size(); i++) {

		unpaintTileNode(path[i]);
	}
}


//////////////////////////////////////////////////////
// events

void HelloWorld::onButtonGetoutTouched(cocos2d::Ref * ref, cocos2d::ui::Button::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		cocos2d::Director::getInstance()->replaceScene(LevelSelectionBoard::createScene());
		break;
	
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	
	default:
		break;
	}
}


void HelloWorld::onButtonReadyTouched(cocos2d::Ref * ref, cocos2d::ui::Button::TouchEventType type)
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


bool HelloWorld::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	cocos2d::Vec2 touchPosOnTileCoord = pointToTileCoordinate(touch->getLocation());
	isTouchedOnMap = (touchPosOnTileCoord.x >= 0 && touchPosOnTileCoord.x < mapSize.width) && 
					 (touchPosOnTileCoord.y >= 0 && touchPosOnTileCoord.y < mapSize.height);

	if (!isTouchedOnMap)
		return false;

	/////////////////////////////////////////////////////
	// check whether player touched on the start node or not
	if (touchPosOnTileCoord != pointToTileCoordinate(startPos))
		return false;

	/////////////////////////////////////////////////////
	// the path reach to the end but player want to change the path
	if (isPathCompleted) {
		resetPath();
		isTouchedOnMap = true;
	}

	/////////////////////////////////////////////////////
	// If player want to start to create the path
	// - first condition  : the first touch location is on the start node 
	// - second condition : player must touch inside map
	// - third condition  : the path is not completed yet
	// - fourth condition : player touched on a valid tile node
	// If one in four conditions is false, reset the path
	if (isTouchedOnMap && !isPathCompleted && !addNodeToPath(pointToTileCoordinate(touch->getLocation()))) {
		resetPath();
	}

	return true;
}


void HelloWorld::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	cocos2d::Vec2 touchPosOnTileCoord = pointToTileCoordinate(touch->getLocation());
	isTouchedOnMap = (touchPosOnTileCoord.x >= 0 && touchPosOnTileCoord.x < mapSize.width) &&
					 (touchPosOnTileCoord.y >= 0 && touchPosOnTileCoord.y < mapSize.height);

	/////////////////////////////////////////////////////
	// If player want to add a node into the path
	// - first condition  : player must touch inside the map
	// - second condition : the path is not completed yet
	// - third condition  : player touched on a valid tile node
	// If one in three conditions is false, reset the path
	if (isTouchedOnMap && !isPathCompleted && !addNodeToPath(pointToTileCoordinate(touch->getLocation()))) {
		resetPath();
	}

}


void HelloWorld::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	isTouchedOnMap = false;

	//////////////////////////////////////////////////
	// After player stopped touching on the map, 
	// if the path is still not completed yet,
	// reset the path
	if (!isPathCompleted) {
		resetPath();
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


bool HelloWorld::onContactBegan(cocos2d::PhysicsContact & contact)
{
	return true;
}


bool HelloWorld::touchedOnNode(cocos2d::Touch * touch, cocos2d::Node * target)
{
	auto runningScene = this->getScene();
	auto shapes = runningScene->getPhysicsWorld()->getShapes(touch->getLocation());

	for (int i = 0; i < shapes.size(); i++) {

		if (shapes.at(i)->getBody()->getNode() == target)
			return true;
	}

	return false;
}

////////////////////////////////////////////////
// coordinates

cocos2d::Vec2 HelloWorld::pointToTileCoordinate(cocos2d::Vec2 point)
{
	auto mapSize = map->getMapSize();
	auto layerSize = backgroundLayer->getLayerSize();
	auto tileSize = backgroundLayer->getMapTileSize();

	int x = (int)(point.x / tileSize.width);
	int y = (int)((mapSize.height * tileSize.height - point.y) / tileSize.height);

	return cocos2d::Vec2(x, y);
}


cocos2d::Vec2 HelloWorld::tileCoordinateToPoint(cocos2d::Vec2 tileCoord)
{
	float x = tileCoord.x * tileSize.width + tileSize.width / 2;
	float y = (mapSize.height * tileSize.height) - tileCoord.y * tileSize.height - tileSize.height / 2;

	return cocos2d::Vec2(x, y);
}


///////////////////////////////////////////////////////
// others

std::string HelloWorld::getTileDescription(cocos2d::Vec2 tileCoord)
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
