#include "Utility.h"
#include "cocos2d.h"
#include "Definition.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>

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
	FILE *file = nullptr;
	file = fopen(DEBUG_FILE, "w");
	fclose(file);

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



utils::AStarChasingAlgorithm::ANode::ANode()
{
	x = y = G = H = F = 0;
	opened = false;
	closed = false;
	parent = NULL;
}


utils::AStarChasingAlgorithm::ANode::ANode(int _x, int _y, bool _w)
	: x(_x), y(_y), walkable(_w)
{
	G = H = F = 0;
	opened = false;
	closed = false;
	parent = NULL;
}


utils::AStarChasingAlgorithm::AStarChasingAlgorithm()
{

}


utils::AStarChasingAlgorithm::~AStarChasingAlgorithm()
{
	while (!path.empty())
	{
		SAFE_DELETE(path.front());
		path.erase(path.begin());
	}
}


void utils::AStarChasingAlgorithm::readInput(int num_row, int num_col, std::vector<std::vector<ANode*>>& graph)
{
	this->num_of_col = num_col;
	this->num_of_row = num_row;

	this->graph.clear();
	this->graph.resize(num_row);
	for (int i = 0; i < num_row; i++)
	{
		this->graph[i].resize(num_col);
		for (int j = 0; j < num_col; j++)
		{
			this->graph[i][j] = graph[i][j];
		}
	}
}


void utils::AStarChasingAlgorithm::pathFinding()
{
	//There are 8 directions to move around from one Node on the graph
	int x_move[] = { -1, 0, 1, 0, -1, 1, 1, -1 };
	int y_move[] = { 0, -1, 0, 1, -1, -1, 1, 1 };

	const int MAX_MOVE = sizeof(x_move) / sizeof(int);

	std::list<ANode *> openList; //A list of Node that we need to check out
	std::list<ANode *> closedList; //A list of Node that we don't need to check again
	std::list<ANode *>::iterator iter; //Use to loop through the openList

	path.clear();

	ANode *current = start; //Begin at Node start
	ANode *child; //The next move from current Node

	unsigned int n = 0; //Use to check the number of check time, n must be less than (num_of_row * num_of_col)

	//add the start point to openList
	openList.push_back(start);
	start->opened = true;

	while (n == 0 || (current != end && n < num_of_row * num_of_col)) {

		//If there is no more Node in open list, we get out this loop and solve the path
		if (openList.empty())
			break;

		//Look for the smallest F value in openList
		current = (*openList.begin());
		for (iter = openList.begin(); iter != openList.end(); iter++) {

			//Choose the Node which has the smallest F value in open list
			if (iter != openList.begin() && (*iter)->F <= current->F)
				current = (*iter);
		}

		//Stop if we reach to the end
		if (current == end)
			break;

		//Remove the current Node from the openList
		openList.remove(current);
		current->opened = false;

		//Add the current Node to the closedList
		closedList.push_back(current);
		current->closed = true;

		//Get all the current's adjacent walkable Node
		for (int i = 0; i < MAX_MOVE; i++) {

			//It next position is invalid
			if (!isValidPosition(current->x + x_move[i], current->y + y_move[i]))
				continue;

			//Get this point
			child = getNode(current->x + x_move[i], current->y + y_move[i]);

			//If it is closed or not walkable, forget it
			if (child->closed || !child->walkable) {
				continue;
			}

			//If it is already in the openList
			if (child->opened) {

				//If the path getting through this node is better, update the heuristic
				if (child->G > child->getGScore(current)) {

					child->parent = current;
					child->computeScores(end); //update F value to this Node
				}
			}
			else {

				//Add it to openList with current node as parent
				openList.push_back(child);
				child->opened = true;
				child->parent = current;
				child->computeScores(end); //update F value to this Node
			}
		}
		n++; //increase check time
	}

	//Solve the path, we go back from the last Node that we checked to the Node start
	while (current->parent && current != start) {
		path.push_front(current);
		current = current->parent;
	}
	path.push_front(current);
}


int utils::AStarChasingAlgorithm::ANode::getGScore(utils::AStarChasingAlgorithm::ANode * n)
{
	//Plus 10 to each horizontal or vertical Node moved
	//and plus 14 for a diagonal moved from this Node to Node n
	return n->G + ((x == n->x || y == n->y) ? 10 : 14);
}

int utils::AStarChasingAlgorithm::ANode::getHScore(utils::AStarChasingAlgorithm::ANode * n)
{
	//Manhattan method, calculate total number of Node moved horizontally and vertically to reach the Node end from current Node
	//Ignore diagonal movement, and ignore any obstacle that may be in the way
	return (abs(n->x - x) + abs(n->y - y)) * 10;
}


void utils::AStarChasingAlgorithm::ANode::computeScores(utils::AStarChasingAlgorithm::ANode * e)
{
	G = getGScore(e);
	H = getHScore(e);
	F = G + H;
}


utils::AStarChasingAlgorithm::ANode * utils::AStarChasingAlgorithm::getNode(int x, int y)
{
	return graph[x][y];
}


bool utils::AStarChasingAlgorithm::isWalkable(int x, int y)
{
	return graph[x][y]->walkable;
}


bool utils::AStarChasingAlgorithm::isValidPosition(int x, int y)
{
	if (x < 0 || x >= graph.size())
		return false;
	if (y < 0 || y >= graph[x].size())
		return false;

	return true;
}
