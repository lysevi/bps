cmake_minimum_required (VERSION 3.10)
set(PROJECT "bpstore")
project (${PROJECT} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0601)
	add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
	add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
endif ()

if (NOT EXISTS conanbuildinfo.cmake)
  message(FATAL_ERROR "Conan needs to be executed: conan install <path_to_your_conanfile.txt>")
endif ()
 
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cotire/CMake")

include(AddCXXCompilerFlag)
include(CompillerSettings)

set(DOUBLE_CHECKS  OFF)
if (CMAKE_BUILD_TYPE EQUAL "Debug")
    message(STATUS "debug mode")
	set(DOUBLE_CHECKS  ON)
endif(CMAKE_BUILD_TYPE EQUAL "Debug")

MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

option(bpstore_ENABLE_TESTS "Enable testing of the bpstore." ON)
option(bpstore_ENABLE_DOUBLECHECKS "More checks in code." ${DOUBLE_CHECKS})
option(bpstore_ENABLE_BENCHMARKS "Enable build benchmarks." ON)
option(bpstore_COVER "coverage mode" OFF)

MESSAGE(STATUS "Options: ")
MESSAGE(STATUS "bpstore_ENABLE_TESTS - " ${bpstore_ENABLE_TESTS})
MESSAGE(STATUS "bpstore_ENABLE_DOUBLECHECKS - " ${bpstore_ENABLE_DOUBLECHECKS})
MESSAGE(STATUS "bpstore_ENABLE_BENCHMARKS - " ${bpstore_ENABLE_BENCHMARKS})



if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
 macro(PRECOMPILED_HEADERS ARG)
 endmacro()
else()
 include(cotire)
 macro(PRECOMPILED_HEADERS ARG)
  cotire(${ARG})
 endmacro()
endif()

if(NOT ${bpstore_COVER})
MESSAGE(STATUS "Build all to ./bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
else()
MESSAGE(STATUS "Standart build folders.")
endif()

########## BOOST
IF(NOT HEADER_ONLY)
    list(FIND CONAN_DEFINES_BOOST "-DBOOST_USE_STATIC_LIBS" IS_STATIC)
    if(${IS_STATIC} GREATER -1)
        set(Boost_USE_STATIC_LIBS ON)
    else()
        set(Boost_USE_STATIC_LIBS OFF)
	endif()
endif()

add_definitions(-DBOOST_ASIO_NO_DEPRECATED)
find_package(Boost 1.69.0 COMPONENTS system date_time REQUIRED)

IF(Boost_FOUND)
else()	
  MESSAGE(FATAL_ERROR "boost not found")
  EXIT()
ENDIF()

#### catch2
if(bpstore_ENABLE_TESTS)
   include_directories(extern/Catch2/single_include/catch2/)
endif(bpstore_ENABLE_TESTS)

SET(bpstore_LIBRARY_DIR)
SET(bpstore_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} )

SET(bpstore_PRIVATE_LIBRARIES  ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

add_subdirectory(bpstore)

if(bpstore_ENABLE_BENCHMARKS)
   add_definitions(-DBPSTORE_ENABLE_BENCHMARKS)
endif(bpstore_ENABLE_BENCHMARKS)

if (bpstore_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(unittests)
endif()
