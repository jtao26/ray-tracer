#ifndef _TriangleH_
#define _TriangleH_

#include "Vector3.h"
#include "Point3.h"
#include "Color.h"

struct Triangle {
public:
	Triangle();
	Triangle(Point3 p0, Point3 p1, Point3 p2);
	~Triangle();

	void SetVertexTextureCoord(Point3& pt0, Point3& pt1, Point3& pt2);
	void SetVertexNormal(Vector3 vn1, Vector3 vn2, Vector3 vn3);
	void SetTriangleNormal(Vector3 pt1);
	// void SetPlaneNormal(Vector3 pn1);
	void SetTriangleMtColor(Color c);
	void SetTriangleSpColor(Color c);
	void SetKa(float k);
	void SetKd(float k);
	void SetKs(float k);
	void SetN(int n_);
	void SetAlpha(float alpha);
	void SetEta(float eta);

	void SetTexture(Color**, int flag);
	void SetTextureW(int width);
	void SetTextureH(int height);

	Point3* GetVertexArr() { return p; }
	Point3 GetVertexP0() { return p[0]; }
	Point3 GetVertexP1() { return p[1]; }
	Point3 GetVertexP2() { return p[2]; }

	Point3* GetVertexTextureArr() { return texture_coordinate; }
	Vector3* GetVertexNormalArr() { return vertex_normal; }
	Vector3 GetVertexNormalP0() { return vertex_normal[0]; }
	Vector3 GetVertexNormalP1() { return vertex_normal[1]; }
	Vector3 GetVertexNormalP2() { return vertex_normal[2]; }
	Vector3 GetTriangleNormal() { return triangle_normal; }

	Color GetMtColor() const { return mt_color_; }
	Color GetSpColor() const { return spec_color_; }
	float GetKaMultiplier() { return ka; }
	float GetKdMultiplier() { return kd; }
	float GetKsMultiplier() { return ks; }
	int GetN() { return n_; }
	float GetAlpha() { return alpha_; }
	float GetEta() { return eta_; }

	int GetVertexNormFlag() { return flag_vertex_norm; }
	int GetTextureFlag() { return flag_texture; }
	int GetTextureW() { return texture_width; }
	int GetTextureH() { return texture_height; }
	Color GetLocationTexture(int i, int j) { return texture[i][j]; }

	Vector3 CalculatePtNorm(Point3 pt, float a, float b, float c);
	Vector3 CalculateTriPlaneNorm();


	Point3 p[3];
	Vector3 vertex_normal[3];
	Point3 texture_coordinate[3];
	// Vector3 plane_normal;
	Vector3 triangle_normal;
	Color mt_color_;
	Color spec_color_;
	float ka = 0.0f;
	float kd = 0.0f;
	float ks = 0.0f;
	int n_ = 0;
	float alpha_;
	float eta_;
	int flag_vertex_norm = 0;
	int flag_texture = 0;
	Color** texture;
	int texture_width, texture_height;
};

#endif
