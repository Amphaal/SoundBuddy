# https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
function(HandleQtTranslation)

    # target installComponent mayInstallAlongExecutable filesToScan

    cmake_parse_arguments(
        HandleQtTranslation
        "" # Options
        "TARGET;INSTALL_COMPONENT;OUTPUT_LOCATION" # Single Value
        "SOURCES;TRANSLATION_FILES" # Multiple Values
        ${ARGN} # mandatory
    )

    #
    if(NOT HandleQtTranslation_OUTPUT_LOCATION)
        message(FATAL_ERROR "OUTPUT_LOCATION is required on all HandleQtTranslation() calls.")
    endif()

    # lupdate: update .ts files against source files
    qt_add_lupdate(${HandleQtTranslation_TARGET} 
        TS_FILES
            ${HandleQtTranslation_TRANSLATION_FILES}
        SOURCES
            ${HandleQtTranslation_SOURCES}
    )

    # lrelease: turn .ts into .qm
    qt_add_lrelease(${HandleQtTranslation_TARGET} 
        TS_FILES
            ${HandleQtTranslation_TRANSLATION_FILES}
        QM_FILES_OUTPUT_VARIABLE
            "HandleQtTranslation_QM_FILES"
    )

    # define build folder output location of .qm generated files
    target_sources(${HandleQtTranslation_TARGET} PRIVATE ${HandleQtTranslation_TRANSLATION_FILES})
    set_source_files_properties(${HandleQtTranslation_TRANSLATION_FILES}
        PROPERTIES 
            OUTPUT_LOCATION ${HandleQtTranslation_OUTPUT_LOCATION}
    )

    # 
    if (HandleQtTranslation_INSTALL_COMPONENT)
        if (APPLE)
            set_source_files_properties(${HandleQtTranslation_QM_FILES} PROPERTIES 
                MACOSX_PACKAGE_LOCATION "Resources/translations"                                                    
            )
        elseif(WIN32)
            #copy translations into output (for debug purposes)
            add_custom_command(TARGET ${HandleQtTranslation_TARGET} POST_BUILD
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
