##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_util.py
# Unit test for the module pysodb.core.util

# ##############################################################################
# Copyright (c) 2015, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the copyright holders nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ##############################################################################

# python standard packages or modules
import unittest
import os

# third party packages or modules

# own packages or modules
from tests.common import dummy_func_raise_exception, current_python_version, \
    python34
from tests.common import compare_files_ignore_EOL

from pysodb.core import util, errorhandler
from pysodb.core.common import EPLSCfg


##
# \brief Base class for testing the pysodb.core.util module
class Test_util(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        super(Test_util, cls).setUpClass()
        cls.old_working_dir = os.getcwd()
        testdata_path = os.path.normpath(os.path.dirname(
                                                    os.path.dirname(__file__)))
        cls.testdata_path = os.path.join(testdata_path, 'testdata')
        os.chdir(cls.testdata_path)
        cls.consolehandle = errorhandler.ErrorHandler.add_console_output(
        errorhandler.get_level(True))

    @classmethod
    def tearDownClass(cls):
        super(Test_util, cls).tearDownClass()
        os.chdir(cls.old_working_dir)


##
# \brief Tests the path helper functions of the module
class Test_path_helper_functions(Test_util):

    ##
    # \brief Tests outpdirectory_in_input_files_directories()
    def test_check_outpdirectory_in_input_files_directories(self):

        output = 'input'

        input_dir_file = ['input/EPLScfg.h']
        self.assertTrue(util.check_outpdirectory_in_input_files_directories(
                                                    input_dir_file, output))

        input_dir_file = ['.', 'input', 'input/EPLScfg.h']
        self.assertTrue(util.check_outpdirectory_in_input_files_directories(
                                                    input_dir_file, output))


        output = 'output'
        self.assertFalse(util.check_outpdirectory_in_input_files_directories(
                                                    input_dir_file, output))

        input_dir_file = ['notexistingfolder', 'input/EPLScfg.h']
        self.assertFalse(util.check_outpdirectory_in_input_files_directories(
                                                    input_dir_file, output))

    ##
    # \brief Tests check_file_exists()
    def test_check_file_exists(self):
        self.assertFalse(util.check_file_exists('notexistingfile'))
        self.assertTrue(util.check_file_exists('pysodbsettings.ini'))
        self.assertFalse(util.check_file_exists('input'))

    ##
    # \brief Tests directory_exists()
    def test_directory_exists(self):
        self.assertTrue(util.check_directory_exists('input'))
        self.assertFalse(util.check_directory_exists('pysodbsettings.ini'))
        self.assertFalse(util.check_directory_exists('notexistingfile.txt'))

    ##
    # \brief Tests get_files_matching_extionsions()
    def test_get_files_matching_extionsions(self):
        result = util.get_files_matching_extensions(None, None)
        self.assertEqual(result[0], [])
        self.assertEqual(result[1], [])

        result = util.get_files_matching_extensions(['.'], None)
        self.assertEqual(result[0], [])
        self.assertEqual(result[1], [])

        result = util.get_files_matching_extensions(None, ['.ini'])
        self.assertEqual(result[0], [])
        self.assertEqual(result[1], [])

        result = util.get_files_matching_extensions(['.'], [])
        self.assertEqual(result[0], [])
        self.assertEqual(result[1], [])

        # count .ini files in folder for validation of the result
        files_in_dir = os.listdir('.')
        ini_files = list(filter(lambda x: x.endswith('.ini'), files_in_dir))

        result = util.get_files_matching_extensions(['.'], ['.ini'])
        self.assertEqual(result[0], ini_files)
        self.assertEqual(result[1], [])

        result = util.get_files_matching_extensions(['.', 'notexistingfolder',
                                                     'notexistingfile.ini'],
                                                    ['.ini'])
        self.assertEqual(result[0], ini_files)
        self.assertEqual(result[1], ['notexistingfolder',
                                     'notexistingfile.ini'])

        result = util.get_files_matching_extensions(['pysodbsettings.ini'],
                                                    ['.ini'])
        self.assertEqual(result[0], ['pysodbsettings.ini'])
        self.assertEqual(result[1], [])


##
# \brief Tests functions primarily related to code generation
class Test_codegeneration_functions(Test_util):

    @classmethod
    def setUpClass(cls):
        super(Test_codegeneration_functions, cls).setUpClass()
        cls.binding_module = 'tests.binding.xosddbinding'

    ##
    # \brief Tests the generation of cog arguments
    def test_make_cog_arguments(self):
        args = util.make_cog_arguments(None)
        self.assertIsNotNone(args)

        #check default values
        expected = ['cog', '-r', '--verbosity=0']
        self.assertEqual(args, expected)

        expected = ['cog']
        args = util.make_cog_arguments(None, None, None, None, None, None)
        self.assertEqual(args, expected)

        expected = ['cog', '-r', '--verbosity=0', 'inputfile.txt']
        args = util.make_cog_arguments('inputfile.txt')
        self.assertEqual(args, expected)

        expected = ['cog', '-o', 'outputfile.txt', '--verbosity=0',
                    'inputfile.txt']
        args = util.make_cog_arguments('inputfile.txt', 'outputfile.txt')
        self.assertEqual(args, expected)

        expected = ['cog', '-o', 'outputfile.txt', '--verbosity=0',
                    'inputfile.txt']
        args = util.make_cog_arguments('inputfile.txt', 'outputfile.txt', True)
        self.assertEqual(args, expected)

        expected = ['cog', '-o', 'outputfile.txt', '--verbosity=0', '-d',
                    'inputfile.txt']
        args = util.make_cog_arguments('inputfile.txt', 'outputfile.txt', True,
                                        True)
        self.assertEqual(args, expected)

        expected = ['cog', '-r', '--verbosity=0', '-d', 'inputfile.txt']
        args = util.make_cog_arguments('inputfile.txt', overwrite=True,
                                       remove_cog_comments=True)
        self.assertEqual(args, expected)

        expected = ['cog', '-r', '--verbosity=0', '-d', '-n', 'utf8',
                    'inputfile.txt']
        args = util.make_cog_arguments('inputfile.txt', overwrite=True,
                                    remove_cog_comments=True, encoding='utf8')
        self.assertEqual(args, expected)

    ##
    # \brief Tests translation of relative paths to package names
    def test_translate_relative_path_to_package(self):
        expected = 'pysodb.core.common'
        result = util.translate_relative_path_to_package(
                                        './pysodb/core/common')
        self.assertEqual(result, expected)
        result = util.translate_relative_path_to_package(
                                        './pysodb/../pysodb/core/common')
        self.assertEqual(result, expected)
        result = util.translate_relative_path_to_package(
                                        r'.\pysodb\..\pysodb\core\common\\')

        self.assertEqual(result, expected)
        result = util.translate_relative_path_to_package(
                                        r'.\pysodb/../pysodb\core\common/')
        self.assertEqual(result, expected)

    ##
    # \brief Tests the listing of osdd modules from .xosdd files
    def test_list_osdd_modules(self):
        # since list_osdd_modules is a pure output / printing function
        # it is just tested, if the function doesn't raise an Exception / Error

        osdd_file = os.path.join(self.testdata_path, 'test.xosdd')
        osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                     self.binding_module)
        self.assertIsNotNone(osdd_data)
        util.list_osdd_modules(osdd_data)

        osdd_file = os.path.join(self.testdata_path, 'two_modules.xosdd')
        osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                     self.binding_module)
        self.assertIsNotNone(osdd_data)
        util.list_osdd_modules(osdd_data)

        util.list_osdd_modules(None)

    ##
    # \brief Tests get_osdd_modules()
    def test_get_osdd_module(self):
        with self.assertRaises(SystemExit):
            util.get_osdd_module(None, None)

        osdd_file = os.path.join(self.testdata_path, 'test.xosdd')
        osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                     self.binding_module)
        self.assertIsNotNone(osdd_data)

        module = util.get_osdd_module(osdd_data, None)
        self.assertIsNotNone(module)
        self.assertEqual(module.id, 'demo-sn-gpio')

        module = util.get_osdd_module(osdd_data, 'demo-sn-gpio')
        self.assertIsNotNone(module)
        self.assertEqual(module.id, 'demo-sn-gpio')

        with self.assertRaises(SystemExit):
            module = util.get_osdd_module(osdd_data, 'notexistentmodule')

        osdd_file = os.path.join(self.testdata_path, 'two_modules.xosdd')
        osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                     self.binding_module)
        self.assertIsNotNone(osdd_data)

        module = util.get_osdd_module(osdd_data, 'module2')
        self.assertIsNotNone(module)
        self.assertEqual(module.id, 'module2')
        with self.assertRaises(SystemExit):
            module = util.get_osdd_module(osdd_data, None)

        with self.assertRaises(SystemExit):
            module = util.get_osdd_module(osdd_data, 'notinfile')

    ##
    # \brief Tests reading different .xosdd files including 'defective' /
    # inaccurate files
    def test_get_osdd_data_from_osdd_file(self):
        osdd_file = os.path.join(self.testdata_path, 'test.xosdd')
        osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                     self.binding_module)
        self.assertIsNotNone(osdd_data)

        osdd_file = os.path.join(self.testdata_path, 'two_modules.xosdd')
        osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                     self.binding_module)
        self.assertIsNotNone(osdd_data)

        osdd_file = os.path.join(self.testdata_path, 'empty.xosdd')
        with self.assertRaises(SystemExit):
            osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                         self.binding_module)

        osdd_file = os.path.join(self.testdata_path,
                                 'missing_closing_tag.xosdd')
        with self.assertRaises(SystemExit):
            osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                         self.binding_module)

        osdd_file = os.path.join(self.testdata_path,
                                 'illegal_multiple_elements.xosdd')
        with self.assertRaises(SystemExit):
            osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                         self.binding_module)

        osdd_file = os.path.join(self.testdata_path,
                                 'missing_mandatory_elements.xosdd')
        with self.assertRaises(SystemExit):
            osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                         self.binding_module)

        module = 'not.existing.module'
        osdd_file = os.path.join(self.testdata_path, 'test.xosdd')
        with self.assertRaises(SystemExit):
            osdd_data = util.get_osdd_data_from_osddfile(osdd_file, module)

    ##
    # \brief Tests reading settings from settings file
    #
    # This test is only executed from Python 3.4 onwards, because of
    # configparser functionality.
    @unittest.skipIf(current_python_version < python34,
                     'Errors raised only supported in newer versions of Python')
    def test_get_settings_from_settingsfile_py34(self):
        # basically tested in test_configparser, so this test only checks
        # for the successful execution of the function
        settings_file = os.path.join(self.testdata_path, 'pysodbsettings.ini')
        settings = util.get_settings_from_settingsfile(settings_file)
        self.assertIsNotNone(settings)
        self.assertIsInstance(settings, EPLSCfg)

        settings_file = os.path.join(self.testdata_path, 'notexistent.ini')
        with self.assertRaises((IOError)):
            settings = util.get_settings_from_settingsfile(settings_file)

        settings_file = os.path.join(self.testdata_path,
                                     'settings_double_entries.ini')
        with self.assertRaises(SystemExit):
            settings = util.get_settings_from_settingsfile(settings_file)

    ##
    # \brief Tests reading settings from settings file
    def test_get_settings_from_settingsfile(self):
        # basically tested in test_configparser, so this test only checks
        # for the successful execution of the function
        settings_file = os.path.join(self.testdata_path, 'pysodbsettings.ini')
        settings = util.get_settings_from_settingsfile(settings_file)
        self.assertIsNotNone(settings)
        self.assertIsInstance(settings, EPLSCfg)

        settings_file = os.path.join(self.testdata_path, 'notexistent.ini')
        with self.assertRaises((IOError)):
            settings = util.get_settings_from_settingsfile(settings_file)

        settings_file = os.path.join(self.testdata_path,
                                     'settings_missing_sectionheader.ini')
        with self.assertRaises(SystemExit):
            settings = util.get_settings_from_settingsfile(settings_file)

        settings_file = os.path.join(self.testdata_path,
                                     'settings_missing_sections.ini')
        with self.assertRaises(SystemExit):
            settings = util.get_settings_from_settingsfile(settings_file)

        settings_file = os.path.join(self.testdata_path,
                                     'settings_no_assignment.ini')
        with self.assertRaises(SystemExit):
            settings = util.get_settings_from_settingsfile(settings_file)

        settings_file = os.path.join(self.testdata_path,
                                     'settings_wrong_attributenames.ini')
        with self.assertRaises(SystemExit):
            settings = util.get_settings_from_settingsfile(settings_file)

    ##
    # \brief Tests the code generation / processing of the input files with the
    # code generation markup
    def test_generate_files_with_cog(self):

        settings_file = os.path.join(self.testdata_path, 'pysodbsettings.ini')
        settings = util.get_settings_from_settingsfile(settings_file)
        self.assertIsNotNone(settings)

        osdd_file = os.path.join(self.testdata_path, 'test.xosdd')
        osdd_data = util.get_osdd_data_from_osddfile(osdd_file,
                                                     self.binding_module)
        self.assertIsNotNone(osdd_data)

        module = util.get_osdd_module(osdd_data, None)
        self.assertIsNotNone(module)
        self.assertEqual(module.id, 'demo-sn-gpio')

        # generate EPLSCfg.h
        output_file = os.path.join(self.testdata_path, 'output/EPLScfg.h')
        if os.path.exists(output_file):
            os.remove(output_file)
        util.generate_files_with_cog(['input'], 'output', ['.h'], settings,
                                     module, False, False, False)

        self.assertTrue(os.path.exists(output_file))
        #os.remove(output_file)
        #self.assertFalse(os.path.exists(output_file))

        # Test.c
        output_file = os.path.join(self.testdata_path, 'output/Test.c')
        validation_file = os.path.join(self.testdata_path, 'ValidationTest.c')

        if os.path.exists(output_file):
            os.remove(output_file)

        util.generate_files_with_cog(['input/Test.c'], 'output', ['.c'],
                                     settings, module, False, False, False)

        self.assertTrue(os.path.exists(output_file))
        self.assertTrue(compare_files_ignore_EOL(output_file, validation_file))

        # test not given output directory
        overwrite_file = os.path.join(self.testdata_path,
                                      'input/TestOverwrite.c')
        util.generate_files_with_cog([overwrite_file], None, ['.c'], settings,
                                     module, True, False, False)
        self.assertTrue(os.path.exists(overwrite_file))
        self.assertTrue(compare_files_ignore_EOL(overwrite_file, validation_file))

        # TestNoCogComments.c
        output_file = os.path.join(self.testdata_path,
                                   'output/TestNoCogcomments.c')
        validation_file = os.path.join(self.testdata_path,
                                       'ValidationTestNoCogcomments.c')

        if os.path.exists(output_file):
            os.remove(output_file)

        util.generate_files_with_cog(['input/TestNoCogcomments.c'], 'output',
                                     ['.c'], settings, module, False, True,
                                     False)

        self.assertTrue(os.path.exists(output_file))
        self.assertTrue(compare_files_ignore_EOL(output_file, validation_file))

        # test file, which doesn't exist
        with self.assertRaises(SystemExit):
            util.generate_files_with_cog(['input/Notexisting.c'], 'output',
                                     ['.c'], settings, module, False, True,
                                     False)

    ##
    # \brief Tests creation of a directory, if it does'nt exist
    def test_create_directory_if_not_exists(self):
        directory = 'test_create_dir'
        if not directory in os.listdir('.'):
            util.create_directory_if_not_exists(directory)
            self.assertTrue(directory in os.listdir('.'))
            os.rmdir(directory)
            self.assertFalse(directory in os.listdir('.'))

        orig_os_makedirs = util.os.makedirs
        util.os.makedirs = dummy_func_raise_exception

        with self.assertRaises(SystemExit):
            util.create_directory_if_not_exists(directory)

        util.os.makedirs = orig_os_makedirs

    ##
    # \brief Tests the generation of the binding file
    def test_generate_binding_file(self):
        input_file = 'XOSDD_R15.xsd'
        output_file = 'bindingoutput.py'

        with self.assertRaises(SystemExit):
            util.generate_binding_file(None, None)

        with self.assertRaises(SystemExit):
            util.generate_binding_file(input_file, None)

        with self.assertRaises(SystemExit):
            util.generate_binding_file(None, output_file)

        util.generate_binding_file(input_file, output_file)
        self.assertTrue(output_file in os.listdir('.'))
        os.remove(output_file)
        self.assertFalse(output_file in os.listdir('.'))

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
