/**
	CS230
	Wojciech Karas
	HW1


 * minigl.cpp
 * -------------------------------
 * Implement miniGL here.
 * Do not use any additional files
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <stack>
#include <vector>
#include <queue>
#include "minigl.h"

#define COLOR_ATTR 3
#define DIMENSIONS 3
#define MATRIX_DIM 4
#define TRIANGLE 3
#define QUAD 4
#define DEG_TO_RAD 3.1415926/180.0


using namespace std;

//My own type definitions
using namespace std;
typedef vector < vector<MGLfloat> > twoDVector;


//Own function declaration
twoDVector createIdentity();

//VARIABLE DECLARATIONS

//Determines whether begin() or end() have been called
bool canDraw = false;

//Holds the current polygon mode
MGLint currentMode;
//Number of vertices read so far since mglBegin() was called
MGLint vertices = 0;

//Holds the current color values
vector<MGLbyte> currentColor(COLOR_ATTR);

//List of current vertices input
//twoDVector vertexList;

//Current matrix transform being used
MGLint matrixMode = 0;
//Current matrix being used; depends on matrixMode
//It is always one of the matrices on the stop of a matrix stack
twoDVector currentMatrix = createIdentity();
//Matrix Stack - each matrix is a 2D vector holding values of type MGLfloat
stack<twoDVector> modelViewStack;
stack<twoDVector> projectionViewStack;

//Current software framebuffer
//struct frameBuffer softwareFrameBuffer;

//Queue for the polygons
//Each polygon is a triangle. Even though quads are allowed, they are stored
//as two seperate triangle polygons.
queue<twoDVector> polygonList;
//This queue stores the color code for each polygon in the queue based on position 
//(1:1 mapping)
queue<vector<MGLbyte> > colorQueue;

//Used to store a list vertices before they finish a polygon between calls
//to the mglVertex* functions
twoDVector buildPolygon;


//Own helper functions

//Creates an identity matrix and returns it
twoDVector createIdentity()
{
	twoDVector tempIdentity;
	vector<MGLfloat> oneDVector(MATRIX_DIM);

	for(int i = 0; i < MATRIX_DIM; i++)
	{
		for(int j = 0; j < MATRIX_DIM; j++)
		{
			if( j == i)
				oneDVector[j] = 1;
			else
				oneDVector[j] = 0;
		}
		tempIdentity.push_back(oneDVector);
	}

	return tempIdentity;

}

//Barycentric coordinate formula
MGLfloat fBary(MGLfloat x, MGLfloat y, MGLfloat xa, MGLfloat xb, MGLfloat ya, MGLfloat yb)
{
	return ((ya - yb)*x + (xb - xa)*y + xa*yb - xb*ya);
}



/**
 * Standard macro to report errors
 */
inline void MGL_ERROR(const char* description) {
    printf("%s\n", description);
    exit(1);
}


/**
 * Read pixel data starting with the pixel at coordinates
 * (0, 0), up to (width,  height), into the array
 * pointed to by data.  The boundaries are lower-inclusive,
 * that is, a call with width = height = 1 would just read
 * the pixel at (0, 0).
 *
 * Rasterization and z-buffering should be performed when
 * this function is called, so that the data array is filled
 * with the actual pixel values that should be displayed on
 * the two-dimensional screen.
 */
