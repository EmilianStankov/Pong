#include "Pong.h"
#include "ConsoleGaming.h"
#include "Vector2D.h"

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
int myScore = 0;
int enemyScore = 0;
// Paddle variables
const int PaddleLength = 5;
int paddleSpeed = 1;
int player2PaddleSpeed = 1;

// Game variables
unsigned long sleepDuration = 100;
GameState gameState;
map<ControlNames, char> controls;

//AI
bool Smart = false;
bool Multiplayer = true;

vector<vector<GameObject>> paddles;
GameObject ball(WindowWidth / 2, WindowHeight / 2, '#');

void Update()
{
	COORD direction = { 0, 0 };
	COORD enemyDirection = { 0, 0 };

	if (kbhit())
	{
		char key = getch();

		//make sure controls work with shift/CAPS LOCK
		if(key >= 'A' && key <= 'Z')
			key -= 'A' - 'a';

		if(key == controls[PaddleUp1])
		{
			direction.Y = -paddleSpeed;
		} else if(key == controls[PaddleDown1]) {
			direction.Y = paddleSpeed;
		} else if(key == controls[PaddleUp2]) {
			enemyDirection.Y = player2PaddleSpeed;
		} else if(key == controls[PaddleDown2]) {
			enemyDirection.Y = player2PaddleSpeed;
		}
	}
	typedef vector<vector<GameObject>>::iterator vector_iterator;
	vector_iterator playerPaddle = paddles.begin();

	for (randomAccess_iterator paddle = playerPaddle->begin(); paddle != playerPaddle->end(); ++paddle)
	{
		if(paddle->Coordinates.Y >= WindowHeight)
		{
			direction.Y = -paddleSpeed;
		}
		else if(paddle->Coordinates.Y <= -1)
		{
			direction.Y = paddleSpeed;
		}
		paddle->Coordinates.X += direction.X;
		paddle->Coordinates.Y += direction.Y;
	}
	//The AI's paddle
	vector_iterator enemyPaddle = paddles.begin() + 1;
	if(Multiplayer == false)
	{
		for (randomAccess_iterator paddle = enemyPaddle->begin(); paddle != enemyPaddle->end() ; ++paddle)
		{
			if(Smart)
			{
				paddle->Coordinates.Y += (SHORT)ballSpeed.y;
			}
			else
			{
				paddle->Coordinates.Y -= (SHORT)ballSpeed.y;
			}
		}

		ball.Coordinates.X += (SHORT)ballSpeed.x;
		if (ball.Coordinates.X >= WindowWidth - 1 || ball.Coordinates.X <= 0)
		{
			ballSpeed.x = -ballSpeed.x;
		}

		ball.Coordinates.Y += (SHORT)ballSpeed.y;
		if (ball.Coordinates.Y >= WindowHeight - 1 || ball.Coordinates.Y <= 0)
		{
			ballSpeed.y = -ballSpeed.y;
		}
	}
	else
	{
		for (randomAccess_iterator paddle = enemyPaddle->begin(); paddle != enemyPaddle->end(); ++paddle)
		{
			if(paddle->Coordinates.Y >= WindowHeight)
			{
				enemyDirection.Y = -player2PaddleSpeed;
			}
			else if(paddle->Coordinates.Y <= -1)
			{
				enemyDirection.Y = player2PaddleSpeed;
			}
			paddle->Coordinates.X += enemyDirection.X;
			paddle->Coordinates.Y += enemyDirection.Y;
		}
		ball.Coordinates.X += (SHORT)ballSpeed.x;
		if (ball.Coordinates.X >= WindowWidth - 1 || ball.Coordinates.X <= 0)
		{
			ballSpeed.x = -ballSpeed.x;
		}

		ball.Coordinates.Y += (SHORT)ballSpeed.y;
		if (ball.Coordinates.Y >= WindowHeight - 1 || ball.Coordinates.Y <= 0)
		{
			ballSpeed.y = -ballSpeed.y;
		}
	}
}

void Draw()
{
	ClearScreen(consoleHandle);

	typedef vector<vector<GameObject>>::iterator vector_iterator;
	for (vector_iterator paddle = paddles.begin(); paddle != paddles.end(); ++paddle)
	{
		for (randomAccess_iterator paddlePart = paddle->begin(); paddlePart != paddle->end(); ++paddlePart)
		{
			paddlePart->Draw(consoleHandle);
			if(((ball.Coordinates.X == paddlePart->Coordinates.X + 1)||(ball.Coordinates.X == paddlePart->Coordinates.X - 1)))
			{
				if(ball.Coordinates.Y == paddlePart->Coordinates.Y)
				{
					ballSpeed.x = -ballSpeed.x;
				}
			}
		}
	}
	/*if(ball.Coordinates.X == 0)
	{
		enemyScore += 1;
		ball.Coordinates.X = WindowWidth / 2;
		ball.Coordinates.Y = WindowHeight / 2;
	}
	else if(ball.Coordinates.X == WindowWidth - 1)
	{
		myScore += 1;
		ball.Coordinates.X = WindowWidth / 2;
		ball.Coordinates.Y = WindowHeight / 2;
	}*/
	ball.Draw(consoleHandle);
}

int main()
{
	InitScreen(WindowWidth*CharWidth, WindowHeight*CharHeight);
	consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	controls[PaddleDown1] = 's';
	controls[PaddleUp1] = 'w';
	controls[PaddleDown2] = 'k';
	controls[PaddleUp2] = 'i';

	srand(time(NULL));

	vector<GameObject> leftPaddle, rightPaddle;
	int paddleStartingPos = WindowHeight / 2 - PaddleLength / 2;
	for (int i = 0; i < PaddleLength; ++i)
	{
		leftPaddle.push_back(GameObject(0, paddleStartingPos + i, '*'));
		rightPaddle.push_back(GameObject(WindowWidth - 1, paddleStartingPos + i, '*'));
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