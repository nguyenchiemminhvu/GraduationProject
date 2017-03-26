#pragma once

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "cocos2d.h"
#include <vector>

namespace utils
{

	int countNumberOfFileWithFormat(const char *format);

	int countNumberOfFolderWithFormat(const char *format);

	cocos2d::Sprite * loadBaseSpriteWithFormat(const char *format);

}

#endif // !__UTILITY_H__