#include "Pong.h"
#include "ConsoleGaming.h"
#include "Vector2D.h"
#include "Paddle.h"

using namespace std;

HANDLE consoleHandle;

typedef vector<GameObject>::iterator randomAccess_iterator;
typedef vector<GameObject>::const_iterator const_iterator;

Vector2D ballSpeed = Vector2D(1, 1);
Vector2D ballPosition = Vector2D(WindowWidth / 2, WindowHeight / 2);
int playerScore = 0;
int enemyScore = 0;
int hits = 0;
// Paddle variables
int paddleSpeed = 2;
int player2PaddleSpeed = 2;

// Game variables
unsigned long sleepDuration = 70;
GameState gameState;
map<ControlNames, char> controls;
GameState LastDrawnState = About; //must initially set to anything other than menu

//AI
bool Smart = false;
bool Multiplayer = false;
bool EpilepsyMode = false;
bool MultiplayerArrowKeys = true;

vector<Paddle> paddles;
vector<GameObject> obstacles;
vector<pair<char*, int>> Highscores;
GameObject ball(WindowWidth / 2, WindowHeight / 2, '#');

void HandleInput(COORD &player1Direction, COORD &player2Direction)
{
	if (_kbhit())
	{
		char key = _getch();

		//cout << (int)key << " ";

		//make sure controls work with shift/CAPS LOCK
		if(key >= 'A' && key <= 'Z')
			key -= 'A' - 'a';

		switch (gameState)
		{
		case Menu:
			if(key == controls[MenuSingleplayer])
			{
				Multiplayer = false;
				gameState = Playing;
			} else if(key == controls[MenuMultiplayer]) {
				Multiplayer = true;
				gameState = Playing;
			} else if(key == controls[Resume]) {
				gameState = Playing;
			} else if(key == controls[MenuSettings]) {
				gameState = Settings;
			} else if(key == controls[MenuInstructions]) {
				gameState = Instructions;
			} else if(key == controls[MenuHighscore]) {
				gameState = Highscore;
			} else if(key == controls[MenuAbout]) {
				gameState = About;
			} else if(key == controls[MenuExit]) {
				exit(0);
			}
			break;
		case Playing:
		case Paused:
			if(key == controls[PaddleUp1])
			{
				player1Direction.Y = -paddleSpeed;
			} else if(key == controls[PaddleDown1]) {
				player1Direction.Y = paddleSpeed;
			} else if(key == controls[PaddleUp2]) {
				player2Direction.Y = -player2PaddleSpeed;
			} else if(key == controls[PaddleDown2]) {
				player2Direction.Y = player2PaddleSpeed;
			} else if(key == controls[Pause]) {
				if(gameState == Playing)
					gameState = Paused;
				else if(gameState == Paused)
					gameState = Playing;
			} else if(key == controls[BackToMenu]) {
				gameState = Menu;
			} else if((key == 0 || key == 224) && MultiplayerArrowKeys) {//0x00 and 0xE0, first half of arrow keys code
				key = _getch();
				if(key == 72)//up
				{
					player2Direction.Y = -player2PaddleSpeed;
				} else if(key == 80) {//down
					player2Direction.Y = player2PaddleSpeed;
				}
			}
			break;
		case Settings:
			if(key == controls[SettingsSmart])
			{
				Smart = true;
			} else if(key == controls[SettingsStupid]) {
				Smart = false;
			} else if(key == controls[SettingsToMainMenu]) {
				gameState = Menu;
			} else if(key == controls[SettingsSinglePlayer]) {
				Multiplayer = false;
			} else if(key == controls[SettingsMultiplayer]) {
				Multiplayer = true;
			} else if(key == controls[SettingsStart]) {
				gameState = Playing;
			} else if(key == controls[SettingsEpilepsy]) {
				EpilepsyMode = !EpilepsyMode;
			}
			break;
		case Instructions:
			if(key == controls[SettingsToMainMenu])
			{
				gameState = Menu;
			}
			break;
		case About:
			if(key == controls[SettingsToMainMenu])
			{
				gameState = Menu;
			}
			break;
		case Highscore:
			if(key == controls[HighscoreToMainMenu])
			{
				gameState = Menu;
			}
		}
	}
}

void HandleAI(COORD &enemyDirection, int paddleIndex)
{
	if(Smart)
	{
		enemyDirection.Y = ballPosition.y > paddles[paddleIndex].position.Y ? paddleSpeed : -paddleSpeed;
	} else {
		enemyDirection.Y = rand()%100 > 50 ? paddleSpeed : -paddleSpeed;
	}
}

