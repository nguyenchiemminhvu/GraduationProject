#include "StoryScene.h"
#include "GameSettings.h"
#include "SoundManager.h"
#include "Toast.h"
#include "Scenes\LevelSelectionBoard.h"


cocos2d::Scene * StoryScene::createScene()
{
	auto scene = cocos2d::Scene::create();

	auto layer = StoryScene::create();
	scene->addChild(layer);

	return scene;
}


bool StoryScene::init()
{
	if (!cocos2d::Layer::init())
	{
		return false;
	}

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();



	SoundManager::getInstance()->playStoryMusic();

	// register touch listener after 1 second
	this->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(1.0F),
			cocos2d::CallFunc::create(this, callfunc_selector(StoryScene::initTouchListener)),
			NULL
		)
	);

	return true;
}


void StoryScene::initTouchListener()
{
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(StoryScene::onTouchBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}


bool StoryScene::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	replaceLevelSelectionBoard();
	return true;
}


void StoryScene::replaceLevelSelectionBoard()
{
	auto levelBoard = LevelSelectionBoard::createScene();
	cocos2d::Director::getInstance()->replaceScene(levelBoard);
}
