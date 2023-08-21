# ===========================================================================
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_YAML_CPP([action-if-found], [action-if-not-found])
#
# DESCRIPTION
#
#   This macro searches for an installed yaml-cpp library. If nothing was
#   specified when calling configure, it searches first in /usr/local and
#   then in /usr, /opt/local and /sw. If the --with-yaml-cpp=DIR is specified,
#   it will try to find it in DIR/include/yaml-cpp/yaml.h and DIR/lib/libyaml-cpp.so. If
#   --without-yaml-cpp is specified, the library is not searched at all.
#
#   If either the header file (yaml-cpp/yaml.h) or the library (libyaml-cpp) is not found,
#   shell commands 'action-if-not-found' is run. If 'action-if-not-found' is
#   not specified, the configuration exits on error, asking for a valid yaml-cpp
#   installation directory or --without-yaml-cpp.
#
#   If both header file and library are found, shell commands
#   'action-if-found' is run. If 'action-if-found' is not specified, the
#   default action appends '-I${YAML_CPP_HOME}/include' to CPFLAGS, appends
#   '-L$YAML_CPP_HOME}/lib' to LDFLAGS, prepends '-lyaml-cpp' to LIBS, and calls
#   AC_DEFINE(HAVE_YAML_CPP). You should use autoheader to include a definition
#   for this symbol in a config.h file. Sample usage in a C/C++ source is as
#   follows:
#
#     #ifdef HAVE_YAML_CPP
#     #include <yaml-cpp/yaml.h>
#     #endif /* HAVE_YAML_CPP */
#
# LICENSE
#
#   Copyright (c) 2023 Lightning Auriga <lightning.auriga@gmail.com>
#
#   Derived from <http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/ax_check_zlib.m4>
#   Copyright (c) 2008 Loic Dachary <loic@senga.org>
#   Copyright (c) 2010 Bastien Chevreux <bach@chevreux.org>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 16

AU_ALIAS([CHECK_YAML_CPP], [AX_CHECK_YAML_CPP])
AC_DEFUN([AX_CHECK_YAML_CPP],
#
# Handle user hints
#
[AC_MSG_CHECKING(if yaml-cpp is wanted)
yaml_cpp_places="/usr/local /usr /opt/local /sw"
AC_ARG_WITH([yaml-cpp],
[  --with-yaml-cpp=DIR     root directory path of yaml-cpp installation @<:@defaults to
                          /usr/local or /usr if not found in /usr/local@:>@
  --without-yaml-cpp      to disable yaml-cpp usage completely],
[if test "$withval" != no ; then
  AC_MSG_RESULT(yes)
  if test -d "$withval"
  then
    yaml_cpp_places="$withval $yaml_cpp_places"
  else
    AC_MSG_WARN([Sorry, $withval does not exist, checking usual places])
  fi
else
  yaml_cpp_places=
  AC_MSG_RESULT(no)
fi],
[AC_MSG_RESULT(yes)])

#
# Locate yaml-cpp, if wanted
#
if test -n "${yaml_cpp_places}"
then
	# check the user supplied or any other more or less 'standard' place:
	#   Most UNIX systems      : /usr/local and /usr
	#   MacPorts / Fink on OSX : /opt/local respectively /sw
	for YAML_CPP_HOME in ${yaml_cpp_places} ; do
	  if test -f "${YAML_CPP_HOME}/include/yaml-cpp/yaml.h"; then break; fi
	  YAML_CPP_HOME=""
	done

  YAML_CPP_OLD_LDFLAGS=$LDFLAGS
  YAML_CPP_OLD_CPPFLAGS=$CPPFLAGS
  if test -n "${YAML_CPP_HOME}"; then
        LDFLAGS="$LDFLAGS -L${YAML_CPP_HOME}/lib"
        CPPFLAGS="$CPPFLAGS -I${YAML_CPP_HOME}/include"
  fi
  AC_LANG_PUSH([C++])
  AC_CHECK_HEADER([yaml-cpp/yaml.h], [yaml_cpp_cv_yaml_cpp_h=yes], [yaml_cpp_cv_yaml_cpp_h=no])
  AC_LANG_POP([C++])
  if test "$yaml_cpp_cv_yaml_cpp_h" = "yes"
  then
    #
    # If both library and header were found, action-if-found
    #
    m4_ifblank([$1],[
                CPPFLAGS="$CPPFLAGS -I${YAML_CPP_HOME}/include"
                LDFLAGS="$LDFLAGS -L${YAML_CPP_HOME}/lib"
                LIBS="-lyaml-cpp $LIBS"
                AC_DEFINE([HAVE_YAML_CPP], [1],
                          [Define to 1 if you have `yaml-cpp' library (-lyaml-cpp)])
               ],[
                # Restore variables
                LDFLAGS="$YAML_CPP_OLD_LDFLAGS"
                CPPFLAGS="$YAML_CPP_OLD_CPPFLAGS"
                $1
               ])
  else
    #
    # If either header or library was not found, action-if-not-found
    #
    m4_default([$2],[
                AC_MSG_ERROR([either specify a valid yaml-cpp installation with --with-yaml-cpp=DIR or disable yaml-cpp usage with --without-yaml-cpp])
                ])
  fi
fi
])
