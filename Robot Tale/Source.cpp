#include <Windows.h>
#include <stdlib.h>
#include <math.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>
#include <iostream>
#include "functions.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 900
#define M_PI 3.1415926f
#define FPS 120

int speed = 1;
float bot9_x = 0;
float bot9_y = 0;
float bot9_angle = 0;
float bot9_scale = 0.5;
float arm_angle = 0; //0 is facing bottom, all rotates counter clockwise
float head_angle = 0; // 0 is facing right
int bot9_face = 1;
int bot9_direction = 1;
float scene0_leftpan = 0;
float cur_leftpan = 0;

float bot10_x = 0;
float bot10_y = 0;
int bot10_face = 1;
int bot10_direction = -1;
float bot10_scale = 0.2;
int face_time = 0;

float wheel_rotation = 0;
float arm_extension = 0;
float claw_x = 0;
float claw_y = 0;
float scanbar = 0;
float scanbar_2 = 0;

float seed_x = 0;
float seed_y = 0;
float seed_y2 = 0;
float plant_scale = 0;
float trash_x = 0;
float trash_y = 0;
float can_x = 0;
float can_y = 0;
float can_scale = 1;
float can_direction = 1;
float can_angle = 0;

bool bar_flip = false;//for scanbar
bool head_flip = false; // for head nod
bool draw_hands = false; //showing/hiding hands
bool draw_hands2 = false;
bool flag = true; //flag for scene 0
bool rock_behind_bot = false;
bool draw_rock = true;

int door_state = 0;//0 if closed, 1 if opened
int pause = 0; // for how long the head stops moving

//int scene = 0;
//int animation_state = 0;
/*skipping anims*/
int scene = 0;
int animation_state = 0;

void initGL() {
	glClearColor(1.0, 1.0, 1.0, 0.0);//white background
    //glClearColor(0, 0, 0, 0.0);//black background

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
}

