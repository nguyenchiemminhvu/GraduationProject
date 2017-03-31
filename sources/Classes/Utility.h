#pragma once

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "cocos2d.h"
#include <vector>
#include <string>

namespace utils
{

	int countNumberOfFileWithFormat(const char *format);

	int countNumberOfFolderWithFormat(const char *format);

	cocos2d::Sprite * loadBaseSpriteWithFormat(const char *format);

	void startNewDebugSession();
	void logToDebugFile(std::string content);
}

#endif // !__UTILITY_H__