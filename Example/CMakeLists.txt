cmake_minimum_required (VERSION 2.6)
project (TinyClock_Example CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
 
include (GNUInstallDirs)
include (CMakePackageConfigHelpers)

set(PROJECT_LABEL "Example")

#enable_testing()

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

find_package(OpenGL)

if(UNIX)
set (LINK_DIRECTORY "/usr/lib/")
set (LIBS "-lX11 ${OPENGL_LIBRARIES}")
set (RELEASE_FLAGS "-std=c++11 -DSO -w -fpermissive")
set (DEBUG_FLAGS "-std=c++11 -DSO -w -g -fpermissive -DDEBUG")

if (CMAKE_BUILD_TYPE MATCHES Release)
	add_definitions(${RELEASE_FLAGS})
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(${DEBUG_FLAGS})
elseif (CMAKE_BUILD_TYPE MATCHES "")
	add_definitions(${DEBUG_FLAGS})
endif()

endif(UNIX)

if (WIN32)
	set (LIBS "${OPENGL_LIBRARIES}")
endif(WIN32)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 3)

set (TINYCLOCK_INCLUDE_DIR "../Include")
set (DEBUG_OUTPUT_DIR "./bin/Debug/")
set (RELEASE_OUTPUT_DIR "./bin/Release/")
set (OUTPUT_DIR "./bin")

if ( MSVC )
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DEBUG_OUTPUT_DIR}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RELEASE_OUTPUT_DIR}")	
endif( MSVC )

set (CMAKE_CXX_FLAGS_DEBUG "${DEBUG_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "${RELEASE_FLAGS}")

include_directories ("${TINYCLOCK_INCLUDE_DIR}")
include_directories ("${PROJECT_BINARY_DIR}")
link_directories (${LINK_DIRECTORY})
link_libraries (${LIBS})
SET ( SOURCE Example.cpp)
SET ( HEADER_FILES ${TINYCLOCK_INCLUDE_DIR}/TinyClock.h)

add_executable(TinyClock_Example ${SOURCE} ${HEADER_FILES})
set_property(TARGET TinyClock_Example PROPERTY OUTPUT_NAME "TinyClock_Example")
