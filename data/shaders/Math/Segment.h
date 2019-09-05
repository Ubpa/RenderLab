#ifndef MATH_SEGMENT_H
#define MATH_SEGMENT_H

#include "basic.h"

// ------------------------------ 接口 ------------------------------

struct Segment {
	vec3 a;
	vec3 b;
};

vec3 Segment_At(Segment segment, float t);
vec3 Segment_ClosestPoint(Segment segment, vec3 p);

// ------------------------------ 实现 ------------------------------

Segment Segment_2P(vec3 p0, vec3 p1) {
	return Segment(p0, normalize(p1 - p0));
}

vec3 Segment_At(Segment segment, float t) {
	return mix(segment.a, segment.b, saturate(t));
}

vec3 Segment_ClosestPoint(Segment segment, vec3 p) {
	vec3 ab = segment.b - segment.a;
	float t = dot(p - segment.a, ab) / dot(ab, ab);
	return Segment_At(segment, t);
}

#endif // !MATH_SEGMENT_H
