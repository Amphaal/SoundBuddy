project(FeedTNZ)
cmake_minimum_required(VERSION 3.13.3)

#download cacert for windows mostly
##too heavy, prefer specific intermediate local 
#SET(CACERT_URL_SOURCE "https://curl.haxx.se/ca/cacert.pem")
##DST Root CA X3 only, from Firefox's "https://ccadb-public.secure.force.com/mozilla/IncludedCACertificateReport"
SET(CACERT_URL_SOURCE "https://crt.sh/?d=0687260331A72403D909F105E69BCF0D32E1BD2493FFC6D9206D11BCD6770739")
SET(PEM_CERT_NAME "cacert.pem")
file(DOWNLOAD ${CACERT_URL_SOURCE} ${CMAKE_BINARY_DIR}/bin/${PEM_CERT_NAME})

########################
## VERSION.H HANDLING ##
########################

#app dest path
SET(APP_ROOT_URL "https://zonme.to2x.ovh")
SET(APP_MAIN_URL ${APP_ROOT_URL}/wtnz)

#version
SET(CMAKE_PROJECT_VERSION_MAJOR 0)
SET(CMAKE_PROJECT_VERSION_MINOR 2)
SET(CMAKE_PROJECT_VERSION_PATCH 6)


#set default maintenance tool path
SET(MAINTENANCE_TOOL_LOCATION "")
SET(IS_DEBUG_APP OFF)

IF(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/buildDebug)

    SET(IS_DEBUG_APP ON)

    if (APPLE)
        SET(MAINTENANCE_TOOL_LOCATION "/Qt/MaintenanceTool")
    endif(APPLE)
    if (WIN32)
        SET(MAINTENANCE_TOOL_LOCATION "C:/Qt/MaintenanceTool")
    endif(WIN32)
ENDIF()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/version.h.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
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


##########################
## Bundle configuration ##
##########################

if (WIN32)
    SET(BUNDLE_TYPE WIN32) #set bundle type
    SET(FEEDTNZ_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.rc) #add icon
endif (WIN32)

if (APPLE)
    SET(BUNDLE_TYPE MACOSX_BUNDLE) #set bundle type

    #add icons
    SET(MACOSX_BUNDLE_ICON_FILE feedtnz.icns)
    SET(FEEDTNZ_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/feedtnz.icns)

endif (APPLE)

#default source contains
SET(SOURCES
    ${FEEDTNZ_ICO}
    resources/resources.qrc
)

####################
# Code compilation #
####################

# Define required linked QT libs
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
SET(EXT_LIBS 
    Qt5::Widgets 
    Qt5::Core
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

if (WIN32)    
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099 /ignore:4267 /ignore:4244 /ignore:4715")
endif (WIN32)

if(APPLE)

    #defines icns localization into the package
    set_source_files_properties(${FEEDTNZ_ICO} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources"                                                    
    )

    #add highdpi support / menu behavior via plist template
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/Info.plist.in     
    ) 

endif(APPLE)


# Bind ext libs
conan_target_link_libraries(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${EXT_LIBS})


######################
# add Publish target #
######################

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakePublish.cmake)