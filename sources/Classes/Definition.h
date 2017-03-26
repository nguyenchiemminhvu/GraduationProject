#pragma once

#define BUTTON_SIZE_SCALE_Y 1.1F

#define MOVEMENT_DURATION_BETWEEN_TWO_NODE 0.25F
#define OPEN_NEXT_LEVEL_DURATION 1.0F

#define MAIN_CHARACTER_BASE_SPEED 2.0F

#define IS_A(obj, Type)		(dynamic_cast<Type *>(obj) != nullptr)

#define SAFE_DELETE(p)				do { delete (p); (p) = NULL; } while(0);
#define SAFE_DELETE_ARRAY(p)		do { delete p[]; (p) = NULL; } while(0);


enum class PathDirection
{
	UP = 1,
	DOWN,
	LEFT,
	RIGHT
};

enum class GameObjectSize
{
	WIDTH = 48,
	HEIGHT = 48
};

enum class ContactTestBitmast {

	MAIN_CHARACTER = 0b1,
	OBSTACLE = 0b101
};

enum class CollisionBismask {

	MAIN_CHARACTER,
	OBSTACLE
};

enum class EnemyTypes {

	LEFT_RIGHT = 1,
	UP_DOWN,
	OUTSIDE_CLOCKWISE,
	OUTSIDE_ANTI_CLOCKWISE,
	INSIDE_CLOCKWISE,
	INSIDE_ANTI_CLOCKWISE
};

enum class EnemyStartDirection {

	UP = 1,
	DOWN,
	LEFT,
	RIGHT
};

enum class ZOrderLayer	{
	LAYER_1,
	LAYER_2,
	LAYER_3,
	LAYER_4,
	LAYER_5,
	LAYER_6,
	LAYER_7,
	LAYER_8,
	LAYER_9,
	LAYER_10,
};
