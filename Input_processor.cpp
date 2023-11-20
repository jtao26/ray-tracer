#include "Input_processor.h"
#include "Point3.h"
#include "Vector3.h"
#include <iostream>
#include <fstream>
#include <regex>

void eliminateComment(std::string& str) {
	std::size_t s1 = str.find("#");
	if (s1 == std::string::npos) {
		return;
	}
	else {
		str = str.substr(0, s1);
	}
}

void eliminateSpace(std::string& str) {
	while (true){
		if (str.substr(0, 1) == " ") {
			str = str.substr(1);
		}
		if (str.substr(str.length() - 1) == " ") {
			str = str.substr(0, str.length() - 1);
		}
		else {
			break;
		}
	}
	return;
}

void InputProcessor::processVertexInput(std::string str, std::string& ans1, std::string& ans2, std::string& ans3) {
	std::size_t s1 = str.find("/");
	if (s1 == std::string::npos) {
		// did not found any
		ans1 = str;
		ans2 = "None";
		ans3 = "None";
		return;
	}
	else {
		ans1 = str.substr(0, s1);
		if (str[s1 + 1] == '/') {
			// no texure info, assign vertex normal
			ans2 = "None";
			ans3 = str.substr(s1 + 2);
			if (ans3 == "") {
				ans3 = "None";
			}
		}
		else {
			std::string str1 = str.substr(s1 + 1);
			std::size_t s2 = str1.find("/");
			ans2 = str1.substr(0, s2);
			ans3 = str1.substr(s2 + 1);
		}
	}
	return;
}