void HandleCollision()
{
	typedef vector<Paddle>::iterator vector_iterator;

	for (vector_iterator paddle = paddles.begin(); paddle != paddles.end(); ++paddle)
	{
		if(((SHORT)(ballPosition.x + ballSpeed.x) == paddle->position.X))
		{
			for (randomAccess_iterator paddlePart = paddle->elements.begin(); paddlePart != paddle->elements.end(); ++paddlePart)
			{
				if((SHORT)ballPosition.y == paddlePart->Coordinates.Y + paddle->position.Y)
				{
					float oldLen = ballSpeed.Length();
					oldLen += BallSpeedIncrease;

					ballSpeed.x = -ballSpeed.x;
					ballSpeed.y += paddlePart->Coordinates.Y *DeflectionAmount;
					ballSpeed = ballSpeed.Normalize()*oldLen;

					hits++;
				}
			}
		}
	}

	for(int i = 0;i < obstacles.size();i++)
	{
		if((SHORT)(ballPosition.x + ballSpeed.x) == obstacles[i].Coordinates.X && (SHORT)(ballPosition.y + ballSpeed.y) == obstacles[i].Coordinates.Y)
		{
			ballSpeed = ballSpeed * -1;
		}
	}
}

void UpdateHighscore()
{
	ifstream file(HighscoreFileName);

	while (!file.eof())
	{
		pair<char*, int> entry;
		char *name = new char[50];

		file >> name >> entry.second;
		entry.first = name;

		if(entry.second == 0)
		{
			delete [] name;
			break;
		}

		Highscores.push_back(entry);
	}

	file.close();

	if(hits > Highscores[Highscores.size()-1].second)
	{
		ofstream file(HighscoreFileName, ios::trunc);

		for(int i = 0;i < Highscores.size();i++)
		{
			if(Highscores[i].second < hits)
			{
				char *name = new char[50];
				cout << "Highscore! Enter yout name: ";
				cin >> name;

				Highscores.insert(Highscores.begin()+i, pair<char*, int>(name, hits));
				Highscores.erase(Highscores.begin()+Highscores.size()-1);
			}

			file << Highscores[i].first << " " << Highscores[i].second << endl;
		}

		file.close();
	}
}

void Update()
{
	vector<COORD> directions;
	directions.resize(paddles.size());

	HandleInput(directions[0], directions[1]);

	if(gameState == Playing)
	{
		if(!Multiplayer)
			for(int i = 1;i < paddles.size();i++)
				HandleAI(directions[i], i);

		HandleCollision();
	
		typedef vector<vector<GameObject>>::iterator vector_iterator;

		//Handle any and all paddles
		for(int i = 0;i < paddles.size();i++)
		{
			for (randomAccess_iterator paddleElement = paddles[i].elements.begin(); paddleElement != paddles[i].elements.end(); ++paddleElement)//check if paddle can move
			{
				if((paddleElement->Coordinates.Y + paddles[i].position.Y >= WindowHeight && directions[i].Y > 0) || (paddleElement->Coordinates.Y + paddles[i].position.Y <= 1) && directions[i].Y < 0)
				{
					directions[i].Y = 0;
				}
			}

			paddles[i].position.X += directions[i].X;
			paddles[i].position.Y += directions[i].Y;
		}

		//Ball movement
		ballPosition.x += ballSpeed.x;
		if (ballPosition.x >= WindowWidth - 1 || ballPosition.x <= 0)
		{
			ballSpeed.x = -ballSpeed.x;
		}

		ballPosition.y += ballSpeed.y;
		if (ballPosition.y >= WindowHeight || ballPosition.y <= 2)
		{
			ballSpeed.y = -ballSpeed.y;
		}
	}
	if(EpilepsyMode)
	{
		int randomizer = 0;
		randomizer = rand() % 10 + 1;
		switch (randomizer)
		{
		case 1:
			system("Color 01");
		case 2:
			system("Color A2");
		case 3:
			system("Color B3");
		case 4:
			system("Color 24");
		case 5:
			system("Color 75");
		case 6:
			system("Color E6");
		case 7:
			system("Color 07");
		case 8:
			system("Color 0A");
		case 9:
			system("Color 0B");
		case 10:
			system("Color 0C");
		default:
			system("Color 0F");
		}
	}
	if(ball.Coordinates.X <= 0)
	{
		UpdateHighscore();
		hits = 0;
		enemyScore += 1;
		ballPosition.x = WindowWidth / 2;
		ballPosition.y = WindowHeight / 2;
		ballSpeed = Vector2D(1, 1);
	}
	else if(ball.Coordinates.X >= WindowWidth - 1)
	{
		UpdateHighscore();
		hits = 0;
		playerScore += 1;
		ballPosition.x = WindowWidth / 2;
		ballPosition.y = WindowHeight / 2;
		ballSpeed = Vector2D(-1, 1);
	}
}

