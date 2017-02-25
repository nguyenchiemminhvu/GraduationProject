#ifndef __CREDITS_SCENE_H__
#define __CREDITS_SCENE_H__

#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

class Credits : public cocos2d::Layer {

public:

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(Credits);

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	////////////////////////////////////
	// credits initializations
	void initKeyboardEventListener();

	////////////////////////////////////
	// credits scene transitions
	void replaceMainMenuScene();

	////////////////////////////////////
	// others
	void onKeyboardReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event);
};

#endif // !__CREDITS_SCENE_H__