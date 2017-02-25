#include "FinishedScene.h"
#include "Scenes\MainMenuScene.h"
#include "Player\MainCharacter.h"
#include "Utility.h"

cocos2d::Scene * FinishedScene::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto finished = FinishedScene::create();
	scene->addChild(finished);
	return scene;
}


FinishedScene::~FinishedScene()
{

}


bool FinishedScene::init()
{
	if (!cocos2d::Layer::init())
		return false;

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	initBackground();
	initMainCharacter();
	initMainCharacterForChasingAction();
	initMonstersForChasingAction();
	initMainCharacterFriends();
	initCredits();
	initSequenceAction();
	initKeyEventListener();

	return true;
}


void FinishedScene::initBackground()
{
	background = cocos2d::Sprite::create("images/UI/finished_scene_ui/finished_scene_background.png");
	background->setAnchorPoint(cocos2d::Vec2(0.0F, 0.0F));
	background->setPosition(
		cocos2d::Vec2(
			origin.x,
			origin.y
		)
	);
	this->addChild(background);
}


void FinishedScene::initMainCharacter()
{
	// load base sprite for main character
	auto mcSprite = cocos2d::Sprite::create("images/UI/finished_scene_ui/main_character/main_character_1.png");
	mcSprite->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 1.8F
		)
	);
	background->addChild(mcSprite);

	// count number of frames need to use for main character
	int countedFrames = utils::countNumberOfFileWithFormat("images/UI/finished_scene_ui/main_character/main_character_%d.png");
	
	// load sprites for animation
	cocos2d::Vector<cocos2d::SpriteFrame *> frames;
	for (int i = 1; i <= countedFrames; i++)
	{
		auto frameFile = cocos2d::String::createWithFormat("images/UI/finished_scene_ui/main_character/main_character_%d.png", i);
		auto tempSprite = cocos2d::Sprite::create(frameFile->getCString());
		auto frame = cocos2d::SpriteFrame::create(frameFile->getCString(), cocos2d::Rect(0, 0, tempSprite->getContentSize().width, tempSprite->getContentSize().height));
		frames.pushBack(frame);
	}

	auto animation = cocos2d::Animation::createWithSpriteFrames(frames, (1.0F / 60.0F) * 10);
	auto animate = cocos2d::Animate::create(animation);

	mcSprite->runAction(cocos2d::RepeatForever::create(animate));
}


void FinishedScene::initMainCharacterForChasingAction()
{
	// create a main character
	mainCharacter = MainCharacter::createMainCharacter(this, cocos2d::Vec2(0, 0));
	mainCharacter->setScale(2.0F);
	mainCharacter->setPosition(
		cocos2d::Vec2(
			origin.x - mainCharacter->getContentSize().width * 3,
			origin.y + visibleSize.height / 2
		)
	);
	mainCharacter->setRunningAnimation();
}


void FinishedScene::initMonstersForChasingAction()
{
	// load enemy sprite
	enemy = cocos2d::Sprite::create("images/enemies/type5or6/obstacle_running_1.png");
	enemy->setScale(2.0F);
	enemy->setPosition(
		cocos2d::Vec2(
			mainCharacter->getPositionX(),
			mainCharacter->getPositionY()
		)
	);
	enemy->setFlippedX(true);
	this->addChild(enemy);

	enemy2 = cocos2d::Sprite::create("images/enemies/type3or4/obstacle_running_1.png");
	enemy2->setScale(2.0F);
	enemy2->setPosition(
		cocos2d::Vec2(
			mainCharacter->getPositionX(),
			mainCharacter->getPositionY()
		)
	);
	enemy2->setVisible(false);
	this->addChild(enemy2);

	// load enemy's running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> frames;
	int countedFrames = utils::countNumberOfFileWithFormat("images/enemies/type5or6/obstacle_running_%d.png");
	for (int i = 1; i <= countedFrames; i++)
	{
		auto fileName = cocos2d::String::createWithFormat("images/enemies/type5or6/obstacle_running_%d.png", i);
		auto tempSprite = cocos2d::Sprite::create(fileName->getCString());
		auto frame = cocos2d::SpriteFrame::create(fileName->getCString(), cocos2d::Rect(0, 0, tempSprite->getContentSize().width, tempSprite->getContentSize().height));
		frames.pushBack(frame);
	}
	auto animation = cocos2d::Animation::createWithSpriteFrames(frames, (1.0F / 60.0F) * 7);
	auto animate = cocos2d::Animate::create(animation);
	enemy->runAction(cocos2d::RepeatForever::create(animate));

	cocos2d::Vector<cocos2d::SpriteFrame *> frames2;
	int countedFrames2 = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_running_%d.png");
	for (int i = 1; i <= countedFrames2; i++)
	{
		auto fileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_running_%d.png", i);
		auto tempSprite = cocos2d::Sprite::create(fileName->getCString());
		auto frame = cocos2d::SpriteFrame::create(fileName->getCString(), cocos2d::Rect(0, 0, tempSprite->getContentSize().width, tempSprite->getContentSize().height));
		frames2.pushBack(frame);
	}
	auto animation2 = cocos2d::Animation::createWithSpriteFrames(frames2, (1.0F/ 60.0F) * 7);
	auto animate2 = cocos2d::Animate::create(animation2);
	enemy2->runAction(cocos2d::RepeatForever::create(animate2));
}


