#include "Input_processor.h"
#include "Ray.h"
#include <cmath>
#include <fstream>
#include <random>

#define MATH_DIV_OFFSET 1e-8

/// <summary>
/// This fucntion checks if the provided output file name is a valid.ppm file.
/// If the file name does not contain '.ppm' the function will add the suffix at the end.
/// If the file name contains suffix but not the correct '.ppm' suffix, the funciton will
/// ask users if they want to auto - change the suffix to '.ppm'.If yes, drop the current
/// suffixand add '.ppm', else ignore itand continue.
/// </summary>
/// <param name="fileName">Setting file name provided by user.</param>
void check_if_out_contain_ppm(std::string& fileName) {
	if (fileName == "-soft" || fileName == "-att") {
		return;
	}
	size_t idx = fileName.find(".");
	if (idx == std::string::npos) {
		std::cout << "Did not found output file suffix, adding '.ppm' automatically." << std::endl;
		fileName += ".ppm";
		return;
	}
	else {
		std::string subFileName = fileName.substr(idx + 1, fileName.length() - idx);
		size_t idx2 = subFileName.find(".");
		if (idx2 != std::string::npos) {
			std::cout << "Illegal filename, '.' is not allowed to be inside a filename." << std::endl;
			std::cout << "Default output file name will be applied." << std::endl;
			fileName = "../default_output.ppm";
			return;
		}
		else {
			std::string fileSuffix;
			fileSuffix = fileName.substr(idx, fileName.length() - idx);
			if (fileSuffix != ".ppm") {
				std::cout << "Provided output file not end with '.ppm', currently is: " << fileName
					<< ", do you want to fix it?" << std::endl;
				std::cout << "enter 'y' to correct it, other to ignore this." << std::endl;
				char ans;
				std::cin >> ans;
				if (ans == 'y') {
					std::string str;
					str = fileName.substr(0, idx);
					str += ".ppm";
					fileName = str;
					return;
				}
				else {
					return;
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {
	InputProcessor input_manage;
	try {
		if (argc == 1) {
			throw std::invalid_argument("Missing ray tracer setting file.");
		}
		input_manage.ReadDataFromFile(argv[1]);
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	for (int i = 0; i < argc; i++) {
		std::string opt = argv[i];
		if (opt == "-att") {
			input_manage.SetFlagLightAtt();
		}
		else if (opt == "-soft") {
			input_manage.SetFlagSoftShadow();
		}
	}

	int width_px = input_manage.GetImageSize()[0];
	int height_px = input_manage.GetImageSize()[1];
	int canvas_size = width_px * height_px;
	if (canvas_size == 0) {
		std::cout << "[ERROR] Either height of width of the image can not be zero." << std::endl;
		return 1;
	}
	else if (canvas_size == 1) {

	}

	// dynamically create an array of pointers of size `width_px`
	// and allocate memory of size `height_px` for each row
	Color** canvas_color = new Color * [width_px];
	for (int i = 0; i < width_px; i++) {
		canvas_color[i] = new Color[height_px];
	}

	float view_distance = input_manage.GetViewDist();
	int sphere_vector_size = input_manage.GetSphereVector().size();
	Point3 eye_pos = input_manage.GetEyePos();
	float vfov_rad = input_manage.GetFov() * 3.14159265359f / 180.0f;
	float view_plane_height = 2 * std::tan(vfov_rad / 2.0f) * view_distance;
	float ratio = (float)input_manage.GetImageSize()[0] / input_manage.GetImageSize()[1];
	float view_plane_width = ratio * view_plane_height;

	std::cout << "=============== building viewing window ===============" << std::endl;
	if (input_manage.GetViwDir().Dot(input_manage.GetUpDir()) == 1
		|| input_manage.GetViwDir().Dot(input_manage.GetUpDir()) == -1) {
		std::cout << "[ERROR] The up vector is parelle with the view direction." << std::endl;
		return 1;
	}

	Vector3 u = input_manage.GetViwDir().Cross(input_manage.GetUpDir());
	Vector3 v = u.Cross(input_manage.GetViwDir());
	Point3 ul = input_manage.GetEyePos() + view_distance * input_manage.GetViwDir() - (view_plane_width / 2.0f) * u + (view_plane_height / 2.0f) * v;
	Point3 ur = input_manage.GetEyePos() + view_distance * input_manage.GetViwDir() + (view_plane_width / 2.0f) * u + (view_plane_height / 2.0f) * v;
	Point3 ll = input_manage.GetEyePos() + view_distance * input_manage.GetViwDir() - (view_plane_width / 2.0f) * u - (view_plane_height / 2.0f) * v;
	Point3 lr = input_manage.GetEyePos() + view_distance * input_manage.GetViwDir() + (view_plane_width / 2.0f) * u - (view_plane_height / 2.0f) * v;
	Vector3 delta_h, delta_v;
	if (input_manage.GetImageSize()[0] - 1 == 0) {
		delta_h = 0;
	}
	else {
		delta_h = Vector3(ur - ul) / (input_manage.GetImageSize()[0] - 1);
	}
	if (input_manage.GetImageSize()[1] - 1 == 0) {
		delta_v = 0;
	}
	else {
		delta_v = Vector3(ll - ul) / (input_manage.GetImageSize()[1] - 1);
	}

	Vector3 delta_c_h = Vector3(ur - ul) / (input_manage.GetImageSize()[0] * 2);
	Vector3 delta_c_v = Vector3(ll - ul) / (input_manage.GetImageSize()[1] * 2);

	std::cout << "==================== tracing color ====================" << std::endl;

	for (int i = 0; i <= input_manage.GetImageSize()[1] - 1; i++) {
		for (int j = 0; j <= input_manage.GetImageSize()[0] - 1; j++) {
			Point3 pixel_upper_left = ul + delta_h * i + delta_v * j;
			Point3 pixel_center = ul + delta_h * j + delta_v * i + delta_c_h + delta_c_v;
			Color obj_color;
			Vector3 ray_dir = pixel_center - eye_pos;
			Ray ray = Ray(eye_pos, ray_dir);
			ray.SetRayEta(1.0f);
			if (input_manage.GetFlagDepthOfView()) {
				float jitter_rad = 0.2f;
				int ray_itr_size = 10;
				int total_R = 0, total_G = 0, total_B = 0;
				int avg_R = 0, avg_G = 0, avg_B = 0;
				int hitcnt = 1;
				for (int i = 0; i < ray_itr_size; i++) {

					obj_color = ray.RayTrace(input_manage, 1, input_manage.GetMaxDepth());
					total_R += obj_color.red();
					total_G += obj_color.green();
					total_B += obj_color.blue();
					if (obj_color != input_manage.GetBgColor()) {
						hitcnt++;
					}

					// generate random shadow ray here for next round
					Point3 new_eye_position = input_manage.GetEyePos();
					for (int l = 0; l < 3; l++) {
						std::random_device randdev;
						std::mt19937 generator(randdev());
						std::uniform_real_distribution<> distrib(-jitter_rad, jitter_rad);
						float jit_delta = distrib(generator);
						new_eye_position[l] += jit_delta;
					}
					Vector3 L_jit = pixel_center - new_eye_position;
					L_jit.Normalize();
					ray.set(new_eye_position, L_jit);
				}
				avg_R = floor(total_R / hitcnt);
				avg_G = floor(total_G / hitcnt);
				avg_B = floor(total_B / hitcnt);
				obj_color = Color(avg_R, avg_G, avg_B);
			}
			else {
				obj_color = ray.RayTrace(input_manage, 1, input_manage.GetMaxDepth());
			}
			if (obj_color.red() != 0) {
				obj_color[0] += 0;
			}
			canvas_color[i][j] = obj_color;
		}
	}

	std::cout << "=================== writing to file ===================" << std::endl;
	std::string output_file = "./default_output.ppm";;
	if (argc == 3) {
		output_file = argv[2];
		if (output_file != "-soft" && output_file != "-att") {
			check_if_out_contain_ppm(output_file);
		}
		else {
			output_file = "./default_output.ppm";
		}
	}

	std::ofstream out_fstream(output_file, std::ios::out);
	out_fstream << "P3\n"
		<< "#oaT euynuJ yb detaerC_a1WH7065ICSC" << "\n"
		<< input_manage.GetImageSize()[0] << "\n"
		<< input_manage.GetImageSize()[1] << "\n"
		<< 255 << "\n";
	for (uint32_t i = 0; i < input_manage.GetImageSize()[1]; i++) {
		for (uint32_t j = 0; j < input_manage.GetImageSize()[0]; j++) {
			if (j % 4 == 0) {
				out_fstream << "\n";
			}
			int pixel_idx = i * input_manage.GetImageSize()[0] + j;
			// out_fstream << canvas_color.at(pixel_idx);
			out_fstream << canvas_color[i][j];
		}
	}
	out_fstream.close();
	std::cout << "Finished written to file: " << output_file << std::endl;
	std::cout << "====================== finished =======================" << std::endl;

	// deallocate memory using the delete operator
	for (int i = 0; i < width_px; i++) {
		delete[] canvas_color[i];
	}
	delete canvas_color;


	return 0;
}
