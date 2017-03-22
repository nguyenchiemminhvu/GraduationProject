#pragma once

#ifndef __STORY_SCENE_H__
#define __STORY_SCENE_H__

#include "cocos2d.h"

class StoryScene : public cocos2d::Layer
{
public:

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(StoryScene);

private:

	cocos2d::Vec2 origin;
	cocos2d::Size visibleSize;

};

#endif // !__STORY_SCENE_H__