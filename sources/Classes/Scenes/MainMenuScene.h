#ifndef __MAIN_MENU_SCENE_H__
#define __MAIN_MENU_SCENE_H__

#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"


class MainMenu : public cocos2d::Layer {

public:

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(MainMenu);

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	///////////////////////////////////
	// main menu initialization

	void initBackground();
	void initButtons();

	///////////////////////////////////
	// main menu scene transitions

	void replaceCreditsScene();
	void replaceStoryScene();
	void replaceLevelSelectionBoard();

	//////////////////////////////////
	// others

	void onButtonFAQTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);
	void onSoundEffectStateChanged(cocos2d::Ref *ref, cocos2d::ui::CheckBox::EventType type);
	void onButtonFacebookTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);
	void onButtonPlayTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);

};

#endif // !__MAIN_MENU_SCENE_H__