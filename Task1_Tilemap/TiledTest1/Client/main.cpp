#include <iostream>
#include "Includes.h"
#include "tileson.hpp"
#include "memory"
#include <vector>
#include <string>
#include <raymath.h>
#include <random>

using namespace std;

//
int playerHP = 10;
int playerScore = 0;

int rounds = 1;

int maxEnemies = 10;
int enemiesKilled = 0;
int enemyKilledMax = 5;

//player Anim
enum AnimationState { IDLE, ATTACK, DAMAGED, ON_BOW, DEAD};

enum EnemyAnimationState { IDLE, ATTACK, DAMAGED, DEAD};

void LoadAnimations()
{

}

void UnloadAnimations()
{

}

//forward declerations
class Collidable;
class CollidableRectangle;
class CollidablePolygon;
class Enemy;
class Arrow;
class TrapArrow;
class HP_PowerUp;

//base class for collidable objects
class Collidable {
public:
    std::string tag;

    Collidable(const std::string& tag) : tag(tag) {}

    virtual bool CheckCollision(const Rectangle& playerRect)  = 0; // Pure virtual function for checking collision with a rectangle
    virtual ~Collidable() = default;
};

//class for collidable rectangles
class CollidableRectangle : public Collidable {
private:
    Rectangle rectangle;

public:
    CollidableRectangle(const Rectangle& rect, const std::string& tag = "")
        : Collidable(tag), rectangle(rect) {}

    bool CheckCollision(const Rectangle& playerRect) override {
        return CheckCollisionRecs(this->rectangle, playerRect);
    }
   
};

//class for collidable polygons
class CollidablePolygon : public Collidable {
private:
    std::vector<Vector2> polygonPoints;

