# https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
function(HandleQtTranslation)

    # target installComponent mayInstallAlongExecutable filesToScan

    cmake_parse_arguments(
        HandleQtTranslation
        "" # Options
        "TARGET;INSTALL_COMPONENT" # Single Value
        "SOURCES;TRANSLATION_FILES" # Multiple Values
        ${ARGN} # mandatory
    )

    #updates TS files from sources
    qt_create_translation(HandleQtTranslation_QM_FILES
        ${HandleQtTranslation_SOURCES}
        ${HandleQtTranslation_TRANSLATION_FILES}
    )

    #generate QM files from TS
    target_sources(${HandleQtTranslation_TARGET} PRIVATE
        ${HandleQtTranslation_QM_FILES}
    )

    if (HandleQtTranslation_INSTALL_COMPONENT)
        if (APPLE)
            set_source_files_properties(${HandleQtTranslation_QM_FILES} PROPERTIES 
                MACOSX_PACKAGE_LOCATION "Resources/translations"                                                    
            )
        elseif(WIN32)
            #copy translations into output (for debug purposes)
            add_custom_command(TARGET ${HandleQtTranslation_TARGET}   
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${HandleQtTranslation_TARGET}>/translations       
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${HandleQtTranslation_QM_FILES} $<TARGET_FILE_DIR:${HandleQtTranslation_TARGET}>/translations
                COMMENT "Copy Qt translation files"
            )

            #
            include(GNUInstallDirs)
            
            # since binary translations files will already be within package resources, we only need to tell to explicitely install it on Windows
            install(FILES ${HandleQtTranslation_QM_FILES}
                DESTINATION ${CMAKE_INSTALL_BINDIR}/translations
                COMPONENT ${HandleQtTranslation_INSTALL_COMPONENT}
            )
        endif()
    endif()
endfunction()
