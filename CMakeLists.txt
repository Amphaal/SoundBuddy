project(FeedTNZ)
cmake_minimum_required(VERSION 3.1.0)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_INCLUDE_CURRENT_DIR YES)

#Qt specifics
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

#enable Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

#specific to Boost lib errors handling
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

#specific handling for visual studio compiler
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif(MSVC)

# Find the QtCore library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
SET(QT_LIBS_TO_INCLUDE Qt5::Widgets Qt5::Core)

#set resrouces 
SET(FEEDTNZ_RES resources/resources.qrc)
SET(FEEDTNZ_ENTRYPOINT src/feedtnz.cpp)

#add OSX+WIN libs
list(APPEND FEEDTNZ_ENTRYPOINT 
    libs/plistcpp/Plist.cpp 
    libs/plistcpp/PlistDate.cpp
    libs/plistcpp/NSPlist.cpp
    libs/plistcpp/NSPlistParser.cpp
    libs/plistcpp/NSPlistLexer.cpp
)
list(APPEND FEEDTNZ_ENTRYPOINT 
    libs/qtautoupdater/autoupdatercore/simplescheduler.cpp
    libs/qtautoupdater/autoupdatercore/updater.cpp
    libs/qtautoupdater/autoupdatercore/updater_p.cpp
)

#platform specfic
list(APPEND FEEDTNZ_ENTRYPOINT 
    src/helpers/platformHelper/platformHelper.cpp)
    src/workers/base/ITNZThread.h
    src/workers/shout/shout.cpp
    src/ui/mainWindow.cpp
    src/ui/tabs/base/TemplateTab.cpp
)

if (WIN32)

    #set bundle type
    SET(FEEDTNZ_BUNDLE_TYPE WIN32)

    #add libs specific to win
    list(APPEND FEEDTNZ_ENTRYPOINT 
        libs/itunescom/iTunesCOMInterface_i.c
        src/workers/shout/win/iTunesCOMHandler.cpp
    )
 
    #COM Qt libs
    find_package(Qt5AxBase REQUIRED)
    find_package(Qt5AxContainer REQUIRED)
    list(APPEND QT_LIBS_TO_INCLUDE 
        Qt5::AxBase
        Qt5::AxContainer
    )

    #add icon
    SET(FEEDTNZ_ICO_WIN resources/resources.rc)

endif (WIN32)

if (APPLE)

    #forcing target due to a bug from Qt5.12.0, fixed in 5.12.1
    SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)

    #set bundle type
    SET(FEEDTNZ_BUNDLE_TYPE MACOSX_BUNDLE)

    #include Object C++ files to compile
    #SET(CMAKE_EXE_LINKER_FLAGS "-framework AppKit -w")
    #list(APPEND FEEDTNZ_ENTRYPOINT src/ui/mac/nativeEvents.mm)

    #add icon
    SET(MACOSX_BUNDLE_ICON_FILE feedtnz.icns)
    SET(FEEDTNZ_ICO_WIN resources/icons/feedtnz.icns)
    set_source_files_properties(${FEEDTNZ_ICO_WIN} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

endif (APPLE)

add_executable(${PROJECT_NAME} ${FEEDTNZ_BUNDLE_TYPE} ${FEEDTNZ_ENTRYPOINT} ${FEEDTNZ_RES} ${FEEDTNZ_ICO_WIN}) 

#add libs
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} ${QT_LIBS_TO_INCLUDE})


if (APPLE)

    #add highdpi support via plist template
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/Info.plist.in
    )
    
endif (APPLE)