void mglReadPixels(MGLsize width,
                   MGLsize height,
                   MGLpixel *data)
{
	//Holds the depth value for each pixel; dependent on the z value of the vertex
	//1.0 is the furthest a vertex can be in normalized coordinates
	vector<MGLfloat> zBuffer (width*height, 1.0);

	//Rasterize each polygon
	while(!polygonList.empty())
	{
		//Used to work on individual vertices of a polygon
		vector<MGLfloat> vertex;
		//Take the first polygon on the queue
		twoDVector polygon = polygonList.front();
		polygonList.pop();
		//Get the corresponding color code
		vector<MGLbyte> colorCode = colorQueue.front();
		colorQueue.pop();

		//Used to hold each x,y,z value for each vertex
		MGLfloat x[3];
		MGLfloat y[3];
		MGLfloat z[3];

		//Get each vertex of the triangle
		for(int i = 0; i < polygon.size(); i++)
		{
			vertex = polygon[i];

			//Normalize by w
			x[i] = (vertex[0]/vertex[3]);
			y[i] = (vertex[1]/vertex[3]);
			z[i] = vertex[2]/vertex[3];

			//Viewport transformation
			x[i] = (x[i]+1)*width/2.0;
			y[i] = (y[i]+1)*height/2.0;

		}

		//alpha,beta,gamma are coefficients for barycentric coordinates
		//cZ holds the interpolated z value for a point based on barycentric coordinates
		MGLfloat alpha, beta, gamma, cZ;

		//Main loop to rasterize the triangle
		for(int i = 0; i < width; i++)
		{

			for(int j = 0; j < height; j++)
			{
				//Calculate the barycentric coordinates
				alpha = fBary(i, j, x[1], x[2], y[1], y[2])/fBary(x[0], y[0], x[1], x[2], y[1], y[2]);
				beta = fBary(i, j, x[2], x[0], y[2], y[0])/fBary(x[1], y[1], x[2], x[0], y[2], y[0]);
				gamma = fBary(i, j, x[0], x[1], y[0], y[1])/ fBary(x[2], y[2], x[0], x[1], y[0], y[1]);

				//If all of the barycentric coordinates are between [0,1] then the pixel is considered
				//to be inside the triangle
				if( (alpha >= 0 && alpha <= 1) && (beta >= 0 && beta <= 1) && (gamma >= 0 && gamma <= 1))
				{
					//Interpolates the depth value
					cZ = alpha*z[0] + beta*z[1] + gamma*z[2];

					//If this new pixel's depth is smaller than the old one, draw it on screen
					//and store the new depth in the z-buffer
					if(cZ < zBuffer[width*j + i])
					{
						zBuffer[width*j + i] = cZ;
						//data[width*j + i] = 0xFFFFFFFF;
						//Sets the color for the pixel
						MGL_SET_RED(data[width*j + i] , colorCode[0]);
						MGL_SET_GREEN(data[width*j + i] , colorCode[1]);
						MGL_SET_BLUE(data[width*j + i] , colorCode[2]);

					}
					else
						;//Not inside the polygon, skip pixel
				}
				else
					;//skip pixel; outside triangle
			}
		}

	}

}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
	//Check to see if mglBegin() has been called already
	if(canDraw == true)
		MGL_ERROR("Begin already specified");
	else
	{

		//Set to draw mode
		canDraw = true;
		//Set the polygon mode
		if(mode == MGL_TRIANGLES)
			currentMode = 3;
		else if(mode == MGL_QUADS)
			currentMode = 4;
		else
			currentMode = mode;

		//Reset number of specified vertices
		vertices = 0;

	}
}

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd()
{
	//Requires a matching mglBegin() call earlier in the code
	if(canDraw == false)
		MGL_ERROR("Begin not yet specified");
	//Stop input of vertices
	else
	{
		canDraw = false;
	
		//Gets rid of any prior vertices as they did not yet form a polygon
		buildPolygon.clear();
	}
}

/**
 * Specify a two-dimensional vertex; the x- and y-coordinates
 * are explicitly specified, while the z-coordinate is assumed
 * to be zero.  Must appear between calls to mglBegin() and
 * mglEnd().
 */
