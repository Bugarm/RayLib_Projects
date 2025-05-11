#include "Game.h"
#include "Common.h"
#include "CustomDebugDraw.h"

Game::Game()
{
	world = std::make_shared<b2World>(b2Vec2(0.0f, -10.0f));//gravity vector (0, -10) means gravity is pointing down with a magnitude of 10
			
	//simple shapes

	//create a circle
	circle = std::make_shared<Circle>(world, Vector2{ 400, 200 }, 25, b2_dynamicBody, BLUE,0.1f,0.1f,0.5f,1.0f,1.0f);

	//create a box acting as a rotated ground
	Vector2 boxCenterPosition = Vector2{ 400, 400 };
	float boxAngle = 2.0;
	box = std::make_shared<Box>(world, boxCenterPosition, 800, 50, b2_staticBody, boxAngle, GRAY, 0.3f, 0.3f, 0, 1.0f, 1.0f);

	//create a box acting as a rotated platform
	box2 = std::make_shared<Box>(world, Vector2{ 400, 300 }, 200, 20, b2_staticBody, 45, GRAY, 0.3f, 0.3f, 0, 1.0f, 1.0f);

	//textured shapes
	LoadResources();

	//create box crate (textured shape)
	Vector2 texturedBoxPosition = { 100, 300 }; // Center position	
	Vector2 texturedBoxSize = { 70, 70 }; // Width and height in pixels
	float texturedBoxRotation = 30; // Rotation angle in degrees	
	texturedBox = std::make_shared<TexturedBox>(world, texturedBoxPosition, texturedBoxSize, texturedBoxRotation, b2_dynamicBody, boxTexture,0.3f,0.3f,0.1f,1.0f,1.0f);

	//create circle smiley (textured shape)
	texturedCircle = std::make_shared<TexturedCircle>(world, Vector2{ 390, 10 }, circleTexture, b2_dynamicBody,0.3f,0.3f,0.9f,0.5f,0.5f);

	// Create an instance of the custom debug drawing class (this is used for visual physics debugging)
	debugDraw = std::make_shared<CustomDebugDraw>();
	debugDraw->DrawSolidOrOutline(CustomDebugDraw::DRAW_OUTLINE); // Set the debug drawing to draw outlines (we can also draw solid shapes)
	debugDraw->SetFlags(b2Draw::e_shapeBit);

	// Set the debug drawing object for the Box2D world
	world->SetDebugDraw(debugDraw.get()); //using .get() to get the raw pointer from the shared pointer (because SetDebugDraw expects a raw pointer)
		
}
void Game::Update()
{
	world->Step(TIME_STEP,VELOCITY_ITERATIONS,POSITION_ITERATIONS); //update the world
}

void Game::Draw()
{
	world->DrawDebugData(); //this is used to draw the debug data (so we can see the physics shapes). you can comment the below lines (in draw method) and see the difference
	
	box->Draw();
	box2->Draw();
	circle->Draw();
	texturedBox->Draw();
	texturedCircle->Draw();

}


Game::~Game()
{
	UnloadResources();
}

void Game::LoadResources()
{
	boxTexture = LoadTexture("images/wood01.png");
	circleTexture = LoadTexture("images/round.png");
}

void Game::UnloadResources()
{
	UnloadTexture(boxTexture);
	UnloadTexture(circleTexture);
}
