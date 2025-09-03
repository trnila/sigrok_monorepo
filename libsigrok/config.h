/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* The canonical host libsigrok will run on. */
#define CONF_HOST "x86_64-pc-linux-gnu"

/* Build-time version of libbluez. */
/* #undef CONF_LIBBLUEZ_VERSION */

/* Build-time version of libftdi. */
#define CONF_LIBFTDI_VERSION "1.5"

/* Build-time version of libgio. */
#define CONF_LIBGIO_VERSION "2.72.4"

/* Build-time version of libgpib. */
/* #undef CONF_LIBGPIB_VERSION */

/* Build-time version of libhidapi. */
/* #undef CONF_LIBHIDAPI_VERSION */

/* Build-time version of libnettle. */
#define CONF_LIBNETTLE_VERSION "3.7.3"

/* Build-time version of librevisa. */
/* #undef CONF_LIBREVISA_VERSION */

/* Build-time version of libserialport. */
/* #undef CONF_LIBSERIALPORT_VERSION */

/* Build-time version of libusb. */
#define CONF_LIBUSB_1_0_VERSION "1.0.25"

/* Build-time version of libzip. */
#define CONF_LIBZIP_VERSION "1.7.3"

/* Build-time version of zlib. */
#define CONF_ZLIB_VERSION "1.2.11"

/* Specifies whether Bluetooth communication is supported. */
/* #undef HAVE_BLUETOOTH */

/* define if the compiler supports basic C++11 syntax */
/* #undef HAVE_CXX11 */

/* define if the compiler supports basic C++14 syntax */
/* #undef HAVE_CXX14 */

/* define if the compiler supports basic C++17 syntax */
#define HAVE_CXX17 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Whether at least one driver is enabled. */
#define HAVE_DRIVERS 1

/* Define to 1 if you have the `ftdi_tciflush' function. */
#define HAVE_FTDI_TCIFLUSH 1

/* Define to 1 if you have the `ftdi_tcioflush' function. */
#define HAVE_FTDI_TCIOFLUSH 1

/* Define to 1 if you have the `ftdi_tcoflush' function. */
#define HAVE_FTDI_TCOFLUSH 1

/* Whether to support Agilent DMM device. */
/* #undef HAVE_HW_AGILENT_DMM */

/* Whether to support Appa 55II device. */
/* #undef HAVE_HW_APPA_55II */

/* Whether to support Arachnid Labs Re:load Pro device. */
/* #undef HAVE_HW_ARACHNID_LABS_RE_LOAD_PRO */

/* Whether to support ASIX OMEGA RTM CLI device. */
#define HAVE_HW_ASIX_OMEGA_RTM_CLI 1

/* Whether to support ASIX SIGMA/SIGMA2 device. */
#define HAVE_HW_ASIX_SIGMA 1

/* Whether to support Atorch device. */
/* #undef HAVE_HW_ATORCH */

/* Whether to support Atten PPS3xxx device. */
/* #undef HAVE_HW_ATTEN_PPS3XXX */

/* Whether to support BayLibre ACME device. */
#define HAVE_HW_BAYLIBRE_ACME 1

/* Whether to support BeagleLogic device. */
#define HAVE_HW_BEAGLELOGIC 1

/* Whether to support BK Precision 1856D device. */
/* #undef HAVE_HW_BKPRECISION_1856D */

/* Whether to support CEM DT-885x device. */
/* #undef HAVE_HW_CEM_DT_885X */

/* Whether to support Center 3xx device. */
/* #undef HAVE_HW_CENTER_3XX */

/* Whether to support ChronoVu LA device. */
#define HAVE_HW_CHRONOVU_LA 1

/* Whether to support Colead SLM device. */
/* #undef HAVE_HW_COLEAD_SLM */

/* Whether to support Conrad DIGI 35 CPU device. */
/* #undef HAVE_HW_CONRAD_DIGI_35_CPU */

