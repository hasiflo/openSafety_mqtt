##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file codegenerator.py
# Implements code generator classes for specific languages
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
import re
from pysodb.util.constants import RegExPatterns

# third party packages or modules

# own packages or module


##
# \brief Placeholder class for programming languages
#
# CodeGenerator sublcasses will get registered here,
# if they are decorated with CodeGeneratorLanguage
class Languages:
    pass


##
# \brief Returns a CodeGenerator subclass for the specified language
#
# This function returns the appropriate CodeGenerator subclass, for the
# specified language, which then can be instantiated.
def get_codegenerator_class(language):
    if hasattr(Languages, language):
        return getattr(Languages, language)
    else:
        raise AttributeError ('Cannot find appropriate CodeGenerator for '\
                              'language "{}"!'.format(language))


##
# \brief Decorator class for decorationg CodeGenerator subclasses
#
# CodeGeneratorClasses decorated with CodeGeneratorLanguage will
# be registered in class Languages
class CodegeneratorLanguage(object):

    ##
    # \brief Initialises the decorator Class with the given language
    #
    # \param language Programminglanguage to set, may contain only
    # alphanumerical letters
    def __init__ (self, language):
        if re.match(RegExPatterns.LANGUAGE_IDENTIFIER, language):
            self.language = language
        else:
            raise SyntaxError('Language parameter does not match valid '\
                              'identifier pattern ([a-zA-Z_][a-zA-Z0-9_])!')

    ##
    # \brief Makes Class callable, needed for decoration
    #
    # \param target Target class to decorate, must be a subclass of
    # CodeGeneratorBase
    def __call__(self, target):
        # check, if class to decorate is allowed to be decorated
        if issubclass(target, CodeGeneratorBase):
            setattr(target, 'language', self.language)
        else:
            raise TypeError ('{} cannot be decorated with {}'.format(
                                target.__name__, self.__class__.__name__))

        # register CodeGenerator with language in Languages
        if (hasattr(Languages, self.language)):
            raise TypeError('Codegenerator class for language "{}" '\
                            'already defined!'.format(self.language))
        else:
            setattr(Languages, self.language, target)
        return target


##
# \brief Base class for code generators
class CodeGeneratorBase(object):

    ## Used programming language by the code generator
    language = None

    ##
    # \brief Initialisation function
    #
    # \param outfunc List of outputfunctions which will be called, when
    # the class generates code
    # \param linelength Maximum length of a line in generated code
    def __init__(self, outfunc=None, linelength=80):
        if not outfunc:
            outfunc = list()
        self.outfuncs = set(outfunc[:])
        self.linelength = linelength
        self.indentWithTab = False
        self.indentWithWhitespace = True
        self.newLine = '\r\n'

    ##
    # \brief Triggers the output of the given text to all output functions
    #
    # \param text String which gets passed to all output functions in the
    # outfunc list
    def outfunc(self, text):
        for func in self.outfuncs:
            func(text)

    ##
    # \brief Adds an additional output function to the list of output functions
    #
    # This enables writing the generated code to several destinations.
    # \param func Function name to add to the list of output functions,
    # the function should take a string
    def addoutfunc(self, func):
        self.outfuncs.add(func)

    ##
    # \brief Removes specific output function form code generator
    #
    # \param func Name of the function to remove from outfunc list
    def removeoutfunc(self, func):
        self.outfuncs.remove(func)


