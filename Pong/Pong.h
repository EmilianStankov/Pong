#ifndef PONG_GUARD
#define PONG_GUARD

#include <iostream>
#include <vector>
#include <conio.h>
#include <ctime>
#include <map>
#include <fstream>
#include <Windows.h>

#include "ConsoleGaming.h"
#include "Vector2D.h"
#include "menu.h"

enum GameState
{
	Menu, Playing, Paused, Settings
};

enum ControlNames
{
	PaddleUp1, PaddleDown1, PaddleUp2, PaddleDown2, Pause,
	MenuSingleplayer, MenuMultiplayer, MenuSettings, MenuHighscore, MenuAbout, MenuExit,
	SettingsSmart, SettingsStupid, SettingsToMainMenu, SettingsSinglePlayer, SettingsMultiplayer, SettingsStart, SettingsEpilepsy
};

#endif