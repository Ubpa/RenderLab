#ifndef MATH_LINE_H
#define MATH_LINE_H

// ------------------------------ 接口 ------------------------------

struct Line {
	vec3 o;
	vec3 d; // normaliezed
};

Line Line_2P(vec3 p0, vec3 p1);
vec3 Line_At(Line line, float t);
vec3 Line_ClosestPoint(Line line, vec3 p);

// ------------------------------ 实现 ------------------------------

Line Line_2P(vec3 p0, vec3 p1) {
	return Line(p0, normalize(p1 - p0));
}

vec3 Line_At(Line line, float t) {
	return line.o + t * line.d;
}

vec3 Line_ClosestPoint(Line line, vec3 p) {
	float t = dot(p - line.o, line.d);
	return Line_At(line, t);
}

#endif // !MATH_LINE_H