void FinishedScene::initMainCharacterFriends()
{
	loadMainCharacterFriendsAnimation();
	setupAllFriendsPosition();
}


void FinishedScene::initCredits()
{
	credits = cocos2d::Sprite::create("images/UI/finished_scene_ui/credits.png");
	credits->setAnchorPoint(cocos2d::Vec2(0.0F, 1.0F));
	credits->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2 - credits->getContentSize().width / 2,
			origin.y
		)
	);
	this->addChild(credits);
}


void FinishedScene::initSequenceAction()
{
	// init sequence action for background
	backgroundSequence = cocos2d::Sequence::create(
		cocos2d::DelayTime::create(WAIT_FOR_SCROLL_BACKGROUND_DURATION),
		cocos2d::MoveBy::create(SCROLL_BACKGROUND_DURATION, cocos2d::Vec2(0, -(visibleSize.height))),
		NULL
	);

	// init sequence action for credits
	creditsSequence = cocos2d::Sequence::create(
		cocos2d::DelayTime::create(WAIT_FOR_SCROLL_BACKGROUND_DURATION + SCROLL_BACKGROUND_DURATION),
		cocos2d::MoveBy::create(SCROLL_CREDITS_DURATION, cocos2d::Vec2(0, visibleSize.height + credits->getContentSize().height)),
		NULL
	);

	// create sequence action for main character
	mainCharacterSequence = cocos2d::Sequence::create(
		cocos2d::DelayTime::create(WAIT_FOR_SCROLL_BACKGROUND_DURATION + SCROLL_BACKGROUND_DURATION),
		cocos2d::MoveBy::create(MAIN_CHARACTER_RUNNING_DURATION, cocos2d::Vec2(visibleSize.width + mainCharacter->getContentSize().width * 6, 0)),
		cocos2d::CallFunc::create(this, callfunc_selector(FinishedScene::flipMainCharacterX)),
		cocos2d::DelayTime::create(MAIN_CHARACTER_RUNNING_DURATION),
		cocos2d::MoveBy::create(MAIN_CHARACTER_RUNNING_DURATION, cocos2d::Vec2(-(visibleSize.width + mainCharacter->getContentSize().width * 6), 0)),
		NULL
	);

	// create sequence action for enemy
	enemySequence = cocos2d::Sequence::create(
		cocos2d::DelayTime::create(WAIT_FOR_SCROLL_BACKGROUND_DURATION + SCROLL_BACKGROUND_DURATION + 1),
		cocos2d::MoveBy::create(MAIN_CHARACTER_RUNNING_DURATION, cocos2d::Vec2(visibleSize.width + mainCharacter->getContentSize().width * 6, 0)),
		NULL
	);

	// create sequence action for enemy 2
	enemy2Sequence = cocos2d::Sequence::create(
		cocos2d::DelayTime::create(WAIT_FOR_SCROLL_BACKGROUND_DURATION + SCROLL_BACKGROUND_DURATION + 1),
		cocos2d::MoveBy::create(MAIN_CHARACTER_RUNNING_DURATION, cocos2d::Vec2(visibleSize.width + mainCharacter->getContentSize().width * 6, 0)),
		cocos2d::CallFunc::create(this, callfunc_selector(FinishedScene::setEnemy2Visible)),
		cocos2d::DelayTime::create(MAIN_CHARACTER_RUNNING_DURATION),
		cocos2d::MoveBy::create(MAIN_CHARACTER_RUNNING_DURATION, cocos2d::Vec2(-(visibleSize.width + mainCharacter->getContentSize().width * 6), 0)),
		NULL
	);

	// create sequence action for finished scene
	finishedSceneSequence = cocos2d::Sequence::create(
		cocos2d::DelayTime::create(WAIT_FOR_SCROLL_BACKGROUND_DURATION + SCROLL_BACKGROUND_DURATION + SCROLL_CREDITS_DURATION + 1),
		cocos2d::CallFunc::create(this, callfunc_selector(FinishedScene::replaceMainMenu)),
		NULL
	);

	// run all sequence actions at the same time
	runSequenceActions();
}


