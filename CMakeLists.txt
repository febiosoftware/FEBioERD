cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
mark_as_advanced(FORCE CMAKE_INSTALL_PREFIX)

project(FEBioERD)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set a default build type if none was specified
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# FEBio
find_package(FEBio REQUIRED)

if(FEBio_LIB_DIR)
    findLib(FECore FEBio_LIB_DIR FECORE)
    findLib(FEBioMech FEBio_LIB_DIR FEBIOMECH)
    findLib(FEBioMix FEBio_LIB_DIR FEBIOMIX)
endif()

##### Set appropriate defines and includes #####

if(WIN32)
    add_definitions(-DWIN32  -DFECORE_DLL /MP /openmp)
elseif(APPLE)
    add_definitions(-D__APPLE__)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
else()
	add_definitions(-DLINUX)
    add_compile_options(-static-libstdc++ -static-libgcc -w -Wall)
    
    set(CMAKE_BUILD_RPATH_USE_LINK_PATH FALSE)
    set(CMAKE_BUILD_RPATH $ORIGIN/../lib/)
endif()

# Extra compiler flags for intel compiler
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -static-intel)
endif()

##### Add library #####

file(GLOB HDR_FEBioERD "FEBioERD/*.h")
file(GLOB SRC_FEBioERD "FEBioERD/*.cpp")

add_library(FEBioERD SHARED ${HDR_FEBioERD} ${SRC_FEBioERD})
set_property(TARGET FEBioERD PROPERTY AUTOGEN_BUILD_DIR ${CMAKE_BINARY_DIR}/CMakeFiles/AutoGen/FEBioERD_autogen)

##### Link Libraries #####

target_link_libraries(FEBioERD FEBio::FECore FEBio::FEBioMech FEBio::FEBioMix)
