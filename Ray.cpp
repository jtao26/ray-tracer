#include "Ray.h"
#include <cmath>
#include <random>
#include "Input_processor.h"
#include "ctime"

#define MATH_DIV_OFFSET 1e-8

void Ray::BiCoordinateTriangle(Triangle& t, float& alpha, float& beta, float& gamma, Point3* i_point) {
	Point3 vertex0 = t.GetVertexP0();
	Point3 vertex1 = t.GetVertexP1();
	Point3 vertex2 = t.GetVertexP2();
	// p0 to p1
	Vector3 e1 = vertex1 - vertex0;
	// p0 to p2
	Vector3 e2 = vertex2 - vertex0;
	Vector3 n = e1.Cross(e2);
	n.Normalize();
	float A = n.x();
	float B = n.y();
	float C = n.z();
	float D = 0 - (A * vertex0.x() + B * vertex0.y() + C * vertex0.z());
	//if (n.Dot(dir_) >= 0 && n.Dot(dir_) <= 1) {
	//	float denominator = A * (-dir_.x()) + B * (-dir_.y()) + C * (-dir_.z());
	//}
	//else {
		float denominator = A * dir_.x() + B * dir_.y() + C * dir_.z();
	//}
	

	// check if the intersect point is within the triangle
	Vector3 ep = *i_point - vertex0;
	float Dbase = e1.Dot(e1) * e2.Dot(e2) - e1.Dot(e2) * e1.Dot(e2);
	float Db = e2.Dot(e2) * ep.Dot(e1) - e1.Dot(e2) * ep.Dot(e2);
	float Dy = e1.Dot(e1) * ep.Dot(e2) - e1.Dot(e2) * ep.Dot(e1);
	beta = Db / Dbase;
	gamma = Dy / Dbase;
	alpha = 1 - (beta + gamma);
}

int ClosestSphere(Point3& pt, std::vector<Sphere>& sphere_arr) {
	int idx = -1;
	for (int i = 0; i < sphere_arr.size(); i++) {
		Sphere sphere_ = sphere_arr.at(i);
		float d = (sphere_.GetOrigin() - pt).Length();
		if (std::abs(sphere_.GetRadius() - d) < MATH_DIV_OFFSET) {
			idx = i;
		}
	}
	return idx;
}

Ray::Ray() {
	origin_ = Point3(0, 0, 0);
	dir_ = Vector3(0, 0, -1);
}

Ray::Ray(const Point3 origin, Vector3 dir) {
	origin_ = origin;
	if (dir.Length() != 1) {
		dir.Normalize();
	}
	dir_ = dir;
}

Ray::~Ray() {
}

bool Ray::operator==(const Ray& other) const {
    return (origin_ == other.origin()) && (dir_ == other.direction());
}


bool Ray::operator!=(const Ray& other) const {
    return (origin_ != other.origin()) || (dir_ != other.direction());
}

float Ray::Length() const {
	return dir_.Length();
}

Point3 Ray::origin() const {
    return origin_;
}


Vector3 Ray::direction() const {
    return dir_;
}

Color Ray::RayTrace(InputProcessor& ipp, int depth, int max_depth) {

	float i_t = -1;
	Point3 i_point = Point3(-1, -1, -1);
	int obj_idx = -1;
	int obj_typ = IntersectDetect(ipp, i_t, i_point, obj_idx);
	if (depth > 1) {
		if (i_t == -1) {
			//refract_t += 0;
		}
		else {
			//refract_t += i_t;
			SetRefractT(i_t);
		}
	}
	
	Color color_ = RayShade(ipp, obj_idx, obj_typ, i_point, depth, max_depth);
	return color_;
}

Color Ray::RayShade(InputProcessor& ipp, int obj_idx, int obj_typ, Point3& i_point, int depth, int max_depth) {
	Color c;

	if (obj_typ == -1 || depth > max_depth) {
		// typ == -1, no intersect, return background color
		// depth > 5, too much recursice depth, return background color
		c = ipp.GetBgColor();
		c[0] = c.red() * 255;
		c[1] = c.green() * 255;
		c[2] = c.blue() * 255;
	}
	else if (obj_typ == 1) {
		// sphere
		c = ShadeSphere(ipp, obj_idx, i_point, depth, max_depth);
	}
	else if (obj_typ == 2) {
		// triangle
		c = ShadeTriangle(ipp, obj_idx, i_point, depth, max_depth);
	}
	
	return c;
}

