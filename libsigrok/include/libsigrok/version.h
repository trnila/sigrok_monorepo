/* include/libsigrok/version.h.  Generated from version.h.in by configure.  */
/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2010-2012 Bert Vermeulen <bert@biot.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBSIGROK_VERSION_H
#define LIBSIGROK_VERSION_H

/*
 * Only libsigrok library builds, and only parts of the library build,
 * need to reference the git-version.h header file. Which contains the
 * version suffix, which is relevant to local development, but is not
 * applicable to release builds. Application builds need not bother with
 * internal library version details, and always can get this information
 * in text form for display purposes from the library at runtime.
 */
#if defined WANT_LIBSIGROK_GIT_VERSION_H
#  include <libsigrok/git-version.h>
#else
/* #  undef SR_PACKAGE_VERSION_STRING_SUFFIX */
#  define SR_PACKAGE_VERSION_STRING_SUFFIX ""
#endif

/**
 * @file
 *
 * Version number definitions and macros.
 */

/**
 * @ingroup grp_versions
 *
 * @{
 */

/*
 * Package version macros (can be used for conditional compilation).
 */

/** The libsigrok package 'major' version number. */
#define SR_PACKAGE_VERSION_MAJOR 0

/** The libsigrok package 'minor' version number. */
#define SR_PACKAGE_VERSION_MINOR 6

/** The libsigrok package 'micro' version number. */
#define SR_PACKAGE_VERSION_MICRO 0

/** The libsigrok package version ("major.minor.micro") as string. */
#define SR_PACKAGE_VERSION_STRING_PREFIX "0.6.0-git"

/** The libsigrok package version with git commit suffix. */
#define SR_PACKAGE_VERSION_STRING (SR_PACKAGE_VERSION_STRING_PREFIX SR_PACKAGE_VERSION_STRING_SUFFIX)

/*
 * Library/libtool version macros (can be used for conditional compilation).
 */

/** The libsigrok libtool 'current' version number. */
#define SR_LIB_VERSION_CURRENT 4

/** The libsigrok libtool 'revision' version number. */
#define SR_LIB_VERSION_REVISION 0

/** The libsigrok libtool 'age' version number. */
#define SR_LIB_VERSION_AGE 0

/** The libsigrok libtool version ("current:revision:age") as string. */
#define SR_LIB_VERSION_STRING "4:0:0"

/** @} */

#endif
