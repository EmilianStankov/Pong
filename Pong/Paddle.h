#ifndef PADDLE_GUARD
#define PADDLE_GUARD

#include <vector>

struct Paddle{
	std::vector<GameObject> elements;

	COORD position;
};

#endif