cmake_minimum_required (VERSION 3.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Check if CMAKE_TOOLCHAIN_FILE is set.
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message (FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not set. Please set it to the path of the vcpkg.cmake file.")
endif ()

# Project setup.
set (project VRTesting)
project (
	${project}
	VERSION 0.1.0
	DESCRIPTION "Testing VR stuff"
	LANGUAGES CXX
)

include(StandardProjectSettings)
include(ConfigureQt)

# Find libraries.
find_package(fmt CONFIG REQUIRED)
find_package(OpenVR REQUIRED)
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

# Add resources.
#set (${project}_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)
if (WIN32)
    list (APPEND ${project}_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources.rc)
endif ()

# Add source to this project's executable.
add_executable (${project}
	main.cpp
	${${project}_RESOURCES}
)

# Add include directories.
target_include_directories(${project} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Qt_INCLUDE_DIRS})

# Add libraries.
target_link_libraries(${project} PRIVATE
	Threads::Threads
	${CMAKE_DL_LIBS}
	${Qt_LIBRARIES}
	fmt::fmt-header-only
	OpenVR::OpenVR
)
if (WIN32)
  target_link_libraries(${project} PRIVATE ws2_32 winmm Iphlpapi)
endif ()

# Hide console window in release mode.
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property (TARGET ${project} PROPERTY WIN32_EXECUTABLE TRUE)
endif ()

# Copy libraries to output directory.
if (WIN32)
  add_custom_command(TARGET ${project} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	  $<TARGET_FILE:OpenVR::OpenVR>
	  $<TARGET_FILE_DIR:${project}>
  )
endif ()
