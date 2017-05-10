#pragma once

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "cocos2d.h"
#include <vector>
#include <string>
#include <list>

namespace utils
{

	int countNumberOfFileWithFormat(const char *format);

	int countNumberOfFolderWithFormat(const char *format);

	cocos2d::Sprite * loadBaseSpriteWithFormat(const char *format);

	void startNewDebugSession();
	void logToDebugFile(std::string content);

	class AStarChasingAlgorithm
	{

		/*
		How this algorithm work:

		Step 1: Add the Node start into the open list.
		Step 2: Repeat the following:
			- Look for the lowest F cost Node in the open list, refer it as Node current.
			- Drop it to closed list.
			- For each 8 adjacent Node from Node current (child Node):
				+ If it is unwalkable or it is in the closed list, ignore it and continue checking the next adjacent Node. In opposite, do the task below.
				+ If it is not in the open list, add it to open list. Make the parent field of this Node point to current Node, and update the F G H scores to this Node.
				+ If it is already in the open list, check to see if the path through this Node is better, using G score to measure. A lower G cost means that this is the better path.
				Change the parent of this Node to the current Node, update F G H score to this Node.
			(We stop the loop when the Node end has been found, or the open list is empty).
		Step 3: Save the path.
		Step 4: Find the Node end in the path and give the result.
		*/

	public:

		enum ANodeType
		{
			GROUND = 0,
			WALL,
			START,
			END
		};

		struct ANode
		{
			int id;
			int x, y; //Coordinate of Node in the a graph
			int H, G, F; //Heuristic value
			ANode *parent; //Use to trace the path
			bool closed; //True if this Node is now in the close list
			bool opened; //True if this Node is now in the open list
			bool walkable; //True if this Node is not an obstacle

			ANode();
			ANode(int _x, int _y, bool _w = true);

			int getGScore(ANode *n); //calculate the movement cost to move from Node Start to the current Node n on graph
			int getHScore(ANode *n); //estimated the movement cost from the current Node to the Node n
			void computeScores(ANode *e); //compute F value by calculate G score from Node start to Node e, then add to H score from Node e to Node end
		};

		AStarChasingAlgorithm();
		virtual ~AStarChasingAlgorithm();

		// intput
		int num_of_row, num_of_col;
		std::vector< std::vector<ANode *> > graph; //2 dimensional graph
		ANode *start; //start point on the graph
		ANode *end; //end point on the graph

		// output
		std::list<ANode *> path;

		// most important method
		void readInput(int num_row, int num_col, std::vector< std::vector<ANode *> > & graph);
		void pathFinding();

	private:

		ANode* getNode(int x, int y);
		bool isWalkable(int x, int y);
		bool isValidPosition(int x, int y);
	};
}

#endif // !__UTILITY_H__