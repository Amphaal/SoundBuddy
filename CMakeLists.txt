project(FeedTNZ)
cmake_minimum_required(VERSION 3.1.0)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_INCLUDE_CURRENT_DIR YES)

if (APPLE)
    #forcing target due to a bug from Qt5.12.0, fixed in 5.12.1
    SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)

    #prevent linking errors once app shared
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

endif (APPLE)

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
    src/helpers/platformHelper/platformHelper.cpp
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
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} ${QT_LIBS_TO_INCLUDE})

if (APPLE)
    #add highdpi support via plist template
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/build_templates/Info.plist.in
    ) 
endif (APPLE)

############################
### CPACK CONFIG + Qt ITW ##
############################

if (APPLE)
    INSTALL(TARGETS ${PROJECT_NAME} DESTINATION .)
endif (APPLE)

if (WIN32)
    INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION .)
endif (WIN32)


SET(CPACK_MONOLITHIC_INSTALL 1)

SET(CPACK_PACKAGE_VERSION_MAJOR 0)
SET(CPACK_PACKAGE_VERSION_MINOR 1)
SET(CPACK_PACKAGE_VERSION_PATCH 1)

SET(FEEDTNZ_DESCRIPTION "Companion app for WTNZ")
SET(FEEDTNZ_DESCRIPTION_LOCALIZED ${FEEDTNZ_DESCRIPTION}
    fr "Logiciel compagnon pour WTNZ"
)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${FEEDTNZ_DESCRIPTION})
SET(CPACK_IFW_PACKAGE_PUBLISHER "LVWL")
SET(CPACK_IFW_PRODUCT_URL "https://zonme.to2x.ovh/wtnz")
SET(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDirX64@/FeedTNZ")


if (APPLE)
    SET(CPACK_IFW_ROOT "/Qt/QtIFW-3.0.6")
    SET(FEEDTNZ_REMOTE_SERVER_PATH "/Volumes/www/feedtnz")
    SET(FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER "osx")
    SET(FEEDTNZ_INSTALLER_EXTENSION ".dmg")
    SET(MOVE_COMMAND "move")
endif (APPLE)

if (WIN32)
    SET(CPACK_IFW_ROOT "C:/Qt/QtIFW-3.0.6")
    SET(FEEDTNZ_REMOTE_SERVER_PATH "//192.168.0.12/www/feedtnz")
    SET(FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER "win")
    SET(FEEDTNZ_INSTALLER_EXTENSION ".exe")
    SET(MOVE_COMMAND "mv")
endif (WIN32)

SET(FEEDTNZ_REMOTE_SERVER_DOWNLOAD_PATH ${FEEDTNZ_REMOTE_SERVER_PATH}/downloads/${FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER})
SET(FEEDTNZ_REMOTE_SERVER_PACKAGES_PATH ${FEEDTNZ_REMOTE_SERVER_PATH}/packages/${FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER})

SET(CPACK_GENERATOR IFW)
INCLUDE(CPack)
INCLUDE(CPackIFW)

cpack_add_component(${PROJECT_NAME}
                    DISPLAY_NAME "FeedTNZ"
                    DESCRIPTION ${FEEDTNZ_DESCRIPTION}
                    REQUIRED
)

cpack_ifw_configure_component(${PROJECT_NAME} 
                    ESSENTIAL
                    FORCED_INSTALLATION
                    NAME "com.lvwl.feedtnz.core"
                    DESCRIPTION ${FEEDTNZ_DESCRIPTION_LOCALIZED}
                    DEFAULT TRUE
)

cpack_ifw_add_repository(coreRepo URL "https://zonme.to2x.ovh/feedtnz/packages/${FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER}")

#############
## Publish ## 
#############

SET(FEEDTNZ_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
SET(FEEDTNZ_PACKAGED_PATH "${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_SYSTEM_NAME}/IFW/${FEEDTNZ_PACKAGE_FILE_NAME}")
SET(FEEDTNZ_PACKAGED_INSTALLER_PATH "${FEEDTNZ_PACKAGED_PATH}${FEEDTNZ_INSTALLER_EXTENSION}")
SET(FEEDTNZ_PACKAGED_REPOSITORY_PATH "${FEEDTNZ_PACKAGED_PATH}/repository")

#install CoreUtils for Win32 if mv missing
add_custom_target(publishPackage 
    COMMAND "${CMAKE_COMMAND}" --build . --config Release --target package
    COMMAND cp -rf ${FEEDTNZ_PACKAGED_INSTALLER_PATH} ${FEEDTNZ_REMOTE_SERVER_DOWNLOAD_PATH}
    COMMAND cp -rf ${FEEDTNZ_PACKAGED_REPOSITORY_PATH}/* ${FEEDTNZ_REMOTE_SERVER_PACKAGES_PATH}
)