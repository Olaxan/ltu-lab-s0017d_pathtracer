#include <stdio.h>
#include "window.h"
#include "vec3.h"
#include "raytracer.h"
#include "sphere.h"

#define degtorad(angle) angle * MPI / 180

int main()
{ 
    Display::Window wnd;
    
    wnd.SetTitle("TrayRacer");
    
    if (!wnd.Open())
        return 1;

    std::vector<Color> framebuffer;

    const unsigned w = 200;
    const unsigned h = 100;
    framebuffer.resize(w * h);
    
    int raysPerPixel = 1;
    int maxBounces = 5;

    Raytracer rt = Raytracer(w, h, framebuffer, raysPerPixel, maxBounces);

    // Create some objects
    Material* mat = new Material();
    mat->type = "Lambertian";
    mat->color = { 0.5,0.5,0.5 };
    mat->roughness = 0.3;
    Sphere* ground = new Sphere(1000, { 0,-1000, -1 }, mat);
    rt.AddObject(ground);
    
    mat = new Material();
    mat->type = "Conductor";
    mat->color = {1,1,1};
    mat->roughness = 0.2f;
    Sphere* test = new Sphere(1, { 0,1,0 }, mat);
    rt.AddObject(test);

    mat = new Material();
    mat->type = "Lambertian";
    mat->color = { 0,0.4,0.6 };
    mat->roughness = 0.2;
    test = new Sphere(1, { -4,1,0 }, mat);
    rt.AddObject(test);

    mat = new Material();
    mat->type = "Dielectric";
    mat->color = { 1,0.8,0.7 };
    mat->roughness = 0.95;
    mat->refractionIndex = 1.65;
    test = new Sphere(1, { -4,1, 2 }, mat);
    rt.AddObject(test);

    mat = new Material();
    mat->type = "Lambertian";
    mat->color = { 1,0,0.2 };
    mat->roughness = 0.04;
    test = new Sphere(1, { 1,1, -3 }, mat);
    rt.AddObject(test);

    mat = new Material();
    mat->type = "Lambertian";
    mat->color = { 1,1,1 };
    mat->roughness = 0.0;
    test = new Sphere(1, { 4,1, 0 }, mat);
    rt.AddObject(test);

    bool exit = false;

    // camera
    bool resetFramebuffer = false;
    vec3 camPos = { 0,1.0f,10.0f };
    vec3 moveDir = { 0,0,0 };

    wnd.SetKeyPressFunction([&exit, &moveDir, &resetFramebuffer](int key, int scancode, int action, int mods)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            exit = true;
            break;
        case GLFW_KEY_W:
            moveDir.z -= 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_S:
            moveDir.z += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_A:
            moveDir.x -= 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_D:
            moveDir.x += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_SPACE:
            moveDir.y += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            moveDir.y -= 1.0f;
            resetFramebuffer |= true;
            break;
        default:
            break;
        }
    });

    float pitch = 0;
    float yaw = 0;
    float oldx = 0;
    float oldy = 0;

    wnd.SetMouseMoveFunction([&pitch, &yaw, &oldx, &oldy, &resetFramebuffer](double x, double y)
    {
        x *= -0.1;
        y *= -0.1;
        yaw = x - oldx;
        pitch = y - oldy;
        resetFramebuffer |= true;
        oldx = x;
        oldy = y;
    });

    float rotx = 0;
    float roty = 0;

    // number of accumulated frames
    int frameIndex = 0;

    std::vector<Color> framebufferCopy;
    framebufferCopy.resize(w * h);

    // rendering loop
    while (wnd.IsOpen() && !exit)
    {
        resetFramebuffer = false;
        moveDir = {0,0,0};
        pitch = 0;
        yaw = 0;

        // poll input
        wnd.Update();

        rotx -= pitch;
        roty -= yaw;

        moveDir = normalize(moveDir);

        mat4 xMat = (rotationx(rotx));
        mat4 yMat = (rotationy(roty));
        mat4 cameraTransform = multiply(yMat, xMat);

        camPos = camPos + transform(moveDir * 0.2f, cameraTransform);
        
        cameraTransform.m30 = camPos.x;
        cameraTransform.m31 = camPos.y;
        cameraTransform.m32 = camPos.z;

        rt.SetViewMatrix(cameraTransform);
        
        if (resetFramebuffer)
        {
            rt.Clear();
            frameIndex = 0;
        }

        rt.Raytrace();
        frameIndex++;

        // Get the average distribution of all samples
        {
            size_t p = 0;
            for (Color const& pixel : framebuffer)
            {
                framebufferCopy[p] = pixel;
                framebufferCopy[p].r /= frameIndex;
                framebufferCopy[p].g /= frameIndex;
                framebufferCopy[p].b /= frameIndex;
                p++;
            }
        }

        glClearColor(0, 0, 0, 1.0);
        glClear( GL_COLOR_BUFFER_BIT );

        wnd.Blit((float*)&framebufferCopy[0], w, h);
        wnd.SwapBuffers();
    }

    if (wnd.IsOpen())
        wnd.Close();

    return 0;
} 