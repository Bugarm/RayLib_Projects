#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <raymath.h>
#include <random>

using namespace std;

enum TileType {
    EMPTY = 0,
    DOOR = 1,
    ENEMYPATH = 2,
    DEFENDERS = 3,

    WALL_L = 4,
    WALL_R = 5,
    WALL_B = 6,
    WALL_T = 7,
    WALL_LC = 8,
    WALL_RC = 9,
    WALL_LCT = 10,
    WALL_RCT = 11,
    WALL_EMPTY = 12,

    DOORBREAK = 13
};

enum Waves{
    WAVE1 = 5,
    WAVE2 = 10,
    WAVE3 = 15
};

enum DefenderTypes {
    KNIGHT,
    WIZARD,
    ARCHER
};

enum EnemyTypes{
    NECRO,
    DEMON,
    SWAMPY
};
      
enum DefenderCost{
    KNIGHT_COST = 150,
    ARCHER_COST = 170,
    WIZARD_COST = 200
};

// Game Data
int doorHP = 0;
int enemiesKilled = 0;
int waveCount = 1;
bool endGame = false;

int money = 0;
bool hasBought = false;

const int rows = 16;
const int cols = 16;
const int tileSize = 40;
int offset = 20;

//
class Game;

class Defender;
class Enemy;
class RemoveDefenderUI;
class ButtonExit;
class CostDefender;
class EnemyUI;
class MoneyUI;
class WaveUI;
class QuitUI;

// textures
Texture2D pathTexture;
Texture2D enemyPathTexture;
Texture2D doorTexture;
Texture2D openDoorTexture;

// Enemies
Texture2D necromancerTexture;
Texture2D demonTexture;
Texture2D swampyTexture;

// Defenders
Texture2D knightTexture;
Texture2D archerTexture;
Texture2D wizardTexture;

// Defender Heart
Texture2D full_heartTexture;
Texture2D half_heartTexture;
Texture2D empty_heartTexture;

// UI
Texture2D costUITexture;
Texture2D enemyUITexture;
Texture2D moneyUITexture;
Texture2D removeDefUITexture;

// Walls Types
Texture2D wallTopTexture;
Texture2D wallBottomTexture;
Texture2D wallRightTexture;
Texture2D wallLeftTexture;

Texture2D wallLeftEdgeTexture;
Texture2D wallRightEdgeTexture;

Texture2D wallTopLeftEdgeTexture;
Texture2D wallTopRightEdgeTexture;

Texture2D wallEmptyTexture;

// bg music
Music bgMusic;

// Sound
Sound hitSound;
Sound ouchSound;
Sound doorBangSound;
Sound doorBreakSound;

int map[rows][cols] = {
    {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12},
    {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12},
    {12, 10, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 11, 12},
    {12, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 12},
    {12, 5, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 12},
    {12, 1, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 0, 4, 12},
    {12, 1, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 0, 4, 12},
    {12, 5, 6, 0, 0, 2, 2, 0, 2, 2, 0, 2, 2, 0, 4, 12},
    {12, 5, 0, 0, 0, 2, 2, 0, 2, 2, 0, 2, 2, 0, 4, 12},
    {12, 5, 0, 0, 0, 2, 2, 2, 2, 2, 0, 2, 2, 0, 4, 12},
    {12, 5, 0, 0, 0, 2, 2, 2, 2, 2, 0, 2, 2, 0, 4, 12},
    {12, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 4, 12},
    {12, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 4, 12},
    {12, 8, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 9, 12},
    {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12},
    {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12}
};

// Loads & Unloads Assets