Color Ray::ShadeSphere(InputProcessor& ipp, int obj_idx, Point3 iPoint, int depth, int max_depth) {
	std::vector<Triangle> triangle_vec = ipp.GetTriangleArr();
	std::vector<Sphere> sphere_vec = ipp.GetSphereVector();
	std::vector<LightSource> light_arr = ipp.GetLightVector();
	Sphere sphere_ = sphere_vec.at(obj_idx);
	int light_size = light_arr.size();
	float ka = sphere_.GetKaMultiplier(), kd = sphere_.GetKdMultiplier(), ks = sphere_.GetKsMultiplier();

	Color diffuse_color_, specu_color_ = sphere_.GetSpColor();
	if (sphere_.GetTextureFlag() == -1) {
		// sphere do not use texture
		diffuse_color_ = sphere_.GetMtColor();
	}
	else {
		// sphere uses texture
		// Nx = (x-xc)/r
		float Nx = (iPoint.x() - sphere_.GetOrigin().x()) / (sphere_.GetRadius() * 1.0f);
		// Ny = (y-yc)/r
		float Ny = (iPoint.y() - sphere_.GetOrigin().y()) / (sphere_.GetRadius() * 1.0f);
		// Nz = (z-zc)/r
		float Nz = (iPoint.z() - sphere_.GetOrigin().z()) / (sphere_.GetRadius() * 1.0f);
		float phi = acos(Ny);
		float theta = atan2(-Nz, Nx);
		// map phi from [0, pi] to v from [0, 1]
		float v = phi / 3.1415926535;
		// map theta from [-pi, pi] to u from [0, 1]
		float u = (theta + 3.1415926535 ) / (2 * 3.1415926535);
		int i = u * (sphere_.GetTextureW() - 1), j = v * (sphere_.GetTextureH() - 1);
		diffuse_color_ = sphere_.GetLocationTexture(j, i);
		diffuse_color_[0] = diffuse_color_[0] / 255;
		diffuse_color_[1] = diffuse_color_[1] / 255;
		diffuse_color_[2] = diffuse_color_[2] / 255;
	}

	float light_intense = 1.0f / light_size;
	Vector3 V = ipp.GetEyePos() - iPoint;
	V.Normalize();
	Vector3 L;
	Vector3 H;
	Vector3 N = iPoint - sphere_.GetOrigin();
	N.Normalize();

	float Ir1 = ka * diffuse_color_.red();
	float Ig1 = ka * diffuse_color_.green();
	float Ib1 = ka * diffuse_color_.blue();

	float Ir2 = 0, Ig2 = 0, Ib2 = 0;

	float shadow_flag;
	for (int i = 0; i < light_size; i++) {
		LightSource light_ = light_arr.at(i);
		shadow_flag = 1.0;

		// implement light attenuation
		float att_factor = 1;
		if (ipp.GetFlagLightAtt() == 1) {
			if (light_.GetTypeMark() == 1 && light_.GetAttVar()[0] != -1) {
				// std::cout << "calculate att light: using " << light_.GetTypeMark() << std::endl;
				float* att = light_.GetAttVar();
				Vector3 dis = light_.GetPointLtOrigin() - iPoint;
				float len = dis.Length();
				att_factor = 1.0f / (att[0] + att[1] * len + att[2] * len * len);
			}
		}

		if (light_.GetW() == 1) {
			L = light_.GetPointLtOrigin() - iPoint;
		}
		else if (light_.GetW() == 0) {
			L = -light_.GetDirLtDirection();
		}
		L.Normalize();
		H = (V + L) / 2;
		H.Normalize();

		// calculating shadows
		Ray shadow_ray = Ray(iPoint, L);
		float shadow_t = -1;
		Point3 shadow_pt = Point3(0, 0, 0);
		float shadow_t_max = -1.0f;
		if (light_.GetW() == 1) {
			shadow_t_max = (light_.GetPointLtOrigin() - iPoint).Length();
		}

		// soft shadow
		if (ipp.GetFlagSoftShadow() == 1) {
			float jitter_rad = 0.5;
			float soft_shadow_val = 0.0f;
			float soft_itr_size = 60.0f;
			// generate 60 different shadow ray to implement soft shadow
			for (int k = 0; k < soft_itr_size; k++) {
				// shadow_flag = 1.0f;
				for (int j = 0; j < sphere_vec.size(); j++) {
					shadow_flag = 1.0f;
					// prevent numerical error due to self_intersect
					if (j == obj_idx) {
						continue;
					}
					Sphere shadow_sphere = sphere_vec.at(j);
					if (shadow_ray.IntersectSphere(shadow_sphere.GetOrigin(), shadow_sphere.GetRadius(), &shadow_t, &shadow_pt)) {
						if (shadow_t > 0) {
							if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
								// there is some another sphere exist between the current sphere and current light source
								shadow_flag = 0.0f;
							}
						}
					}
					soft_shadow_val += shadow_flag;
				}
				for (int j = 0; j < triangle_vec.size(); j++) {
					// prevent numerical error due to self_intersect
					if (j == obj_idx) {
						continue;
					}
					Triangle t = triangle_vec.at(j);
					if (shadow_ray.IntersectTriangle(t.GetVertexP0(), t.GetVertexP1(), t.GetVertexP2(), &shadow_t, &shadow_pt)) {
						if (shadow_t > 0) {
							if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
								// there is some anpther sphere exist between the current sphere and current light source
								shadow_flag = 0.0f;
							}
						}
					}
					soft_shadow_val += shadow_flag;
				}

				// generate random shadow ray here for next round
				Point3 new_lt_origin = light_.GetPointLtOrigin();
				for (int l = 0; l < 3; l++) {
					std::random_device randdev;
					std::mt19937 generator(randdev());
					std::uniform_real_distribution<> distrib(-jitter_rad, jitter_rad);
					float jit_delta = distrib(generator);
					new_lt_origin[l] += jit_delta;
				}
				Vector3 L_jit = new_lt_origin - iPoint;
				L_jit.Normalize();
				shadow_ray.set(new_lt_origin, L_jit);
			}
			shadow_flag = soft_shadow_val / soft_itr_size * (sphere_vec.size() + triangle_vec.size());
		}
		// hard shadow
		else {
			for (int j = 0; j < sphere_vec.size(); j++) {
				// prevent numerical error due to self_intersect
				if (j == obj_idx) {
					continue;
				}
				Sphere shadow_sphere = sphere_vec.at(j);
				if (shadow_ray.IntersectSphere(shadow_sphere.GetOrigin(), shadow_sphere.GetRadius(), &shadow_t, &shadow_pt)) {
					if (shadow_t > 0) {
						if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
							// there is some anpther sphere exist between the current sphere and current light source
							shadow_flag = 0.0f;
						}
					}
				}
			}
			for (int j = 0; j < triangle_vec.size(); j++) {
				// prevent numerical error due to self_intersect
				// if (j == obj_idx) {
				// 	 continue;
				// }
				Triangle t = triangle_vec.at(j);
				if (shadow_ray.IntersectTriangle(t.GetVertexP0(), t.GetVertexP1(), t.GetVertexP2(), &shadow_t, &shadow_pt)) {
					if (shadow_t > 0) {
						if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
							// there is some another sphere triangle between the current sphere and current light source
							shadow_flag = 0.0f;
						}
					}
				}
			}
		}

		// adding I2 terms together
		Ir2 += shadow_flag * att_factor * light_intense * (kd * diffuse_color_.red() * (N.Dot(L) > 0.2 ? N.Dot(L) : 0) +
			ks * specu_color_.red() * pow((N.Dot(H) > 0.1 ? N.Dot(H) : 0), sphere_.GetN()));

		Ig2 += shadow_flag * att_factor * light_intense * (kd * diffuse_color_.green() * (N.Dot(L) > 0.2 ? N.Dot(L) : 0) +
			ks * specu_color_.green() * pow((N.Dot(H) > 0.1 ? N.Dot(H) : 0), sphere_.GetN()));

		Ib2 += shadow_flag * att_factor * light_intense * (kd * diffuse_color_.blue() * (N.Dot(L) > 0.2 ? N.Dot(L) : 0) +
			ks * specu_color_.blue() * pow((N.Dot(H) > 0.1 ? N.Dot(H) : 0), sphere_.GetN()));
	}

	float Ir3 = 0, Ig3 = 0, Ib3 = 0;
	float Ir4 = 0, Ig4 = 0, Ib4 = 0;
	// if sphere material contains, calculate recursive color-ray tracing
	if (sphere_.GetAlpha() != -1 && depth <= max_depth) {
		if (depth == 1) {
			N = N * 1;
		}
		Color refraction_color;
		Color reflect_color;
		Vector3 NT = N;
		Vector3 I = -1 * dir_;
		if (I.Dot(NT) < 0) {
			NT = -NT;
		}
		float cos_theta_i = NT.Dot(I);
		float sin_theta_t = (GetRayEta() / sphere_.GetEta()) * cos_theta_i;
		float cos_theta_t = sqrt(1 - (pow((GetRayEta() / sphere_.GetEta()), 2) * (1 - pow(cos_theta_i, 2))));
		Vector3 R = 2 * (cos_theta_i) *NT - I;
		Vector3 T = (-NT) * cos_theta_t + (GetRayEta() / sphere_.GetEta()) * (cos_theta_i * NT - I);
		R.Normalize();
		T.Normalize();
		
		Ray reflect_ray = Ray(iPoint, R);
		Ray refraction_ray = Ray(iPoint, T);
		reflect_ray.SetRayEta(sphere_.GetEta());
		refraction_ray.SetRayEta(sphere_.GetEta());

		float F0 = pow(((sphere_.GetEta() - GetRayEta()) / (sphere_.GetEta() + GetRayEta())), 2);
		float Fr = F0 + (1 - F0) * pow((1 - cos_theta_i), 5);

		reflect_color = reflect_ray.RayTrace(ipp, depth + 1, max_depth);
		refraction_color = refraction_ray.RayTrace(ipp, depth + 1, max_depth);

		reflect_color[0] /= 255.0f;
		reflect_color[1] /= 255.0f;
		reflect_color[2] /= 255.0f;
		refraction_color[0] /= 255.0f;
		refraction_color[1] /= 255.0f;
		refraction_color[2] /= 255.0f;
		Ir3 = Fr * reflect_color.red();
		Ig3 = Fr * reflect_color.green();
		Ib3 = Fr * reflect_color.blue();
		Ir4 = (1 - Fr) * exp(-sphere_.GetAlpha() * GetRefractT()) * refraction_color.red();
		Ig4 = (1 - Fr) * exp(-sphere_.GetAlpha() * GetRefractT()) * refraction_color.green();
		Ib4 = (1 - Fr) * exp(-sphere_.GetAlpha() * GetRefractT()) * refraction_color.blue();
	}


	
	float Ir = Ir1 + Ir2 + Ir3 + Ir4;
	float Ig = Ig1 + Ig2 + Ig3 + Ig4;
	float Ib = Ib1 + Ib2 + Ib3 + Ib4;

	Ir = Ir > 1.0f ? 1.0f : Ir;
	Ig = Ig > 1.0f ? 1.0f : Ig;
	Ib = Ib > 1.0f ? 1.0f : Ib;

	Color c = Color(floor(Ir * 255), floor(Ig * 255), floor(Ib * 255));

	return c;
}

