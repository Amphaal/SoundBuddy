##########################
### CPACK CONFIGURATION ##
##########################

SET(CPACK_GENERATOR IFW)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})

SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
SET(CPACK_IFW_PACKAGE_PUBLISHER ${APP_PUBLISHER_ORG})
SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${APP_PUBLISHER_ORG})
SET(CPACK_IFW_PRODUCT_URL ${APP_PATCHNOTE_URL})
SET(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDirX64@/${PROJECT_NAME}")

IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".exe")
elseif(APPLE)
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".dmg")
endif()

# icons
SET(CPACK_IFW_PACKAGE_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_64.png")
SET(CPACK_IFW_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/package.ico")


# https://cmake.org/cmake/help/v3.2/module/CPackBundle.html#variable:CPACK_BUNDLE_APPLE_CERT_APP
# https://github.com/ziglang/zig/issues/6971
# Since recent SDK, if not signed, any executable will silently crash
SET(CPACK_BUNDLE_APPLE_CERT_APP ${APPLE_CERT_APP_CODE_SIGN_IDENTITY})

##############
### INSTALL ##
##############

SET(CPACK_IFW_VERBOSE ON)
INCLUDE(CPack)
INCLUDE(CPackIFW)

# https://gitlab.kitware.com/cmake/cmake/-/issues/20072
SET(CPACK_NSIS_INSTALL_ROOT "C:\\\\Program Files\\\\${APP_PUBLISHER_ORG}" CACHE STRING "..." FORCE)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}" CACHE STRING "..." FORCE)
SET(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME} ${CMAKE_PROJECT_VERSION}" CACHE STRING "..." FORCE)

# App
SET(CPACK_COMPONENT_App_DISPLAY_NAME  "${PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
SET(CPACK_COMPONENT_App_DESCRIPTION   ${PROJECT_DESCRIPTION})
cpack_add_component(
    "App" REQUIRED #DOWNLOADED
    DISPLAY_NAME ${CPACK_COMPONENT_App_DISPLAY_NAME}
    DESCRIPTION ${CPACK_COMPONENT_App_DESCRIPTION}
)

    # > IFW
    cpack_ifw_configure_component("App"
        DISPLAY_NAME 
            ${CPACK_COMPONENT_App_DISPLAY_NAME}
            fr ${CPACK_COMPONENT_App_DISPLAY_NAME}
        DESCRIPTION 
            ${CPACK_COMPONENT_App_DESCRIPTION}
            fr ${CPACK_COMPONENT_App_DESCRIPTION}
        SCRIPT "ifw/EndInstallerForm.js"
        SORTING_PRIORITY 1000
        USER_INTERFACES "ifw/EndInstallerForm.ui"
        TRANSLATIONS "ifw/i18n/fr.qm" # as IFW binary translation file must be in file system when configuring, ensure copy of said file
        FORCED_INSTALLATION
    )


# Runtime
SET(CPACK_COMPONENT_AppRuntime_DISPLAY_NAME "Runtime")
SET(CPACK_COMPONENT_AppRuntime_DESCRIPTION "Essential components used by ${PROJECT_NAME} and other libraries")
cpack_add_component(
    "AppRuntime" REQUIRED #DOWNLOADED
    DISPLAY_NAME ${CPACK_COMPONENT_AppRuntime_DISPLAY_NAME}
    DESCRIPTION ${CPACK_COMPONENT_AppRuntime_DESCRIPTION}
)

    # > IFW
    cpack_ifw_configure_component("AppRuntime"
        DISPLAY_NAME 
            ${CPACK_COMPONENT_AppRuntime_DISPLAY_NAME} 
            fr "Composants de base"
        DESCRIPTION 
            ${CPACK_COMPONENT_AppRuntime_DESCRIPTION}
            fr "Composants essentiels utilisés par ${PROJECT_NAME} et autres librairies"
        SORTING_PRIORITY 900
        VERSION "1.0.0"
        FORCED_INSTALLATION
    )

# Qt
SET(CPACK_COMPONENT_QtRuntime_DISPLAY_NAME "Qt ${Qt6Core_VERSION}")
SET(CPACK_COMPONENT_QtRuntime_DESCRIPTION "Essential framework used by ${PROJECT_NAME}")
cpack_add_component(
    "QtRuntime" REQUIRED #DOWNLOADED
    DISPLAY_NAME ${CPACK_COMPONENT_QtRuntime_DISPLAY_NAME}
    DESCRIPTION ${CPACK_COMPONENT_QtRuntime_DESCRIPTION}
)

    # > IFW
    cpack_ifw_configure_component("QtRuntime"
        DISPLAY_NAME
            ${CPACK_COMPONENT_QtRuntime_DISPLAY_NAME}
            fr ${CPACK_COMPONENT_QtRuntime_DISPLAY_NAME}
        DESCRIPTION 
            ${CPACK_COMPONENT_QtRuntime_DESCRIPTION}
            fr "Framework essentiel utilisé par ${PROJECT_NAME}"
        SORTING_PRIORITY 98
        VERSION ${Qt6Core_VERSION}
        FORCED_INSTALLATION
    )

######################################
# CPACK IFW COMPONENTS CONFIGURATION #
######################################

#
SET(APP_REMOTE_IFW_DIR "https://ifw.to2x.ovh/${PROJECT_NAME}/${CPACK_SYSTEM_NAME}")
SET(APP_REMOTE_MANIFEST_URL ${APP_REMOTE_IFW_DIR}/Updates.xml)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/_version.h
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
)

# repository for updates
#cpack_ifw_add_repository(coreRepo 
#    URL ${APP_REMOTE_IFW_DIR}
#)

########################
## ZIP FOR DEPLOYMENT ## 
########################

# source
SET(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}) #override as CPACK_SYSTEM_NAME may end up wrong (CMAKE bug?)
SET(CPACK_PACKAGE_FILE_NAME_FULL ${CPACK_PACKAGE_FILE_NAME}${CPACK_IFW_PACKAGE_FILE_EXTENSION})

SET(CPACK_PACKAGES_DIR ${CMAKE_BINARY_DIR}/_CPack_Packages)
SET(APP_REPOSITORY ${CPACK_PACKAGES_DIR}/${CPACK_SYSTEM_NAME}/IFW/${CPACK_PACKAGE_FILE_NAME}/repository)

SET(APP_PACKAGE_LATEST ${CPACK_PACKAGE_NAME}-latest-${CPACK_SYSTEM_NAME})
SET(APP_PACKAGE_LATEST_FULL ${APP_PACKAGE_LATEST}${CPACK_IFW_PACKAGE_FILE_EXTENSION})

# create target to be invoked with bash
add_custom_target(zipForDeploy DEPENDS package)

# zip repository
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/repository.zip --format=zip .
    WORKING_DIRECTORY ${APP_REPOSITORY}
    COMMENT "Ziping IFW repository..."
)

# zip installer
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E rename ${CPACK_PACKAGE_FILE_NAME_FULL} ${APP_PACKAGE_LATEST_FULL}
    COMMAND ${CMAKE_COMMAND} -E tar c installer.zip --format=zip ${APP_PACKAGE_LATEST_FULL}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Ziping IFW installer..."
)

# cleanup
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E rm -r
        ${CPACK_PACKAGES_DIR} 
        ${APP_PACKAGE_LATEST_FULL}
    COMMENT "Cleanup CPack files..."
)