#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Function prototypes
void drawScene0();
void drawScene1();
void drawScene2();

//shapes
void rectangle(int x, int y, int h, int w);
void circle(int n, int x, int y, int r);
void trapezium(int x, int y, int h, int w);
void ellipse(float cx, float cy, float rx, float ry, int num_segments);
//void semi_ellipse(float cx, float cy, float rx, float ry, int num_segments, float width = 10.0f);
//void drawRoundedRect(float x, float y, float width, float height, float radius, int segments = 16);
//void drawRoundedRectOutline(float x, float y, float width, float height, float radius, int segments = 16);

//objects
void window(int x, int y, int h, int w);
void door(int x, int y, int w);
void pot(int x, int y, int h, int w);
void drawSeed(float x, float y, float scale);


//characters
//void drawBot9(float x = 500.0f, float y = 500.0f, float size = 1, int face = 1);
//void drawBot9SideView(float x = 400.0f, float y = 500.0f, float size = 0.5, int face = 1);

#endif // FUNCTIONS_H
#pragma once
