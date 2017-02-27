#include "HUD.h"



HUD * HUD::createLayer(cocos2d::Layer *parent)
{
	HUD *hud = new HUD(parent);
	hud->create();
	hud->init();
	hud->autorelease();
	hud->setColor(cocos2d::Color3B::BLACK);
	
	hud->initComponents();

	hud->parentLayer->addChild(hud);

	return hud;
}


HUD::HUD(cocos2d::Layer *parent)
	: parentLayer(parent)
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


void HUD::showArrow(cocos2d::Vec2 pos)
{
	isArrowShowing = true;
	arrow->setPosition(pos);
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


void HUD::showTextBoard(cocos2d::String text)
{
	isTextBoardShowing = true;
	
	// setup scale up action
	auto scaleUp = cocos2d::ScaleTo::create(0.3F, 1.0F, 1.0F);
	textBoard->runAction(scaleUp);
}


void HUD::hideTextBoard()
{
	isTextBoardShowing = false;

	// setup scale down action
	auto scaleDown = cocos2d::ScaleTo::create(0.3F, 1.0F, 0.0F);
	textBoard->runAction(scaleDown);
}


void HUD::initComponents()
{
	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	loadArrow();
	loadTextBoard();
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

	this->addChild(textBoard);
}
