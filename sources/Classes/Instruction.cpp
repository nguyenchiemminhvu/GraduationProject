#include "Instruction.h"
#include "Definition.h"
#include "GameSettings.h"
#include "HUD.h"

#define INFINITE_POINT_TO	(0xFFFFFFFF)


Instruction::Instruction(HUD *hud, cocos2d::Vec2 startPos, cocos2d::Vec2 endPos, cocos2d::Vec2 buttonRunPos)
{
	this->hud = hud;
	this->startPos = startPos;
	this->endPos = endPos;
	this->buttonRunPos = buttonRunPos;

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	initGuideLines();
	currentStep = InstructionStep::NONE;
}


Instruction::~Instruction()
{
}


Instruction::InstructionStep Instruction::getCurrentStep()
{
	return currentStep;
}


void Instruction::showSpeed()
{

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

	// switch state of the guide line
	guideLines.at(step - 1).isAlreadyShowed = true;
	
	// jump to next step
	currentStep = step;

	hud->showTextBoard(guideLines.at(step - 1).guideText);
	hud->showArrow(
		guideLines.at(step - 1).pointTo, 
		guideLines.at(step - 1).xOffset,
		guideLines.at(step - 1).isRightToLeft
	);
	cocos2d::log("\n%f %f\n", guideLines.at(step - 1).pointTo.x, guideLines.at(step - 1).pointTo.y);
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
			"Touch on main character and draw a path.",
			startPos
		)
	);

	// second step: warning player to draw the path carefully
	guideLines.push_back(
		GuideLine(
			InstructionStep::DRAW_THE_PATH,
			"Try to choose the right path.\nAvoid those monster!",
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

	// third step: Specify the end of the path
	guideLines.push_back(
		GuideLine(
			InstructionStep::SPECIFY_THE_END_POS,
			"Your path must end at the next door.",
			endPos,
			48,
			false
		)
	);

	// fourth step: use button Run
	guideLines.push_back(
		GuideLine(
			InstructionStep::USE_BUTTON_RUN,
			"Now, hold the button Run then release to go!",
			buttonRunPos,
			60.0F,
			false
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

	return false;
}


void Instruction::showInstruction(GuideLine instruction)
{
	hud->showTextBoard(instruction.guideText);
	hud->showArrow(instruction.pointTo, instruction.xOffset, instruction.isRightToLeft);
}
