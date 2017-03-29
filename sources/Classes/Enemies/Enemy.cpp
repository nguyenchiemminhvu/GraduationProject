#include "Enemy.h"
#include "Utility.h"
#include "Scenes\GameScene.h"


//////////////////////////////////////////////////////
// Enemy
//////////////////////////////////////////////////////

Enemy::Enemy(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
{
	this->gameLayer = gameLayer;
	this->gameLayer->addChild(this, (int)ZOrderLayer::LAYER_10);
	this->setPosition(pos);
	this->previousPos = this->getPosition();
	setStartDirection(startDir);
	this->speed = speed;

	isRunningAction = false;
	
	actionManager = cocos2d::Director::getInstance()->getActionManager();
	this->scheduleUpdate();
}


Enemy::~Enemy()
{
	CC_SAFE_RELEASE(idleAnimation);
	CC_SAFE_RELEASE(runningAnimation);
	CC_SAFE_RELEASE(eatingAnimation);
}


////////////////////////////////////////////////////
// Updating each frame, keep enemy moving.
// Check if there is no action which is running on this enemy, then
// calculate the next movement and implement the next subsequent action
void Enemy::update(float dt)
{
	if (previousPos.x > this->getPositionX())
	{
		flipLeft();
	}

	else if (previousPos.x < this->getPositionX())
	{
		flipRight();
	}
	previousPos = this->getPosition();

	if (!isRunningAction) {
		setRunningAction();
		setupNextMove();
		move();
	}
}


void Enemy::setStartDirection(int startDir)
{
	switch (startDir)
	{
	case (int)EnemyStartDirection::UP:
		direction = VECTOR_UP;
		break;

	case (int)EnemyStartDirection::DOWN:
		direction = VECTOR_DOWN;
		break;
	
	case (int)EnemyStartDirection::LEFT:
		direction = VECTOR_LEFT;
		break;
	
	case (int)EnemyStartDirection::RIGHT:
		direction = VECTOR_RIGHT;
		break;
	
	default:
		setDefaultStartDirection();
		break;
	}
}

////////////////////////////////////////////////////
// return true if there is at least one subsequence action 
// in the action vector
bool Enemy::readyToMove()
{
	return !nextMove.empty();
}


void Enemy::setIdleAnimation()
{
	this->stopAllActionsByTag(IDLE_ANIMATION_TAG);
	this->stopAllActionsByTag(RUNNING_ANIMATION_TAG);
	this->stopAllActionsByTag(EATING_ANIMATION_TAG);

	auto animate = cocos2d::Animate::create(idleAnimation);
	animate->setTag(IDLE_ANIMATION_TAG);
	auto repeatIdleAction = cocos2d::RepeatForever::create(animate);
	repeatIdleAction->setTag(IDLE_ANIMATION_TAG);
	this->runAction(repeatIdleAction);
}


void Enemy::setRunningAnimation()
{
	this->stopAllActionsByTag(IDLE_ANIMATION_TAG);
	this->stopAllActionsByTag(RUNNING_ANIMATION_TAG);
	this->stopAllActionsByTag(EATING_ANIMATION_TAG);

	auto animate = cocos2d::Animate::create(runningAnimation);
	animate->setTag(RUNNING_ANIMATION_TAG);
	auto repeatIdleAction = cocos2d::RepeatForever::create(animate);
	repeatIdleAction->setTag(RUNNING_ANIMATION_TAG);
	this->runAction(repeatIdleAction);
}


void Enemy::setEatingAnimation()
{
	this->stopAllActionsByTag(RUNNING_ANIMATION_TAG);
	this->stopAllActionsByTag(IDLE_ANIMATION_TAG);
	this->stopAllActionsByTag(EATING_ANIMATION_TAG);

	auto animate = cocos2d::Animate::create(eatingAnimation);
	animate->setTag(EATING_ANIMATION_TAG);
	auto repeatIdleAction = cocos2d::Repeat::create(animate, 1);
	repeatIdleAction->setTag(EATING_ANIMATION_TAG);
	this->runAction(repeatIdleAction);
}


///////////////////////////////////////////////////
// run the next subsequence action store in action vector
void Enemy::move()
{
	auto seq = cocos2d::Sequence::create(nextMove);
	this->runAction(seq);
}

//////////////////////////////////////////////////
// store the next subsequence action into action vector
void Enemy::setupNextMove()
{
	nextMove.clear();
	nextMove = createNextMoveSequence();
}


void Enemy::setRunningAction()
{
	isRunningAction = true;
}


void Enemy::stopRunningAction()
{
	isRunningAction = false;
}

////////////////////////////////////////////////
// return a normal vector to the left of the given vector
cocos2d::Vec2 Enemy::normalLeft(cocos2d::Vec2 v)
{
	return cocos2d::Vec2(-v.y, v.x);
}

///////////////////////////////////////////////
// return a normal vector to the right of the given vector
cocos2d::Vec2 Enemy::normalRight(cocos2d::Vec2 v)
{
	return cocos2d::Vec2(v.y, -v.x);
}


void Enemy::turnLeft()
{
	direction = normalLeft(direction);
}


void Enemy::turnRight()
{
	direction = normalRight(direction);
}


void Enemy::turnBackX()
{
	direction = cocos2d::Vec2(-direction.x, direction.y);
}


void Enemy::turnBackY()
{
	direction = cocos2d::Vec2(direction.x, -direction.y);
}


void Enemy::turnBack()
{
	turnBackX();
	turnBackY();
}


void Enemy::flipLeft()
{
	this->setFlippedX(false);
}


void Enemy::flipRight()
{
	this->setFlippedX(true);
}


///////////////////////////////////////////////////////
// return a MoveBy action that a node will perform, 
// that node will move up one tile in tiled map
cocos2d::MoveBy * Enemy::moveUpOneTile()
{
	cocos2d::MoveBy *movement = cocos2d::MoveBy::create((MOVEMENT_DURATION_BETWEEN_TWO_NODE * 2) / speed, cocos2d::Vec2(0, 64));
	return movement;
}

///////////////////////////////////////////////////////
// return a MoveBy action that a node will perform, 
// that node will move down one tile in tiled map
cocos2d::MoveBy * Enemy::moveDownOneTile()
{
	cocos2d::MoveBy *movement = cocos2d::MoveBy::create((MOVEMENT_DURATION_BETWEEN_TWO_NODE * 2) / speed, cocos2d::Vec2(0, -64));
	return movement;
}

///////////////////////////////////////////////////////
// return a MoveBy action that a node will perform, 
// that node will move left one tile in tiled map
cocos2d::MoveBy * Enemy::moveLeftOneTile()
{
	cocos2d::MoveBy *movement = cocos2d::MoveBy::create((MOVEMENT_DURATION_BETWEEN_TWO_NODE * 2) / speed, cocos2d::Vec2(-64, 0));
	return movement;
}

///////////////////////////////////////////////////////
// return a MoveBy action that a node will perform, 
// that node will move right one tile in tiled map
cocos2d::MoveBy * Enemy::moveRightOneTile()
{
	cocos2d::MoveBy *movement = cocos2d::MoveBy::create((MOVEMENT_DURATION_BETWEEN_TWO_NODE * 2) / speed, cocos2d::Vec2(64, 0));
	return movement;
}

/////////////////////////////////////////////////////
// return a MoveBy action (up, down, left, or right) 
// based on the movement direction of this enemy
cocos2d::MoveBy * Enemy::moveForwardOneTile()
{
	if (direction == VECTOR_UP)
		return moveUpOneTile();

	if (direction == VECTOR_DOWN)
		return moveDownOneTile();

	if (direction == VECTOR_LEFT)
		return moveLeftOneTile();

	if (direction == VECTOR_RIGHT)
		return moveRightOneTile();
}


//////////////////////////////////////////////////////
// Vertical movement enemy
//////////////////////////////////////////////////////

Enemy * VerticalMovementEnemy::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
{
	Enemy *e = new (std::nothrow) VerticalMovementEnemy(gameLayer, pos, startDir, speed);
	if (e && e->initWithFile("images/enemies/type1or2/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	CC_SAFE_DELETE(e);
	return nullptr;
}


VerticalMovementEnemy::VerticalMovementEnemy(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
	: Enemy(gameLayer, pos, startDir, speed)
{
	turnBack();
	isFirstMove = true;

	initEnemyAnimation();
	setRunningAnimation();
}


void VerticalMovementEnemy::initEnemyAnimation()
{
	// init idle animation
	cocos2d::Vector<cocos2d::SpriteFrame *> idleFrames;
	
	int numOfIdleFrames = utils::countNumberOfFileWithFormat("images/enemies/type1or2/obstacle_idle_%d.png");
	for (int i = 1; i <= 10; i++)
	{
		auto idleFileName = cocos2d::String::createWithFormat("images/enemies/type1or2/obstacle_idle_%d.png", 1);
		auto frame = cocos2d::SpriteFrame::create(idleFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		idleFrames.pushBack(frame);
	}
	idleAnimation = cocos2d::Animation::createWithSpriteFrames(idleFrames, (1.0F / 60.0F) * 5);
	idleAnimation->retain();

	// init running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> runningFrames;

	int numOfRungningFrames = utils::countNumberOfFileWithFormat("images/enemies/type1or2/obstacle_running_%d.png");
	for (int i = 1; i <= numOfRungningFrames; i++)
	{
		auto runningFileName = cocos2d::String::createWithFormat("images/enemies/type1or2/obstacle_running_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(runningFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		runningFrames.pushBack(frame);
	}
	runningAnimation = cocos2d::Animation::createWithSpriteFrames(runningFrames, (1.0F / 60.0F) * 7);
	runningAnimation->retain();

	// init eating animation
	cocos2d::Vector<cocos2d::SpriteFrame *> eatingFrames;

	int numOfEatingFrames = utils::countNumberOfFileWithFormat("images/enemies/type1or2/obstacle_eating_%d.png");
	for (int i = 1; i <= numOfEatingFrames; i++)
	{
		auto eatingFileName = cocos2d::String::createWithFormat("images/enemies/type1or2/obstacle_eating_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(eatingFileName->getCString(), cocos2d::Rect(0, 0, 72, (int)GameObjectSize::HEIGHT));
		eatingFrames.pushBack(frame);
	}
	eatingAnimation = cocos2d::Animation::createWithSpriteFrames(eatingFrames, (1.0F / 60.0F) * 4);
	eatingAnimation->retain();
}


void VerticalMovementEnemy::setStartDirection(int startDir)
{
	switch (startDir)
	{
	case (int)EnemyStartDirection::UP:
		direction = VECTOR_UP;
		break;

	case (int)EnemyStartDirection::DOWN:
		direction = VECTOR_DOWN;
		break;

	default:
		setDefaultStartDirection();
		break;
	}
}


void VerticalMovementEnemy::setDefaultStartDirection()
{
	direction = VECTOR_UP;
}


VerticalMovementEnemy::~VerticalMovementEnemy()
{

}


cocos2d::Vector<cocos2d::FiniteTimeAction*> VerticalMovementEnemy::createNextMoveSequence()
{
	cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(VerticalMovementEnemy::setRunningAction)));
	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(VerticalMovementEnemy::calculateNextMoveForwardSteps)));

	for (int i = 0; i < nextMoveSteps; i++) {
		actions.pushBack(moveForwardOneTile());
	}

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(VerticalMovementEnemy::turnBackY)));
	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(VerticalMovementEnemy::stopRunningAction)));

	return actions;
}

