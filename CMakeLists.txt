project(FeedTNZ)
cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtCore library
find_package(Qt5Widgets REQUIRED)

add_executable(${PROJECT_NAME} feedtnz.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)