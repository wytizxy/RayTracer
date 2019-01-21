// OpenGL2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "stdafx.h"
#include <cmath>
#include <GL/glut.h>

using namespace std;

const int W = 256;
const int H = 256;
float data1[W][H][3];

void display()
{
	glClearColor(0, 0, 0, 1);				//background color set
	glClear(GL_COLOR_BUFFER_BIT);			//background clean
	glDrawPixels(W, H, GL_RGB, GL_FLOAT, data1);
	glutSwapBuffers();					//swap buffers
}
struct vect {
	float x, y, z;
	vect(float(x), float(y), float(z)) : x(x), y(y), z(z) {};
	vect operator + (const vect& v) const
	{
		return vect(x + v.x, y + v.y, z + v.z);
	}
	vect operator - (const vect& v) const
	{
		return vect(x - v.x, y - v.y, z - v.z);
	}
	vect operator * (float d) const
	{
		return vect(x * d, y * d, z * d);
	}
	vect operator / (float d) const
	{
		return vect(x / d, y / d, z / d);
	}
	vect normalize() const {
		float mg = sqrt(x*x + y*y + z*z);
		return vect(x / mg, y / mg, z / mg);
	}
};
float dot(const vect& a, const vect& b) {
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}
vect cross(const vect& a, const vect& b) {
	return vect(a.y*b.z - b.y*a.z, a.z*b.x - a.x*b.z, a.x*b.y - b.x*a.y);
}
struct plane {
public:
	vect n;
	vect p;
	plane(const vect& n, const vect& p) : n(n), p(p) {}
};

struct sphere {
public:
	vect center;		//center:(x, y, z)
	float radii;			//r
	sphere(const vect& center, float radii) : center(center), radii(radii) {}
	vect getNormal(const vect& pi) const { return (pi - center) / radii; }
};
struct ray {
public:
	vect p, d;			//p:point d:direction
	ray(const vect& p, const vect& d) : p(p), d(d.normalize()) {}
};

bool intersect(sphere& sf, const ray& r, float &t) {
	const vect p = r.p;
	const vect d = r.d;
	const vect pc = p - sf.center;
	const float dp = dot(pc, d);
	const float pp = dot(pc, pc) - sf.radii*sf.radii;
	float disc = dp*dp - pp;
	if (disc < 0) return false;
	disc = sqrt(disc);
	t = -dp + disc;
	const float t0 = -dp - disc;
	const float t1 = -dp + disc;
	t = (t0 < t1) ? t0 : t1;
	return true;
}
void clamp1(vect& col) {
	col.x = (col.x > 1) ? 1 : (col.x < 0) ? 0 : col.x;
	col.y = (col.y > 1) ? 1 : (col.y < 0) ? 0 : col.y;
	col.z = (col.z > 1) ? 1 : (col.z < 0) ? 0 : col.z;
}
struct triangle {
public:
	vect a, b, c;
	triangle(const vect& a, const vect& b, const vect& c) :
		a(a), b(b), c(c) {}
};
bool intertri(triangle& tri, const ray& r, float &t) {
	const vect d = r.d;
	const vect p = r.p;
	vect E1 = tri.b - tri.a;
	vect E2 = tri.c - tri.a;
	vect n = cross(E1, E2).normalize();
	vect ap = tri.a - p;
	float apn = dot(ap, n);
	float dn = dot(d, n);
	t = apn / dn;
	if (t < 1e-4) return false;
	else return true;
}

int main(int argc, char**argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);	//set display mode
	glutInitWindowSize(W, H);
	glutCreateWindow("hw1-2");

	sphere sf1(vect(80, 120, 50), 50);		//color of sf1: blue
	sphere sf2(vect(40, 40, 50), 30);		//color of sf2: green
	vect LightDir(0, 1, 0);
	vect a(140, 140, 0), b(150, 140, 0), c(140, 150, 0), d(145, 145, 100);
	triangle tri1(a, b, c), tri2(a, b, d), tri3(a, c, d), tri4(b, c, d);


	const vect white(1, 1, 1);
	const vect black(0, 0, 0);
	const vect blue(0, 0, 1);
	const vect green(0, 1, 0);
	const vect red(1, 0, 0);
	vect LookAt(0, 0, -1);			//LookAt
	LookAt = LookAt.normalize();

	float t;
	vect pix_col(black);

	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			pix_col = black;
			const ray r(vect(x, y, 0), LookAt);
			if (intersect(sf1, r, t)) {
				const vect pi = r.p + r.d*t;
				const vect L = LightDir - pi;
				const vect N = sf1.getNormal(pi);
				const float dt = dot(L.normalize(), N.normalize());
				pix_col = (blue + white*dt) * 0.5;
				//pix_col = blue;
				clamp1(pix_col);
			}
			if (intersect(sf2, r, t)) {
				const vect pi = r.p + r.d*t;
				const vect L = LightDir - pi;
				const vect N = sf2.getNormal(pi);
				const float dt = dot(L.normalize(), N.normalize());
				pix_col = (green + white*dt) * 0.5;
				//pix_col = green;
				clamp1(pix_col);
			}
			if (intertri(tri1, r, t)) {
				const vect pi = r.p + r.d*t;
				const vect L = LightDir - pi;
				const vect N = sf2.getNormal(pi);
				const float dt = dot(L.normalize(), N.normalize());
				pix_col = red;
				clamp1(pix_col);
			}
			data1[y][x][0] = pix_col.x;
			data1[y][x][1] = pix_col.y;
			data1[y][x][2] = pix_col.z;
		}
	}

	glutDisplayFunc(display);
	glutMainLoop();
	return 0; // never reached
}