/* Whether to support dcttech usbrelay device. */
/* #undef HAVE_HW_DCTTECH_USBRELAY */

/* Whether to support demo device. */
#define HAVE_HW_DEMO 1

/* Whether to support Devantech ETH008 device. */
/* #undef HAVE_HW_DEVANTECH_ETH008 */

/* Whether to support DreamSourceLab DSLogic device. */
#define HAVE_HW_DREAMSOURCELAB_DSLOGIC 1

/* Whether to support Fluke 45 device. */
#define HAVE_HW_FLUKE_45 1

/* Whether to support Fluke DMM device. */
/* #undef HAVE_HW_FLUKE_DMM */

/* Whether to support FTDI LA device. */
#define HAVE_HW_FTDI_LA 1

/* Whether to support fx2lafw device. */
#define HAVE_HW_FX2LAFW 1

/* Whether to support GMC MH 1x/2x device. */
/* #undef HAVE_HW_GMC_MH_1X_2X */

/* Whether to support Great Scott Gadgets GreatFET One device. */
#define HAVE_HW_GREATFET 1

/* Whether to support GW Instek GDS-800 device. */
/* #undef HAVE_HW_GWINSTEK_GDS_800 */

/* Whether to support GW Instek GPD device. */
/* #undef HAVE_HW_GWINSTEK_GPD */

/* Whether to support GW Instek PSP device. */
#define HAVE_HW_GWINSTEK_PSP 1

/* Whether to support Hameg HMO device. */
#define HAVE_HW_HAMEG_HMO 1

/* Whether to support Hantek 4032L device. */
#define HAVE_HW_HANTEK_4032L 1

/* Whether to support Hantek 6xxx device. */
#define HAVE_HW_HANTEK_6XXX 1

/* Whether to support Hantek DSO device. */
#define HAVE_HW_HANTEK_DSO 1

/* Whether to support HP 3457A device. */
#define HAVE_HW_HP_3457A 1

/* Whether to support HP 3478A device. */
/* #undef HAVE_HW_HP_3478A */

/* Whether to support hp-59306a device. */
#define HAVE_HW_HP_59306A 1

/* Whether to support Hung-Chang DSO-2100 device. */
/* #undef HAVE_HW_HUNG_CHANG_DSO_2100 */

/* Whether to support ICStation USBRelay device. */
/* #undef HAVE_HW_ICSTATION_USBRELAY */

/* Whether to support Ikalogic Scanalogic-2 device. */
#define HAVE_HW_IKALOGIC_SCANALOGIC2 1

/* Whether to support Ikalogic Scanaplus device. */
#define HAVE_HW_IKALOGIC_SCANAPLUS 1

/* Whether to support IPDBG LA device. */
#define HAVE_HW_IPDBG_LA 1

/* Whether to support ITECH IT8500 device. */
/* #undef HAVE_HW_ITECH_IT8500 */

/* Whether to support JUNTEK JDS6600 device. */
/* #undef HAVE_HW_JUNTEK_JDS6600 */

/* Whether to support Kecheng KC-330B device. */
#define HAVE_HW_KECHENG_KC_330B 1

/* Whether to support KERN scale device. */
/* #undef HAVE_HW_KERN_SCALE */

/* Whether to support Kingst LA2016 device. */
#define HAVE_HW_KINGST_LA2016 1

/* Whether to support Korad KAxxxxP device. */
/* #undef HAVE_HW_KORAD_KAXXXXP */

/* Whether to support Lascar EL-USB device. */
#define HAVE_HW_LASCAR_EL_USB 1

/* Whether to support LeCroy LogicStudio device. */
#define HAVE_HW_LECROY_LOGICSTUDIO 1

/* Whether to support LeCroy X-Stream device. */
#define HAVE_HW_LECROY_XSTREAM 1

/* Whether to support Link Instruments MSO-19 device. */
/* #undef HAVE_HW_LINK_MSO19 */