void mglVertex2(MGLfloat x,
                MGLfloat y)
{
	
	//Gets the current matrices, without a pop, to be used for vertex transformations
	twoDVector modelViewM = modelViewStack.top();
	twoDVector projectionM = projectionViewStack.top();
	//Transform coordinates

	//Set up specified coordinates as an n-dimensional vector
	vector<MGLfloat> coordinateVector(MATRIX_DIM);
	coordinateVector[0] = x;
	coordinateVector[1] = y;
	coordinateVector[2] = 0;
	coordinateVector[3] = 1; 


	//modelview transform
	MGLfloat temp = 0;
	for(int i = 0; i < MATRIX_DIM; i++)
	{
		for(int j = 0; j < MATRIX_DIM; j++)
		{
			temp += (modelViewM[i][j] * coordinateVector[j]);
		}
		coordinateVector[i] = temp;
		temp = 0;
	}

	//Projection transform
	for(int i = 0; i < MATRIX_DIM; i++)
	{
		for(int j = 0; j < MATRIX_DIM; j++)
		{
			temp += (projectionM[i][j] * coordinateVector[j]);
		}
		coordinateVector[i] = temp;
		temp = 0;
	}

	//Store coordinates as vertex

	//If there are enough vertices for a polygon, it should have been stored last call
	//and those temporary vertices can now be erased
	if(vertices % currentMode == 0)
		buildPolygon.clear();

	//Saves the transformed vertices into the not yet finished polygon
	buildPolygon.push_back(coordinateVector);

	vertices++;

	//Checks to see if there are now enough vertices to create a polygon
	//If yes, then push the polygon onto the polygon queue to render it
	//Otherwise continue
	if(vertices % currentMode == 0)
	{
		//If quad, break up into two triangles
		if(currentMode == QUAD)
		{
			twoDVector triangleOne, triangleTwo;
			
				
			triangleOne.push_back(buildPolygon[0]);
			triangleOne.push_back(buildPolygon[1]);
			triangleOne.push_back(buildPolygon[2]);

			triangleTwo.push_back(buildPolygon[2]);
			triangleTwo.push_back(buildPolygon[3]);
			triangleTwo.push_back(buildPolygon[0]);
			
			polygonList.push(triangleOne);
			colorQueue.push(currentColor);
			
			polygonList.push(triangleTwo);
			colorQueue.push(currentColor);
			
			
		}
		else
		{
			polygonList.push(buildPolygon);
			colorQueue.push(currentColor);
		}
	}
	


}

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x,
                MGLfloat y,
                MGLfloat z)
{
	//Gets the current matrices, without a pop, to be used for vertex transformations
	twoDVector modelViewM = modelViewStack.top();
	twoDVector projectionM = projectionViewStack.top();
	//Transform coordinates

	//Set up specified coordinates as an n-dimensional vector
	vector<MGLfloat> coordinateVector(MATRIX_DIM);
	coordinateVector[0] = x;
	coordinateVector[1] = y;
	coordinateVector[2] = z;
	coordinateVector[3] = 1; 


	//modelview transform
	MGLfloat temp = 0;
	for(int i = 0; i < MATRIX_DIM; i++)
	{
		for(int j = 0; j < MATRIX_DIM; j++)
		{
			temp += (modelViewM[i][j] * coordinateVector[j]);
		}
		coordinateVector[i] = temp;
		temp = 0;
	}

	//Projection transform
	for(int i = 0; i < MATRIX_DIM; i++)
	{
		for(int j = 0; j < MATRIX_DIM; j++)
		{
			temp += (projectionM[i][j] * coordinateVector[j]);
		}
		coordinateVector[i] = temp;
		temp = 0;
	}


	//Store coordinates as vertex

	//If there are enough vertices for a polygon, it should have been stored last call
	//and those temporary vertices can now be erased
	if(vertices % currentMode == 0)
		buildPolygon.clear();

	//Saves the transformed vertices into the not yet finished polygon
	buildPolygon.push_back(coordinateVector);

	vertices++;

	//Checks to see if there are now enough vertices to create a polygon
	//If yes, then push the polygon onto the polygon queue to render it
	//Otherwise continue
	if(vertices % currentMode == 0)
	{
		//If quad, break up into two triangles
		if(currentMode == QUAD)
		{
			twoDVector triangleOne, triangleTwo;
			
			triangleOne.push_back(buildPolygon[0]);
			triangleOne.push_back(buildPolygon[1]);
			triangleOne.push_back(buildPolygon[2]);

			triangleTwo.push_back(buildPolygon[2]);
			triangleTwo.push_back(buildPolygon[3]);
			triangleTwo.push_back(buildPolygon[0]);
			
			polygonList.push(triangleOne);
			colorQueue.push(currentColor);
			
			polygonList.push(triangleTwo);
			colorQueue.push(currentColor);
			
			
		}
		else
		{
			polygonList.push(buildPolygon);
			colorQueue.push(currentColor);
		}
	}
	
}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{
	if(mode == MGL_MODELVIEW || mode == MGL_PROJECTION)
		matrixMode = mode;
	else
		MGL_ERROR("Incorrect matrix mode value");
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{
	//Test for current matrix mode and push the current matrix onto the corresponding
	//matrix stack
	if(matrixMode == MGL_MODELVIEW)
	{
		modelViewStack.push(currentMatrix);
		currentMatrix = modelViewStack.top();
	}
	else if(matrixMode == MGL_PROJECTION)
	{
		projectionViewStack.push(currentMatrix);
		currentMatrix = projectionViewStack.top();
	}
	else
		MGL_ERROR("Invalid matrix mode");
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
	//Test for matrix mode and pop the matrix off the corresponding stack
	//Only if it is not the sole matrix on the stack. There should always
	//be an identity matrix at the bottom of the stack.
	if(matrixMode == MGL_MODELVIEW)
	{
		if(modelViewStack.size() > 1)
			modelViewStack.pop();
		currentMatrix = modelViewStack.top();
	}
	else if(matrixMode == MGL_PROJECTION)
	{
		if(projectionViewStack.size() > 1)
			projectionViewStack.pop();
		currentMatrix = projectionViewStack.top();
	}
	else
		MGL_ERROR("Invalid matrix mode");
}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
	currentMatrix = createIdentity();

	if(matrixMode == MGL_MODELVIEW)
		modelViewStack.push(currentMatrix);
	else if(matrixMode == MGL_PROJECTION)
		projectionViewStack.push(currentMatrix);
	else
		MGL_ERROR("Invalid matrix mode");
}

