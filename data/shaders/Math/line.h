#ifndef MATH_LINE_H
#define MATH_LINE_H

// ------------------------------ 接口 ------------------------------

struct Line {
	vec3 o;
	vec3 d;
};

vec3 Line_At(Line line, float t);

// ------------------------------ 实现 ------------------------------

vec3 Line_At(Line line, float t) {
	return line.o + t * line.d;
}

#endif // !MATH_LINE_H
