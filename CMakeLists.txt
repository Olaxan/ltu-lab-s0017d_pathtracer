#--------------------------------------------------------------------------
# trayracer
#--------------------------------------------------------------------------
PROJECT(trayracer)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

if(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
ENDIF()
IF(APPLE)
	SET(CMAKE_CXX_FLAGS "-g -std=c++17 -stdlib=libc++")
ENDIF()

SET(ENV_ROOT ${CMAKE_CURRENT_DIR})

IF(MSVC)
    SET(OPENGL_LIBS opengl32.lib)
ELSE()
    SET(OPENGL_LIBS GL X11 Xxf86vm pthread Xrandr Xi Xinerama Xcursor)
ENDIF()

if (MSVC)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/bin>)
else()
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS GLEW_STATIC)

ADD_SUBDIRECTORY(exts)

SET(files
		main.cc
		window.h
		window.cc
		vec3.h
		color.h
		mat4.h
		object.h
		pbr.h
		ray.h
		raytracer.h
		raytracer.cc
		sphere.h
		xrng.h
		material.h
		material.cc
	)
SOURCE_GROUP("trayracer" FILES ${files})

ADD_EXECUTABLE(trayracer ${files})
ADD_DEPENDENCIES(trayracer stb glew glfw)

TARGET_LINK_LIBRARIES(trayracer PUBLIC exts stb glew glfw ${OPENGL_LIBS})

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
SET(CMAKE_CXX_FLAGS_DEBUG "-g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

