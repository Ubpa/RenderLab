#ifndef MATH_INTERSECT_H
#define MATH_INTERSECT_H

#include "line.h"
#include "plane.h"

// ------------------------------ 接口 ------------------------------

vec3 Intersect(Line line, Plane plane);

// ------------------------------ 实现 ------------------------------

vec3 Intersect(Line line, Plane plane) {
	// P : line.o + t * line.d
	//    dot(plane.n, P - plane.o) = 0
	// => t * dot(plane.n, line.d) + dot(plane.n, line.o - plane.o) = 0
	// => t = dot(plane.n, plane.o - line.o) / dot(plane.n, line.d)
	float t = dot(plane.n, plane.o - line.o) / dot(plane.n, line.d);
	return Line_At(line, t);
}

#endif // !MATH_INTERSECT_H