void LoadTextures()
{
    //Load texture from images
    pathTexture = LoadTexture("images/tile_path.png");
    enemyPathTexture = LoadTexture("images/enemy_path.png");
    doorTexture = LoadTexture("images/door.png");
    openDoorTexture = LoadTexture("images/doorOpen.png");

    // Defenders
    wizardTexture = LoadTexture("images/wizard.png");
    knightTexture = LoadTexture("images/knight.png");
    archerTexture = LoadTexture("images/archer.png");

    // Enemies
    necromancerTexture = LoadTexture("images/necromancer.png");
    demonTexture = LoadTexture("images/demon.png");
    swampyTexture = LoadTexture("images/swampy.png");

    // Defender Heart
    full_heartTexture = LoadTexture("images/ui_heart_full.png");
    half_heartTexture = LoadTexture("images/ui_heart_half.png");
    empty_heartTexture = LoadTexture("images/ui_heart_empty.png");

    // UI
    costUITexture = LoadTexture("images/costUI.png");
    enemyUITexture = LoadTexture("images/enemyUI.png");
    moneyUITexture = LoadTexture("images/moneyUI.png");
    removeDefUITexture = LoadTexture("images/removeDefUI.png");

    // Wall types
    wallTopTexture = LoadTexture("images/wall_top_mid.png");
    wallBottomTexture = LoadTexture("images/wall_bottom_mid.png");
    wallRightTexture = LoadTexture("images/wall_edge_mid_right.png");
    wallLeftTexture = LoadTexture("images/wall_edge_mid_left.png");

    wallTopLeftEdgeTexture = LoadTexture("images/wall_outer_top_left.png");
    wallTopRightEdgeTexture = LoadTexture("images/wall_outer_top_right.png");

    wallLeftEdgeTexture = LoadTexture("images/wall_outer_bottom_left.png");
    wallRightEdgeTexture = LoadTexture("images/wall_outer_bottom_right.png");

    wallEmptyTexture = LoadTexture("images/wall_mid.png");
    
}

void UnLoadTextures()
{
    //unloads texture to free memory
    UnloadTexture(pathTexture);
    UnloadTexture(enemyPathTexture);
    UnloadTexture(wizardTexture);
    UnloadTexture(knightTexture);
    UnloadTexture(archerTexture);
    UnloadTexture(necromancerTexture);
    UnloadTexture(demonTexture);
    UnloadTexture(swampyTexture);
    UnloadTexture(doorTexture);

    // Defenders Heart
    UnloadTexture(full_heartTexture);
    UnloadTexture(half_heartTexture);
    UnloadTexture(empty_heartTexture);

    // UI
    UnloadTexture(costUITexture);
    UnloadTexture(enemyUITexture);
    UnloadTexture(moneyUITexture);
    UnloadTexture(removeDefUITexture);

    // Wall 
    UnloadTexture(wallTopTexture);
    UnloadTexture(wallBottomTexture);
    UnloadTexture(wallRightTexture);
    UnloadTexture(wallLeftTexture);

    UnloadTexture(wallLeftEdgeTexture);
    UnloadTexture(wallRightEdgeTexture);

    UnloadTexture(wallTopLeftEdgeTexture);
    UnloadTexture(wallRightEdgeTexture);

    UnloadTexture(wallEmptyTexture);
}

// Sound

void LoadSoundsAudio()
{
    hitSound = LoadSound("audio/hit.wav");
    ouchSound = LoadSound("audio/ouch.wav");
    doorBangSound = LoadSound("audio/DamageEffect.mp3");
    doorBreakSound = LoadSound("audio/DoorBreak.mp3");
}

void AdjustSoundAudio()
{
    SetSoundVolume(doorBangSound,0.15f);
    SetSoundVolume(doorBreakSound,0.15f);
    SetSoundVolume(ouchSound,0.15f);
    SetSoundVolume(hitSound,0.5f);
}

void UnLoadSoundsAudio()
{
    UnloadSound(hitSound);
    UnloadSound(ouchSound);
    UnloadSound(doorBangSound);
    UnloadSound(doorBreakSound);
}

// Store
vector<Defender> defendersGroup;
vector<Enemy> enemiesGroup;
vector<CostDefender> costDefGroup;

