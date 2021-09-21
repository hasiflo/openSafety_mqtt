CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (tst_pysodbuilder)
SET ( TEST_NAME "TSTPYSODBUILDER" )

SET ( PYSDOBUILDER_COPY_TIMESTAMP "${CMAKE_CURRENT_BINARY_DIR}/copytimestamp" )

INCLUDE(FindPythonInterp)

SET ( PYSODBUNITTEST_NOSE "nose" )
SET ( PYSODBUNITTEST_COVERAGE "coverage" )
SET ( PYSODBUNITTEST_COVERAGE_REPORT "pysodbuilder_coverage.xml" )
SET ( PYSODBUNITTEST_REPORT "pysodbuilder_unittest.xml" )

SET ( PYSODBUNITTEST_NECESSARY_APPS
    ${PYSODBUNITTEST_NOSE}
    ${PYSODBUNITTEST_COVERAGE}
)

SET ( TST_DRIVER
    ${pysodbuilder_SOURCE_DIR}/tests/__init__.py
    ${pysodbuilder_SOURCE_DIR}/tests/common.py
    ${pysodbuilder_SOURCE_DIR}/tests/test_pysodbmain.py

    ${pysodbuilder_SOURCE_DIR}/tests/binding/__init__.py

    ${pysodbuilder_SOURCE_DIR}/tests/core/__init__.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/gentestc.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_argparser.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_codegenerator.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_common.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_configparser.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_errorhandler.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_filehandling.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_util.py
    ${pysodbuilder_SOURCE_DIR}/tests/core/test_versioninfo.py

    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/__init__.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_codegenutil.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_common.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_c_code_util.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_c_target_data.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_genappc.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_geneplscfgh.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_gensodc.py
    ${pysodbuilder_SOURCE_DIR}/tests/codegeneration/test_gensodutilh.py

    ${pysodbuilder_SOURCE_DIR}/tests/osddprocessing/__init__.py
    ${pysodbuilder_SOURCE_DIR}/tests/osddprocessing/test_base.py
    ${pysodbuilder_SOURCE_DIR}/tests/osddprocessing/test_mainprocessing.py
    ${pysodbuilder_SOURCE_DIR}/tests/osddprocessing/test_postprocessing.py
    ${pysodbuilder_SOURCE_DIR}/tests/osddprocessing/test_preprocessing.py
    ${pysodbuilder_SOURCE_DIR}/tests/osddprocessing/test_processingdata.py

    ${pysodbuilder_SOURCE_DIR}/tests/testdata/empty.xosdd
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/illegal_multiple_elements.xosdd
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/input/EPLScfg.h
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/input/Test.c
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/input/TestNoCogcomments.c
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/input/TestOverwrite.c
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/missing_closing_tag.xosdd
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/missing_mandatory_elements.xosdd
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/pysodbsettings.ini
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/settings_double_entries.ini
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/settings_missing_sectionheader.ini
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/settings_missing_sections.ini
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/settings_no_assignment.ini
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/settings_wrong_attributenames.ini
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/test.xosdd
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/two_modules.xosdd
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/utf16test.txt
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/utf32letest.txt
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/utf8test.txt
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/ValidationTest.c
    ${pysodbuilder_SOURCE_DIR}/tests/testdata/ValidationTestNoCogcomments.c

    ${pysodbuilder_SOURCE_DIR}/tests/util/test_constants.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/test_defaultsod.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/test_macro.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/test_osdd_data.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/test_sod_data.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/test_stringfunctions.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/test_structure.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/test_type_conversion.py
    ${pysodbuilder_SOURCE_DIR}/tests/util/__init__.py
)

SET (PYSODBUILDER_SOURCE
    ${pysodbuilder_SOURCE_DIR}/pysodb/pysodbmain.py

    ${pysodbuilder_SOURCE_DIR}/pysodb/binding/README.txt
    ${pysodbuilder_SOURCE_DIR}/pysodb/binding/__init__.py

    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/codegenutil.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/common.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/c_code_util.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/c_target_data.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/genappc.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/geneplscfgh.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/gensodc.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/gensodutilh.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/codegeneration/__init__.py

    ${pysodbuilder_SOURCE_DIR}/pysodb/core/argparser.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/codegenerator.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/common.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/configparser.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/errorhandler.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/filehandling.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/util.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/versioninfo.py.in
    ${pysodbuilder_SOURCE_DIR}/pysodb/core/__init__.py

    ${pysodbuilder_SOURCE_DIR}/pysodb/osddprocessing/base.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/osddprocessing/mainprocessing.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/osddprocessing/postprocessing.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/osddprocessing/preprocessing.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/osddprocessing/processingdata.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/osddprocessing/__init__.py

    ${pysodbuilder_SOURCE_DIR}/pysodb/util/constants.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/defaultsod.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/macro.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/osdd_data.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/sod_data.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/stringfunctions.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/structure.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/type_conversion.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/util/__init__.py
    ${pysodbuilder_SOURCE_DIR}/pysodb/__init__.py

    ${pysodbuilder_SOURCE_DIR}/setup.py
    ${pysodbuilder_SOURCE_DIR}/scripts/pysodbuilder.py
)

