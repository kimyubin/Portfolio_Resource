#include "EnochGeometry.h"

VectorF2D::VectorF2D(Vector2D pos)
{
    *this = VectorF2D(pos.x,pos.y);
} 

VectorF2D VectorF2D::Normalize(float targetLength=1)
{
    auto length = sqrt(x*x+y*y);
    if(length == 0) return VectorF2D(0,0);
    targetLength /= length;
    return VectorF2D(x*targetLength, y*targetLength);
}

inline VectorF2D Vector2D::operator + (VectorF2D rhs) { return VectorF2D(x + rhs.x, y + rhs.y); }
inline VectorF2D Vector2D::operator - (VectorF2D rhs) { return VectorF2D(x - rhs.x, y - rhs.y); }
inline VectorF2D Vector2D::operator * (float rhs) { return VectorF2D(x * rhs, y * rhs); }

bool isCollided(Circle circle, Segment segment)
{
	double a=0,b=0,c=0;	//ax2 + bx + c
	double s1=0,s2=0;	//solution of ax2 + bx + c
	double segment_min=0, segment_max=0;	//segment range(x or y)
	circle.radius += segment.width;
	if (abs(segment.dst.y - segment.src.y) < 0.000001) {
		//linear formula of segment : y = a;
		//get (ax2 + bx + c) from simultaneous equations of Circle and Segment 
		a = 1;
		b = -2 * circle.center.x;
		c = pow(circle.center.x,2) + pow(segment.src.y - circle.center.y, 2) - pow(circle.radius,2);

		segment_min = std::min(segment.src.x, segment.dst.x);
		segment_max = std::max(segment.src.x, segment.dst.x);
	}
	else if (abs(segment.dst.x - segment.src.x) < 0.000001) {
		//linear formula of segment : x = a;
		//get (ay2 + by + c) from simultaneous equations of Circle and Segment 
		a = 1;
		b = -2 * circle.center.y;
		c = pow(circle.center.y, 2) + pow(segment.src.x - circle.center.x, 2) - pow(circle.radius, 2);

		segment_min = std::min(segment.src.y, segment.dst.y);
		segment_max = std::max(segment.src.y, segment.dst.y);
	}
	else {	
		//linear formula of segment : y = a1x + b1;
		auto a1 = (segment.dst.y - segment.src.y) / (segment.dst.x - segment.src.x);
		auto b1 = segment.src.y - a1 * segment.src.x;

		//get (ax2 + bx + c = 0) from simultaneous equations of Circle and Segment 
		a = pow(a1, 2) + 1;
		b = 2 * a1 * b1 - 2 * a1 * circle.center.y - 2 * circle.center.x;
		c = pow(circle.center.x, 2) + pow(b1 - circle.center.y, 2) - pow(circle.radius, 2);

		segment_min = std::min(segment.src.x, segment.dst.x);
		segment_max = std::max(segment.src.x, segment.dst.x);
	}
	//get x using quadratic formula
	auto d = pow(b, 2) - 4 * a * c;
	if (d < 0.000001)
		return false;
	d = sqrt(d);

	s1 = (-b + d) / (2 * a);
	s2 = (-b - d) / (2 * a);
	//check if x1,x2 is in range of segment
	if (segment_min <= s1 && s1 <= segment_max)
		return true;
	else if (segment_min <= s2 && s2 <= segment_max)
		return true;
	else
		return false;
}