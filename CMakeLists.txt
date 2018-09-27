project(FeedTNZ)
cmake_minimum_required(VERSION 2.8.12)
add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(feedtnz feedtnz.cpp)
target_link_libraries(feedtnz ${CONAN_LIBS})
set_target_properties(feedtnz PROPERTIES LINK_FLAGS "/ignore:4099")