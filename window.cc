// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
//------------------------------------------------------------------------------
// window.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "window.h"
#include <assert.h>

namespace Display
{


#ifdef _WIN32
#define APICALLTYPE __stdcall
#else
#define APICALLTYPE
#endif
//------------------------------------------------------------------------------
/**
*/
static void GLAPIENTRY
GLDebugCallback(GLenum, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*)
{
	std::string msg("[OPENGL DEBUG MESSAGE] ");

	// print error severity
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:
		msg.append("<Low severity> ");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		msg.append("<Medium severity> ");
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		msg.append("<High severity> ");
		break;
	}

	// append message to output
	msg.append(message);

	// print message
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		printf("Error: %s\n", msg.c_str());
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		printf("Performance issue: %s\n", msg.c_str());
		break;
	default:		// Portability, Deprecated, Other
		break;
	}
}

int32_t Window::WindowCount = 0;
//------------------------------------------------------------------------------
/**
*/
Window::Window() :
	width(1024),
	height(768),
	title("Trayracer"),
	window(nullptr),
	frameCopy(0),
	texture(0)
{
	
}

//------------------------------------------------------------------------------
/**
*/
Window::~Window()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Window::StaticKeyPressCallback(GLFWwindow* win, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (nullptr != window->keyPressCallback)
		window->keyPressCallback(key, scancode, action, mods);
}

//------------------------------------------------------------------------------
/**
*/
void
Window::StaticMousePressCallback(GLFWwindow* win, int32_t button, int32_t action, int32_t mods)
{
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (nullptr != window->mousePressCallback)
		window->mousePressCallback(button, action, mods);
}

//------------------------------------------------------------------------------
/**
*/
void
Window::StaticMouseMoveCallback(GLFWwindow* win, double x, double y)
{
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (nullptr != window->mouseMoveCallback)
		window->mouseMoveCallback(x, y);
}

//------------------------------------------------------------------------------
/**
*/
void
Window::StaticMouseEnterLeaveCallback(GLFWwindow* win, int32_t mode)
{
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (nullptr != window->mouseLeaveEnterCallback)
		window->mouseLeaveEnterCallback(mode == 0);
}

//------------------------------------------------------------------------------
/**
*/
void
Window::StaticMouseScrollCallback(GLFWwindow* win, double x, double y)
{
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (nullptr != window->mouseScrollCallback)
		window->mouseScrollCallback(x, y);
}

//------------------------------------------------------------------------------
/**
*/
void
Window::StaticWindowResizeCallback(GLFWwindow* win, int32_t x, int32_t y)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
    window->width = x;
    window->height = y;
    window->Resize();
    if (nullptr != window->windowResizeCallback)
        window->windowResizeCallback(x, y);
}

//------------------------------------------------------------------------------
/**
*/
void
Window::Resize()
{
	if (nullptr != this->window)
	{
		glfwSetWindowSize(this->window, this->width, this->height);
		glViewport(0, 0, this->width, this->height);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Window::Retitle()
{
	if (nullptr != this->window)
		glfwSetWindowTitle(this->window, this->title.c_str());
}

//------------------------------------------------------------------------------
/**
*/
bool
Window::Open()
{
	if (Window::WindowCount == 0)
	{
		if (!glfwInit())
			return false;
	}

	// setup window
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
	//glfwWindowHint(GLFW_SAMPLES, 8);

	// open window
	this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(this->window);

	if (nullptr != this->window && WindowCount == 0)
	{
		GLenum res = glewInit();
		assert(res == GLEW_OK);
		if (!(GLEW_VERSION_4_0))
		{
			printf("[WARNING]: OpenGL 4.0+ is not supported on this hardware!\n");
			glfwDestroyWindow(this->window);
			this->window = nullptr;
			return false;
		}

		// setup debug callback
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLDebugCallback, NULL);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);

		// setup stuff
		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
		//glEnable(GL_MULTISAMPLE);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		// setup viewport
		glViewport(0, 0, this->width, this->height);
	}

	glfwSetWindowUserPointer(this->window, this);
	glfwSetKeyCallback(this->window, Window::StaticKeyPressCallback);
	glfwSetMouseButtonCallback(this->window, Window::StaticMousePressCallback);
	glfwSetCursorPosCallback(this->window, Window::StaticMouseMoveCallback);
	glfwSetCursorEnterCallback(this->window, Window::StaticMouseEnterLeaveCallback);
	glfwSetScrollCallback(this->window, Window::StaticMouseScrollCallback);
    glfwSetWindowSizeCallback(this->window, Window::StaticWindowResizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Framebuffer setup
    glGenFramebuffers(1, &frameCopy);
    glBindFramebuffer(GL_FRAMEBUFFER, frameCopy);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	// increase window count and return result
	Window::WindowCount++;
	return this->window != nullptr;
}

//------------------------------------------------------------------------------
/**
*/
void
Window::Close()
{
	if (nullptr != this->window)
		glfwDestroyWindow(this->window);

	this->window = nullptr;
	Window::WindowCount--;
	if (Window::WindowCount == 0)
	{
		glfwTerminate();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Window::MakeCurrent()
{
	glfwMakeContextCurrent(this->window);
}

//------------------------------------------------------------------------------
/**
*/
void
Window::Update()
{
	glfwPollEvents();	
}

//------------------------------------------------------------------------------
/**
*/
void
Window::SwapBuffers()
{
	if (this->window)
	{
		glfwSwapBuffers(this->window);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Window::Blit(float const* data, int w, int h)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, (void*)data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
    //glBlitNamedFramebuffer(frameCopy, NULL, 0, 0, w, h, 0, 0, this->width, this->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameCopy);
	glBlitFramebuffer(0, 0, w, h, 0, 0, this->width, this->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	
	// switch back to default read buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

} // namespace Display
