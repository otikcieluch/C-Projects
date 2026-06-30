#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define MAX_BULLETS 100
#define MAX_ENEMIES 50

// --- DATA STRUCTURES ---
typedef struct {
    Vector2 position;
    float speed;
    Color color;
} Player;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool active;
    float radius;
} Bullet;

typedef struct {
    Vector2 position;
    bool active;
    float radius;
    float speed;
} Enemy;

int main(void) {
    // 1. Initialize the Window
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Cosmic Swarm - 1000x1000");
    SetTargetFPS(60); // Run at a buttery smooth 60 Frames Per Second

    // 2. Setup Game Objects
    Player player = { (Vector2){ screenWidth/2.0f, screenHeight/2.0f }, 300.0f, SKYBLUE };
    Bullet bullets[MAX_BULLETS] = { 0 };
    Enemy enemies[MAX_ENEMIES] = { 0 };
    
    int score = 0;
    bool gameOver = false;

    // 3. The Main Game Loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime(); // Keeps movement smooth regardless of computer speed

        if (!gameOver) {
            // --- PLAYER MOVEMENT (WASD) ---
            if (IsKeyDown(KEY_W)) player.position.y -= player.speed * deltaTime;
            if (IsKeyDown(KEY_S)) player.position.y += player.speed * deltaTime;
            if (IsKeyDown(KEY_A)) player.position.x -= player.speed * deltaTime;
            if (IsKeyDown(KEY_D)) player.position.x += player.speed * deltaTime;

            // Keep player inside the 1000x1000 screen
            if (player.position.x < 15) player.position.x = 15;
            if (player.position.x > screenWidth - 15) player.position.x = screenWidth - 15;
            if (player.position.y < 15) player.position.y = 15;
            if (player.position.y > screenHeight - 15) player.position.y = screenHeight - 15;

            // --- SHOOTING (Left Mouse Click) ---
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].active = true;
                        bullets[i].position = player.position;
                        bullets[i].radius = 4.0f;
                        
                        // Calculate bullet direction towards mouse
                        Vector2 mousePos = GetMousePosition();
                        float angle = atan2f(mousePos.y - player.position.y, mousePos.x - player.position.x);
                        bullets[i].velocity.x = cosf(angle) * 800.0f; // Bullet speed
                        bullets[i].velocity.y = sinf(angle) * 800.0f;
                        break;
                    }
                }
            }

            // --- ENEMY SPAWNING ---
            if (GetRandomValue(0, 100) < 2) { // 2% chance to spawn an enemy each frame
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        enemies[i].active = true;
                        enemies[i].radius = GetRandomValue(10, 25);
                        enemies[i].speed = GetRandomValue(100, 250);
                        
                        // Spawn randomly at the edges
                        if (GetRandomValue(0, 1) == 0) {
                            enemies[i].position.x = (GetRandomValue(0, 1) == 0) ? 0 : screenWidth;
                            enemies[i].position.y = GetRandomValue(0, screenHeight);
                        } else {
                            enemies[i].position.x = GetRandomValue(0, screenWidth);
                            enemies[i].position.y = (GetRandomValue(0, 1) == 0) ? 0 : screenHeight;
                        }
                        break;
                    }
                }
            }

            // --- UPDATE BULLETS & ENEMIES ---
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    bullets[i].position.x += bullets[i].velocity.x * deltaTime;
                    bullets[i].position.y += bullets[i].velocity.y * deltaTime;
                    // Deactivate if off-screen
                    if (bullets[i].position.x < 0 || bullets[i].position.x > screenWidth ||
                        bullets[i].position.y < 0 || bullets[i].position.y > screenHeight) {
                        bullets[i].active = false;
                    }
                }
            }

            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    // Enemies always chase the player
                    float angle = atan2f(player.position.y - enemies[i].position.y, player.position.x - enemies[i].position.x);
                    enemies[i].position.x += cosf(angle) * enemies[i].speed * deltaTime;
                    enemies[i].position.y += sinf(angle) * enemies[i].speed * deltaTime;

                    // Collision: Enemy hits Player
                    if (CheckCollisionCircles(player.position, 15.0f, enemies[i].position, enemies[i].radius)) {
                        gameOver = true;
                    }

                    // Collision: Bullet hits Enemy
                    for (int j = 0; j < MAX_BULLETS; j++) {
                        if (bullets[j].active && CheckCollisionCircles(bullets[j].position, bullets[j].radius, enemies[i].position, enemies[i].radius)) {
                            enemies[i].active = false;
                            bullets[j].active = false;
                            score += 10;
                            break;
                        }
                    }
                }
            }
        } else {
            // Restart game logic
            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                score = 0;
                player.position = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
            }
        }

        // --- DRAWING GRAPHICS ---
        BeginDrawing();
        ClearBackground((Color){ 15, 15, 25, 255 }); // Dark cosmic blue background

        if (!gameOver) {
            // Draw Bullets (Neon Yellow)
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) DrawCircleV(bullets[i].position, bullets[i].radius, YELLOW);
            }

            // Draw Enemies (Neon Red)
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) DrawCircleV(enemies[i].position, enemies[i].radius, RED);
            }

            // Draw Player (Neon Blue)
            DrawCircleV(player.position, 15.0f, player.color);
            DrawCircleLines(player.position.x, player.position.y, 18.0f, WHITE); // Shield outline

            // Draw UI
            DrawText(TextFormat("SCORE: %04i", score), 20, 20, 30, WHITE);
            DrawText("Aim: MOUSE  |  Shoot: LEFT CLICK  |  Move: WASD", 20, 950, 20, GRAY);
        } else {
            // Game Over Screen
            DrawText("SYSTEM FAILURE", screenWidth/2 - MeasureText("SYSTEM FAILURE", 60)/2, screenHeight/2 - 50, 60, RED);
            DrawText(TextFormat("FINAL SCORE: %i", score), screenWidth/2 - MeasureText(TextFormat("FINAL SCORE: %i", score), 30)/2, screenHeight/2 + 30, 30, WHITE);
            DrawText("PRESS ENTER TO RESTART", screenWidth/2 - MeasureText("PRESS ENTER TO RESTART", 20)/2, screenHeight/2 + 80, 20, GRAY);
        }

        EndDrawing();
    }

    // 4. Cleanup
    CloseWindow();
    return 0;
}