/////////////////////////////////////////////////////////
// move up (or down) until the next tile is a wall
void VerticalMovementEnemy::calculateNextMoveForwardSteps()
{
	nextMoveSteps = 0;
	GameScene *layer = dynamic_cast<GameScene *>(this->gameLayer);
	cocos2d::Vec2 currentTile = layer->pointToTileCoordinate(this->getPosition());

	while (layer->getTileDescription(currentTile + direction) != "wall")
	{
		currentTile += direction;
		nextMoveSteps++;
	}
}


//////////////////////////////////////////////////////
// Horizontal movement enemy
//////////////////////////////////////////////////////

Enemy * HorizontalMovementEnemy::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
{
	Enemy *e = new (std::nothrow) HorizontalMovementEnemy(gameLayer, pos, startDir, speed);
	if (e && e->initWithFile("images/enemies/type1or2/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	return nullptr;
}


HorizontalMovementEnemy::HorizontalMovementEnemy(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
	: Enemy(gameLayer, pos, startDir, speed)
{
	turnBackX();
	isFirstMove = true;

	initEnemyAnimation();
	setRunningAnimation();
}


void HorizontalMovementEnemy::initEnemyAnimation()
{
	// init idle animation
	cocos2d::Vector<cocos2d::SpriteFrame *> idleFrames;

	int numOfIdleFrames = utils::countNumberOfFileWithFormat("images/enemies/type1or2/obstacle_idle_%d.png");
	for (int i = 1; i <= 10; i++)
	{
		auto idleFileName = cocos2d::String::createWithFormat("images/enemies/type1or2/obstacle_idle_%d.png", 1);
		auto frame = cocos2d::SpriteFrame::create(idleFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		idleFrames.pushBack(frame);
	}
	idleAnimation = cocos2d::Animation::createWithSpriteFrames(idleFrames, 1.0F / 60.0F);
	idleAnimation->retain();

	// init running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> runningFrames;

	int numOfRungningFrames = utils::countNumberOfFileWithFormat("images/enemies/type1or2/obstacle_running_%d.png");
	for (int i = 1; i <= numOfRungningFrames; i++)
	{
		auto runningFileName = cocos2d::String::createWithFormat("images/enemies/type1or2/obstacle_running_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(runningFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		runningFrames.pushBack(frame);
	}
	runningAnimation = cocos2d::Animation::createWithSpriteFrames(runningFrames, (1.0F / 60.0F) * 7);
	runningAnimation->retain();

	// init eating animation
	cocos2d::Vector<cocos2d::SpriteFrame *> eatingFrames;

	int numOfEatingFrames = utils::countNumberOfFileWithFormat("images/enemies/type1or2/obstacle_eating_%d.png");
	for (int i = 1; i <= numOfEatingFrames; i++)
	{
		auto eatingFileName = cocos2d::String::createWithFormat("images/enemies/type1or2/obstacle_eating_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(eatingFileName->getCString(), cocos2d::Rect(0, 0, 72, (int)GameObjectSize::HEIGHT));
		eatingFrames.pushBack(frame);
	}
	eatingAnimation = cocos2d::Animation::createWithSpriteFrames(eatingFrames, (1.0F / 60.0F) * 4);
	eatingAnimation->retain();
}


void HorizontalMovementEnemy::setStartDirection(int startDir)
{
	switch (startDir)
	{
	case (int)EnemyStartDirection::LEFT:
		direction = VECTOR_LEFT;
		break;
	
	case (int)EnemyStartDirection::RIGHT:
		direction = VECTOR_RIGHT;
		break;
	
	default:
		setDefaultStartDirection();
		break;
	}
}


void HorizontalMovementEnemy::setDefaultStartDirection()
{
	direction = VECTOR_LEFT;
}


HorizontalMovementEnemy::~HorizontalMovementEnemy()
{

}


cocos2d::Vector<cocos2d::FiniteTimeAction*> HorizontalMovementEnemy::createNextMoveSequence()
{
	cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(HorizontalMovementEnemy::setRunningAction)));

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(HorizontalMovementEnemy::turnBackX)));
	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(HorizontalMovementEnemy::calculateNextMoveForwardSteps)));

	for (int i = 0; i < nextMoveSteps; i++) {
		actions.pushBack(moveForwardOneTile());
	}

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(HorizontalMovementEnemy::stopRunningAction)));

	return actions;
}

