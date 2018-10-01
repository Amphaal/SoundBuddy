project(FeedTNZ)
cmake_minimum_required(VERSION 3.1.0)

#specific handling for visual studio
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

# Find the QtCore library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

#set resrouces 
SET(CMAKE_AUTORCC ON)
SET(FEEDTNZ_RES src/resources/resources.qrc)
SET(FEEDTNZ_ENTRYPOINT src/feedtnz.cpp)

if (WIN32)
    #set bundle type
    SET(FEEDTNZ_BUNDLE_TYPE WIN32)

    #add icon
    SET(FEEDTNZ_ICO_WIN src/resources/resources.rc)
endif (WIN32)

if (APPLE)
    #set bundle type
    SET(FEEDTNZ_BUNDLE_TYPE MACOSX_BUNDLE)

    SET(CMAKE_EXE_LINKER_FLAGS "-framework AppKit -w")

    list(APPEND FEEDTNZ_ENTRYPOINT src/ui/mac/nativeEvents.mm)

    #add icon
    set(MACOSX_BUNDLE_ICON_FILE feedtnz.icns)
    set(FEEDTNZ_ICO_WIN src/resources/icons/feedtnz.icns)
    set_source_files_properties(${FEEDTNZ_ICO_WIN} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif (APPLE)

add_executable(${PROJECT_NAME} ${FEEDTNZ_BUNDLE_TYPE} ${FEEDTNZ_ENTRYPOINT} ${FEEDTNZ_RES} ${FEEDTNZ_ICO_WIN}) 
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core)

if (APPLE)
    #add highdpi support via plist template
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/Info.plist.in)
endif (APPLE)
