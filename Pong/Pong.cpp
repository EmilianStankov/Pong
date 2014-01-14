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
int playerScore = 0;
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
bool Smart = true;
bool Multiplayer = false;

vector<vector<GameObject>> paddles;
GameObject ball(WindowWidth / 2, WindowHeight / 2, '#');

void HandleInput(COORD &player1Direction, COORD &player2Direction)
{
	if (_kbhit())
	{
		char key = _getch();

		//make sure controls work with shift/CAPS LOCK
		if(key >= 'A' && key <= 'Z')
			key -= 'A' - 'a';

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
		}
	}
}

void HandleAI(COORD &enemyDirection, int paddleIndex)
{
	if(Smart)
	{
		enemyDirection.Y = ball.Coordinates.Y > paddles[paddleIndex][0].Coordinates.Y + PaddleLength/2 ? paddleSpeed : -paddleSpeed;
	} else {
		enemyDirection.Y = rand()%100 > 50 ? paddleSpeed : -paddleSpeed;
	}
}

void HandleCollision()
{
	typedef vector<vector<GameObject>>::iterator vector_iterator;

	for (vector_iterator paddle = paddles.begin(); paddle != paddles.end(); ++paddle)
	{
		for (randomAccess_iterator paddlePart = paddle->begin(); paddlePart != paddle->end(); ++paddlePart)
		{
			if(((ball.Coordinates.X == paddlePart->Coordinates.X + 1)||(ball.Coordinates.X == paddlePart->Coordinates.X - 1)))
			{
				if(ball.Coordinates.Y == paddlePart->Coordinates.Y)
				{
					ballSpeed.x = -ballSpeed.x;
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
			for (randomAccess_iterator paddle = paddles[i].begin(); paddle != paddles[i].end(); ++paddle)//check if paddle can move
			{
				if(paddle->Coordinates.Y >= WindowHeight || paddle->Coordinates.Y <= 0)
				{
					directions[i].Y = 0;
				}
			}

			for (randomAccess_iterator paddle = paddles[i].begin(); paddle != paddles[i].end(); ++paddle)//actually move it
			{
				paddle->Coordinates.X += directions[i].X;
				paddle->Coordinates.Y += directions[i].Y;
			}
		}

		//Ball movement
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

void DrawPaddles()
{
	typedef vector<vector<GameObject>>::iterator vector_iterator;
	for (vector_iterator paddle = paddles.begin(); paddle != paddles.end(); ++paddle)
	{
		for (randomAccess_iterator paddlePart = paddle->begin(); paddlePart != paddle->end(); ++paddlePart)
		{
			paddlePart->Draw(consoleHandle);
		}
	}
}

void DrawMenu()
{

}

void Draw()
{
	ClearScreen(consoleHandle);

	switch (gameState)
	{
	case Menu:
		break;
	case Paused:
	case Playing:
		DrawPaddles();
		ball.Draw(consoleHandle);
		break;
	default:
		break;
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
}

int main()
{
	InitScreen(WindowWidth*CharWidth, WindowHeight*CharHeight);
	consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	controls[PaddleDown1] = 's';
	controls[PaddleUp1] = 'w';
	controls[PaddleDown2] = 'k';
	controls[PaddleUp2] = 'i';
	controls[Pause] = 'p';

	gameState = Playing;

	srand((unsigned int)time(NULL));

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