#ifndef INPUT_PROCESSOR_H_
#define INPUT_PROCESSOR_H_

#include <string>
#include <vector>
#include "Point3.h"
#include "Vector3.h"
#include "Color.h"
#include "Sphere.h"
#include "LightSource.h"
#include "Triangle.h"

struct InputProcessor {
public:
	InputProcessor();
	~InputProcessor();

	void OutputStoredData();

	int CheckFileContent(const std::string keyword, std::ifstream& in_stream);
	void ReadDataFromFile(const std::string& filename);
	void SetEyePos(float x_, float y_, float z_);
	void SetViwDir(float x_, float y_, float z_);
	void SetUpDir(float x_, float y_, float z_);
	void SetFov(float vfov_);
	void SetImageSize(int width, int height);
	void SetBgColor(float red, float green, float blue);
	void SetMtColor(float red, float green, float blue);
	void SetSpColor(float red, float green, float blue);
	void SetLightSource(float x_, float y_, float z_, float w, float red, float green, float blue);
	void SetAttPtLightSource(float x_, float y_, float z_, float w, float red, float green, float blue, float c1, float c2, float c3);
	void SetKaMultiplier(float ka);
	void SetKdMultiplier(float kd);
	void SetKsMultiplier(float ks);
	void SetAlpha(float alpha);
	void SetEta(float eta);
	void SetTexturePpm(std::string filename);

	void AddVerticiesToArray(float x_, float y_, float z_);
	void AddVerticiesIdxToArray(int v1, int v2, int v3);
	void AddTriangleToArray(int v1, int v2, int v3);
	void AddTriangleToArray(int v1, int v2, int v3, int vn1, int vn2, int vn3, std::string typ);
	void AddTriangleToArray(int v1, int v2, int v3, int vt1, int vt2, int vt3, int vn1, int vn2, int vn3);
	void AddVertexNormalToArray(float vn1, float vn2, float vn3);
	void AddVertexNormIdxToArray(int vn1, int vn2, int vn3);
	void AddVertexTextureToArray(int vt1, int vt2);
	void AddVertexTextureIdxToArray(int vt1, int vt2, int vt3);

	void SetSphereData(float x_, float y_, float z_, float radius);
	void SetFlagLightAtt();
	void SetFlagSoftShadow();
	void SetN(int n);
	void SetMaxDepth(int d);
	void SetViewDist(float view_dist);
	void SetFlagDepthOfView(bool val);

	Point3 GetEyePos() { return eye; }
	Vector3 GetViwDir() { return view_dir; }
	Vector3 GetUpDir() { return up_dir; }
	float GetFov() { return fov; }
	int* GetImageSize() { return image_size; }
	Color GetBgColor() { return bg_color; }
	Color GetIppMtColor() { return material_color; }
	Color GetIppSpColor() { return specular_color; }
	float GetKaMultiplier() { return ka_; }
	float GetKdMultiplier() { return kd_; }
	float GetKsMultiplier() { return ks_; }
	int GetFlagLightAtt() { return flag_light_attenuation; }
	int GetFlagSoftShadow() { return flag_soft_shadow; }
	bool GetFlagDepthOfView() { return flag_depth_of_field; }
	int GetN() { return n_; }
	float GetAlpha() { return alpha_; }
	float GetEta() { return eta_; }
	int GetMaxDepth() { return max_depth; }
	float GetViewDist() { return view_distance; }

	Color** GetTexture() { return texture_color; }
	int GetTextureFlag() { return texture_flag; }
	int GetTextureW() { return texture_width; }
	int GetTextureH() { return texture_height; }
	
	std::vector<LightSource> GetLightVector() { return light_arr; }
	std::vector<Sphere> GetSphereVector() { return sphere_arr; }
	std::vector<Point3> GetVertexArr() { return vertex_arr; }
	std::vector<int> GetVertexIdxArr() { return vertex_idx_arr; }
	// std::vector<Color> GetVerticiesDiffColor() { return verticies_diff_color; }
	// std::vector<Color> GetVerticiesSpColor() { return verticies_sp_color; }
	// std::vector<float> GetVerticiesParaColor() { return verticies_para; }
	std::vector<Vector3> GetVertexNormalArr() { return vertex_norm_arr; }
	std::vector<Triangle> GetTriangleArr() { return triangle_arr; }
	std::vector<Point3> GetVertexTextureArr() { return vertex_texture_arr; }

protected:
	void processVertexInput(std::string str, std::string& ans1, std::string& ans2, std::string& ans3);

	Point3 eye;
	Vector3 view_dir;
	Vector3 up_dir;
	float fov;
	int image_size[2];
	Color bg_color;
	Color material_color;
	Color specular_color;
	std::vector<Sphere> sphere_arr;
	std::vector<Triangle> triangle_arr;
	std::vector<LightSource> light_arr;
	float ka_, kd_, ks_;
	int n_;
	float alpha_, eta_;
	int flag_light_attenuation = 0, flag_soft_shadow = 0;
	std::vector<Point3> vertex_arr;
	std::vector<int> vertex_idx_arr;
	std::vector<Vector3> vertex_norm_arr;
	std::vector<int> vertex_norm_idx_arr;

	int texture_width, texture_height;
	Color** texture_color;
	int texture_flag = 0;
	std::vector<Point3> vertex_texture_arr;
	std::vector<int> vertex_texture_idx_arr;
	int max_depth = 1;
	float view_distance = 5.0f;
	bool flag_depth_of_field = false;
};



#endif // !INPUT_PROCESSOR_H_
