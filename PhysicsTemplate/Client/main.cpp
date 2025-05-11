#include <raylib.h>
#include <Game.h>
#include <memory>


int main() {
    // Initialize the Raylib window
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Raylib and Box2D Template");

    // Set target framerate (matching Box2D update rate)
    SetTargetFPS(60);

    // Create the game    
    std::shared_ptr<Game> game = std::make_shared<Game>();
 
    // Game loop
    while (!WindowShouldClose()) {
    
    	// Update the game
        game->Update();


        // Start drawing with Raylib
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the game
        game->Draw();
      
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