void DrawPaddles()
{
	typedef vector<Paddle>::iterator vector_iterator;
	for (vector_iterator paddle = paddles.begin(); paddle != paddles.end(); ++paddle)
	{
		for (randomAccess_iterator paddlePart = paddle->elements.begin(); paddlePart != paddle->elements.end(); ++paddlePart)
		{
			paddlePart->Coordinates.Y += paddle->position.Y;
			paddlePart->Draw(consoleHandle);
			paddlePart->Coordinates.Y -= paddle->position.Y;
		}
	}
}

void DrawMenu()
{
	cout << MenuString << endl;
}

void DrawInstructions()
{
	cout << InstructionsString << endl;
}

void DrawAbout()
{
	cout << AboutString << endl;
}

void DrawHighscore()
{
	for(int i = 0;i < Highscores.size();i++)
		cout << Highscores[i].first << " " << Highscores[i].second << endl;
	cout << "[Q]uit to Main Menu" << endl;
}

void DrawScore()
{
	cout << Boundaries << endl;
	cout << "Score: " << playerScore << "|" << enemyScore << endl;
	cout << "Hits: " << hits << endl;
}

void DrawSettings()
{
	cout << SettingsString << endl;
}

void Draw()
{
	if(LastDrawnState != gameState || gameState == Playing)
	{
		ClearScreen(consoleHandle);

		switch (gameState)
		{
		case Menu:
			DrawMenu();
			LastDrawnState = Menu;
			break;
		case Settings:
			DrawSettings();
			LastDrawnState = Settings;
			break;
		case Paused:
		case Playing:
			DrawScore();
			DrawPaddles();

			ball.Coordinates.X = (SHORT)ballPosition.x;
			ball.Coordinates.Y = (SHORT)ballPosition.y;
			ball.Draw(consoleHandle);
			for(int i = 0;i < obstacles.size();i++)
			{
				obstacles[i].Draw(consoleHandle);
			}
			LastDrawnState = Playing;
			break;
		case Instructions:
				DrawInstructions();
				LastDrawnState = Instructions;
			break;
		case About:
				DrawAbout();
				LastDrawnState = About;
			break;
		case Highscore:
				DrawHighscore();
				LastDrawnState = Highscore;
			break;
		default:
			break;
		}
	}
}

void SetupControls()
{
	controls[PaddleDown1] = 's';
	controls[PaddleUp1] = 'w';
	controls[PaddleDown2] = 'k';
	controls[PaddleUp2] = 'i';
	controls[Pause] = 'p';
	controls[BackToMenu] = 'q';
	controls[Resume] = 'r';

	controls[MenuSingleplayer] = 's';
	controls[MenuMultiplayer] = 'm';
	controls[MenuSettings] = 't';
	controls[MenuHighscore] = 'h';
	controls[MenuAbout] = 'a';
	controls[MenuExit] = 'e';
	controls[MenuInstructions] = 'i';

	controls[SettingsSmart] = 's';
	controls[SettingsStupid] = 't';
	controls[SettingsToMainMenu] = 'q';
	controls[SettingsSinglePlayer] = 'p';
	controls[SettingsMultiplayer] = 'm';
	controls[SettingsStart] = 'n';
	controls[SettingsEpilepsy] = 'e';

	controls[HighscoreToMainMenu] = 'q';
}

void SetupObstacles()
{
	obstacles.push_back(GameObject(20, 10, '@'));
	obstacles.push_back(GameObject(20, 11, '@'));
	obstacles.push_back(GameObject(20, 12, '@'));
	obstacles.push_back(GameObject(20, 13, '@'));
}

int main()
{
	PlaySound(TEXT("pong_OST.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);

	InitScreen(WindowWidth*CharWidth, WindowHeight*CharHeight);
	consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	SetupControls();
	SetupObstacles();
	UpdateHighscore();

	gameState = Menu;

	srand((unsigned int)time(NULL));

	Paddle leftPaddle, rightPaddle;
	int paddleStartingPos = WindowHeight / 2;

	leftPaddle.position.X = 0;
	leftPaddle.position.Y = paddleStartingPos;

	rightPaddle.position.X = WindowWidth - 1;
	rightPaddle.position.Y = paddleStartingPos;

	for (int i = -PaddleLength/2; i <= PaddleLength/2; ++i)
	{
		leftPaddle.elements.push_back(GameObject(leftPaddle.position.X, i, '$'));
		rightPaddle.elements.push_back(GameObject(rightPaddle.position.X, i, '*'));
	}
	paddles.push_back(leftPaddle);
	paddles.push_back(rightPaddle);

	ball.Color = ConsoleColors::Yellow;

	while (true)
	{
		Update();
		Draw();
		Sleep(sleepDuration);
	}

	return 0;
}