Color Ray::ShadeTriangle(InputProcessor& ipp, int obj_idx, Point3& iPoint, int depth, int max_depth) {
	// std::cout << "shading vertex idx: " << p0 << " " << p1 << " " << p2 << std::endl;
	//if (p0 == -1 || p1 == -1 || p2 == -1) {
	//	throw std::invalid_argument("Ivalid vertex idx in RayShade");
	//}
	std::vector<Triangle> triangle_vec = ipp.GetTriangleArr();
	Triangle t = triangle_vec.at(obj_idx);
	std::vector<Sphere> sphere_vec = ipp.GetSphereVector();
	std::vector<LightSource> light_arr = ipp.GetLightVector();
	std::vector<Point3> vertex_arr = ipp.GetVertexArr();
	std::vector<int> vertex_idx_arr = ipp.GetVertexIdxArr();
	int light_size = light_arr.size();

	float ka = t.GetKaMultiplier(), kd = t.GetKdMultiplier(), ks = t.GetKsMultiplier();

	// each triangle has same answer, same as the first vertex

	Color diffuse_color_, specu_color_ = t.GetSpColor();
	if (t.GetTextureFlag() == 0) {
		// sphere do not use texture
		diffuse_color_ = t.GetMtColor();
	}
	else {
		Point3 p0 = t.GetVertexTextureArr()[0];
		Point3 p1 = t.GetVertexTextureArr()[1];
		Point3 p2 = t.GetVertexTextureArr()[2];
		float alpha = 0, beta = 0, gamma = 0;
		BiCoordinateTriangle(t, alpha, beta, gamma, &iPoint);
		Vector3 e1 = t.GetVertexP1() - t.GetVertexP0();
		Vector3 e2 = t.GetVertexP2() - t.GetVertexP0();
		Vector3 N_color = e1.Cross(e2);
		N_color.Normalize();
		float u, v;
		u = alpha * p0.x() + beta * p1.x() + gamma * p2.x();
		v = alpha * p0.y() + beta * p1.y() + gamma * p2.y();

		int i = v * (t.GetTextureH() - 1), j = u * (t.GetTextureW() - 1);
		diffuse_color_ = t.GetLocationTexture(i, j);
		diffuse_color_[0] = diffuse_color_[0] / 255;
		diffuse_color_[1] = diffuse_color_[1] / 255;
		diffuse_color_[2] = diffuse_color_[2] / 255;
	}
	
	// each vetrex has own color, later linear interplate?

	float light_intense = 1.0f / light_size;

	Vector3 V = ipp.GetEyePos() - iPoint;
	V.Normalize();
	Vector3 L;
	Vector3 H;

	float Ir1 = ka * diffuse_color_.red();
	float Ig1 = ka * diffuse_color_.green();
	float Ib1 = ka * diffuse_color_.blue();

	float Ir2 = 0, Ig2 = 0, Ib2 = 0;

	float shadow_flag;
	for (int i = 0; i < light_size; i++) {
		LightSource light_ = light_arr.at(i);
		shadow_flag = 1.0;

		// light attenuation
		float att_factor = 1;
		if (ipp.GetFlagLightAtt() == 1) {
			if (light_.GetTypeMark() == 1 && light_.GetAttVar()[0] != -1) {
				// std::cout << "calculate att light: using " << light_.GetTypeMark() << std::endl;
				float* att = light_.GetAttVar();
				Vector3 dis = light_.GetPointLtOrigin() - iPoint;
				float len = dis.Length();
				att_factor = 1.0f / (att[0] + att[1] * len + att[2] * len * len);
			}
		}
		
		// point light
		if (light_.GetW() == 1) {
			L = light_.GetPointLtOrigin() - iPoint;
		}
		// directional light
		else if (light_.GetW() == 0) {
			L = -light_.GetDirLtDirection();
		}

		L.Normalize();
		H = (V + L) / 2;
		H.Normalize();

		
		Vector3 e1 = t.GetVertexP1() - t.GetVertexP0();
		Vector3 e2 = t.GetVertexP2() - t.GetVertexP0();
		Vector3 N;
		if (t.GetVertexNormFlag() == 0) {
			// each triangle shares same normal vertex
			N = e1.Cross(e2);
			N.Normalize();
		}
		else {
			// each point has own normal
			float alpha = 0, beta = 0, gamma = 0;
			BiCoordinateTriangle(t, alpha, beta, gamma, &iPoint);
			Vector3 vn0 = t.GetVertexNormalP0();
			Vector3 vn1 = t.GetVertexNormalP1();
			Vector3 vn2 = t.GetVertexNormalP2();
			N = alpha * vn0 + beta * vn1 + gamma * vn2;
			N.Normalize();
		}

		// calculating shadows
		Ray shadow_ray = Ray(iPoint, L);
		float shadow_t = -1;
		Point3 shadow_pt = Point3(0, 0, 0);
		float shadow_t_max = -1.0f;
		if (light_.GetW() == 1) {
			shadow_t_max = (light_.GetPointLtOrigin() - iPoint).Length();
		}
		// soft shadow
		if (ipp.GetFlagSoftShadow() == 1) {
			float jitter_rad = 0.5;
			float soft_shadow_val = 0.0f;
			float soft_itr_size = 60.0f;
			// generate 60 different shadow ray to implement soft shadow
			for (int k = 0; k < soft_itr_size; k++) {
				// shadow_flag = 1.0f;
				for (int j = 0; j < sphere_vec.size(); j++) {
					shadow_flag = 1.0f;
					// prevent numerical error due to self_intersect
					//if (j == obj_idx) {
					//	continue;
					//}
					Sphere shadow_sphere = sphere_vec.at(j);
					if (shadow_ray.IntersectSphere(shadow_sphere.GetOrigin(), shadow_sphere.GetRadius(), &shadow_t, &shadow_pt)) {
						if (shadow_t > 0) {
							if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
								// there is some anpther sphere exist between the current sphere and current light source
								shadow_flag = 0.0f;
							}
						}
					}
					soft_shadow_val += shadow_flag;
				}
				for (int j = 0; j < triangle_vec.size(); j++) {
					// prevent numerical error due to self_intersect
					if (j == obj_idx) {
						continue;
					}
					Triangle t = triangle_vec.at(j);
					if (shadow_ray.IntersectTriangle(t.GetVertexP0(), t.GetVertexP1(), t.GetVertexP2(), &shadow_t, &shadow_pt)) {
						if (shadow_t > 0) {
							if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
								// there is some anpther sphere exist between the current sphere and current light source
								shadow_flag = 0.0f;
							}
						}
					}
					soft_shadow_val += shadow_flag;
				}

				// generate random shadow ray here for next round
				Point3 new_lt_origin = light_.GetPointLtOrigin();
				for (int l = 0; l < 3; l++) {
					std::random_device randdev;
					std::mt19937 generator(randdev());
					std::uniform_real_distribution<> distrib(-jitter_rad, jitter_rad);
					float jit_delta = distrib(generator);
					new_lt_origin[l] += jit_delta;
				}
				Vector3 L_jit = new_lt_origin - iPoint;
				L_jit.Normalize();
				shadow_ray.set(new_lt_origin, L_jit);
			}
			shadow_flag = soft_shadow_val / soft_itr_size * (sphere_vec.size() + triangle_vec.size());
		}
		// hard shadow
		else {
			for (int j = 0; j < sphere_vec.size(); j++) {
				// prevent numerical error due to self_intersect
				// if (j == obj_idx) {
				//	 continue;
				// }
				Sphere shadow_sphere = sphere_vec.at(j);
				if (shadow_ray.IntersectSphere(shadow_sphere.GetOrigin(), shadow_sphere.GetRadius(), &shadow_t, &shadow_pt)) {
					if (shadow_t > 0) {
						if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
							// there is some another sphere exist between the current sphere and current light source
							shadow_flag = 0.0f;
						}
					}
				}
			}
			for (int j = 0; j < triangle_vec.size(); j++) {
				// prevent numerical error due to self_intersect
				if (j == obj_idx) {
					continue;
				}
				Triangle t = triangle_vec.at(j);
				if (shadow_ray.IntersectTriangle(t.GetVertexP0(), t.GetVertexP1(), t.GetVertexP2(), &shadow_t, &shadow_pt)) {
					if (shadow_t > 0) {
						if ((light_.GetW() == 1 && shadow_t < shadow_t_max) || light_.GetW() == 0) {
							// there is some anpther sphere exist between the current sphere and current light source
							shadow_flag = 0.0f;
						}
					}
				}
			}
		}

		// adding I terms together
		Ir2 += shadow_flag * att_factor * light_intense * (kd * diffuse_color_.red() * (N.Dot(L) > 0 ? N.Dot(L) : 0) +
				ks * specu_color_.red() * pow((N.Dot(H) > 0 ? N.Dot(H) : 0), t.GetN()));

		Ig2 += shadow_flag * att_factor * light_intense * (kd * diffuse_color_.green() * (N.Dot(L) > 0 ? N.Dot(L) : 0) +
				ks * specu_color_.green() * pow((N.Dot(H) > 0 ? N.Dot(H) : 0), t.GetN()));

		Ib2 += shadow_flag * att_factor * light_intense * (kd * diffuse_color_.blue() * (N.Dot(L) > 0 ? N.Dot(L) : 0) +
				ks * specu_color_.blue() * pow((N.Dot(H) > 0 ? N.Dot(H) : 0), t.GetN()));
	}

	float Ir3 = 0, Ig3 = 0, Ib3 = 0;
	float Ir4 = 0, Ig4 = 0, Ib4 = 0;
	// if triangle material contains, calculate recursive color-ray tracing
	if (t.GetAlpha() != -1 && depth <= max_depth) {
		// use triangle normal as point's normal dir
		Color refraction_color;
		Color reflect_color;

		Vector3 N = t.GetTriangleNormal();
		Vector3 NT = N;

		Vector3 I = -1 * dir_;
		if (I.Dot(NT) < 0) {
			NT = -NT;
		}
		float cos_theta_i = I.Dot(N);
		float sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
		float sin_theta_t = (GetRayEta() / t.GetEta()) * sin_theta_i;
		float cos_theta_t = sqrt(1 - pow(sin_theta_i, 2));
		Vector3 R = 2 * (cos_theta_i) * N - I;
		Vector3 T = (-NT) * cos_theta_t + (GetRayEta() / t.GetEta()) * (cos_theta_i * NT - I);
		R.Normalize();
		T.Normalize();

		Ray reflect_ray = Ray(iPoint, R);
		Ray refraction_ray = Ray(iPoint, T);
		reflect_ray.SetRayEta(t.GetEta());
		refraction_ray.SetRayEta(t.GetEta());

		reflect_color = reflect_ray.RayTrace(ipp, depth + 1, max_depth);
		refraction_color = refraction_ray.RayTrace(ipp, depth + 1, max_depth);

		reflect_color[0] /= 255.0f;
		reflect_color[1] /= 255.0f;
		reflect_color[2] /= 255.0f;
		refraction_color[0] /= 255.0f;
		refraction_color[1] /= 255.0f;
		refraction_color[2] /= 255.0f;
		float F0 = pow(((t.GetEta() - GetRayEta()) / (t.GetEta() + GetRayEta())), 2);
		float Fr = F0 + (1 - F0) * pow((1 - cos_theta_i), 5);

		Ir3 = Fr * reflect_color.red();
		Ig3 = Fr * reflect_color.green();
		Ib3 = Fr * reflect_color.blue();
		Ir4 = (1 - Fr) * exp(-t.GetAlpha() * GetRefractT()) * refraction_color.red();
		Ig4 = (1 - Fr) * exp(-t.GetAlpha() * GetRefractT()) * refraction_color.green();
		Ib4 = (1 - Fr) * exp(-t.GetAlpha() * GetRefractT()) * refraction_color.blue();

	}
	/*
	float Ir = Ir1 + Ir2;
	float Ig = Ig1 + Ig2;
	float Ib = Ib1 + Ib2;
	*/
	
	float Ir = Ir1 + Ir2 + Ir3;
	float Ig = Ig1 + Ig2 + Ig3;
	float Ib = Ib1 + Ib2 + Ib3;
	
	/*
	float Ir = Ir1 + Ir2 + Ir3 + Ir4;
	float Ig = Ig1 + Ig2 + Ig3 + Ig4;
	float Ib = Ib1 + Ib2 + Ib3 + Ib4;
	*/
	
	Ir = Ir > 1.0f ? 1.0f : Ir;
	Ig = Ig > 1.0f ? 1.0f : Ig;
	Ib = Ib > 1.0f ? 1.0f : Ib;

	Color c = Color(floor(Ir * 255), floor(Ig * 255), floor(Ib * 255));

	return c;
	
}

