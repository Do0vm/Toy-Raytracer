#pragma once

#include <glm.hpp>

using namespace glm;

#define EPSILON 0.00001f

class HitInfo {
	// Output return what i hit
public:
	HitInfo() : t(0.f), hit(false), hitLocation(0.f), normal(0.f),emissive(false){} // constructor

	float t;
	bool hit;
	vec3 hitLocation;
	vec3 normal;
	vec3 colour;
	bool emissive;

};

class Ray {
	// Input did i hit something?
public:

	Ray() : origin(0.f), direction(0.f){}

	vec3 origin;
	vec3 direction;

};

class Object {
public:
	Object() : colour (0.f), emissive(false){} //construcr

	vec3 colour;
	bool emissive;

	virtual HitInfo hit(Ray ray) const = 0; // Returns info about whether the ray hit the sphere

};

class Triangle : public Object {

public:
	Triangle() : v1(0.f),v2(0.f),v3(0.f){}

	vec3 v1, v2, v3;

	HitInfo hit(Ray ray) const {
		HitInfo hi;


		vec3 originalNormal = normalize(cross(v2 - v1, v3 - v1)); // getting normal the vector perp to both said vectrs

		vec3 n = originalNormal;

		if (dot(n, ray.direction) > 0.f)
			n = -n;

		//case 1 hit nothing
		if (fabs(dot(n, ray.direction)) < EPSILON)
			return hi;
		//case 2 hit nothing
		float t = (dot(n, v1) - dot(ray.origin, n)) / dot(ray.direction, n);

		if (fabs(t) < 0.005f)
			return hi;

		vec3 p = ray.origin + ray.direction * t;

		vec3 c1 = cross(v2 - v1, p - v1);
		vec3 c2 = cross(v3 - v2, p - v2);
		vec3 c3 = cross(v1 - v3, p - v3);

		//case 3 not inside traingle
		if (dot(c1, originalNormal) < 0 || dot(c2, originalNormal) < 0 || dot(c3, originalNormal) < 0)
			return hi;

		//We hit somthing
		hi.hit = true;
		hi.t = t;
		hi.hitLocation = p;
		hi.normal = n;
		hi.colour = colour;
		hi.emissive = emissive;

		return hi;
	}

};

float random() {
	return ((float)rand() / RAND_MAX);
}

//get random vector in hemisphere
vec3 randomVector() {


	vec3 d;

	do {

		d = 2.f * vec3(random(), random(), random()) - vec3(1.f);
	} while (dot(d, d) > 1.f);

	return normalize(d);
}

vec3 randomDirection(vec3 normal) {

	return normalize(randomVector() + normal);
}