void ellipse(float cx, float cy, float rx, float ry, int num_segments)
{
    float theta = 2 * M_PI / float(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    float x = 1;//we start at angle = 0 
    float y = 0;

    glBegin(GL_POLYGON);
    for (int ii = 0; ii <= num_segments; ii++)
    {
        //apply radius and offset
        glVertex2f(x * rx + cx, y * ry + cy);//output vertex 

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}

void semi_ellipse(float cx, float cy, float rx, float ry, int num_segments, float width = 10.0f)
{
    float theta = M_PI / float(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    float x = -1;//we start at angle = 0 
    float y = 0;
    glLineWidth(width);
    glBegin(GL_LINE_STRIP);
    for (int ii = 0; ii <= num_segments; ii++)
    {
        //apply radius and offset
        glVertex2f(x * rx + cx, y * ry + cy);//output vertex 

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}


void drawRoundedRect(float x, float y, float width, float height, float radius, int segments = 16) {
    float halfW = width / 2.0f;
    float halfH = height / 2.0f;

    float left = x - halfW + radius;
    float right = x + halfW - radius;
    float top = y + halfH - radius;
    float bottom = y - halfH + radius;

    glBegin(GL_POLYGON);

    // Top edge
    glVertex2f(left, y + halfH);
    glVertex2f(right, y + halfH);

    // Top-right corner
    for (int i = 0; i <= segments; ++i) {
        float theta = i * M_PI / 2.0f / segments;
        glVertex2f(right + sin(theta) * radius, top + cos(theta) * radius);
    }

    // Right edge
    glVertex2f(x + halfW, bottom);
    glVertex2f(x + halfW, top);

    // Bottom-right corner
    for (int i = segments; i >= 0; --i) {
        float theta = i * M_PI / 2.0f / segments + 3 * M_PI / 2.0f;
        glVertex2f(right + cos(theta) * radius, bottom + sin(theta) * radius);
    }

    // Bottom edge
    glVertex2f(right, y - halfH);
    glVertex2f(left, y - halfH);

    // Bottom-left corner
    for (int i = 0; i <= segments; ++i) {
        float theta = i * M_PI / 2 / segments + M_PI;
        glVertex2f(left + sin(theta) * radius, bottom + cos(theta) * radius);
    }

    // Left edge
    glVertex2f(x - halfW, top);
    glVertex2f(x - halfW, bottom);

    // Top-left corner
    for (int i = segments; i >= 0; --i) {
        float theta = i * M_PI / 2 / segments + M_PI / 2;
        glVertex2f(left + cos(theta) * radius, top + sin(theta) * radius);
    }

    glEnd();
}

void drawRoundedRectOutline(float x, float y, float width, float height, float radius, int segments = 16) {
    float halfW = width / 2.0f;
    float halfH = height / 2.0f;

    float left = x - halfW + radius;
    float right = x + halfW - radius;
    float top = y + halfH - radius;
    float bottom = y - halfH + radius;
    glLineWidth(5.0);
    glBegin(GL_LINE_STRIP);

    // Top edge
    glVertex2f(left, y + halfH);
    glVertex2f(right, y + halfH);

    // Top-right corner
    for (int i = 0; i <= segments; ++i) {
        float theta = i * M_PI / 2.0f / segments;
        glVertex2f(right + sin(theta) * radius, top + cos(theta) * radius);
    }

    // Right edge
    glVertex2f(x + halfW, bottom);
    glVertex2f(x + halfW, top);

    // Bottom-right corner
    for (int i = segments; i >= 0; --i) {
        float theta = i * M_PI / 2.0f / segments + 3 * M_PI / 2.0f;
        glVertex2f(right + cos(theta) * radius, bottom + sin(theta) * radius);
    }

    // Bottom edge
    glVertex2f(right, y - halfH);
    glVertex2f(left, y - halfH);

    // Bottom-left corner
    for (int i = 0; i <= segments; ++i) {
        float theta = i * M_PI / 2 / segments + M_PI;
        glVertex2f(left + sin(theta) * radius, bottom + cos(theta) * radius);
    }

    // Left edge
    glVertex2f(x - halfW, top);
    glVertex2f(x - halfW, bottom);

    // Top-left corner
    for (int i = segments; i >= 0; --i) {
        float theta = i * M_PI / 2 / segments + M_PI / 2;
        glVertex2f(left + cos(theta) * radius, top + sin(theta) * radius);
    }

    glEnd();
}

/*Backgrounds*/
/*Scene 0*/
void drawScene0() {
    glLineWidth(10.0f);

    //wall panel
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 300.0f); // Bottom-left corner
    glVertex2f(2 * SCREEN_WIDTH, 300.0f); // Bottom-right corner
    glVertex2f(2 * SCREEN_WIDTH, SCREEN_HEIGHT); // Top-right corner
    glVertex2f(0.0f, SCREEN_HEIGHT); // Top-left corner
    glEnd();
    //windows
    glColor3f(0.1, 0.1, 0.2);
    float windows_x = 100;
    for (int i = 0; i < 4; i++) {
        drawRoundedRect(windows_x, 800, 350, 600, 150);
        windows_x += 500;
    }
    
    //floor
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f); // Bottom-left corner
    glVertex2f(2 * SCREEN_WIDTH, 0.0f); // Bottom-right corner
    glVertex2f(2 * SCREEN_WIDTH, 300.0f); // Top-right corner
    glVertex2f(0.0f, 300.0f); // Top-left corner
    glEnd();

    //door and wall into next scene
    glPushMatrix();
    glTranslatef(SCREEN_WIDTH + 600, 400, 0);
    glRotated(-20, 0, 0, 1);
    glTranslatef(-SCREEN_WIDTH - 600, -400, 0);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(SCREEN_WIDTH + 600, 300.0f); // Bottom-left corner
        glVertex2f(2 * SCREEN_WIDTH, 300.0f); // Bottom-right corner
        glVertex2f(2 * SCREEN_WIDTH, SCREEN_HEIGHT); // Top-right corner
        glVertex2f(SCREEN_WIDTH + 600, SCREEN_HEIGHT); // Top-left corner
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(SCREEN_WIDTH + 1000, 400, 0);
    glRotated(-10, 0, 0, 1);
    glTranslatef(-SCREEN_WIDTH - 1000, -400, 0);
    glColor3d(0, 0, 0);
        ellipse(SCREEN_WIDTH + 1000, 200, 250, 600, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(SCREEN_WIDTH + 600, 400, 0);
    glRotated(-20, 0, 0, 1);
    glTranslatef(-SCREEN_WIDTH - 600, -400, 0);
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(SCREEN_WIDTH + 600, 0.0f); // Bottom-left corner
    glVertex2f(SCREEN_WIDTH + 1200, 0.0f); // Bottom-right corner
    glVertex2f(SCREEN_WIDTH + 1200, 300.0f); // Top-right corner
    glVertex2f(SCREEN_WIDTH + 600, 300.0f); // Top-left corner
    glEnd();
    glPopMatrix();
    glColor3f(0.9f, 0.9f, 0.85f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(SCREEN_WIDTH + 570, SCREEN_HEIGHT); 
        glVertex2f(SCREEN_WIDTH + 570, 300);
    glEnd();
}

/*Scene 1*/
void drawScene1() {
    // Draw the back wall (dark gray)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(300.0f, 400.0f); // Bottom-left corner
    glVertex2f(1000.0f, 400.0f); // Bottom-right corner
    glVertex2f(1000.0f, 900.0f); // Top-right corner
    glVertex2f(300.0f, 900.0f); // Top-left corner
    glEnd();

    // Draw the floor (medium gray)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f); // Bottom-left corner
    glVertex2f(SCREEN_WIDTH, 0.0f); // Bottom-right corner
    glVertex2f(1000.0f, 400.0f); // Top-right corner
    glVertex2f(300.0f, 400.0f); // Top-left corner
    glEnd();

    // Draw the left wall (dark gray)
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f); // Bottom-left corner
    glVertex2f(300.0f, 400.0f); // Top-right corner
    glVertex2f(300.0f, 900.0f); // Top-left corner
    glVertex2f(0.0f, 900.0f); // Bottom-right corner
    glEnd();

    // Draw the right wall (dark gray)
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(SCREEN_WIDTH, 0.0f); // Bottom-right corner
    glVertex2f(1000.0f, 400.0f); // Top-left corner
    glVertex2f(1000.0f, 900.0f); // Top-right corner
    glVertex2f(SCREEN_WIDTH, 900.0f); // Bottom-left corner
    glEnd();

    // Draw the light source (white rectangle)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(600.0f, 850.0f); // Bottom-left corner
    glVertex2f(680.0f, 850.0f); // Bottom-right corner
    glVertex2f(680.0f, 870.0f); // Top-right corner
    glVertex2f(600.0f, 870.0f); // Top-left corner
    glEnd();

    // Draw the circle in the center (light gray) using Pi
    glColor3f(0.5f, 0.5f, 0.5f);
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f + 50.0f; // Slightly higher
    float radius = 30.0f; // Slightly larger
    int numSegments = 100;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(numSegments);
        float x = radius * cos(theta);
        float y = radius * sin(theta);
        glVertex2f(centerX + x, centerY + y);
    }
    glEnd();

    // Draw the cross inside the circle (light gray)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    // Horizontal line
    glVertex2f(centerX - radius, centerY);
    glVertex2f(centerX + radius, centerY);
    // Vertical line
    glVertex2f(centerX, centerY - radius);
    glVertex2f(centerX, centerY + radius);
    glEnd();
}
/* Scene 1 end */

/* Scene 2 */
void drawScene2() {
    glColor3ub(30, 30, 30);
    glBegin(GL_POLYGON);
    glVertex2d(0.0f, 0.0f);
    glVertex2d(0.0f, SCREEN_HEIGHT);
    glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2d(SCREEN_WIDTH, 0.0f);
    glEnd();
}
/* Scene 2 end */

/* Scene 4 */
void rectangle(int x, int y, int h, int w) {

    glBegin(GL_POLYGON);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();
}

void circle(int n, int x, int y, int r) {
    double inc = 2 * M_PI / n;

    glBegin(GL_POLYGON);
    for (double theta = 0.0; theta <= 2 * M_PI; theta += inc) {
        glVertex2i(r * cos(theta) + x, r * sin(theta) + y);
    }
    glEnd();
}

/*plant*/
void plant(int x, int y, float scale = 1) {
    glColor3ub(57, 255, 20);
    rectangle(x, y, scale * 60, scale * 8);

    circle(50, x + 15, y + 60, scale * 10);
    circle(50, x - 10, y + 55, scale * 10);

    glBegin(GL_POLYGON);
    glVertex2i(x + 16, y + 70);
    glVertex2i(x + 35, y + 60);
    glVertex2i(x + 16, y + 50);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2i(x - 12, y + 65);
    glVertex2i(x - 30, y + 55);
    glVertex2i(x - 12, y + 45);
    glEnd();
}

void window1(int x, int y, int h, int w) { //window type hairuz
    glColor3ub(0, 0, 0);
    rectangle(x, y, h, w);
    //inner rectangle
    glColor3ub(76, 78, 108);
    rectangle(x + 5, y + 5, h - 10, w - 10);

}

void window2(int x, int y, int h, int w) { //window type hendra
    glColor3ub(0, 0, 0);
    rectangle(x, y, h, w);

    glColor3ub(94, 94, 94);
    rectangle(x + 4, y + 4, h - 8, w - 8);
}

void door(int x, int y, int w, int colour = 0) { // black colour if 1, normal if 0
    int h = w;
    //main
    if (colour == 0)
        glColor3ub(100, 100, 100);
    else if (colour == 1)
        glColor3f(0, 0, 0);
    rectangle(x, y, h, w);
    //small rectangle
    int innerW = w * 0.8;
    int innerH = h - 20;
    int innerX = x + (w - 15 * (w / 16));
    int innerY = y + (h / 16);

    //back circle
    if (colour == 0)
        glColor3ub(100, 100, 100);
    circle(100, x + w / 2, y + h, 75);
    if (colour == 0)
        glColor3ub(50, 50, 50);
    rectangle(innerX, innerY, innerH, innerW);
    //front circle
    if (colour == 0)
        glColor3ub(50, 50, 50);
    circle(100, x + w / 2, y + h, 60);
    if (colour == 0)
        glColor3ub(173, 177, 255);
    circle(100, x + w / 2, y + h, 20);

    //handle
    if (colour == 0)
        glColor3ub(50, 50, 50);
    circle(100, x + w - 5, y + h / 2 + 10, 5);
}

void trapezium(int x, int y, int h, int w) {
    glBegin(GL_POLYGON);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w - 10, y + h);
    glVertex2i(x + 10, y + h);
    glEnd();
}

void pot(int x, int y, int h, int w) {
    int a = x / 2 + x / 4;
    int M = w / 8;
    glColor3ub(120, 67, 21);
    glBegin(GL_POLYGON);
    glVertex2i(a, y);
    glVertex2i(a + w, y);
    glVertex2i(a + 7 * M, y + h);
    glVertex2i(a + M, y + h);
    glEnd();

    //inside
    glColor3ub(70, 30, 8);
    glBegin(GL_POLYGON);
    glVertex2i(a + 15, y);
    glVertex2i(a + w - 15, y);
    glVertex2i(a + 7 * M - 10, y + h - 10);
    glVertex2i(a + M + 10, y + h - 10);
    glEnd();

    //body
    glColor3ub(94, 53, 17);
    rectangle(a, y - 20, 20, w);
    glColor3ub(94, 53, 17);
    glBegin(GL_POLYGON);
    glVertex2i(a + 20, y - 70);
    glVertex2i(a + 100, y - 70);
    glVertex2i(a + 110, y - 10);
    glVertex2i(a + 10, y - 10);
    glEnd();
}

void drawScene4() {
    glColor3ub(30, 30, 30);
    rectangle(0, 0, 500, SCREEN_WIDTH);

    pot(SCREEN_WIDTH, 400, 40, 120);

    window2(3 * SCREEN_WIDTH / 4, 2 * SCREEN_HEIGHT / 3, 200, 600);
    window2(50, 3 * SCREEN_HEIGHT / 4 + 15, 100, 450);

    //box
    glColor3ub(114, 50, 17);
    trapezium(SCREEN_WIDTH / 4, 520, 20, 80);
    glColor3ub(105, 40, 10);
    rectangle(SCREEN_WIDTH / 4, 470, 50, 80);

    glColor3ub(114, 50, 17);
    trapezium(SCREEN_WIDTH / 4 - 200, 530, 20, 110);
    glColor3ub(105, 40, 10);
    rectangle(SCREEN_WIDTH / 4 - 200, 470, 60, 110);

    door(SCREEN_WIDTH / 2, 500, 150, 0);
}

/*Scene 4 end*/

/*Scene 5*/
void sideline(int x, int y, int h, int w) {
    glBegin(GL_POLYGON);
    glVertex2i(x, y);
    glVertex2i(x + w, y + 4);
    glVertex2i(x + w, y + h + 4);
    glVertex2i(x, y + h);
    glEnd();

}

void wallTexture(int x, int y, int h, int w) {
    glColor3ub(0, 0, 0);
    sideline(x, y, h, w);
    sideline(x + 20, y - 35, h + 300, w - 8);
}

void sidewall(int x, int y) {
    glColor3ub(70, 70, 70);
    glBegin(GL_POLYGON);
    glVertex2i(x, y);
    glVertex2i(0.42 * SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2i(0.55 * SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2i(0, SCREEN_HEIGHT);
    glEnd();

    wallTexture(x + 50, y + 50, 110, 10);
    wallTexture(x + 110, y + 70, 110, 10);
    wallTexture(x + 170, y + 85, 110, 10);
    wallTexture(x + 230, y + 105, 110, 10);
}

void door() {
    glColor3ub(40, 40, 40);
    int x = SCREEN_WIDTH / 3 + 260;
    int y = SCREEN_HEIGHT / 2 + 0.1 * SCREEN_HEIGHT;
    int h = SCREEN_HEIGHT / 2 - 250;
    int w = 200;
    rectangle(x, y, h, w);

    //back circle
    circle(100, x + w / 2, y + h, w / 2);
    //front rectangle
    glColor3ub(34, 34, 34);
    rectangle(x + 15, y + 10, h, w - 30);
    //front circle
    circle(100, x + w / 2, y + h, w / 2 - 15);
    //handle
    glColor3ub(17, 17, 17);
    circle(20, x + w - 8, y + h / 2 + 25, 5);
}

void drawScene5() {
    //background
    glColor3ub(30, 30, 30);
    glBegin(GL_POLYGON);
    glVertex2d(0, 0);
    glVertex2d(SCREEN_WIDTH, 0);
    glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2d(0, SCREEN_HEIGHT);

    //sidewall
    sidewall(0, SCREEN_HEIGHT * 0.78);

    //wall
    glColor3ub(70, 70, 70);
    rectangle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2 + 0.1 * SCREEN_HEIGHT, SCREEN_HEIGHT, SCREEN_WIDTH);
    glColor3ub(0, 0, 0);
    circle(50, SCREEN_WIDTH / 3 + 50, SCREEN_HEIGHT - 20, 5);
    circle(50, SCREEN_WIDTH / 3 + 100, SCREEN_HEIGHT - 20, 5);
    circle(50, SCREEN_WIDTH / 3 + 150, SCREEN_HEIGHT - 20, 5);
    //sidewall
    sidewall(0, SCREEN_HEIGHT * 0.78);
    //line
    glColor3ub(20, 20, 20);
    rectangle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2 + 0.1 * SCREEN_HEIGHT, SCREEN_HEIGHT, 4);
    //door
    door();
    //window
    window2(3 * SCREEN_WIDTH / 4, 4 * SCREEN_HEIGHT / 5, 60, 400);

}
/*Scene 5 end*/

/*Scene 7*/
void drawScene7() {
    glColor3ub(255, 255, 255);
    glBegin(GL_POLYGON);
    glVertex2d(0.0f, 0.0f);
    glVertex2d(0.0f, SCREEN_HEIGHT);
    glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2d(SCREEN_WIDTH, 0.0f);
    glEnd();
}

/*Scene 9 (Final Scene)*/
void nGon(int x, int y, int r, int n) {
    double inc = (2 * M_PI) / (double)n;
    glBegin(GL_POLYGON);
    for (double theta = 0.0; theta <= 2 * M_PI; theta += inc) {
        glVertex2d(x + r * cos(theta), y + r * sin(theta));
    }
    glEnd();
}

void rectangle3(int x, int y, int h, int w) {
    glBegin(GL_POLYGON);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();
}

void flower(int x, int y, int r) {
    const GLubyte colors[][3] = {
        {255, 0, 0},     // Red
        {255, 165, 0},   // Orange
        {255, 255, 0},   // Yellow
        {0, 128, 0},     // Green
        {0, 0, 255},     // Blue
        {128, 0, 128},   // Purple
        {255, 192, 203}  // Pink
    };

    int colorIndex = (x / 50 + y / 50) % 7;

    // Stem
    glColor3ub(34, 139, 34); // Forest green
    glBegin(GL_LINES);
    glVertex2i(x, y - 10);
    glVertex2i(x, y);
    glEnd();

    // Petals
    glColor3ubv(colors[colorIndex]);
    for (int i = 0; i < 6; i++) {
        double angle = i * M_PI / 3;
        int px = x + r * cos(angle);
        int py = y + r * sin(angle);
        nGon(px, py, r, 20);
    }

    // Center
    glColor3ub(255, 255, 255);
    nGon(x, y, r - 2, 20);

    glColor3ub(21, 54, 10);
    rectangle3(x - 2, y - 40, 25, 4);
}

void floor() {
    int a = 219;
    int b = 122;
    int c = 73;
    glColor3ub(a, b, c);
    rectangle3(0, SCREEN_HEIGHT / 3, 80, SCREEN_WIDTH - 400);

    glBegin(GL_POLYGON);
    glVertex2i(SCREEN_WIDTH - 400, SCREEN_HEIGHT / 3);
    glVertex2i(SCREEN_WIDTH - 200, 2 * SCREEN_HEIGHT / 3 - 50);
    glVertex2i(SCREEN_WIDTH - 200, 2 * SCREEN_HEIGHT / 3 + 5);
    glVertex2i(SCREEN_WIDTH - 400, SCREEN_HEIGHT / 3 + 80);
    glEnd();

    // DIRT base
    glColor3ub(110, 56, 23); // Base dirt color
    glBegin(GL_POLYGON);
    glVertex2i(0, SCREEN_HEIGHT / 3 + 80);
    glVertex2i(SCREEN_WIDTH - 400, SCREEN_HEIGHT / 3 + 80);
    glVertex2i(SCREEN_WIDTH - 200, 2 * SCREEN_HEIGHT / 3 + 5);
    glVertex2i(0, 2 * SCREEN_HEIGHT / 3 + 5);
    glEnd();

    // Simulated dirt texture using random patches
    int dirtStartY = SCREEN_HEIGHT / 3 + 80;
    int dirtEndY = 2 * SCREEN_HEIGHT / 3;
    int patchCount = 200;

    for (int i = 0; i < patchCount; i++) {
        int x = rand() % (SCREEN_WIDTH - 400);
        int y = dirtStartY + rand() % (dirtEndY - dirtStartY);
        int size = 3 + rand() % 3;

        // Randomly pick a light or dark shade for variation
        int r = 90 + rand() % 60;
        int g = 45 + rand() % 50;
        int b = 15 + rand() % 40;

        glColor3ub(r, g, b);
        glBegin(GL_POLYGON);
        glVertex2i(x, y);
        glVertex2i(x + size, y);
        glVertex2i(x + size, y + size);
        glVertex2i(x, y + size);
        glEnd();
    }

    // BLACK LINE
    glColor3ub(0, 0, 0);
    rectangle3(SCREEN_WIDTH - 410, SCREEN_HEIGHT / 3, 80, 10);

    //back
    glColor3ub(a, b, c);
    rectangle3(0, 2 * SCREEN_HEIGHT / 3 + 5, 4, SCREEN_WIDTH - 200);


    glColor3ub(0, 0, 0);
    rectangle3(0, SCREEN_HEIGHT / 3 + 75, 5, SCREEN_WIDTH - 400);

    glBegin(GL_POLYGON);
    glVertex2i(0, SCREEN_HEIGHT / 3 + 10);
    glVertex2i(0, SCREEN_HEIGHT / 3 + 5);
    glVertex2i(SCREEN_WIDTH - 400, SCREEN_HEIGHT / 3 + 70);
    glVertex2i(SCREEN_WIDTH - 405, SCREEN_HEIGHT / 3 + 75);
    glEnd();

    glColor3ub(0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2i(SCREEN_WIDTH - 400, SCREEN_HEIGHT / 3);
    glVertex2i(SCREEN_WIDTH - 200, 2 * SCREEN_HEIGHT / 3);
    glVertex2i(SCREEN_WIDTH - 200, 2 * SCREEN_HEIGHT / 3 + 10);
    glVertex2i(SCREEN_WIDTH - 400, SCREEN_HEIGHT / 3 + 10);
    glEnd();

    rectangle3(SCREEN_WIDTH - 205, 2 * SCREEN_HEIGHT / 3 - 60, 60, 5);

}

void window3(int x, int y, int h, int w) {
    glColor3ub(0, 0, 0);
    rectangle3(x - 50, y + 70, h, w);

    glColor3ub(173, 177, 255);
    rectangle3(x - 48, y + 72, h - 4, w - 4);
}

void drawScene10() {    
    // Wall
    glColor3ub(60, 60, 60);
    rectangle3(0, SCREEN_HEIGHT / 2 + 0.25 * SCREEN_HEIGHT, SCREEN_HEIGHT, SCREEN_WIDTH);

    // Windows
    window3(SCREEN_WIDTH / 4, 2 * SCREEN_HEIGHT / 3 + 100, 80, 250);
    window3(SCREEN_WIDTH / 2 + 100, 2 * SCREEN_HEIGHT / 3 + 100, 80, 250);

    floor();

    // Colorful flowers on dirt pile with rightward slant
    int flowerCount = 30;
    int flowersPerRow = 6; // Fewer flowers per row = more horizontal space
    int spacingX = 150;    // Wider horizontal spacing
    int spacingY = 40;     // More vertical spacing

    for (int i = 0; i < flowerCount; i++) {
        int row = i / flowersPerRow;
        int col = i % flowersPerRow;

        int slantOffset = row * 50; // Gradual rightward slant
        int xJitter = i % 10 - 5; // Horizontal variation
        int yJitter = i % 5;      // Vertical variation

        int x = 0 + col * spacingX + slantOffset + xJitter;
        int y = SCREEN_HEIGHT / 3 + 150 + row * spacingY + yJitter; // Adjusted to sit on dirt pile
        flower(x, y, 6 + i % 2); // Slightly bigger petals
    }
}
    
   

/*Characters*/
void drawBot9(float x = 500.0f, float y = 500.0f, float size = 1, int face = 1) {
    float unit = 25;
    float scale = size * unit; //25
    float height = 8 * size * 25; //200
    float width = 11 * size * 25; //275
    /*neck?*/
    glColor3d(0.8f, 0.8f, 0.8f);
    drawRoundedRect(x, y - scale * 5, width / 2, height / 6, unit / 2.5f);
    /*head*/
    
    glColor3d(1,1,1);
    drawRoundedRect(x, y, 12 * scale, 9 * scale, 25.0f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y, 12 * scale, 9 * scale, 25.0f);
    if (face == 1) {//neutral face

        /*face*/
        glColor3d(0, 0, 0);
        drawRoundedRect(x, y, width, height, 25.0f);
        /*eyes*/
        glColor3d(1, 1, 1);
        ellipse(x + 3 * scale, y, scale, 7.0f / 5.0f * scale, 2 * scale);
        ellipse(x - 3 * scale, y, scale, 7.0f / 5.0f * scale, 2 * scale);
        /*mouth*/
        glColor3d(1, 1, 1);
        semi_ellipse(x, y - 2 * scale, 7.0f / 5.0f * scale, scale, 30, scale / 2.5);
    }
    else if (face == 2) {//scanning face
        glColor3d(0, 0.2, 0.1);
        drawRoundedRect(x, y, 11 * scale, 8 * scale, scale);
        glColor3d(0, 0.7, 0);
        glPushMatrix();
        glTranslated(0, scanbar, 0);
        glLineWidth(5.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(x - 5 * scale, y - 4 * scale);
        glVertex2f(x + 5 * scale, y - 4 * scale);
        glEnd();
        glPopMatrix();
    }
    else if (face == 3) {//shows plant
        glColor3d(0, 0.2, 0.1);
        drawRoundedRect(x, y, 11 * scale, 8 * scale, scale);
        plant(x, y);
        plant(x, y - 50);
        plant(x, y - 100);
    }
    else if (face == 4) {//shows seed
        glColor3d(0, 0.2, 0.1);
        drawRoundedRect(x, y, 11 * scale, 8 * scale, scale);
        drawSeed(x, y - scale + seed_y2, 0.3);
        glPushMatrix();
        glTranslated(x, y, 0);
        glScaled(plant_scale, plant_scale, 1);
        glTranslated(-x, -y, 0);
        plant(x, y - 3.5 * scale - seed_y2);
        glPopMatrix();
    }
    /*body*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);
    /*arms*/

    /*wheels*/
    //stage right wheel
    glColor3d(0, 0, 0);
    drawRoundedRect(x + 8 * scale, y - 20 * scale, 3 * scale, 5 * scale, unit / 2.5f);
    glColor3d(120, 150, 180);
    glLineWidth(scale / 10);
        int i = 0;
        while (i < 3 * scale) {
            glBegin(GL_LINE_STRIP);
            glVertex2d(x + 9.5 * scale, y - 18 * scale - i);
            glVertex2d(x + 6.5 * scale, y - 18 * scale - i);
            i += scale;
            glEnd();
        }
    //stage left wheel
    glColor3d(0, 0, 0);
    drawRoundedRect(x - 8 * scale, y - 20 * scale, 3 * scale, 5 * scale, unit / 2.5f);
    glColor3d(200, 150, 180);
    i = 0;
    while (i < 3 * scale) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(x - 9.5 * scale, y - 18 * scale - i);
        glVertex2d(x - 6.5 * scale, y - 18 * scale - i);
        i += scale;
        glEnd();
    }
}

void drawBot9SideView(float x = 400.0f, float y = 500.0f, float size = 0.5, int face = 1) {
    float unit = 25;
    float scale = size * unit;
    
    /*wheel behind body*/
    glColor3d(0.2f, 0.2f, 0.2f);
    drawRoundedRect(x + 3 * scale, y - 20 * scale, 15 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + 3 * scale, y - 20 * scale, 15 * scale, 5 * scale, 2 * scale);
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x + scale, y - 20 * scale, 12 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + scale, y - 20 * scale, 12 * scale, 5 * scale, 2 * scale);
    glColor3d(0.5f, 0.5f, 0.5f);
    ellipse(x - 3  * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);
    ellipse(x + 1 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);
    ellipse(x + 5 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);

    /*Trashbin behind body*/
    glColor3d(0.1,0.1,0.1);
    drawRoundedRect(x - 8 * scale, y - 10 * scale, 15 * scale, 12 * scale, scale / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 8 * scale, y - 10 * scale, 15 * scale, 12 * scale, scale / 2.5f);
    glColor3d(0.1, 0.1, 0.1);
    drawRoundedRect(x - 8 * scale, y - 10 * scale, 15 * scale, 12 * scale, scale / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 8 * scale, y - 10 * scale, 15 * scale, 12 * scale, scale / 2.5f);

    /*neck*/
    glColor3d(0, 0, 0);
    drawRoundedRect(x - 4.5 * scale, y - scale * 5, 6 * scale, 8.5 * scale, scale / 2.5f);
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x - 4.5 * scale, y - scale * 5, 5.5 * scale, 8 * scale, scale / 2.5f);

    /*body*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x - 7.5 * scale, y - 13 * scale, 15 * scale, 15 * scale, scale / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 7.5 * scale, y - 13 * scale, 15 * scale, 15 * scale, scale / 2.5f);

    float head_x = x;
    float head_y = y;
    glPushMatrix();
    glTranslatef(head_x, head_y, 0);
    glRotatef(head_angle, 0, 0, 1);
    glTranslatef(-head_x, -head_y, 0);

    /*head behind*/

    glColor3d(1, 1, 1);
    drawRoundedRect(x - 4.5 * scale, y, 12 * scale, 9 * scale, scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 4.5 * scale, y, 12 * scale, 9 * scale, scale);

    /*head*/
       
    glColor3d(1, 1, 1);
    drawRoundedRect(x, y, 12 * scale, 9 * scale, scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y, 12 * scale, 9 * scale, scale);

    /*face*/
    if (face == 1) {//neutral face
        glColor3d(0, 0, 0);
        drawRoundedRect(x, y, 11 * scale, 8 * scale, scale);
        /*eyes*/
        glColor3d(1, 1, 1);
        ellipse(x + 3 * scale, y, scale, 7.0f / 5.0f * scale, 2 * scale);
        ellipse(x - 3 * scale, y, scale, 7.0f / 5.0f * scale, 2 * scale);
        /*mouth*/
        glColor3d(1, 1, 1);
        semi_ellipse(x, y - 2 * scale, 7.0f / 5.0f * scale, scale, scale, scale / 2.5f);
    }
    else if (face == 2) {//scanning face
        glColor3d(0, 0.2, 0.1);
        drawRoundedRect(x, y, 11 * scale, 8 * scale, scale);
        glColor3d(0, 0.7, 0);
        glPushMatrix();
        glTranslated(0, scanbar, 0);
        glLineWidth(5.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(x - 5 * scale, y - 4 * scale);
            glVertex2f(x + 5 * scale, y - 4 * scale);
        glEnd();
        glPopMatrix();
    }
    else if (face == 3) {//Plant display on face
        glColor3d(0, 0.2, 0.1);
        drawRoundedRect(x, y, 11 * scale, 8 * scale, scale);
        glPushMatrix();
        glTranslated(x, y - 2 * scale, 0);
        glScaled(0.5, 0.5, 1);
        glTranslated(-x, -y + 2 * scale, 0);
        plant(x, y - 2 * scale);
        glPopMatrix();
    }
        glPopMatrix();
    /*body*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x, y - 13 * scale, 15 * scale, 15 * scale, scale / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y - 13 * scale, 15 * scale, 15 * scale, scale / 2.5f);

    
    /*wheels*/
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x - 12 * scale, y - 20 * scale, 15 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 12 * scale, y - 20 * scale, 15 * scale, 5 * scale, 2 * scale);
    glColor3d(0.5f, 0.5f, 0.5f); // Medium grey (RGB: 128, 128, 128)
    drawRoundedRect(x - 14 * scale, y - 20 * scale, 12 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 14 * scale, y - 20 * scale, 12 * scale, 5 * scale, 2 * scale);
    glColor3d(0.9f, 0.9f, 0.9f);
    ellipse(x - 10 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    ellipse(x - 14 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    ellipse(x - 18 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    /*rotary display*/
    glPushMatrix();
    glTranslatef(x - 10 * scale, y - 20 * scale, 0);
    glRotatef(wheel_rotation, 0, 0, 1);
    glTranslatef(-x + 10 * scale, -y + 20 * scale, 0);
        glColor3d(255, 255, 255);
        ellipse(x - 11 * scale, y - 20 * scale, 0.4 * scale, 0.4 * scale, unit);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x - 14 * scale, y - 20 * scale, 0);
    glRotatef(wheel_rotation, 0, 0, 1);
    glTranslatef(-x + 14 * scale, -y + 20 * scale, 0);
        ellipse(x - 15 * scale, y - 20 * scale, 0.4 * scale, 0.4 * scale, unit);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x - 18 * scale, y - 20 * scale, 0);
    glRotatef(wheel_rotation, 0, 0, 1);
    glTranslatef(-x + 18 * scale, -y + 20 * scale, 0);
        ellipse(x - 19 * scale, y - 20 * scale, 0.4 * scale, 0.4 * scale, unit);
    glPopMatrix();

    /*arms*/
    float arms_x = x - 11 * scale;
    claw_x = arms_x + 2 * scale;
    float arms_y = y - 10 * scale;
    claw_y = arms_y;
    if (draw_hands) {
        glPushMatrix();
        glTranslatef(arms_x, arms_y, 0);
        glRotatef(90, 0, 0, 1); //rotates arm
        glRotatef(arm_angle, 0, 0, 1); //rotates arm
        glTranslatef(-arms_x, -arms_y, 0);

        glPushMatrix();
        glTranslatef(arms_x, arms_y, 0);
        glScalef(arm_extension, 1.0f, 1.0f); // Stretch object horizontally
        glTranslatef(-arms_x, -arms_y, 0);


        glColor3d(1, 1, 1);
        drawRoundedRect(x - 5 * scale, y - 10 * scale, 13 * scale, 2 * scale, unit / 2.5f);
        glColor3d(0, 0, 0);
        drawRoundedRectOutline(x - 5 * scale, y - 10 * scale, 13 * scale, 2 * scale, unit / 2.5f);

        glPopMatrix();

        glColor3d(0, 0, 0);
        ellipse(x - 11 * scale, y - 10 * scale, 2 * scale, 2 * scale, unit); //arm hole
        glColor3d(1, 1, 1);
        ellipse(x - 11 * scale, y - 10 * scale, 1.5 * scale, 1.5 * scale, unit); //arm hole


        glColor3d(0, 0, 0);
        glPushMatrix();

        glTranslatef(arms_x, arms_y, 0);
        glRotatef(270, 0, 0, 1);
        glTranslatef(-arms_x, -arms_y, 0);

        glPushMatrix();
        glTranslated(0, arm_extension * 12 * scale, 0);

        /*Claw*/
        glColor3d(0.0f, 0.0f, 0.0f);
        semi_ellipse(arms_x, arms_y + 2.5 * scale, 2 * scale, 2 * scale, 10, scale * 1.5);
        glColor3d(0.9f, 0.9f, 0.9f);
        semi_ellipse(arms_x, arms_y + 2 * scale, 2 * scale, 2 * scale, 10, scale);
        glColor3d(0.0f, 0.0f, 0.0f);
        semi_ellipse(arms_x, arms_y + 2 * scale, 2 * scale, 2 * scale, 10, scale / 5);

        glPopMatrix();
        glPopMatrix();
    }
    glPopMatrix();
}

void drawBot10(float x = 500.0f, float y = 500.0f, float size = 1, int face = 1) {
    float unit = 25;
    float scale = size * unit; //25
    float height = 14 * size * 25; //200
    float width = 8 * size * 25; //275
    /*neck?*/
    glColor3d(0.8f, 0.8f, 0.8f);
    drawRoundedRect(x, y - scale * 5, width / 2, height / 6, unit / 2.5f);
    /*head*/
    glColor3ub(53, 62, 67);
    drawRoundedRect(x, y + 10 * scale, 9 * scale, 15 * scale, 25.0f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y + 10 * scale, 9 * scale, 15 * scale, 25.0f);
    if (face == 1) {//neutral face

        /*face*/
        glColor3d(0, 0, 0);
        drawRoundedRect(x, y + 10 * scale, width, height, 25.0f);
        /*eyes*/
        glColor3d(0.5, 0, 0);
        ellipse(x + 2 * scale, y + 10 * scale, scale, 7.0f / 5.0f * scale, 2 * scale);
        ellipse(x - 2 * scale, y + 10 * scale, scale, 7.0f / 5.0f * scale, 2 * scale);
       
    }
    else if (face == 2) {//scanning face
        glColor3d(0.2, 0, 0.1);
        drawRoundedRect(x, y + 10 * scale, width, height, 25.0f);
        glColor3d(0.7, 0.1, 0);
        glPushMatrix();
        glTranslated(scanbar_2, 0, 0);
        glLineWidth(5.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + 4 * scale, y + 17 * scale);
        glVertex2f(x + 4 * scale, y + 4 * scale);
        glEnd();
        glPopMatrix();
    }
    
    /*body*/
    glColor3ub(53, 62, 67);
    drawRoundedRect(x, y - 8 * scale, 15 * scale, 25 * scale, scale * 5);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y - 8 * scale, 15 * scale, 25 * scale, scale * 5);
    /*arms*/

    /*wheels*/
    //stage right wheel
    glColor3d(0, 0, 0);
    drawRoundedRect(x + 8 * scale, y - 20 * scale, 3 * scale, 5 * scale, unit / 2.5f);
    glColor3d(120, 150, 180);
    glLineWidth(scale / 10);
    int i = 0;
    while (i < 3 * scale) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(x + 9.5 * scale, y - 18 * scale - i);
        glVertex2d(x + 6.5 * scale, y - 18 * scale - i);
        i += scale;
        glEnd();
    }
    //stage left wheel
    glColor3d(0, 0, 0);
    drawRoundedRect(x - 8 * scale, y - 20 * scale, 3 * scale, 5 * scale, unit / 2.5f);
    glColor3d(200, 150, 180);
    i = 0;
    while (i < 3 * scale) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(x - 9.5 * scale, y - 18 * scale - i);
        glVertex2d(x - 6.5 * scale, y - 18 * scale - i);
        i += scale;
        glEnd();
    }
}

