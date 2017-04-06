#include "LevelSelectionBoard.h"
#include "Scenes\MainMenuScene.h"
#include "Scenes\GameScene.h"
#include "Definition.h"
#include "GameSettings.h"
#include "Utility.h"


cocos2d::Scene * LevelSelectionBoard::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto board = LevelSelectionBoard::create();
	scene->addChild(board, (int)ZOrderLayer::LAYER_1);
	return scene;
}


bool LevelSelectionBoard::init()
{
	if (!cocos2d::Layer::init())
		return false;

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	loadDungeonMaps();
	loadArrows();
	initKeyEventListener();

	return true;
}


bool LevelSelectionBoard::loadDungeonMaps()
{
	this->pageView = cocos2d::ui::PageView::create();
	this->pageView->setTouchEnabled(true);
	this->pageView->setContentSize(visibleSize);

	totalPage = getTotalPage();
	for (int i = 1; i <= totalPage; i++)
	{
		auto layout = cocos2d::ui::Layout::create();
		layout->setContentSize(visibleSize);
		
		auto map = loadDungeonMap(i);
		if (!map)
			return false;

		layout->addChild(map);
		this->pageView->addPage(layout);
	}

	this->pageView->addEventListener(CC_CALLBACK_2(LevelSelectionBoard::onPageViewEvent, this));
	this->addChild(this->pageView);

	this->pageView->setCurPageIndex(GameSettings::getInstance()->getCurrentMap());
	refreshPageView();

	return true;
}


cocos2d::TMXTiledMap * LevelSelectionBoard::loadDungeonMap(int page)
{
	// load tiled map
	auto mapFile = cocos2d::String::createWithFormat("tiledmaps/dungeon_map_%d.tmx", page);
	auto map = cocos2d::TMXTiledMap::create(mapFile->getCString());
	if (!map)
		return false;

	// find background layer
	auto backgroundLayer = map->getLayer("background");
	if (!backgroundLayer)
		return false;

	// find room group
	auto roomGroup = map->getObjectGroup("room");
	if (!roomGroup)
		return false;

	// get tiled map properties
	mapSize = map->getMapSize();
	layerSize = backgroundLayer->getLayerSize();
	tileSize = backgroundLayer->getMapTileSize();

	// init the rooms
	auto rooms = roomGroup->getObjects();
	for (int i = 0; i < rooms.size(); i++)
	{
		auto roomProperty = rooms[i].asValueMap();

		// get level number of the room
		int levelNumber = roomProperty["level_number"].asInt();

		// get the door position
		float x = roomProperty["x"].asFloat();
		float y = roomProperty["y"].asFloat();
		//y = (mapSize.height * tileSize.height) - y;
		cocos2d::Vec2 doorPos(x, y);
		
		// create the door
		auto door = cocos2d::ui::Button::create(
			"images/UI/level_selection_ui/button_level_opened.png",
			"images/UI/level_selection_ui/button_level_pressed.png",
			"images/UI/level_selection_ui/button_level_disabled.png"
		);
		door->setAnchorPoint(cocos2d::Vec2(0.0F, 0.0F));
		door->addTouchEventListener(CC_CALLBACK_2(LevelSelectionBoard::onLevelSelected, this));
		door->setTag(levelNumber);
		door->setPosition(doorPos);
		door->setScale(0.5F);
		map->addChild(door, (int)ZOrderLayer::LAYER_10);
	
		if (!GameSettings::getInstance()->isLevelEnabled(levelNumber))
		{
			door->setEnabled(false);
		}
	}

	return map;
}


bool LevelSelectionBoard::loadArrows()
{
	leftArrow = cocos2d::ui::ImageView::create("images/UI/level_selection_ui/left_arrow.png");
	leftArrow->setAnchorPoint(cocos2d::Vec2(0.0F, 0.5F));
	leftArrow->setPosition(
		cocos2d::Vec2(
			origin.x + 10,
			origin.y + visibleSize.height / 2
		)
	);
	this->addChild(leftArrow);
	leftArrow->setVisible(false);

	rightArrow = cocos2d::ui::ImageView::create("images/UI/level_selection_ui/right_arrow.png");
	rightArrow->setAnchorPoint(cocos2d::Vec2(1.0F, 0.5F));
	rightArrow->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width - 10,
			origin.y + visibleSize.height / 2
		)
	);
	this->addChild(rightArrow);
	rightArrow->setVisible(false);

	refreshPageView();

	return true;
}


void LevelSelectionBoard::initKeyEventListener()
{
	auto keyListener = cocos2d::EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(LevelSelectionBoard::onKeyReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
}


void LevelSelectionBoard::replaceMainMenuScene()
{
	auto menu = MainMenu::createScene();
	cocos2d::Director::getInstance()->replaceScene(menu);
}


void LevelSelectionBoard::replaceGameScene()
{
	auto gameScene = GameScene::createScene();
	cocos2d::Director::getInstance()->replaceScene(gameScene);
}


void LevelSelectionBoard::refreshPageView()
{
	if (!this->pageView || !this->leftArrow || !this->rightArrow)
		return;

	if (this->pageView->getCurPageIndex() > 0)
	{
		leftArrow->setVisible(true);
	}
	else
	{
		leftArrow->setVisible(false);
	}


	if (this->pageView->getCurPageIndex() < this->pageView->getPages().size() - 1)
	{
		rightArrow->setVisible(true);
	}
	else
	{
		rightArrow->setVisible(false);
	}
}


int LevelSelectionBoard::getTotalPage()
{
	return utils::countNumberOfFileWithFormat("tiledmaps/dungeon_map_%d.tmx");
}


void LevelSelectionBoard::onLevelSelected(cocos2d::Ref * sender, cocos2d::ui::Button::TouchEventType type)
{
	auto room = dynamic_cast<cocos2d::ui::Button *>(sender);

	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		GameSettings::getInstance()->selectLevel(room->getTag());
		cocos2d::Director::getInstance()->replaceScene(GameScene::createScene());
		break;
	
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	
	default:
		break;
	}
}


void LevelSelectionBoard::onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event * event)
{
	switch (key)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
		replaceMainMenuScene();
		break;

	default:
		break;
	}
}


void LevelSelectionBoard::onPageViewEvent(cocos2d::Ref * ref, cocos2d::ui::PageView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::PageView::EventType::TURNING:
		GameSettings::getInstance()->selectMapNumber(this->pageView->getCurPageIndex());
		refreshPageView();
		break;
	
	default:
		break;
	}
}
