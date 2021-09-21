# This file is originally part of the Wirehark source code distribution.
# It has been copied from /cmake/modules/LocatePythonModule.cmake
# and has been modified with the license texts of the Wireshark source
# code distribution in /cmake/modules/LICENSE.txt
# see wireshark.org and code.wireshark.org

###################################################################
#
#  Copyright (c) ....
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
###################################################################
#
#  Copyright (c) ...
#  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
#
# * Neither the name of the <ORGANIZATION> nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#LOCATE_PYTHON_MODULE(<module> [PATHS <path1> ... <pathN>] [REQUIRED])
#
# This function tries to find the given python module.
# If found the path is provided in <PY_<module> and <<module>_FOUND> is set to TRUE.
#
# After PATHS additional paths for python to search can be provided.
# When REQUIRED is set, the function will abort the cmake execution is the module is not found
function(LOCATE_PYTHON_MODULE module)
	find_package(PythonInterp)

	# Parse (additional) arguments
	set(options REQUIRED)
	set(multiValueArgs PATHS)
	cmake_parse_arguments(LPM "${options}" "" "${multiValueArgs}" ${ARGN})

	string(TOUPPER ${module} module_upper)
	if(NOT PY_${module_upper})

		if(LPM_PATHS)
			# Append LPM_PATHS to PYTHONPATH to search at provided location (first)
			file(TO_CMAKE_PATH "$ENV{PYTHONPATH}" CMAKE_PATH)
			list(INSERT CMAKE_PATH 0 ${LPM_PATHS})
			file(TO_NATIVE_PATH "${CMAKE_PATH}" NATIVE_PATH)
			if(UNIX)
				string(REPLACE ";" ":" NATIVE_PATH "${NATIVE_PATH}")
			endif(UNIX)
			set(ENV{PYTHONPATH} "${NATIVE_PATH}")
		endif(LPM_PATHS)

		# Use the (native) python impl module to find the location of the requested module
		execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c"
			"import ${module}; print('${module} found')"
			RESULT_VARIABLE _${module}_status
			OUTPUT_VARIABLE _${module}_location
			ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

		if(NOT _${module}_status)
			set(PY_${module_upper} ${_${module}_location} CACHE STRING
				"Location of Python module ${module}")
			set(${module_upper}_FOUND TRUE)
			message(STATUS "Found python module ${module}: ${PY_${module_upper}}")
		else(NOT _${module}_status)
			set(${module_upper}_FOUND FALSE)
			if(LPM_REQUIRED)
				message(FATAL_ERROR "Could NOT find python module ${module}")
			else(LPM_REQUIRED)
				message(STATUS "Could NOT find python module ${module}")
			endif(LPM_REQUIRED)
		endif(NOT _${module}_status)
	endif(NOT PY_${module_upper})
endfunction(LOCATE_PYTHON_MODULE)