void drawBot10SideView(float x = 400.0f, float y = 500.0f, float size = 0.5, int face = 1) {
    float unit = 25;
    float scale = size * unit; //25
    float height = 14 * size * 25; //200
    float width = 8 * size * 25; //275

    /*wheel behind body*/
    glColor3d(0.25f, 0.25f, 0.25f);
    drawRoundedRect(x + 5 * scale, y - 19 * scale, 7.5 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + 5 * scale, y - 19 * scale, 7.5 * scale, 5 * scale, 2 * scale);
    glColor3d(0.2f, 0.2f, 0.2f); // Medium grey (RGB: 128, 128, 128)
    drawRoundedRect(x + 2 * scale, y - 19 * scale, 10 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + 2 * scale, y - 19 * scale, 10 * scale, 5 * scale, 2 * scale);
    
    float head_x = x;
    float head_y = y;
    glPushMatrix();
    glTranslatef(head_x, head_y, 0);
    glRotatef(head_angle, 0, 0, 1);
    glTranslatef(-head_x, -head_y, 0);

    /*head behind*/
    glColor3ub(53, 62, 67);
    drawRoundedRect(x - 4 * scale, y + 10 * scale, 9 * scale, 15 * scale, 25.0f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 4 * scale, y + 10 * scale, 9 * scale, 15 * scale, 25.0f);

    /*head*/
    glColor3ub(53, 62, 67);
    drawRoundedRect(x, y + 10 * scale, 9 * scale, 15 * scale, 25.0f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y + 10 * scale, 9 * scale, 15 * scale, 25.0f);
    if (face == 1) {//neutral face

        /*face*/
        glColor3d(0, 0, 0);
        drawRoundedRect(x, y + 10 * scale, width, height, 25.0f);
        /*eyes*/
        glColor3d(0.5, 0, 0);
        ellipse(x + 2 * scale, y + 10 * scale, scale, 7.0f / 5.0f * scale, 2 * scale);
        ellipse(x - 2 * scale, y + 10 * scale, scale, 7.0f / 5.0f * scale, 2 * scale);

    }
    else if (face == 2) {//scanning face
        glColor3d(0.2, 0, 0.1);
        drawRoundedRect(x, y + 10 * scale, width, height, 25.0f);
        glColor3d(0.7, 0.1, 0);
        glPushMatrix();
        glTranslated(scanbar_2, 0, 0);
        glLineWidth(5.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + 4 * scale, y + 17 * scale);
        glVertex2f(x + 4 * scale, y + 4 * scale);
        glEnd();
        glPopMatrix();
    }
    else if (face == 3) {//Waste detected
        glColor3d(0.8, 0, 0.2);
        drawRoundedRect(x, y + 10 * scale, width, height, 25.0f);
    }
    else if (face == 4) {
        glColor3d(0.8, 0, 0.2);
        drawRoundedRect(x, y + 10 * scale, width, height, 25.0f);
        drawSeed(x, y + 10 * scale, 0.2);
    }
    glPopMatrix();
    /*body*/
    glColor3ub(53, 62, 67);
    drawRoundedRect(x - 4 * scale, y - 8 * scale, 15 * scale, 25 * scale, scale * 5);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 4 * scale, y - 8 * scale, 15 * scale, 25 * scale, scale * 5);

    /*body*/
    glColor3ub(53, 62, 67);
    drawRoundedRect(x, y - 8 * scale, 15 * scale, 25 * scale, scale * 5);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y - 8 * scale, 15 * scale, 25 * scale, scale * 5);

    /*wheels*/
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x - 6 * scale, y - 20 * scale, 7.5 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 6 * scale, y - 20 * scale, 7.5 * scale, 5 * scale, 2 * scale);
    glColor3d(0.5f, 0.5f, 0.5f); // Medium grey (RGB: 128, 128, 128)
    drawRoundedRect(x - 10 * scale, y - 20 * scale, 10 * scale, 5 * scale, 2 * scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 10 * scale, y - 20 * scale, 10 * scale, 5 * scale, 2 * scale);
    glColor3d(0.9f, 0.9f, 0.9f);
    ellipse(x - 8 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    ellipse(x - 12 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    /*rotary display*/
    glPushMatrix();
    glTranslatef(x - 9 * scale, y - 20 * scale, 0);
    glRotatef(wheel_rotation, 0, 0, 1);
    glTranslatef(-x + 9 * scale, -y + 20 * scale, 0);
    glColor3d(255, 255, 255);
    ellipse(x - 9 * scale, y - 20 * scale, 0.4 * scale, 0.4 * scale, unit);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x - 13 * scale, y - 20 * scale, 0);
    glRotatef(wheel_rotation, 0, 0, 1);
    glTranslatef(-x + 13 * scale, -y + 20 * scale, 0);
    ellipse(x - 13 * scale, y - 20 * scale, 0.4 * scale, 0.4 * scale, unit);
    glPopMatrix();

    /*arms*/
    float arms_x = x - 11 * scale;
    claw_x = arms_x + 2 * scale;
    float arms_y = y - 10 * scale;
    claw_y = arms_y;
    if (draw_hands2) {
        glPushMatrix();
        glTranslatef(arms_x, arms_y, 0);
        glRotatef(90, 0, 0, 1); //rotates arm
        glRotatef(arm_angle, 0, 0, 1); //rotates arm
        glTranslatef(-arms_x, -arms_y, 0);

        glPushMatrix();
        glTranslatef(arms_x, arms_y, 0);
        glScalef(arm_extension, 1.0f, 1.0f); // Stretch object horizontally
        glTranslatef(-arms_x, -arms_y, 0);


        glColor3d(1, 1, 1);
        drawRoundedRect(x - 5 * scale, y - 10 * scale, 13 * scale, 2 * scale, unit / 2.5f);
        glColor3d(0, 0, 0);
        drawRoundedRectOutline(x - 5 * scale, y - 10 * scale, 13 * scale, 2 * scale, unit / 2.5f);

        glPopMatrix();

        glColor3d(0, 0, 0);
        ellipse(x - 11 * scale, y - 10 * scale, 2 * scale, 2 * scale, unit); //arm hole
        glColor3d(1, 1, 1);
        ellipse(x - 11 * scale, y - 10 * scale, 1.5 * scale, 1.5 * scale, unit); //arm hole


        glColor3d(0, 0, 0);
        glPushMatrix();

        glTranslatef(arms_x, arms_y, 0);
        glRotatef(270, 0, 0, 1);
        glTranslatef(-arms_x, -arms_y, 0);

        glPushMatrix();
        glTranslated(0, arm_extension * 12 * scale, 0);

        /*Claw*/
        glColor3d(0.0f, 0.0f, 0.0f);
        semi_ellipse(arms_x, arms_y + 2.5 * scale, 2 * scale, 2 * scale, 10, scale * 1.5);
        glColor3d(0.9f, 0.9f, 0.9f);
        semi_ellipse(arms_x, arms_y + 2 * scale, 2 * scale, 2 * scale, 10, scale);
        glColor3d(0.0f, 0.0f, 0.0f);
        semi_ellipse(arms_x, arms_y + 2 * scale, 2 * scale, 2 * scale, 10, scale / 5);

        glPopMatrix();
        glPopMatrix();
    }
    glPopMatrix();
}

