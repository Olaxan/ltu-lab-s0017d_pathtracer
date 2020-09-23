// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "shared.h"

#include "window.h"
#include "vec3.h"
#include "sphere.h"
#include "raytracer.h"
#include "xrng.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <getopt.h>

#include <chrono>

#define degtorad(angle) angle * MPI / 180

void print_usage()
{
	printf("This utility renders a single frame of a raytraced scene, "
			"optionally displaying it in an OpenGL window, and saving the finished result to a file.\n\n"
			"usage: \ttrayracer [-h] [-s] [-w <wxh>] [-o <path>] [-r <rays>]\n"
		       "\t[-n <name>] [-x <seed>] [-b <bounces>] <spheres>\n\n"
		       "options:\n"
		       "\t-h:\t\tShows this help.\n"
		       "\t-s:\t\tRun without opening a render window. Use -o to specify an output path.\n"
		       "\t-w <wxh>: \tSpecify the resolution of the render. Example: 1920x1080 - no spaces allowed.\n"
		       "\t-o <path>: \tSpecify an (optional)  output path for the finished image.\n"
		       "\t-r <rays>: \tSelect the number of rays to fire per pixel in the image (default = 1).\n"
		       "\t-n <name>: \tSpecify the OpenGL window name.\n"
		       "\t-x <seed>: \tAn optional seed for placing the spheres in the scene.\n"
		       "\t-b <bounces>: \tThe number of bounces to perform per ray. Higher gives improved reflections.\n"
		       "\t<spheres>:\tThe number of spheres to render randomly.\n\nn"
	      );
}

xrng::xoshiro128_plus rng = xrng::xoshiro128_plus();

int main(int argc, char* argv[])
{

	typedef std::chrono::duration<float> duration;
	typedef std::chrono::high_resolution_clock clock;

	bool silent = false;

	unsigned width = 256;
	unsigned height = 256;

	int rays = 1;
	int seed = 1337;
	int bounces = 5;
	int spheres = 8;	
	
	char* window_name = nullptr;
	char* output_path = nullptr;
	
	int index;
	int c;

	opterr = 0;

	while ((c = getopt (argc, argv, "hsw:o:r:n:x:b:")) != -1)
	{
		switch (c)
		{
			case 'h':
				print_usage();
				return 0;
			case 's':
				silent = true;
				break;
			case 'w':
				if (sscanf(optarg, "%ux%u", &width, &height) != 2)
				{
					fprintf(stderr, "Invalid resolution format for '-%c': use 'WxH'\n", c);
					return 1;
				}
				break;
			case 'o':
				output_path = optarg;
				break;
			case 'r':
				if ((rays = atoi(optarg)) <= 0)
				{
					fprintf(stderr, "Invalid ray count for '-%c': Must be positive integer.\n", c);
					return 1;
				}
				break;
			case 'n':
				window_name = optarg;
				break;
			case 'x':
				seed = atoi(optarg);
				break;
			case 'b':
				bounces = atoi(optarg);
				break;
			case '?':
				if (optopt == 'c')
					fprintf (stderr, "Option '-%c' requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option '-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character '\\x%x'.\n",optopt);
				return 1;
			default:
				abort ();
		}
	}

	if (optind == argc - 1)
		spheres = atoi(argv[optind]);

	rng.seed(seed);	

	std::vector<Color> framebuffer(width * height);
	
	Raytracer rt = Raytracer(width, height, framebuffer, rays, bounces);

	for (int i = 0; i < spheres; i++)
	{

	}

	// Create some objects
	Material* mat = new Material();
	mat->type = MaterialType::Lambertian;
	mat->color = { 0.5,0.5,0.5 };
	mat->roughness = 0.3;
	Sphere* ground = new Sphere(1000, { 0,-1000, -1 }, mat);
	rt.add_object(ground);
	
	mat = new Material();
	mat->type = MaterialType::Conductor;
	mat->color = {1,1,1};
	mat->roughness = 0.2f;
	Sphere* test = new Sphere(1, { 0,1,0 }, mat);
	rt.add_object(test);

	mat = new Material();
	mat->type = MaterialType::Lambertian;
	mat->color = { 0,0.4,0.6 };
	mat->roughness = 0.2;
	test = new Sphere(1, { -4,1,0 }, mat);
	rt.add_object(test);

	mat = new Material();
	mat->type = MaterialType::Dielectric;
	mat->color = { 1,0.8,0.7 };
	mat->roughness = 0.95;
	mat->refractionIndex = 1.65;
	test = new Sphere(1, { -4,1, 2 }, mat);
	rt.add_object(test);

	mat = new Material();
	mat->type = MaterialType::Lambertian;
	mat->color = { 1,0,0.2 };
	mat->roughness = 0.04;
	test = new Sphere(1, { 1,1, -3 }, mat);
	rt.add_object(test);

	mat = new Material();
	mat->type = MaterialType::Lambertian;
	mat->color = { 1,1,1 };
	mat->roughness = 0.0;
	test = new Sphere(1, { 4,1, 0 }, mat);
	rt.add_object(test);

	bool exit = false;

	// camera
	vec3 camPos = { 0, 1.0f ,10.0f };
	vec3 moveDir = { 0, 0, 0 };	

	mat4 cameraTransform = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	cameraTransform.m30 = camPos.x;
	cameraTransform.m31 = camPos.y;
	cameraTransform.m32 = camPos.z;

	rt.set_view_matrix(cameraTransform);

	printf("Tracing %i spheres to buffer of size %ux%u with %i rays, %i bounces...\n", spheres, width, height, rays, bounces);

	auto begin_time = clock::now();
	rt.trace();
	duration elapsed = clock::now() - begin_time;

	float mrs = float(width * height * rays) / (elapsed.count() * 1000000);

	printf("Trace complete in %f seconds, %f MRay/s.\n", elapsed.count(), mrs);

	if (output_path != nullptr)
	{
		uint8_t* pixels = new uint8_t[width * height * 3];
	
		int index = 0;	
		for (int i = 0; i < width * height; i++)
		{
			pixels[index++] = 255.0f * framebuffer[i].r;
			pixels[index++] = 255.0f * framebuffer[i].g;
			pixels[index++] = 255.0f * framebuffer[i].b;
		}

		stbi_flip_vertically_on_write(1);

		if (stbi_write_png(output_path, width, height, 3, pixels, width * 3) == 0)
			fprintf(stderr, "Failed to write file to '%s'.\n", output_path);
		else
			printf("File written to '%s'\n", output_path);

		delete[] pixels;
	}

	if (silent)
		return 0;

	Display::Window wnd;
	
	wnd.SetTitle(window_name == nullptr ? "TrayRacer" : window_name);
	
	if (!wnd.Open())
		return 1;

	wnd.SetKeyPressFunction([&exit](int key, int scancode, int action, int mods)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			exit = true;
			break;
		default:
			break;
		}
	});

	// rendering loop
	while (wnd.IsOpen() && !exit)
	{
		// poll input
		wnd.Update();
		wnd.Blit((float*)&framebuffer[0], width, height);
		wnd.SwapBuffers();
	}

	if (wnd.IsOpen())
		wnd.Close();

	return 0;
} 
