#pragma once

#ifndef __HUD_H__
#define __HUD_H__

#include "cocos2d.h"

class HUD : public cocos2d::LayerColor
{
public:

	static HUD* createLayer();

	HUD();
	virtual ~HUD();

	virtual void update(float dt);
	virtual void draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, bool transformUpdated);

	void onGameInstructionShow();
	void onGameInstructionClose();

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	void initComponents();

};

#endif // !__HUD_H__
