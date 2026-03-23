
#pragma once

#include "UtilVector2.h"

struct BoidData
{
	Vector2 Center{ 0.f, 0.f };
	float Angle = 0.f;
};

class BoidsSystem
{
public:
	static BoidsSystem* GetSystem();
	virtual void InitializeBoids(int NumBoids, int InScreenWidth, int InScreenHeight) = 0;
	virtual void ShowGui() = 0;
	virtual void RenderAdditionalData() = 0;
	virtual void SimulateBoids(float DeltaTime) = 0;
	virtual size_t GetNumBoids() const = 0;
	virtual const BoidData& GetBoidData(size_t Index) const = 0;
};

