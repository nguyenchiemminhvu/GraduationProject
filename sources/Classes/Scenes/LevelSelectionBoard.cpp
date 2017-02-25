#include "LevelSelectionBoard.h"
#include "Scenes\MainMenuScene.h"
#include "Scenes\GameScene.h"
#include "Definition.h"
#include "GameSettings.h"


cocos2d::Scene * LevelSelectionBoard::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto board = LevelSelectionBoard::create();
	scene->addChild(board);
	return scene;
}


bool LevelSelectionBoard::init()
{
	if (!cocos2d::Layer::init())
		return false;

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	initLevelBoardBackground();
	loadLevels();
	initKeyEventListener();

	return true;
}


void LevelSelectionBoard::initLevelBoardBackground()
{
	auto background = cocos2d::Sprite::create("images/UI/level_selection_ui/level_selection_background.png");
	background->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2
		)
	);
	this->addChild(background);
}


void LevelSelectionBoard::loadLevels()
{
	int totalLevel = GameSettings::getInstance()->getTotalLevel();
	int currentLevelNumber = 0;
	cocos2d::Vector<cocos2d::Menu *> rows;
	while (currentLevelNumber < totalLevel) {

		auto row = createRowOfLevels(currentLevelNumber + 1, totalLevel);
		rows.pushBack(row);
		currentLevelNumber += row->getChildrenCount();
	}

	auto gridLayout = cocos2d::ui::VBox::create(cocos2d::Size(visibleSize.width, 0));
	gridLayout->setAnchorPoint(cocos2d::Vec2(0.5F, 0.5F));

	for (int i = 0; i < rows.size(); i++) {

		auto layoutSpaceTop = cocos2d::ui::Layout::create();
		layoutSpaceTop->setContentSize(cocos2d::Size(rows.at(i)->getContentSize().width, rows.at(i)->getContentSize().height / 4));
		gridLayout->addChild(layoutSpaceTop);

		auto layoutRow = cocos2d::ui::Layout::create();
		layoutRow->setLayoutType(cocos2d::ui::Layout::Type::VERTICAL);
		layoutRow->setContentSize(cocos2d::Size(rows.at(i)->getContentSize().width, rows.at(i)->getContentSize().height));
		layoutRow->addChild(rows.at(i));
		layoutRow->setTouchEnabled(true);
		gridLayout->addChild(layoutRow);

		auto layoutSpaceBottom = cocos2d::ui::Layout::create();
		layoutSpaceBottom->setContentSize(cocos2d::Size(rows.at(i)->getContentSize().width, rows.at(i)->getContentSize().height / 4));
		gridLayout->addChild(layoutSpaceBottom);

		gridLayout->setContentSize(
			cocos2d::Size(
				gridLayout->getContentSize().width,
				gridLayout->getContentSize().height + rows.at(i)->getContentSize().height
			)
		);
	}
	
	this->addChild(gridLayout);
	gridLayout->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2,
			origin.y + gridLayout->getContentSize().height / 4
		)
	);
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


cocos2d::Menu * LevelSelectionBoard::createRowOfLevels(int level, int totalLevel)
{
	cocos2d::Vector<cocos2d::MenuItem *> items;
	cocos2d::Size itemSize;

	for (int i = 1; i <= NUMBER_OF_LEVEL_ON_EACH_ROW && level <= totalLevel; i++, level++) {

		auto item = cocos2d::MenuItemImage::create(
			"images/UI/level_selection_ui/button_level_opened.png", 
			"images/UI/level_selection_ui/button_level_pressed.png", 
			"images/UI/level_selection_ui/button_level_disabled.png"
		);
		item->setScaleY(BUTTON_SIZE_SCALE_Y);
		itemSize = item->getContentSize();
		item->setTag(level);

		item->setCallback(CC_CALLBACK_1(LevelSelectionBoard::onLevelSelected, this));
		items.pushBack(item);

		if (!GameSettings::getInstance()->isLevelEnabled(level)) {
			item->setEnabled(false);
		}
	}

	auto row = cocos2d::Menu::createWithArray(items);
	row->alignItemsHorizontallyWithPadding(itemSize.width / 1.5F);
	row->setContentSize(cocos2d::Size(visibleSize.width, itemSize.height));
	return row;
}


void LevelSelectionBoard::onLevelSelected(cocos2d::Ref * ref)
{
	auto item = dynamic_cast<cocos2d::MenuItemImage *>(ref);
	GameSettings::getInstance()->selectLevel(item->getTag());

	cocos2d::Director::getInstance()->replaceScene(GameScene::createScene());
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
