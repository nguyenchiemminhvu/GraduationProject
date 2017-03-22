#include "StoryScene.h"
#include "GameSettings.h"
#include "SoundManager.h"
#include "Toast.h"


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

	return true;
}
