#include "HUD.h"
#include "Definition.h"

#define TEXT_PADDING_ON_BOARD 10.0F


HUD * HUD::createLayer()
{
	HUD *hud = new HUD();
	hud->init();
	hud->autorelease();

	hud->initComponents();

	return hud;
}


HUD::HUD()
{

}


HUD::~HUD()
{

}


void HUD::update(float dt)
{

}


void HUD::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, bool transformUpdated)
{

}


void HUD::showArrow(cocos2d::Vec2 pos, float xOffset, bool rightToLeft)
{
	isArrowShowing = true;
	
	if (!rightToLeft)
	{
		arrow->setFlippedX(true);
		arrow->setPositionX(pos.x - (arrow->getContentSize().width + xOffset));
		arrow->setPositionY(pos.y);
	}
	else
	{
		arrow->setFlippedX(false);
		arrow->setPositionX(pos.x + (arrow->getContentSize().width + xOffset));
		arrow->setPositionY(pos.y);
	}

	arrow->setVisible(true);

	arrow->resumeSchedulerAndActions();
}


void HUD::hideArrow()
{
	isArrowShowing = false;
	arrow->setPosition(cocos2d::Vec2());
	arrow->setVisible(false);

	arrow->pauseSchedulerAndActions();
}


void HUD::showTextBoard(cocos2d::String newText)
{
	if (newText.length() == 0)
	{
		hideTextBoard();
		return;
	}

	if (isTextBoardShowing)
	{
		// clear text on text board and make it invisible
		textBoard->setScaleY(0.0F);
		if (text)
		{
			text->setText("");
		}
	}

	isTextBoardShowing = true;
	text->setText(newText.getCString());

	// setup scale up action
	auto scaleUp = cocos2d::ScaleTo::create(0.3F, 1.0F, 1.0F);
	textBoard->runAction(scaleUp);
}


void HUD::hideTextBoard()
{
	isTextBoardShowing = false;

	// setup scale down action
	auto scaleDown = cocos2d::ScaleTo::create(0.3F, 1.0F, 0.0F);
	textBoard->runAction(
		cocos2d::Sequence::create(
			scaleDown,
			cocos2d::CallFunc::create(this, callfunc_selector(HUD::clearTextOnBoard)),
			NULL
		)
	);
}


void HUD::changeTextOnBoard(cocos2d::String newText)
{
	showTextBoard(newText);
}


void HUD::clearTextOnBoard()
{
	if (text)
	{
		text->setText("");
	}
}


void HUD::initComponents()
{
	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	loadTextBoard();
	loadArrow();
}


void HUD::loadArrow()
{
	// load arrow texture
	arrow = cocos2d::Sprite::create("images/instruction/arrow.png");

	// setup arrow sequence action
	auto moveRight = cocos2d::MoveBy::create(0.5F, cocos2d::Vec2(arrow->getContentSize().width, 0));
	auto moveLeft = cocos2d::MoveBy::create(0.5F, cocos2d::Vec2(-(arrow->getContentSize().width), 0));
	auto arrowSequenceAction = cocos2d::Sequence::create(moveRight, moveLeft, NULL);
	arrow->runAction(cocos2d::RepeatForever::create(arrowSequenceAction));

	// make arrow invisible
	hideArrow();

	this->addChild(arrow);
}


void HUD::loadTextBoard()
{
	// load text board texture
	textBoard = cocos2d::Sprite::create("images/instruction/text_board.png");

	// set fixed position
	textBoard->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width - textBoard->getContentSize().width / 2 - 15,
			origin.y + visibleSize.height / 2
		)
	);

	// make text board invisible
	isTextBoardShowing = false;
	textBoard->setScaleY(0.0F);

	// create empty Text
	text = cocos2d::ui::Text::create("", "fonts/Schoolbook.ttf", 30);
	text->setAnchorPoint(cocos2d::Vec2(0.0F, 0.0F));
	text->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
	text->setTextVerticalAlignment(cocos2d::TextVAlignment::TOP);
	text->ignoreContentAdaptWithSize(false);
	text->setTextAreaSize(cocos2d::Size(textBoard->getContentSize().width - TEXT_PADDING_ON_BOARD * 2, textBoard->getContentSize().height - TEXT_PADDING_ON_BOARD * 2));
	text->setPositionX(text->getPositionX() + TEXT_PADDING_ON_BOARD);

	textBoard->addChild(text);

	this->addChild(textBoard);
}