/*trash*/
void drawTrash(float x,float y, float size) {
    float scale = 25.0f * size;
    glColor3d(0, 0, 0);
    ellipse(x, y, 9 * scale, 6 * scale, 50);
    glColor3ub(136, 140, 141);
    ellipse(x, y, 8 * scale, 5 * scale, 50);
}

void drawSeed(float x, float y, float scale) {
    float unit = 25;
    float size = scale * unit;
    glPushMatrix();
    glTranslated(seed_x, seed_y, 0);
        glColor3ub(0, 0, 0);
        ellipse(x, y, size * 1.7, size * 1.7, 25);
        ellipse(x + 2 * size, y, size * 1.7, size * 1.7, 25);
        ellipse(x + size, y - size, size * 2.2, size * 1.2, 25);
        glColor3ub(150, 75, 0);
        ellipse(x, y, size * 1.5, size * 1.5, 25);
        ellipse(x + 2 * size, y, size * 1.5, size * 1.5, 25);
        ellipse(x + size, y - size, size * 2, size, 25);
    glPopMatrix();
}

/*claw as a separate object*/
void drawClaw(float x, float y) {
    glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(30, 0, 0, 1);
    glTranslated(-x, -y, 0);
    glColor3d(1, 1, 1);
    drawRoundedRect(x - 300, y, 450, 50, 5);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 300, y, 450, 50, 5);
    glPopMatrix();

    glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(-60, 0, 0, 1);
    glTranslated(-x, -y, 0);
    glColor3d(0.0f, 0.0f, 0.0f);
    semi_ellipse(x, y, 70, 70, 50, 50);
    glColor3d(0.9f, 0.9f, 0.9f);
    semi_ellipse(x, y, 70, 70, 50, 10);
    glColor3d(0.0f, 0.0f, 0.0f);
    //semi_ellipse(x, y, 100, 100, 50,100);
    glPopMatrix();

}