bool isPosNumber(std::string& str) {
	if (str == "") {
		return false;
	}
	else if (str[0] == '.') {
		std::string zero = "0";
		str = zero + str;
	}
	return std::regex_match(str, std::regex(("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
}

InputProcessor::InputProcessor() {
	fov = 0;
	image_size[0] = 0;
	image_size[1] = 0;
	ka_ = 0;
	kd_ = 0;
	ks_ = 0;
	n_ = 0;
	alpha_ = -1;
	eta_ = -1;
	max_depth = 1;
	view_distance = 5.0f;
	flag_depth_of_field = false;
}

InputProcessor::~InputProcessor() {
	if (texture_flag == 1) {
		for (int i = 0; i < texture_height; i++) {
			delete[] texture_color[i];
		}
		delete texture_color;
	}
	
}

void InputProcessor::OutputStoredData() {
	std::cout << GetEyePos() << std::endl;
	std::cout << GetViwDir() << std::endl;
	std::cout << GetUpDir() << std::endl;
	std::cout << GetFov() << std::endl;
	std::cout << "width: " << GetImageSize()[0] << ", height: " << GetImageSize()[1] << std::endl;
	std::cout << GetBgColor() << std::endl;
	std::cout << GetIppMtColor() << std::endl;
	std::cout << GetSphereVector().at(0) << std::endl;
	return;
}

void InputProcessor::ReadDataFromFile(const std::string& filename) {
	std::cout << "================= start process input =================" << std::endl;
	std::ifstream in_stream(filename, std::ios::in);
	int end_cnt = 0;
	if (!in_stream.is_open()) {
		return;
	}
	else {
		std::string content;
		while (true) {
			content = "";
			getline(in_stream, content);
			// in_stream >> keyword;
			eliminateComment(content);
			if (content == "") {
				end_cnt++;
				if (end_cnt >= 8) {
					std::cout << "reach the end of file" << std::endl;
					break;
				}
			}
			else {
				end_cnt = 0;
				std::cout << "read in: " << content << std::endl;
				CheckFileContent(content, in_stream);
			}
		}
		
	}
	in_stream.close();
}

int InputProcessor::CheckFileContent(const std::string read_keyword, std::ifstream& in_stream) {

	std::size_t s1 = read_keyword.find(" ");
	std::string key = read_keyword.substr(0, s1);
	std::string content = read_keyword.substr(s1+1);
	eliminateSpace(content);
	if (key == "eye") {
		std::string eye[3] = { "", "", "" };
		for (int i = 0; i < 3; i++) {
			std::size_t loc = content.find(" ");
			eye[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		// in_stream >> eye_x >> eye_y >> eye_z;
		if (isPosNumber(eye[0]) && isPosNumber(eye[1]) && isPosNumber(eye[2])) {
			SetEyePos(std::stof(eye[0]), std::stof(eye[1]), std::stof(eye[2]));
		}
		else {
			std::cout << "[ERROR] Incorrect or missing eye position corrdinates data." << std::endl;
			throw std::invalid_argument("Invalid eye position argument.");
		}
	}
	else if (key == "viewdir") {
		std::string viewdir[3] = { "", "", "" };
		for (int i = 0; i < 3; i++) {
			std::size_t loc = content.find(" ");
			viewdir[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		if (isPosNumber(viewdir[0]) && isPosNumber(viewdir[1]) && isPosNumber(viewdir[2])) {
			SetViwDir(std::stof(viewdir[0]), std::stof(viewdir[1]), std::stof(viewdir[2]));
		
		}
		else {
			std::cout << "[ERROR] Incorrect or missing view direction data." << std::endl;
			throw std::invalid_argument("Invalid view direction argument.");
		}
	}
	else if (key == "updir") {
		std::string updir[3] = { "", "", "" };
		for (int i = 0; i < 3; i++) {
			std::size_t loc = content.find(" ");
			updir[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		if (isPosNumber(updir[0]) && isPosNumber(updir[1]) && isPosNumber(updir[2])) {
			SetUpDir(std::stof(updir[0]), std::stof(updir[1]), std::stof(updir[2]));
		}
		else {
			std::cout << "[ERROR] Incorrect or missing up direction data." << std::endl;
			throw std::invalid_argument("Invalid up direction argument.");
		}
	}
	else if (key == "vfov") {
		std::string vfov = "";
		vfov = content;
		if (isPosNumber(vfov)) {
			if (std::stof(vfov) > 180) {
				std::cout << "[ERROR] Invalid fov value, too large." << std::endl;
				}
			else {
				// fov = std::stof(view_fov);
				SetFov(std::stof(vfov));
			}
		}
		else {
			std::cout << "[ERROR] Incorrect or missing fov data." << std::endl;
			throw std::invalid_argument("Invalid vfov argument.");
		}
	}
	else if (key == "imsize") {
		std::string imsize[2] = { "", "" };
		for (int i = 0; i < 2; i++) {
			std::size_t loc = content.find(" ");
			imsize[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		if (isPosNumber(imsize[0]) && isPosNumber(imsize[1])) {
			if (std::stoi(imsize[0]) * std::stoi(imsize[1]) == 0) {
				throw std::invalid_argument("Screen size can not be zero.");
			}
			SetImageSize(std::stoi(imsize[0]), std::stoi(imsize[1]));
		}
		else {
			std::cout << "[ERROR] Incorrect or missing image size data." << std::endl;
			throw std::invalid_argument("Invalid image size data.");
		}
	}
	else if (key == "bkgcolor") {
		std::string color[3] = { "", "", "" };
		for (int i = 0; i < 3; i++) {
			std::size_t loc = content.find(" ");
			color[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		std::string red = color[0], green = color[1], blue = color[2];
		if (isPosNumber(red) && isPosNumber(green) && isPosNumber(blue)) {
			if (std::stof(red) > 1.0f || std::stof(red) < 0.0f ||
				std::stof(green) > 1.0f || std::stof(green) < 0.0f ||
				std::stof(blue) > 1.0f || std::stof(blue) < 0.0f) {
				std::cout << "[ERROR] Enter color value between 0.0 - 1.0." << std::endl;
			}
			else {
				SetBgColor(std::stof(red), std::stof(green), std::stof(blue));
			}
			
		}
		else {
			std::cout << "[ERROR] Incorrect or missing background color data." << std::endl;
			throw std::invalid_argument("Invalid background color argument.");
		}
	}
	else if (key == "mtlcolor") {
		std::string color[12] = { "", "", "", "", "", "", "", "", "", "", "", ""};
		for (int i = 0; i < 12; i++) {
			std::size_t loc = content.find(" ");
			if(loc == std::string::npos && content != "") {
				color[i] = content;
				break;
			}
			else {
				color[i] = content.substr(0, loc);
				content = content.substr(loc + 1);
			}
			if (content == "") {
				break;
			}
		}
		std::string red = color[0], green = color[1], blue = color[2];
			if (isPosNumber(red) && isPosNumber(green) && isPosNumber(blue)) {
			if (std::stof(red) > 1.0f || std::stof(red) < 0.0f ||
				std::stof(green) > 1.0f || std::stof(green) < 0.0f ||
				std::stof(blue) > 1.0f || std::stof(blue) < 0.0f) {
				std::cout << "[ERROR] Enter color value between 0.0 - 1.0." << std::endl;
			}
			else {
				SetMtColor(std::stof(red), std::stof(green), std::stof(blue));
				red = color[3], green = color[4], blue = color[5];
				if (isPosNumber(red) && isPosNumber(green) && isPosNumber(blue)) {
					if (std::stof(red) > 1.0f || std::stof(red) < 0.0f ||
						std::stof(green) > 1.0f || std::stof(green) < 0.0f ||
						std::stof(blue) > 1.0f || std::stof(blue) < 0.0f) {
						std::cout << "[ERROR] Enter color value between 0.0 - 1.0." << std::endl;
					}
					else {
						SetSpColor(std::stof(red), std::stof(green), std::stof(blue));
						std::string ka = color[6], kd = color[7], ks = color[8];
						if (isPosNumber(ka) && isPosNumber(kd) && isPosNumber(ks)) {
							if (std::stof(ka) < 0.0f || std::stof(kd) < 0.0f || std::stof(ks) < 0.0f) {
								std::cout << "[ERROR] Enter pisitive k value." << std::endl;
								throw std::invalid_argument("Invalid material color argument.");
							}
							else {
								SetKaMultiplier(std::stof(ka));
								SetKdMultiplier(std::stof(kd));
								SetKsMultiplier(std::stof(ks));
								std::string n = color[9];
								if (isPosNumber(n)) {
									if (std::stoi(n) >= 0) {
										SetN(std::stoi(n));
										if (color[10] != "") {
											std::string alpha = color[10];
											std::string eta = color[11];
											if (isPosNumber(alpha) && isPosNumber(eta)) {
												SetAlpha(std::stof(alpha));
												SetEta(std::stof(eta));
											}
											else {
												std::cout << "[ERROR] Enter pisitive alpha/eta value." << std::endl;
												throw std::invalid_argument("Invalid material color argument.");
											}
										}
										else {
											SetAlpha(-1);
											SetEta(-1);
										}
									}
									else {
										std::cout << "[ERROR] Enter pisitive k value." << std::endl;
										throw std::invalid_argument("Invalid material color argument.");
									}
								}
							}
						}
						else {
							std::cout << "[ERROR] Incorrect or missing material color data." << std::endl;
							throw std::invalid_argument("Invalid material color argument.");
						}
					}
				}
				else {
					std::cout << "[ERROR] Incorrect or missing material color data." << std::endl;
					throw std::invalid_argument("Invalid material color argument.");
				}
			}
		}
		else {
			std::cout << "[ERROR] Incorrect or missing material color data." << std::endl;
			throw std::invalid_argument("Invalid material color argument.");
		}
	}
	// normal point or dir light
	else if (key == "light") {
		std::string light[7] = { "", "", "", "", "", "", "" };
		for (int i = 0; i < 7; i++) {
			std::size_t loc = content.find(" ");
			light[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		std::string l_x = light[0], l_y = light[1], l_z = light[2], w = light[3], r = light[4], g = light[5], b = light[6];
		if (isPosNumber(l_x) && isPosNumber(l_y) && isPosNumber(l_z) && isPosNumber(w)
				&& isPosNumber(r) && isPosNumber(g) && isPosNumber(b)) {
			SetLightSource(std::stof(l_x), std::stof(l_y), std::stof(l_z), 
				std::stoi(w), std::stof(r), std::stof(g), std::stof(b));
		}
	}
	// light attenuation
	else if (key == "attlight") {
		std::string attlight[10] = { "", "", "", "", "", "", "", "", "", "" };
		for (int i = 0; i < 10; i++) {
			std::size_t loc = content.find(" ");
			attlight[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		std::string l_x = attlight[0], l_y = attlight[1], l_z = attlight[2], w = attlight[3],
			r = attlight[4], g = attlight[5], b = attlight[6], c1 = attlight[7], c2 = attlight[8], c3 = attlight[9];
		if (isPosNumber(l_x) && isPosNumber(l_y) && isPosNumber(l_z) && isPosNumber(w)
			&& isPosNumber(r) && isPosNumber(g) && isPosNumber(b)
			&& isPosNumber(c1) && isPosNumber(c2) && isPosNumber(c3)) {
			SetAttPtLightSource(std::stof(l_x), std::stof(l_y), std::stof(l_z),
				std::stoi(w), std::stof(r), std::stof(g), std::stof(b),
				std::stof(c1), std::stof(c2), std::stof(c3));
		}
	}
	// verticies
	else if (key == "v") {
		std::string v[3] = { "", "", "" };
		for (int i = 0; i < 3; i++) {
			std::size_t loc = content.find(" ");
			v[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
			eliminateSpace(content);
		}
		std::string x_ = v[0], y_ = v[1], z_ = v[2];
		if (isPosNumber(x_) && isPosNumber(y_) && isPosNumber(z_)) {
			// color process go within the function
			AddVerticiesToArray(std::stof(x_), std::stof(y_), std::stof(z_));
		}
	}
	// vertex normal
	else if (key == "vn") {
		std::string vn[3] = { "", "", "" };
		for (int i = 0; i < 3; i++) {
			std::size_t loc = content.find(" ");
			vn[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		std::string vn1 = vn[0], vn2 = vn[1], vn3 = vn[2];
		if (isPosNumber(vn1) && isPosNumber(vn2) && isPosNumber(vn3)) {
			AddVertexNormalToArray(std::stof(vn1), std::stof(vn2), std::stof(vn3));
		}
	}
	// vertex texture arr
	else if (key == "vt") {
		std::string vt[2] = { "", "" };
		for (int i = 0; i < 2; i++) {
			std::size_t loc = content.find(" ");
			vt[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		std::string vt0 = vt[0], vt1 = vt[1];
		if (isPosNumber(vt0) && isPosNumber(vt1)) {
			AddVertexTextureToArray(std::stoi(vt0), std::stoi(vt1));
		}
	}
	// verticies face idx
	else if (key == "f") {
		std::string v1 = "", v2 = "", v3 = "";
		std::string vt1 = "", vt2 = "", vt3 = "";
		std::string vn1 = "", vn2 = "", vn3 = "";
		std::string str1 = "", str2 = "", str3 = "";
		std::string f_block[3] = { "", "", "" };
		for (int i = 0; i < 3; i++) {
			std::size_t space_pos = content.find(" ");
			f_block[i] = content.substr(0, space_pos);
			content = content.substr(space_pos + 1);
		}
		std::size_t loc1 = f_block[0].find("/");
		if (loc1 == std::string::npos) {
			// only vertex inputed
			v1 = f_block[0];
			v2 = f_block[1];
			v3 = f_block[2];
			vt1 = "None";
			vt2 = "None";
			vt3 = "None";
			vn1 = "None";
			vn2 = "None";
			vn3 = "None";
		}
		else {
			// find one "/", seek for second "/" position
			// vt vn exists
			std::size_t loc2 = f_block[0].substr(loc1 + 1).find("/");
			if (loc2 == std::string::npos) {
				// only vt exist
				// 1/2
				std::size_t loc = f_block[0].find("/");
				v1 = f_block[0].substr(0, loc);
				vt1 = f_block[0].substr(loc + 1);
				vn1 = "None";
				loc = f_block[1].find("/");
				v2 = f_block[1].substr(0, loc);
				vt2 = f_block[1].substr(loc + 1);
				vn2 = "None";
				loc = f_block[2].find("/");
				v3 = f_block[2].substr(0, loc);
				vt3 = f_block[2].substr(loc + 1);
				vn3 = "None";
			}
			else if (loc2 == 0) {
				// two "/" next to each other
				// no vt exist
				// 1//3
				std::size_t loc = f_block[0].find("/");
				v1 = f_block[0].substr(0, loc);
				vt1 = "None";
				vn1 = f_block[0].substr(loc + 2);
				loc = f_block[1].find("/");
				v2 = f_block[1].substr(0, loc);
				vt2 = "None";
				vn2 = f_block[1].substr(loc + 2);
				loc = f_block[2].find("/");
				v3 = f_block[2].substr(0, loc);
				vt3 = "None";
				vn3 = f_block[2].substr(loc + 2);
			}
			else {
				// both vt vn appear
				// 1/2/3
				std::size_t loc = f_block[0].find("/");
				std::size_t loc2 = f_block[0].substr(loc + 1).find("/");
				v1 = f_block[0].substr(0, loc);
				vt1 = f_block[0].substr(loc + 1, loc2);
				vn1 = f_block[0].substr(loc + 2);
				loc = f_block[1].find("/");
				loc2 = f_block[1].substr(loc + 1).find("/");
				v2 = f_block[1].substr(0, loc);
				vt1 = f_block[1].substr(loc + 1, loc2);
				vn2 = f_block[1].substr(loc + 2);
				loc = f_block[2].find("/");
				loc2 = f_block[2].substr(loc + 1).find("/");
				v3 = f_block[2].substr(0, loc);
				vt1 = f_block[2].substr(loc + 1, loc2);
				vn3 = f_block[2].substr(loc + 2);
			}
		}
		if (vn1 != "None" && vt1 == "None" && isPosNumber(v1) && isPosNumber(v2) && isPosNumber(v3)) {
			// have vertex normal but without texture
			AddVertexNormIdxToArray(std::stoi(vn1), std::stoi(vn2), std::stoi(vn3));
			AddVerticiesIdxToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3));
			AddTriangleToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3), 
				std::stoi(vn1), std::stoi(vn2), std::stoi(vn3), "normal");
		}
		else if (vn1 == "None" && vt1 != "None" && isPosNumber(v1) && isPosNumber(v2) && isPosNumber(v3)) {
			// have vertex texture but without vertex normal
			AddVertexTextureIdxToArray(std::stoi(vt1), std::stoi(vt2), std::stoi(vt3));
			AddVerticiesIdxToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3));
			AddTriangleToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3),
				std::stoi(vt1), std::stoi(vt2), std::stoi(vt3), "texture");
		}
		else if (vn1 != "None" && vt1 != "None" && isPosNumber(v1) && isPosNumber(v2) && isPosNumber(v3)) {
			// have both vertex normal and texture
			AddVerticiesIdxToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3));
			AddVertexTextureIdxToArray(std::stoi(vt1), std::stoi(vt2), std::stoi(vt3));
			AddVertexNormIdxToArray(std::stoi(vn1), std::stoi(vn2), std::stoi(vn3));
			AddTriangleToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3), 
				std::stoi(vt1), std::stoi(vt2), std::stoi(vt3),
				std::stoi(vn1), std::stoi(vn2), std::stoi(vn3));
		}
		else if (vn1 == "None" && vt1 == "None" && isPosNumber(v1) && isPosNumber(v2) && isPosNumber(v3)) {
			AddVerticiesIdxToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3));
			AddTriangleToArray(std::stoi(v1), std::stoi(v2), std::stoi(v3));
		}
		
	}
	else if (key == "texture") {
		SetTexturePpm(content);
	}
	// read in sphere
	else if (key == "sphere") {
		std::string sphere[4] = { "", "", "", "" };
		for (int i = 0; i < 4; i++) {
			std::size_t loc = content.find(" ");
			sphere[i] = content.substr(0, loc);
			content = content.substr(loc + 1);
		}
		std::string cor_x = sphere[0], cor_y = sphere[1], cor_z = sphere[2], radius = sphere[3];
		if (isPosNumber(cor_x) && isPosNumber(cor_y) && isPosNumber(cor_z) && isPosNumber(radius)) {
			SetSphereData(std::stof(cor_x), std::stof(cor_y), std::stof(cor_z), std::stof(radius));
		}
		else {
			std::cout << "[ERROR] Incorrect or missing background color data." << std::endl;
			throw std::invalid_argument("Invalid Sphere data.");
		}
	}
	else if (key == "tracedepth") {
		int depth = std::stoi(content);
		SetMaxDepth(depth);
	}
	else if (key == "viewdist") {
		float distance = std::stof(content);
		SetViewDist(distance);
		SetFlagDepthOfView(true);
	}
	return 0;
}

void InputProcessor::SetEyePos(float x_, float y_, float z_) {
	eye = Point3(x_, y_, z_);
}

void InputProcessor::SetViwDir(float x_, float y_, float z_) {
	view_dir = Vector3(x_, y_, z_);
	view_dir.Normalize();
}

void InputProcessor::SetUpDir(float x_, float y_, float z_) {
	up_dir = Vector3(x_, y_, z_);
	up_dir.Normalize();
}

void InputProcessor::SetFov(float vfov_) {
	fov = vfov_;
}

void InputProcessor::SetImageSize(int width, int height) {
	image_size[0] = width;
	image_size[1] = height;
}

void InputProcessor::SetBgColor(float red, float green, float blue) {
	bg_color = Color(red, green, blue);
}

void InputProcessor::SetMtColor(float red, float green, float blue) {
	material_color = Color((float)red, (float)green, (float)blue);
}

void InputProcessor::SetSpColor(float red, float green, float blue) {
	specular_color = Color((float)red, (float)green, (float)blue);
}

void InputProcessor::SetLightSource(float x_, float y_, float z_, float w, float red, 
	float green, float blue) {
	if (w == 1) {
		// point light
		LightSource lt;
		lt.SetLightSource(Point3(x_, y_, z_), Color(red, green, blue));
		light_arr.push_back(lt);
	}
	else if (w == 0) {
		// directional light
		LightSource lt;
		lt.SetLightSource(Vector3(x_, y_, z_), Color(red, green, blue));
		light_arr.push_back(lt);
	}
}

void InputProcessor::SetAttPtLightSource(float x_, float y_, float z_, float w, float red, 
	float green, float blue, float c1, float c2, float c3) {
		LightSource lt;
		lt.SetAttPtLight(Point3(x_, y_, z_), Color(red, green, blue), c1, c2, c3);
		light_arr.push_back(lt);
}

void InputProcessor::SetKaMultiplier(float ka) {
	ka_ = ka;
}

void InputProcessor::SetKdMultiplier(float kd) {
	kd_ = kd;
}

void InputProcessor::SetKsMultiplier(float ks) {
	ks_ = ks;
}

void InputProcessor::SetFlagLightAtt() {
	flag_light_attenuation = 1;
}

void InputProcessor::SetFlagSoftShadow() {
	flag_soft_shadow = 1;
}

void InputProcessor::SetN(int n) {
	n_ = n;
}

void InputProcessor::SetAlpha(float alpha) {
	alpha_ = alpha;
}

void InputProcessor::SetEta(float eta) {
	eta_ = eta;
}

void InputProcessor::AddVerticiesToArray(float x_, float y_, float z_) {
	vertex_arr.push_back(Point3(x_, y_, z_));
}

void InputProcessor::AddVerticiesIdxToArray(int v1, int v2, int v3) {
	vertex_idx_arr.push_back(v1 - 1);
	vertex_idx_arr.push_back(v2 - 1);
	vertex_idx_arr.push_back(v3 - 1);
}

void InputProcessor::AddTriangleToArray(int v1, int v2, int v3) {
	Point3 p1 = GetVertexArr().at(v1 - 1);
	Point3 p2 = GetVertexArr().at(v2 - 1);
	Point3 p3 = GetVertexArr().at(v3 - 1);
	Triangle *t = &Triangle(p1, p2, p3);
	t->SetTriangleMtColor(GetIppMtColor());
	t->SetTriangleSpColor(GetIppSpColor());
	t->SetKa(GetKaMultiplier());
	t->SetKd(GetKdMultiplier());
	t->SetKs(GetKsMultiplier());
	t->SetN(GetN());
	t->SetTriangleNormal(t->CalculateTriPlaneNorm());

	if (GetAlpha() != -1) {
		t->SetAlpha(GetAlpha());
		t->SetEta(GetEta());
	}
	
	this->triangle_arr.push_back(*t);
}

void InputProcessor::AddTriangleToArray(int v1, int v2, int v3, int vnt1, int vnt2, int vnt3, std::string typ) {
	Point3 p1 = GetVertexArr().at(v1 - 1);
	Point3 p2 = GetVertexArr().at(v2 - 1);
	Point3 p3 = GetVertexArr().at(v3 - 1);
	Triangle* t = &Triangle(p1, p2, p3);
	if (typ == "normal") {
		Vector3 vn0 = GetVertexNormalArr().at(vnt1 - 1);
		Vector3 vn1 = GetVertexNormalArr().at(vnt2 - 1);
		Vector3 vn2 = GetVertexNormalArr().at(vnt3 - 1);
		t->SetVertexNormal(vn0, vn1, vn2);
	}
	else if (typ == "texture") {
		Point3 vt0 = GetVertexTextureArr().at(vnt1 - 1);
		Point3 vt1 = GetVertexTextureArr().at(vnt2 - 1);
		Point3 vt2 = GetVertexTextureArr().at(vnt3 - 1);
		t->SetTexture(GetTexture(), GetTextureFlag());
		t->SetTextureW(GetTextureW());
		t->SetTextureH(GetTextureH());
		t->SetVertexTextureCoord(vt0, vt1, vt2); // set up triangle so taht it can handle texture like sphere
	}
	t->SetTriangleMtColor(GetIppMtColor());
	t->SetTriangleSpColor(GetIppSpColor());
	t->SetKa(GetKaMultiplier());
	t->SetKd(GetKdMultiplier());
	t->SetKs(GetKsMultiplier());
	t->SetN(GetN());
	t->SetTriangleNormal(t->CalculateTriPlaneNorm());
	if (GetAlpha() != -1) {
		t->SetAlpha(GetAlpha());
		t->SetEta(GetEta());
	}
	this->triangle_arr.push_back(*t);
}

void InputProcessor::AddTriangleToArray(int v1, int v2, int v3, int vt1, int vt2, int vt3, int vn1, int vn2, int vn3) {
	Point3 p1 = GetVertexArr().at(v1 - 1);
	Point3 p2 = GetVertexArr().at(v2 - 1);
	Point3 p3 = GetVertexArr().at(v3 - 1);
	Triangle* t = &Triangle(p1, p2, p3);
	Vector3 V0 = GetVertexNormalArr().at(vn1 - 1);
	Vector3 V1 = GetVertexNormalArr().at(vn2 - 1);
	Vector3 V2 = GetVertexNormalArr().at(vn3 - 1);
	t->SetVertexNormal(V0, V1, V2);
	Point3 Vt0 = GetVertexTextureArr().at(vt1 - 1);
	Point3 Vt1 = GetVertexTextureArr().at(vt2 - 1);
	Point3 Vt2 = GetVertexTextureArr().at(vt3 - 1);
	t->SetTexture(GetTexture(), GetTextureFlag());
	t->SetTextureW(GetTextureW());
	t->SetTextureH(GetTextureH());
	t->SetVertexTextureCoord(Vt0, Vt1, Vt2);

	t->SetTriangleMtColor(GetIppMtColor());
	t->SetTriangleSpColor(GetIppSpColor());
	t->SetKa(GetKaMultiplier());
	t->SetKd(GetKdMultiplier());
	t->SetKs(GetKsMultiplier());
	t->SetN(GetN());
	t->SetTriangleNormal(t->CalculateTriPlaneNorm());
	if (GetAlpha() != -1) {
		t->SetAlpha(GetAlpha());
		t->SetEta(GetEta());
	}
	this->triangle_arr.push_back(*t);
}

void InputProcessor::AddVertexNormalToArray(float vn1, float vn2, float vn3) {
	vertex_norm_arr.push_back(Vector3(vn1, vn2, vn3));
}

void InputProcessor::AddVertexNormIdxToArray(int vn1, int vn2, int vn3) {
	vertex_norm_idx_arr.push_back(vn1 - 1);
	vertex_norm_idx_arr.push_back(vn2 - 1);
	vertex_norm_idx_arr.push_back(vn3 - 1);
}

void InputProcessor::AddVertexTextureToArray(int vt1, int vt2) {
	vertex_texture_arr.push_back(Point3(vt1, vt2, 0));
}
void InputProcessor::AddVertexTextureIdxToArray(int vt1, int vt2, int vt3) {
	vertex_texture_idx_arr.push_back(vt1 - 1);
	vertex_texture_idx_arr.push_back(vt2 - 1);
	vertex_texture_idx_arr.push_back(vt3 - 1);
}

void InputProcessor::SetSphereData(float x_, float y_, float z_, float radius) {
	Sphere s = Sphere(x_, y_, z_, radius);
	s.SetMtColor(GetIppMtColor());
	s.SetSpColor(GetIppSpColor());
	s.SetKaMultiplier(GetKaMultiplier());
	s.SetKdMultiplier(GetKdMultiplier());
	s.SetKsMultiplier(GetKsMultiplier());
	s.SetN(GetN());
	if (texture_flag == 1) {
		s.SetTexture(GetTexture());
		s.SetTextureW(GetTextureW());
		s.SetTextureH(GetTextureH());
	}
	if (alpha_ != -1) {
		s.SetAlpha(GetAlpha());
		s.SetEta(GetEta());
	}
	sphere_arr.push_back(s);
}

void InputProcessor::SetTexturePpm(std::string filename) {
	std::ifstream texture_in(filename, std::ios::in);
	int end_cnt = 0;
	if (!texture_in.is_open()) {
		return;
	}
	else {
		std::string P = "", textureW = "", textureH = "", limit = "";
		texture_in >> P;
		texture_in >> textureW >> textureH;
		texture_in >> limit;
		int texture_map_size = std::stoi(textureW) * std::stoi(textureH);
		texture_width = std::stoi(textureW);
		texture_height = std::stoi(textureH);
		texture_color = new Color * [texture_height];
		for (int i = 0; i < texture_height; i++) {
			texture_color[i] = new Color[texture_width];
		}
		std::string r = "", g = "", b = "";
		for (int i = 0; i < texture_height; i++) {
			for (int j = 0; j < texture_width; j++) {
				texture_in >> r >> g >> b;
				int r_ = std::stoi(r);
				int g_ = std::stoi(g);
				int b_ = std::stoi(b);
				texture_color[i][j] = Color(r_, g_, b_);
			}
		}
		
	}
	texture_flag = 1;
	texture_in.close();
}

void InputProcessor::SetMaxDepth(int d) {
	max_depth = d;
}

void InputProcessor::SetViewDist(float view_dist) {
	view_distance = view_dist;
}

void InputProcessor::SetFlagDepthOfView(bool val) {
	flag_depth_of_field = val;
}