    //There's a bug in CheckCollisionPointPoly RayLibV5.0. CustomCheckCollisionPointPoly() is the fixed code which is published here: https://github.com/raysan5/raylib/commit/ef92ced370815a47d6d417ce6394b3156dd4f528
    bool CustomCheckCollisionPointPoly(Vector2 point, Vector2* points, int pointCount)
    {
        bool inside = false;
        if (pointCount > 2)
        {
            for (int i = 0, j = pointCount - 1; i < pointCount; j = i++)
            {
                if ((points[i].y > point.y) != (points[j].y > point.y) &&
                    (point.x < (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
                {
                    inside = !inside;
                }
            }
        }
        return inside;
    }

    // Function to check collision between a rectangle and a polygon
    bool CheckCollisionRectPoly(Rectangle rect, std::vector<Vector2>& polygonPoints) {
        // Define the corners of the rectangle
        Vector2 corners[4] = {
            {rect.x, rect.y}, // Top-left
            {rect.x + rect.width, rect.y}, // Top-right
            {rect.x, rect.y + rect.height}, // Bottom-left
            {rect.x + rect.width, rect.y + rect.height} // Bottom-right
        };

        // Check if any corner is inside the polygon
        for (const auto& corner : corners) {
            if (CustomCheckCollisionPointPoly(corner, polygonPoints.data(), polygonPoints.size())) {
                return true; // Collision detected
            }
        }

        // Check if any polygon vertex is within the rectangle
        for (const auto& point : polygonPoints) {
            if (CheckCollisionPointRec(point, rect)) {
                return true; // Collision detected
            }
        }

        return false; // No collision detected
    }

public:
    CollidablePolygon(const std::vector<Vector2>& points, const std::string& tag = "")
        : Collidable(tag), polygonPoints(points) {}

    bool CheckCollision(const Rectangle& playerRect)  override {    
        return CheckCollisionRectPoly(playerRect, polygonPoints);
    }
};

std::vector<HP_PowerUp> hpGroup;
std::vector<Enemy> enemyGroup;
std::vector<Arrow> arrowGroup;
std::vector<TrapArrow> trapArrowGroup;
std::vector<std::unique_ptr<Collidable>> collidables;


class HP_PowerUp : Collidable
{
private:
    Rectangle rectangle;
    Vector2 position;
public:
    bool collected = false;

    HP_PowerUp(Vector2 pos) : Collidable(tag)
    {
        this->position = pos;

        rectangle = { position.x,position.y,16,16 };

    }

    bool CheckCollision(const Rectangle& playerRect) override {
        return CheckCollisionRecs(this->rectangle, playerRect);
    }

    void Update(Rectangle& playerRect)
    {
        if (CheckCollision(playerRect))
        {
            if(playerHP < 10)
            {
                playerHP++;
            }
            collected = true;
        }
    }

    void Draw()
    {
        DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, YELLOW);
    }
};


class Enemy : Collidable{
private:
    Rectangle rectangle;
    Vector2 potentialMovement;

    vector<Vector2> spawnPos = {
        {22,665},
        {22,425},
        {22,190},

        {1227,190},
        {1227,420},
        {1227,670}
    };
;
    int currentPoint = 0;

    string enemyType[3] = { "SKELETON_ONE","SKELETON_TWO","VAMPIRE" };
    int enemyScore[3] = { 50, 75, 100 };
public:
    Vector2 enemyPosition;
    bool isDead = false;
    float moveSpeed = 0.5f;
    int score;
    string curEnemy;

    Enemy() : Collidable(tag)
    {
        srand(time(0));

        int r = GetRandomValue(0,spawnPos.size() - 1);
        enemyPosition = spawnPos[r];
        
        rectangle = { enemyPosition.x, enemyPosition.y, 32, 32 };

        int enemyR = GetRandomValue(0, 2);

        curEnemy = enemyType[enemyR];
        score = enemyScore[enemyR];
    }

    bool CheckCollision(const Rectangle& playerRect) override {
        return CheckCollisionRecs(this->rectangle, playerRect);
    }

    void Update(Vector2 target)
    {
        potentialMovement = Vector2MoveTowards({ rectangle.x,rectangle.y}, target, moveSpeed);

        Rectangle potentialRect = { potentialMovement.x, potentialMovement.y, 32, 32 };

        bool collMovement = false;
        for (const auto& collidable : collidables)
        {
            if (collidable->CheckCollision(potentialRect))
            {
                collMovement = true;
                //fmt::print("Collision detected with {}\n", collidable->tag);
                break;
            }
        }

        
        if (!collMovement)
        {
            enemyPosition = potentialMovement;
        }

        rectangle.x = enemyPosition.x;
        rectangle.y = enemyPosition.y;
        
    }


    void Draw()
    {
        DrawRectangle(rectangle.x,rectangle.y, rectangle.width, rectangle.height, BLUE);
    }

    void EnemyDestroy()
    {
        //Play ANim then true

        int r = GetRandomValue(1, 10);

        if (r > 8)
        {
            hpGroup.push_back(HP_PowerUp({ rectangle.x,rectangle.y }));
        }
        playerScore += score;
        enemiesKilled++;

    }

    Rectangle ReturnRect()
    {
        return rectangle;
    }
};

class BaseArrow
{
   
public:
    Rectangle rectangle;

    Vector2 direction;
    Vector2 scale;
    Vector2 arrowPos;
    float arrowSpeed = 150.0f;
    bool arrowVisable = false;
    bool arrowAnim = false;
    bool despawned = false;

    void Draw()
    {
        DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, WHITE);
    }


    Rectangle ReturnRect()
    {
        return rectangle;
    }


    virtual void OutOfBounds(int screenWidth, int screenHeight);
    
};

class TrapArrow : public BaseArrow, Collidable
{
private:
    int dir;

public:
    TrapArrow(Vector2 pos) : Collidable(tag)
    {
        this->arrowPos = pos;
        this->scale = Vector2{ 12, 32 };

        this->rectangle = { arrowPos.x, arrowPos.y, scale.x, scale.y };

        this->dir = pos.y <= 1 ? 1 : -1;
    }

