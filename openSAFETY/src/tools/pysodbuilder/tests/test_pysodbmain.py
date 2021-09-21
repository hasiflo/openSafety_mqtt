##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_pysodbmain.py
# Unit test for the module pysodb.pysodbmain

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

# third party packages or modules

# own packages or modules


argument_attributes = [
                'osdd_file',
                'module_id',
                'settings_file',
                'input_files_dirs',
                'output_directory',
                'overwrite',
                'file_encoding',
                'list_modules',
                'overrule_osdd',
                'generate_binding_file',
                'remove_cog_comments',
                'dry_run',
                'verbose',
                'log_file',
                ]

file_extensions = ['.c', '.h']


##
# \brief Tests the execution of the main program with different given arguments
class Test_pysodbmain(unittest.TestCase):

    def setUp(self):
        import pysodb.pysodbmain
        from tests.common import MagicDummy, DummyArgs
        unittest.TestCase.setUp(self)
        self.dummy_errorhandler = MagicDummy()
        self.dummy_argparser = MagicDummy()
        self.dummy_util = MagicDummy()
        self.dummy_filehandling = MagicDummy()
        self.dummy_consolehandle = MagicDummy()

        pysodb.pysodbmain.errorhandler = self.dummy_errorhandler
        pysodb.pysodbmain.argparser = self.dummy_argparser
        pysodb.pysodbmain.util = self.dummy_util
        pysodb.pysodbmain.filehandling = self.dummy_filehandling
        pysodb.pysodbmain.common.DefaultFileExtensions.EXT_COG = file_extensions

        self.dummy_consolehandle.setLevel.returnvalue = 'set_level'
        self.dummy_errorhandler.add_console_output.returnvalue = \
            self.dummy_consolehandle
        self.dummy_errorhandler.ErrorHandler.add_console_output.returnvalue = \
            self.dummy_consolehandle
        self.dummy_errorhandler.get_level.returnvalue = 'get_level'

        self.args = DummyArgs()
        for attribute in argument_attributes:
            setattr(self.args, attribute, None)

        self.dummy_argparser.init_argparser.returnvalue = 'parser'
        self.dummy_argparser.parse_args.returnvalue = 'args'
        self.dummy_argparser.validate_args.returnvalue = self.args

    ##
    # \brief Tests the main() function for expected function calls
    def test_main(self):
        import pysodb.pysodbmain

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 0)

    ##
    # \brief Tests the main() function for expected function calls with
    # provided log_file argument
    def test_main_args_logfile(self):
        import pysodb.pysodbmain

        self.args.log_file = 'log_file'

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_function_call_params(),
            [(('get_level', 'log_file',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 0)

    ##
    # \brief Tests the main() function for expected function calls with
    # provided encoding argument
    def test_main_args_file_encoding(self):
        import pysodb.pysodbmain
        self.args.file_encoding = 'file_encoding'

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_function_call_params(),
            [(('file_encoding',), {})])

        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 0)
        self.assertEqual(
             self.dummy_util.get_osdd_module.get_number_of_calls(), 0)
        self.assertEqual(
             self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 0)

    ##
    # \brief Tests the main() function for expected function calls with
    # provided generate_binding_file argument
    def test_main_args_generate_binding_file(self):
        import pysodb.pysodbmain
        self.args.generate_binding_file = ('schema', 'binding')

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
             self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.generate_binding_file.get_function_call_params(),
            [(('schema', 'binding',), {})])

        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 0)

    ##
    # \brief Tests the main() function for expected function calls with
    # provided osdd_file argument
    def test_main_args_osdd_file(self):
        import pysodb.pysodbmain
        self.args.osdd_file = 'osdd_file'
        self.args.module_id = 'module_id'
        self.dummy_util.get_osdd_data_from_osddfile.returnvalue = 'osdd_data'

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)
        self.assertEqual(
             self.dummy_util.generate_binding_file.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_function_call_params(),
            [(('osdd_file',), {})])

        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_osdd_module.get_function_call_params(),
            [(('osdd_data', 'module_id',), {})])

        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 0)

    ##
    # \brief Tests the main() function for expected function calls with
    # provided list_modules argument
    def test_main_args_list_modules(self):
        import pysodb.pysodbmain
        self.args.list_modules = 'list_modules'
        self.args.osdd_file = 'osdd_file'
        self.args.module_id = 'module_id'
        self.dummy_util.get_osdd_data_from_osddfile.returnvalue = 'osdd_data'

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_function_call_params(),
            [(('osdd_file',), {})])

        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_function_call_params(),
            [(('osdd_data',), {})])

        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 0)

    ##
    # \brief Tests the main() function for expected function calls with
    # provided settings_file argument
    def test_main_args_setings_file(self):
        import pysodb.pysodbmain
        self.args.settings_file = 'settings_file'

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_function_call_params(),
            [(('settings_file',), {})])

        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 0)

    ##
    # \brief Tests the main() function for expected function calls with
    # provided input argument
    def test_main_args_input_files_dirs(self):
        import pysodb.pysodbmain
        self.args.output_directory = 'output_directory'
        self.args.overwrite = 'overwrite'
        self.args.input_files_dirs = 'input_files_dirs'
        self.args.settings_file = 'settings_file'
        self.args.osdd_file = 'osdd_file'
        self.args.module_id = 'module_id'
        self.dummy_util.get_osdd_data_from_osddfile.returnvalue = 'osdd_data'
        self.dummy_util.get_settings_from_settingsfile.returnvalue = \
            'sodbsettings'
        self.dummy_util.get_osdd_module.returnvalue = 'osdd_module'

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_function_call_params(),
            [(('osdd_file',), {})])

        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_osdd_module.get_function_call_params(),
            [(('osdd_data', 'module_id',), {})])

        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 1)

        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_function_call_params(),
            [(('input_files_dirs', 'output_directory', file_extensions,
               'sodbsettings', 'osdd_module', 'overwrite', None, None, None,),
              {})])

    ##
    # \brief Tests the main() function for expected function calls with
    # provided dry_run argument
    def test_main_args_dry_run(self):
        import pysodb.pysodbmain
        self.args.output_directory = 'output_directory'
        self.args.overwrite = 'overwrite'
        self.args.dry_run = 'dry_run'
        self.args.input_files_dirs = 'input_files_dirs'
        self.args.settings_file = 'settings_file'
        self.args.osdd_file = 'osdd_file'
        self.args.module_id = 'module_id'
        self.dummy_util.get_osdd_data_from_osddfile.returnvalue = \
            'osdd_data'
        self.dummy_util.get_settings_from_settingsfile.returnvalue = \
            'sodbsettings'
        self.dummy_util.get_osdd_module.returnvalue = \
            'osdd_module'

        with self.assertRaises(SystemExit):
            pysodb.pysodbmain.main()

        self.assertEqual(
            self.dummy_argparser.init_argparser.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.init_argparser.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            self.dummy_argparser.parse_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.parse_args.get_function_call_params(),
            [(('parser',), {})])

        self.assertEqual(
            self.dummy_argparser.validate_args.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_argparser.validate_args.get_function_call_params(),
            [(('parser', 'args',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_console_output.get_function_call_params(),
            [(('get_level',), {})])

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.add_file_output.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_errorhandler.ErrorHandler.debug.get_function_call_params(),
            [((self.args,), {})])

        self.assertEqual(
            self.dummy_filehandling.FileHandling.set_encoding.get_number_of_calls(), 0)
        self.assertEqual(
            self.dummy_util.generate_binding_file.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_osdd_data_from_osddfile.get_function_call_params(),
            [(('osdd_file',), {})])

        self.assertEqual(
            self.dummy_util.get_osdd_module.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.get_osdd_module.get_function_call_params(),
            [(('osdd_data', 'module_id',), {})])

        self.assertEqual(
            self.dummy_util.list_osdd_modules.get_number_of_calls(), 0)

        self.assertEqual(
            self.dummy_util.get_settings_from_settingsfile.get_number_of_calls(), 1)

        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_number_of_calls(), 1)
        self.assertEqual(
            self.dummy_util.generate_files_with_cog.get_function_call_params(),
            [(('input_files_dirs', None, file_extensions, 'sodbsettings',
               'osdd_module', None, None, None, None,), {})])


if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
