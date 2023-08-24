# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Make compilation more verbose if required.
option(VERBOSE "Verbose compiler output" OFF)

# If not build type is specified, default to Release.
set (default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message (STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set (CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

# Set C++ compiler flags.
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

if (VERBOSE)
  set (CMAKE_VERBOSE_MAKEFILE ON)
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  set (BUILD_TYPE "rel")
  if (CMAKE_COMPILER_IS_GNUCXX)
    string (APPEND CMAKE_CXX_FLAGS " -O3 -g0")
  elseif (MSVC)
    string (APPEND CMAKE_CXX_FLAGS " /MD /DEBUG:NONE /O2 /Ob2")
  else ()
    message (FATAL_ERROR "Unsupported compiler")
  endif ()
else ()
  set (BUILD_TYPE "dbg")
  if (CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O0 -g3")
  elseif (MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /O0 /Ob0")
  else ()
    message (FATAL_ERROR "Unsupported compiler")
  endif ()
endif ()