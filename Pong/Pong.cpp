#include "Pong.h"
#include "ConsoleGaming.h"
#include "Vector2D.h"
#include "Paddle.h"

using namespace std;

HANDLE consoleHandle;

typedef vector<GameObject>::iterator randomAccess_iterator;
typedef vector<GameObject>::const_iterator const_iterator;

// Window constants
const int WindowWidth = 70;
const int WindowHeight = 30;
const int CharWidth = 9;
const int CharHeight = 15;


Vector2D ballSpeed = Vector2D(1, 1);
Vector2D ballPosition = Vector2D(WindowWidth / 2, WindowHeight / 2);
int playerScore = 0;
int enemyScore = 0;
// Paddle variables
const int PaddleLength = 5;
int paddleSpeed = 2;
int player2PaddleSpeed = 2;

// Game variables
unsigned long sleepDuration = 70;
GameState gameState;
map<ControlNames, char> controls;
const float DeflectionAmount = 0.3f;
const float BallSpeedIncrease = 0.2f;

//AI
bool Smart = false;
bool Multiplayer = false;
bool EpilepsyMode = false;
bool MultiplayerArrowKeys = true;

vector<Paddle> paddles;
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
			} else if(key == controls[MenuSettings]) {
				gameState = Settings;
			} else if(key == controls[MenuHighscore]) {
				//TODO
			} else if(key == controls[MenuAbout]) {
				//TODO
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
		if((((SHORT)(ballPosition.x + ballSpeed.x) == paddle->position.X)||((SHORT)(ballPosition.x + ballSpeed.x) == paddle->position.X)))
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
				}
			}
		}
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
		if (ballPosition.y >= WindowHeight || ballPosition.y <= 1)
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
		enemyScore += 1;
		ballPosition.x = WindowWidth / 2;
		ballPosition.y = WindowHeight / 2;
		ballSpeed = Vector2D(1, 1);
	}
	else if(ball.Coordinates.X >= WindowWidth - 1)
	{
		playerScore += 1;
		ballPosition.x = WindowWidth / 2;
		ballPosition.y = WindowHeight / 2;
		ballSpeed = Vector2D(1, 1);
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

void DrawScore()
{
	cout << Boundaries << endl;
	cout << "Score: " << playerScore << "|" << enemyScore << endl;
}

void DrawSettings()
{
	cout << SettingsString << endl;
}

void Draw()
{
	ball.Color = ConsoleColors::Yellow;

	ClearScreen(consoleHandle);

	switch (gameState)
	{
	case Menu:
		DrawMenu();
		break;
	case Settings:
		DrawSettings();
		break;
	case Paused:
	case Playing:
		DrawScore();
		DrawPaddles();

		ball.Coordinates.X = (SHORT)ballPosition.x;
		ball.Coordinates.Y = (SHORT)ballPosition.y;
		ball.Draw(consoleHandle);
		break;
	default:
		break;
	}
}

void SetupControls()
{
	controls[PaddleDown1] = 's';
	controls[PaddleUp1] = 'w';
	controls[PaddleDown2] = 'k';
	controls[PaddleUp2] = 'i';
	controls[Pause] = 'p';

	controls[MenuSingleplayer] = 's';
	controls[MenuMultiplayer] = 'm';
	controls[MenuSettings] = 't';
	controls[MenuHighscore] = 'h';
	controls[MenuAbout] = 'a';
	controls[MenuExit] = 'e';

	controls[SettingsSmart] = 's';
	controls[SettingsStupid] = 't';
	controls[SettingsToMainMenu] = 'q';
	controls[SettingsSinglePlayer] = 'p';
	controls[SettingsMultiplayer] = 'm';
	controls[SettingsStart] = 'n';
	controls[SettingsEpilepsy] = 'e';
};

int main()
{
	PlaySound(TEXT("pong_OST.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);

	InitScreen(WindowWidth*CharWidth, WindowHeight*CharHeight);
	consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	SetupControls();

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

	while (true)
	{
		Update();
		Draw();
		Sleep(sleepDuration);
	}

	return 0;
}