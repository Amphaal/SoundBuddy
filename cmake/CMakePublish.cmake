##########################
### CPACK CONFIGURATION ##
##########################

SET(CPACK_GENERATOR IFW)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})

SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
SET(CPACK_IFW_PACKAGE_PUBLISHER ${APP_PUBLISHER})
SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${APP_PUBLISHER})
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

##############
### INSTALL ##
##############

SET(CPACK_IFW_VERBOSE ON)
INCLUDE(CPack)
INCLUDE(CPackIFW)

# App
cpack_add_component("App" DOWNLOADED)
cpack_ifw_configure_component("App"
    DISPLAY_NAME "${PROJECT_NAME} ${CMAKE_PROJECT_VERSION}"
    DESCRIPTION 
        ${PROJECT_DESCRIPTION}
        fr "L'experience JdR simplifiée !"
    SCRIPT "src/_ifw/EndInstallerForm.js"
    SORTING_PRIORITY 1000
    USER_INTERFACES "src/_ifw/EndInstallerForm.ui"
    # TRANSLATIONS ${CMAKE_BINARY_DIR}/EndInstallerForm_fr.qm
    FORCED_INSTALLATION
)

# Runtime 
cpack_add_component("Runtime" DOWNLOADED)
cpack_ifw_configure_component("Runtime"
    DISPLAY_NAME 
        "Runtime" 
        fr "Composants de base"
    DESCRIPTION 
        "Essential components used by ${PROJECT_NAME} and other libraries"
        fr "Composants essentiels utilisés par ${PROJECT_NAME} et autres librairies"
    SORTING_PRIORITY 900
    VERSION "1.0.0"
    FORCED_INSTALLATION
)

# AudioTube
cpack_add_component("AudioTube" DOWNLOADED)
cpack_ifw_configure_component("AudioTube"
    DISPLAY_NAME "AudioTube ${AUDIOTUBE_PROJECT_VERSION}"
    DESCRIPTION 
        "Component that allows fetching Youtube audio sources"
        fr "Composant qui permet la récupération de flux audio depuis Youtube"
    SORTING_PRIORITY 99
    VERSION ${AUDIOTUBE_PROJECT_VERSION}
    FORCED_INSTALLATION
)

# Qt
cpack_add_component("Qt" DOWNLOADED)
cpack_ifw_configure_component("Qt"
    DISPLAY_NAME "Qt ${Qt6Core_VERSION_STRING}"
    DESCRIPTION 
        "Essential framework used by ${PROJECT_NAME}"
        fr "Framework essentiel utilisé par ${PROJECT_NAME}"
    SORTING_PRIORITY 98
    VERSION ${Qt6Core_VERSION_STRING}
    FORCED_INSTALLATION
)

# OpenSSL
cpack_add_component("OpenSSL" DOWNLOADED)
cpack_ifw_configure_component("OpenSSL"
    DISPLAY_NAME "OpenSSL ${OPENSSL_VERSION}"
    DESCRIPTION
        "Library allowing secure networking"
        fr "Librairie permettant de sécuriser les communications"
    SORTING_PRIORITY 97
    VERSION ${OPENSSL_VERSION}
    FORCED_INSTALLATION
)

# GStreamer
cpack_add_component("GStreamer" DOWNLOADED)
cpack_ifw_configure_component("GStreamer"
    DISPLAY_NAME "GStreamer ${Gst_VERSION}"
    DESCRIPTION
        "Library used for audio streaming"
        fr "Librairie permettant les fonctionnalités de streaming audio"
    SORTING_PRIORITY 96
    VERSION ${Gst_VERSION}
    FORCED_INSTALLATION
)

# sentry-native
cpack_add_component("SentryNative" DOWNLOADED)
cpack_ifw_configure_component("SentryNative"
    DISPLAY_NAME "Sentry-Native ${SENTRY_NATIVE_PROJECT_VERSION}"
    DESCRIPTION
        "Utility that allows automatic bugs reports"
        fr "Utilitaire qui permet la génération de rapports de bugs automatiques"
    SORTING_PRIORITY 95
    VERSION ${SENTRY_NATIVE_PROJECT_VERSION}
    FORCED_INSTALLATION
)

######################################
# CPACK IFW COMPONENTS CONFIGURATION #
######################################

# repository for updates
cpack_ifw_add_repository(coreRepo 
    URL "https://dl.bintray.com/amphaal/rpgrpz/ifw-${CPACK_SYSTEM_NAME}"
)

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