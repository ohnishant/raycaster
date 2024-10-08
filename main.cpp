#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <stdlib.h>
#include <sys/types.h>

#define WIN_WIDTH 1024
#define WIN_HEIGHT 512

#define MAP_NUM_ROWS 8
#define MAP_NUM_COLS 8
#define MAP_TILE_SIZE 64

typedef struct {
    float x, y;
} Vec2;

Vec2 playerPos = { 0 };
Vec2 playerDelta = { 0, 0 };
float playerAngle = 0;

int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    { 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1 },
};

void drawMap2D() {
    int x, y, xo, yo;
    for (y = 0; y < MAP_NUM_ROWS; ++y) {
        for (x = 0; x < MAP_NUM_COLS; ++x) {
            // Set Context color for walls
            if (map[y][x] == 1) {
                glColor3f(1, 1, 1);
            } else {
                glColor3f(0, 0, 0);
            }

            xo = x * MAP_TILE_SIZE;
            yo = y * MAP_TILE_SIZE;
            glBegin(GL_QUADS);
            glVertex2i(xo + 1, yo + 1);
            glVertex2i(xo + 1, yo + MAP_TILE_SIZE - 1);
            glVertex2i(xo + MAP_TILE_SIZE - 1, yo + MAP_TILE_SIZE - 1);
            glVertex2i(xo + MAP_TILE_SIZE - 1, yo + 1);
            glEnd();
        }
    }
}

void drawPlayer() {
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(playerPos.x, playerPos.y);
    glEnd();

    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2i(playerPos.x, playerPos.y);
    glVertex2i(playerPos.x + playerDelta.x * 5, playerPos.y + playerDelta.y * 5);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y) {
    if (key == 'a') {
        playerAngle -= 0.1;
        if (playerAngle < 0) {
            playerAngle += 2 * M_PI;
        }
        playerDelta.x = cos(playerAngle) * 5;
        playerDelta.y = sin(playerAngle) * 5;
    }
    if (key == 'd') {
        playerAngle += 0.1;
        if (playerAngle < 2 * M_PI) {
            playerAngle -= 2 * M_PI;
        }
        playerDelta.x = cos(playerAngle) * 5;
        playerDelta.y = sin(playerAngle) * 5;
    }
    if (key == 'w') {
        playerPos.x += playerDelta.x;
        playerPos.y += playerDelta.y;
    }
    if (key == 's') {
        playerPos.x -= playerDelta.x;
        playerPos.y -= playerDelta.y;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, 1024, 512, 0);

    playerPos.x = 250;
    playerPos.y = 250;

    playerDelta.x = cos(playerAngle) * 5;
    playerDelta.y = sin(playerAngle) * 5;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("raycaster");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
    return 0;
}
