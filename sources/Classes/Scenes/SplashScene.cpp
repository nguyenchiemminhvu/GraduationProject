#include "SplashScene.h"
#include "Scenes\MainMenuScene.h"
#include "Definition.h"


cocos2d::Scene * SplashScene::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto splashLayer = SplashScene::create();

	scene->addChild(splashLayer, (int)ZOrderLayer::LAYER_1);
	return scene;
}


bool SplashScene::init()
{
	if (!cocos2d::Layer::init()) {
		return false;
	}

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	initSplash();
	initKeyboardEventListener();
	this->scheduleOnce(schedule_selector(SplashScene::replaceMainMenuScene), 2.0F);

	return true;
}


void SplashScene::initSplash()
{
	this->splash = cocos2d::Sprite::create("images/logo/cocos2d_logo.png");
	this->splash->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2
		)
	);
	
	this->addChild(this->splash);
}


void SplashScene::initKeyboardEventListener()
{
	auto keyboardListener = cocos2d::EventListenerKeyboard::create();
	keyboardListener->onKeyReleased = CC_CALLBACK_2(SplashScene::onKeyReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}


void SplashScene::replaceMainMenuScene(float dt)
{
	auto mainMenuScene = MainMenu::createScene();
	cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(1.0F, mainMenuScene));
}


void SplashScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event * event)
{
	switch (key)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
		cocos2d::Director::getInstance()->end();
		break;

	default:
		break;
	}
}