/////////////////////////////////////////////////////////
// move left (or right) until the next tile is a wall
void HorizontalMovementEnemy::calculateNextMoveForwardSteps()
{
	nextMoveSteps = 0;
	GameScene *layer = dynamic_cast<GameScene *>(this->gameLayer);
	cocos2d::Vec2 currentTile = layer->pointToTileCoordinate(this->getPosition());

	while (layer->getTileDescription(currentTile + direction) != "wall")
	{
		currentTile += direction;
		nextMoveSteps++;
	}
}


//////////////////////////////////////////////////////
// Outside clockwise movement enemy
//////////////////////////////////////////////////////

Enemy * OutsideClockwiseEnemy::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
{
	Enemy *e = new (std::nothrow) OutsideClockwiseEnemy(gameLayer, pos, startDir, speed);
	if (e && e->initWithFile("images/enemies/type3or4/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	return nullptr;
}


OutsideClockwiseEnemy::OutsideClockwiseEnemy(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
	: Enemy(gameLayer, pos, startDir, speed)
{
	turnLeft();
	isFirstMove = true;

	initEnemyAnimation();
	setRunningAnimation();
}


void OutsideClockwiseEnemy::initEnemyAnimation()
{
	// init idle animation
	cocos2d::Vector<cocos2d::SpriteFrame *> idleFrames;

	int numOfIdleFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_idle_%d.png");
	for (int i = 1; i <= 10; i++)
	{
		auto idleFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_idle_%d.png", 1);
		auto frame = cocos2d::SpriteFrame::create(idleFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		idleFrames.pushBack(frame);
	}
	idleAnimation = cocos2d::Animation::createWithSpriteFrames(idleFrames, 1.0F / 60.0F);
	idleAnimation->retain();

	// init running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> runningFrames;

	int numOfRungningFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_running_%d.png");
	for (int i = 1; i <= numOfRungningFrames; i++)
	{
		auto runningFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_running_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(runningFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		runningFrames.pushBack(frame);
	}
	runningAnimation = cocos2d::Animation::createWithSpriteFrames(runningFrames, (1.0F / 60.0F) * 7);
	runningAnimation->retain();

	// init eating animation
	cocos2d::Vector<cocos2d::SpriteFrame *> eatingFrames;

	int numOfEatingFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_eating_%d.png");
	for (int i = 1; i <= numOfEatingFrames; i++)
	{
		auto eatingFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_eating_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(eatingFileName->getCString(), cocos2d::Rect(0, 0, 72, (int)GameObjectSize::HEIGHT));
		eatingFrames.pushBack(frame);
	}
	eatingAnimation = cocos2d::Animation::createWithSpriteFrames(eatingFrames, (1.0F / 60.0F) * 6);
	eatingAnimation->retain();
}


void OutsideClockwiseEnemy::setDefaultStartDirection()
{
	direction = VECTOR_RIGHT;
}


OutsideClockwiseEnemy::~OutsideClockwiseEnemy()
{
	
}


cocos2d::Vector<cocos2d::FiniteTimeAction*> OutsideClockwiseEnemy::createNextMoveSequence()
{
	cocos2d::Vector<cocos2d::FiniteTimeAction *> actions;

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(OutsideClockwiseEnemy::turnRight)));
	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(OutsideClockwiseEnemy::calculateNextMoveForwardSteps)));

	for (int i = 0; i < nextMoveSteps; i++)
		actions.pushBack(moveForwardOneTile());

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(OutsideClockwiseEnemy::stopRunningAction)));

	return actions;
}

