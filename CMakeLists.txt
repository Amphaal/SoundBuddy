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

project(FeedTNZ
    VERSION 0.4.1
    DESCRIPTION "Small companion app for desktop to interact with WTNZ"
    HOMEPAGE_URL "https://github.com/Amphaal/feedtnz"
    LANGUAGES CXX
)

########################
## VERSION.H HANDLING ##
########################

#app dest path
SET(APP_PUBLISHER "LVWL")
SET(APP_ROOT_URL "https://zonme.to2x.ovh")
SET(APP_MAIN_URL ${APP_ROOT_URL}/wtnz)
SET(PEM_CERT_NAME "cacert.pem")

#set default maintenance tool path
SET(MAINTENANCE_TOOL_LOCATION "")

#config file to source code
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/version.h.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
)

#download cacert file to source code
file(DOWNLOAD 
    "https://crt.sh/?d=0687260331A72403D909F105E69BCF0D32E1BD2493FFC6D9206D11BCD6770739" 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${PEM_CERT_NAME}
)
SET(CACERT src/${PEM_CERT_NAME})
get_filename_component(CACERT_FP ${CACERT} ABSOLUTE)

################################
## CPP Compiler Configuration ##
################################

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

if (APPLE)
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) #prevent linking errors once app shared
endif (APPLE)

if(MSVC)

    add_definitions(-DWIN32_LEAN_AND_MEAN) #std::btye c++17 ambiguity

    #ignore more
    add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING) #rapidjson
    add_definitions(-DNOMINMAX) #rapidjson
    add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING) #socketio
    add_definitions(-D_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING) #socketio
    
endif(MSVC)

#########################
## Libraries specifics ##
#########################

#Qt
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON) #for moc auto 

if (WIN32)
    SET(Qt5_DIR "C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5")
endif (WIN32)
if (APPLE)
    SET(Qt5_DIR "/Qt/5.13.0/clang_64/lib/cmake/Qt5")
endif (APPLE)

##########################
## Bundle configuration ##
##########################

if (WIN32)
    #set bundle type
    SET(BUNDLE_TYPE WIN32) 
    #add icon
    SET(APP_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.rc) 
endif (WIN32)

if (APPLE)
    #set bundle type
    SET(BUNDLE_TYPE MACOSX_BUNDLE) 
    #add icons
    SET(MACOSX_BUNDLE_ICON_FILE "app.icns")
    SET(APP_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app.icns)
    set_source_files_properties(${APP_ICO} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources"                                                    
    )
    #CACERT
    set_source_files_properties(${CACERT} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "MacOS"                                                    
    )
endif (APPLE)

#default source contains
SET(SOURCES
    ${APP_ICO}
    ${CACERT}
    resources/resources.qrc
)

####################
# Code compilation #
####################

# Define required linked QT libs
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
SET(EXT_LIBS 
    Qt5::Widgets 
    Qt5::Core
    Qt5::Gui
)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE HPP_FILES src/*.hpp)

list(APPEND SOURCES
    ${CPP_FILES}
    ${HPP_FILES}
)

#win specifics
if (WIN32)
    
    #COM Qt libs
    find_package(Qt5 REQUIRED COMPONENTS AxBase AxContainer)
    list(APPEND EXT_LIBS 
        Qt5::AxBase
        Qt5::AxContainer
    )

endif (WIN32)

#########################
# Executable Generation #
#########################

# Create executable
add_executable(${PROJECT_NAME} ${BUNDLE_TYPE} ${SOURCES} ${INT_LIBS}) 

#
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_DEBUG>) #define _DEBUG markup if in DEBUG config

#ignore warnings(WINDOWS)
if (WIN32)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES 
        LINK_FLAGS "/ignore:4099 /ignore:4267 /ignore:4244 /ignore:4715 /ignore:4267"
    )
endif (WIN32)

#add highdpi support / menu behavior via plist template (MACOS)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/templates/Info.plist.in     
) 

###
### LIBRARIES COPY
###

#default
add_custom_command(TARGET ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

#apple
if(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME}
        #UI plugin
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:Qt5::QCocoaIntegrationPlugin> 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/$<TARGET_FILE_NAME:Qt5::QCocoaIntegrationPlugin>
    )
endif(APPLE)

#win
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
        #CA CERT
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CACERT_FP} $<TARGET_FILE_DIR:${PROJECT_NAME}>
        #UI plugin
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/$<TARGET_FILE_NAME:Qt5::QWindowsIntegrationPlugin>
    )
endif (WIN32)


######################
# add Publish target #
######################

include(CMakePublish)