/* Whether to support Manson HCS-3xxx device. */
/* #undef HAVE_HW_MANSON_HCS_3XXX */

/* Whether to support Mastech MS6514 device. */
/* #undef HAVE_HW_MASTECH_MS6514 */

/* Whether to support maynuo-m97 device. */
#define HAVE_HW_MAYNUO_M97 1

/* Whether to support Microchip PICkit2 device. */
#define HAVE_HW_MICROCHIP_PICKIT2 1

/* Whether to support MIC 985xx device. */
/* #undef HAVE_HW_MIC_985XX */

/* Whether to support Mooshimeter DMM device. */
/* #undef HAVE_HW_MOOSHIMETER_DMM */

/* Whether to support Motech LPS 30x device. */
/* #undef HAVE_HW_MOTECH_LPS_30X */

/* Whether to support Norma DMM device. */
/* #undef HAVE_HW_NORMA_DMM */

/* Whether to support OpenBench Logic Sniffer device. */
/* #undef HAVE_HW_OPENBENCH_LOGIC_SNIFFER */

/* Whether to support PCE PCE-322A device. */
/* #undef HAVE_HW_PCE_322A */

/* Whether to support Pipistrello-OLS device. */
#define HAVE_HW_PIPISTRELLO_OLS 1

/* Whether to support RaspberryPI PICO device. */
/* #undef HAVE_HW_RASPBERRYPI_PICO */

/* Whether to support RDTech DPSxxxx/DPHxxxx device. */
/* #undef HAVE_HW_RDTECH_DPS */

/* Whether to support RDTech TCXX device. */
/* #undef HAVE_HW_RDTECH_TC */

/* Whether to support RDTech UMXX device. */
/* #undef HAVE_HW_RDTECH_UM */

/* Whether to support Rigol DG device. */
#define HAVE_HW_RIGOL_DG 1

/* Whether to support Rigol DS device. */
#define HAVE_HW_RIGOL_DS 1

/* Whether to support Rohde&Schwarz SME-0x device. */
/* #undef HAVE_HW_ROHDE_SCHWARZ_SME_0X */

/* Whether to support Saleae Logic16 device. */
#define HAVE_HW_SALEAE_LOGIC16 1

/* Whether to support Saleae Logic Pro device. */
#define HAVE_HW_SALEAE_LOGIC_PRO 1

/* Whether to support SCPI DMM device. */
#define HAVE_HW_SCPI_DMM 1

/* Whether to support SCPI PPS device. */
#define HAVE_HW_SCPI_PPS 1

/* Whether to support serial DMM device. */
/* #undef HAVE_HW_SERIAL_DMM */

/* Whether to support serial LCR device. */
/* #undef HAVE_HW_SERIAL_LCR */

/* Whether to support SIGLENT SDL10x0 device. */
#define HAVE_HW_SIGLENT_SDL10X0 1

/* Whether to support Siglent SDS device. */
#define HAVE_HW_SIGLENT_SDS 1

/* Whether to support Sysclk LWLA device. */
#define HAVE_HW_SYSCLK_LWLA 1

/* Whether to support Sysclk SLA5032 device. */
#define HAVE_HW_SYSCLK_SLA5032 1

/* Whether to support Teleinfo device. */
/* #undef HAVE_HW_TELEINFO */

/* Whether to support Testo device. */
#define HAVE_HW_TESTO 1

/* Whether to support Tondaj SL-814 device. */
/* #undef HAVE_HW_TONDAJ_SL_814 */

/* Whether to support UNI-T DMM device. */
#define HAVE_HW_UNI_T_DMM 1

/* Whether to support UNI-T UT181A device. */
/* #undef HAVE_HW_UNI_T_UT181A */

/* Whether to support UNI-T UT32x device. */
/* #undef HAVE_HW_UNI_T_UT32X */

/* Whether to support Yokogawa DL/DLM device. */
#define HAVE_HW_YOKOGAWA_DLM 1

