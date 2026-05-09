#pragma once

#include <cmath>
#include <limits>
#include <math.h>
#include "raylib.h"

inline Vector2 operator+(const Vector2& Operator1, const Vector2& Operator2)
{
	return { Operator1.x + Operator2.x, Operator1.y + Operator2.y };
}

inline Vector2 operator-(const Vector2& Operator1, const Vector2& Operator2)
{
	return { Operator1.x - Operator2.x, Operator1.y - Operator2.y };
}

inline Vector2 operator-(const Vector2& Operator)
{
	return { -Operator.x, -Operator.y };
}

inline Vector2 operator*(const Vector2& Operator, float Multiplier)
{
	return { Operator.x * Multiplier, Operator.y * Multiplier };
}

inline Vector2 operator*=(Vector2& Operator, float Multiplier)
{
	Operator.x *= Multiplier;
	Operator.y *= Multiplier;
	return Operator;
}

inline Vector2 operator+=(Vector2& Operator1, const Vector2& Operator2)
{
	Operator1 = Operator1 + Operator2;
	return Operator1;
}

inline float ModuleSq(const Vector2& Vector)
{
	return Vector.x * Vector.x + Vector.y * Vector.y;
}

inline float Module(const Vector2& Vector)
{
	return sqrtf(Vector.x * Vector.x + Vector.y * Vector.y);
}

inline Vector2 ClampSize(const Vector2& Vector, float MaxSize)
{
	const float CurSize = Module(Vector);
	return CurSize > MaxSize ? Vector * (MaxSize / CurSize) : Vector;
}

inline Vector2 ClampSize(const Vector2& Vector, float MinSize, float MaxSize)
{
	const float CurSize = Module(Vector);
	return CurSize < MinSize ? Vector * (MinSize / CurSize) : (CurSize > MaxSize ? Vector * (MaxSize / CurSize) : Vector);
}

inline float DotProd(const Vector2& Vector1, const Vector2& Vector2)
{
	return Vector1.x * Vector2.x + Vector1.y * Vector2.y;
}

inline float CrossProd(const Vector2& Vector1, const Vector2& Vector2)
{
	return Vector1.x * Vector2.y - Vector1.y * Vector2.x;
}

inline float DistanceSq(const Vector2& Vector1, const Vector2& Vector2)
{
	return ModuleSq(Vector1 - Vector2);
}

inline float Distance(const Vector2& Vector1, const Vector2& Vector2)
{
	return Module(Vector1 - Vector2);
}

inline Vector2 Normalize(const Vector2& Vector)
{
	const float Length = Module(Vector);
	if (abs(Length) < std::numeric_limits<float>::epsilon())
	{
		return { 1.f, 0.f };
	}

	return Vector * (1.f / Length);
}

inline float Angle (const Vector2& Vector1, const Vector2& Vector2)
{
	return std::atan2(CrossProd(Vector1, Vector2), DotProd(Vector1, Vector2));
}
