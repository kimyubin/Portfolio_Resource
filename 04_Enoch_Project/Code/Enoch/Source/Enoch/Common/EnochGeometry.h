#pragma once

#include<math.h>

//Vector Integer
struct Vector2D {
    int y;
    int x;
	inline bool operator == (Vector2D rhs) { return (y == rhs.y) && (x == rhs.x);}
	inline Vector2D operator + (Vector2D rhs) {	return {y + rhs.y, x + rhs.x};}
	inline Vector2D operator - (Vector2D rhs) { return {y - rhs.y, x - rhs.x}; }
	inline struct VectorF2D operator + (VectorF2D rhs);
	inline struct VectorF2D operator - (VectorF2D rhs);
	inline struct VectorF2D operator * (float rhs);
};

//Vector float
struct VectorF2D {
	float y;
	float x;
    VectorF2D() : y(0), x(0) {};
    VectorF2D(int xIn, int yIn) : y(yIn), x(xIn) {};
    VectorF2D(float xIn, float yIn) : y(yIn), x(xIn) {};
    VectorF2D(Vector2D pos);
	inline bool operator == (VectorF2D rhs) { return (y == rhs.y) && (x == rhs.x);}
	inline VectorF2D operator + (VectorF2D rhs) {	return VectorF2D(x + rhs.x, y + rhs.y);}
	inline VectorF2D operator - (VectorF2D rhs) { return VectorF2D(x - rhs.x, y - rhs.y); }
	inline VectorF2D operator + (Vector2D rhs) { return VectorF2D(x + rhs.x, y + rhs.y); }
	inline VectorF2D operator * (float p) { return VectorF2D(x * p, y * p); }
    Vector2D GetFieldPoint() { return {int(y+0.5f), int(x+0.5f)}; }   //諛섏삱由?
    VectorF2D Normalize(float targetLength);
	inline float distance(VectorF2D other) { return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2)); };
};

struct Segment {
	VectorF2D src;
	VectorF2D dst;
	float width;
	Segment(VectorF2D srcIn, VectorF2D dstIn, float widthIn=0.0f) : src(srcIn), dst(dstIn), width(widthIn) { };
};

struct Circle {
	VectorF2D center;
	float radius;
	Circle(VectorF2D centerIn, float radiusIn) : center(centerIn), radius(radiusIn) { };
};

bool isCollided(Circle circle, Segment segment);