SET ( OSDD_SCHEMA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../doc" )
SET ( PYSODBUNITTEST_TEST_DIR "${PROJECT_BINARY_DIR}/tests" )
SET ( PYSODBUNITTEST_TESTDATA_DIR "${PYSODBUNITTEST_TEST_DIR}/testdata" )

SET ( OSDD_SCHEMA_FILES
    "XOSDD_R15.xsd"
    "XOSDDLocalize_R15.xsd"
)

# copy the schema files, if
FOREACH ( _item IN ITEMS ${OSDD_SCHEMA_FILES} )
    SET ( ORIG_FILE  ${OSDD_SCHEMA_SOURCE_DIR}/${_item} )
    SET ( ORIG_SCHEMA_FILES
        ${ORIG_SCHEMA_FILES}
        ${ORIG_FILE}
    )
    SET ( TESTDATA_FILE ${PYSODBUNITTEST_TESTDATA_DIR}/${_item} )
    SET ( TESTDATA_SCHEMA_FILES
        ${TESTDATA_SCHEMA_FILES}
        ${TESTDATA_FILE}
    )
    ADD_CUSTOM_COMMAND ( OUTPUT ${TESTDATA_FILE}
                        COMMAND ${CMAKE_COMMAND} -E copy
                        ${ORIG_FILE} ${TESTDATA_FILE}
                        COMMENT "Copying file ${_item} to binary dir"
                        DEPENDS ${ORIG_FILE}
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
ENDFOREACH ()

# copy OSDD schema files to the testdata directory (if changed)
FILE ( COPY ${ORIG_SCHEMA_FILES} DESTINATION ${PYSODBUNITTEST_TESTDATA_DIR} )

SET ( TESTDATA_SCHEMA_FILE "${PYSODBUNITTEST_TESTDATA_DIR}/XOSDD_R15.xsd" )
SET ( TESTDATA_BINDING_FILE "${PYSODBUNITTEST_TEST_DIR}/binding/xosddbinding.py" )

SET (PYSODBUILDER_TEST_SOURCE
    ${PYSODBUILDER_SOURCE}
    ${TST_DRIVER}
)

# try to locate needed third party python packages / modules in standard paths
PySODBuilderCheckPythonModules (
    "${PYSODBUNITTEST_NECESSARY_APPS}"
)

# set options according to available packages
IF ( PY_NOSE )
    SET ( PYSODB_UNITTEST_OPTIONS "-m" "nose" "--with-xunit"
        "--xunit-file=${PYSODBUNITTEST_REPORT}" "-w" "tests" )
    IF (PY_COVERAGE)
        SET ( NOSE_COVERAGE_PARAMS "--with-coverage" "--cover-package=pysodb"
            "--cover-erase" "--cover-xml"
            "--cover-xml-file=../${PYSODBUNITTEST_COVERAGE_REPORT}"
             )
        SET (PYSODB_UNITTEST_OPTIONS "${PYSODB_UNITTEST_OPTIONS}"
            "${NOSE_COVERAGE_PARAMS}" )
    ENDIF()
ELSE()
    SET ( PYSODB_UNITTEST_OPTIONS "-m" "unittest" "discover" "-t"
        "${PROJECT_BINARY_DIR}" "-s" "${PROJECT_BINARY_DIR}/tests" )
ENDIF()


CONFIGURE_FILE( ${pysodbuilder_SOURCE_DIR}/pysodb/core/versioninfo.py.in
                ${PROJECT_BINARY_DIR}/pysodb/core/versioninfo.py )

CONFIGURE_FILE( ${pysodbuilder_SOURCE_DIR}/pysodbsettings.ini.in
                ${PROJECT_BINARY_DIR}/pysodbsettings.ini )


# copy source files to binary directory, from where the pysodbuilder will be
# executed
# because add_test allows only one command and "make test" executes just the
# test of the pysodbuilder, all files have to be copied to binary dir prior
# to the execution of "make test", this is also done with the macro
PySODBuilderCopy (
    "${pysodbuilder_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
    "${PYSDOBUILDER_COPY_TIMESTAMP}"
    "${PYSODBUILDER_TEST_SOURCE}"
)

SET ( BINDING_DEPENDENCIES
    ${PYSDOBUILDER_COPY_TIMESTAMP}
    ${TESTDATA_SCHEMA_FILES})

# generate new binding file, if schema file is newer than binding file or
# binding does not exist
PySODBuilderGenerateBinding (
    "${TESTDATA_BINDING_FILE}"
    "${TESTDATA_SCHEMA_FILE}"
    "${BINDING_DEPENDENCIES}"
    "${PROJECT_BINARY_DIR}"
)

# add test for ctest
ADD_TEST (
    NAME "${TEST_NAME}"
    COMMAND ${PYTHON_EXECUTABLE} ${PYSODB_UNITTEST_OPTIONS}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

# add test as custom target
ADD_CUSTOM_TARGET (
    ${PROJECT_NAME}
    COMMAND ${PYTHON_EXECUTABLE} ${PYSODB_UNITTEST_OPTIONS}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS ${PYSDOBUILDER_COPY_TIMESTAMP} ${TESTDATA_BINDING_FILE}
)
