#include "HUD.h"
#include "Definition.h"
#include "Utility.h"

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
	arrow->setVisible(true);

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
}


void HUD::hideArrow()
{
	isArrowShowing = false;
	arrow->setPosition(cocos2d::Vec2(0xFFFFFFFF, 0xFFFFFFFF));
	arrow->setVisible(false);
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


void HUD::loadSpeed(std::map<int, float> speedForHud)
{
	this->speedInfo = speedForHud;

	/////////////////////////////////////////////////////
	// remove unnecessary elements

	if (speedInfo[(int)EnemyTypes::LEFT_RIGHT] != 0)
	{
		speedInfo.erase((int)EnemyTypes::UP_DOWN);
	}
	else
	{
		speedInfo.erase((int)EnemyTypes::LEFT_RIGHT);
	}

	if (speedInfo[(int)EnemyTypes::OUTSIDE_CLOCKWISE] != 0)
	{
		speedInfo.erase((int)EnemyTypes::OUTSIDE_ANTI_CLOCKWISE);
	}
	else
	{
		speedInfo.erase((int)EnemyTypes::OUTSIDE_CLOCKWISE);
	}

	if (speedInfo[(int)EnemyTypes::INSIDE_CLOCKWISE] != 0)
	{
		speedInfo.erase((int)EnemyTypes::INSIDE_ANTI_CLOCKWISE);
	}
	else
	{
		speedInfo.erase((int)EnemyTypes::INSIDE_CLOCKWISE);
	}

	//////////////////////////////////////////////////////
	// init layout to show speed info

	speedLayout = cocos2d::ui::Layout::create();
	speedLayout->setLayoutType(cocos2d::ui::Layout::Type::VERTICAL);
	speedLayout->setBackGroundColorType(cocos2d::ui::LayoutBackGroundColorType::NONE);
	speedLayout->setContentSize(cocos2d::Size(textBoard->getContentSize().width, textBoard->getContentSize().height));
	speedLayout->setAnchorPoint(cocos2d::Vec2(0.5F, 1.0F));
	speedLayout->setPosition(
		cocos2d::Vec2(
			textBoard->getPositionX(),
			textBoard->getPositionY() + textBoard->getContentSize().height / 2
		)
	);
	textBoard->getParent()->addChild(speedLayout);

	auto layoutColumnParam = cocos2d::ui::LinearLayoutParameter::create();
	layoutColumnParam->setGravity(cocos2d::ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
	layoutColumnParam->setMargin(cocos2d::ui::Margin(0, 20, 0, 30));

	auto layoutCollumnParam2 = cocos2d::ui::LinearLayoutParameter::create();
	layoutCollumnParam2->setGravity(cocos2d::ui::LinearLayoutParameter::LinearGravity::LEFT);
	layoutCollumnParam2->setMargin(cocos2d::ui::Margin(10, 0, 0, 60));

	auto layoutRowParam = cocos2d::ui::LinearLayoutParameter::create();
	layoutRowParam->setGravity(cocos2d::ui::LinearLayoutParameter::LinearGravity::CENTER_VERTICAL);
	layoutRowParam->setMargin(cocos2d::ui::Margin(10, 10, 10, 10));

	/////////////////////////////////////////////////////
	// add speed text first
	auto speedText = cocos2d::ui::ImageView::create("images/instruction/speed_text.png");
	speedText->setLayoutParameter(layoutColumnParam);
	speedLayout->addChild(speedText);

	/////////////////////////////////////////////////////
	// add main character speed
	auto mcSpeedLayout = cocos2d::ui::Layout::create();
	mcSpeedLayout->setLayoutParameter(layoutCollumnParam2);
	mcSpeedLayout->setLayoutType(cocos2d::ui::Layout::Type::HORIZONTAL);

	auto mcSprite = cocos2d::ui::ImageView::create("images/main_character/main_character_idle_1.png");
	mcSprite->setLayoutParameter(layoutRowParam);
	mcSpeedLayout->addChild(mcSprite);

	auto multiplication = cocos2d::ui::Text::create(" x ", "fonts/gt-pressura-regular.ttf", 30);
	multiplication->setLayoutParameter(layoutRowParam);
	mcSpeedLayout->addChild(multiplication);

	auto speedValue = cocos2d::ui::Text::create(std::to_string(MAIN_CHARACTER_BASE_SPEED), "fonts/gt-pressura-regular.ttf", 30);
	speedValue->setLayoutParameter(layoutRowParam);
	mcSpeedLayout->addChild(speedValue);

	speedLayout->addChild(mcSpeedLayout);

	/////////////////////////////////////////////////////
	// finally add list of speed of monsters
	std::map<int, float>::iterator iter;
	for (iter = speedInfo.begin(); iter != speedInfo.end(); iter++)
	{
		auto monsterSpeed = cocos2d::ui::Layout::create();
		monsterSpeed->setLayoutParameter(layoutCollumnParam2);
		monsterSpeed->setLayoutType(cocos2d::ui::Layout::Type::HORIZONTAL);
		
		cocos2d::ui::ImageView *monsterSprite = nullptr;
		if ((*iter).first == (int)EnemyTypes::LEFT_RIGHT || 
			(*iter).first == (int)EnemyTypes::UP_DOWN)
		{
			monsterSprite = cocos2d::ui::ImageView::create("images/enemies/type1or2/obstacle_speed.png");
		}
		else if ((*iter).first == (int)EnemyTypes::OUTSIDE_CLOCKWISE ||
				(*iter).first == (int)EnemyTypes::OUTSIDE_ANTI_CLOCKWISE)
		{
			monsterSprite = cocos2d::ui::ImageView::create("images/enemies/type3or4/obstacle_speed.png");
		}
		else if ((*iter).first == (int)EnemyTypes::INSIDE_CLOCKWISE ||
				(*iter).first == (int)EnemyTypes::INSIDE_ANTI_CLOCKWISE)
		{
			monsterSprite = cocos2d::ui::ImageView::create("images/enemies/type5or6/obstacle_speed.png");
		}

		monsterSprite->setLayoutParameter(layoutRowParam);
		monsterSpeed->addChild(monsterSprite);

		auto multiplication = cocos2d::ui::Text::create(" x ", "fonts/gt-pressura-regular.ttf", 30);
		multiplication->setLayoutParameter(layoutRowParam);
		monsterSpeed->addChild(multiplication);

		auto speedValue = cocos2d::ui::Text::create(std::to_string((*iter).second), "fonts/gt-pressura-regular.ttf", 30);
		speedValue->setLayoutParameter(layoutRowParam);
		monsterSpeed->addChild(speedValue);

		speedLayout->addChild(monsterSpeed);
	}
}


void HUD::showSpeed()
{

}


void HUD::hideSpeed()
{

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
	arrow = cocos2d::Sprite::create("images/instruction/arrow_1.png");
	arrow->setPosition(cocos2d::Vec2(0xFFFFFFFF, 0xFFFFFFFF));

	// setup arrow animation
	cocos2d::Vector<cocos2d::SpriteFrame *> frames;

	int numOfFrames = utils::countNumberOfFileWithFormat("images/instruction/arrow_%d.png");
	for (int i = 1; i <= numOfFrames; i++)
	{
		auto fileName = cocos2d::String::createWithFormat("images/instruction/arrow_%d.png", i);
		auto sprite = cocos2d::Sprite::create(fileName->getCString());
		auto frame = cocos2d::SpriteFrame::create(fileName->getCString(), cocos2d::Rect(cocos2d::Vec2(0, 0), sprite->getContentSize()));
		frames.pushBack(frame);
	}
	
	auto animation = cocos2d::Animation::createWithSpriteFrames(frames, (1.0F / 60.0F) * 6);
	auto animate = cocos2d::Animate::create(animation);
	arrow->runAction(cocos2d::RepeatForever::create(animate));

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
	text = cocos2d::ui::Text::create("", "fonts/gt-pressura-regular.ttf", 30);
	text->setAnchorPoint(cocos2d::Vec2(0.0F, 0.0F));
	text->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
	text->setTextVerticalAlignment(cocos2d::TextVAlignment::TOP);
	text->ignoreContentAdaptWithSize(false);
	text->setTextAreaSize(cocos2d::Size(textBoard->getContentSize().width - TEXT_PADDING_ON_BOARD * 2, textBoard->getContentSize().height - TEXT_PADDING_ON_BOARD * 2));
	text->setPositionX(text->getPositionX() + TEXT_PADDING_ON_BOARD);

	textBoard->addChild(text);

	this->addChild(textBoard);
}
