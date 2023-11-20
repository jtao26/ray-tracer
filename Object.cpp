#include "Object.h"

Object::Object() {

}

void Object::AddToSphereArr(Sphere s) {
	Sphere_arr.push_back(s);
}

void Object::AddToTriangleArr(Triangle& t) {
	Triangle_arr.push_back(t);
}

void Object::AddToTriangleIdxArr(int i) {
	Triangle_idx_arr.push_back(i);
}