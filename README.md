# software_graphics_pipeline

A. Introduction

This is a software graphics pipeline modelled after the old fixed-function OpenGL pipeline created for a 2012 computer graphics course.
It originally covered vertex and viewing transformations, rasterization, color interpolation, and z-buffer test using triangles
and quads for primitives.

B. Compiling and Running

Originally developed in visual studios, this has most recently been tested on Xubuntu and comes with a Makefile to compile and
run the program from the command line. 

C. Testing

There are currently six tests. Tests are chosen at compile time in main.cpp through a '#define TEST' directive. 

Test 1: Triangle rasterization

Test 2: Quad rasterization

Test 3: Vertex transformations

Test 4: Z-buffer test

Test 5: More transformations

Test 6: Color interpolation

Not all tests are currently working. Code needs to be reviewed and fixed. 