    bool CheckCollision(const Rectangle& enemyRect) override {
        return CheckCollisionRecs(this->rectangle, enemyRect);
    }

    void Update()
    {
        float deltaTime = GetFrameTime();

        rectangle.y += dir * arrowSpeed * deltaTime;
    }

    void OutOfBounds(int screenWidth, int screenHeight) override
    {
        despawned = false;

        if (rectangle.x > screenWidth || rectangle.x < 0)
        {
            despawned = true;
        }

        if (rectangle.y > screenHeight || rectangle.y < 0)
        {
            despawned = true;
        }
    }
};

class Arrow : public BaseArrow , Collidable{
public:

    int offsetX = 0;
    int offsetY = 0;

    float timer = 0;

    Arrow(Vector2 pos, Vector2 dir) : Collidable(tag)
    {

        this->arrowPos = pos;
        this->direction = dir;

        offsetX = direction.x > 0 || direction.x < 0 ? 0 : 10;
        offsetY = direction.y > 0 || direction.y < 0 ? 0 : 10;
        //cout << offsetX << " " << offsetY << endl;
        scale = direction.x > 0 || direction.x < 0 ? Vector2{32, 12} : Vector2{12, 32};

        rectangle = { arrowPos.x, arrowPos.y, scale.x, scale.y};
    }


    bool CheckCollision(const Rectangle& enemyRect) override {
        return CheckCollisionRecs(this->rectangle, enemyRect);
    }

