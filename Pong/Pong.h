#ifndef PONG_GUARD
#define PONG_GUARD

#include <iostream>
#include <vector>
#include <conio.h>
#include <ctime>
#include <map>

#include "ConsoleGaming.h"
#include "Vector2D.h"

enum GameState
{
	Menu, Playing, Paused
};

enum ControlNames
{
	PaddleUp1, PaddleDown1, PaddleUp2, PaddleDown2, Pause
};

#endif