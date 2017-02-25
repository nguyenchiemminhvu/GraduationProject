#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#pragma once

#include "cocos2d.h"

class SplashScene : public cocos2d::Layer
{
public:

	static cocos2d::Scene* createScene();
	virtual bool init();

	CREATE_FUNC(SplashScene);
	
private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	cocos2d::Sprite *splash;

	void initSplash();
	void initKeyboardEventListener();
	void replaceMainMenuScene(float dt);

	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event);
};

#endif //__SPLASH_SCENE_H__