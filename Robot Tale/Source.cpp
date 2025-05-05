#include <Windows.h>
#include <stdlib.h>
#include <math.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>
#include <iostream>
#include 'functions.h'

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 900
#define M_PI 3.1415926f
#define FPS 120

float bot9_x = 0;
float arm_angle = 0; //0 is facing bottom, all rotates counter clockwise
float head_angle = 0; // 0 is facing right

float wheel_rotation = 0;
float arm_extension = 0;
float claw_x = 0;
float claw_y = 0;
float scanbar = 0;

float seed_x = 0;
float seed_y = 0;

bool bar_flip = false;//for scanbar
bool head_flip = false; // for head nod
bool draw_hands = false; //showing/hiding hands

int pause = 0; // for how long the head stops moving

int scene = 0;
int animation_state = 0;

void initGL() {
	glClearColor(1.0, 1.0, 1.0, 0.0);//white background
    //glClearColor(0, 0, 0, 0.0);//black background

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
}

/*backgrounds*/
/*Scene 0*/
void drawScene0() {
    glColor3f(0.2, 0.2, 0.6);
    drawRoundedRect(500, 500, 400, 250, 5);

    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f); // Bottom-left corner
    glVertex2f(SCREEN_WIDTH, 0.0f); // Bottom-right corner
    glVertex2f(SCREEN_WIDTH, 300.0f); // Top-right corner
    glVertex2f(0.0f, 300.0f); // Top-left corner
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

void window(int x, int y, int h, int w) {
    glColor3ub(0, 0, 0);
    rectangle(x, y, h, w);

    glColor3ub(94, 94, 94);
    rectangle(x + 4, y + 4, h - 8, w - 8);
}

void door(int x, int y, int w) {
    int h = w;
    //main
    glColor3ub(100, 100, 100);
    rectangle(x, y, h, w);
    //small rectangle
    int innerW = w * 0.8;
    int innerH = h - 20;
    int innerX = x + (w - 15 * (w / 16));
    int innerY = y + (h / 16);

    //back circle
    glColor3ub(100, 100, 100);
    circle(100, x + w / 2, y + h, 75);

    glColor3ub(50, 50, 50);
    rectangle(innerX, innerY, innerH, innerW);
    //front circle
    glColor3ub(50, 50, 50);
    circle(100, x + w / 2, y + h, 60);

    glColor3ub(173, 177, 255);
    circle(100, x + w / 2, y + h, 20);

    //handle
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
    glColor3ub;(94, 53, 17);
    glBegin(GL_POLYGON);
    glVertex2i(a + 20, y - 70);
    glVertex2i(a + 100, y - 70);
    glVertex2i(a + 110, y - 10);
    glVertex2i(a + 10, y - 10);
    glEnd();
}

void drawScene2() {
    //glPointSize(10.0);
    //glLineWidth(4.0);

    glColor3ub(30, 30, 30);
    rectangle(0, 0, 500, SCREEN_WIDTH);

    pot(SCREEN_WIDTH, 400, 40, 120);

    window(3 * SCREEN_WIDTH / 4, 2 * SCREEN_HEIGHT / 3, 200, 600);
    window(50, 3 * SCREEN_HEIGHT / 4 + 15, 100, 450);

    //box
    glColor3ub(114, 50, 17);
    trapezium(SCREEN_WIDTH / 4, 520, 20, 80);
    glColor3ub(105, 40, 10);
    rectangle(SCREEN_WIDTH / 4, 470, 50, 80);

    glColor3ub(114, 50, 17);
    trapezium(SCREEN_WIDTH / 4 - 200, 530, 20, 110);
    glColor3ub(105, 40, 10);
    rectangle(SCREEN_WIDTH / 4 - 200, 470, 60, 110);

    door(SCREEN_WIDTH / 2, 500, 150);

    //glEnd();
}

