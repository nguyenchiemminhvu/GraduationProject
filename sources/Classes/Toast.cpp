#include "Toast.h"


Toast* Toast::instance = nullptr;

void Toast::displayToast(cocos2d::Node * target, const char * text, float duration)
{
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
	Toast *t = new Toast(target, text, duration);
}


Toast::Toast(cocos2d::Node * target, const char * text, float duration)
{
	if (!cocos2d::LayerColor::init()) {
		return;
	}

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	label = cocos2d::LabelTTF::create(text, "fonts/Schoolbook.ttf", 30);
	label->setFontFillColor(cocos2d::Color3B::GRAY);
	label->setCascadeOpacityEnabled(true);
	label->setOpacity(255);
	this->addChild(label);

	this->setCascadeOpacityEnabled(true);
	this->setOpacity(255);
	this->setContentSize(cocos2d::Size());
	this->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2,
			origin.y + label->getContentSize().height
		)
	);
	
	target->addChild(this);

	this->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(duration),
			cocos2d::FadeOut::create(1.0F),
			cocos2d::CallFunc::create(this, callfunc_selector(Toast::dismiss)),
			NULL
		)
	);
}


Toast::~Toast()
{

}


void Toast::dismiss()
{
	this->removeFromParentAndCleanup(true);
}
