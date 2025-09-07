/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* The canonical host libsigrokdecode will run on. */
#define CONF_HOST "x86_64-pc-linux-gnu"

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "sigrok-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libsigrokdecode"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libsigrokdecode 0.6.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libsigrokdecode"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.sigrok.org"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.6.0"

/* Binary age of libsigrokdecode. */
#define SRD_LIB_VERSION_AGE 0

/* Binary version of libsigrokdecode. */
#define SRD_LIB_VERSION_CURRENT 4

/* Binary revision of libsigrokdecode. */
#define SRD_LIB_VERSION_REVISION 0

/* Binary version triple of libsigrokdecode. */
#define SRD_LIB_VERSION_STRING "4:0:0"

/* Major version number of libsigrokdecode. */
#define SRD_PACKAGE_VERSION_MAJOR 0

/* Micro version number of libsigrokdecode. */
#define SRD_PACKAGE_VERSION_MICRO 0

/* Minor version number of libsigrokdecode. */
#define SRD_PACKAGE_VERSION_MINOR 6

/* Version of libsigrokdecode. */
#define SRD_PACKAGE_VERSION_STRING "0.6.0-git-71f4514"

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* The targeted POSIX standard. */
#ifndef _POSIX_C_SOURCE
# define _POSIX_C_SOURCE 200112L
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