bool Ray::IntersectSphere(const Point3& center, float radius, float* scale_t, Point3* i_point) const {
	const double A = dir_[0] * dir_[0] + dir_[1] * dir_[1] + dir_[2] * dir_[2];

	const double B = 2 * (dir_[0] * (origin_[0] - center[0])
		+ dir_[1] * (origin_[1] - center[1])
		+ dir_[2] * (origin_[2] - center[2]));

	const double C = std::pow((origin_[0] - center[0]), 2)
		+ std::pow((origin_[1] - center[1]), 2)
		+ std::pow((origin_[2] - center[2]), 2)
		- std::pow(radius, 2);

	double discriminant = std::pow(B, 2) - 4 * A * C;

	float insertion_time = 0.0f;

	if (discriminant < 0.0) {
		return false;
	}
	else {
		// got one or two interceotion with the sphere
		double discRoot = std::sqrt(discriminant);
		double t1 = (-B - discRoot) / 2 * A;
		double t2 = (-B + discRoot) / 2 * A;
		bool hit1 = false;
		bool hit2 = false;
		if (t1 > 0.01) {
			hit1 = true;
		}
		if (t2 > 0.01) {
			hit2 = true;
		}
		if (!(hit1) && !(hit2)) {
			return false;
		}
		if (hit1 && hit2) {
			if (t1 == t2) {
				*scale_t = (float)t1;
			}
			else if (t1 < t2) {
				*scale_t = (float)t1;
			}
			else {
				*scale_t = (float)t2;
			}
		}
		if (hit1 && !hit2) {
			*scale_t = (float)t1;
		}
		else if (!hit1 && hit2) {
			*scale_t = (float)t2;
		}
		*i_point = origin_ + (*scale_t) * dir_;
		return true;
	}
}

