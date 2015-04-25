// ** THIS IS A CODE SNIPPET WHICH WILL EFFICIEINTLY TRIANGULATE ANY
// ** POLYGON/CONTOUR (without holes) AS A STATIC CLASS.  THIS SNIPPET
// ** IS COMPRISED OF 3 FILES, TRIANGULATE.H, THE HEADER FILE FOR THE
// ** TRIANGULATE BASE CLASS, TRIANGULATE.CPP, THE IMPLEMENTATION OF
// ** THE TRIANGULATE BASE CLASS, AND TEST.CPP, A SMALL TEST PROGRAM
// ** DEMONSTRATING THE USAGE OF THE TRIANGULATOR.  THE TRIANGULATE
// ** BASE CLASS ALSO PROVIDES TWO USEFUL HELPER METHODS, ONE WHICH
// ** COMPUTES THE AREA OF A POLYGON, AND ANOTHER WHICH DOES AN EFFICENT
// ** POINT IN A TRIANGLE TEST.
/*****************************************************************/
/** Static class to triangulate any contour/polygon efficiently **/
/** Does not support polygons with holes.      **/
/** Uses STL vectors to represent a dynamic array of vertices.  **/
/*****************************************************************/

#pragma once

#include "Common.h"
#include <vector>  // Include STL vector class.

class Triangulate
{
public:

	// triangulate a contour/polygon, places results in STL vector
	// as series of triangles.
	static bool Process(const vector<Vec2> &contour, vector<Vec2> &result);

	// compute area of a contour/polygon
	static float Area(const vector<Vec2> &contour);

	// decide if point Px/Py is inside triangle defined by
	// (Ax,Ay) (Bx,By) (Cx,Cy)
	static bool InsideTriangle(float Ax, float Ay,
		float Bx, float By,
		float Cx, float Cy,
		float Px, float Py);


private:
	static bool Snip(const vector<Vec2> &contour, int u, int v, int w, int n, int *V);

};
