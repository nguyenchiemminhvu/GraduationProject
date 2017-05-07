#pragma once

#ifndef __HUD_H__
#define __HUD_H__

#include "cocos2d.h"
#include "ui\CocosGUI.h"

#include <map>

class HUD : public cocos2d::Layer
{
public:

	static HUD* createLayer();

	HUD();
	virtual ~HUD();

	virtual void update(float dt);
	virtual void draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, bool transformUpdated);

	void showArrow(cocos2d::Vec2 pos, float xOffset = 0, bool rightToLeft = true);
	void showArrowPointToTextBoard();
	void hideArrow();

	void showTextBoard(cocos2d::String newText);
	void hideTextBoard();
	void changeTextOnBoard(cocos2d::String newText);
	void clearTextOnBoard();

	void loadSpeed(std::map<int, float> speedForHud);
	void showSpeed();
	void hideSpeed();

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	bool isArrowShowing;
	cocos2d::Sprite *arrow;

	bool					isTextBoardShowing;
	cocos2d::Sprite			*textBoard;
	cocos2d::ui::Text		*text;
	cocos2d::ui::Layout		*speedLayout;
	cocos2d::Sprite			*speedText;
	std::map<int, float>	speedInfo;

	void initComponents();
	void loadArrow();
	void loadTextBoard();

	void setSpeedVisible();
	void setSpeedInvisible();
};

#endif // !__HUD_H__
