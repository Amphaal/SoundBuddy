project(FeedTNZ)
cmake_minimum_required(VERSION 3.13.3)

######################
## VERSION HANDLING ##
######################

SET(CMAKE_PROJECT_VERSION_MAJOR 0)
SET(CMAKE_PROJECT_VERSION_MINOR 1)
SET(CMAKE_PROJECT_VERSION_PATCH 1)

#set default maintenance tool path
SET(MAINTENANCE_TOOL_LOCATION "")
IF(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/buildDebug)
    if (APPLE)
        SET(MAINTENANCE_TOOL_LOCATION "/Qt/MaintenanceTool")
    endif(APPLE)
    if (WIN32)
        SET(MAINTENANCE_TOOL_LOCATION "C:/Qt/MaintenanceTool")
    endif(WIN32)
ENDIF()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/version.h.in 
    ${CMAKE_BINARY_DIR}/version.h
)

##################
## Enable Conan ##
##################

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
conan_basic_setup(KEEP_RPATHS)

################################
## CPP Compiler Configuration ##
################################

SET(CMAKE_CXX_STANDARD 17)

if (APPLE)
    SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)          #forcing target due to a bug from Qt5.12.0, fixed in 5.12.1
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)     #prevent linking errors once app shared
endif (APPLE)

if(MSVC)
    add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif(MSVC)

#########################
## Libraries specifics ##
#########################

#Qt
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)


##########################
## Bundle configuration ##
##########################

if (WIN32)
    SET(BUNDLE_TYPE WIN32) #set bundle type
    SET(FEEDTNZ_ICO resources/resources.rc) #add icon
endif (WIN32)

if (APPLE)
    SET(BUNDLE_TYPE MACOSX_BUNDLE) #set bundle type

    #add icons
    SET(MACOSX_BUNDLE_ICON_FILE feedtnz.icns)
    SET(FEEDTNZ_ICO resources/icons/feedtnz.icns)
    
    #defines icns localization into the package
    set_source_files_properties(${FEEDTNZ_ICO} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources"                                                    
    )

    #add highdpi support / menu behavior via plist template
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/Info.plist.in     
    ) 

endif (APPLE)

#default source contains
SET(SOURCES
    resources/resources.qrc
    ${FEEDTNZ_ICO}
)

#########################
# Executable Generation #
#########################

# Define required linked QT libs
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Pugixml REQUIRED)
SET(EXT_LIBS 
    Qt5::Widgets 
    Qt5::Core
    pugixml
)

#add local libs to generation
list(APPEND SOURCES 
    libs/plistcpp/Plist.cpp 
    libs/plistcpp/PlistDate.cpp
    libs/plistcpp/NSPlist.cpp
    libs/plistcpp/NSPlistParser.cpp
    libs/plistcpp/NSPlistLexer.cpp
)
list(APPEND SOURCES 
    libs/qtautoupdater/autoupdatercore/simplescheduler.cpp
    libs/qtautoupdater/autoupdatercore/updater.cpp
    libs/qtautoupdater/autoupdatercore/updater_p.cpp
)

#mandatory explicit generation
list(APPEND SOURCES
    src/feedtnz.cpp
    src/helpers/platformHelper/platformHelper.cpp
    src/workers/base/ITNZThread.h
    src/workers/shout/shout.cpp
    src/ui/mainWindow.cpp
    src/ui/tabs/base/TemplateTab.cpp
)

#win specifics
if (WIN32)

    #COM iTunes libs
    list(APPEND SOURCES 
        libs/itunescom/iTunesCOMInterface_i.c
        src/workers/shout/win/iTunesCOMHandler.cpp
    )
 
    #COM Qt libs
    find_package(Qt5 REQUIRED COMPONENTS AxBase AxContainer)
    list(APPEND EXT_LIBS 
        Qt5::AxBase
        Qt5::AxContainer
    )

endif (WIN32)

# Create executable
add_executable(${PROJECT_NAME} ${BUNDLE_TYPE} ${SOURCES}) 

# Bind ext libs
target_link_libraries(${PROJECT_NAME} ${EXT_LIBS})


######################
# add Publish target #
######################

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakePublish.cmake)