##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file argparser.py
# Command line parsing and parameter validation
#

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
from argparse import ArgumentParser
import codecs
import os
import sys

# third party packages or modules

# own packages or modules
from pysodb.core import common
from pysodb.core import errorhandler
from pysodb.core import util
from pysodb.core import versioninfo

METAVAR_FILE = 'FILE'
METAVAR_MODULE_ID = 'MODULE_ID'
METAVAR_DIRECTORY = 'DIRECTORY'
METAVAR_FILE_OR_DIRECTORY = ' | '.join([METAVAR_FILE, METAVAR_DIRECTORY])
METAVAR_ENCODING = 'ENCODING'


##
# \brief Initialises the argument parser
#
# This function instantiates an argument parser object and adds command line
# options to the parser.
def init_argparser():
    parser = ArgumentParser(
    description='Builds the files EPLScfg.h, sod.h, sod.c according to the '\
    'given osdd file and the settings made in cmake.')

    parser.add_argument(
        '-d', '--osdd-file', help='osdd file to obtain values from',
        metavar=METAVAR_FILE, dest='osdd_file', type=str
        )

    parser.add_argument(
        '-m', '--module-id', help='generate files for given module id. this '\
        'option is necessary, when using osdd files which contain several '\
        'modules', metavar=METAVAR_MODULE_ID, dest='module_id', type=str)

    parser.add_argument(
        '-s', '--settings-file', help='uses the given settingsfile.',
        metavar=METAVAR_FILE, dest='settings_file', type=str)

    parser.add_argument(
        '-i', '--input', help='input files or directories containing the ' \
        'files used for code generation, basically .h and .c files.',
         nargs='*', metavar=METAVAR_FILE_OR_DIRECTORY,
         dest='input_files_dirs', type=str)

    parser.add_argument(
        '-o', '--output', help='output directory', metavar=METAVAR_DIRECTORY,
        dest='output_directory', type=str)

    parser.add_argument(
        '--overwrite-input-files', help='overwrite input files with generated '\
        'files.', action='store_true', dest='overwrite')

    parser.add_argument(
        '-n', '--encoding', help='specyfiy file encoding',
        metavar=METAVAR_ENCODING, dest='file_encoding', type=str)

    parser.add_argument(
        '-l', '--list-modules', help='list modules contained in specified '\
        'osdd file', action='store_true', dest='list_modules')

    parser.add_argument(
        '--generate-binding-file', help='use schema file ({}) to generate a '\
        'bindings file ({}).'.format(common.DefaultFileExtensions.EXT_SCHEMA, \
                                     common.DefaultFileExtensions.EXT_BINDING),
        nargs=2, metavar=METAVAR_FILE, dest='generate_binding_file', type=str)

    parser.add_argument(
        '--no-overrule-osdd', help='settings obtained from osdd file do not '\
        'get overruled, if the corresponding values in the settings file are '\
        'bigger.', action='store_false', dest='overrule_osdd', default=True)

    parser.add_argument(
        '--remove-cog-comments', help='remove comments used for code '\
        'generation out of the generated files. this option requires -o {}. '\
        'Also, the output directory must not be part of the input '\
        'directories to prevent unwanted overwriting of files.'.format(
            METAVAR_DIRECTORY),
        action='store_true', dest='remove_cog_comments')

    parser.add_argument(
        '--dry-run', help='prints the input files with the generated code, '\
        'but doesn\'t save the result', action='store_true', dest='dry_run')

    parser.add_argument(
        '-v', '--verbose', help='increase verbosity of the program',
        action='store_true', dest='verbose')

    parser.add_argument(
        '--log', help='write log to file', metavar=METAVAR_FILE,
        dest='log_file', type=str)

    parser.add_argument(
        '--version', help='print version of the program', action='version',
        version='%(prog)s v{}'.format(
            versioninfo.SODBVersionInfo.openSAFETY_VERSION_RELEASE))

    return parser


##
# \brief Trigges argument parsing and validation
def parse_args(parser, args=None, namespace=None):

    if args is None and len (sys.argv) <= 1:
        parser.print_usage()
        sys.exit(common.SODBsysexit.EXIT_SUCCESS)

    args = parser.parse_args(args, namespace)
    return args;


##
# \brief  Wrapping function for path normalization
def normpath(path):
    return  os.path.normpath(path)