/* Watering can */
void drawWateringCan(float x = 325, float y = 525) {
    glColor3d(0.6, 0.6, 0.6);
    trapezium(x, y, 50, 50);
    glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(90, 0, 0, 1);
    glTranslated(-x, -y, 0);
    semi_ellipse(x + 20, y - 40, 10, 30, 40, 5.0);
    glPopMatrix();
    glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(-40, 0, 0, 1);
    glTranslated(-x, -y, 0);
    drawRoundedRect(x - 25, y + 15, 40, 10, 1);
    glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	//drawn items here
    if (scene == 0) {
        glPushMatrix();
        glTranslated(scene0_leftpan, 0, 0);
        drawScene0();
        glPopMatrix();
        if (!rock_behind_bot) {
            glPushMatrix();
            glTranslatef(400, 300, 0);
            glTranslatef(-400, -300, 0);
            glTranslatef(bot9_x, bot9_y, 0);
            drawBot9SideView(300, 600, 0.7, bot9_face);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(trash_x, trash_y, 0);
            drawTrash(650, 200, 0.25f);
            glPopMatrix();
        }
        else {
            glPushMatrix();
            glTranslatef(trash_x, trash_y, 0);
            if (draw_rock)
                drawTrash(650, 200, 0.25f);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(400, 300, 0);
            glTranslatef(-400, -300, 0);
            glTranslatef(bot9_x, bot9_y, 0);
            drawBot9SideView(300, 600, 0.7, bot9_face);
            glPopMatrix();
        }
    }
    else if (scene == 1) {
        drawScene1();
        glPushMatrix();
        glTranslatef(400, 300, 0);
        glTranslatef(-400, -300, 0);
            glTranslatef(bot9_x, bot9_y, 0);
            drawBot9SideView(400, 500, 0.3, 1);
        glPopMatrix();
        drawSeed(600, 300, 0.25f);
    }
    else if (scene == 2) {
        drawScene2();
        drawBot9(SCREEN_WIDTH / 2, 500, 2, bot9_face);
        drawClaw(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
        drawSeed(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3, 1.0);
    }
    else if (scene == 3) {
        drawScene1();
        glPushMatrix();
        glTranslatef(400, 300, 0);
        glTranslatef(-400, -300, 0);
        glTranslatef(bot9_x, bot9_y, 0);
        drawBot9SideView(400, 500, 0.3, bot9_face);
        glPopMatrix();
        drawSeed(600 + seed_x, 300 + seed_y, 0.25f);
    }
    else if (scene == 4) {
        drawScene4();
        
        if (door_state == 1){
            glColor3d(0, 0, 0);
            door(SCREEN_WIDTH / 2, 500, 150, 1);
        }
        if (flag == true && animation_state == 8) {
            glPushMatrix();
            glTranslated(700, 600, 0);
            glScaled(bot9_scale, bot9_scale, 0);
            glTranslated(-700, -600, 0);
            drawBot9(725, 650, 0.5, 1);
            glPopMatrix();
        }
        else if (flag == true) {
            glPushMatrix();
            glTranslated(700, 600, 0);
            glScaled(bot9_scale, bot9_scale, 0);
            glTranslated(-700, -600, 0);
            glTranslated(bot9_x, 0, 0);
            glTranslated(700, 600, 0);
            glScaled(bot9_direction, 1, 1);
            glTranslated(-700, -600, 0);
            drawBot9SideView(750, 650, 0.5, bot9_face);
            glPopMatrix();
            glPushMatrix();
            glTranslated(seed_x, seed_y, 0);
            drawSeed(625, 525, 0.3);
            glPopMatrix();
        }
        glPushMatrix();
        glTranslated(325, 525, 0);
        glScaled(can_scale, can_scale, 1);
        glTranslated(-325, -525, 0);
        glTranslated(can_x, can_y, 0);
        glTranslated(425, 525, 0);
        glScaled(can_direction, 1, 1);
        glTranslated(-425, -525, 0);
        glTranslated(325, 525, 0);
        glRotated(can_angle, 0, 0, 1);
        glTranslated(-325, -525, 0);
        drawWateringCan();
        glPopMatrix();
    }
    else if (scene == 5) {
        drawScene5();
        if (door_state == 1 && pause < 100) {
            glColor3f(0, 0, 0);
            rectangle(685, 540, 210, 200);
            circle(50, 785, 750, 100);
        }
        glPushMatrix();
        glTranslatef(bot9_x, bot9_y, 0);
        if (pause <= 100 && door_state == 1) {
            drawBot9(780, 750, 0.4, bot9_face);
        }
        if (door_state == 0 && pause >= 100) {
            drawBot9SideView(780, 750, 0.4, bot9_face);
        }
        glPopMatrix();
        glPushMatrix();
        glTranslatef(bot10_x, bot10_y, 0);
        glTranslatef(500, 1000, 0);
        glScalef(bot10_direction, 1, 1);
        glTranslatef(-500, -1000, 0);
        if (bot9_x >= 600 && bot10_direction == -1) {
            drawBot10SideView(450, 1050, 0.4, bot10_face);
            //wall
            glColor3ub(70, 70, 70);
            rectangle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2 + 0.1 * SCREEN_HEIGHT, SCREEN_HEIGHT, SCREEN_WIDTH);
            glColor3ub(0, 0, 0);
            
            //line
            glColor3ub(20, 20, 20);
            rectangle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2 + 0.1 * SCREEN_HEIGHT, SCREEN_HEIGHT, 4);
            //door
            door();
            //window
            window2(3 * SCREEN_WIDTH / 4, 4 * SCREEN_HEIGHT / 5, 60, 400);
        }
        else if (bot10_direction == 1){
            drawBot10SideView(450, 1050, 0.4, bot10_face);
        }
        glPopMatrix();
    }
    else if (scene == 6) {
        drawScene4();
        glPushMatrix();
        glTranslated(325, 525, 0);
        glScaled(can_scale, can_scale, 1);
        glTranslated(-325, -525, 0);
        glTranslated(can_x, can_y, 0);
        glTranslated(425, 525, 0);
        glScaled(can_direction, 1, 1);
        glTranslated(-425, -525, 0);
        glTranslated(325, 525, 0);
        glRotated(can_angle, 0, 0, 1);
        glTranslated(-325, -525, 0);
        drawWateringCan();
        glPopMatrix();
        drawSeed(1000, 425, 0.25f);
        if (door_state == 1 && animation_state == 18) {
            glColor3d(0, 0, 0);
            door(SCREEN_WIDTH / 2, 500, 150, 1);
            glPushMatrix();
            glTranslatef(bot10_x, bot10_y, 0);
            drawBot10(725, 600, bot10_scale, 1);
            glPopMatrix();
        }
        else if (animation_state == 19 || animation_state == 20){
            glPushMatrix();
            glTranslatef(bot10_x, bot10_y, 0);
            glTranslatef(725, 600, 0);
            glScaled(bot10_direction, 1, 1);
            glTranslatef(-725, -600, 0);
            drawBot10SideView(725, 600, bot10_scale, bot10_face);
            glPopMatrix();
        }
        if (animation_state == 21) {
            if (door_state == 1) {
                glColor3d(0, 0, 0);
                door(SCREEN_WIDTH / 2, 500, 150, 1);
                glPushMatrix();
                glTranslatef(bot9_x, bot9_y, 0);
                glTranslatef(710, 650, 0);
                glScaled(bot9_direction, 1, 1);
                glTranslatef(-710, -650, 0);
                drawBot9(710, 650, bot9_scale, 1);
                glPopMatrix();
                glPushMatrix();
                glTranslatef(bot10_x, bot10_y, 0);
                glTranslatef(725, 600, 0);
                glScaled(bot10_direction, 1, 1);
                glTranslatef(-725, -600, 0);
                drawBot10SideView(725, 600, bot10_scale, bot10_face);
                glPopMatrix();
            }
            else if (door_state == 0) {
                glPushMatrix();
                glTranslatef(bot9_x, bot9_y, 0);
                glTranslatef(710, 650, 0);
                glScaled(bot9_direction, 1, 1);
                glTranslatef(-710, -650, 0);
                drawBot9SideView(710, 650, bot9_scale, bot9_face);
                glPopMatrix();
                glPushMatrix();
                glTranslatef(bot10_x, bot10_y, 0);
                glTranslatef(725, 600, 0);
                glScaled(bot10_direction, 1, 1);
                glTranslatef(-725, -600, 0);
                drawBot10SideView(725, 600, bot10_scale, bot10_face);
                glPopMatrix();
            }
            
        }
    }
    else if (scene == 7) {
        drawScene7();
        drawBot9(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1.0, bot9_face);
    }
    else if (scene == 8) {
        drawScene4();
        
        drawSeed(1000, 425, 0.25f);
        glPushMatrix();
        glTranslatef(bot9_x, bot9_y, 0);
        glTranslatef(710, 650, 0);
        glScaled(bot9_direction, 1, 1);
        glTranslatef(-710, -650, 0);
        drawBot9SideView(710, 650, bot9_scale, bot9_face);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(bot10_x, bot10_y, 0);
        glTranslatef(725, 600, 0);
        glScaled(bot10_direction, 1, 1);
        glTranslatef(-725, -600, 0);
        drawBot10SideView(725, 600, bot10_scale, bot10_face);
        glPopMatrix();
        glPushMatrix();
        glTranslated(325, 525, 0);
        glScaled(can_scale, can_scale, 1);
        glTranslated(-325, -525, 0);
        glTranslated(can_x, can_y, 0);
        glTranslated(425, 525, 0);
        glScaled(can_direction, 1, 1);
        glTranslated(-425, -525, 0);
        glTranslated(325, 525, 0);
        glRotated(can_angle, 0, 0, 1);
        glTranslated(-325, -525, 0);
        drawWateringCan();
        glPopMatrix();
    }
    else if (scene == 9) {//blank
        glColor3ub(0, 0, 0);
        glBegin(GL_POLYGON);
        glVertex2d(0.0f, 0.0f);
        glVertex2d(0.0f, SCREEN_HEIGHT);
        glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
        glVertex2d(SCREEN_WIDTH, 0.0f);
        glEnd();
    }
    else if (scene == 10) {
        drawScene10();
        glPushMatrix();
        glTranslated(1000, 400, 0);
        glScaled(-1, 1, 1);
        glTranslated(-1000, -400, 0);
        drawBot9SideView(1000, 400, 0.3, 1);
        glPopMatrix();
        drawBot10SideView(600, 400, 0.3, 1);
    }
    glutSwapBuffers();
	glFlush();
}


