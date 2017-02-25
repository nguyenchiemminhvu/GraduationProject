#ifndef __ENEMY_H__
#define __ENEMY_H__

#pragma once

#include "cocos2d.h"

#define VECTOR_ZERO		cocos2d::Vec2(0.0F, 0.0F)
#define VECTOR_UP		cocos2d::Vec2(0.0F, 1.0F)
#define VECTOR_DOWN		cocos2d::Vec2(0.0F, -1.0F)
#define VECTOR_LEFT		cocos2d::Vec2(-1.0F, 0.0F)
#define VECTOR_RIGHT	cocos2d::Vec2(1.0F, 0.0F)

#define IDLE_ANIMATION_TAG		0x000000FF
#define RUNNING_ANIMATION_TAG	0x0000FF00
#define EATING_ANIMATION_TAG	0x00FF0000

class Enemy : public cocos2d::Sprite {
public:

	virtual ~Enemy();

	virtual void update(float dt);
	bool readyToMove();

	void setIdleAnimation();
	void setRunningAnimation();
	void setEatingAnimation();

	void setRunningAction();
	void stopRunningAction();

	///////////////////////////////////////////
	// compute normal vector with direction
	cocos2d::Vec2 normalLeft(cocos2d::Vec2 v);
	cocos2d::Vec2 normalRight(cocos2d::Vec2 v);
	
	////////////////////////////////////////////
	// change direction
	void turnLeft();
	void turnRight();
	void turnBackX();
	void turnBackY();
	void turnBack();

	////////////////////////////////////////////
	// flipping
	void flipLeft();
	void flipRight();

	////////////////////////////////////////////
	// enemy action set
	cocos2d::MoveBy* moveUpOneTile();
	cocos2d::MoveBy* moveDownOneTile();
	cocos2d::MoveBy* moveLeftOneTile();
	cocos2d::MoveBy* moveRightOneTile();
	cocos2d::MoveBy* moveForwardOneTile();

	virtual void calculateNextMoveForwardSteps() = 0;

protected:

	cocos2d::ActionManager *actionManager;
	cocos2d::Layer *gameLayer;

	cocos2d::Animation *idleAnimation;
	cocos2d::Animation *runningAnimation;
	cocos2d::Animation *eatingAnimation;

	cocos2d::Vec2 previousPos;
	cocos2d::Vec2 direction;
	float speed;
	cocos2d::Vector<cocos2d::FiniteTimeAction *> nextMove;
	int nextMoveSteps;
	bool isRunningAction;

	Enemy(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	
	virtual void setStartDirection(int startDir);
	virtual void move();
	virtual void setupNextMove();

	virtual void initEnemyAnimation() = 0;
	virtual void setDefaultStartDirection() = 0;
	virtual cocos2d::Vector<cocos2d::FiniteTimeAction *> createNextMoveSequence() = 0;

private:

	Enemy(const Enemy &other) = delete;
	Enemy& operator = (const Enemy &other) = delete;
};

#ifndef __VERTICAL_MOVEMENT_ENEMY__
#define __VERTICAL_MOVEMENT_ENEMY__

class VerticalMovementEnemy : public Enemy {
public:

	static Enemy* create(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual ~VerticalMovementEnemy();

protected:

	VerticalMovementEnemy(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual void initEnemyAnimation();
	virtual void setStartDirection(int startDir);
	virtual void setDefaultStartDirection();
	virtual cocos2d::Vector<cocos2d::FiniteTimeAction *> createNextMoveSequence();
	virtual void calculateNextMoveForwardSteps();

private:

	cocos2d::Vec2 currentTile;
	bool isFirstMove;

};

#endif // !__VERTICAL_MOVEMENT_ENEMY__

#ifndef __HORIZONTAL_MOVEMENT_ENEMY__
#define __HORIZONTAL_MOVEMENT_ENEMY__

class HorizontalMovementEnemy : public Enemy {
public:

	static Enemy* create(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual ~HorizontalMovementEnemy();

protected:

	HorizontalMovementEnemy(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual void initEnemyAnimation(); 
	virtual void setStartDirection(int startDir);
	virtual void setDefaultStartDirection();
	virtual cocos2d::Vector<cocos2d::FiniteTimeAction *> createNextMoveSequence();
	virtual void calculateNextMoveForwardSteps();

private:

	cocos2d::Vec2 currentTile;
	bool isFirstMove;

};

#endif // !__HORIZONTAL_MOVEMENT_ENEMY__

#ifndef __OUTSIDE_CLOCKWISE_ENEMY__
#define __OUTSIDE_CLOCKWISE_ENEMY__

class OutsideClockwiseEnemy : public Enemy {
public:

	static Enemy* create(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual ~OutsideClockwiseEnemy();

protected:

	OutsideClockwiseEnemy(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual void initEnemyAnimation(); 
	virtual void setDefaultStartDirection();
	virtual cocos2d::Vector<cocos2d::FiniteTimeAction *> createNextMoveSequence();
	virtual void calculateNextMoveForwardSteps();

private:

	cocos2d::Vec2 currentTile;
	bool isFirstMove;

};

#endif // !__OUTSIDE_CLOCKWISE_ENEMY__

#ifndef __OUTSIDE_ANTI_CLOCKWISE_ENEMY__
#define __OUTSIDE_ANTI_CLOCKWISE_ENEMY__

class OutsideAntiClockwiseEnemy : public Enemy {
public:

	static Enemy* create(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual ~OutsideAntiClockwiseEnemy();

protected:

	OutsideAntiClockwiseEnemy(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual void initEnemyAnimation(); 
	virtual void setDefaultStartDirection();
	virtual cocos2d::Vector<cocos2d::FiniteTimeAction *> createNextMoveSequence();
	virtual void calculateNextMoveForwardSteps();

private:

	cocos2d::Vec2 currentTile;
	bool isFirstMove;

};

#endif // !__OUTSIDE_ANTI_CLOCKWISE_ENEMY__

#ifndef __INSIDE_CLOCKWISE_ENEMY__
#define __INSIDE_CLOCKWISE_ENEMY__

class InsideClockwiseEnemy : public Enemy {
public:

	static Enemy* create(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual ~InsideClockwiseEnemy();

protected:

	InsideClockwiseEnemy(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual void initEnemyAnimation(); 
	virtual void setDefaultStartDirection();
	virtual cocos2d::Vector<cocos2d::FiniteTimeAction *> createNextMoveSequence();
	virtual void calculateNextMoveForwardSteps();

private:

	cocos2d::Vec2 currentTile;
	bool isFirstMove;

};

#endif // !__INSIDE_CLOCKWISE_ENEMY__

#ifndef __INSIDE_ANTI_CLOCKWISE_ENEMY__
#define __INSIDE_ANTI_CLOCKWISE_ENEMY__

class InsideAntiClockwiseEnemy : public Enemy {
public:

	static Enemy* create(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual ~InsideAntiClockwiseEnemy();

protected:

	InsideAntiClockwiseEnemy(cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDir, float speed);
	virtual void initEnemyAnimation(); 
	virtual void setDefaultStartDirection();
	virtual cocos2d::Vector<cocos2d::FiniteTimeAction *> createNextMoveSequence();
	virtual void calculateNextMoveForwardSteps();

private:

	cocos2d::Vec2 currentTile;
	bool isFirstMove;

};

#endif // !__INSIDE_ANTI_CLOCKWISE_ENEMY__

#ifndef __ENEMY_FACTORY__
#define __ENEMY_FACTORY__

class EnemyFactory {
public:

	EnemyFactory();
	virtual ~EnemyFactory();
	Enemy* createEnemy(int type, cocos2d::Layer *gameLayer, cocos2d::Vec2 pos, int startDirection, float speed);

private:

};

#endif // !__ENEMY_FACTORY__

#endif // !__ENEMY_H__