/**
 * Replace the current matrix with an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglLoadMatrix(const MGLfloat *matrix)
{
	//i is the row number and j is the column number
	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			//j*MATRIX_DIM + i is the position in the 2D matrix if it were turned into a 1D vector
			//For example: i = 3, j = 2 means (2*4 + 3) = 11
			currentMatrix[i][j] = matrix[j*MATRIX_DIM + i];
		}
	}

	if(matrixMode == MGL_MODELVIEW)
	{
		modelViewStack.pop();
		modelViewStack.push(currentMatrix);
	}
	else if(matrixMode == MGL_PROJECTION)
	{
		projectionViewStack.pop();
		projectionViewStack.push(currentMatrix);
	}
	else
		MGL_ERROR("Invalid matrix mode");
}

/**
 * Multiply the current matrix by an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglMultMatrix(const MGLfloat *matrix)
{
	
	//Create a temporary matrix to hold the argument
	twoDVector tempMatrix = createIdentity();
	//i is the row number and j is the column number
	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			//j*MATRIX_DIM + i is the position in the 2D matrix if it were turned into a 1D vector
			//For example: i = 3, j = 2 means (2*4 + 3) = 11
			tempMatrix[i][j] = matrix[j*MATRIX_DIM + i];
		}
	}

	//Holds
	MGLfloat temp = 0;

	//Multiply the two matrices
	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			for(int k = 0; k < MATRIX_DIM; k++)
			{
				temp += tempMatrix[i][k] * currentMatrix[k][j];
			}
			currentMatrix[i][j] = temp;
			temp = 0;
		}
	}

	if(matrixMode == MGL_MODELVIEW)
	{
		modelViewStack.pop();
		modelViewStack.push(currentMatrix);
	}
	else if(matrixMode == MGL_PROJECTION)
	{
		projectionViewStack.pop();
		projectionViewStack.push(currentMatrix);
	}
	else
		MGL_ERROR("Invalid matrix mode");
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x,
                  MGLfloat y,
                  MGLfloat z)
{

	twoDVector translationMatrix = createIdentity();

	//Set up the necessary values for the transformation matrix
	translationMatrix[0][MATRIX_DIM - 1] = x;
	translationMatrix[1][MATRIX_DIM - 1] = y;
	translationMatrix[2][MATRIX_DIM - 1] = z;

	//Convert the matrix into a 1D array and send it to multiply the current matrix
	MGLfloat oneDTranslation[MATRIX_DIM*MATRIX_DIM];

	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			oneDTranslation[j*MATRIX_DIM + i] = translationMatrix[i][j];
		}
	}

	mglMultMatrix(oneDTranslation);
	
}

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle,
               MGLfloat x,
               MGLfloat y,
               MGLfloat z)
{
	
	twoDVector rotateMatrix = createIdentity();
	
	MGLfloat cosAng = cos(angle*DEG_TO_RAD);
	MGLfloat sinAng = sin(angle*DEG_TO_RAD);
	
	//Sets up the values of the rotation matrix
	rotateMatrix[0][0] = x*x*(1-cosAng)+cosAng;
	rotateMatrix[0][1] = x*y*(1-cosAng)-z*sinAng;
	rotateMatrix[0][2] = x*z*(1-cosAng)+y*sinAng;
	rotateMatrix[1][0] = y*x*(1-cosAng)+z*sinAng;
	rotateMatrix[1][1] = y*y*(1-cosAng)+cosAng;
	rotateMatrix[1][2] = y*z*(1-cosAng)-x*sinAng;
	rotateMatrix[2][0] = x*z*(1-cosAng)-y*sinAng;
	rotateMatrix[2][1] = y*z*(1-cosAng)+x*sinAng;
	rotateMatrix[2][2] = z*z*(1-cosAng)+cosAng;
	
	//Convert the matrix into a 1D array and send it to multiply the current matrix
	MGLfloat oneDRotate[MATRIX_DIM*MATRIX_DIM];
	
	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			oneDRotate[j*MATRIX_DIM + i] = rotateMatrix[i][j];
		}
	}

	mglMultMatrix(oneDRotate);
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z)
{
	 twoDVector scaleMatrix = createIdentity();

	//set up the values of the scale matrix
	scaleMatrix[0][0] = x;
	scaleMatrix[1][1] = y;
	scaleMatrix[2][2] = z;

	//Convert the matrix into a 1D array and send it to multiply the current matrix
	MGLfloat oneDScale[MATRIX_DIM*MATRIX_DIM];

	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			oneDScale[j*MATRIX_DIM + i] = scaleMatrix[i][j];
		}
	}

	mglMultMatrix(oneDScale);
}

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left,
                MGLfloat right,
                MGLfloat bottom,
                MGLfloat top,
                MGLfloat near,
                MGLfloat far)
{
	twoDVector frustumM = createIdentity();

	//Set up the values of the frustum matrix
	frustumM[0][0] = 2.0 * near / (right - left);
	frustumM[1][1] = 2.0 * near / (top - bottom);
	frustumM[2][2] = -1.0 * (far + near)/(far - near);
	frustumM[3][3] = 0;
	frustumM[0][2] = (right + left)/(right - left);
	frustumM[1][2] = (top + bottom)/(top - bottom);
	frustumM[3][2] = -1;
	frustumM[2][3] = -2.0 * far * near / (far - near);

	//Translate into 1D array and pass it to be multiplied

	MGLfloat oneDfrustum[MATRIX_DIM*MATRIX_DIM];

	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			oneDfrustum[j*MATRIX_DIM + i] = frustumM[i][j];
		}
	}

	mglMultMatrix(oneDfrustum);
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left,
              MGLfloat right,
              MGLfloat bottom,
              MGLfloat top,
              MGLfloat near,
              MGLfloat far)
{
	//Creation of the orthographic matrix for 4x4

	twoDVector orthoM = createIdentity();

	orthoM[0][0] = 2.0/(right - left);
	orthoM[1][1] = 2.0/(top - bottom);
	orthoM[2][2] = 2.0/(near - far);
	orthoM[0][3] = -1.0 * (left + right) / (right - left);
	orthoM[1][3] = -1.0 * (bottom + top) / (top - bottom);
	orthoM[2][3] = (far + near) / (near - far);

	//Translate into 1D array and pass it to be multiplied

	MGLfloat oneDOrtho[MATRIX_DIM*MATRIX_DIM];

	for(int j = 0; j < MATRIX_DIM; j++)
	{
		for(int i = 0; i < MATRIX_DIM; i++)
		{
			oneDOrtho[j*MATRIX_DIM + i] = orthoM[i][j];
		}
	}

	mglMultMatrix(oneDOrtho);

}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
	//Special case in order to allow for else if statement
	//This way color can be specified before mglBegin()
	if(vertices == 0)
	{
		currentColor[0] = red;
		currentColor[1] = green;
		currentColor[2] = blue;
	}
	//Allows only change if a polygon hasn't been partially specified yet
	else if(vertices % currentMode == 0)
	{
		currentColor[0] = red;
		currentColor[1] = green;
		currentColor[2] = blue;
	}
	else
	;	//MGL_ERROR("Can't change color in the middle of specifying a polygon");
}
