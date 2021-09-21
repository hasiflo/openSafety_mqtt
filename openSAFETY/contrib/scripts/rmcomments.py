#!/usr/bin/env python

# Original source: http://www.saltycrane.com/blog/2007/11/remove-c-comments-python/

import re
import sys

def remove_comments(text):
    """ remove c-style comments.
        text: blob of text with comments (can include newlines)
        returns: text with comments removed
    """
    pattern = r"""
                            ##  --------- COMMENT ---------
           /\*              ##  Start of /* ... */ comment
           [^*]*\*+         ##  Non-* followed by 1-or-more *'s
           (                ##
             [^/*][^*]*\*+  ##
           )*               ##  0-or-more things which don't start with /
                            ##    but do end with '*'
           /                ##  End of /* ... */ comment
         |                  ##  -OR-  various things which aren't comments:
           (                ## 
                            ##  ------ " ... " STRING ------
             "              ##  Start of " ... " string
             (              ##
               \\.          ##  Escaped char
             |              ##  -OR-
               [^"\\]       ##  Non "\ characters
             )*             ##
             "              ##  End of " ... " string
           |                ##  -OR-
                            ##
                            ##  ------ ' ... ' STRING ------
             '              ##  Start of ' ... ' string
             (              ##
               \\.          ##  Escaped char
             |              ##  -OR-
               [^'\\]       ##  Non '\ characters
             )*             ##
             '              ##  End of ' ... ' string
           |                ##  -OR-
                            ##
                            ##  ------ ANYTHING ELSE -------
             .              ##  Anything other char
             [^/"'\\]*      ##  Chars which doesn't start a comment, string
           )                ##    or escape
    """
    regex = re.compile(pattern, re.VERBOSE|re.MULTILINE|re.DOTALL)
    noncomments = [m.group(2) for m in regex.finditer(text) if m.group(2)]

    return "".join(noncomments)

if __name__ == '__main__':
	if ( len(sys.argv) == 3 ):
		filename = sys.argv[1]
		if ( sys.version_info < (3,0)):
			code_w_comments = open(filename).read().decode('latin_1')
		else:
			code_w_comments = open(filename, encoding="latin-1").read()
		code_wo_comments = remove_comments(code_w_comments)
		# Convert CRLF to LF
		code_wo_comments = code_wo_comments.replace("\r\n", "\n")
		# b is necessary, to ensure, that files are written with the same line
		# endings they had when they where opened
		fh = open(sys.argv[2], "wb")
		if ( sys.version_info < (3,0)):
			fh.write(code_wo_comments.encode('utf_8'))
		else:
			fh.write(bytes(code_wo_comments, 'UTF-8'))
		fh.close()
	else:
		print ( "Insufficient number of arguments!\n%s <file_with_comments> <output_file>" % sys.argv[0] )