#ifndef ENGINE_LIGHT_SPHERE_LIGHT_H
#define ENGINE_LIGHT_SPHERE_LIGHT_H

// ------------------------------ 接口 ------------------------------

// 32
struct SphereLight {
	vec3 position;  // 12    0
	vec3 luminance; // 12   16
	float radius;   //  4   28
};

vec3 SphereLight_Illuminance(SphereLight light, vec3 vertex, vec3 norm);
vec3 SphereLight_LuminancePower(SphereLight light);
vec3 SphereLight_MRP(SphereLight light, vec3 vertex, vec3 R);

// ------------------------------ 实现 ------------------------------

vec3 SphereLight_Illuminance(SphereLight light, vec3 vertex, vec3 norm) {
	vec3 diff = light.position - vertex;
	float dist = length(diff);
	vec3 wi = diff / dist;
	float cosTheta = dot(wi, norm);

	float ratio = dist / light.radius;
	float ratio2 = ratio * ratio;

	float illuminanceFactor;
	if (ratio * cosTheta > 1)
		illuminanceFactor = cosTheta / ratio2;
	else {
		float sinTheta = sqrt(1 - cosTheta * cosTheta);
		float cotTheta = cosTheta / sinTheta;
		float x = sqrt(ratio2 - 1);
		float y = -x * cotTheta;

		illuminanceFactor = (1 / (PI * ratio2)) *
			(cosTheta * acos(y) - x * sinTheta * sqrt(1 - y * y)) +
			(1 / PI) * atan(sinTheta * sqrt(1 - y * y) / x);
	}
	illuminanceFactor *= PI;
	illuminanceFactor = max(0.0, illuminanceFactor);
	return illuminanceFactor * light.luminance;
}

vec3 SphereLight_LuminancePower(SphereLight light) {
	return FOUR_PI2 * light.radius * light.radius * light.luminance;
}

vec3 SphereLight_MRP(SphereLight light, vec3 vertex, vec3 R) {
	vec3 fragToLight = light.position - vertex;
	vec3 LtoR = dot(fragToLight, R) * R - fragToLight;
	return light.position + saturate(light.radius / length(LtoR)) * LtoR;
}

#endif // !ENGINE_LIGHT_SPHERE_LIGHT_H