void FinishedScene::initKeyEventListener()
{
	auto keyListener = cocos2d::EventListenerKeyboard::create();

	keyListener->onKeyReleased = CC_CALLBACK_2(FinishedScene::onButtonBackTouched, this);

	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
}


void FinishedScene::replaceMainMenu()
{
	auto mainMenu = MainMenu::createScene();
	cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(2.0F, mainMenu));
}


void FinishedScene::onButtonBackTouched(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event)
{
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_BACK:
		replaceMainMenu();
		break;

	default:
		break;
	}
}


void FinishedScene::loadMainCharacterFriendsAnimation()
{
	int num_of_friends = utils::countNumberOfFolderWithFormat("images/UI/finished_scene_ui/friends/friend_%d");

	for (int i = 1; i <= num_of_friends; i++)
	{
		// parse the format of animation for friend i-th
		auto pathPreFormat = cocos2d::String::createWithFormat("images/UI/finished_scene_ui/friends/friend_%d/friend_%d_%s.png", i, i, "%d");

		// counting number of frame need to load for friend i-th animation
		int countedFrames = utils::countNumberOfFileWithFormat(pathPreFormat->getCString());

		// load base sprite of friend i-th
		auto friendSprite = utils::loadBaseSpriteWithFormat(pathPreFormat->getCString());
		friendSprite->setAnchorPoint(cocos2d::Vec2(0.0F, 0.0F));
		friends.pushBack(friendSprite);

		cocos2d::Vector<cocos2d::SpriteFrame *> frames;
		for (int frameIndex = 1; frameIndex <= countedFrames; frameIndex++)
		{
			auto frameFile = cocos2d::String::createWithFormat(pathPreFormat->getCString(), frameIndex);
			auto tempSprite = cocos2d::Sprite::create(frameFile->getCString());

			auto frame = cocos2d::SpriteFrame::create(frameFile->getCString(), cocos2d::Rect(0, 0, tempSprite->getContentSize().width, tempSprite->getContentSize().height));
			frames.pushBack(frame);
		}

		auto animation = cocos2d::Animation::createWithSpriteFrames(frames, (1.0F / 60.0F) * cocos2d::random(5, 7));
		auto animate = cocos2d::Animate::create(animation);
		auto repeateJumpAction = cocos2d::RepeatForever::create(animate);
		friendSprite->runAction(repeateJumpAction);
	}
}


void FinishedScene::setupAllFriendsPosition()
{
	// adjust position of 5 sprites here
	// friends.at(2) is the plant

	friends.at(0)->setPosition(
		cocos2d::Vec2(
			origin.x + friends.at(0)->getContentSize().width / 3.0F,
			origin.y
		)
	);

	friends.at(1)->setPosition(
		cocos2d::Vec2(
			origin.x + friends.at(1)->getContentSize().width / 1.5F,
			origin.y
		)
	);

	friends.at(2)->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2 - friends.at(2)->getContentSize().width / 2.2F,
			origin.y
		)
	);

	friends.at(3)->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2 + friends.at(3)->getContentSize().width / 1.2F,
			origin.y
		)
	);

	friends.at(4)->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2 + friends.at(4)->getContentSize().width / 1.4F,
			origin.y
		)
	);

	// add all sprites to background children list
	background->addChild(friends.at(0));
	background->addChild(friends.at(3));
	background->addChild(friends.at(1));
	background->addChild(friends.at(4));
	background->addChild(friends.at(2));
}


void FinishedScene::runSequenceActions()
{
	background->runAction(backgroundSequence);
	credits->runAction(creditsSequence);
	mainCharacter->runAction(mainCharacterSequence);
	enemy->runAction(enemySequence);
	enemy2->runAction(enemy2Sequence);
	this->runAction(finishedSceneSequence);
}


void FinishedScene::setEnemy2Visible()
{
	enemy2->setVisible(true);
}


void FinishedScene::flipMainCharacterX()
{
	mainCharacter->setFlippedX(!mainCharacter->isFlippedX());
}