##
# \brief CodeGenerator subclass for generating C code
#
# C code generator class with specific properties for the
# C programming language.
@CodegeneratorLanguage('C')
class CCodeGenerator(CodeGeneratorBase):

    begincommenttoken = '/*'
    endcommenttoken = '*/'

    def __init__(self, outfunc=None):
        self.whitespaceIndent = 4
        CodeGeneratorBase.__init__(self, outfunc)

    ##
    # \brief Returns the letter / letters for a tabulator according to the
    # settings done before
    #
    # \return '\\t' or whitespaces
    def getTab(self, number=1, strlen_before=0):
        if (self.indentWithTab):
            return '\t'
        else:
            return (' ' * ((self.whitespaceIndent * number) -
                           (strlen_before % self.whitespaceIndent)))

    ##
    # \brief Generates a C define with given name and value
    #
    # \param name Identifier of the define
    # \param value Value of the define
    def gen_definition (self, name, value):
        self.outfunc('#define {0} {1}'.format(name, value))

    ##
    # \brief Generates a C define with given name, value and comment
    #
    # This function generates a C define with given name, value and comment.
    # The comment is assumend to be a valid comment (with appropriate
    # start (and ending), therefore no validation of the comment is performed.
    #
    # \param name Identifier of the define
    # \param value Value of the define
    # \param valid_comment Comment of the define
    def gen_definition_with_comment(self, name, value, valid_comment):
        self.outfunc('#define {0} {1} {2}'.format(name, value, valid_comment))

    ##
    # \brief Generates a C comment
    #
    # \param comment Comment text
    def gen_comment(self, comment):
        self.outfunc('/* {} */'.format(comment))

    ##
    # \brief Writes unformatted code, just as passed to the function
    #
    # \param code Text or code which gets directly passed to all output
    # functions
    def gen_rawCode(self, code):
        self.outfunc(code)

    ##
    # \brief Creates a section header comment
    #
    # \param text Text which goes inside that header
    def gen_comment_sectionheader(self, text):
        len_begin_tk = len(CCodeGenerator.begincommenttoken)
        line = ('*' * (self.linelength - len_begin_tk))
        self.outfunc(''.join([CCodeGenerator.begincommenttoken, line]))
        self.outfunc(''.join([' **', self.getTab(), text]))
        line = '*' * ((self.linelength - len_begin_tk) - 1)
        self.outfunc(''.join([' ', line, CCodeGenerator.endcommenttoken]))

    ##
    # \brief Generates a doxygen comment begin token
    def gen_comment_doxygen_begin(self):
        self.outfunc('/**')

    ##
    # \brief Generates a doxygen comment end token
    def gen_comment_doxygen_end(self):
        self.outfunc(' */')

    ##
    # \brief Passes given content to output function
    #
    # \param content Text to pass to the output functions
    def gen_comment_doxygen_content(self, content):
        self.outfunc(''.join([' * ', content]))

    ##
    # \brief Generates multiline doxygen comments
    #
    # \param lines List of strings (lines) which are printed between
    # doxygen comment beginning and closing tags / tokens
    def gen_comment_multiline_doxygen (self, lines):
        self.gen_comment_doxygen_begin()
        for line in lines:
            self.gen_comment_doxygen_content(line)
        self.gen_comment_doxygen_end()

    ##
    # \brief Generates an inline doxygen comment
    #
    # \param comment Text for the comment
    def gen_comment_inline_doxygen(self, comment):
        self.outfunc(self.get_comment_inline_doxygen(comment))

    ##
    # \brief Returns an inline doxygen comment
    #
    # \param comment Text for the comment
    # \return Doxygen style comment string
    def get_comment_inline_doxygen(self, comment):
        return ''.join(['/** ', comment, ' */'])

    ##
    # \brief Generates a doxygen member documentation comment
    #
    # \param comment Text for the comment
    def gen_comment_inline_doxygen_member(self, comment):
        self.outfunc(self.get_comment_inline_doxygen_member(comment))

    ##
    # \brief Returns a doxygen member documentation comment
    #
    # \param comment Text for the comment
    # \return Doxygen style inline comment string
    def get_comment_inline_doxygen_member(self, comment):
        return ''.join(['/**< ', comment, ' */'])

    ##
    # \brief Sets new line style
    #
    # \param newline String for newline
    def set_newline_style(self, newline):
        self.newLine = newline

    ##
    # \brief Sets identation with tabs the default
    def setIndentWithTab(self):
        self.indentWithTab = True
        self.indentWithWhitespace = False

    ##
    # \brief Sets identation with whitespaces the default
    def setIndentWithWhitespace(self):
        self.indentWithTab = False
        self.indentWithWhitespace = True

    ##
    # \brief Sets number of whitespaces for identation
    #
    # \param num Number of whitespaces for use in identation
    def setWhitespaceIndentation(self, num):
        self.whitespaceIndent = num

##
# \}
# \}
