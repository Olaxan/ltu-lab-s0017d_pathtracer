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
#include <math.h>

#include <chrono>
#include <thread>

#define degtorad(angle) angle * MPI / 180

void print_usage()
{
	printf("This utility renders a single frame of a raytraced scene, "
			"optionally displaying it in an OpenGL window, and saving the finished result to a file.\n\n"
			"usage: \ttrayracer [-h] [-s] [-w <wxh>] [-o <path>] [-r <rays>]\n"
		       "\t[-n <name>] [-x <seed>] [-b <bounces>] [-t <threads>] <spheres>\n\n"
		       "options:\n"
		       "\t-h:\t\tShows this help.\n"
		       "\t-s:\t\tRun without opening a render window. Use -o to specify an output path.\n"
		       "\t-w <wxh>: \tSpecify the resolution of the render. Example: 1920x1080 - no spaces allowed.\n"
		       "\t-o <path>: \tSpecify an (optional)  output path for the finished image.\n"
		       "\t-r <rays>: \tSelect the number of rays to fire per pixel in the image.\n"
		       "\t-n <name>: \tSpecify the OpenGL window name.\n"
		       "\t-x <seed>: \tAn optional seed for placing the spheres in the scene.\n"
		       "\t-b <bounces>: \tThe number of bounces to perform per ray. Higher gives improved reflections.\n"
		       "\t-t <threads>: \tThe number of threads available for tracing, 0 to disable. Default all cores.\n"
		       "\t<spheres>:\tThe number of spheres to render randomly.\n\nn"
	      );
}

xrng::xoshiro128_plus rng = xrng::xoshiro128_plus();

int main(int argc, char* argv[])
{

	typedef std::chrono::duration<float> duration;
	typedef std::chrono::high_resolution_clock clock;

	bool silent = false;

	size_t width = 256;
	size_t height = 256;

	size_t rays = 1;
	size_t seed = 1337;
	size_t bounces = 5;
	size_t spheres = 8;
	size_t max_threads = std::thread::hardware_concurrency();
	size_t max_memory = 0;
	
	char* window_name = nullptr;
	char* output_path = nullptr;
	
	int c;
	int argval;

	opterr = 0;

	while ((c = getopt (argc, argv, "hsw:o:r:n:x:b:t:m:")) != -1)
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
				if (sscanf(optarg, "%zux%zu", &width, &height) != 2)
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
				argval = atoi(optarg);
				if (argval < 1)
				{
					fprintf(stderr, "Invalid bounce count for '-%c': Must be at least 1.\n", c);
					return 1;
				}
				bounces = argval;
				break;
			case 't':
				argval = atoi(optarg);
				if (argval < 0)
				{
					fprintf(stderr, "Invalid thread count for '-%c': Must be positive integer or 0.\n", c);
					return 1;
				}
				max_threads = argval;
				break;
			case 'm':
				argval = atoi(optarg);
				if (argval < 0)
				{
					fprintf(stderr, "Invalid memory size for '%c': Must be positive integer or 0.\n", c);
					return 1;
				}
				max_memory = atoi(optarg);
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

	auto begin_time = clock::now();
	rng.seed(seed);	

	// camera
	vec3 camPos = { 0, 1.0f ,10.0f };
	mat4 cameraTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	cameraTransform.m30 = camPos.x;
	cameraTransform.m31 = camPos.y;
	cameraTransform.m32 = camPos.z;

	std::vector<Color> framebuffer(width * height);
	
	TraceData data = {
		width,
		height,
		rays,
		bounces,
		max_threads,
		max_memory,
	};

	Raytracer rt = Raytracer(cameraTransform, framebuffer, data);

	// Ground plane
	Material ground_mat = 
	{
		MaterialType::Lambertian,
		{ 0.5,0.5,0.5 },
		0.3
	};

	rt.add_sphere({0, -1000, -1}, 1000, rt.add_material(ground_mat));

	// Random spheres
	for (size_t i = 0; i < spheres; i++)
	{
		Material mat = 
		{
			static_cast<MaterialType>(int(round(rng.fnext() * 3))),
			{rng.fnext(),rng.fnext(),rng.fnext()},
			rng.fnext()
		};

		vec3 center = { rng.fnext(-5, 5), rng.fnext(0, 3), rng.fnext(0, 5) };
		rt.add_sphere(center, rng.fnext(0, 2), rt.add_material(mat));
	}	

	printf("Tracing %zu spheres to buffer of size %zux%zu with %zu rays, %zu bounces...\n", spheres, width, height, rays, bounces);

	rt.trace();
	duration elapsed = clock::now() - begin_time;

	float mrs = float(width * height * rays) / (elapsed.count() * 1000000.0f);
	float bmrs = mrs * bounces;

	printf("Trace complete in %f seconds, %f / %f MRay/s [complete / per-bounce].\n", elapsed.count(), mrs, bmrs);

	if (output_path != nullptr)
	{
		uint8_t* pixels = new uint8_t[width * height * 3];
	
		size_t index = 0;	
		for (size_t i = 0; i < width * height; i++)
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
	bool exit = false;	
	
	wnd.SetTitle(window_name == nullptr ? "TrayRacer" : window_name);
	wnd.SetSize(width, height);
	
	if (!wnd.Open())
		return 1;

	wnd.SetKeyPressFunction([&exit](int key, int, int, int)
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
