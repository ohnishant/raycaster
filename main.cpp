#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
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

    /*glLineWidth(1.0f);*/
    /*glBegin(GL_LINES);*/
    /*glVertex2i(playerPos.x, playerPos.y);*/
    /*glVertex2i(playerPos.x + playerDelta.x * 5, playerPos.y + playerDelta.y * 5);*/
    /*glEnd();*/
}

float dist(Vec2 a, Vec2 b) {
    return (sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)));
}

void drawRays() {
    int r, mx, my, mp, dof;
    // ray angles
    Vec2 rayH = { 0 };
    Vec2 rayV = { 0 };

    float disH = MAXFLOAT;
    float disV = MAXFLOAT;

    float rayAngle, xo, yo;

    rayAngle = playerAngle;

    for (r = 0; r < 1; r++) {
        // Check horizontal lines
        dof = 0;
        float aTan = -1 / tan(rayAngle);
        // Looking up, angles go clockwise due to bottom right being +ve+ve quadrant
        if (rayAngle > M_PI) {
            rayH.y = (((int)playerPos.y >> 6) << 6);
            rayH.x = (playerPos.y - rayH.y) * aTan + playerPos.x;
            // Negative because looking up
            yo = -MAP_TILE_SIZE;
            xo = -yo * aTan;
        }
        // Looking down
        if (rayAngle < M_PI) {
            rayH.y = (((int)playerPos.y >> 6) << 6) + 64;
            rayH.x = (playerPos.y - rayH.y) * aTan + playerPos.x;
            yo = 64;
            xo = -yo * aTan;
        }

        // Straight Left or right
        if (rayAngle == 0 || rayAngle == M_PI) {
            rayH.x = playerPos.x;
            rayH.y = playerPos.y;
            dof = 8;
        }

        if (rayAngle < 0) {
            rayAngle += 2 * M_PI;
        }
        if (rayAngle >= 2 * M_PI) {
            rayAngle -= 2 * M_PI;
        }

        while (dof < 8) {
            mx = (int)rayH.x >> 6;
            my = (int)rayH.y >> 6;
            if (mx >= 0 && mx < MAP_NUM_COLS && my >= 0 && my < MAP_NUM_ROWS) {
                if (map[my][mx] == 1) {
                    dof = 8;
                } else {
                    rayH.x += xo;
                    rayH.y += yo;
                    dof++;
                }
            } else {
                dof = 8; // Break if out of bounds
            }
        }
        disH = dist(playerPos, rayH);
        /*glColor3f(0, 1, 0);*/
        /*glLineWidth(3);*/
        /*glBegin(GL_LINES);*/
        /*glVertex2i(playerPos.x, playerPos.y);*/
        /*glVertex2i(rayH.x, rayH.y);*/
        /*glEnd();*/

        // Check Vertical lines

        dof = 0;
        float nTan = -tan(rayAngle);
        // Looking left
        if (rayAngle > M_PI / 2 && rayAngle < 3 * M_PI / 2) {
            rayV.x = (((int)playerPos.x >> 6) << 6);
            rayV.y = (playerPos.x - rayV.x) * nTan + playerPos.y;
            // Negative because looking up
            xo = -MAP_TILE_SIZE;
            yo = -xo * nTan;
        }
        // Looking right
        if (rayAngle < M_PI / 2 || rayAngle > 3 * M_PI / 2) {
            rayV.x = (((int)playerPos.x >> 6) << 6) + 64;
            rayV.y = (playerPos.x - rayV.x) * nTan + playerPos.y;
            xo = 64;
            yo = -xo * nTan;
        }

        // Straight up or down
        if (rayAngle == 0 || rayAngle == M_PI) {
            rayV.x = playerPos.x;
            rayV.y = playerPos.y;
            dof = 8;
        }

        if (rayAngle < 0) {
            rayAngle += 2 * M_PI;
        }
        if (rayAngle >= 2 * M_PI) {
            rayAngle -= 2 * M_PI;
        }

        while (dof < 8) {
            mx = (int)rayV.x >> 6;
            my = (int)rayV.y >> 6;
            mp = my * MAP_NUM_COLS + mx;
            // Collision check
            if (mx > 0 && mx < MAP_NUM_COLS - 1 && my > 0 && my < MAP_NUM_ROWS - 1 && map[my][mx] == 1) {
                dof = 9;
            } else {
                rayV.x += xo;
                rayV.y += yo;
                dof++;
            }
        }
    }
    disV = dist(playerPos, rayV);
    /*printf("[INFO]: Ray-X: %f\n", rayX);*/
    /*printf("[INFO]: Ray-Y: %f\n", rayY);*/
    /*printf("[INFO]: Ray-Angle: %f\n", rayAngle);*/

    if (disH < disV) {
        glColor3f(1, 0, 0);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerPos.x, playerPos.y);
        glVertex2i(rayV.x, rayV.y);
        glEnd();
    } else {
        glColor3f(1, 0, 0);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerPos.x, playerPos.y);
        glVertex2i(rayH.x, rayH.y);
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    drawRays();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y) {
    if (key == 'a') {
        playerAngle -= 0.1;
        if (playerAngle < 0) {
            playerAngle += 2 * M_PI;
        }
        playerDelta.x = cosf(playerAngle) * 5;
        playerDelta.y = sinf(playerAngle) * 5;
    }
    if (key == 'd') {
        playerAngle += 0.1;
        if (playerAngle >= 2 * M_PI) {
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
