#include "Instruction.h"
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
}


Instruction::~Instruction()
{
}


void Instruction::showSpeed()
{

}


void Instruction::showNextInstruction()
{
	for (int i = 0; i < guideLines.size(); i++)
	{
		if (!guideLines.at(i).isAlreadyShowed)
		{
			showInstruction(guideLines.at(i));
			guideLines.at(i).isAlreadyShowed = true;
		}
	}
}


void Instruction::initGuideLines()
{
	guideLines.clear();

	// say hello from instructor
	guideLines.push_back(
		GuideLine(
			"Hello!\nI'm your instructor.\nI'll guide you how to play this game!", 
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

	// first step: touch on start position
	guideLines.push_back(
		GuideLine(
			"Touch on main character and draw a path.",
			startPos
		)
	);

	// second step: warning player to draw the path carefully
	guideLines.push_back(
		GuideLine(
			"Try to choose the right path.\nAvoid those monster!",
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

	// third step: Specify the end of the path
	guideLines.push_back(
		GuideLine(
			"Your path must end at the next door.",
			endPos,
			0,
			false
		)
	);

	// fourth step: use button Run
	guideLines.push_back(
		GuideLine(
			"Now, hold the button Run then release to go!",
			buttonRunPos,
			60.0F,
			false
		)
	);

	// finally, just finished instruction
	guideLines.push_back(
		GuideLine(
			"",
			cocos2d::Vec2(INFINITE_POINT_TO, INFINITE_POINT_TO)
		)
	);

}


void Instruction::showInstruction(GuideLine instruction)
{
	hud->showTextBoard(instruction.guideText);
	hud->showArrow(instruction.pointTo, instruction.xOffset, instruction.isRightToLeft);
}