bool Ray::IntersectTriangle(const Point3& vertex0, const Point3& vertex1, const Point3& vertex2, float* scale_t, Point3* i_point) const {
	// calculate the plane equation
	// po to p1
	Vector3 e1 = vertex1 - vertex0;
	// p0 to p2
	Vector3 e2 = vertex2 - vertex0;
	Vector3 n = e1.Cross(e2);
	n.Normalize();
	float A = n.x();
	float B = n.y();
	float C = n.z();
	float D = 0 - (A * vertex0.x() + B * vertex0.y() + C * vertex0.z());
	float denominator = A * dir_.x() + B * dir_.y() + C * dir_.z();
	if (denominator == 0) {
		// the ray is paralle with the plane, impossible to have a intersection point
		*scale_t = -1;
		return false;
	}
	*scale_t = -(A * origin_.x() + B * origin_.y() + C * origin_.z() + D) / denominator;
	if (*scale_t <= 0.01) {
		*scale_t = -1;
		return false;
	}
	else {
		*i_point = origin_ + (*scale_t) * dir_;

		// check if the intersect point is within the triangle
		Vector3 ep = *i_point - vertex0;
		float Dbase = e1.Dot(e1) * e2.Dot(e2) - e1.Dot(e2) * e1.Dot(e2);
		float Db = e2.Dot(e2) * ep.Dot(e1) - e1.Dot(e2) * ep.Dot(e2);
		float Dy = e1.Dot(e1) * ep.Dot(e2) - e1.Dot(e2) * ep.Dot(e1);
		float beta = Db / Dbase, gama = Dy / Dbase;
		float alpha = 1 - (beta + gama);

		// if any alpha,beta,gama is not within [0,1] then outside, reset parameter
		// and return false;
		if (beta >= 0 && gama >= 0 && alpha >= 0) {
			if (beta <= 1 && gama <= 1 && alpha <= 1) {
				return true;
			}
		}
		*scale_t = -1;
	}
	return false;
}