/* Whether to support ZEROPLUS Logic Cube device. */
#define HAVE_HW_ZEROPLUS_LOGIC_CUBE 1

/* Whether to support ZKETECH EBD-USB device. */
/* #undef HAVE_HW_ZKETECH_EBD_USB */

/* Is the STF input module supported? */
#define HAVE_INPUT_STF 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Whether libbluez is available. */
/* #undef HAVE_LIBBLUEZ */

/* Whether libftdi is available. */
#define HAVE_LIBFTDI 1

/* Whether libgio is available. */
#define HAVE_LIBGIO 1

/* Whether libgpib is available. */
/* #undef HAVE_LIBGPIB */

/* Whether libhidapi is available. */
/* #undef HAVE_LIBHIDAPI */

/* Whether libieee1284 is available. */
/* #undef HAVE_LIBIEEE1284 */

/* Whether libnettle is available. */
#define HAVE_LIBNETTLE 1

/* Whether librevisa is available. */
/* #undef HAVE_LIBREVISA */

/* Whether libserialport is available. */
/* #undef HAVE_LIBSERIALPORT */

/* Whether libusb is available. */
#define HAVE_LIBUSB_1_0 1

/* Define to 1 if the system has the type `libusb_os_handle'. */
/* #undef HAVE_LIBUSB_OS_HANDLE */

/* Define to 1 if you have the `memcmp' function. */
#define HAVE_MEMCMP 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Specifies whether we have the poll(2) function. */
#define HAVE_POLL 1

/* Specifies whether we have RPC support (either by SunRPC or libtirpc). */
#define HAVE_RPC 0

/* Specifies whether we have the select(2) function. */
#define HAVE_SELECT 1

/* Specifies whether serial communication is supported. */
#define HAVE_SERIAL_COMM 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Specifies whether we have the stoi and stod functions. */
#define HAVE_STOI_STOD 1

/* Specifies whether we have the std::stoul function. */
#define HAVE_STOUL 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timerfd.h> header file. */
#define HAVE_SYS_TIMERFD_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `zip_discard' function. */
#define HAVE_ZIP_DISCARD 1

/* Whether zlib is available. */
#define HAVE_ZLIB 1

/* Define to 1 if the system has the type `__int128_t'. */
#define HAVE___INT128_T 1

/* Define to 1 if the system has the type `__uint128_t'. */
#define HAVE___UINT128_T 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "sigrok-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libsigrok"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libsigrok 0.6.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libsigrok"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.sigrok.org"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.6.0"

/* Whether last argument to pyg_flags_get_value() is signed. */
/* #undef PYGOBJECT_FLAGS_SIGNED */

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `ptrdiff_t', as computed by sizeof. */
#define SIZEOF_PTRDIFF_T 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 8

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* The size of `__int64', as computed by sizeof. */
#define SIZEOF___INT64 0

/* Binary age of libsigrok. */
#define SR_LIB_VERSION_AGE 0

/* Binary version of libsigrok. */
#define SR_LIB_VERSION_CURRENT 4

/* Binary revision of libsigrok. */
#define SR_LIB_VERSION_REVISION 0

/* Binary version triple of libsigrok. */
#define SR_LIB_VERSION_STRING "4:0:0"

/* Major version number of libsigrok. */
#define SR_PACKAGE_VERSION_MAJOR 0

/* Micro version number of libsigrok. */
#define SR_PACKAGE_VERSION_MICRO 0

/* Minor version number of libsigrok. */
#define SR_PACKAGE_VERSION_MINOR 6

/* Version of libsigrok. */
#define SR_PACKAGE_VERSION_STRING_PREFIX "0.6.0-git"

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
#if defined __BIG_ENDIAN__
#define WORDS_BIGENDIAN 1
#endif
#else
#ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
#endif
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* The targeted POSIX standard. */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef ptrdiff_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
