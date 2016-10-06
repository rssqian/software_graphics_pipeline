/**
 * main.cpp
 * -------------------------------
 * This is simple testbed for your miniGL implementation. 
 * Modify the function Render() to issue commands to your
 * miniGL pipeline. 
 *
 * The program will draw the solution in the left half of the window and your result in the right half of the window
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "minigl.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

// change this to test a different test (1-6)
#define TEST 1
// Change these to test MiniGL at different resolutions
#define WIDTH  640
#define HEIGHT 480

//--------------------------------------------------------------------------------
// this calls your implementation which you should compare against the OpenGL implementation
void Render()
{
#include "include_test.h"
}

// this draws the left half of the window using OpenGL
void glRender()
{
#include "include_test_gl.h"
}
//--------------------------------------------------------------------------------
void Draw_Pixels()
{
    // get the data from assignment
    //MGLpixel white=0;MGL_SET_RED(white,255);MGL_SET_GREEN(white,255);MGL_SET_BLUE(white,255);
    MGLpixel pixel_data[WIDTH*HEIGHT];
    for(int i=0;i<WIDTH;i++)for(int j=0;j<HEIGHT;j++) pixel_data[j*WIDTH+i]=0;

    mglReadPixels(WIDTH,HEIGHT,pixel_data);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2f(1,0);
    glDrawPixels(WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_INT_8_8_8_8,(GLvoid*)pixel_data);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Initialize()
{
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT);
}

void Display()
{
    glViewport(0,0,WIDTH,HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glRender();
    Draw_Pixels();
    glFlush();
}

int main(int argc, char** argv)
{
    Render();

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize (2*WIDTH, HEIGHT);
    glutCreateWindow ("Test OpenGL Program");
    Initialize();
    glutDisplayFunc(Display);
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glDrawBuffer(GL_FRONT);
    glutMainLoop();
    return 0;
}