int Ray::IntersectDetect(InputProcessor& ipp, float& scale_t, Point3& i_point, int& obj_idx) {
	std::vector<Sphere> sphere_vector = ipp.GetSphereVector();
	std::vector<Triangle> triangle_vector = ipp.GetTriangleArr();
	float min_time1 = -1;
	int sphere_obj_idx = -1;
	Point3 front_intersect_pt = Point3(0, 0, 0);
	for (int i = 0; i < sphere_vector.size(); i++) {
		float iTime = 0.0f;
		Point3 iPoint = Point3(0, 0, 0);
		Sphere sphere_ = sphere_vector.at(i);
		if (IntersectSphere(sphere_.GetOrigin(), sphere_.GetRadius(), &iTime, &iPoint)) {
			// The object will be displayed only if there are at the back of the viewing window
			//if (time >= std_t) {
			if (min_time1 == -1) {
				min_time1 = iTime;
				front_intersect_pt = iPoint;
				sphere_obj_idx = i;
			}
			if (iTime < min_time1 && i > 0) {
				min_time1 = iTime;
				front_intersect_pt = iPoint;
				sphere_obj_idx = i;
			}
		}
	}

	float min_time2 = -1;
	int triangle_obj_idx = -1;
	Point3 front_intersect_pt2 = Point3(0, 0, 0);
	int idx0 = -1, idx1 = -1, idx2 = -1;
	for (int j = 0; j < ipp.GetTriangleArr().size(); j++) {
		float iTime = 0.0f;
		Point3 iPoint = Point3(0, 0, 0);
		Triangle triangle_ = triangle_vector.at(j);
		Point3 p0 = triangle_.GetVertexP0();
		Point3 p1 = triangle_.GetVertexP1();
		Point3 p2 = triangle_.GetVertexP2();
		if (IntersectTriangle(p0, p1, p2, &iTime, &iPoint)) {
			if (min_time2 == -1) {
				min_time2 = iTime;
				front_intersect_pt2 = iPoint;
				triangle_obj_idx = j;
			}
			if (iTime < min_time2 && j > 0) {
				min_time2 = iTime;
				front_intersect_pt2 = iPoint;
				triangle_obj_idx = j;
			}
		}
	}
	if (min_time1 == -1 && min_time2 == -1) {
		return -1;
	}
	else {
		// no intersect with sphere, simply triangle
		if (min_time1 == -1 && min_time2 > 0) {
			// front_intersect_pt = front_intersect_pt2;
			i_point = front_intersect_pt2;
			scale_t = min_time2;
			// intersect triangle
			obj_idx = triangle_obj_idx;
			return 2;
		}
		// no intersect with triangle, simply sphere
		else if (min_time1 > 0 && min_time2 == -1) {
			i_point = front_intersect_pt;
			scale_t = min_time1;
			obj_idx = sphere_obj_idx;
			return 1;
		}
		// intersect with both sphere and triangle
		else if (min_time1 >= 0 && min_time2 >= 0) {
			if (min_time2 < min_time1) {
				// front_intersect_pt = front_intersect_pt2;
				i_point = front_intersect_pt2;
				scale_t = min_time2;
				// intersect triangle
				obj_idx = triangle_obj_idx;
				return 2;
			}
			else {
				// intersect sphere
				i_point = front_intersect_pt;
				scale_t = min_time1;
				obj_idx = sphere_obj_idx;
				return 1;
			}
		}
	}
}

void Ray::set(Point3 newOrigin, Vector3 newDir) {
	origin_ = newOrigin;
	dir_ = newDir;
}

void Ray::SetRayEta(float eta) {
	ray_through_eta = eta;
}

void Ray::SetRefractT(float re_t) {
	refract_t = re_t;
}

void Ray::SetMaxDepth(int d) {
	max_depth = d;
}

/*
eye 0.0 0.0 8.0
viewdir 0.0 0.0 -1.0
updir 0 1 0
vfov 60
imsize 800 800

bkgcolor 0 0 0

attlight 0 0 -1 1 1 1 1 1 0.5 0.1

mtlcolor 1 1 0 1 1 1 0.1 0.9 0 22
sphere -4 0 -7.5 3.0

mtlcolor 0 1 0 1 1 1 0.1 0.8 0.6 22
sphere 4 0 -5 3


*/