    void Update(Vector2 playerPos)
    {
        float deltaTime = GetFrameTime();

        // Arrow Movement
        if (arrowVisable)
        {
            arrowAnim = true;

            timer += deltaTime;

            if (timer > 10)
            {
                arrowVisable = false;
                arrowAnim = false;
                timer = 0;
            }

            rectangle.x += direction.x * arrowSpeed * deltaTime;
            rectangle.y += direction.y * arrowSpeed * deltaTime;
        }
        else
        {
            rectangle.x = playerPos.x + offsetX + (32 * (direction.x));
            rectangle.y = playerPos.y + offsetY + (32 * (direction.y));
        } 
    }

};

void DeleteEntities(vector<Enemy>* enemyGroup,vector<Arrow>* arrowGroup, vector<TrapArrow>* trapArrowGroup, vector<HP_PowerUp>* hpGroup)
{
    if((*enemyGroup).size() > 0)
    {
        // To Delete Entities
        for (int i = (*enemyGroup).size() - 1; i >= 0; i--) //iterating backwards
        {
            if ((*enemyGroup)[i].isDead)
            {
                (*enemyGroup).erase((*enemyGroup).begin() + i);
            }
        }
    }

    if((*arrowGroup).size() > 0)
    {
        // To Delete Entities
        for (int i = (*arrowGroup).size() - 1; i >= 0; i--) //iterating backwards
        {
            if ((*arrowGroup)[i].despawned)
            {
                (*arrowGroup).erase((*arrowGroup).begin() + i);
            }
        }
    }

    if ((*trapArrowGroup).size() > 0)
    {
        // To Delete Entities
        for (int i = (*trapArrowGroup).size() - 1; i >= 0; i--) //iterating backwards
        {
            if ((*trapArrowGroup)[i].despawned)
            {
                (*trapArrowGroup).erase((*trapArrowGroup).begin() + i);
            }
        }
    }

    if ((*hpGroup).size() > 0)
    {
        // To Delete Entities
        for (int i = (*hpGroup).size() - 1; i >= 0; i--) //iterating backwards
        {
            if ((*hpGroup)[i].collected)
            {
                (*hpGroup).erase((*hpGroup).begin() + i);
            }
        }
    }
}
float arrowTimer = 1;
bool arrowVisable = false;
bool arrowSpawn = true;

void ArrowDelay(float deltaTime)
{
    if(arrowVisable)
    {
        if (arrowTimer < 1)
        {
            arrowTimer += deltaTime;
        }
        else if (arrowTimer > 1)
        {
            arrowVisable = false;
            arrowSpawn = true;
        }
    }
    //cout << arrowTimer << endl;
}

const int maxTrapArrows = 5;

vector<Vector2> SpawnTrapArrows(int screenWidth, int screenHeight)
{
    int randDir = GetRandomValue(0, 1);
    vector<Vector2> arrowSpawn;

    int randX = 0;
    int spawned = 0;
    
    int dir = randDir == 0 ? 1 : screenHeight;

    while (spawned < maxTrapArrows)
    {
        srand(time(0));
        randX = GetRandomValue(0, screenWidth);

        if(arrowSpawn.size() > 0)
        {
            for (auto& pos : arrowSpawn)
            { 
                if(randX != pos.x)
                {
                    arrowSpawn.push_back({ (float)randX,(float)dir });
                    spawned++;
                    break;
                }
            }

        }
        else
        {
            //cout << randX << endl;
            arrowSpawn.push_back({ (float)randX,(float)dir });
            spawned++;
        }
    }
    

    return arrowSpawn;
}

void BaseArrow::OutOfBounds(int screenWidth, int screenHeight)
{
    despawned = false;
    if(arrowAnim == true)
    {
        if (rectangle.x > screenWidth || rectangle.x < 0)
        {
            despawned = true;
        }

        if (rectangle.y > screenHeight || rectangle.y < 0)
        {
            despawned = true;
        }

        // if it hits any tiles
        for (const auto& collidable : collidables)
        {
            if (collidable->CheckCollision(rectangle))
            {
                despawned = true;
            }
        }
    }

}


float spawnTimer = 0;
float spawnTimerTrap = 0;

int spawnMaxTime = 2;
int spawnMaxTimeTrap = 2;


void SpawnEnemies(int screenWidth, int screenHeight)
{
    float deltaTime = GetFrameTime();

    if (enemyGroup.size() < maxEnemies)
    {
        spawnTimer += deltaTime;

        if (spawnTimer > spawnMaxTimeTrap)
        {
            srand(time(0));
            spawnMaxTimeTrap = GetRandomValue(2, 5);
            spawnTimer = 0;
            enemyGroup.push_back(Enemy());
        }
    }

    if (rounds >= 2)
    {
        spawnTimerTrap += deltaTime;

        if (spawnTimerTrap > spawnMaxTime)
        {
            srand(time(0));
            spawnMaxTime = GetRandomValue(4, 6);
            spawnTimerTrap = 0;

            vector<Vector2> arwSpawns = SpawnTrapArrows(screenWidth, screenHeight);

            for (auto& arSpwn : arwSpawns)
            {
                cout << arSpwn.x << " " << arSpwn.y << endl;
            }

            for (auto& pos : arwSpawns)
            {
                trapArrowGroup.push_back(TrapArrow(pos));
            }
        }
    }
}

int main()
{
    // Initialize Raylib window
    int screenWidth = 1280;
    int screenHeight = 960;

    float scaleFactor = 2.0f; // This can be any positive float. 2.0 means twice as big.

    InitWindow(screenWidth, screenHeight, "Raylib and Tileson Example");
    SetTargetFPS(60);

    // Load the Tiled map using Tileson
    tson::Tileson tileson;
    std::unique_ptr<tson::Map> map = tileson.parse("map/overworld.tmj");

    if (map->getStatus() != tson::ParseStatus::OK)
    {        
        fmt::print("Error: {}\n", "Failed to load map!");
        return -1;
    }

    // Load the tileset texture (assuming there is one tileset or you want the first)
    Texture2D tilesetTexture = LoadTexture(map->getTilesets()[0].getImage().u8string().c_str()); 
    Texture2D tilesetTexture2 = LoadTexture(map->getTilesets()[1].getImage().u8string().c_str()); 
    Texture2D tilesetTexture3 = LoadTexture(map->getTilesets()[2].getImage().u8string().c_str()); 

    if (tilesetTexture.id==0)
    {   
        fmt::print("Error: {}\n", "Tileset texture not loaded!");
        return -1;
    }
    

    // Iterate through the map layers
    for (auto& layer : map->getLayers())
    {
        if (layer.getType() == tson::LayerType::TileLayer)
        {
        
            // Iterate through the tile objects in the layer
            for (auto& [pos, tileObject] : layer.getTileObjects())
            {
                // Check if the tile has an object group with objects
                if (tileObject.getTile()->getObjectgroup().getObjects().size() > 0)
                {
                    // Check if the tile has a "collidable" property set to true
                    if (tileObject.getTile()->getProperties().hasProperty("collidable")) {
                        if (tileObject.getTile()->getProperties().getProperty("collidable")->getValue<bool>()) {
                            fmt::print("Loading Collidable Object \n");

                            std::string tag;
                            if (tileObject.getTile()->getProperties().hasProperty("tag")) {
                                tag = tileObject.getTile()->getProperties().getProperty("tag")->getValue<std::string>();
                            }

                            // Iterate through the objects in the tile's object group
                            for (auto& obj : tileObject.getTile()->getObjectgroup().getObjects())
                            {
                        
                                if (obj.getPolygons().empty()) { //we assume a rectangle boudning box if no polygon is detected
                                    // Calculate the scaled position and size of the tile
                                    Rectangle colliderRect = {
                                        (float)(tileObject.getPosition().x + obj.getPosition().x) * scaleFactor,
                                        (float)(tileObject.getPosition().y + obj.getPosition().y) * scaleFactor,
                                        (float)obj.getSize().x * scaleFactor,
                                        (float)obj.getSize().y * scaleFactor
                                    };

                                    // Create a CollidableRectangle and add it to the collidables vector
                                    collidables.push_back(std::make_unique<CollidableRectangle>(colliderRect, tag));
                                }

                                if (!obj.getPolygons().empty()) { //if there are polygons, we assume the object is a polygon
                                    const std::vector<tson::Vector2i>& polygonPoints = obj.getPolygons();

                                    std::vector<Vector2> scaledPolygonPoints;

                                    for (const auto& point : polygonPoints) {
                                        // Scale the polygon points based on the scale factor
                                        Vector2 scaledPoint = {
                                            (float)(tileObject.getPosition().x + obj.getPosition().x + point.x) * scaleFactor,
                                            (float)(tileObject.getPosition().y + obj.getPosition().y + point.y) * scaleFactor
                                        };
                                        scaledPolygonPoints.push_back(scaledPoint);
                                    }

                                    // Create a CollidablePolygon and add it to the collidables vector
                                    collidables.push_back(std::make_unique<CollidablePolygon>(scaledPolygonPoints, tag));                                    

                                }
                            }
                        }
				
					}
				}
			}

        }
    }
    cout << collidables.size();

    // Define player properties
    int lookingX = 0;
    int lookingY = 0;
    Vector2 playerPosition = { 625, 355 }; 
    Vector2 attackPosition = playerPosition;

    const int playerSize = 32; // Example size, adjust based on your player's sprite size
    Rectangle playerRect = { playerPosition.x, playerPosition.y, playerSize , playerSize  };
    Rectangle playerAttackRect = { playerPosition.x, playerPosition.y, playerSize , playerSize };

    float playerSpeed = 150.0f;
    bool attackVisable = false;

    bool attackAnim = false;

    float timer = 0;

    Vector2 potentialAttackPosition = playerPosition;
    Vector2 potentialArrowPosition = playerPosition;
    Vector2 direction = {0,0};


    // Main game loop
    while (!WindowShouldClose())
    {
        //update
        float deltaTime = GetFrameTime();

        // Calculate potential new position based on input
        Vector2 potentialPosition = playerPosition;
        
        if (IsKeyPressed(KEY_E) && attackAnim == false)
        {
            attackVisable = true;
        }

        if((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) && !attackVisable)
        {
            potentialPosition.x += (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * playerSpeed * deltaTime;

            direction.y = 0;
            direction.x = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT); 

            potentialAttackPosition.y = potentialPosition.y;
            lookingX = IsKeyDown(KEY_RIGHT) ? 32 : -32;
            lookingY = 0;
            potentialAttackPosition.x = potentialPosition.x + lookingX;

        }
        else if((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP)) && !attackVisable)
        {
            potentialPosition.y += (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)) * playerSpeed * deltaTime;

            if (arrowVisable == false)
            {
                direction.x = 0;
                direction.y = IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP);
            }

