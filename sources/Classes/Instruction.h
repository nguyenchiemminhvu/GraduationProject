#pragma once

#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "cocos2d.h"
#include <vector>
#include <limits.h>

class HUD;

class Instruction
{
public:

	struct GuideLine
	{
		bool isAlreadyShowed;

		cocos2d::String guideText;
		cocos2d::Vec2	pointTo;
		float			xOffset;
		bool			isRightToLeft;

		GuideLine(cocos2d::String text, cocos2d::Vec2 pos, float xOffset = 0, bool isRtoL = true)
		{
			isAlreadyShowed = false;

			this->guideText = text;
			this->pointTo = pos;
			this->xOffset = xOffset;
			this->isRightToLeft = isRtoL;
		}
	};

	Instruction(HUD *hud, cocos2d::Vec2 startPos, cocos2d::Vec2 endPos, cocos2d::Vec2 buttonRunPos);
	virtual ~Instruction();

	void showSpeed();
	void showNextInstruction();


private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	HUD *hud;
	cocos2d::Vec2 startPos;
	cocos2d::Vec2 endPos;
	cocos2d::Vec2 buttonRunPos;

	std::vector<GuideLine> guideLines;

	void initGuideLines();
	void showInstruction(GuideLine instruction);
};

#endif // !__INSTRUCTION_H__