void anim(int value) {
    glutPostRedisplay(); // Tell GLUT to redraw
    if (animation_state == 0) {
        if (bot9_x < 100) {//bot9 moves across the corridor
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else
                wheel_rotation = 0;
            bot9_x += 2.0f * speed;
        }
        else if (head_angle > -20 && pause <= 30/speed) {//bot 9 looks at trash
            head_angle -= 1.0f * speed;
            pause = 0;
        }
        else if (head_angle <= -20 && pause <= 60/speed) {//scans it
            bot9_face = 2;
            if (scanbar < 130 && !bar_flip) {
                scanbar += 3.0 * speed;
            }
            else if (scanbar >= 130 && !bar_flip) {
                bar_flip = true;
            }
            else if (scanbar > 0 && bar_flip) {
                scanbar -= 3.0 * speed;
            }
            else {
                bar_flip = false;
            }
            pause++;
            arm_angle = -90;
        }
        else if (arm_extension < 2.0) {//arm reaches towards trash
            draw_hands = true;
            bot9_face = 1;

            if (arm_angle > -120) {
                arm_angle -= 2 * speed;
            }
            if (arm_extension < 2.0) {
                arm_extension += 0.1 * speed;
            }
        }
        else {
            pause = 0;

            animation_state = 1;
        }
    }

    if (animation_state == 1) {
        if (pause < 10 / speed) {// trash goes slightly in
            pause++;
            trash_x -= 1.0 * speed;
            trash_y += 1.0 * speed;
        }
        else if (flag) { // puts trash into bin
            if (arm_angle < 10)
                arm_angle += 5 * speed;
            if (trash_y < 450)
                trash_y += 30 * speed;
            if (trash_x > -450)
                trash_x -= 20 * speed;
            if (arm_extension > 1.0f) {
                arm_extension -= 0.1 * speed;
            }
            if (arm_angle >= 10 && trash_y >= 450 && arm_extension >= 0.9f) {
                flag = false;
            }
        }
        else if (arm_extension > 0.0f) {
            arm_extension -= 0.1 * speed;
        }
        else {
            draw_hands = false;
            rock_behind_bot = true;
            if (trash_y > 50)
                trash_y -= 20 * speed;
            if (head_angle < 0) {
                head_angle += 1.0 * speed;
            }
            else if (pause < 30/speed) {
                pause++;
            }
            else {
                pause = 0;
                animation_state = 2;
                arm_angle = -90;
                draw_rock = false;
                trash_x = 800;
                trash_y = 0;
            }
        }

    }

    if (animation_state == 2) {
        if (scene0_leftpan >= -600) {
            rock_behind_bot = false;
            draw_rock = true;
        }
        if (cur_leftpan >= -400 && scene0_leftpan > -1200) {
            if (trash_x > 0) {
                trash_x -= 40 * speed;
            }
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else
                wheel_rotation = 0;
            if (cur_leftpan > -400) {
                cur_leftpan -= 20 * speed;
                scene0_leftpan -= 20 * speed;
            }
            else if (scene0_leftpan >= -800) {
                animation_state = 0;
                arm_angle = -90;
                flag = true;
                cur_leftpan = 0;
                speed = 2;
            }
            else {
                pause = 0;
                arm_angle = -90;
                flag = true;
                trash_x = 0;
                trash_y = 0;
            }
        }
        else if (bot9_x <= 900) {
            bot9_x += 20;
        }
        else {
            pause = 0;
            bot9_x = 0;
            animation_state = 3;
            arm_angle = -90;
            scene = 1;
        }
    }

    if (animation_state == 3) {
        if (bot9_x < 100) {//used to be 100
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else
                wheel_rotation = 0;
            bot9_x += 1.0f;
        }
        else
            animation_state = 4;
    }
    else if (animation_state == 4) {//bot9 tilts head to look at the seed
        if (head_angle > -30 && !head_flip) {
            head_angle--;
            arm_angle = -90;
        }
        else if (head_angle == -30) {
            draw_hands = true;
            if (arm_angle > -123) {
                arm_angle -= 2;
            }
            if (arm_extension < 2.2f) {
                arm_extension += 0.1;
            }
            if (arm_angle <= -123 && arm_extension >= 2.0f) { // need to use greater than or equal for floats (binary math issues)
                if (pause < 25)
                    pause++;
                else
                    animation_state = 5;
            }
        }
    }
    else if (animation_state == 5) {//bot9 picks up the seed
        if (seed_x > -90) {
            pause = 0;
            if (arm_extension > 1.0)
                arm_extension -= 0.05;
            if (seed_x > -90)
                seed_x -= 4;
            if (seed_y < 60)
                seed_y += 60.0f / (90.0f / 4.0f);
        }
        else if (arm_angle < -60) {
            if (pause < 50)
                pause++;
            else {
                arm_angle += 2;
                seed_y += 4;
            }
        }
        else {
            if (pause < 100)
                pause++;
            else {
                pause = 0;
                seed_x = 0;
                seed_y = 0;
                scene = 2;
                animation_state = 6;
            }
        }
    }
    else if (animation_state == 6) {//bot9 scans the seed
        if (pause < 30)
            pause++;
        else if (pause < 200) {
            bot9_face = 2;
            if (scanbar < 400 && !bar_flip) {
                scanbar += 10;
            }
            else if (scanbar >= 400 && !bar_flip) {
                bar_flip = true;
            }
            else if (scanbar > 0 && bar_flip) {
                scanbar -= 10;
            }
            else {
                bar_flip = false;
            }
            pause++;
        }
        else if (pause < 300) {
            bot9_face = 3;
            pause++;
        }
        else if (pause < 340) {
            bot9_face = 1;
            pause++;
        }
        else {
            seed_x = -45;
            seed_y = 90;
            scene = 3;
            animation_state = 7;
            pause = 0;
        }
    }
    else if (animation_state == 7) {//after scanning, bot9 holds the seed in hand and moves to the next scene
        if (pause < 50) {
            pause++;
        }
        else if (head_angle < 0) {
            head_angle++;
        }
        else if (bot9_y >= -600) {
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else
                wheel_rotation = 0;
            bot9_y -= 4;
            seed_y -= 2;
        }
        else {
            flag = false;
            bot9_y = 0;
            bot9_x = 0;
            seed_y = 0;
            seed_x = 0;
            pause = 0;
            animation_state = 8;
            scene = 4;
        }
    }
    else if (animation_state == 8) {
        if (pause < 50) {//door initially closed
            door_state = 0;
            pause++;
        }
        else if (pause < 75) {//door opens up
            door_state = 1;
            pause++;
        }
        else if (pause < 100) {//bot9 walks in
            flag = true;
            pause++;
        }
        else if (bot9_scale < 1) {
            bot9_scale += 0.01;
            door_state = 0;
        }
        else {
            if (pause < 120) {
                pause++;
            }
            else {
                animation_state = 9;
                pause = 0;
                draw_hands = true;
                arm_angle = -90;
                arm_extension = 0;
            }
        }
    }
    else if (animation_state == 9) {//seed in hand, reaches out towards the pot
        if (arm_extension < 2) {
            arm_extension += 0.05;
            seed_x += 4;
        }
        if (bot9_x < 50) {
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else
                wheel_rotation = 0;
            bot9_x++;
            seed_x += 0.5;
        }
        if (arm_extension >= 2 && bot9_x >= 50) {
            animation_state = 10;
        }
    }
    else if (animation_state == 10) {//drops and plants the seed into a pot
        if (arm_extension > 0) {
            arm_extension -= 0.05;
        }
        if (seed_y >= -50) {
            seed_y -= 2;
        }
        if (seed_y <= -50 && arm_extension <= 0) {
            animation_state = 11;
        }
    }
    else if (animation_state == 11) {
        bot9_direction = -1;
        if (arm_angle < -88) {
            arm_angle++;
        }
        if (arm_extension < 2.8) {
            arm_extension += 0.1;
        }
        if (arm_angle >= -88 && arm_extension >= 2.8)
            animation_state = 12;
    }
    else if (animation_state == 12) {
        if (arm_extension > 1.0) {
            arm_extension -= 0.1;
            can_scale += 0.05;
            can_x += 6;
            can_y -= 1;
        }
        else {
            can_direction = -1;
            bot9_direction = 1;
            animation_state = 13;
            pause = 0;
        }
    }
    if (animation_state == 13) {
        if (arm_angle > -110) {
            arm_angle--;
            can_angle++;
            can_y -= 2.5;
        }
        else {
            if (pause < 25)
                pause++;
            else
                animation_state = 14;
        }
    }
    else if (animation_state == 14) {
        if (arm_angle < -90) {
            arm_angle++;
            can_angle--;
            can_y += 2.5;
        }
        else {
            animation_state = 15;
            can_direction = 1;
            bot9_direction = -1;
        }
    }
    else if (animation_state == 15) {
        if (arm_extension < 2.8) {
            arm_extension += 0.1;
            can_scale -= 0.05;
            can_x -= 6;
            can_y += 1;
        }
        else {
            scanbar = 0;
            bar_flip = false;
            pause = 0;
            animation_state = 16;
        }
    }
    else if (animation_state == 16) {
        if (arm_extension > 0.0)
            arm_extension -= 0.1;
        else if (pause < 50) {
            bot9_direction = 1;
            bot9_face = 2;
            if (scanbar < 75 && !bar_flip) {
                scanbar += 3;
            }
            else if (scanbar >= 75 && !bar_flip) {
                bar_flip = true;
            }
            else if (scanbar > 0 && bar_flip) {
                scanbar -= 3;
            }
            else {
                bar_flip = false;
            }
            pause++;
        }
        else {
            animation_state = 17;
            bot9_x = 0;
            bot9_y = 0;
            bot9_face = 1;
            pause = 0;
            scene = 5;
        }
    }
    else if (animation_state == 17) {
        if (pause < 75) {
            door_state = 0;
            pause++;
        }
        else if (pause < 100) {
            door_state = 1;
            pause++;
        }
        else if (bot9_y > -200) {
            bot9_y -= 4;
        }
        else if (bot9_x < 800) {
            door_state = 0;
            bot9_x += 4;
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else {
                wheel_rotation = 0;
            }
        }
        else if (bot10_y > -400){
            bot10_y -= 2.0;
            bot10_x -= 2.0;
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else {
                wheel_rotation = 0;
            }
        }
        else if (bot10_direction != 1){
            bot10_direction = 1;
            bot10_x += 100;
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else {
                wheel_rotation = 0;
            }
        }
        else if (bot10_x < 200){
            bot10_x += 2.0;
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else {
                wheel_rotation = 0;
            }
        }
        else if (pause < 150) {
            pause++;
        }
        else {
            scene = 6;
            animation_state = 18;
            pause = 0;
            bot10_x = 0;
            bot10_y = 0;
            bot10_direction = -1;
            seed_x = 0;
            seed_y = 0;
            bot9_x = 0;
            bot9_y = 0;
            door_state = 0;
            //Reset all
        }
        if (face_time < 100) {
            face_time++;
        }
        else if (face_time == 100) {
            face_time = 0;
            if (bot10_face == 1)
                bot10_face = 2;
            else
                bot10_face = 1;
        }
        //bot 10 scan
        if (scanbar_2 > -75 && !bar_flip) {
            scanbar_2 -= 2.0;
        }
        else if (scanbar_2 <= -75 && !bar_flip) {
            bar_flip = true;
        }
        else if (scanbar_2 < 0 && bar_flip) {
            scanbar_2 += 2.0;
        }
        else {
            bar_flip = false;
        }

    }
    else if (animation_state == 18) {
        if (pause < 25) {
            pause++;
        }
        else if (door_state != 1) {
            door_state = 1;
        }
        else if (pause < 50) {
            pause++;
        }
        else {
            animation_state = 19;
            face_time = 0;
        }
    }
    else if (animation_state == 19) {
        if (face_time < 100) {
            face_time++;
        }
        else if (face_time >= 100) {
            face_time = 0;
            if (bot10_face == 1)
                bot10_face = 2;
            else
                bot10_face = 1;
        }
        if (scanbar_2 > -50 && !bar_flip) {
            scanbar_2 -= 0.5;
        }
        else if (scanbar_2 <= -50 && !bar_flip) {
            bar_flip = true;
        }
        else if (scanbar_2 < 0 && bar_flip) {
            scanbar_2 += 0.5;
        }
        else {
            bar_flip = false;
        }
        if (bot10_x > -250 && bot10_direction == -1) {
            bot10_y -= 0.1;
            bot10_x -= 2.0;
        }
        else if (pause < 150) {
            pause++;
        }
        else if (bot10_direction != 1) {
            bot10_direction = 1;
        }
        else if (bot10_scale < 0.3) {
            bot10_scale += 0.001;
            if (bot10_x < 800) {
                bot10_x += 2.0;
                bot10_y -= 1;
            }
        }
        else if (pause < 200) {
            pause++;
        }
        else{
            animation_state = 20;
            bot9_scale = 0.3;
            pause = 0;
            arm_angle = -90;
            door_state = 1;
        }
    }
    else if (animation_state == 20) {
        if (bot10_face != 3) {
            bot10_face = 3;
            draw_hands2 = true;
        }
        else if (arm_extension < 0.75){//bot 10 extends its arm, try to get rid of the seed
            arm_extension += 0.01;
        }
        else {
            door_state = 1;
            animation_state = 21;
            pause = 0;
        }
    }
    else if (animation_state == 21) {
        if (arm_extension < 1.50) {
            arm_extension += 0.01;
        }
        if(pause < 30) {
            door_state = 1;
            pause++;
        }
        else if (bot9_y > -150) {
            door_state = 0;
            bot9_y -= 12;
            bot9_x += 10;
        }
        else if (bot9_direction != -1){
            bot9_direction = -1;
            bot9_face = 3;
            bot10_face = 1;
        }
        else if (pause < 100) {
            pause++;
        }
        else if (bot10_face != 4) {
            bot10_face = 4;
            bot9_face = 1;
        }
        else if (pause < 150) {
            pause++;
        }
        else {
            draw_hands = false;
            animation_state = 22;
            scene = 7;
            pause = 0;
        }
    }
    else if (animation_state == 22) {
        if (pause < 50) {
            pause++;
        }
        else if (pause < 100) {
            bot9_face = 4;
            pause++;
        }
        else if (pause < 150) {
            plant_scale += 0.04;
            seed_y2 -= 1;
            pause++;
        }
        else if (pause < 200) {
            pause++;
        }
        else {
            bot9_face = 3;
            bot10_face = 1;
            scene = 8;
            animation_state = 23;
            pause = 0;
        }
    }
    else if (animation_state == 23) {
        if (arm_extension > 0 && pause == 0) {
            arm_extension -= 0.1;
        }
        else if (pause < 50) {
            pause++;
        }
        else if (bot10_direction != -1 && pause >= 50) {
            bot10_direction = -1;
        }
        else if (arm_extension < 4.0) {
            if (arm_angle < 30) {
                arm_angle += 1;
            }
            arm_extension += 0.2;
        }
        else {
            animation_state = 24;
            pause = 0;
        }
    }
    else if (animation_state == 24) {
        if (arm_extension > 1.0) {
            can_x += 18;
            can_y -= 6;
            arm_extension -= 0.2;
            if (arm_angle < -90) {
                arm_angle -= 2;
            }

        }
        else if (bot10_direction != 1) {
            bot10_direction = 1;
            can_direction = -1;
            bot9_face = 1;
            can_x -= 75;
        }
        else if (bot9_y < -50) {
            bot9_y += 2;
            if (bot10_x < 350) {
                bot10_x += 2;
                can_x += 2;
            }
        }
        else if (bot9_direction != 1) {
            bot9_direction = 1;
        }
        else if (pause < 50) {
            pause++;
        }
        else if (pause < 100){
            scene = 9;
            pause++;
        }
        else {
            scene = 10;
            pause = 0;
            animation_state = 25;
            draw_hands = 1;
        }
    }
    else if (animation_state == 25){
        if (pause < 500) {
            pause++;
        }
        else {
            std::exit(1);
        }
    }
   	glutTimerFunc(16, anim, 0); // Call again in 16ms (~60 FPS)
}

