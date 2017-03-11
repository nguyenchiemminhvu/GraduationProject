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
	
	enum InstructionStep
	{
		NONE = 0,
		SAY_HELLO,
		TOUCH_AT_START_POS,
		DRAW_THE_PATH,
		SPECIFY_THE_END_POS,
		USE_BUTTON_RUN,
		FINISHED_INSTRUCTION
	};

	struct GuideLine
	{
		int ID;
		bool isAlreadyShowed;

		cocos2d::String guideText;
		cocos2d::Vec2	pointTo;
		float			xOffset;
		bool			isRightToLeft;

		GuideLine()
		{
			this->ID = InstructionStep::NONE;
		}

		GuideLine(int id, cocos2d::String text, cocos2d::Vec2 pos, float xOffset = 0, bool isRtoL = true)
		{
			this->ID = id;
			isAlreadyShowed = false;

			this->guideText = text;
			this->pointTo = pos;
			this->xOffset = xOffset;
			this->isRightToLeft = isRtoL;
		}

		GuideLine(const GuideLine &other)
		{
			this->ID = other.ID;
			this->isAlreadyShowed = other.isAlreadyShowed;

			this->guideText = other.guideText;
			this->pointTo = other.pointTo;
			this->xOffset = other.xOffset;
			this->isRightToLeft = other.isRightToLeft;
		}
	};

	////////////////////////////////////////
	// implement singleton pattern
	static Instruction* getInstance();
	static void destroyInstance();
	static bool hasInstance();

	virtual ~Instruction();

	void loadRequirement(HUD *hud, cocos2d::Vec2 startPos, cocos2d::Vec2 endPos, cocos2d::Vec2 buttonRunPos);

	bool isShowFirstTime();
	InstructionStep getCurrentStep();

	void showSpeed();
	void showInstruction(InstructionStep step);

private:

	static Instruction *instance;

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

	bool isFirstTime;

	HUD *hud;
	cocos2d::Vec2 startPos;
	cocos2d::Vec2 endPos;
	cocos2d::Vec2 buttonRunPos;

	std::vector<GuideLine>	guideLines;
	InstructionStep			currentStep;

	Instruction();

	void initGuideLines();
	
	bool checkPreCondition(InstructionStep step);
	void showInstruction(GuideLine instruction);

};

#endif // !__INSTRUCTION_H__
