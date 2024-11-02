#include "game_objects.h"

#include "raylib.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

#define MAP_NUM_ROWS 16
#define MAP_NUM_COLS 16

#define MAP_TILE_SIZE (int)(WINDOW_HEIGHT / MAP_NUM_ROWS)

#define RENDER_DISTANCE 16
#define FOV 70
#define RESOLUTION 60

#define ROTATION_SPEED 0.1f
#define WALKING_SPEED 1.0f

typedef struct {
    float distance;
    int color;
    char side;
} ScreenSlice;

typedef ScreenSlice ScreenBuffer[RESOLUTION];

int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 4, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
    { 1, 4, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

Player p = {
    .pos = { 2 * MAP_TILE_SIZE, 2 * MAP_TILE_SIZE },
    .delta = { 0, 0 },
    .rot_angle = (float)-M_PI / 3.0,
};
bool hideMap = false;

ScreenBuffer screenBuffer = { 0 };

void drawMap();
void drawRays(Player p);
void handleControls(Player* p);
void drawPlayer(Player p);
float distance(Vector2 a, Vector2 b);

void printScreenBuffer();

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raycaster");
    SetTargetFPS(60);

    p.delta.x
        = cosf(p.rot_angle) * WALKING_SPEED;
    p.delta.y = sinf(p.rot_angle) * WALKING_SPEED;

    while (!WindowShouldClose()) {
        ClearBackground(DARKGRAY);
        BeginDrawing();
        {
            const char* coords = TextFormat("X: %f Y: %f Angle: %f", p.pos.x, p.pos.y, p.rot_angle);
            drawMap();
            drawPlayer(p);
            drawRays(p);
            // Sky
            DrawRectangle(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT / 2, SKYBLUE);
            // Ground
            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2, BLUE);
            for (int i = 0; i < RESOLUTION; ++i) {

                Color color = { 0 };
                switch (screenBuffer[i].color) {
                case 1:
                    color = (Color) { 130, 130, 130, 255 };
                    break;
                case 2:
                    color = (Color) { 0, 228, 48, 255 };
                    break;
                case 3:
                    color = (Color) { 230, 41, 55, 255 };
                    break;
                case 4:
                    color = (Color) { 253, 249, 0, 255 };
                    break;
                default:
                    color = (Color) { 0, 0, 0, 255 };
                    break;
                }
                if (screenBuffer[i].side == 'H') {
                    color = (Color) { 3 * color.r / 4, 3 * color.g / 4, 3 * color.b / 4, color.a };
                }

                if (screenBuffer[i].distance < 1024) {

                    // Calculate the rectangle height inversely proportional to the distance
                    float rectHeight = 10000 / screenBuffer[i].distance; // You can adjust the 1000 for scaling
                    float rectWidth = (float)WINDOW_WIDTH / RESOLUTION; // Keep a fixed width

                    // Calculate vertical position so the rectangle is centered vertically
                    float rectY = ((float)WINDOW_HEIGHT / 2) - rectHeight / 2;

                    // Draw the rectangle with a height that changes based on the distance
                    DrawRectangle(i * rectWidth, rectY, rectWidth, rectHeight, color);
                }
                if (IsKeyDown(KEY_I)) {
                    printScreenBuffer();
                }
            }
            DrawText(coords, 70, 70, 20, WHITE);
            handleControls(&p);
        }
        EndDrawing();
    }
}

// _____________________________________________________________________________

void drawPlayer(Player p) {
    if (hideMap) {
        return;
    }
    DrawCircleV(p.pos, 5, RED);
    DrawLineEx(p.pos, (Vector2) { p.pos.x + p.delta.x * 10, p.pos.y + p.delta.y * 10 }, 4, RED);
}

void printScreenBuffer() {
    printf("Printing screen buffer\n");
    for (int i = 0; i < RESOLUTION; ++i) {
        printf("Distance: %f Color: %d\n", screenBuffer[i].distance, screenBuffer[i].color);
    }
}

