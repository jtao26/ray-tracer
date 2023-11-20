#ifndef SPHERE_H_
#define SPHERE_H_

#include "Point3.h"
#include "Color.h"

struct Sphere {
public:
	Sphere();
	Sphere(float x_, float y_, float z_, float r_);
	~Sphere();

	void SetOrigin(int x_, int y_, int z_);
	void SetRaidus(float r_);
	void SetMtColor(Color c);
	void SetSpColor(Color s);
	void SetKaMultiplier(float ka);
	void SetKdMultiplier(float kd);
	void SetKsMultiplier(float ks);
	void SetN(int n);
	void SetAlpha(float alpha);
	void SetEta(float eta);

	void SetTexture(Color **);
	void SetTextureW(int width);
	void SetTextureH(int height);

	Point3 GetOrigin() const { return origin; }
	float GetRadius() const { return radius; }
	Color GetMtColor() const { return mt_color; }
	Color GetSpColor() const { return sp_color; }
	float GetKaMultiplier() { return ka_; }
	float GetKdMultiplier() { return kd_; }
	float GetKsMultiplier() { return ks_; }
	int GetN() { return n_; }
	float GetAlpha() { return alpha_; }
	float GetEta() { return eta_; }

	int GetTextureFlag() { return flag_texture; }
	Color GetLocationTexture(int i, int j) { return texture[i][j]; }
	int GetTextureW() { return texture_width; }
	int GetTextureH() { return texture_height; }

	friend std::ostream& operator<< (std::ostream& os, const Sphere& p);
	friend std::istream& operator>> (std::istream& is, Sphere& p);

protected:
	Point3 origin;
	float radius;
	Color mt_color;
	float ka_, kd_, ks_;
	int n_;
	Color sp_color;
	int flag_texture = -1;
	Color** texture;
	int texture_width, texture_height;
	float alpha_;
	float eta_;
};

#endif // !SPHERE_H_

