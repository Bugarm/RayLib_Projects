#pragma once
#include <raylib.h>
#include <box2d/box2d.h>
#include <memory>
#include "Circle.h"
#include "Box.h"
#include "TexturedBox.h"
#include "TexturedCircle.h"
#include "CustomDebugDraw.h"

class Game
{
private:
	//Physics world
	std::shared_ptr<b2World> world;

	//Game objects
	std::shared_ptr<Box> box;
	std::shared_ptr<Box> box2;
	std::shared_ptr<Circle> circle;
	std::shared_ptr<TexturedBox> texturedBox;
	std::shared_ptr<TexturedCircle> texturedCircle;

	//Textures
	Texture2D boxTexture;
	Texture2D circleTexture;

	//Debug draw
	std::shared_ptr<CustomDebugDraw> debugDraw;

public:
	Game();
	~Game();
	void Update();
	void Draw();
	void LoadResources();
	void UnloadResources();
	
};

