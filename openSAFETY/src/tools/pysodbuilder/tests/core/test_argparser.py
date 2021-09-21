##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_argparser.py
# Unit test for the module pysodb.core.argparser

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
from random import randint
import os
import sys
import unittest

# third party packages or modules

# own packages or modules
from tests.common import dummy_func_pass, MagicDummy
from pysodb.core import argparser
from argparse import ArgumentParser

expected_args_none_passed = {
                'osdd_file' : None,
                'module_id' : None,
                'settings_file' :None,
                'input_files_dirs' : None,
                'output_directory' : None,
                'overwrite' : False,
                'file_encoding' : None,
                'list_modules' : False,
                'overrule_osdd' : True,
                'generate_binding_file' : None,
                'remove_cog_comments' : False,
                'dry_run' : False,
                'verbose' : False,
                'log_file' : None,
                }

arguments_to_pass = \
                '-d ./osddfiles/afile.xosdd -m mID '\
                '-s ./settings/settings.ini '\
                '-i ./input ./input2/files -o ./output '\
                '--overwrite-input-files -n utf8 -l '\
                '--generate-binding-file schema.xsd binding.py '\
                '--no-overrule-osdd --remove-cog-comments '\
                '--dry-run --log ./logfiles/logfile.txt '\
                '-v'

arguments_to_pass_long_options = \
                '--osdd-file ./osddfiles/afile.xosdd --module-id mID '\
                '--settings-file ./settings/settings.ini '\
                '--input ./input ./input2/files --output ./output '\
                '--overwrite-input-files --encoding utf8 '\
                '--list-modules '\
                '--generate-binding-file schema.xsd binding.py '\
                '--no-overrule-osdd --remove-cog-comments '\
                '--dry-run --log ./logfiles/logfile.txt '\
                '--verbose'

expeted_args = {'osdd_file' : './osddfiles/afile.xosdd',
                'module_id' : 'mID',
                'settings_file' :'./settings/settings.ini',
                'input_files_dirs' : ['./input', './input2/files'],
                'output_directory' : './output',
                'overwrite' : True,
                'file_encoding' : 'utf8',
                'list_modules' : True,
                'overrule_osdd' : False,
                'generate_binding_file' : ['schema.xsd', 'binding.py'],
                'remove_cog_comments' : True,
                'dry_run' : True,
                'verbose' : True,
                'log_file' : './logfiles/logfile.txt',
                }

##
# \brief Class for overriding the default ArgumentParser's error method
class ModifiedArgparser(ArgumentParser):

    ##
    # \brief Python's standard library ArgumentParser.error() method
    # with a workaround for build tools.
    #
    # This method contains the implementation of Python's standard
    # library ArgumentParser.error() with a changed error string from
    # 'error' to 'an_error'. This is a workaround for build tools,
    # recognising lines containing the string 'error' as build error.
    def error(self, message):
        self.print_usage(sys.stderr)
        args = {'prog': self.prog, 'message': message}
        self.exit(2, ('%(prog)s: an_error: %(message)s\n') % args)

