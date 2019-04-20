############################
### CPACK CONFIG + Qt ITW ##
############################

#configure IFW
SET(CPACK_GENERATOR IFW)

#define install
install(TARGETS ${PROJECT_NAME}
DESTINATION .
COMPONENT app)

set(CPACK_COMPONENTS_ALL app)

###################
# MAIN DEFINITION #
###################

    SET(APP_DESCRIPTION ${PROJECT_NAME}
        fr "Logiciel compagnon pour WTNZ"
    )

    SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})
    SET(CPACK_IFW_PACKAGE_PUBLISHER ${APP_PUBLISHER})
    SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${APP_PUBLISHER})
    SET(CPACK_IFW_PRODUCT_URL ${APP_MAIN_URL})
    SET(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDirX64@/${PROJECT_NAME}")

    if (APPLE)
        SET(CPACK_IFW_ROOT "/Qt/QtIFW-3.0.6")
        SET(APP_REMOTE_SERVER_PATH "/Volumes/www/feedtnz")
        SET(APP_REMOTE_SERVER_PLATFORM_FOLDER "osx")
        SET(APP_INSTALLER_EXTENSION ".dmg")
        SET(APP_ICON_EXT ".icns")
    endif (APPLE)

    if (WIN32)
        SET(CPACK_IFW_ROOT "C:/Qt/QtIFW-3.0.6")
        SET(APP_REMOTE_SERVER_PATH "//192.168.0.12/www/feedtnz")
        SET(APP_REMOTE_SERVER_PLATFORM_FOLDER "win")
        SET(APP_INSTALLER_EXTENSION ".exe")
        SET(APP_ICON_EXT ".ico")
    endif (WIN32)

    #icons
    SET(CPACK_IFW_PACKAGE_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_64.png")
    SET(CPACK_IFW_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/package${APP_ICON_EXT}")

#componenent
INCLUDE(CPackIFW)

cpack_add_component(app
                    DISPLAY_NAME ${PROJECT_NAME}
                    DESCRIPTION ${APP_DESCRIPTION}
)

#additionnal configuration
cpack_ifw_configure_component(app
                    ESSENTIAL
                    FORCED_INSTALLATION
                    NAME "com.lvwl.feedtnz"
                    DESCRIPTION ${APP_DESCRIPTION}
                    VERSION ${CPACK_PACKAGE_VERSION} 
                    SCRIPT "src/ifw/install.js"
                    USER_INTERFACES "src/ifw/install.ui"
                    DEFAULT TRUE
)

#remote repo
cpack_ifw_add_repository(coreRepo URL "${APP_ROOT_URL}/feedtnz/packages/${APP_REMOTE_SERVER_PLATFORM_FOLDER}")

INCLUDE(CPack)

#############
## Publish ## 
#############

#target
SET(APP_REMOTE_SERVER_DOWNLOAD_PATH "${APP_REMOTE_SERVER_PATH}/downloads/${APP_REMOTE_SERVER_PLATFORM_FOLDER}")
SET(APP_REMOTE_SERVER_PACKAGES_PATH "${APP_REMOTE_SERVER_PATH}/packages/${APP_REMOTE_SERVER_PLATFORM_FOLDER}")

#source
SET(APP_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
SET(APP_PACKAGED_PATH "${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_SYSTEM_NAME}/IFW/${APP_PACKAGE_FILE_NAME}")
SET(APP_PACKAGED_INSTALLER_PATH "${APP_PACKAGED_PATH}${APP_INSTALLER_EXTENSION}")
SET(APP_PACKAGED_REPOSITORY_PATH "${APP_PACKAGED_PATH}/repository")

#install CoreUtils for Win32 if mv missing
add_custom_target(publishPackage 
    #build
    COMMAND ${CMAKE_COMMAND} --build . --config Release --target package

    #copy to dest
    COMMAND ${CMAKE_COMMAND} -E copy ${APP_PACKAGED_INSTALLER_PATH} ${APP_REMOTE_SERVER_DOWNLOAD_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${APP_PACKAGED_REPOSITORY_PATH} ${APP_REMOTE_SERVER_PACKAGES_PATH}
)