/*Scene 2 end*/

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
    /*face*/
    glColor3d(0, 0, 0);
    drawRoundedRect(x, y, width, height, 25.0f);
    /*eyes*/
    glColor3d(1, 1, 1);
    ellipse(x + 3 * scale, y, scale, 7.0f/5.0f * scale, 2 * scale);
    ellipse(x - 3 * scale, y, scale, 7.0f/5.0f * scale, 2 * scale);
    /*mouth*/
    glColor3d(1, 1, 1);
    semi_ellipse(x, y - 2 * scale, 7.0f / 5.0f * scale, scale, 30, 10.0f);
    /*body*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);
    /*arms*/

    /*wheels*/
    //stage right wheel
    glColor3d(0, 0, 0);
    drawRoundedRect(x + 8 * scale, y - 15 * scale, 3 * scale, 15 * scale, unit / 2.5f);
    glColor3d(120, 150, 180);
        int i = 0;
        while (i < 15 * scale) {
            glBegin(GL_LINE_STRIP);
            glVertex2d(x + 9.5 * scale, y - 8 * scale - i);
            glVertex2d(x + 6.5 * scale, y - 8 * scale - i);
            i += scale;
            glEnd();
        }
    //stage left wheel
    glColor3d(0, 0, 0);
    drawRoundedRect(x - 8 * scale, y - 15 * scale, 3 * scale, 15 * scale, unit / 2.5f);
    glColor3d(200, 150, 180);
    i = 0;
    while (i < 15 * scale) {
        glBegin(GL_LINE_STRIP);
        glVertex2d(x - 9.5 * scale, y - 8 * scale - i);
        glVertex2d(x - 6.5 * scale, y - 8 * scale - i);
        i += scale;
        glEnd();
    }
}

void drawBot9SideView(float x = 400.0f, float y = 500.0f, float size = 0.5, int face = 1) {
    float unit = 25;
    float scale = size * unit;
    /*neck*/
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x - 4.5 * scale, y - scale * 5, 5.5 * scale, 8 * scale, scale / 2.5f);
    
    
    /*wheel behind body*/
    glColor3d(0.2f, 0.2f, 0.2f);
    drawRoundedRect(x + 3 * scale, y - 20 * scale, 15 * scale, 5 * scale, scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + 3 * scale, y - 20 * scale, 15 * scale, 5 * scale, scale);
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x + scale, y - 20 * scale, 12 * scale, 5 * scale, scale);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + scale, y - 20 * scale, 12 * scale, 5 * scale, scale);
    glColor3d(0.5f, 0.5f, 0.5f);
    ellipse(x - 3  * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);
    ellipse(x + 1 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);
    ellipse(x + 5 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);


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

/*trash*/
void drawTrash() {
    
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

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	//drawn items here
    if (scene == 0) {
        drawScene0();
    }
    else if (scene == 1) {
        drawScene1();
        glPushMatrix();
        glTranslatef(400, 300, 0);
        glTranslatef(-400, -300, 0);
            glTranslatef(bot9_x, 0, 0);
            drawBot9SideView(400, 500, 0.3, 1);
        glPopMatrix();
        drawSeed(600, 300, 0.25f);
    }
    else if (scene == 2) {
        drawScene2();
        drawBot9(500,500,1,2);
        
    }
      
    glutSwapBuffers();
	glFlush();
}

void anim(int value) {
    glutPostRedisplay(); // Tell GLUT to redraw
    if (animation_state == 0) {
        if (bot9_x < 100) {
            if (wheel_rotation > -360)
                wheel_rotation -= 10;
            else
                wheel_rotation = 0;
            bot9_x += 1.0f;
        }
        else
            animation_state = 1;
    }
    else if (animation_state == 1) {
        if (head_angle > -30 && !head_flip) {
            head_angle--;
            arm_angle = -90;
        }
        else if (head_angle == -30) {
            draw_hands = true;
            if (arm_angle > -123 ) {
                arm_angle -= 2;
            }
            if (arm_extension < 2.2f) {
                arm_extension += 0.1;
            }
            if (arm_angle <= -123 && arm_extension >= 2.0f) { // need to use greater than or equal for floats (binary math issues)
                if (pause < 25)
                    pause++;
                else
                    animation_state = 2;
            }
        }
    }
    else if (animation_state == 2) {
        if (seed_x > -90) {
            pause = 0;
            if (arm_extension > 1.0)
                arm_extension -= 0.05;
            if (seed_x > -90)
                seed_x -= 4;
            if (seed_y < 60)
                seed_y += 60.0f / (90.0f / 4.0f);
        }
        if (arm_angle < -60) {
            if (pause < 100)
                pause++;
            else {
                arm_angle += 0.5;
                seed_y += 1;
            }
        }
        if (arm_angle == -60 && arm_extension <= 1.0f) {
            if (pause < 50)
                pause++;
            else {
                pause = 0;
                scene = 1;
            }
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
    glutCreateWindow("Test");
   
    glutDisplayFunc(display);
   
	glutTimerFunc(16, anim, 0);

	initGL();

	glutMainLoop();
}