//////////////////////////////////////////////////////////
// move forward until reach the next right turn
void OutsideClockwiseEnemy::calculateNextMoveForwardSteps()
{
	nextMoveSteps = 0;

	GameScene *layer = dynamic_cast<GameScene *>(this->gameLayer);
	cocos2d::Vec2 normalVector = cocos2d::Vec2(normalRight(direction).x, -normalRight(direction).y);

	//calculate the current position to tile coordinate for the first move
	//on the next move, use current tile position
	if (isFirstMove) {
		isFirstMove = false;
		currentTile = layer->pointToTileCoordinate(this->getPosition());
	}

	//move forward one tile after turned right
	if (
		layer->getTileDescription(currentTile + normalVector) == "ground" && 
		layer->getTileDescription(currentTile + cocos2d::Vec2(direction.x, -direction.y)) != "wall"
		) 
	{
		currentTile += cocos2d::Vec2(direction.x, -direction.y);
		nextMoveSteps++;
	}

	//then move forward until reach the next right turn
	while (
		layer->getTileDescription(currentTile + normalVector) != "ground" && 
		layer->getTileDescription(currentTile + cocos2d::Vec2(direction.x, -direction.y)) != "wall"
		)
	{
		currentTile += cocos2d::Vec2(direction.x, -direction.y);
		nextMoveSteps++;
	}
}