            potentialAttackPosition.x = potentialPosition.x;
            lookingY = IsKeyDown(KEY_DOWN) ? 32 : -32;
            lookingX = 0;
            potentialAttackPosition.y = potentialPosition.y + lookingY;
        }

        if (IsKeyDown(KEY_Q) && attackAnim == false && arrowSpawn == true)
        {
            arrowGroup.push_back(Arrow({ playerPosition.x,playerPosition.y },direction));
            arrowSpawn = false;
        }

        if (IsKeyReleased(KEY_Q) && arrowVisable == false)
        {
            arrowVisable = true;
            arrowTimer = 0;

            if (arrowGroup.size() > 0)
            {
                for (auto& arrow : arrowGroup)
                {
                    arrow.arrowVisable = true;
                }
            }
        }
 
        //cout << playerPosition.x << " " << playerPosition.y << endl;
        
        // Attack Anim and Delay
        if (attackVisable)
        {
            attackAnim = true;

            timer += deltaTime;

            if (timer > 1)
            {
                attackVisable = false;
                attackAnim = false;
                timer = 0;
            }

        }
       

        // Create a rectangle for the potential new position
        Rectangle potentialPlayerRect = { potentialPosition.x, potentialPosition.y, playerRect.width, playerRect.height };
      
         // Check for collisions
        bool collisionDetected = false;
    

        // Iterate through the collidable objects and check for collisions
        for (const auto& collidable : collidables)
        {
            if (collidable->CheckCollision(potentialPlayerRect))
            {
				collisionDetected = true;
                //fmt::print("Collision detected with {}\n", collidable->tag);
				break;
			}
		}

        // Update the player's position if no collision is detected
        if (!collisionDetected)
        {
            playerPosition = potentialPosition;
            attackPosition = potentialAttackPosition;
        }

        // Update player rectangle's position for drawing
        playerRect.x = playerPosition.x;
        playerRect.y = playerPosition.y;

        playerAttackRect.x = attackPosition.x;
        playerAttackRect.y = attackPosition.y;


        //  Enemy Related Collision
        if (enemyGroup.size() > 0)
        {
            for (auto& enemy : enemyGroup)
            {
                if (enemy.CheckCollision(playerAttackRect) && attackVisable)
                {
                    enemy.isDead = true;
                    enemy.EnemyDestroy();
                    //std::cout << "ouch";
                }

                if (enemy.CheckCollision(playerRect))
                {
                    std::cout << "player_ouch";
                    playerHP--;
                    enemy.isDead = true;
                }

                enemy.Update(playerPosition);
            }
        }

        if (arrowGroup.size() > 0)
        {
            for (auto& arrow : arrowGroup)
            {
                if(arrow.arrowAnim)
                {
                    for (auto& enemy : enemyGroup)
                    {
                        if (arrow.CheckCollision(enemy.ReturnRect()))
                        {
                            enemy.isDead = true;
                            arrow.despawned = true;
                            enemy.EnemyDestroy();
                        }
                    }
                }

                arrow.Update({ playerRect.x,playerRect.y });
            }
        }

        if (trapArrowGroup.size() > 0)
        {
            for (auto& arrow : trapArrowGroup)
            {  
                
                if (arrow.CheckCollision(playerRect))
                {
                    playerHP--;
                    arrow.despawned = true;
                }

                arrow.Update();
            }
        }

        if (hpGroup.size() > 0)
        {
            for (auto& hp : hpGroup)
            {
                hp.Update(playerRect);
            }
        }

        ArrowDelay(deltaTime);

        SpawnEnemies(screenWidth, screenHeight);
        //cout << arrowGroup.size();
        DeleteEntities(&enemyGroup,&arrowGroup, &trapArrowGroup, &hpGroup);
        if (arrowGroup.size() > 0)
        {
            for (auto& arrow : arrowGroup)
            {
                arrow.OutOfBounds(screenWidth, screenHeight);
            }
        }

        if (trapArrowGroup.size() > 0)
        {
            for (auto& arrow : trapArrowGroup)
            {
                arrow.OutOfBounds(screenWidth, screenHeight);
            }
        }

        // Rounds
        if (enemiesKilled > enemyKilledMax)
        {
            enemiesKilled = 0;
            enemyKilledMax += 5;
            rounds++;
        }

        BeginDrawing();
        ClearBackground(WHITE);

        // Draw the map layers
        for (auto& layer : map->getLayers())
        {
            if (layer.getType() == tson::LayerType::TileLayer)
            {
                // Iterate through the tile objects in the layer
                for (auto& [pos, tileObject] : layer.getTileObjects())
                {
                    tson::Rect drawingRect = tileObject.getDrawingRect();

                    // Calculate the destination rectangle for drawing the tile
                    Rectangle destRect = {
                        (float)tileObject.getPosition().x* scaleFactor,
                        (float)tileObject.getPosition().y* scaleFactor,
                        (float)map->getTileSize().x* scaleFactor,
                        (float)map->getTileSize().y* scaleFactor
                    };

               

                    // Calculate the source rectangle for the tile in the tileset texture
                    Rectangle sourceRect = {
                        (float)drawingRect.x,
                        (float)drawingRect.y,
                        (float)drawingRect.width,
                        (float)drawingRect.height
                    };

                    // Draw the tile
                    if (layer.getId() == 4)
                    {
                        DrawTexturePro(tilesetTexture, sourceRect, destRect, Vector2{ 0, 0 }, 0.0f, WHITE);
                    }
                    else if (layer.getId() == 6)
                    {
                        DrawTexturePro(tilesetTexture2, sourceRect, destRect, Vector2{ 0, 0 }, 0.0f, WHITE);
                    }
                    else if (layer.getId() == 5 || layer.getId() == 7)
                    {
                        DrawTexturePro(tilesetTexture3, sourceRect, destRect, Vector2{ 0, 0 }, 0.0f, WHITE);

                    }


                }
            }
        }


        //draw the player
        DrawRectangle(playerPosition.x, playerPosition.y, playerSize, playerSize, BLACK);
        if(attackVisable)
        { 
            DrawRectangle(attackPosition.x, attackPosition.y, playerSize, playerSize, RED);
        }
       
        for(auto& enemy : enemyGroup)
        {
            enemy.Draw();
        }

        if (arrowGroup.size() > 0)
        {
            for (auto& arrow : arrowGroup)
            {
                arrow.Draw();
            }
        }

        if (trapArrowGroup.size() > 0)
        {
            for (auto& trapArrow : trapArrowGroup)
            {
                trapArrow.Draw();
            }
        }

        if (hpGroup.size() > 0)
        {
            for (auto& hp : hpGroup)
            {
                hp.Draw();
            }
        }

        DrawText(TextFormat("HP: %02i", playerHP), playerRect.x - 10, playerRect.y - 20, 20, BLACK);
        DrawText(TextFormat("Score: %02i", playerScore), 0, 0, 50, BLACK);
        DrawText(TextFormat("Rounds: %02i", rounds), screenWidth - 300, 0, 50, BLACK);


        EndDrawing();
    }

    // Unload the tileset texture
    UnloadTexture(tilesetTexture);

    // Close the Raylib window
    CloseWindow();

    return 0;
}