//void anim (int value) {
//    if (arm_extension < 1.5f) { //can only reach 1.5 or else it bugs out
//        arm_extension += 0.01;
//    }
//    else {
//        arm_extension = 0;
//    }
//    if (head_angle > -30 && head_angle <= 0) {
//        head_angle--;
//    }
//    else if (head_angle == -30) {
//        if (pause == 100) {
//            animation_state = 0;
//
//            head_angle = 330;
//            
//        }
//        else {
//            animation_state = 1;
//
//            pause++;
//        }
//    }
//    else if (head_angle < 360){
//        head_angle++;
//    }
//    else {
//        if (pause == 200) {
//            head_angle = 0;
//            pause = 0;
//        }
//        else {
//            pause++;
//        }
//    }
//        
//	if (arm_angle >= -180 && arm_angle <= 0) {
//        arm_angle--;
//	}
//	else if (arm_angle < -180){
//        arm_angle = abs(arm_angle);
//        arm_angle++;
//	}
//    else if (arm_angle < 360) {
//        arm_angle++;
//    }
//    else {
//        arm_angle = 0;
//    }
//    if (wheel_rotation > -360)
//        wheel_rotation -= 10;
//    else {
//        wheel_rotation = 0;
//    }
//    if (scanbar < 100 && !flip) {
//        scanbar += 1;
//    }
//    else if (scanbar == 100 && !flip) {
//        flip = true;
//    }
//    else if (scanbar > 0 && flip) {
//        scanbar -= 1;
//    }
//    else {
//        flip = false;
//    }
//	glutPostRedisplay(); // Tell GLUT to redraw
//
//	glutTimerFunc(16, anim, 0); // Call again in 16ms (~60 FPS)
//}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutCreateWindow("Robot Tale");
   
    glutDisplayFunc(display);
   
	glutTimerFunc(16, anim, 0);

	initGL();

	glutMainLoop();
}