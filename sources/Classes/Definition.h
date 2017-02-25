#pragma once

#define BUTTON_SIZE_SCALE_Y 1.1F

#define MOVEMENT_DURATION_BETWEEN_TWO_NODE 0.25F
#define OPEN_NEXT_LEVEL_DURATION 1.0F

#define IS_A(obj, Type)		(dynamic_cast<Type *>(obj) != nullptr)

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