//////////////////////////////////////////////////////
// Outside anti-clockwise movement enemy
//////////////////////////////////////////////////////

Enemy * OutsideAntiClockwiseEnemy::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
{
	Enemy *e = new (std::nothrow) OutsideAntiClockwiseEnemy(gameLayer, pos, startDir, speed);
	if (e && e->initWithFile("images/enemies/type3or4/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	return nullptr;
}


OutsideAntiClockwiseEnemy::OutsideAntiClockwiseEnemy(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
	: Enemy(gameLayer, pos, startDir, speed)
{
	turnRight();
	isFirstMove = true;

	initEnemyAnimation();
	setRunningAnimation();
}


void OutsideAntiClockwiseEnemy::initEnemyAnimation()
{
	// init idle animation
	cocos2d::Vector<cocos2d::SpriteFrame *> idleFrames;

	int numOfIdleFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_idle_%d.png");
	for (int i = 1; i <= 10; i++)
	{
		auto idleFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_idle_%d.png", 1);
		auto frame = cocos2d::SpriteFrame::create(idleFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		idleFrames.pushBack(frame);
	}
	idleAnimation = cocos2d::Animation::createWithSpriteFrames(idleFrames, 1.0F / 60.0F);
	idleAnimation->retain();

	// init running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> runningFrames;

	int numOfRungningFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_running_%d.png");
	for (int i = 1; i <= numOfRungningFrames; i++)
	{
		auto runningFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_running_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(runningFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		runningFrames.pushBack(frame);
	}
	runningAnimation = cocos2d::Animation::createWithSpriteFrames(runningFrames, (1.0F / 60.0F) * 7);
	runningAnimation->retain();

	// init eating animation
	cocos2d::Vector<cocos2d::SpriteFrame *> eatingFrames;

	int numOfEatingFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_eating_%d.png");
	for (int i = 1; i <= numOfEatingFrames; i++)
	{
		auto eatingFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_eating_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(eatingFileName->getCString(), cocos2d::Rect(0, 0, 72, (int)GameObjectSize::HEIGHT));
		eatingFrames.pushBack(frame);
	}
	eatingAnimation = cocos2d::Animation::createWithSpriteFrames(eatingFrames, (1.0F / 60.0F) * 6);
	eatingAnimation->retain();
}


void OutsideAntiClockwiseEnemy::setDefaultStartDirection()
{
	direction = VECTOR_LEFT;
}


OutsideAntiClockwiseEnemy::~OutsideAntiClockwiseEnemy()
{

}


cocos2d::Vector<cocos2d::FiniteTimeAction*> OutsideAntiClockwiseEnemy::createNextMoveSequence()
{
	cocos2d::Vector<cocos2d::FiniteTimeAction *> actions;

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(OutsideAntiClockwiseEnemy::turnLeft)));
	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(OutsideAntiClockwiseEnemy::calculateNextMoveForwardSteps)));

	for (int i = 0; i < nextMoveSteps; i++)
		actions.pushBack(moveForwardOneTile());

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(OutsideAntiClockwiseEnemy::stopRunningAction)));

	return actions;
}

