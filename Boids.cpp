
#include "Boids.h"
#include "optick.h"
#include "time.h"
#include <algorithm>
#include <vector>
#include "imgui/imgui.h"


struct Boid : public BoidData
{
	Vector2 Force{ 0.f, 0.f };
	Vector2 Velocity{ 0.f, 0.f };
};

struct ObstacleData
{
	Vector2 Center{ 0.f, 0.f };
	float Radius = 0.f;
};

class BoidsSystemImplementation : public BoidsSystem
{
public:
	static BoidsSystem* GetSystem() { return &System; }

protected:
	virtual void InitializeBoids(int NumBoids, int InScreenWidth, int InScreenHeight)  override;
	virtual void ShowGui() override;
	virtual void RenderAdditionalData() override;
	virtual void SimulateBoids(float DeltaTime)  override;
	virtual size_t GetNumBoids() const override { return Boids.size(); }
	virtual const BoidData& GetBoidData(size_t Index) const override { return Boids[Index]; }

private:
	void SimulateBoid(Boid& Boid, float DeltaTime);

	void FollowDriver(Boid& Boid);
	void Separation(Boid& Boid);
	void Alignment(Boid& Boid);
	void Cohesion(Boid& Boid);

	static BoidsSystemImplementation System;

	int ScreenWidth = 0;
	int ScreenHeight = 0;

	Vector2 Driver;
	float DriverCircleRadiusX = 0.f;
	float DriverCircleRadiusY = 0.f;
	float DriverTime = 0.f;
	float ObstacleTime = 0.f;

	std::vector<Boid> Boids;
	std::vector<ObstacleData> Obstacles;

	// Settings
	float DriverSpeed = 0.75f;
	// How speed of boids are dragged. The higher the value, the more drag is applied
	float DragBoidSpeed = 0.2f;
	// Boid's speed is clamped to MaxBoidSpeed
	float MaxBoidSpeed = 500.f;
	// Radius of neighborhood
	float NeighborhoodRadius = 50.f;

	// Follow driver behaviour
	float FollowDriverForceMultiplier = 1.5f;
	float MinFollowDriverForce = 500.f;
	float MaxFollowDriverForce = 750.f;
	float FollowDriverWeight = 1.f;
	// Separation behaviour
	float SeparationWeight = 1.f;
	// Cohesion behaviour
	float CohesionWeight = 1.f;
	// Alignment behaviour
	float AlignmentWeight = 1.f;

};

BoidsSystemImplementation BoidsSystemImplementation::System;

BoidsSystem* BoidsSystem::GetSystem()
{
	return BoidsSystemImplementation::GetSystem();
}

void BoidsSystemImplementation::InitializeBoids(int NumBoids, int InScreenWidth, int InScreenHeight)
{
	float ScreenMultiplierX = InScreenWidth / 1280.f;
	float ScreenMultiplierY = InScreenHeight / 720.f;
	ScreenWidth = InScreenWidth;
	ScreenHeight = InScreenHeight;

	DriverCircleRadiusX = InScreenWidth * 0.4f;
	DriverCircleRadiusY = InScreenHeight * 0.4f;
	DriverTime = 0.f;

	Boids = std::vector<Boid>(NumBoids);
	srand((unsigned int)time(nullptr));
	for (size_t Index = 0; Index < NumBoids; ++Index)
	{
		float x = ScreenWidth * (float)rand() / RAND_MAX;
		float y = ScreenHeight * (float)rand() / RAND_MAX;
		Boids[Index].Center = { x, y };
	}

	Obstacles.push_back({ {0, ScreenHeight * 0.5f}, 40 });
	Obstacles.push_back({ {350 * ScreenMultiplierX, 620 * ScreenMultiplierY}, 40 });
	Obstacles.push_back({ {790 * ScreenMultiplierX, 220 * ScreenMultiplierY}, 35 });
	Obstacles.push_back({ {140 * ScreenMultiplierX, 300 * ScreenMultiplierY}, 25 });
	Obstacles.push_back({ {1150 * ScreenMultiplierX, 400 * ScreenMultiplierY}, 50 });
}

