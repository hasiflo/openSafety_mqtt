# - Run Doxygen
#
# Adds a doxygen target that runs doxygen to generate the html
# and optionally the LaTeX API documentation.
# The doxygen target is added to the doc target as a dependency.
# i.e.: the API documentation is built with:
#  make doc
#
# USAGE: GLOBAL INSTALL
#
# Install it with:
#  cmake ./ && sudo make install
# Add the following to the CMakeLists.txt of your project:
#  include(UseDoxygen OPTIONAL)
# Optionally copy Doxyfile.in in the directory of CMakeLists.txt and edit it.
#
# USAGE: INCLUDE IN PROJECT
#
#  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
#  include(UseDoxygen)
# Add the Doxyfile.in and UseDoxygen.cmake files to the projects source directory.
#
#
# Variables you may define are:
#  DOXYFILE_SOURCE_DIR - Path where the Doxygen input files are.
#      Defaults to the current source and binary directory.
#  DOXYFILE_OUTPUT_DIR - Path where the Doxygen output is stored. Defaults to "doc".
#
#  DOXYFILE_LATEX - Set to "NO" if you do not want the LaTeX documentation
#      to be built.
#  DOXYFILE_LATEX_DIR - Directory relative to DOXYFILE_OUTPUT_DIR where
#      the Doxygen LaTeX output is stored. Defaults to "latex".
#
#  DOXYFILE_HTML_DIR - Directory relative to DOXYFILE_OUTPUT_DIR where
#      the Doxygen html output is stored. Defaults to "html".
#

#
#  Copyright (c) 2009, 2010 Tobias Rautenkranz <tobias@rautenkranz.ch>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

macro(usedoxygen_set_default name value)
    if(NOT DEFINED "${name}")
        set("${name}" "${value}")
    endif()
endmacro()

find_package(Doxygen)

if(DOXYGEN_FOUND)
    find_file(DOXYFILE_IN "doxyfile.in"
            PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/../doc/doxygen" "${CMAKE_ROOT}/Modules/"
            NO_DEFAULT_PATH)
    set(DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/doxyfile")
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(DOXYFILE_IN DEFAULT_MSG "DOXYFILE_IN")
endif()

if(DOXYGEN_FOUND AND DOXYFILE_IN_FOUND)
    usedoxygen_set_default(DOXYFILE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
    usedoxygen_set_default(DOXYFILE_HTML_DIR "html")
    usedoxygen_set_default(DOXYFILE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}\"
        \"${CMAKE_CURRENT_BINARY_DIR}")

    set_property(DIRECTORY APPEND PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES
        "${DOXYFILE_OUTPUT_DIR}/${DOXYFILE_HTML_DIR}")

    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE}
            ${DOXYFILE}
        COMMENT "Writing documentation to ${DOXYFILE_OUTPUT_DIR}..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


    IF(NOT TARGET generate_dot)
        add_custom_target(generate_dot)
    ENDIF()

    if(NOT DOXYGEN_DOT_FILEFORMAT)
        SET(DOXYGEN_DOT_FILEFORMAT "png" )
    ENDIF()

    FILE ( GLOB_RECURSE TARGET_VARIABLE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../doc/images/dot/*.dot" )

	EXECUTE_PROCESS (
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/doc
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/doc/html
    	WORKING_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}
    )

    FOREACH ( dotfile IN ITEMS ${TARGET_VARIABLE_SOURCE} )
        STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/../doc/images/dot/" "" FILE_PATH ${dotfile} )
        STRING(REPLACE ".dot" "" FILE_NAME ${FILE_PATH} )
        ADD_CUSTOM_TARGET(
            "dot_${FILE_NAME}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/doc/html
            COMMAND ${DOXYGEN_DOT_EXECUTABLE} -T${DOXYGEN_DOT_FILEFORMAT} ${dotfile} -o ${CMAKE_CURRENT_BINARY_DIR}/doc/html/${FILE_NAME}.png
            COMMENT "Generating image for ${FILE_NAME} ..."
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/)
        ADD_DEPENDENCIES ( generate_dot "dot_${FILE_NAME}" )
    ENDFOREACH()

    SET ( ADD_DEPEND "doxygen" )

    ## LaTeX
    set(DOXYFILE_PDFLATEX "NO")
    set(DOXYFILE_DOT "NO")

    find_package(LATEX)
    find_program(MAKE_PROGRAM make)
    if(LATEX_COMPILER AND MAKEINDEX_COMPILER AND MAKE_PROGRAM AND
            (NOT DEFINED DOXYFILE_LATEX OR DOXYFILE_LATEX STREQUAL "YES"))
        set(DOXYFILE_LATEX "YES")
        usedoxygen_set_default(DOXYFILE_LATEX_DIR "latex")

        set_property(DIRECTORY APPEND PROPERTY
                ADDITIONAL_MAKE_CLEAN_FILES
                "${DOXYFILE_OUTPUT_DIR}/${DOXYFILE_LATEX_DIR}")

        if(PDFLATEX_COMPILER)
            set(DOXYFILE_PDFLATEX "YES")
        endif()
        if(DOXYGEN_DOT_EXECUTABLE)
            set(DOXYFILE_DOT "YES")
        endif()

        add_custom_command(TARGET doxygen
            POST_BUILD
            COMMAND ${MAKE_PROGRAM}
            COMMENT    "Running LaTeX for Doxygen documentation in ${DOXYFILE_OUTPUT_DIR}/${DOXYFILE_LATEX_DIR}..."
            WORKING_DIRECTORY "${DOXYFILE_OUTPUT_DIR}/${DOXYFILE_LATEX_DIR}")
    else()
        set(DOXYGEN_LATEX "NO")
    endif()

    configure_file(${DOXYFILE_IN} doxyfile IMMEDIATE @ONLY)

    if(NOT TARGET doc)
        add_custom_target(doc)
        add_dependencies ( doc generate_dot )
    endif()

    add_dependencies(doc ${ADD_DEPEND})
endif()