//////////////////////////////////////////////////////
// move forward until reach the next left turn
void OutsideAntiClockwiseEnemy::calculateNextMoveForwardSteps()
{
	nextMoveSteps = 0;

	GameScene *layer = dynamic_cast<GameScene *>(this->gameLayer);
	cocos2d::Vec2 normalVector = cocos2d::Vec2(normalLeft(direction).x, -normalLeft(direction).y);

	//calculate the current position to tile coordinate for the first move
	//on the next move, use current tile position
	if (isFirstMove) {
		isFirstMove = false;
		currentTile = layer->pointToTileCoordinate(this->getPosition());
	}

	//move forward one tile after turned left
	if (
		layer->getTileDescription(currentTile + normalVector) == "ground" &&
		layer->getTileDescription(currentTile + cocos2d::Vec2(direction.x, -direction.y)) != "wall"
		)
	{
		currentTile += cocos2d::Vec2(direction.x, -direction.y);
		nextMoveSteps++;
	}

	//then move forward until reach the next turn left
	while (
		layer->getTileDescription(currentTile + normalVector) != "ground" &&
		layer->getTileDescription(currentTile + cocos2d::Vec2(direction.x, -direction.y)) != "wall"
		)
	{
		currentTile += cocos2d::Vec2(direction.x, -direction.y);
		nextMoveSteps++;
	}
}


//////////////////////////////////////////////////////
// Inside clockwise movement enemy
//////////////////////////////////////////////////////

Enemy * InsideClockwiseEnemy::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
{
	Enemy *e = new (std::nothrow) InsideClockwiseEnemy(gameLayer, pos, startDir, speed);
	if (e && e->initWithFile("images/enemies/type5or6/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	return nullptr;
}


InsideClockwiseEnemy::InsideClockwiseEnemy(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
	: Enemy(gameLayer, pos, startDir, speed)
{
	turnLeft();
	isFirstMove = true;

	initEnemyAnimation();
	setRunningAnimation();
}


void InsideClockwiseEnemy::initEnemyAnimation()
{
	// init idle animation
	cocos2d::Vector<cocos2d::SpriteFrame *> idleFrames;

	int numOfIdleFrames = utils::countNumberOfFileWithFormat("images/enemies/type5or6/obstacle_idle_%d.png");
	for (int i = 1; i <= 10; i++)
	{
		auto idleFileName = cocos2d::String::createWithFormat("images/enemies/type5or6/obstacle_idle_%d.png", 1);
		auto frame = cocos2d::SpriteFrame::create(idleFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		idleFrames.pushBack(frame);
	}
	idleAnimation = cocos2d::Animation::createWithSpriteFrames(idleFrames, 1.0F / 60.0F);
	idleAnimation->retain();

	// init running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> runningFrames;

	int numOfRungningFrames = utils::countNumberOfFileWithFormat("images/enemies/type5or6/obstacle_running_%d.png");
	for (int i = 1; i <= numOfRungningFrames; i++)
	{
		auto runningFileName = cocos2d::String::createWithFormat("images/enemies/type5or6/obstacle_running_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(runningFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		runningFrames.pushBack(frame);
	}
	runningAnimation = cocos2d::Animation::createWithSpriteFrames(runningFrames, (1.0F / 60.0F) * 7);
	runningAnimation->retain();

	// init eating animation
	cocos2d::Vector<cocos2d::SpriteFrame *> eatingFrames;

	int numOfEatingFrames = utils::countNumberOfFileWithFormat("images/enemies/type5or6/obstacle_eating_%d.png");
	for (int i = 1; i <= numOfEatingFrames; i++)
	{
		auto eatingFileName = cocos2d::String::createWithFormat("images/enemies/type5or6/obstacle_eating_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(eatingFileName->getCString(), cocos2d::Rect(0, 0, 72, (int)GameObjectSize::HEIGHT));
		eatingFrames.pushBack(frame);
	}
	eatingAnimation = cocos2d::Animation::createWithSpriteFrames(eatingFrames, (1.0F / 60.0F) * 6);
	eatingAnimation->retain();
}


void InsideClockwiseEnemy::setDefaultStartDirection()
{
	direction = VECTOR_RIGHT;
}


InsideClockwiseEnemy::~InsideClockwiseEnemy()
{

}


cocos2d::Vector<cocos2d::FiniteTimeAction*> InsideClockwiseEnemy::createNextMoveSequence()
{
	cocos2d::Vector<cocos2d::FiniteTimeAction *> actions;

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(InsideClockwiseEnemy::turnRight)));
	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(InsideClockwiseEnemy::calculateNextMoveForwardSteps)));
	
	for (int i = 0; i < nextMoveSteps; i++)
		actions.pushBack(moveForwardOneTile());

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(InsideClockwiseEnemy::stopRunningAction)));
	
	return actions;
}

/////////////////////////////////////////////////////////
// move forward until the next tile is a wall, then turn right
void InsideClockwiseEnemy::calculateNextMoveForwardSteps()
{
	nextMoveSteps = 0;
	GameScene *layer = dynamic_cast<GameScene *>(this->gameLayer);
	
	//calculate the current position to tile coordinate for the first move
	//on the next move, use current tile position
	if (isFirstMove) {
		isFirstMove = false;
		currentTile = layer->pointToTileCoordinate(this->getPosition());
	}

	//move forward until the next tile is a wall
	while (layer->getTileDescription(currentTile + cocos2d::Vec2(direction.x, -direction.y)) != "wall")
	{
		currentTile += cocos2d::Vec2(direction.x, -direction.y);
		nextMoveSteps++;
	}
}


