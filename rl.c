#include <float.h>
#include <math.h>
#include <raylib.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

#define MAP_NUM_ROWS 16
#define MAP_NUM_COLS 16

#define MAP_TILE_SIZE (int)(WINDOW_HEIGHT / MAP_NUM_ROWS)

#define RENDER_DISTANCE 16
#define FOV 60
#define ROTATION_SPEED 0.1f
#define WALKING_SPEED 1.0f

typedef struct {
    Vector2 pos;
    Vector2 delta;
    float rot_angle;
} Player;

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
    { 1, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

Player p = {
    .pos = { 2 * MAP_TILE_SIZE, 2 * MAP_TILE_SIZE },
    .delta = { 0, 0 },
    .rot_angle = -M_PI / 3,
};

void drawMap();
void drawRays(Player p);
void handleControls(Player* p);
void drawPlayer(Player p);
float distance(Vector2 a, Vector2 b);

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raycaster");
    SetTargetFPS(60);

    p.delta.x = cosf(p.rot_angle) * WALKING_SPEED;
    p.delta.y = sinf(p.rot_angle) * WALKING_SPEED;

    while (!WindowShouldClose()) {
        ClearBackground(DARKGRAY);
        BeginDrawing();
        {
            const char* coords = TextFormat("X: %f Y: %f Angle: %f", p.pos.x, p.pos.y, p.rot_angle);
            drawMap();
            drawPlayer(p);
            drawRays(p);
            DrawText(coords, 70, 70, 20, WHITE);
            handleControls(&p);
        }
        EndDrawing();
    }
}

// _____________________________________________________________________________

void drawPlayer(Player p) {
    DrawCircleV(p.pos, 5, RED);
    DrawLineEx(p.pos, (Vector2) { p.pos.x + p.delta.x * 10, p.pos.y + p.delta.y * 10 }, 4, RED);
}

void drawMap() {
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
    int numRays = 20;
    int mx, my; // map position
    int depthOfField = 0;

    Vector2 rayH = { 0, 0 };
    Vector2 rayV = { 0, 0 };

    float distH = FLT_MAX;
    float distV = FLT_MAX;

    float rayAngle;
    float xOffset, yOffset;

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
                break;
            }

            rayV.x += xOffset;
            rayV.y += yOffset;
            depthOfField++;
        }

        distH = distance(p.pos, rayH);
        distV = distance(p.pos, rayV);
        if (distH < distV) {
            DrawLineEx(p.pos, rayH, 1, GREEN);
        } else {
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

    float offset = 2.0f;

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
}