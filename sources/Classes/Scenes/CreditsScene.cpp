#include "CreditsScene.h"
#include "Scenes\MainMenuScene.h"


cocos2d::Scene * Credits::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto credits = Credits::create();
	scene->addChild(credits);
	return scene;
}


bool Credits::init()
{
	if (!cocos2d::Layer::init())
		return false;

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	initKeyboardEventListener();

	return true;
}


void Credits::initKeyboardEventListener()
{
	auto keyboardListener = cocos2d::EventListenerKeyboard::create();
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Credits::onKeyboardReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}


void Credits::replaceMainMenuScene()
{
	auto mainMenu = MainMenu::createScene();
	cocos2d::Director::getInstance()->replaceScene(mainMenu);
}


void Credits::onKeyboardReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event * event)
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
