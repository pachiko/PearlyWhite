# CMakeList.txt : CMake project for PearlyWhite, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("PearlyWhite")

set(CMAKE_CXX_STANDARD 20)

# need to find the OpenCVConfig.cmake
set(OpenCV_DIR $ENV{OpenCV_DIR}\\lib)
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})


# Add source to this project's executable.
add_executable (PearlyWhite 
	# source files
	"src/main.cpp"
	"src/trackbarThresholder.cpp"
	"src/indicator.cpp"
	"src/histogram.cpp"
	"src/filter.cpp"

	# header files
	"src/trackbarThresholder.h"
	"src/indicator.h"
	"src/histogram.h"
	"src/filter.h"
)

TARGET_LINK_LIBRARIES(PearlyWhite ${OpenCV_LIBS})