void BoidsSystemImplementation::ShowGui()
{
	if (ImGui::CollapsingHeader("General Boids Settings"))
	{
		if (ImGui::InputFloat("Drag Speed", &DragBoidSpeed, 0.05f, 0.f, "%.2f"))
		{
			DragBoidSpeed = std::clamp(DragBoidSpeed, 0.f, 1.f);
		}
		if (ImGui::InputFloat("Max Speed", &MaxBoidSpeed, 10.f, 30.f, "%.0f"))
		{
			MaxBoidSpeed = std::clamp(MaxBoidSpeed, 0.f, 1000.f);
		}
		if (ImGui::InputFloat("Neighborhood Radius", &NeighborhoodRadius, 1.f, 10.f, "%.0f"))
		{
			NeighborhoodRadius = std::clamp(NeighborhoodRadius, 0.f, 1000.f);
		}
	}
	if (ImGui::CollapsingHeader("Driver Settings"))
	{
		if (ImGui::InputFloat("Speed", &DriverSpeed, 0.05f, 0.f, "%.2f"))
		{
			DriverSpeed = std::clamp(DriverSpeed, 0.1f, 3.f);
		}
	}
	if (ImGui::CollapsingHeader("Follow Driver Behaviour"))
	{
		if (ImGui::InputFloat("Force Multiplier", &FollowDriverForceMultiplier, 0.05f, 0.f, "%.2f"))
		{
			FollowDriverForceMultiplier = std::clamp(FollowDriverForceMultiplier, 0.1f, 5.f);
		}
		if (ImGui::InputFloat("Min Force", &MinFollowDriverForce, 10.f, 30.f, "%.0f"))
		{
			MinFollowDriverForce = std::clamp(MinFollowDriverForce, 0.f, MaxFollowDriverForce);
		}
		if (ImGui::InputFloat("Max Force", &MaxFollowDriverForce, 10.f, 30.f, "%.0f"))
		{
			MaxFollowDriverForce = std::clamp(MaxFollowDriverForce, MinFollowDriverForce, 2000.f);
		}
		if (ImGui::InputFloat("Behaviour Weight", &FollowDriverWeight, 0.1f, 0.f, "%.1f"))
		{
			FollowDriverWeight = std::clamp(FollowDriverWeight, 0.f, 5.f);
		}
	}
	if (ImGui::CollapsingHeader("Separation Behaviour"))
	{
		if (ImGui::InputFloat("Behaviour Weight", &SeparationWeight, 0.1f, 0.f, "%.1f"))
		{
			SeparationWeight = std::clamp(SeparationWeight, 0.f, 5.f);
		}
	}
	if (ImGui::CollapsingHeader("Cohesion Behaviour"))
	{
		if (ImGui::InputFloat("Behaviour Weight", &CohesionWeight, 0.1f, 0.f, "%.1f"))
		{
			CohesionWeight = std::clamp(CohesionWeight, 0.f, 5.f);
		}
	}
	if (ImGui::CollapsingHeader("Alignment Behaviour"))
	{
		if (ImGui::InputFloat("Behaviour Weight", &AlignmentWeight, 0.1f, 0.f, "%.1f"))
		{
			AlignmentWeight = std::clamp(AlignmentWeight, 0.f, 5.f);
		}
	}
}

void BoidsSystemImplementation::RenderAdditionalData()
{
	for (const ObstacleData& Obstacle : Obstacles)
	{
		DrawCircle((int)Obstacle.Center.x, (int)Obstacle.Center.y, Obstacle.Radius, { 128, 0, 0, 255 });
	}

	DrawCircle((int)Driver.x, (int)Driver.y, 5, {0, 0, 255, 255});
}

void BoidsSystemImplementation::SimulateBoids(float DeltaTime)
{
	OPTICK_EVENT();

	// Move Driver
	DriverTime += (DeltaTime * DriverSpeed);
	const float DriverSinTime = sinf(DriverTime * 2.f);
	const float DriverCosTime = cosf(DriverTime);
	Driver.x = ScreenWidth * 0.5f + DriverCircleRadiusX * DriverCosTime + DriverCircleRadiusX * DriverSinTime;
	Driver.y = ScreenHeight * 0.5f - DriverCircleRadiusY * DriverSinTime + DriverCircleRadiusY * DriverCosTime;
	Driver.x = ScreenWidth * 0.5f + DriverCircleRadiusX * DriverCosTime;
	Driver.y = ScreenHeight * 0.5f - DriverCircleRadiusY * DriverSinTime;

	// Move obstacle
	ObstacleData& MovingObstacle = Obstacles[0];
	ObstacleTime += DeltaTime;
	const float ObstacleSinTime = sinf(ObstacleTime * 0.7f) * 0.8f;
	const float ObstacleCosTime = cosf(ObstacleTime * 0.3f) * 0.8f;
	MovingObstacle.Center.x = ScreenWidth * 0.5f * (1.f + ObstacleCosTime);
	MovingObstacle.Center.y = ScreenHeight * 0.5f * (1.f + ObstacleSinTime);


	// Boids
	for (size_t Index = 0; Index < Boids.size(); ++Index)
	{
		SimulateBoid(Boids[Index], DeltaTime);
	}
}

void BoidsSystemImplementation::SimulateBoid(Boid& Boid, float DeltaTime)
{
	OPTICK_EVENT();

	// Drag
	Boid.Velocity *= (1.f - DragBoidSpeed * 0.1f);

	// Compute new forces
	FollowDriver(Boid);
	Separation(Boid);
	Alignment(Boid);
	Cohesion(Boid);
	// AvoidObstacles(Boid);
	// AvoidBorders(Boid)
	//...

	// Add force to velocity
	Boid.Velocity = ClampSize(Boid.Velocity + Boid.Force * DeltaTime, MaxBoidSpeed);
	// Compute new position of Boid
	const Vector2 DeltaLoc = Boid.Velocity * DeltaTime;
	Boid.Center.x = std::clamp(Boid.Center.x + DeltaLoc.x, 0.f, (float)ScreenWidth);
	Boid.Center.y = std::clamp(Boid.Center.y + DeltaLoc.y, 0.f, (float)ScreenHeight);
	// Compute direction of Boid using its velocity vector
	Boid.Angle = atan2f(Boid.Velocity.y, Boid.Velocity.x);
	// Reset force for next frame
	Boid.Force = {0, 0};
}

void BoidsSystemImplementation::FollowDriver(Boid& Boid)
{
	// Add force to follow the driver
	const Vector2 ForceToDriver = (Driver - Boid.Center) * FollowDriverForceMultiplier;
	Boid.Force += ClampSize(ForceToDriver, MinFollowDriverForce, MaxFollowDriverForce) * FollowDriverWeight;
}

void BoidsSystemImplementation::Separation(Boid& Boid)
{
	// TODO
}

void BoidsSystemImplementation::Alignment(Boid& Boid)
{
	// TODO
}

void BoidsSystemImplementation::Cohesion(Boid& Boid)
{
	// TODO
}
