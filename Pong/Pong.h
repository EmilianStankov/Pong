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
	Menu, Playing, Paused, Settings, Instructions, About, Highscore
};

enum ControlNames
{
	PaddleUp1, PaddleDown1, PaddleUp2, PaddleDown2, Pause, BackToMenu,
	MenuSingleplayer, MenuMultiplayer, MenuSettings, MenuHighscore, MenuAbout, MenuExit, MenuInstructions, Resume,
	SettingsSmart, SettingsStupid, SettingsToMainMenu, SettingsSinglePlayer, SettingsMultiplayer, SettingsStart, SettingsEpilepsy,
	HighscoreToMainMenu
};

const int PaddleLength = 5;

const float DeflectionAmount = 0.3f;
const float BallSpeedIncrease = 0.05f;
const char* HighscoreFileName = "highscore.dat";

// Window constants
const int WindowWidth = 70;
const int WindowHeight = 30;
const int CharWidth = 9;
const int CharHeight = 17;

#endif