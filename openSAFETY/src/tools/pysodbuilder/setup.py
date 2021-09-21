##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file setup.py
# Functionality for installing the package and making a source distribution

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
import distutils.cmd
from distutils.core import setup
import sys
import os

# third party packages or modules

# own packages or modules
from pysodb.core import versioninfo


PYTHON_FILE_EXTENSION = '.py'

do_filechecks = False

if len(sys.argv) >= 2 and sys.argv[1] == 'sdist':
    do_filechecks = True

pypackages_modules = {
    './pysodb':['__init__.py', 'pysodbmain.py'],
    './pysodb/binding':['__init__.py'],
    './pysodb/core':[
        '__init__.py', 'argparser.py', 'codegenerator.py', 'common.py',
        'configparser.py', 'errorhandler.py', 'filehandling.py', 'util.py',
        'versioninfo.py'
        ],
    './pysodb/codegeneration':[
        '__init__.py', 'codegenutil.py', 'common.py', 'c_code_util.py',
        'c_target_data.py', 'genappc.py', 'geneplscfgh.py', 'gensodc.py',
        'gensodutilh.py',
        ],
    './pysodb/osddprocessing':[
        '__init__.py', 'base.py' , 'mainprocessing.py', 'postprocessing.py',
        'preprocessing.py', 'processingdata.py'
        ],
    './pysodb/util':[
        '__init__.py', 'constants.py', 'defaultsod.py', 'macro.py',
        'osdd_data.py', 'sod_data.py', 'stringfunctions.py', 'structure.py',
        'type_conversion.py'
        ],
    './tests':['__init__.py', 'common.py', 'test_pysodbmain.py'],
    './tests/binding':['__init__.py', 'xosddbinding.py'],
    './tests/codegeneration':[
        '__init__.py', 'test_codegenutil.py', 'test_common.py',
        'test_c_code_util.py', 'test_c_target_data.py', 'test_genappc.py',
        'test_geneplscfgh.py', 'test_gensodc.py', 'test_gensodutilh.py'
        ],
    './tests/core':[
        '__init__.py', 'gentestc.py', 'test_argparser.py',
        'test_codegenerator.py', 'test_common.py', 'test_configparser.py',
        'test_errorhandler.py', 'test_filehandling.py', 'test_util.py',
        'test_versioninfo.py'
        ],
    './tests/osddprocessing':[
        '__init__.py', 'test_base.py', 'test_mainprocessing.py',
        'test_postprocessing.py', 'test_preprocessing.py',
        'test_processingdata.py',
        ],
    './tests/util/':[
        '__init__.py', 'test_constants.py', 'test_defaultsod.py',
        'test_macro.py', 'test_osdd_data.py', 'test_sod_data.py',
        'test_stringfunctions.py', 'test_structure.py',
        'test_type_conversion.py'
        ]
    }

other_files = [
'./README.txt',
'./MANIFEST.in',
'./LICENSE.txt',
'./setup.py',
'./scripts/pysodbuilder.py',
'./pysodb/binding/README.txt',
'./tests/testdata/empty.xosdd',
'./tests/testdata/illegal_multiple_elements.xosdd',
'./tests/testdata/input/EPLScfg.h',
'./tests/testdata/input/Test.c',
'./tests/testdata/input/TestNoCogcomments.c',
'./tests/testdata/input/TestOverwrite.c',
'./tests/testdata/missing_closing_tag.xosdd',
'./tests/testdata/missing_mandatory_elements.xosdd',
'./tests/testdata/pysodbsettings.ini',
'./tests/testdata/settings_double_entries.ini',
'./tests/testdata/settings_missing_sectionheader.ini',
'./tests/testdata/settings_missing_sections.ini',
'./tests/testdata/settings_no_assignment.ini',
'./tests/testdata/settings_wrong_attributenames.ini',
'./tests/testdata/test.xosdd',
'./tests/testdata/two_modules.xosdd',
'./tests/testdata/utf16test.txt',
'./tests/testdata/utf32letest.txt',
'./tests/testdata/utf8test.txt',
'./tests/testdata/ValidationTest.c',
'./tests/testdata/ValidationTestNoCogcomments.c',
'./tests/testdata/XOSDDLocalize_R15.xsd',
'./tests/testdata/XOSDD_R15.xsd'
                           ]
pypackages = []
foundfiles = []

class TestCommand(distutils.cmd.Command):
    user_options = []
    def initialize_options(self):
        self.tests_path = 'tests'

    def finalize_options(self):
        if self.tests_path:
            assert os.path.exists(self.tests_path)

    def run(self):
        #try to import nose
        try:
            print ('try to use "nose" for unittesting')
            import nose
            nose.run(argv=['', '-w', 'tests', '--with-xunit'])
        except:
            print('running unittests with "nose" not successfull, '\
                  'running standard python unittesting')
            import unittest
            test_loader = unittest.defaultTestLoader
            test_suite = test_loader.discover("tests")
            unittest.TextTestRunner().run(test_suite)


def translate_rel_path_to_package (path):
    npath = os.path.normpath(path)
    npath = npath.replace("\\", ".")
    npath = npath.replace("/", ".")
    return npath

for item in pypackages_modules.keys():
    pypackages.append(translate_rel_path_to_package(item))

if do_filechecks:

    print (os.getcwd())
    caller_cwd = os.getcwd()
    setuppy_dir = os.path.dirname(os.path.abspath(__file__))
    print ("changing to {}".format(setuppy_dir))
    os.chdir(setuppy_dir)
    print (os.getcwd())

    print ("checking if filelist is complete")

    python_sources = []
    visited_directories = []

    for item in pypackages_modules.keys():
        for file in pypackages_modules[item]:
            pythonfilepath = os.path.join(item, file)
            python_sources.append(pythonfilepath)

    for file_path in python_sources + other_files:
        if os.path.exists(file_path) and os.path.isfile(file_path):
            print ("Success: file {} found".format(file_path))
            foundfiles.append(os.path.normpath(file_path))
            visited_directories.append(os.path.dirname(file_path))

        else:
            print ("ERROR: file {} not found".format(
                                                os.path.basename(file_path)))
            sys.exit(1)

    dirlist = []

    visited_directories = set(visited_directories)

    for item in visited_directories:
        for directory_file in os.listdir(item):
            if os.path.isfile(directory_file):
                file_path = os.path.normpath(os.path.join(item, directory_file))
                dirlist.append(file_path)

    maybe_forgotten_files = list(set(dirlist) - set(foundfiles))
    #remove .pyc files from list
    for item in maybe_forgotten_files[:]:
        if item.endswith('.pyc'):
            maybe_forgotten_files.remove(item)
    if len(maybe_forgotten_files) > 0:
        print()
        print ('Warning: there are more files the given directories, '\
               'than specified for checking. Make sure nothing has ' \
               'been forgotten.')
        print ('Files: ', ', '.join(maybe_forgotten_files))
        print()

    print ("checking complete\n")


setup(
      cmdclass={'test': TestCommand},
      name='pysodbuilder',
      version=versioninfo.SODBVersionInfo.openSAFETY_VERSION_RELEASE,
      description='Python SODBuilder',
      author='Alexander Brunner',
      author_email='alexander.brunner@br-automation.com',
      url='http://sourceforge.net/projects/opensafety/',
      packages=pypackages,
      scripts=['scripts/pysodbuilder.py'],
      license='BSD'
      )

if do_filechecks:
    os.chdir(caller_cwd)

##
# \}
# \}
