function(HandleQtTranslation target installComponent filesToScan)

    SET(TS_FILES ${ARGN})

    #updates TS files from sources
    qt_create_translation(QM_FILES
        ${filesToScan}
        ${TS_FILES}
    )

    #generate QM files from TS
    target_sources(${target} PRIVATE
        ${QM_FILES}
    )

    if(installComponent)
        #copy translations into output (for debug purposes)
        add_custom_command(TARGET ${target}   
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target}>/translations       
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QM_FILES} $<TARGET_FILE_DIR:${target}>/translations
            COMMENT "Copy Qt translation files"
        )

        #install
        include(GNUInstallDirs)
        install(FILES ${QM_FILES}
            DESTINATION ${CMAKE_INSTALL_BINDIR}/translations
            COMPONENT ${installComponent}
        )
    endif()

endfunction()