void drawMap() {
    if (hideMap) {
        return;
    }

    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            Rectangle r = {
                .x = (j * (int)MAP_TILE_SIZE) + 1.0f,
                .y = (i * (int)MAP_TILE_SIZE) + 1.0f,
                .width = (int)MAP_TILE_SIZE - 1,
                .height = (int)MAP_TILE_SIZE - 1,
            };
            switch (map[i][j]) {
            case 1:
                DrawRectangleRec(r, LIGHTGRAY);
                break;
            case 2:
                DrawRectangleRec(r, GREEN);
                break;
            case 3:
                DrawRectangleRec(r, RED);
                break;
            case 4:
                DrawRectangleRec(r, YELLOW);
                break;
            default:
                DrawRectangleRec(r, BLACK);
                break;
            }
        }
    }
}

void drawRays(Player p) {
    int numRays = RESOLUTION;
    int mx, my; // map position
    int depthOfField = 0;

    Vector2 rayH = { 0, 0 };
    Vector2 rayV = { 0, 0 };

    float distH = FLT_MAX;
    float distV = FLT_MAX;

    float rayAngle;
    float xOffset, yOffset;
    int colorH;
    int colorV;

    // Horizontal Ray Casting
    for (int i = 0; i < numRays; ++i) {
        rayAngle = p.rot_angle - FOV / 2 * DEG2RAD + 1 * (i * (FOV / numRays) * DEG2RAD);
        if (rayAngle < 0) {
            rayAngle += 2 * M_PI;
        }
        if (rayAngle >= 2 * M_PI) {
            rayAngle -= 2 * M_PI;
        }
        depthOfField = 0;

        // Looking up
        if (rayAngle > M_PI) {
            rayH.y = (((int)p.pos.y >> 6) << 6) - 0.0001f;
            rayH.x = -((p.pos.y - rayH.y) / tanf(rayAngle)) + p.pos.x;
            yOffset = -(int)MAP_TILE_SIZE;
            xOffset = yOffset / tanf(rayAngle);
        }
        // Looking downward
        if (rayAngle < M_PI) {
            rayH.y = (((int)p.pos.y >> 6) << 6) + MAP_TILE_SIZE + 0.0001f;
            rayH.x = -(p.pos.y - rayH.y) / tanf(rayAngle) + p.pos.x;
            yOffset = MAP_TILE_SIZE;
            xOffset = yOffset / tanf(rayAngle);
        }

        if (rayAngle == 0 || rayAngle == M_PI) {
            rayH.x = p.pos.x + (RENDER_DISTANCE * cosf(rayAngle));
            rayH.y = p.pos.y;
            depthOfField = RENDER_DISTANCE;
        }

        // cast Horizontal Ray
        while (depthOfField < RENDER_DISTANCE) {
            mx = (int)(rayH.x) >> 6;
            my = (int)(rayH.y) >> 6;

            if (mx < 0 || mx >= MAP_NUM_COLS || my < 0 || my >= MAP_NUM_ROWS) {
                depthOfField = RENDER_DISTANCE;
                break;
            }
            if (map[my][mx] != 0) {
                depthOfField = RENDER_DISTANCE;
                colorH = map[my][mx];
                distH = distance(p.pos, rayH);
                break;
            }

            rayH.x += xOffset;
            rayH.y += yOffset;
            depthOfField++;
        }

        depthOfField = 0;

        // Looking Left
        if (rayAngle < 3 * M_PI / 2 && rayAngle > M_PI / 2) {
            rayV.x = (((int)p.pos.x >> 6) << 6) - 0.0001f;
            rayV.y = -((p.pos.x - rayV.x) * tanf(rayAngle)) + p.pos.y;
            xOffset = -(int)MAP_TILE_SIZE;
            yOffset = xOffset * tanf(rayAngle);
        }

        // Looking right
        if (rayAngle > 3 * M_PI / 2 || rayAngle < M_PI / 2) {
            rayV.x = (((int)p.pos.x >> 6) << 6) + MAP_TILE_SIZE + 0.0001f;
            rayV.y = -(p.pos.x - rayV.x) * tanf(rayAngle) + p.pos.y;
            xOffset = MAP_TILE_SIZE;
            yOffset = xOffset * tanf(rayAngle);
        }

        if (rayAngle == 0 || rayAngle == M_PI) {
            rayV.x = p.pos.x + (RENDER_DISTANCE * cosf(rayAngle));
            rayV.y = p.pos.y;
            depthOfField = RENDER_DISTANCE;
        }

        while (depthOfField < RENDER_DISTANCE) {
            mx = (int)(rayV.x) >> 6;
            my = (int)(rayV.y) >> 6;

            if (mx < 0 || mx >= MAP_NUM_COLS || my < 0 || my >= MAP_NUM_ROWS) {
                depthOfField = RENDER_DISTANCE;
                break;
            }
            if (map[my][mx] != 0) {
                depthOfField = RENDER_DISTANCE;
                colorV = map[my][mx];
                distV = distance(p.pos, rayV);
                break;
            }

            rayV.x += xOffset;
            rayV.y += yOffset;
            depthOfField++;
        }

        if (distH < distV) {
            screenBuffer[i].distance = distance(p.pos, rayH);
            screenBuffer[i].color = colorH;
            screenBuffer[i].side = 'H';
            if (hideMap) {
                return;
            }
            DrawLineEx(p.pos, rayH, 1, GREEN);
        } else {
            screenBuffer[i].distance = distance(p.pos, rayV);
            screenBuffer[i].color = colorV;
            screenBuffer[i].side = 'V';
            if (hideMap) {
                return;
            }
            DrawLineEx(p.pos, rayV, 1, GREEN);
        }
    }
}

