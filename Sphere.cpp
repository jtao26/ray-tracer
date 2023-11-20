#include "Sphere.h"


Sphere::Sphere() {
	origin = Point3(0, 0, 0);
	radius = 0;
	mt_color[0] = 0;
	mt_color[1] = 0;
	mt_color[2] = 0;
	sp_color[0] = 0;
	sp_color[1] = 0;
	sp_color[2] = 0;
	alpha_ = -1;
	eta_ = -1;
}

Sphere::Sphere(float x_, float y_, float z_, float r_) {
	origin = Point3(x_, y_, z_);
	radius = r_;
	mt_color[0] = 0;
	mt_color[1] = 0;
	mt_color[2] = 0;
	sp_color[0] = 0;
	sp_color[1] = 0;
	sp_color[2] = 0;
	alpha_ = -1;
	eta_ = -1;
}

Sphere::~Sphere() {

}

void Sphere::SetOrigin(int x_, int y_, int z_) {
	origin = Point3(x_, y_, z_);
}

void Sphere::SetRaidus(float r_) {
	radius = r_;
}

void Sphere::SetMtColor(Color c) {
	mt_color = c;
}

void Sphere::SetSpColor(Color s) {
	sp_color = s;
}
void Sphere::SetKaMultiplier(float ka) {
	ka_ = ka;
}
void Sphere::SetKdMultiplier(float kd) {
	kd_ = kd;
}
void Sphere::SetKsMultiplier(float ks) {
	ks_ = ks;
}
void Sphere::SetN(int n) {
	n_ = n;
}
void Sphere::SetAlpha(float alpha) {
	alpha_ = alpha;
}

void Sphere::SetEta(float eta) {
	eta_ = eta;
}

void Sphere::SetTexture(Color** c) {
	texture = c;
	flag_texture = 1;
}

void Sphere::SetTextureW(int width) {
	texture_width = width;
}

void Sphere::SetTextureH(int height) {
	texture_height = height;
}

std::ostream& operator<< (std::ostream& os, const Sphere& p) {
	return os << p.origin << " " << p.radius;
}

std::istream& operator>> (std::istream& is, Sphere& p) {
	char dummy;
	Point3 origin;
	return is >> dummy >> origin >> dummy >> p.radius;
}