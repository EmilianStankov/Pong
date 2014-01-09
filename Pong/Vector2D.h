#ifndef VECTOR2D_GUARD
#define VECTOR2D_GUARD

#include <cmath>

struct Vector2D
{
	float x, y;

	Vector2D()
	{
		x = y = 0.0f;
	}

	Vector2D(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	float Length()
	{
		return std::sqrtf(x*x + y*y);
	}

	Vector2D Normalize()
	{
		return Vector2D(x/Length(), y/Length());
	}
};

Vector2D operator+(const Vector2D &a, const Vector2D &b)
{
	return Vector2D(a.x + b.x, a.y + b.y);
}

Vector2D operator-(const Vector2D &a, const Vector2D &b)
{
	return Vector2D(a.x - b.x, a.y - b.y);
}

Vector2D operator*(const Vector2D &a, const float &b)
{
	return Vector2D(a.x * b, a.y * b);
}

Vector2D operator/(const Vector2D &a, const float &b)
{
	return Vector2D(a.x / b, a.y / b);
}

#endif