#include "Triangle.h"

Triangle::Triangle() {
	ka = 0;
	kd = 0;
	ks = 0;
	n_ = 0;
	mt_color_[0] = 0;
	mt_color_[1] = 0;
	mt_color_[2] = 0;
	spec_color_[0] = 0;
	spec_color_[1] = 0;
	spec_color_[2] = 0;
}

Triangle::Triangle(Point3 p0, Point3 p1, Point3 p2) {
	p[0] = p0;
	p[1] = p1;
	p[2] = p2;
	ka = 0;
	kd = 0;
	ks = 0;
	n_ = 0;
}

Triangle::~Triangle() {

}

void Triangle::SetVertexTextureCoord(Point3& pt0, Point3& pt1, Point3& pt2) {
	texture_coordinate[0] = pt0;
	texture_coordinate[1] = pt1;
	texture_coordinate[2] = pt2;
}

void Triangle::SetVertexNormal(Vector3 vn1, Vector3 vn2, Vector3 vn3) {
	vertex_normal[0] = vn1;
	vertex_normal[1] = vn2;
	vertex_normal[2] = vn3;
	flag_vertex_norm = 1;
}

void Triangle::SetTriangleNormal(Vector3 pt1) {
	triangle_normal = pt1;
}

//void Triangle::SetPlaneNormal(Vector3 pn1) {
//	plane_normal = pn1;
//}

void Triangle::SetTriangleMtColor(Color c) {
	mt_color_ = c;
}

void Triangle::SetTriangleSpColor(Color c) {
	spec_color_ = c;
}

void Triangle::SetKa(float k) {
	ka = k;
}

void Triangle::SetKd(float k) {
	kd = k;
}

void Triangle::SetKs(float k) {
	ks = k;
}

void Triangle::SetN(int n) {
	n_ = n;
}

void Triangle::SetAlpha(float alpha) {
	alpha_ = alpha;
}

void Triangle::SetEta(float eta) {
	eta_ = eta;
}

void Triangle::SetTexture(Color** c, int flag) {
	texture = c;
	flag_texture = flag;
}

void Triangle::SetTextureW(int width) {
	texture_width = width;
}

void Triangle::SetTextureH(int height) {
	texture_height = height;
}

Vector3 Triangle::CalculatePtNorm(Point3 pt, float a, float b, float c) {
	Vector3 res = a * vertex_normal[0] + b * vertex_normal[1] + c * vertex_normal[2];
	res.Normalize();
	return res;
}

Vector3 Triangle::CalculateTriPlaneNorm() {
	Vector3 e1 = GetVertexP1() - GetVertexP0();
	Vector3 e2 = GetVertexP2() - GetVertexP0();
	Vector3 N = e1.Cross(e2);
	N.Normalize();
	return N;
}

