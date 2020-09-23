#pragma once
//------------------------------------------------------------------------------
/**
	Manages the opening and closing of a window.
	
	(C) 2015-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

namespace Display
{
class Window
{
public:
	/// constructor
	Window();
	/// destructor
	~Window();

	/// set size of window
	void SetSize(int32_t width, int32_t height);
	/// get size of windows
	void GetSize(int32_t & width, int32_t & height);
	/// set title of window
	void SetTitle(const std::string& title);

	/// open window
	bool Open();
	/// close window
	void Close();
	/// returns true if window is open
	const bool IsOpen() const;

	/// make this window current, meaning all draws will direct to this window context
	void MakeCurrent();

	/// update a tick
	void Update();
	/// swap buffers at end of frame
	void SwapBuffers();

	/// set key press function callback
	void SetKeyPressFunction(const std::function<void(int32_t, int32_t, int32_t, int32_t)>& func);
	/// set mouse press function callback
	void SetMousePressFunction(const std::function<void(int32_t, int32_t, int32_t)>& func);
	/// set mouse move function callback
	void SetMouseMoveFunction(const std::function<void(double, double)>& func);
	/// set mouse enter leave function callback
	void SetMouseEnterLeaveFunction(const std::function<void(bool)>& func);
	/// set mouse scroll function callback
	void SetMouseScrollFunction(const std::function<void(double, double)>& func);
    	/// set window resize function callback
    	void SetWindowResizeFunction(const std::function<void(int32_t, int32_t)>& func);
	/// bit block transfer from buffer to screen. data buffer must be exactly w * h * 3 large!
	void Blit(float const* data, int w, int h);

private:

	/// static key press callback
	static void StaticKeyPressCallback(GLFWwindow* win, int32_t key, int32_t scancode, int32_t action, int32_t mods);
	/// static mouse press callback
	static void StaticMousePressCallback(GLFWwindow* win, int32_t button, int32_t action, int32_t mods);
	/// static mouse move callback
	static void StaticMouseMoveCallback(GLFWwindow* win, double x, double y);
	/// static mouse enter/leave callback
	static void StaticMouseEnterLeaveCallback(GLFWwindow* win, int32_t mode);
	/// static mouse scroll callback
	static void StaticMouseScrollCallback(GLFWwindow* win, double x, double y);
    /// static resize window callback
    static void StaticWindowResizeCallback(GLFWwindow* win, int32_t x, int32_t y);

	/// resize update
	void Resize();
	/// title rename update
	void Retitle(); 

	static int32_t WindowCount;

	/// function for key press callbacks
	std::function<void(int32_t, int32_t, int32_t, int32_t)> keyPressCallback;
	/// function for mouse press callbacks
	std::function<void(int32_t, int32_t, int32_t)> mousePressCallback;
	/// function for mouse move callbacks
	std::function<void(double, double)> mouseMoveCallback;
	/// function for mouse enter/leave callbacks
	std::function<void(bool)> mouseLeaveEnterCallback;
	/// function for mouse scroll callbacks
	std::function<void(double, double)> mouseScrollCallback;
    /// function for window resize callbacks
    std::function<void(int32_t, int32_t)> windowResizeCallback;
	
	int32_t width;
	int32_t height;
	std::string title;
	GLFWwindow* window;

private:
	GLuint frameCopy;
    GLuint texture;
};

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetSize(int32_t width, int32_t height)
{
	this->width = width;
	this->height = height;
	if (nullptr != this->window) this->Resize();
}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::GetSize(int32_t & width, int32_t & height)
{
	width = this->width;
	height = this->height;

}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetTitle(const std::string& title)
{
	this->title = title;
	if (nullptr != this->window) this->Retitle();
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
Window::IsOpen() const
{
	return nullptr != this->window;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetKeyPressFunction(const std::function<void(int32_t, int32_t, int32_t, int32_t)>& func)
{
	this->keyPressCallback = func;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetMousePressFunction(const std::function<void(int32_t, int32_t, int32_t)>& func)
{
	this->mousePressCallback = func;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetMouseMoveFunction(const std::function<void(double, double)>& func)
{
	this->mouseMoveCallback = func;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetMouseEnterLeaveFunction(const std::function<void(bool)>& func)
{
	this->mouseLeaveEnterCallback = func;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetMouseScrollFunction(const std::function<void(double, double)>& func)
{
	this->mouseScrollCallback = func;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Window::SetWindowResizeFunction(const std::function<void(int32_t, int32_t)>& func)
{
    this->windowResizeCallback = func;
}

} // namespace Display
