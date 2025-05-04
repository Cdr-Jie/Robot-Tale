#include <Windows.h>
#include <stdlib.h>
#include <math.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>
#include <iostream>

#define SCREEN_WIDTH 980
#define SCREEN_HEIGHT 720
#define M_PI 3.1415926f
#define FPS 120

float arm_angle = 0; //0 is facing bottom, all rotates counter clockwise
float head_angle = 0; // 0 is facing right

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

void semi_ellipse(float cx, float cy, float rx, float ry, int num_segments, float width = 10)
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

void drawBot9(float x = 500.0f, float y = 500.0f, float size = 1) {
    float unit = 25;
    float scale = size * unit; //25
    float height = 8 * size * 25; //200
    float width = 11 * size * 25; //275
    /*neck?*/
    glColor3d(0, 0, 0);
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
    semi_ellipse(x, y - 2 * scale, 7.0f / 5.0f * scale, scale, 30);
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

void drawBot9SideView(float x = 400.0f, float y = 500.0f, float size = 0.5) {
    float unit = 25;
    float scale = size * unit;
    /*neck*/
    glColor3d(0, 0, 0);
    drawRoundedRect(x - 4.5 * scale, y - scale * 5, 5.5 * scale, 8 * scale, unit / 2.5f);
    /*head*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x - 4.5 * scale, y, 12 * scale, 9 * scale, 25.0f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 4.5 * scale, y, 12 * scale, 9 * scale, 25.0f);
    
    /*wheel behind body*/
    glColor3d(0.2f, 0.2f, 0.2f);
    drawRoundedRect(x + 3 * scale, y - 20 * scale, 15 * scale, 5 * scale, unit);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + 3 * scale, y - 20 * scale, 15 * scale, 5 * scale, unit);
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x + scale, y - 20 * scale, 12 * scale, 5 * scale, unit);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x + scale, y - 20 * scale, 12 * scale, 5 * scale, unit);
    glColor3d(0.5f, 0.5f, 0.5f);
    ellipse(x - 3  * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);
    ellipse(x + 1 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);
    ellipse(x + 5 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, 50);


    /*body*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x - 7.5 * scale, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 7.5 * scale, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);

   


    /*head*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x, y, 12 * scale, 9 * scale, 25.0f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y, 12 * scale, 9 * scale, 25.0f);
    /*face*/
    glColor3d(0, 0, 0);
    drawRoundedRect(x, y, 11 * scale, 8 * scale, 25.0f);
    /*eyes*/
    glColor3d(1, 1, 1);
    ellipse(x + 3 * scale, y, scale, 7.0f / 5.0f * scale, 2 * scale);
    ellipse(x - 3 * scale, y, scale, 7.0f / 5.0f * scale, 2 * scale);

    /*mouth*/
    glColor3d(1, 1, 1);
    semi_ellipse(x, y - 2 * scale, 7.0f / 5.0f * scale, scale, 30);
    
    /*body*/
    glColor3d(1, 1, 1);
    drawRoundedRect(x, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x, y - 13 * scale, 15 * scale, 15 * scale, unit / 2.5f);

    
    /*wheels*/
    glColor3d(0.3f, 0.3f, 0.3f);
    drawRoundedRect(x - 12 * scale, y - 20 * scale, 15 * scale, 5 * scale, unit);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 12 * scale, y - 20 * scale, 15 * scale, 5 * scale, unit);
    glColor3d(0.5f, 0.5f, 0.5f); // Medium grey (RGB: 128, 128, 128)
    drawRoundedRect(x - 14 * scale, y - 20 * scale, 12 * scale, 5 * scale, unit);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 14 * scale, y - 20 * scale, 12 * scale, 5 * scale, unit);
    glColor3d(0.9f, 0.9f, 0.9f);
    ellipse(x - 10 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    ellipse(x - 14 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    ellipse(x - 18 * scale, y - 20 * scale, 1.5 * scale, 1.5 * scale, unit);
    /*arms*/
    float arms_x = x - 11 * scale;
    float arms_y = y - 10 * scale;
    glPushMatrix();
    glTranslatef(arms_x, arms_y, 0);
    glRotatef(90, 0, 0, 1); //rotates arm
    glRotatef(arm_angle, 0, 0, 1); //rotates arm
    glTranslatef(-arms_x, -arms_y, 0);

    glColor3d(1, 1, 1);
    drawRoundedRect(x - 5 * scale, y - 10 * scale, 13 * scale, 2 * scale, unit / 2.5f);
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(x - 5 * scale, y - 10 * scale, 13 * scale, 2 * scale, unit / 2.5f);
    glColor3d(0, 0, 0);
    ellipse(x - 11 * scale, y - 10 * scale, 2 * scale, 2 * scale, unit); //arm hole
    glColor3d(1, 1, 1);
    ellipse(x - 11 * scale, y - 10 * scale, 1.5 * scale, 1.5 * scale, unit); //arm hole

    glColor3d(0, 0, 0);
    glPushMatrix();
    float clawposx = x + 4 * scale;
    float clawposy = y - 10 * scale;
    glTranslatef(clawposx, clawposy, 0);

    glRotatef(270, 0, 0, 1);
    glTranslatef(-clawposx, -clawposy, 0);
    semi_ellipse(clawposx, clawposy, 2 * scale, 2 * scale, 10, 20.0f);

    glPopMatrix();
    glPopMatrix();
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	//drawn items here   

    drawBot9SideView();

    //glPushMatrix();
    //float shear = -0.1f;
    //GLfloat m[16] = {
    //1.0f, shear, 0.0f, 0.0f,
    //shear, 1.0f, 0.0f, 0.0f,
    //0.0f, 0.0f, 1.0f, 0.0f,
    //0.0f, 0.0f, 0.0f, 1.0f
    //};
    //glMultMatrixf(m);
    //drawBot9(500.0f, 500.0f, 0.5f);//min 0.5f as any further will cause issues
    //glPopMatrix();
    
    
    glutSwapBuffers();
	glFlush();
}

void anim (int value) {
	if (arm_angle >= -180 && arm_angle <= 0) {
        arm_angle--;
	}
	else if (arm_angle < -180){
        arm_angle = abs(arm_angle);
        arm_angle++;
	}
    else if (arm_angle < 360) {
        arm_angle++;
    }
    else {
        arm_angle = 0;
    }
    //size += 0.01;

	glutPostRedisplay(); // Tell GLUT to redraw

	glutTimerFunc(16, anim, 0); // Call again in 16ms (~60 FPS)
}


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