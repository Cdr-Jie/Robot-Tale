#include <Windows.h>
#include <stdlib.h>
#include <math.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>
#include <iostream>

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720
#define M_PI 3.1415926f
#define FPS 120

int animation_state = 0;

void initGL() {
	glClearColor(255.0, 255.0, 255.0, 0.0);
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

void semi_ellipse(float cx, float cy, float rx, float ry, int num_segments)
{
    float theta = M_PI / float(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    float x = -1;//we start at angle = 0 
    float y = 0;
    glLineWidth(10.0);
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

void drawBot9() {
    //height = 
    //width =
    /*face*/
    glColor3d(0, 0, 0);
    drawRoundedRect(500.0f, 500.0f, 275.0f, 200.0f, 25.0f);
    /*eyes*/
    glColor3d(255, 255, 255);
    ellipse(575, 500, 25, 35, 50);
    ellipse(425, 500, 25, 35, 50);
    /*head*/
    glColor3d(0, 0, 0);
    drawRoundedRectOutline(500.0f, 500.0f, 300.0f, 225.0f, 25.0f);
    /*mouth*/
    glColor3d(255, 255, 255);
    semi_ellipse(500, 450, 35, 25, 30);
    /*body*/
    /*arms*/
    /*wheels*/
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	//drawn items here
    glPushMatrix();
        glTranslatef(500, 400, 0.0);
            glRotated(10, 0, 0, 50);
            glScaled(0.99, 0.99, 0.99);
        glTranslatef(-500, -400, 0.0);
        drawBot9();
    if (animation_state == 100){
        glPopMatrix();
    }
	glutSwapBuffers();
	glFlush();
}

void anim(int value) {
	if (animation_state <= 100) {
        animation_state++;
        std::cout << animation_state << std::endl;
	}
	else if (animation_state == 1) {
	}
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