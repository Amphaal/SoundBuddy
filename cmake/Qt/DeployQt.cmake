#find helper
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_program(WINDEPLOYQT "windeployqt-qt6" REQUIRED)
endif()

# Add commands that copy the Qt runtime to the target's output directory after
# build and install the Qt runtime to the specified directory
macro(DeployQt target)
    
    # Run windeployqt immediately after build to determine Qt dependencies
    add_custom_command(TARGET ${target}
        COMMAND ${WINDEPLOYQT}
                --verbose 0
                --no-angle
                --no-opengl-sw
                --dir QtRuntime
                --translations fr,en
                $<TARGET_FILE:${target}>
        COMMENT "Create dummy folder with matching Qt runtime components"
    )

    # install rule
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtRuntime/"
        TYPE BIN
        COMPONENT "Qt"
    )

endmacro()