//////////////////////////////////////////////////////
// Inside anti-clockwise movement enemy
//////////////////////////////////////////////////////

Enemy * InsideAntiClockwiseEnemy::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
{
	Enemy *e = new (std::nothrow) InsideAntiClockwiseEnemy(gameLayer, pos, startDir, speed);
	if (e && e->initWithFile("images/enemies/type5or6/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	return nullptr;
}


InsideAntiClockwiseEnemy::InsideAntiClockwiseEnemy(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, int startDir, float speed)
	: Enemy(gameLayer, pos, startDir, speed)
{
	turnRight();
	isFirstMove = true;

	initEnemyAnimation();
	setRunningAnimation();
}


void InsideAntiClockwiseEnemy::initEnemyAnimation()
{
	// init idle animation
	cocos2d::Vector<cocos2d::SpriteFrame *> idleFrames;

	int numOfIdleFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_idle_%d.png");
	for (int i = 1; i <= 10; i++)
	{
		auto idleFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_idle_%d.png", 1);
		auto frame = cocos2d::SpriteFrame::create(idleFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		idleFrames.pushBack(frame);
	}
	idleAnimation = cocos2d::Animation::createWithSpriteFrames(idleFrames, 1.0F / 60.0F);
	idleAnimation->retain();

	// init running animation
	cocos2d::Vector<cocos2d::SpriteFrame *> runningFrames;

	int numOfRungningFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_running_%d.png");
	for (int i = 1; i <= numOfRungningFrames; i++)
	{
		auto runningFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_running_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(runningFileName->getCString(), cocos2d::Rect(0, 0, (int)GameObjectSize::WIDTH, (int)GameObjectSize::HEIGHT));
		runningFrames.pushBack(frame);
	}
	runningAnimation = cocos2d::Animation::createWithSpriteFrames(runningFrames, (1.0F / 60.0F) * 7);
	runningAnimation->retain();

	// init eating animation
	cocos2d::Vector<cocos2d::SpriteFrame *> eatingFrames;

	int numOfEatingFrames = utils::countNumberOfFileWithFormat("images/enemies/type3or4/obstacle_eating_%d.png");
	for (int i = 1; i <= numOfEatingFrames; i++)
	{
		auto eatingFileName = cocos2d::String::createWithFormat("images/enemies/type3or4/obstacle_eating_%d.png", i);
		auto frame = cocos2d::SpriteFrame::create(eatingFileName->getCString(), cocos2d::Rect(0, 0, 72, (int)GameObjectSize::HEIGHT));
		eatingFrames.pushBack(frame);
	}
	eatingAnimation = cocos2d::Animation::createWithSpriteFrames(eatingFrames, (1.0F / 60.0F) * 6);
	eatingAnimation->retain();
}


void InsideAntiClockwiseEnemy::setDefaultStartDirection()
{
	direction = VECTOR_LEFT;
}


InsideAntiClockwiseEnemy::~InsideAntiClockwiseEnemy()
{

}


cocos2d::Vector<cocos2d::FiniteTimeAction*> InsideAntiClockwiseEnemy::createNextMoveSequence()
{
	cocos2d::Vector<cocos2d::FiniteTimeAction *> actions;

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(InsideAntiClockwiseEnemy::turnLeft)));
	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(InsideAntiClockwiseEnemy::calculateNextMoveForwardSteps)));

	for (int i = 0; i < nextMoveSteps; i++)
		actions.pushBack(moveForwardOneTile());

	actions.pushBack(cocos2d::CallFunc::create(this, callfunc_selector(InsideAntiClockwiseEnemy::stopRunningAction)));

	return actions;
}

/////////////////////////////////////////////////////////
// move forward until the next tile is a wall, then turn left
void InsideAntiClockwiseEnemy::calculateNextMoveForwardSteps()
{
	nextMoveSteps = 0;
	GameScene *layer = dynamic_cast<GameScene *>(this->gameLayer);

	//calculate the current position to tile coordinate for the first move
	//on the next move, use current tile position
	if (isFirstMove) {
		isFirstMove = false;
		currentTile = layer->pointToTileCoordinate(this->getPosition());
	}

	// move forward until the next tile is a wall
	while (layer->getTileDescription(currentTile + cocos2d::Vec2(direction.x, -direction.y)) != "wall")
	{
		currentTile += cocos2d::Vec2(direction.x, -direction.y);
		nextMoveSteps++;
	}
}


//////////////////////////////////////////////////////
// Chaser
//////////////////////////////////////////////////////



