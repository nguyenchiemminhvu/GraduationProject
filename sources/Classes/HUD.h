#pragma once

#ifndef __HUD_H__
#define __HUD_H__

#include "cocos2d.h"
#include "ui\CocosGUI.h"

class HUD : public cocos2d::LayerColor
{
public:

	static HUD* createLayer(cocos2d::Layer *parent);

	HUD(cocos2d::Layer *parent);
	virtual ~HUD();

	virtual void update(float dt);
	virtual void draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, bool transformUpdated);

	void showArrow(cocos2d::Vec2 pos);
	void hideArrow();

	void showTextBoard(cocos2d::String newText);
	void hideTextBoard();
	void changeTextOnBoard(cocos2d::String newText);
	void clearTextOnBoard();

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	cocos2d::Layer *parentLayer;

	bool isArrowShowing;
	cocos2d::Sprite *arrow;

	bool isTextBoardShowing;
	cocos2d::Sprite *textBoard;
	cocos2d::ui::Text *text;

	void initComponents();
	void loadArrow();
	void loadTextBoard();

};

#endif // !__HUD_H__
