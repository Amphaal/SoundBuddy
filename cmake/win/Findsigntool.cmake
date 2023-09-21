#[============================================================================
# Copyright 2022, Khronos Group, Inc.
# SPDX-License-Identifier: Apache-2.0
#============================================================================]

#  Functions to convert unix-style paths into paths useable by cmake on windows.
#[=======================================================================[.rst:
Findsigntool
-------

Finds the signtool executable used for codesigning on Windows.

Note that signtool does not offer a way to make it print its version
so version selection and reporting is not possible.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``signtool_FOUND``
  True if the system has the signtool executable.
``signtool_EXECUTABLE``
  The signtool command executable.

#]=======================================================================]

if (WIN32 AND NOT signtool_EXECUTABLE)
  if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set(arch "x64")
  else()
    set(arch ${CMAKE_HOST_SYSTEM_PROCESSOR})
  endif()

  # Note: must be a cache operation in order to read from the registry.
  get_filename_component(WIN_SDK_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]"
      ABSOLUTE CACHE
  )

  # Look for latest signtool
  if (WIN_SDK_DIR)
    #
    find_program(signtool_EXECUTABLE
        NAMES           signtool
        PATHS           ${WIN_SDK_DIR}
        PATH_SUFFIXES   "App Certification Kit"
        NO_DEFAULT_PATH
    )

    #
    if (signtool_EXECUTABLE)
      mark_as_advanced (signtool_EXECUTABLE)
    endif ()
  endif()

  #
  unset(WIN_SDK_DIR CACHE)
  
  # handle the QUIETLY and REQUIRED arguments and set *_FOUND to TRUE
  # if all listed variables are found or TRUE
  include (FindPackageHandleStandardArgs)

  find_package_handle_standard_args (
    signtool
    REQUIRED_VARS
      signtool_EXECUTABLE
    FAIL_MESSAGE
      "Could NOT find signtool. Will be unable to sign Windows binaries."
  )
endif()