# policies first
if(COMMAND cmake_policy)
    # cmake_policy(SET CMP0011 NEW)
    # cmake_policy(SET CMP0020 NEW)
    # cmake_policy(SET CMP0003 NEW)
endif()

# use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message("== Using CCache ! ==")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# default configuration
list(APPEND CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Qt
)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

#############
## STARTUP ##
#############

cmake_minimum_required(VERSION 3.14.3)

project(SoundBuddy
    VERSION 0.4.1
    DESCRIPTION "Small companion app that feeds SoundVitrine with ITunes / Music library metadata"
    HOMEPAGE_URL "https://github.com/Amphaal/SoundBuddy"
    LANGUAGES CXX
)

########################
## VERSION.H HANDLING ##
########################

#app dest path
SET(APP_PUBLISHER "LVWL")
SET(DEST_PLATFORM_PRODUCT_NAME "SoundVitrine")
SET(APP_PATCHNOTE_URL ${PROJECT_HOMEPAGE_URL}/releases)
SET(APP_REMOTE_MANIFEST_URL "")

#config file to source code
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/_version.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
)

################################
## CPP Compiler Configuration ##
################################

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_C_STANDARD 11)

# Qt
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets
    WebSockets
    Network
    LinguistTools
)

if(WIN32)
    find_package(Qt6 REQUIRED COMPONENTS 
        ActiveQt
        AxContainer
    )
endif()

##########################
## Bundle configuration ##
##########################

if (WIN32)
    #set bundle type
    SET(BUNDLE_TYPE WIN32) 
    #add icon
    SET(APP_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.rc) 
endif()

if (APPLE)
    #set bundle type
    SET(BUNDLE_TYPE MACOSX_BUNDLE) 
    #add icons
    SET(MACOSX_BUNDLE_ICON_FILE "app.icns")
    SET(APP_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app.icns)
    set_source_files_properties(${APP_ICO} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources"                                                    
    )
endif()

####################
# Code compilation #
####################

file(GLOB_RECURSE APP_CPP_FILES "src/*.cpp")
file(GLOB_RECURSE APP_HPP_FILES "src/*.hpp")

#########################
# Executable Generation #
#########################

# Create executable
add_executable(appExec ${BUNDLE_TYPE}
    ${APP_HPP_FILES}
    ${APP_CPP_FILES}
    ${APP_ICO}
    resources/resources.qrc
) 

target_include_directories(appExec PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#
set_target_properties(appExec PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

#
target_compile_definitions(appExec PRIVATE $<$<CONFIG:Debug>:_DEBUG>) #define _DEBUG markup if in DEBUG config

if (APPLE)
    #add highdpi support / menu behavior via plist template (MACOS)
    set_target_properties(appExec PROPERTIES 
        BUNDLE                              ON
        MACOSX_BUNDLE_INFO_PLIST            ${CMAKE_CURRENT_SOURCE_DIR}/templates/Info.plist
        MACOSX_BUNDLE_GUI_IDENTIFIER        com.${APP_PUBLISHER}.${CMAKE_PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME           ${CMAKE_PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION        ${CMAKE_PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING  ${CMAKE_PROJECT_VERSION} 
    ) 
endif()

################
# translations #
################

include(QtTranslation)

# app sources
list(APPEND TS_SOURCE_FILES
    ${APP_CPP_FILES}
    ${APP_HPP_FILES}
)
list(APPEND TS_FILES
    "src/_i18n/fr.ts"
)
HandleQtTranslation(appExec
    "App"
    "${TS_SOURCE_FILES}"
    ${TS_FILES}
)

# IFW sources
list(APPEND TS_INSTALL_SOURCE_FILES
    "src/_ifw/EndInstallerForm.ui"
)
list(APPEND TS_INSTALL_FILES
    "src/_i18n/EndInstallerForm_fr.ts"
)
HandleQtTranslation(appExec
    OFF
    "${TS_INSTALL_SOURCE_FILES}"
    ${TS_INSTALL_FILES}
)

####################
# IFWUpdateChecker #
####################

add_subdirectory(deps/IFWUpdateChecker)

# https://github.com/chriskohlhoff/asio/pull/910
target_compile_definitions(IFWUpdateChecker 
    INTERFACE 
        ASIO_HAS_STD_INVOKE_RESULT=1
)

target_link_libraries(appExec PRIVATE
    IFWUpdateChecker
)

#######################
# ITunesLibraryParser #
#######################

add_subdirectory(deps/ITunesLibraryParser)

target_link_libraries(appExec PRIVATE
    itunes-library-parser
)

########
# QT 6 #
########

target_link_libraries(appExec PRIVATE
    Qt6::Core
    Qt6::Gui 
    Qt6::Widgets
    Qt6::WebSockets
    Qt6::Network
)

if(WIN32)
    target_link_libraries(appExec PRIVATE
        Qt6::ActiveQt
        Qt6::AxContainer
    )
endif()

# QT LIBRARIES COPY
include(DeployQt)
DeployQt(appExec)

######################
# add Publish target #
######################

include(CMakePublish)