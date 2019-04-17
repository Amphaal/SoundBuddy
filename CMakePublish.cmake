############################
### CPACK CONFIG + Qt ITW ##
############################

install(TARGETS ${PROJECT_NAME} 
        DESTINATION .)

# # #WINDOWS
# # plugins/platforms, qwindows*.dll -> ./bin/platforms
# # bin, Qt5Widgets*.dll -> ./bin 
# # bin, Qt5Core*.dll -> ./bin 
# # bin, Qt5Gui*.dll -> ./bin

# # #MAC
# # plugins/platforms, libqcocoa*.dylib -> ./bin/FeedTNZ.app/Contents/MacOS/
# # lib, libQt5Widgets*.dylib -> ./bin/FeedTNZ.app/Contents/MacOS/
# # lib, libQt5Core*.dylib -> ./bin/FeedTNZ.app/Contents/MacOS/
# # lib, libQt5Gui*.dylib -> ./bin/FeedTNZ.app/Contents/MacOS/

# # Qt Libraries

# # install(FILES
# #   ${QT_DLL_DIR}/icudt51.dll
# #   ${QT_DLL_DIR}/icuin51.dll
# #   ${QT_DLL_DIR}/icuuc51.dll
# #   ${QT_DLL_DIR}/Qt5Core.dll
# #   ${QT_DLL_DIR}/Qt5Gui.dll
# #   ${QT_DLL_DIR}/Qt5Widgets.dll
# #   ${CMAKE_SOURCE_DIR}/win/qt.conf
# #   DESTINATION .
# # )
# # Qt Platform Plugin

foreach(plugin ${Qt5Gui_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()


SET(CPACK_MONOLITHIC_INSTALL 1)

SET(FEEDTNZ_DESCRIPTION "FeedTNZ")
SET(FEEDTNZ_DESCRIPTION_LOCALIZED ${FEEDTNZ_DESCRIPTION}
    fr "Logiciel compagnon pour WTNZ"
)

SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${FEEDTNZ_DESCRIPTION})
SET(CPACK_IFW_PACKAGE_PUBLISHER ${APP_PUBLISHER})
SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${APP_PUBLISHER})
SET(CPACK_IFW_PRODUCT_URL ${APP_MAIN_URL})
SET(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDirX64@/FeedTNZ")

if (APPLE)
    SET(CPACK_IFW_ROOT "/Qt/QtIFW-3.0.6")
    SET(FEEDTNZ_REMOTE_SERVER_PATH "/Volumes/www/feedtnz")
    SET(FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER "osx")
    SET(FEEDTNZ_INSTALLER_EXTENSION ".dmg")
    SET(FEEDTNZ_ICON_EXT ".icns")
endif (APPLE)

if (WIN32)
    SET(CPACK_IFW_ROOT "C:/Qt/QtIFW-3.0.6")
    SET(FEEDTNZ_REMOTE_SERVER_PATH "//192.168.0.12/www/feedtnz")
    SET(FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER "win")
    SET(FEEDTNZ_INSTALLER_EXTENSION ".exe")
    SET(FEEDTNZ_ICON_EXT ".ico")
endif (WIN32)

#icons
SET(CPACK_IFW_PACKAGE_LOGO ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/feedtnz_64.png)
SET(CPACK_IFW_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/package${FEEDTNZ_ICON_EXT})

SET(FEEDTNZ_REMOTE_SERVER_DOWNLOAD_PATH ${FEEDTNZ_REMOTE_SERVER_PATH}/downloads/${FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER})
SET(FEEDTNZ_REMOTE_SERVER_PACKAGES_PATH ${FEEDTNZ_REMOTE_SERVER_PATH}/packages/${FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER})

SET(CPACK_GENERATOR IFW)
INCLUDE(CPack)
INCLUDE(CPackIFW)

cpack_add_component(${PROJECT_NAME}
                    DISPLAY_NAME ${PROJECT_NAME}
                    DESCRIPTION ${FEEDTNZ_DESCRIPTION}
                    REQUIRED
)

cpack_ifw_configure_component(${PROJECT_NAME} 
                    ESSENTIAL
                    FORCED_INSTALLATION
                    NAME "com.lvwl.feedtnz.core"
                    DESCRIPTION ${FEEDTNZ_DESCRIPTION}
                    DEFAULT TRUE
)

cpack_ifw_add_repository(coreRepo URL ${APP_ROOT_URL}/feedtnz/packages/${FEEDTNZ_REMOTE_SERVER_PLATFORM_FOLDER})

#############
## Publish ## 
#############

SET(FEEDTNZ_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME})
SET(FEEDTNZ_PACKAGED_PATH ${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_SYSTEM_NAME}/IFW/${FEEDTNZ_PACKAGE_FILE_NAME})
SET(FEEDTNZ_PACKAGED_INSTALLER_PATH ${FEEDTNZ_PACKAGED_PATH}${FEEDTNZ_INSTALLER_EXTENSION})
SET(FEEDTNZ_PACKAGED_REPOSITORY_PATH ${FEEDTNZ_PACKAGED_PATH}/repository)

#install CoreUtils for Win32 if mv missing
add_custom_target(publishPackage 
    #build
    COMMAND ${CMAKE_COMMAND} --build . --config Release --target package

    #copy to dest
    #COMMAND cp -rf ${FEEDTNZ_PACKAGED_INSTALLER_PATH} ${FEEDTNZ_REMOTE_SERVER_DOWNLOAD_PATH}
    #COMMAND cp -rf ${FEEDTNZ_PACKAGED_REPOSITORY_PATH}/* ${FEEDTNZ_REMOTE_SERVER_PACKAGES_PATH}
)