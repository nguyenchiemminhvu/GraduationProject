#ifndef __MAIN_CHARACTER_H__
#define __MAIN_CHARACTER_H__

#pragma once

#include "cocos2d.h"

#include <vector>

#define IDLE_ANIMATION_TAG		0x000000FF
#define RUNNING_ANIMATION_TAG	0x0000FF00


class ImmobilizedEnemy;

class MainCharacter : public cocos2d::Sprite {
public:

	static MainCharacter* createMainCharacter(cocos2d::Layer *gameLayer, cocos2d::Vec2 initPos);

	MainCharacter(cocos2d::Layer *gameLayer, cocos2d::Vec2 initPos);
	virtual ~MainCharacter();

	void setIdleAnimation();
	void setRunningAnimation();

	void setPath(std::vector<cocos2d::Vec2> &path);
	void resetPath();
	void move();

	bool ready();
	bool isStarted();
	bool isArrived();

	void beDestroyed();

	void receiveImmobilizedQueue(std::vector<ImmobilizedEnemy *> &enemies);

private:

	std::vector<cocos2d::Vec2> path;
	cocos2d::Vector<cocos2d::FiniteTimeAction *> moveSequence;

	bool started;
	bool arrived;

	cocos2d::Animation *idleAnimation;
	cocos2d::Animation *runningAnimation;

	std::vector<ImmobilizedEnemy *> immobilizedQueue;

	bool initMainCharacterAnimation();

	void flipLeft();
	void flipRight();

	cocos2d::Vector<cocos2d::FiniteTimeAction *> createMoveSequence();
	void onArrived();

	void closeEntranceDoor();
	void readyToPlay();
	void sendTileLocationChangedSignal();
	void removePhysicsBody();
	void openNextLevel();
	void backToLevelSelectionBoard();
	void replaceFinishedScene();
};

#endif // !__MAIN_CHARACTER_H__