##
# \brief Checks dependent command line options and their values
def crosscheck_args(parser, args):
    if args.remove_cog_comments and args.overwrite:
        parser.error('--remove-cog-comments cannot be used with '\
                     '--overwrite-input-files'.format(METAVAR_DIRECTORY))

    if args.overwrite and args.output_directory is not None:
        parser.error('--overwrite-input-files cannot be used '\
                     'with -o {}'.format(METAVAR_DIRECTORY))

    if args.input_files_dirs and args.output_directory:
        if util.check_outpdirectory_in_input_files_directories(
                args.input_files_dirs, args.output_directory):
            parser.error(
                'Output directory is part of specified input, this can '\
                'destroy your input data! To overwrite, use option '\
                '--overwrite-input-files'.format(args.output_directory))

    if args.input_files_dirs:
        if not (args.output_directory or args.overwrite):
            parser.error('if -i is specified, -o or --overwrite-input-files '\
                         'is required!')
        if not (args.osdd_file and args.settings_file):
            parser.error('if -i is specified, -d and -s is required!')

    if args.output_directory and not args.input_files_dirs:
        parser.error('if -o is specified, -i is required!')

    if args.list_modules and not args.osdd_file:
        parser.error ('-l / --list-modules requires option -d!')


##
# \brief Wraps the os.path exists function and uses errorhandling
def check_path_exists(file_dir):
    if not os.path.exists(file_dir):
        errorhandler.ErrorHandler.error(
            'could not find file / directory "{}"'.format(file_dir))
        # SystemExit via Errorhandler
    return True


##
# \brief validate commandline args according to expected values
def validate_args(parser, args):

    if args is None:
        parser.print_usage()
        sys.exit(common.SODBsysexit.EXIT_SUCCESS)

    # list of input files / directories, which existences need to be checked
    input_files_dirs_to_check = []

    if args.osdd_file:
        if not args.osdd_file.endswith(common.DefaultFileExtensions.EXT_OSDD):
            parser.error('expected osdd file with ending {}'.format(
                            common.DefaultFileExtensions.EXT_OSDD))
        else:
            input_files_dirs_to_check.append(args.osdd_file)
            args.osdd_file = normpath(args.osdd_file)

    if args.module_id:
        pass

    if args.settings_file:
        if not args.settings_file.endswith(
                common.DefaultFileExtensions.EXT_SETTINGS):
            parser.error('expected settings file with ending {}'.format(
                            common.DefaultFileExtensions.EXT_SETTINGS))
        input_files_dirs_to_check.append(args.settings_file)
        args.settings_file = normpath(args.settings_file)

    if args.input_files_dirs:
        args.input_files_dirs = list (map(normpath, args.input_files_dirs))
        input_files_dirs_to_check.extend(args.input_files_dirs)

    if args.output_directory:
        args.output_directory = normpath(args.output_directory)

    if args.overwrite:
        assert (isinstance(args.overwrite, bool))

    if args.file_encoding:
        # try to find a valid codec for desired encoding
        try:
            codecs.lookup(args.file_encoding)
        except:
            parser.error('encoding "{}" not supported'.format(
                            args.file_encoding))

    if args.list_modules:
        assert (isinstance(args.list_modules, bool))

    if args.generate_binding_file:
        if not args.generate_binding_file[0].endswith(
                common.DefaultFileExtensions.EXT_SCHEMA):
            parser.error('expected xml schema file with ending {}'.format(
                            common.DefaultFileExtensions.EXT_SCHEMA))

        input_files_dirs_to_check.append(args.generate_binding_file[0])

        if not args.generate_binding_file[1].endswith(
                common.DefaultFileExtensions.EXT_BINDING):
            parser.error('expected binding file with ending {}'.format(
                            common.DefaultFileExtensions.EXT_BINDING))

        args.generate_binding_file = list(map(normpath,
                                              args.generate_binding_file))

    if args.overrule_osdd:
        assert (isinstance(args.overrule_osdd, bool))

    if args.remove_cog_comments:
        assert (isinstance(args.remove_cog_comments, bool))

    if args.dry_run:
        assert (isinstance(args.dry_run, bool))

    if args.verbose:
        assert (isinstance(args.verbose, bool))

    if args.log_file:
        args.log_file = normpath(args.log_file)

    crosscheck_args(parser, args)

    # check, if all entered input files / directories exist
    list(map(check_path_exists, input_files_dirs_to_check))

    return args

##
# \}
# \}
