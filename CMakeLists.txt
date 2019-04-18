project(FeedTNZ)
cmake_minimum_required(VERSION 3.13.3)

########################
## VERSION.H HANDLING ##
########################

#app dest path
SET(APP_PUBLISHER "LVWL")
SET(APP_ROOT_URL "https://zonme.to2x.ovh")
SET(APP_MAIN_URL ${APP_ROOT_URL}/wtnz)
SET(PEM_CERT_NAME "cacert.pem")

#version
SET(CMAKE_PROJECT_VERSION_MAJOR 0)
SET(CMAKE_PROJECT_VERSION_MINOR 3)
SET(CMAKE_PROJECT_VERSION_PATCH 2)

#set default maintenance tool path
SET(MAINTENANCE_TOOL_LOCATION "")
SET(IS_DEBUG_APP OFF)

#if debug build
IF(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/buildDebug)

    #define debug
    SET(IS_DEBUG_APP ON)

    #define maintenance tool location
    if (APPLE)
        SET(MAINTENANCE_TOOL_LOCATION "/Qt/MaintenanceTool")
    endif(APPLE)
    if (WIN32)
        SET(MAINTENANCE_TOOL_LOCATION "C:/Qt/MaintenanceTool")
    endif(WIN32)

ENDIF()

#config file to source code
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/version.h.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
)

#download cacert file to source code
file(DOWNLOAD 
    "https://crt.sh/?d=0687260331A72403D909F105E69BCF0D32E1BD2493FFC6D9206D11BCD6770739" 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${PEM_CERT_NAME}
)
SET(CACERT src/${PEM_CERT_NAME})
get_filename_component(CACERT_FP ${CACERT} ABSOLUTE)

##################
## Enable Conan ##
##################

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

################################
## CPP Compiler Configuration ##
################################

SET(CMAKE_CXX_STANDARD 17)

if (APPLE)
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)     #prevent linking errors once app shared
endif (APPLE)

if(MSVC)
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
    SET(Qt5_DIR "C:/Qtd/5.12.2/msvc2017_64/lib/cmake/Qt5")
endif (WIN32)
if (APPLE)
    SET(Qt5_DIR "$ENV{HOME}/Qt/5.12.2/clang_64/lib/cmake/Qt5")
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

#private libs
file(GLOB_RECURSE INT_LIBS
    "libs/*.c"
    "libs/*.cpp"
    "libs/*.hpp"
)

#mandatory explicit generation
list(APPEND SOURCES
    src/helpers/platformHelper/platformHelper.cpp
    src/helpers/iTunesLibParser/iTunesLibParser.cpp
    src/workers/base/ITNZThread.h
    src/workers/base/IConnectivityWorker.h
    src/workers/connectivity/sio.cpp
    src/workers/shout/shout.cpp
    src/ui/tabs/base/TemplateTab.cpp 
    src/ui/widgets/LightWidget.cpp
    src/ui/mainWindow/parts/mainWindowUI.cpp
    src/ui/mainWindow/mainWindow.cpp
    src/main.cpp
)

#win specifics
if (WIN32)
    
    #COM iTunes libs
    list(APPEND SOURCES 
        src/workers/shout/win/iTunesCOMHandler.cpp
    )
 
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
 
#ignore warnings(WINDOWS)
set_target_properties(
    ${PROJECT_NAME} PROPERTIES 
    LINK_FLAGS "/ignore:4099 /ignore:4267 /ignore:4244 /ignore:4715 /ignore:4267"
)

#add highdpi support / menu behavior via plist template (MACOS)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/Info.plist.in     
) 

# Bind ext libs
conan_target_link_libraries(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${EXT_LIBS})

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

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakePublish.cmake)
