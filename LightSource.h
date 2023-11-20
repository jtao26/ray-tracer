#ifndef _LIGHTSOURCE_H_
#define _LIGHTSOURCE_H_

#include "Point3.h"
#include "Vector3.h"
#include "Color.h"

struct LightSource {
public:
	LightSource();

	LightSource(int light_type, Point3 origin);

	LightSource(int light_type, Vector3 dir);

	LightSource(int light_type, Point3 origin, Color color);

	LightSource(int light_type, Vector3 dir, Color color);

	virtual ~LightSource();

	void SetLightSource(Point3 origin_);

	void SetLightSource(Point3 origin_, Color color_);

	void SetLightSource(Vector3 direction_);

	void SetLightSource(Vector3 direction_, Color color_);

	void SetAttPtLight(Point3 origin_, Color color_, float c0, float c1, float c2);

	Point3 GetPointLtOrigin() {	return point_light_origin_;	}

	Vector3 GetDirLtDirection() { return directional_light_dir_; }

	int GetW() { return type_mark_; }

	Color GetLtColor() { return color_; }

	float* GetAttVar() { return att_var; }

	int GetTypeMark() { return type_mark_; }

	bool operator==(const LightSource& p) const;

	bool operator!=(const LightSource& p) const;

	LightSource& operator=(const LightSource& p);


private:
	std::string type_name_;
	Point3 point_light_origin_;
	Vector3 directional_light_dir_;
	Color color_;
	float att_var[3];
	int type_mark_;
};



#endif // !_LIGHTSOURCE_H_