##
# \brief Tests the argparser module
class Test_argparser (unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        super(Test_argparser, cls).setUpClass()
        cls.orig_argparser = argparser.ArgumentParser
        argparser.ArgumentParser = ModifiedArgparser

    @classmethod
    def tearDownClass(cls):
        super(Test_argparser, cls).tearDownClass()
        argparser.ArgumentParser = cls.orig_argparser

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.parser = argparser.init_argparser()

    ##
    # \brief Tests the initialisation of the argparser
    def test_init_parser(self):
        self.assertIsNotNone(argparser.init_argparser())

    ##
    # \brief Tests argument parsing with no arguments given, no sys.argv
    # arguments
    def test_arguments_none(self):
        orig_sysargs = sys.argv
        sys.argv = []

        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser)

        sys.argv = orig_sysargs

    ##
    # \brief Tests argument parsing with no arguments given
    def test_empty_arguments(self):
        args = argparser.parse_args(self.parser, [])
        self.assertIsNotNone(args)

    ##
    # \brief Tests, if the parsed argument data structure contains all expected
    # attributes
    def test_argument_attributes(self):
        args = argparser.parse_args(self.parser, [])
        for item in expected_args_none_passed.keys():
            self.assertTrue(hasattr(args, item))

    ##
    # \brief Test the default values of the parsed arguments data structure,
    # when no arguments are passed
    def test_default_values(self):
        args = argparser.parse_args(self.parser, [])
        for item in expected_args_none_passed.keys():
            args_value = getattr(args, item)
            self.assertTrue(expected_args_none_passed[item] == args_value)

    ##
    # \brief Test successful exectuion of validate_args() when no
    # arguments are passed to argparser
    def test_validate_args_default_values(self):
        args = argparser.parse_args(self.parser, [])
        argparser.validate_args(self.parser, args)

    ##
    # \brief Tests behaviour when unknown arguments are given
    def test_parse_args_unknown_args(self):
        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser, ['unknownarg'])

        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser, ['-u'])

        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser, '-z param.txt'.split())

        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser, ['--unknown-param'])

    ##
    # \brief Tests for expected SystemExits
    def test_system_exit_verison_arg(self):
        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser, ['--version'])

        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser,
                                 '-i . -o ./output -s settings.ini ' \
                                 '-d osddfile.xosdd --version'.split())

        #no SystemExit should be raised
        argparser.parse_args(self.parser, '-i . -o ./output -s settings.ini ' \
                            '-d osddfile.xosdd'.split())

    ##
    # \brief Tests for expected SystemExit, when the help argument is passed
    def test_help(self):
        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser, ['-h'])

        with self.assertRaises(SystemExit):
            argparser.parse_args(self.parser, ['--help'])

    ##
    # \brief Test with arguments, which should not cause a SystemExit when
    # parsing those arguments
    def test_all_args_without_system_exit_given(self):

        # test short forms of arguments
        args = argparser.parse_args(self.parser, arguments_to_pass.split())

        for item in expeted_args.keys():
            args_value = getattr(args, item)
            self.assertEqual(expeted_args[item], args_value)

        # test long forms of arguments
        args = argparser.parse_args(self.parser,
                                    arguments_to_pass_long_options.split())

        for item in expeted_args.keys():
            args_value = getattr(args, item)
            self.assertEqual(expeted_args[item], args_value)

    ##
    # \brief Tests argument validation with None value
    def test_validate_args_none_value(self):
        with self.assertRaises(SystemExit):
            argparser.validate_args(self.parser, None)

    ##
    # \brief Tests argument validation, when no arguments were given to the
    # parser
    def test_validate_args_no_arguments(self):
        args = argparser.parse_args(self.parser, [])
        validated_args = argparser.validate_args(self.parser, args)
        self.assertEqual(args, validated_args)

    ##
    # \brief Tests argument validation, when arguments were passed to the
    # parser
    def test_validate_arsgs_with_values(self):
        testdata_path = os.path.normpath(
                                os.path.dirname(os.path.dirname(__file__)))
        testdata_path = os.path.join(testdata_path, 'testdata')

        # disable calls to sub functions
        argparser.crosscheck_args = dummy_func_pass
        argparser.check_path_exists = dummy_func_pass

        # test invalid values
        args = argparser.parse_args(self.parser,
                    ['-d', os.path.join(testdata_path, 'test.txt')])
        with self.assertRaises(SystemExit):
            argparser.validate_args(self.parser, args)

        args = argparser.parse_args(self.parser,
                    ['-s', os.path.join(testdata_path, 'pysodbsettings.txt')])
        with self.assertRaises(SystemExit):
            argparser.validate_args(self.parser, args)

        args = argparser.parse_args(self.parser,
                    '--generate-binding-file schema.xs binding.py'.split())
        with self.assertRaises(SystemExit):
            argparser.validate_args(self.parser, args)

        args = argparser.parse_args(self.parser,
                    '--generate-binding-file schema.xsd binding.pd'.split())
        with self.assertRaises(SystemExit):
            argparser.validate_args(self.parser, args)

        args = argparser.parse_args(self.parser, '-n anEncoding'.split())
        with self.assertRaises(SystemExit):
            argparser.validate_args(self.parser, args)

        # test valid values
        args = argparser.parse_args(self.parser, arguments_to_pass.split())
        argparser.validate_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args(self):
        args = argparser.parse_args(self.parser,
                                    '--remove-cog-comments '\
                                    '--overwrite-input-files'.split())
        with self.assertRaises(SystemExit):
            argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args2(self):
        args = argparser.parse_args(self.parser,
                                    '-o . --overwrite-input-files'.split())
        with self.assertRaises(SystemExit):
            argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args3(self):
        args = argparser.parse_args(self.parser, '-i . -o .'.split())
        with self.assertRaises(SystemExit):
            argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args4(self):
        args = argparser.parse_args(self.parser, '-i .'.split())
        with self.assertRaises(SystemExit):
            argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args5(self):
        args = argparser.parse_args(self.parser, '-i . -o ./test'.split())
        with self.assertRaises(SystemExit):
            argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args6(self):
        args = argparser.parse_args(self.parser,
            '-i . --overwrite-input-files -d test.xosdd ' \
            '-s settings.ini'.split())
        argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args7(self):
        args = argparser.parse_args(self.parser, '-o output'.split())
        with self.assertRaises(SystemExit):
            argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests exection of crosscheck_args()
    def test_crosscheck_args8(self):
        args = argparser.parse_args(self.parser, '-l'.split())
        with self.assertRaises(SystemExit):
            argparser.crosscheck_args(self.parser, args)

    ##
    # \brief Tests normpath() for expected results
    def test_normpath(self):
        # since the result is os dependent, tests are done only with
        # a subfolder as result
        value = argparser.normpath('.//output/')
        self.assertTrue(value == 'output')

        value = argparser.normpath('./output/subfolder/../../input/')
        self.assertTrue(value == 'input')

    ##
    # \brief Tests check_path_exists() functionality
    def test_check_path_exists(self):
        current_path = os.getcwd()
        dir_list = os.listdir('.')

        self.assertTrue(argparser.check_path_exists(current_path))

        # generate a random directory number
        for _ in range(0, 10):
            rand_dir = ''.join(['randdir', str(randint(1, 10000))])
            if not rand_dir in dir_list:
                with self.assertRaises(SystemExit):
                    self.assertFalse(argparser.check_path_exists(rand_dir))
                break

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
