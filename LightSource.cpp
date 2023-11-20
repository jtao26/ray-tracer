#include "LightSource.h"
#include "Point3.h"
#include "Vector3.h"

#define MATH_DIV_OFFSET 1e-8

LightSource::LightSource() {
	point_light_origin_ = Point3(0, 0, 0);
	directional_light_dir_ = Vector3(0, 0, 0);
	color_ = Color(1, 1, 1);
	type_mark_ = 1;
	
}

LightSource::LightSource(int light_type, Point3 origin) {
	type_mark_ = light_type;
	point_light_origin_ = origin;
	color_ = Color(1, 1, 1);
}

LightSource::LightSource(int light_type, Vector3 dir) {
	type_mark_ = light_type;
	directional_light_dir_ = dir;
	color_ = Color(1, 1, 1);
}

LightSource::LightSource(int light_type, Point3 origin, Color color) {
	type_mark_ = light_type;
	point_light_origin_ = origin;
	color_ = color;
}

LightSource::LightSource(int light_type, Vector3 dir, Color color) {
	type_mark_ = light_type;
	directional_light_dir_ = dir;
	color_ = color;
}

LightSource::~LightSource() {

}

void LightSource::SetLightSource(Point3 origin_) {
	type_mark_ = 1;
	point_light_origin_ = origin_;
	color_ = Color(1, 1, 1);
	att_var[0] = -1;
	att_var[1] = -1;
	att_var[2] = -1;
}

void LightSource::SetLightSource(Point3 origin_, Color color_) {
	type_mark_ = 1;
	point_light_origin_ = origin_;
	this->color_ = color_;
	att_var[0] = -1;
	att_var[1] = -1;
	att_var[2] = -1;
}

void LightSource::SetLightSource(Vector3 direction_) {
	type_mark_ = 0;
	directional_light_dir_ = direction_;
	color_ = Color(1, 1, 1);
	att_var[0] = -1;
	att_var[1] = -1;
	att_var[2] = -1;
}

void LightSource::SetLightSource(Vector3 direction_, Color color_) {
	type_mark_ = 0;
	directional_light_dir_ = direction_;
	this->color_ = color_;
	att_var[0] = -1;
	att_var[1] = -1;
	att_var[2] = -1;
}

void LightSource::SetAttPtLight(Point3 origin_, Color color_, float c0, float c1, float c2) {
	type_mark_ = 1;
	point_light_origin_ = origin_;
	this->color_ = color_;
	att_var[0] = c0;
	att_var[1] = c1;
	att_var[2] = c2;
}

bool LightSource::operator==(const LightSource& l) const {
	if (type_mark_ == l.type_mark_) {
		if (type_mark_ == 0) {
			return directional_light_dir_ == l.directional_light_dir_ && color_ == l.color_;
		}
		else {
			return point_light_origin_ == l.point_light_origin_ && color_ == l.color_;
		}
	}
	return false;
}

bool LightSource::operator!=(const LightSource& l) const {
	if (type_mark_ == l.type_mark_) {
		if (type_mark_ == 0) {
			return directional_light_dir_ != l.directional_light_dir_ || color_ != l.color_;
		}
		else {
			return point_light_origin_ != l.point_light_origin_ || color_ != l.color_;
		}
	}
	return false;
}

LightSource& LightSource::operator=(const LightSource& l) {
	type_mark_ = l.type_mark_;
	color_ = l.color_;
	if (type_mark_ == 1) {
		point_light_origin_ = l.point_light_origin_;
		directional_light_dir_ = Vector3(0, 0, 0);
	}
	else if (type_mark_ == 0) {
		directional_light_dir_ = l.directional_light_dir_;
		point_light_origin_ = Point3(0, 0, 0);
	}
	return *this;
}