#ifndef __TOAST_H__
#define __TOAST_H__

#pragma once

#include "cocos2d.h"

class Toast : public cocos2d::LayerColor {
public:

	static void displayToast(cocos2d::Node *target, const char *text, float duration);

	Toast(cocos2d::Node *target, const char *text, float duration);
	virtual ~Toast();

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	cocos2d::LabelTTF *label;
	
	void dismiss();

	static Toast *instance;
};

#endif // !__TOAST_H__