#include "Utility.h"
#include "cocos2d.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

const char *DEBUG_FILE = "DEBUG_LOG.txt";
unsigned int log_line_number = 1;

int utils::countNumberOfFileWithFormat(const char *format)
{
	if (!strstr(format, "%d"))
	{
		cocos2d::log("Wrong format: %s", format);
		return 0;
	}

	int counted = 0;
	while (true)
	{
		counted++;
		auto filePath = cocos2d::String::createWithFormat(format, counted);
		if (!cocos2d::FileUtils::getInstance()->isFileExist(filePath->getCString())) 
		{
			break;
		}
	}

	return counted - 1;
}


int utils::countNumberOfFolderWithFormat(const char * format)
{
	if (!strstr(format, "%d"))
	{
		return 0;
	}

	int counted = 0;
	while (true)
	{
		counted++;
		auto folderPath = cocos2d::String::createWithFormat(format, counted);
		if (!cocos2d::FileUtils::getInstance()->isDirectoryExist(folderPath->getCString()))
		{
			break;
		}
	}

	return counted - 1;
}


cocos2d::Sprite * utils::loadBaseSpriteWithFormat(const char * format)
{
	cocos2d::Sprite *sprite = nullptr;
	
	if (!strstr(format, "%d"))
	{
		return nullptr;
	}

	auto baseFile = cocos2d::String::createWithFormat(format, 1);
	sprite = cocos2d::Sprite::create(baseFile->getCString());

	if (!sprite)
	{
		cocos2d::log("sprite can not load file: %s", baseFile->getCString());
	}
	
	return sprite;
}


void utils::startNewDebugSession()
{
	if (cocos2d::FileUtils::getInstance()->isFileExist(DEBUG_FILE))
	{
		cocos2d::FileUtils::getInstance()->removeFile(DEBUG_FILE);
	}

	log_line_number = 1;
}


void utils::logToDebugFile(std::string content)
{
	FILE *file = nullptr;
	file = fopen(DEBUG_FILE, "a");
	if (!file)
		return;
	
	fprintf(file, "%d: %s\n", log_line_number++, content.c_str());

	fclose(file);
}