//////////////////////////////////////////////////////
// Forwarding Chaser
//////////////////////////////////////////////////////



//////////////////////////////////////////////////////
// Upgraded Chaser
//////////////////////////////////////////////////////



//////////////////////////////////////////////////////
// Enemy factory
//////////////////////////////////////////////////////

EnemyFactory::EnemyFactory()
{

}


EnemyFactory::~EnemyFactory()
{

}

////////////////////////////////////////////////////
// return an enemy based-on the type, add it to game layer at pos position
cocos2d::Node * EnemyFactory::createEnemy(int type, cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, float speed, int startDirection)
{
	cocos2d::Node *e = nullptr;

	///////////////////////////////////////////////
	// create enemy based-on enemy type
	switch (type)
	{
	case (int)EnemyTypes::LEFT_RIGHT:
		e = HorizontalMovementEnemy::create(gameLayer, pos, startDirection, speed);
		break;

	case (int)EnemyTypes::UP_DOWN:
		e = VerticalMovementEnemy::create(gameLayer, pos, startDirection, speed);
		break;

	case (int)EnemyTypes::OUTSIDE_CLOCKWISE:
		e = OutsideClockwiseEnemy::create(gameLayer, pos, startDirection, speed);
		break;

	case (int)EnemyTypes::OUTSIDE_ANTI_CLOCKWISE:
		e = OutsideAntiClockwiseEnemy::create(gameLayer, pos, startDirection, speed);
		break;

	case (int)EnemyTypes::INSIDE_CLOCKWISE:
		e = InsideClockwiseEnemy::create(gameLayer, pos, startDirection, speed);
		break;

	case (int)EnemyTypes::INSIDE_ANTI_CLOCKWISE:
		e = InsideAntiClockwiseEnemy::create(gameLayer, pos, startDirection, speed);
		break;

		// TODO: create a forwarding chaser
	case (int)EnemyTypes::FORWARDING_CHASER:
		e = ForwardingChaser::create(gameLayer, pos, speed);
		break;

		// TODO: create a upgraded chaser
	case (int)EnemyTypes::UPGRADED_CHASER:
		e = UpgradedChaser::create(gameLayer, pos, speed);
		break;

	default:
		break;
	}

	//create enemy physics body
	if (e) {
		e->setPhysicsBody(cocos2d::PhysicsBody::createCircle((int)GameObjectSize::WIDTH / 2));
		e->getPhysicsBody()->setDynamic(false);
		e->getPhysicsBody()->setContactTestBitmask((int)ContactTestBitmast::OBSTACLE);
		e->getPhysicsBody()->setCollisionBitmask((int)CollisionBismask::OBSTACLE);
	}

	return e;
}


Chaser::~Chaser()
{
	CC_SAFE_RELEASE(idleAnimation);
	CC_SAFE_RELEASE(runningAnimation);
	CC_SAFE_RELEASE(eatingAnimation);
}


void Chaser::update(float dt)
{

}

void Chaser::setIdleAnimation()
{
}

void Chaser::setRunningAnimation()
{
}

void Chaser::setEatingAnimation()
{
}


Chaser::Chaser(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, float speed)
{
	this->gameLayer = gameLayer;
	this->gameLayer->addChild(this, (int)ZOrderLayer::LAYER_10);
	this->setPosition(pos);
	this->speed = speed;

	this->actionManager = cocos2d::Director::getInstance()->getActionManager();
	this->scheduleUpdate();
}


Chaser * ForwardingChaser::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, float speed)
{
	Chaser *e = new (std::nothrow) ForwardingChaser(gameLayer, pos, speed);
	if (e && e->initWithFile("images/enemies/type1or2/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	CC_SAFE_DELETE(e);
	return nullptr;
}


ForwardingChaser::~ForwardingChaser()
{

}


void ForwardingChaser::update(float dt)
{

}


ForwardingChaser::ForwardingChaser(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, float speed)
	: Chaser(gameLayer, pos, speed)
{
	initEnemyAnimation();
	setIdleAnimation();
}


void ForwardingChaser::initEnemyAnimation()
{

}


Chaser * UpgradedChaser::create(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, float speed)
{
	Chaser *e = new (std::nothrow) UpgradedChaser(gameLayer, pos, speed);
	if (e && e->initWithFile("images/enemies/type1or2/obstacle_idle_1.png")) {

		e->autorelease();
		return e;
	}

	CC_SAFE_DELETE(e);
	return nullptr;
}


UpgradedChaser::~UpgradedChaser()
{

}


void UpgradedChaser::update(float dt)
{

}


UpgradedChaser::UpgradedChaser(cocos2d::Layer * gameLayer, cocos2d::Vec2 pos, float speed)
	: Chaser(gameLayer, pos, speed)
{
	initEnemyAnimation();
	setIdleAnimation();
}


void UpgradedChaser::initEnemyAnimation()
{

}
