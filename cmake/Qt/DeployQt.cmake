# Add commands that copy the Qt runtime to the target's output directory after
# build and install the Qt runtime to the specified directory
macro(DeployQt target)
    # FOR WINDOWS
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        #
        find_program(DEPLOYQT_EXE "windeployqt-qt6" REQUIRED)

        #
        SET(QT_DEPS_OUTPUT_DIRNAME "QtRuntime")

        # Run deployqt immediately after build to determine Qt dependencies
        # SHOULD NOT BE "POST_BUILD" because pe-util might be used later on, and you want it to ignore QT libraries
        add_custom_command(TARGET ${target}
            COMMAND ${DEPLOYQT_EXE}
                    --verbose 0
                    --no-opengl-sw
                    --no-quick-import
                    --no-system-d3d-compiler
                    --dir ${QT_DEPS_OUTPUT_DIRNAME}
                    --translations fr,en
                    $<TARGET_FILE:${target}>
            COMMENT "Create dummy folder with matching Qt runtime components"
        )

        # SHOULD NOT BE "POST_BUILD" because pe-util might be used later on, and you want it to ignore QT libraries
        add_custom_command(TARGET ${target}
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_BINARY_DIR}/${QT_DEPS_OUTPUT_DIRNAME} $<TARGET_FILE_DIR:${target}>
        )

        # install rule
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${QT_DEPS_OUTPUT_DIRNAME}/
            TYPE BIN
            COMPONENT "QtRuntime" # component name must NOT CHANGE since it is referred to elsewhere !
        )

    # FOR MACOS
    elseif(APPLE)
        #
        find_program(DEPLOYQT_EXE "macdeployqt" REQUIRED)

        # Run deployqt immediately after build to determine Qt dependencies
        add_custom_command(TARGET ${target}
        COMMAND ${DEPLOYQT_EXE}
                $<TARGET_BUNDLE_DIR:${target}>
                -libpath=$<TARGET_BUNDLE_CONTENT_DIR:${target}>
                -verbose=0
            COMMENT "Create dummy folder with matching Qt runtime components"
        )
    endif()

endmacro()