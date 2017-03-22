#include "MainCharacter.h"
#include "Definition.h"
#include "Scenes\LevelSelectionBoard.h"
#include "Scenes\FinishedScene.h"
#include "Instruction.h"
#include "GameSettings.h"
#include "Utility.h"
#include "SoundManager.h"


MainCharacter * MainCharacter::createMainCharacter(cocos2d::Layer * gameLayer, cocos2d::Vec2 initPos)
{
	MainCharacter *mc = new (std::nothrow) MainCharacter(gameLayer, initPos);
	if (mc && mc->initWithFile("images/main_character/main_character_idle_1.png")) {

		mc->autorelease();
		return mc;
	}

	CC_SAFE_DELETE(mc);
	return nullptr;
}


MainCharacter::MainCharacter(cocos2d::Layer * gameLayer, cocos2d::Vec2 initPos)
{
	started = false;
	arrived = false;
	this->setPosition(initPos);
	gameLayer->addChild(this, 100);

	initMainCharacterAnimation();
	setIdleAnimation();
}


MainCharacter::~MainCharacter()
{
	CC_SAFE_RELEASE(idleAnimation);
	CC_SAFE_RELEASE(runningAnimation);
}


void MainCharacter::setIdleAnimation()
{
	this->stopAllActionsByTag(RUNNING_ANIMATION_TAG);

	auto animate = cocos2d::Animate::create(idleAnimation);
	animate->setTag(IDLE_ANIMATION_TAG);
	auto repeatIdleAction = cocos2d::RepeatForever::create(animate);
	repeatIdleAction->setTag(IDLE_ANIMATION_TAG);
	this->runAction(repeatIdleAction);
}


void MainCharacter::setRunningAnimation()
{
	this->stopAllActionsByTag(IDLE_ANIMATION_TAG);
	
	auto animate = cocos2d::Animate::create(runningAnimation);
	animate->setTag(RUNNING_ANIMATION_TAG);
	auto repeatRunningAction = cocos2d::RepeatForever::create(animate);
	repeatRunningAction->setTag(RUNNING_ANIMATION_TAG);
	this->runAction(repeatRunningAction);
}


///////////////////////////////////////////////////////
// clear the path and moveSequence vector,
// assign the new path to path vector which is used for
// create the main character sequence movement
void MainCharacter::setPath(std::vector<cocos2d::Vec2>& path)
{
	this->path.clear();
	moveSequence.clear();
	
	this->path = path;
	moveSequence = createMoveSequence();
	moveSequence.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(MainCharacter::setIdleAnimation)));
	moveSequence.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(MainCharacter::removePhysicsBody)));
	moveSequence.pushBack(cocos2d::FadeOut::create(OPEN_NEXT_LEVEL_DURATION));
	moveSequence.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(MainCharacter::onArrived)));
}


void MainCharacter::resetPath()
{
	path.clear();
	moveSequence.clear();
}

////////////////////////////////////////////////////
// run the sequence action that is stored in moveSequence vector
void MainCharacter::move()
{
	started = true;
	setRunningAnimation();
	this->runAction(cocos2d::Sequence::create(moveSequence));
}

//////////////////////////////////////////////////
// return true if the moveSequence vector is not empty
bool MainCharacter::ready()
{
	return !path.empty();
}


bool MainCharacter::isStarted()
{
	return started;
}


bool MainCharacter::isArrived()
{
	return arrived;
}

////////////////////////////////////////////////////////
// this method will be called if main character collided with any obstacle
// to destroy main character, then go back to level setection board
void MainCharacter::beDestroyed()
{
	this->stopAllActions();
	this->setVisible(false);
	this->getPhysicsBody()->removeFromWorld();
}


bool MainCharacter::initMainCharacterAnimation()
{
	// init idle animation
	cocos2d::Vector<cocos2d::SpriteFrame *> idleFrames;
	
	int numOfIdleFrame = utils::countNumberOfFileWithFormat("images/main_character/main_character_idle_%d.png");
	for (int i = 1; i < 10; i++) {
		auto idleFileName = cocos2d::String::createWithFormat("images/main_character/main_character_idle_%d.png", 1);
		auto idleFrame = cocos2d::SpriteFrame::create(idleFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		idleFrames.pushBack(idleFrame);
	}
	idleAnimation = cocos2d::Animation::createWithSpriteFrames(idleFrames, 10.0F / 60.0F);
	idleAnimation->retain();

	// init running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> runningFrames;
	int numOfRunningFrames = utils::countNumberOfFileWithFormat("images/main_character/main_character_running_%d.png");
	for (int i = 1; i <= numOfRunningFrames; i++) {

		auto runningFileName = cocos2d::String::createWithFormat("images/main_character/main_character_running_%d.png", i);
		auto runningFrame = cocos2d::SpriteFrame::create(runningFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		runningFrames.pushBack(runningFrame);
	}
	runningAnimation = cocos2d::Animation::createWithSpriteFrames(runningFrames, (numOfRunningFrames * 2 / 60.0F));
	runningAnimation->retain();

	return true;
}


void MainCharacter::flipLeft()
{
	this->setFlippedX(true);
}


void MainCharacter::flipRight()
{
	this->setFlippedX(false);
}


cocos2d::Vector<cocos2d::FiniteTimeAction*> MainCharacter::createMoveSequence()
{
	cocos2d::Vector<cocos2d::FiniteTimeAction *> actions;

	for (int i = 1; i < path.size(); i++) {

		if (path[i].x > path[i - 1].x) {
			actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(MainCharacter::flipRight)));
		}
		else if (path[i].x < path[i - 1].x) {
			actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(MainCharacter::flipLeft)));
		}

		auto movement = cocos2d::MoveTo::create(MOVEMENT_DURATION_BETWEEN_TWO_NODE, path[i]);
		actions.pushBack(movement);
	}

	return actions;
}


void MainCharacter::onArrived()
{
	arrived = true;
	openNextLevel();

	// In-game instruction is no more needed
	GameSettings::getInstance()->disableInstruction();

	// Player can pass the first level, so we don't need instruction anymore
	GameSettings::getInstance()->disableInstruction();
	if (Instruction::hasInstance())
	{
		Instruction::destroyInstance();
	}

	// On the first time the player win the game (pass all level), 
	// transition to the finished scene.
	// On the next time and so on, just go back to level selection board
	if (	GameSettings::getInstance()->getSelectedLevel() == GameSettings::getInstance()->getTotalLevel()
		&& !GameSettings::getInstance()->wonTheGame()
		)
	{
		GameSettings::getInstance()->onPlayerWinAllLevel();
		replaceFinishedScene();
	}
	else
	{
		backToLevelSelectionBoard();
	}
}


void MainCharacter::removePhysicsBody()
{
	if (this->getPhysicsBody())
		this->getPhysicsBody()->removeFromWorld();
}


void MainCharacter::openNextLevel()
{
	GameSettings::getInstance()->enableLevel(GameSettings::getInstance()->getSelectedLevel() + 1);
	GameSettings::getInstance()->save();
}


void MainCharacter::backToLevelSelectionBoard()
{
	SoundManager::getInstance()->stopMusic();

	auto levelBoard = LevelSelectionBoard::createScene();
	cocos2d::Director::getInstance()->replaceScene(levelBoard);
}


void MainCharacter::replaceFinishedScene()
{
	SoundManager::getInstance()->stopMusic();

	auto finishedScene = FinishedScene::createScene();
	cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(2.0F, finishedScene));
}
