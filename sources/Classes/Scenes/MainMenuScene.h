#ifndef __MAIN_MENU_SCENE_H__
#define __MAIN_MENU_SCENE_H__

#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "pluginfacebook\PluginFacebook.h"
#endif

class MainMenu 
	: public cocos2d::Layer 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	, public sdkbox::FacebookListener
#endif
{

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
	void replaceTutorialLevel();

	///////////////////////////////////
	// facebook listener callbacks
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	virtual void onLogin(bool isLogin, const std::string& msg) {};
	virtual void onSharedSuccess(const std::string& message);
	virtual void onSharedFailed(const std::string& message);
	virtual void onSharedCancel() {};
	virtual void onAPI(const std::string& key, const std::string& jsonData) {};
	virtual void onPermission(bool isLogin, const std::string& msg) {};
	virtual void onFetchFriends(bool ok, const std::string& msg) {};
	virtual void onRequestInvitableFriends(const sdkbox::FBInvitableFriendsInfo& friends) {};
	virtual void onInviteFriendsWithInviteIdsResult(bool result, const std::string& msg) {};
	virtual void onInviteFriendsResult(bool result, const std::string& msg) {};

	virtual void onGetUserInfo(const sdkbox::FBGraphUser& userInfo) {};
#endif
	//////////////////////////////////
	// others

	void onButtonFAQTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);
	void onSoundEffectStateChanged(cocos2d::Ref *ref, cocos2d::ui::CheckBox::EventType type);
	void onButtonFacebookTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);
	void onButtonPlayTouched(cocos2d::Ref *ref, cocos2d::ui::Button::TouchEventType type);

};

#endif // !__MAIN_MENU_SCENE_H__