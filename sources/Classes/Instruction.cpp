#include "Instruction.h"
#include "Definition.h"
#include "GameSettings.h"
#include "HUD.h"

#define INFINITE_POINT_TO	(0xFFFFFFFF)


Instruction * Instruction::instance = NULL;

Instruction * Instruction::getInstance()
{
	if (instance == NULL)
	{
		instance = new Instruction();
	}
	return instance;
}


void Instruction::destroyInstance()
{
	SAFE_DELETE(instance);
}


bool Instruction::hasInstance()
{
	return instance != NULL;
}


Instruction::Instruction()
{
	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	isFirstTime = true;
	currentStep = InstructionStep::NONE;
}


Instruction::~Instruction()
{
}


void Instruction::loadRequirement(HUD * hud, cocos2d::Vec2 startPos, cocos2d::Vec2 endPos, cocos2d::Vec2 buttonRunPos)
{
	this->hud = hud;
	this->startPos = startPos;
	this->endPos = endPos;
	this->buttonRunPos = buttonRunPos;

	initGuideLines();
}


bool Instruction::isShowFirstTime()
{
	return isFirstTime;
}


Instruction::InstructionStep Instruction::getCurrentStep()
{
	return currentStep;
}


void Instruction::showSpeed()
{
	hud->showSpeed();
}


void Instruction::hideSpeed()
{
	hud->hideSpeed();
}


void Instruction::showInstruction(InstructionStep step)
{
	if (!GameSettings::getInstance()->isInstructionNeeded())
	{
		return;
	}

	if (guideLines.at(step - 1).isAlreadyShowed)
	{
		return;
	}

	if (!checkPreCondition(step))
	{
		return;
	}

	isFirstTime = false;

	// switch state of the guide line
	guideLines.at(step - 1).isAlreadyShowed = true;
	
	// jump to next step
	currentStep = step;

	// special case
	if (step == InstructionStep::DRAW_THE_PATH)
	{
		hud->showArrowPointToTextBoard();
		hud->showSpeed();
		return;
	}

	hud->showTextBoard(guideLines.at(step - 1).guideText);
	hud->showArrow(
		guideLines.at(step - 1).pointTo, 
		guideLines.at(step - 1).xOffset,
		guideLines.at(step - 1).isRightToLeft
	);
}


void Instruction::initGuideLines()
{
	guideLines.clear();

	// say hello from instructor
	guideLines.push_back(
		GuideLine(
			InstructionStep::SAY_HELLO,
			"Hello!\nI'm your instructor.\nI'll guide you how to play this game!", 
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

	// first step: touch on start position
	guideLines.push_back(
		GuideLine(
			InstructionStep::TOUCH_AT_START_POS,
			"Touch that guy and draw a path!",
			startPos
		)
	);

	// second step: warning player to draw the path carefully
	guideLines.push_back(
		GuideLine(
			InstructionStep::DRAW_THE_PATH,
			"Try to avoid monsters!",
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

	// third step: Specify the end of the path
	guideLines.push_back(
		GuideLine(
			InstructionStep::SPECIFY_THE_END_POS,
			"The path must end at the next door.",
			endPos,
			0,
			false
		)
	);

	// fourth step: use button Run
	guideLines.push_back(
		GuideLine(
			InstructionStep::USE_BUTTON_RUN,
			"Now, hold the button Run! And choose the right time to release.",
			buttonRunPos,
			30.0F,
			false
		)
	);

	// good luck
	guideLines.push_back(
		GuideLine(
			InstructionStep::GOOD_LUCK,
			"Good luck!",
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

	// when main character dead
	guideLines.push_back(
		GuideLine(
			InstructionStep::MISSION_FAILED,
			"!!!...",
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

	// finally, just finished instruction
	guideLines.push_back(
		GuideLine(
			InstructionStep::FINISHED_INSTRUCTION,
			"",
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

}


bool Instruction::checkPreCondition(Instruction::InstructionStep step)
{
	if (step == currentStep + 1)
	{
		return true;
	}

	// check if all steps before was showed
	for (int i = 0; i < currentStep - 1; i++)
	{
		if (!guideLines.at(i).isAlreadyShowed)
			return false;
	}

	return true;
}


void Instruction::showInstruction(GuideLine instruction)
{
	hud->showTextBoard(instruction.guideText);
	hud->showArrow(instruction.pointTo, instruction.xOffset, instruction.isRightToLeft);
}
