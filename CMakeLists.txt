project(FeedTNZ)
cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Find the QtCore library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

#set resrouces 
SET(CMAKE_AUTORCC ON)
SET(FEEDTNZ_RES src/resources/resources.qrc)

if (WIN32)
    SET(FEEDTNZ_ICO_WIN src/resources/resources.rc)
    add_executable(${PROJECT_NAME} WIN32 src/feedtnz.cpp ${FEEDTNZ_RES} ${FEEDTNZ_ICO_WIN})
endif (WIN32)

if (APPLE)
    add_executable(${PROJECT_NAME} src/feedtnz.cpp)
endif (APPLE)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core)