void DrawMap() {
    //TODO

    //Loop through each row
    for (int row = 0; row < rows; ++row)
    {
        //Loop through each column
        for (int col = 0; col < cols; ++col)
        {
            //Calculate the position to draw at based on the row and the column
            int posX = col * tileSize;
            int posY = row * tileSize;
            
            
            DrawTexture(pathTexture, posX, posY, WHITE);

            if(map[row][col] == ENEMYPATH)
            {
                DrawTexture(enemyPathTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_B)
            {
                DrawTexture(wallBottomTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_L)
            {
                DrawTexture(wallLeftTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_R)
            {
                DrawTexture(wallRightTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_T)
            {
                DrawTexture(wallTopTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_RC)
            {
                DrawTexture(wallRightEdgeTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_LC)
            {
                DrawTexture(wallLeftEdgeTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_LCT)
            {
                DrawTexture(wallTopLeftEdgeTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_RCT)
            {
                DrawTexture(wallTopRightEdgeTexture, posX, posY, WHITE);
            }

            if (map[row][col] == WALL_EMPTY)
            {
                DrawTexture(wallEmptyTexture, posX, posY, WHITE);
            }

            if (map[row][col] == DOOR)
            {
                DrawTexture(doorTexture, posX, posY, WHITE);
            }

            if (map[row][col] == DOORBREAK)
            {
                DrawTexture(openDoorTexture, posX, posY, WHITE);
            }
        }
    }
}

struct CellPosition{
    int x;
    int y;
};

// Default texture
DefenderTypes textureDef = KNIGHT;
EnemyTypes textureEnemy = NECRO;

// Classes

// Entities

class DefaultEntity
{
    public:
        Color color = WHITE;
        bool active = true;

        bool isHit = false;

        float delayTimer;

        int HP;
        int attackPower;
        float attackRange;
        float attackSpeed;

        Vector2 position;
        Texture2D texture;

        DefaultEntity(Vector2 position)
        {
            this->position = position;
        }

        virtual void Draw()
        {
            color = isHit == false ? WHITE : RED;
            
            CreateRange();

            DrawTexture(texture,floor(5 + position.x * tileSize), floor(position.y * tileSize), color);
        }

        Vector2 GetPos() const
        {
            return position;
        }

        bool GetIsActive()
        {
            return active;
        }

        void CreateRange()
        {
            DrawCircleV({offset + position.x * tileSize, offset + position.y * tileSize},attackRange,ColorAlpha(BLACK,0.1f));
        }

};

class Enemy : public DefaultEntity
{
    private:
        vector<Vector2> path = {
            {3,5.5f},
            {5.5f,5.5f},
            {5.5f,9.5f},
            {8.5f,9.5f},
            {8.5f,5.5f},
            {11.5f,5.5f},
            {11.5f,12.5f}
        };
        int currentPoint = 0;

        enum EnemyHP{
            NECRO_HP = 5,
            DEMON_HP = 7,
            SWAMPY_HP = 8
        };
        
        enum EnemyValue{
            NECRO_VALUE = 50,
            DEMON_VALUE = 100,
            SWAMPY_VALUE = 150
        };

        enum EnemyAttackPower{
            NECRO_POWER = 2,
            DEMON_POWER = 3,
            SWAMPY_POWER = 4
        };

        enum EnemyAttackRange{
            NECRO_RANGE = 70,
            DEMON_RANGE = 80,
            SWAMPY_RANGE = 90
        };

        enum EnemyAttackSpeed{
            NECRO_ATTACKSPEED = 1,
            DEMON_ATTACKSPEED = 2,
            SWAMPY_ATTACKSPEED = 2
        };


    public:
        int value;
        EnemyTypes enemies;
        float moveSpeed;
        bool hasReachedEnd = false;

        Enemy(EnemyTypes enemies) : DefaultEntity(position)
        {
            this->enemies = enemies;
            this->position = path[0];

            AttackSettings();
        }

        void Draw() override
        {
            switch (enemies)
            {
                case NECRO:
                    texture = necromancerTexture;
                    value = NECRO_VALUE;
                    break;

                case SWAMPY:
                    texture = swampyTexture;
                    value = SWAMPY_VALUE;
                    break;
                
                case DEMON:
                    texture = demonTexture;
                    value = DEMON_VALUE;
                    break;
                
                default:
                    break;
            }

            DefaultEntity::Draw();

        }

        void Update()
        {
            if(HP <= 0)
            {
                active = false;
            }
            else
            {
                DamageDefender();
                EnemyPathing();
            }
        }

        void EnemyPathing()
        {
            if (currentPoint < (int)path.size())
            {
                moveToSimplified(path[currentPoint]);

                // Check if the NPC has reached the target point
                if (position.x == path[currentPoint].x &&
                    position.y == path[currentPoint].y)
                {
                    currentPoint++; //Move to the next point in the path
                }
            }
            else if (currentPoint >= (int)path.size())
            {
                hasReachedEnd = true;
                active = false;
                doorHP += 1;
                PlaySound(doorBangSound);
                CheckDoorHP();
                RemoveEnemy();
            }
        }

        void moveToSimplified(Vector2 target)
        {
            position = Vector2MoveTowards(position, target, moveSpeed);
        }

        void DamageDefender();

        void RemoveEnemy();

        bool CheckCollision(const Defender& defender);

        void AttackSettings()
        {
            switch (enemies)
            {
                case NECRO:
                    HP = NECRO_HP;
                    attackPower = NECRO_POWER;
                    attackRange = NECRO_RANGE;
                    attackSpeed = NECRO_ATTACKSPEED;
                    moveSpeed = 0.03f;
                    break;

                case SWAMPY:
                    HP = SWAMPY_HP;
                    attackPower = SWAMPY_POWER;
                    attackRange = SWAMPY_RANGE;
                    attackSpeed = SWAMPY_ATTACKSPEED;
                    moveSpeed = 0.02f;
                    break;

                case DEMON:
                    HP = DEMON_HP;
                    attackPower = DEMON_POWER;
                    attackRange = DEMON_RANGE;
                    attackSpeed = DEMON_ATTACKSPEED;
                    moveSpeed = 0.02f;
                
                default:
                    break;
            }
        }
        
        void CheckDoorHP();

        ~Enemy()
        {

        }

};

class Defender : public DefaultEntity
{
    private:
        enum DefenderAttackSpeed{
            KNIGHT_ATTACKSPEED = 2,
            WIZARD_ATTACKSPEED = 3,
            ARCHER_ATTACKSPEED = 4
        };

        enum DefenderAttackPower{
            ARCHER_POWER = 3,
            KNIGHT_POWER = 4,
            WIZARD_POWER = 9
        };
        
        enum DefenderHP{
            KNIGHT_HP = 8,
            WIZARD_HP = 5,
            ARCHER_HP = 3
        };
        
        enum DefenderAttackRange{
            KNIGHT_RANGE = 70,
            WIZARD_RANGE = 90,
            ARCHER_RANGE = 140
        };

    public:
        int halfHp = floor(HP / 2) + 2;
        DefenderTypes defenders;
        int sellValue;
        
        Defender(Vector2 position,DefenderTypes defenders) : DefaultEntity(position)
        {
            this->defenders = defenders;

            AttackSettings();
        }

        void Draw() override
        {

            switch (defenders)
            {
                case KNIGHT:
                    texture = knightTexture;
                    break;

                case WIZARD:
                    texture = wizardTexture;
                    break;

                case ARCHER:
                    texture = archerTexture;
                    break;
                
                default:
                    break;
            }

            

            HeartDraw();

            DefaultEntity::Draw();
        }

        void Update()
        {
            if(HP <= 0)
            {
                map[(int)position.y][(int)position.x] = EMPTY;
                active = false;
            }
            else
            {
                DamageEnemy();
            }
        }

        void HeartDraw()
        {
            Texture2D curState = full_heartTexture;

            if(HP <= halfHp)
            {
                curState = half_heartTexture;
            }
            else if(HP <= 0)
            {
                curState = empty_heartTexture;
            }
            DrawTexture(curState, position.x * tileSize + 33,position.y * tileSize + 30,WHITE);
        }
        
        bool CheckCollision(const Enemy& enemy);

        void DamageEnemy();

        void AttackSettings()
        {
            switch (defenders)
            {
                case KNIGHT:
                    HP = KNIGHT_HP;
                    attackPower = KNIGHT_POWER;
                    attackRange = KNIGHT_RANGE;
                    attackSpeed = KNIGHT_ATTACKSPEED;
                    sellValue = KNIGHT_COST;
                    break;

                case WIZARD:
                    HP = WIZARD_HP;
                    attackPower = WIZARD_POWER;
                    attackRange = WIZARD_RANGE;
                    attackSpeed = WIZARD_ATTACKSPEED;
                    sellValue = WIZARD_COST;
                    break;

                case ARCHER:
                    HP = ARCHER_HP;
                    attackPower = ARCHER_POWER;
                    attackRange = ARCHER_RANGE;
                    attackSpeed = ARCHER_ATTACKSPEED;
                    sellValue = ARCHER_COST;
                    break;
                
                default:
                    break;
            }
        }

        ~Defender()
        {
            
        }
};

//UI

class UI
{
    public:
        Vector2 position;

        UI(Vector2 position) : position(position)
        {}

        virtual void Draw()
        {
            DrawTexture(costUITexture,position.x * tileSize,position.y * tileSize,WHITE);
        }
};

class CostDefender : public UI
{
    private:
        DefenderTypes defender;
        Texture2D drawDef;
        int price;

    public:
        Vector2 position;

        CostDefender(DefenderTypes defender, Vector2 position) : UI(position)
        {
            this->defender = defender;
            this->position = position;

            DrawDefender();
        }

        void Draw()
        {

            UI::Draw();

            DrawText(TextFormat("Cost: %02i", price), position.x * tileSize + 10, position.y * tileSize + 15 , 15, WHITE);
            
            DrawTexture(drawDef,floor(position.x * tileSize + 20), floor(position.y * tileSize - 50), WHITE);

            //cout << position.x << " " << position.y << endl;
        }

        void BuyDefender()
        {
            if(money >= price && hasBought == false)
            {
                switch(defender)
                {
                    case KNIGHT:
                        textureDef = KNIGHT;
                        break;
                    
                    case WIZARD:
                        textureDef = WIZARD;
                        break;
                        
                    case ARCHER:
                        textureDef = ARCHER;
                        break;
                }

                hasBought = true;

                if(!(money <= 0))
                {
                    money -= price;
                }
                
            }
        }

        void DrawDefender()
        {
            switch(defender)
            {
                case KNIGHT:
                    price = KNIGHT_COST;
                    drawDef = knightTexture;
                    break;
                
                case WIZARD:
                    price = WIZARD_COST;
                    drawDef = wizardTexture;
                    break;

                case ARCHER:
                    price = ARCHER_COST;
                    drawDef = archerTexture;
                    break;
            }

        }

        void Update()
        {

        }

        ~CostDefender()
        {

        }
};

class MoneyUI : public UI
{

    public:
        MoneyUI(Vector2 position) : UI(position)
        {}
        
        void Draw() override
        {
            DrawTexture(moneyUITexture,position.x * tileSize,position.y * tileSize,WHITE);

            DrawText(TextFormat("Money: %02i", money), position.x * tileSize + 8, position.y * tileSize + 15 , 15, BLACK);
        }

};

class EnemyUI : public UI
{

    public:
        EnemyUI(Vector2 position) : UI(position)
        {}
        
        void Draw()
        {
            DrawTexture(enemyUITexture,position.x * tileSize,position.y * tileSize,WHITE);

            DrawText(TextFormat("Enemies: %02i", doorHP), position.x * tileSize + 8, position.y * tileSize + 15 , 15, BLACK);
        }

};

class RemoveDefenderUI : public UI
{
    public:
        RemoveDefenderUI(Vector2 position) : UI(position)
        {}

        void Draw()
        {
            DrawTexture(removeDefUITexture,position.x * tileSize,position.y * tileSize,WHITE);
            // X
        }
};

class WaveUI : public UI
{
    public:
        WaveUI(Vector2 position) : UI(position)
        {}
        
        void Draw()
        {
            DrawTexture(enemyUITexture,position.x * tileSize,position.y * tileSize,WHITE);

            DrawText(TextFormat("Wave: %02i", waveCount), position.x * tileSize + 17, position.y * tileSize + 15 , 15, BLACK);
        }
};

class QuitUI : public UI
{
    public:
        QuitUI(Vector2 position) : UI(position)
        {}

        void Draw()
        {
            UI::Draw(); // Draws Rectangle

            DrawText(TextFormat("Quit Game"), position.x * tileSize + 15, position.y * tileSize + 15 , 15, WHITE);
        }
};

// Spawn Dupe functions

void SpawnDefender(Vector2 pos,DefenderTypes textureName)
{
    defendersGroup.push_back(Defender({pos.x,pos.y},textureDef));
    hasBought = false;
}

void SpawnEnemy(EnemyTypes textureName)
{
    enemiesGroup.push_back(Enemy(textureName));
}

void SpawnCostUI(DefenderTypes defenderType,Vector2 pos)
{
    costDefGroup.push_back(CostDefender(defenderType,{pos.x,pos.y}));
}

// Others

void DeleteEntities(vector<Defender>* defendersGroup,vector<Enemy>* enemiesGroup)
{
    // To Delete Entities
    for(int i = (*defendersGroup).size() - 1; i >= 0; i--) //iterating backwards
    {
        if(!(*defendersGroup)[i].GetIsActive())
        {
            (*defendersGroup).erase((*defendersGroup).begin() + i);
        }
    }

    for(int i = (*enemiesGroup).size() - 1; i >= 0; i--) //iterating backwards
    {
        if(!(*enemiesGroup)[i].GetIsActive())
        {
            if((*enemiesGroup)[i].hasReachedEnd == false)
            {
                money += (*enemiesGroup)[i].value;
            }
            enemiesKilled++;
            (*enemiesGroup).erase((*enemiesGroup).begin() + i);
        }
    }

}

void RemoveAllDefenders(vector<Defender>* defendersGroupP)
{
    if((*defendersGroupP).size() != 0)
    {
        for(int i = (*defendersGroupP).size() - 1; i >= 0; i--) //iterating backwards
        {
            money += (*defendersGroupP)[i].sellValue;

            map[(int)(*defendersGroupP)[i].position.y][(int)defendersGroup[i].position.x] = EMPTY;
            (*defendersGroupP).erase((*defendersGroupP).begin() + i);
        }
    }
}

void RemoveAllEnemies(vector<Enemy>* enemiesGroup)
{
    if((*enemiesGroup).size() != 0)
    {
        for(int i = (*enemiesGroup).size() - 1; i >= 0; i--) //iterating backwards
        {
            (*enemiesGroup).erase((*enemiesGroup).begin() + i);
        }
    }
}

void SpawnUIOnScreen()
{
    SpawnCostUI(KNIGHT,{(float) 14,(float) 5});
    SpawnCostUI(WIZARD,{(float)14,(float) 8});
    SpawnCostUI(ARCHER,{(float)14,(float) 11});
}


// Enemy & Defender Collision and Damage func

bool Enemy::CheckCollision(const Defender& defender)
{
    bool hit = CheckCollisionCircles({5 + defender.GetPos().x* tileSize,5 + defender.GetPos().y* tileSize},8,{5 + position.x * tileSize,5 + position.y * tileSize},attackRange);

    return hit;
}

void Enemy::DamageDefender()
{
    for(auto& defender : defendersGroup)
    {
        defender.isHit = false;
        if(CheckCollision(defender) == true)
        {
            float deltatime = GetFrameTime();

            delayTimer += deltatime;

            if(delayTimer >= (attackSpeed /2) * (GetFrameTime() * 16))
            {
                defender.isHit = true;
            }

            if(delayTimer >= attackSpeed * (GetFrameTime() * 16))
            {
                defender.HP -= attackPower;
                delayTimer = 0;
                defender.isHit = false;
                PlaySound(ouchSound);
                // so it checks once every damage
                DeleteEntities(&defendersGroup, &enemiesGroup);
                cout << "EF";
            }
            
        }

    }
    
}

void Enemy::CheckDoorHP()
{
    if(doorHP >= 10)
    {
        //Lose Game
        PlaySound(doorBreakSound);
        // Lose Sprite
        map[13][11] = DOORBREAK;
        map[13][12] = DOORBREAK;

        // Delete all enemies
        RemoveAllEnemies(&enemiesGroup);
        StopMusicStream(bgMusic);
    }
}

void Enemy::RemoveEnemy()
{
    DeleteEntities(&defendersGroup, &enemiesGroup);
}

bool Defender::CheckCollision(const Enemy& enemy)
{
    bool hit = CheckCollisionCircles({5 + enemy.GetPos().x * tileSize,5 + enemy.GetPos().y* tileSize},8,{5 + position.x * tileSize,5 + position.y * tileSize},attackRange);

    return hit;
}

void Defender::DamageEnemy()
{
    for(auto& enemy : enemiesGroup)
    {
        enemy.isHit = false;
        if(CheckCollision(enemy) == true)
        {
            float deltatime = GetFrameTime();

            delayTimer += deltatime;
            
            if(delayTimer >= (attackSpeed /2) * (GetFrameTime() * 16))
            {
                enemy.isHit = true;
            }
            
            if(delayTimer >= attackSpeed * (GetFrameTime() * 16))
            {
                enemy.HP -= attackPower;
                delayTimer = 0;
                enemy.isHit = false;
                PlaySound(hitSound);
                // so it checks once every damage
                DeleteEntities(&defendersGroup, &enemiesGroup);
            }
            
        }

    }
    
}

CellPosition HandleClick(){
    CellPosition cellPosition = {-1, -1};

    Vector2 mousePoint = GetMousePosition();

    //TODO
    if(mousePoint.x < tileSize * rows *3 && mousePoint.y < tileSize * cols *3)
    {
        cellPosition ={
            static_cast<int>(floor(mousePoint.x / tileSize)),
            static_cast<int>(floor(mousePoint.y / tileSize))
        };
    }

    return cellPosition;
};

// Game

class Game
{
    private:
        MoneyUI moneyUI = MoneyUI({3,1});
        EnemyUI enemyUI = EnemyUI({11,1});
        WaveUI waveUI = WaveUI({7,1});
        QuitUI quitUI = QuitUI({11,14});
        RemoveDefenderUI removeUI = RemoveDefenderUI({3,14});

        float spawnDelayTimer;
        bool hasSpawned;

        int spawnTime = 3;
        int randomIndex;

        vector<EnemyTypes> types = {NECRO,SWAMPY,DEMON};
        
    public:

    Game()
    {
        SpawnUIOnScreen();
    }

    void Draw()
    {
        moneyUI.Draw();
        enemyUI.Draw();
        removeUI.Draw();
        waveUI.Draw();
        quitUI.Draw();

        for(auto &costUI : costDefGroup)
        {
            costUI.Draw();
        }

        for(auto &defender : defendersGroup)
        {
            defender.Draw();
        }

        for(auto &enemy : enemiesGroup)
        {
            enemy.Draw();
        }

        if(doorHP >= 10)
        {
            DrawText(TextFormat("YOU LOST"),8 * tileSize - 200,8 * tileSize - 30,80,RED);
        }
    }

    void Update()
    {
        if(doorHP < 10)
        {
            UpdateEntities();
            EnemyWaves();  
        }

        ButtonHandling();
        
    }

    void UpdateEntities()
    {
        // Update the entities

        for(auto &defender : defendersGroup)
        {
            defender.Update();
        }

        for(auto &enemy : enemiesGroup)
        {
            enemy.Update();
        }
    }

    void EnemyWaves()
    {
        int spawnInc = 0; // used to spawn enemies faster
        vector<Waves> waves = {WAVE1,WAVE2,WAVE3};

        if(!(waveCount > (int)waves.size()))
        {
            if(enemiesKilled > waves[waveCount - 1])
            {
                spawnInc++;
                waveCount++;
                enemiesKilled = 0;
            }
        }

        if(hasSpawned == true)
        {
            randomIndex = GetRandomValue(0, types.size() -1);
            spawnTime = GetRandomValue(3 ,7 - spawnInc);
            hasSpawned = false;

        }
        
        float deltatime = GetFrameTime();

        spawnDelayTimer += deltatime;

        if(spawnDelayTimer >= spawnTime)
        {
            SpawnEnemy(types[randomIndex]);
            spawnDelayTimer = 0;
            hasSpawned = true;
        }
    }
    
    void ButtonHandling()
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            CellPosition cellPositions = HandleClick();

            if(doorHP < 10)
            {
                // Bought & Place Defender
                if(hasBought == true && map[cellPositions.y][cellPositions.x] == EMPTY)
                {
                    map[cellPositions.y][cellPositions.x] = DEFENDERS;
                        
                    SpawnDefender({(float)cellPositions.x,(float)cellPositions.y},textureDef);
                }

                // Defender Buy Button
                for(auto &costUI : costDefGroup)
                {
                    if((cellPositions.x == costUI.position.x || cellPositions.x == costUI.position.x + 1) && cellPositions.y == costUI.position.y)
                    {
                        costUI.BuyDefender();
                    }
                }

                // Sell all Defenders
                if( (cellPositions.x == removeUI.position.x || cellPositions.x == removeUI.position.x + 1) && cellPositions.y == removeUI.position.y)
                {
                    RemoveAllDefenders(&defendersGroup);
                }
            }

            // Quit Game
            if( (cellPositions.x == quitUI.position.x || cellPositions.x == quitUI.position.x + 1) && cellPositions.y == quitUI.position.y)
            {
                endGame = true;
            }

        }
    }
    

};


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = tileSize * rows;
    const int screenHeight = tileSize * cols;

    InitWindow(screenWidth, screenHeight, "A Medieval Tower Defence Game - Lucan Armeni");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    LoadTextures();
    Game game = Game();

    money = 1000;

    InitAudioDevice();

    LoadSoundsAudio();
    AdjustSoundAudio();

    // Background Music
    bgMusic = LoadMusicStream("audio/ColdSpaghetti.mp3");
    SetMusicVolume(bgMusic,0.10f);
    bgMusic.looping = true;
    
    PlayMusicStream(bgMusic);

    // Main game loop
    while (!WindowShouldClose() && endGame == false)    // Detect window close button or ESC key
    {
        // Update
        game.Update();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawMap();

            game.Draw();

        EndDrawing();
        //----------------------------------------------------------------------------------
    
        UpdateMusicStream(bgMusic);

    }

    // Unload Music
    StopMusicStream(bgMusic);
    UnloadMusicStream(bgMusic);

    CloseAudioDevice(); 

    UnLoadSoundsAudio();
    UnLoadTextures();
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}