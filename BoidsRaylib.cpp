// BoidsRaylib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "raylib/src/raylib.h"
#include "rlImGui/rlImGui.h"
#include "imgui/imgui.h"
#include "Boids.h"
#include "optick.h"
#include <iostream>

constexpr float BoidLength = 20.f;
constexpr float BoidWidth = 15.f;

void DrawBoid(const Vector2& Center, float AngleRotation, Color BoidColor)
{
	float SinAngle = std::sinf(AngleRotation);
	float CosAngle = std::cosf(AngleRotation);
	Vector2 Direction = { CosAngle, SinAngle };
	Vector2 PerpDir = { -SinAngle, CosAngle };

	Vector2 PointA = Center + Direction * BoidLength * 0.75f;
	Vector2 PointB = Center + Direction * BoidLength * -0.25f + PerpDir * BoidWidth * -0.5f;
	Vector2 PointC = Center + Direction * BoidLength * -0.25f + PerpDir * BoidWidth * 0.5f;

	DrawTriangle(PointA, PointB, PointC, BoidColor);
}

int main()
{
	// Initialization
	 //--------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(0, 0, "Boids");
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight() - 100;
	SetWindowPosition(0, 40);
	SetWindowSize(screenWidth, screenHeight);
	SetTargetFPS(144);
	rlImGuiSetup(true);
	bool bBoidsSettingWindowOpen = true;

	//--------------------------------------------------------------------------------------

	// Initialize boids
	BoidsSystem* BoidSystem = BoidsSystem::GetSystem();
	constexpr int NumBoids = 2500;
	BoidSystem->InitializeBoids(NumBoids, screenWidth, screenHeight);

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		OPTICK_FRAME("MainThread");

		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		// GUI
		{
			OPTICK_EVENT("GUI");

			// start ImGui content
			rlImGuiBegin();

			// show ImGui Content
			//ImGui::ShowDemoWindow(&open);
			if (ImGui::Begin("Boids Settings", &bBoidsSettingWindowOpen))
			{
				BoidSystem->ShowGui();
			}
			ImGui::End();

			// end ImGui Content
			rlImGuiEnd();
		}

		// Simulate boids
		float DeltaTime = GetFrameTime();
		BoidSystem->SimulateBoids(DeltaTime);

		// Render
		{
			OPTICK_EVENT("Render Additional Data");

			BoidSystem->RenderAdditionalData();
		}

		{
			OPTICK_EVENT("Render Boids");

			size_t NumBoids = BoidSystem->GetNumBoids();
			for (size_t Index = 0; Index < NumBoids; ++Index)
			{
				const BoidData& Boid = BoidSystem->GetBoidData(Index);
				DrawBoid(Boid.Center, Boid.Angle, { 0, 128, 0, 255 });
			}
		}

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
