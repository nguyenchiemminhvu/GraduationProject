#include "HUD.h"



HUD * HUD::createLayer()
{
	HUD *hud = new HUD();
	hud->create();
	hud->init();
	hud->autorelease();
	hud->setColor(cocos2d::Color3B::BLACK);
	
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


void HUD::onGameInstructionShow()
{

}


void HUD::onGameInstructionClose()
{

}


void HUD::initComponents()
{
	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();


}