float distance(Vector2 a, Vector2 b) {
    return sqrtf(powf(b.x - a.x, 2) + powf(b.y - a.y, 2));
}

void handleControls(Player* p) {
    float sprint_multiplier = 1;

    Vector2 nextPos = p->pos;

    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        sprint_multiplier = 3.4;
    } else {
        sprint_multiplier = 1;
    }

    if (IsKeyDown(KEY_W)) {
        nextPos.x += p->delta.x * sprint_multiplier;
        nextPos.y += p->delta.y * sprint_multiplier;
    }
    if (IsKeyDown(KEY_S)) {
        nextPos.x -= p->delta.x;
        nextPos.y -= p->delta.y;
    }

    int mapX = (int)(nextPos.x / MAP_TILE_SIZE);
    int mapY = (int)(nextPos.y / MAP_TILE_SIZE);

    if (mapX >= 0 && mapX < MAP_NUM_COLS && mapY >= 0 && mapY < MAP_NUM_ROWS && map[mapY][mapX] == 0) {
        // No wall, update position
        p->pos = nextPos;
    }

    if (IsKeyDown(KEY_A)) {
        p->rot_angle -= ROTATION_SPEED;
        if (p->rot_angle < 0) {
            p->rot_angle += 2 * M_PI;
        }
        p->delta.x = cosf(p->rot_angle) * WALKING_SPEED;
        p->delta.y = sinf(p->rot_angle) * WALKING_SPEED;
    }
    if (IsKeyDown(KEY_D)) {
        p->rot_angle += ROTATION_SPEED;
        if (p->rot_angle >= 2 * M_PI) {
            p->rot_angle -= 2 * M_PI;
        }
        p->delta.x = cosf(p->rot_angle) * WALKING_SPEED;
        p->delta.y = sinf(p->rot_angle) * WALKING_SPEED;
    }
    if (IsKeyPressed(KEY_M)) {
        hideMap = !hideMap;
    }
}
