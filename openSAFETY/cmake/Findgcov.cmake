# - Find gcov executable
# Will define:
#
# GCOV_EXECUTABLE - the gcov executable
#
# Uses:
#
# GCOV_ROOT - root to search for the script
#
# Copyright (C) 2012 by Roland Knall roland dot knall at br minus automation dot co dot at
#
# This file may be licensed under the terms of the
# GNU Lesser General Public License Version 3 (the ``LGPL''),
# or (at your option) any later version.
#
# Software distributed under the License is distributed
# on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
# express or implied. See the LGPL for the specific language
# governing rights and limitations.
#
# You should have received a copy of the LGPL along with this
# program. If not, go to http://www.gnu.org/licenses/lgpl.html
# or write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The development of this software was supported by:
#   CoR-Lab, Research Institute for Cognition and Robotics
#     Bielefeld University

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM(GCOV_EXECUTABLE gcov HINTS ${GCOV_ROOT} "${GCOV_ROOT}/bin" "${CMAKE_SOURCE_PATH}../contrib/scripts" )

FIND_PACKAGE_HANDLE_STANDARD_ARGS(gcov DEFAULT_MSG GCOV_EXECUTABLE)

# only visible in advanced view
MARK_AS_ADVANCED(GCOV_EXECUTABLE)

