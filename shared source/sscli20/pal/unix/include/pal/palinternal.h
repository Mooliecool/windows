/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    palinternal.h

Abstract:

    Rotor Platform Adaptation Layer (PAL) header file used by source
    file part of the PAL implementation. This is a wrapper over 
    rotor/pal/rotor_pal.h. It allows avoiding name collisions when including 
    system header files, and it allows redirecting calls to 'standard' functions
    to their PAL counterpart

Details :

A] Rationale (see B] for the quick recipe)
There are 2 types of namespace collisions that must be handled.

1) standard functions declared in rotor_pal.h, which do not need to be 
   implemented in the PAL because the system's implementation is sufficient.

   (examples : memcpy, strlen, fclose)

   The problem with these is that a prototype for them is provided both in 
   rotor_pal.h and in a system header (stdio.h, etc). If a PAL file needs to 
   include the files containing both prototypes, the compiler may complain 
   about the multiple declarations.

   To avoid this, the inclusion of rotor_pal.h must be wrapped in a 
   #define/#undef pair, which will effectiveily "hide" the rotor_pal.h 
   declaration by renaming it to something else. this is done by palinternal.h
   in this way :

   #define some_function DUMMY_some_function
   #include <rotor_pal.h>
   #undef some_function

   when a PAL source file includes palinternal.h, it will see a prototype for 
   DUMMY_some_function instead of some_function; so when it includes the 
   system header with the "real" prototype, no collision occurs.

   (note : technically, no functions should ever be treated this way, all 
   system functions should be wrapped according to method 2, so that call 
   logging through ENTRY macros is done for all functions n the PAL. However 
   this reason alone is not currently considered enough to warrant a wrapper)

2) standard functions which must be reimplemented by the PAL, because the 
   system's implementation does not offer suitable functionnality.
   
   (examples : widestring functions, networking)
   
   Here, the problem is more complex. The PAL must provide functions with the 
   same name as system functions. Due to the nature of Unix dynamic linking, 
   if this is done, the PAL's implementation will effectively mask the "real" 
   function, so that all calls are directed to it. This makes it impossible for
   a function to be implemented as calling its counterpart in the system, plus 
   some extra work, because instead of calling the system's implementation, the
   function would only call itself in an infinitely recursing nightmare. Even 
   worse, if by bad luck the system libraries attempt to call the function for 
   which the PAL provides an implementation, it is the PAL's version that will 
   be called.
   It is therefore necessary to give the PAL's implementation of such functions
   a different name. However, PAL consumers (applications built on top of the 
   PAL) must be able to call the function by its 'official' name, not the PAL's 
   internal name. 
   This can be done with some more macro magic, by #defining the official name 
   to the internal name *in rotor_pal.h*. :

   #define some_function PAL_some_function

   This way, while PAL consumer code can use the official name, it is the 
   internal name that wil be seen at compile time.
   However, one extra step is needed. While PAL consumers must use the PAL's 
   implementation of these functions, the PAL itself must still have access to
   the "real" functions. This is done by #undefining in palinternal.h the names
   #defined in rotor_pal.h :

   #include <rotor_pal.h>
   #undef some_function.

   At this point, code in the PAL implementation can access *both* its own 
   implementation of the function (with PAL_some_function) *and* the system's 
   implementation (with some_function)

    [side note : for the Win32 PAL, this can be accomplished without touching 
    rotor_pal.h. In Windows, symbols in in dynamic libraries are resolved at 
    compile time. if an application that uses some_function is only linked to 
    rotor_pal.dll, some_function will be resolved to the version in that DLL, 
    even if other DLLs in the system provide other implementations. In addition,
    the function in the DLL can actually have a different name (e.g. 
    PAL_some_function), to which the 'official' name is aliased when the DLL 
    is compiled. All this is not possible with Unix dynamic linking, where 
    symbols are resolved at run-time in a first-found-first-used order. A 
    module may end up using the symbols from a module it was never linked with,
    simply because that module was located somewhere in the dependency chain. ]

    It should be mentionned that even if a function name is not documented as 
    being implemented in the system, it can still cause problems if it exists. 
    This is especially a problem for functions in the "reserved" namespace 
    (names starting with an underscore : _exit, etc). (We shouldn't really be 
    implementing functions with such a name, but we don't really have a choice)
    If such a case is detected, it should be wrapped according to method 2

    Note that for all this to work, it is important for the PAL's implementation
    files to #include palinternal.h *before* any system files, and to never 
    include rotor_pal.h directly.

B] Procedure for name conflict resolution :

When adding a function to rotor_pal.h, which is implemented by the system and 
which does not need a different implementation :

- add a #define function_name DUMMY_function_name to palinternal.h, after all 
  the other DUMMY_ #defines (above the #include <rotor_pal.h> line)
- add the function's prototype to rotor_pal.h (if that isn't already done)
- add a #undef function_name to palinternal.h near all the other #undefs 
  (after the #include <rotor_pal.h> line)
  
When overriding a system function with the PAL's own implementation :

- add a #define function_name PAL_function_name to rotor_pal.h, somewhere 
  before the function's prototype, inside a #ifndef _MSCVER/#endif pair 
  (to avoid affecting the Win32 build)
- add a #undef function_name to palinternal.h near all the other #undefs 
  (after the #include <rotor_pal.h> line)
- implement the function in the pal, naming it PAL_function_name
- within the PAL, call PAL_function_name() to call the PAL's implementation, 
function_name() to call the system's implementation
--*/

#ifndef _PAL_INTERNAL_H_
#define _PAL_INTERNAL_H_

#define PAL_IMPLEMENTATION

/* Include our configuration information so it's always present when
   compiling PAL implementation files. */
#include "config.h"


/* C runtime functions needed to be renamed to avoid duplicate definition
   of those functions when including standard C header files */
#define memcpy DUMMY_memcpy 
#define memcmp DUMMY_memcmp 
#define memset DUMMY_memset 
#define memmove DUMMY_memmove 
#define memchr DUMMY_memchr
#define strlen DUMMY_strlen
#define stricmp DUMMY_stricmp 
#define strstr DUMMY_strstr 
#define strcmp DUMMY_strcmp 
#define strcat DUMMY_strcat
#define strncat DUMMY_strncat
#define strcpy DUMMY_strcpy
#define strcspn DUMMY_strcspn
#define strncmp DUMMY_strncmp
#define strncpy DUMMY_strncpy
#define strchr DUMMY_strchr
#define strrchr DUMMY_strrchr 
#define strpbrk DUMMY_strpbrk
#define strtod DUMMY_strtod
#define strspn DUMMY_strspn
#define _snprintf DUMMY__snprintf
#if HAVE__SNWPRINTF
#define _snwprintf DUMMY__snwprintf
#endif  /* HAVE__SNWPRINTF */
#define _vsnprintf DUMMY__vsnprintf
#define _vsnwprintf DUMMY__vsnwprintf
#define tolower DUMMY_tolower
#define toupper DUMMY_toupper
#define islower DUMMY_islower
#define isupper DUMMY_isupper
#define atoi DUMMY_atoi
#define atof DUMMY_atof
#define malloc DUMMY_malloc 
#define free DUMMY_free 
#define time DUMMY_time
#define tm PAL_tm
#define size_t DUMMY_size_t
#define time_t PAL_time_t
#define va_list DUMMY_va_list
#define abs DUMMY_abs

/* RAND_MAX needed to be renamed to avoid duplicate definition when including 
   stdlib.h header files. PAL_RAND_MAX should have the same value as RAND_MAX 
   defined in rotor_pal.h  */
#define PAL_RAND_MAX 0x7fff

/* The standard headers define isspace and isxdigit as macros and functions,
   To avoid redefinition problems, undefine those macros. */
#ifdef isspace
#undef isspace
#endif
#ifdef isxdigit
#undef isxdigit
#endif
#ifdef isalpha
#undef isalpha
#endif
#ifdef isalnum
#undef isalnum
#endif
#define isspace DUMMY_isspace 
#define isxdigit DUMMY_isxdigit
#define isalpha DUMMY_isalpha
#define isalnum DUMMY_isalnum

#ifdef stdin
#undef stdin
#endif
#ifdef stdout
#undef stdout
#endif
#ifdef stderr
#undef stderr
#endif

#ifdef SCHAR_MIN
#undef SCHAR_MIN
#endif
#ifdef SCHAR_MAX
#undef SCHAR_MAX
#endif
#ifdef SHRT_MIN
#undef SHRT_MIN
#endif
#ifdef SHRT_MAX
#undef SHRT_MAX
#endif
#ifdef INT_MIN
#undef INT_MIN
#endif
#ifdef INT_MAX
#undef INT_MAX
#endif
#ifdef UCHAR_MAX
#undef UCHAR_MAX
#endif
#ifdef USHRT_MAX
#undef USHRT_MAX
#endif
#ifdef ULONG_MAX
#undef ULONG_MAX
#endif
#ifdef UINT_MAX
#undef UINT_MAX
#endif
#ifdef LONG_MIN
#undef LONG_MIN
#endif
#ifdef LONG_MAX
#undef LONG_MAX
#endif
#ifdef RAND_MAX
#undef RAND_MAX
#endif
#ifdef DBL_MAX
#undef DBL_MAX
#endif
#ifdef FLT_MAX
#undef FLT_MAX
#endif
#ifdef __record_type_class
#undef __record_type_class
#endif
#ifdef __real_type_class
#undef __real_type_class
#endif

// The standard headers define va_start and va_end as macros,
// To avoid redefinition problems, undefine those macros.
#ifdef va_start
#undef va_start
#endif
#ifdef va_end
#undef va_end
#endif


/* The standard headers defines FD_SETSIZE
   To avoid redefinition problem, undefine it 
   Source code that requires FD_SETSIZE value defines in standard header
   will have to include palinternal.h before standard headers. Source code that
   requires the FD_SETSIZE value from rotor_pal.h will have to calculate it
   with sizeof(fd_array)/sizeof(PAL_SOCKET) */
#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif

/* Those types are defined differently in Rotor than in C header files.
   They have been redefined to give the chance to the implementation 
   code to work with both version of those types. */
#define u_char PAL_u_char
#define u_short PAL_u_short
#define u_int PAL_u_int
#define u_long PAL_u_long

#define wchar_t wchar_16

#define ptrdiff_t PAL_ptrdiff_t
#define intptr_t PAL_intptr_t
#define uintptr_t PAL_uintptr_t
#define SOCKET PAL_SOCKET
#define fd_set PAL_fd_set
#define hostent PAL_hostent
#define in_addr PAL_in_addr
#define ip_mreq PAL_ip_mreq
#define linger PAL_linger
#define sockaddr PAL_sockaddr
#define sockaddr_in PAL_sockaddr_in
#define timeval PAL_timeval
#define FILE PAL_FILE
#define fpos_t PAL_fpos_t

#include "rotor_pal.h"


// Store these values so we can use the values defined in rotor_pal.h and
// the native values (if they exist) side by side inside the PAL.
enum {
    PAL_IOC_VOID = IOC_VOID,
    PAL_IOC_IN = IOC_IN,
    PAL_IOC_OUT = IOC_OUT
};

enum {
    PAL_FIONREAD = FIONREAD,
    PAL_FIONBIO = FIONBIO
};

enum {
    PAL_SOL_SOCKET = SOL_SOCKET,
    PAL_IPPROTO_IP = IPPROTO_IP,
    PAL_IPPROTO_ICMP = IPPROTO_ICMP,
    PAL_IPPROTO_IGMP = IPPROTO_IGMP,
    PAL_IPPROTO_GGP = IPPROTO_GGP,
    PAL_IPPROTO_TCP = IPPROTO_TCP,
    PAL_IPPROTO_PUP = IPPROTO_PUP,
    PAL_IPPROTO_UDP = IPPROTO_UDP,
    PAL_IPPROTO_IDP = IPPROTO_IDP,
    PAL_IPPROTO_ND = IPPROTO_ND,
    PAL_IPPROTO_RAW = IPPROTO_RAW,
    PAL_NSPROTO_IPX = NSPROTO_IPX,
    PAL_NSPROTO_SPX = NSPROTO_SPX,
    PAL_NSPROTO_SPXII = NSPROTO_SPXII,
    
    PAL_INADDR_ANY = INADDR_ANY,
    PAL_INADDR_BROADCAST = INADDR_BROADCAST,
    PAL_INADDR_NONE = INADDR_NONE,
};

// I split the above enum because visual age compiler gives the following error:
//     An enumeration must not contain both a negative value and an unsigned value greater than LONG_MAX.
// #define INADDR_BROADCAST        (u_long)0xffffffff
// #define SO_LINGER       0x0080
// #define SO_DONTLINGER   (int)(~SO_LINGER)

enum {
    PAL_SOCK_STREAM = SOCK_STREAM,
    PAL_SOCK_DGRAM = SOCK_DGRAM,
    PAL_SOCK_RAW = SOCK_RAW,
    PAL_SOCK_RDM = SOCK_RDM,
    PAL_SOCK_SEQPACKET = SOCK_SEQPACKET,

    PAL_SO_DEBUG = SO_DEBUG,
    PAL_SO_ACCEPTCONN = SO_ACCEPTCONN,
    PAL_SO_REUSEADDR = SO_REUSEADDR,
    PAL_SO_KEEPALIVE = SO_KEEPALIVE,
    PAL_SO_DONTROUTE = SO_DONTROUTE,
    PAL_SO_BROADCAST = SO_BROADCAST,
    PAL_SO_USELOOPBACK = SO_USELOOPBACK,
    PAL_SO_LINGER = SO_LINGER,
    PAL_SO_OOBINLINE = SO_OOBINLINE,
    PAL_SO_DONTLINGER = SO_DONTLINGER,
    PAL_SO_EXCLUSIVEADDRUSE = SO_EXCLUSIVEADDRUSE,
    PAL_SO_SNDBUF = SO_SNDBUF,
    PAL_SO_RCVBUF = SO_RCVBUF,
    PAL_SO_SNDLOWAT = SO_SNDLOWAT,
    PAL_SO_RCVLOWAT = SO_RCVLOWAT,
    PAL_SO_SNDTIMEO = SO_SNDTIMEO,
    PAL_SO_RCVTIMEO = SO_RCVTIMEO,
    PAL_SO_ERROR = SO_ERROR,
    PAL_SO_TYPE = SO_TYPE,
    PAL_SO_CONDITIONAL_ACCEPT = SO_CONDITIONAL_ACCEPT,
    PAL_SO_MAX_MSG_SIZE = SO_MAX_MSG_SIZE,
    PAL_SO_PROTOCOL_INFO = SO_PROTOCOL_INFO,

    PAL_AF_UNSPEC = AF_UNSPEC,
    PAL_AF_UNIX = AF_UNIX,
    PAL_AF_INET = AF_INET,
    PAL_AF_IPX = AF_IPX,
    PAL_AF_OSI = AF_OSI,
    PAL_AF_SNA = AF_SNA,
    PAL_AF_DECnet = AF_DECnet,
    PAL_AF_APPLETALK = AF_APPLETALK,
    PAL_AF_NETBIOS = AF_NETBIOS,
    PAL_AF_ATM = AF_ATM,
    PAL_AF_INET6 = AF_INET6,
    PAL_AF_IRDA = AF_IRDA,

    PAL_IP_OPTIONS = IP_OPTIONS,
    PAL_IP_HDRINCL = IP_HDRINCL,
    PAL_IP_TOS = IP_TOS,
    PAL_IP_TTL = IP_TTL,
    PAL_IP_MULTICAST_IF = IP_MULTICAST_IF,
    PAL_IP_MULTICAST_TTL = IP_MULTICAST_TTL,
    PAL_IP_MULTICAST_LOOP = IP_MULTICAST_LOOP,
    PAL_IP_ADD_MEMBERSHIP = IP_ADD_MEMBERSHIP,
    PAL_IP_DROP_MEMBERSHIP = IP_DROP_MEMBERSHIP,
    PAL_IP_DONTFRAGMENT = IP_DONTFRAGMENT,
    PAL_IP_ADD_SOURCE_MEMBERSHIP = IP_ADD_SOURCE_MEMBERSHIP,
    PAL_IP_DROP_SOURCE_MEMBERSHIP = IP_DROP_SOURCE_MEMBERSHIP,
    PAL_IP_BLOCK_SOURCE = IP_BLOCK_SOURCE,
    PAL_IP_UNBLOCK_SOURCE = IP_UNBLOCK_SOURCE,
    PAL_IP_PKTINFO = IP_PKTINFO,
    
    PAL_TCP_NODELAY = TCP_NODELAY,
    PAL_TCP_BSDURGENT = TCP_BSDURGENT,
    
    PAL_UDP_NOCHECKSUM = UDP_NOCHECKSUM,
    PAL_UDP_CHECKSUM_COVERAGE = UDP_CHECKSUM_COVERAGE,

    PAL_MSG_OOB = MSG_OOB,
    PAL_MSG_PEEK = MSG_PEEK,
    PAL_MSG_DONTROUTE = MSG_DONTROUTE,
    PAL_MSG_PARTIAL = MSG_PARTIAL,
};

enum {
    PAL_LC_ALL      = LC_ALL,
    PAL_LC_COLLATE  = LC_COLLATE,
    PAL_LC_CTYPE    = LC_CTYPE,
    PAL_LC_MONETARY = LC_MONETARY,
    PAL_LC_NUMERIC  = LC_NUMERIC,
    PAL_LC_TIME     = LC_TIME,
};

#undef IOC_VOID
#undef IOC_IN
#undef IOC_OUT
#undef FIONREAD
#undef FIONBIO

#undef _IO
#undef _IOR
#undef _IOW
#undef _WSAIO
#undef _WSAIOR
#undef _WSAIOW
#undef _WSAIORW

#define _IO(x,y)         (PAL_IOC_VOID|((x)<<8)|(y))
#define _IOR(x,y,t)      (PAL_IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define _IOW(x,y,t)      (PAL_IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _WSAIO(x,y)                   (PAL_IOC_VOID|(x)|(y))
#define _WSAIOR(x,y)                  (PAL_IOC_OUT|(x)|(y))
#define _WSAIOW(x,y)                  (PAL_IOC_IN|(x)|(y))
#define _WSAIORW(x,y)                 (PAL_IOC_INOUT|(x)|(y))

#undef SOL_SOCKET
#undef IPPROTO_IP
#undef IPPROTO_ICMP
#undef IPPROTO_IGMP
#undef IPPROTO_GGP
#undef IPPROTO_TCP
#undef IPPROTO_PUP
#undef IPPROTO_UDP
#undef IPPROTO_IDP
#undef IPPROTO_ND
#undef IPPROTO_RAW
#undef NSPROTO_IPX
#undef NSPROTO_SPX
#undef NSPROTO_SPXII

#undef INADDR_ANY
#undef INADDR_BROADCAST
#undef INADDR_NONE

#undef SOCK_STREAM
#undef SOCK_DGRAM
#undef SOCK_RAW
#undef SOCK_RDM
#undef SOCK_SEQPACKET

#undef SO_DEBUG
#undef SO_ACCEPTCONN
#undef SO_REUSEADDR
#undef SO_KEEPALIVE
#undef SO_DONTROUTE
#undef SO_BROADCAST
#undef SO_USELOOPBACK
#undef SO_LINGER
#undef SO_OOBINLINE
#undef SO_DONTLINGER
#undef SO_EXCLUSIVEADDRUSE
#undef SO_SNDBUF
#undef SO_RCVBUF
#undef SO_SNDLOWAT
#undef SO_RCVLOWAT
#undef SO_SNDTIMEO
#undef SO_RCVTIMEO
#undef SO_ERROR
#undef SO_TYPE

#undef AF_UNSPEC
#undef AF_UNIX
#undef AF_INET
#undef AF_IPX
#undef AF_OSI
#undef AF_SNA
#undef AF_DECnet
#undef AF_APPLETALK
#undef AF_NETBIOS
#undef AF_ATM
#undef AF_INET6
#undef AF_IRDA

#undef IP_OPTIONS
#undef IP_HDRINCL
#undef IP_TOS
#undef IP_TTL
#undef IP_MULTICAST_IF
#undef IP_MULTICAST_TTL
#undef IP_MULTICAST_LOOP
#undef IP_ADD_MEMBERSHIP
#undef IP_DROP_MEMBERSHIP
#undef IP_DONTFRAGMENT
#undef IP_ADD_SOURCE_MEMBERSHIP
#undef IP_DROP_SOURCE_MEMBERSHIP
#undef IP_BLOCK_SOURCE
#undef IP_UNBLOCK_SOURCE
#undef IP_PKTINFO

#undef MSG_OOB
#undef MSG_PEEK
#undef MSG_DONTROUTE
#undef MSG_PARTIAL

#undef LC_ALL
#undef LC_COLLATE
#undef LC_CTYPE
#undef LC_MONETARY
#undef LC_NUMERIC
#undef LC_TIME

#undef IPPROTO_IP
#undef IPPROTO_ICMP
#undef IPPROTO_IGMP
#undef IPPROTO_GGP
#undef IPPROTO_TCP
#undef IPPROTO_PUP
#undef IPPROTO_UDP
#undef IPPROTO_IDP
#undef IPPROTO_ND
#undef IPPROTO_RAW
#undef NSPROTO_IPX
#undef NSPROTO_SPX
#undef NSPROTO_SPXII

/* rotor_pal.h does "#define alloca _alloca", but we need access to the "real"
   alloca */
#undef alloca

/* Undef all functions and types previously defined so those functions and
   types could be mapped to the C runtime and socket implementation of the 
   native OS */
#undef exit
#undef alloca
#undef atexit
#undef setlocale
#undef memcpy
#undef memcmp
#undef memset
#undef memmove
#undef memchr
#undef strlen
#undef stricmp
#undef strstr
#undef strcmp
#undef strcat
#undef strcspn
#undef strncat
#undef strcpy
#undef strncmp
#undef strncpy
#undef strchr
#undef strrchr
#undef strpbrk
#undef strtoul
#undef strtod
#undef strspn
#undef strtok
#undef tolower
#undef toupper
#undef islower
#undef isupper
#undef isspace
#undef iswdigit
#undef iswxdigit
#undef iswalpha
#undef iswprint
#undef isxdigit
#undef isalpha
#undef isalnum
#undef atoi
#undef atol
#undef atof
#undef malloc
#undef realloc
#undef free
#undef qsort
#undef bsearch
#undef time
#undef tm
#undef localtime
#undef mktime
#undef FILE
#undef fclose
#undef setbuf
#undef fopen
#undef fread
#undef feof
#undef ferror
#undef ftell
#undef fflush
#undef fwrite
#undef fgets
#undef fgetws
#undef fputc
#undef putchar
#undef fputs
#undef fseek
#undef fgetpos
#undef fsetpos
#undef getc
#undef fgetc
#undef ungetc
#undef size_t
#undef time_t
#undef va_list
#undef va_start
#undef va_end
#undef stdin
#undef stdout
#undef stderr
#undef abs
#undef labs
#undef acos
#undef asin
#undef atan2
#undef exp
#undef log
#undef log10
#undef pow
#undef rand
#undef errno
#undef getenv 
#undef wcsspn

/* by #undefing SOCKET, the INVALID_SOCKET macro becomes invalid. redefine it
   with PAL_SOCKET */
#undef INVALID_SOCKET
#define INVALID_SOCKET (PAL_SOCKET)(~0)
#undef wchar_t
#undef ptrdiff_t
#undef intptr_t
#undef uintptr_t
#undef SOCKET
#undef FD_SET
#undef FD_ZERO
#undef FD_SETSIZE
#undef SOMAXCONN
#undef fd_set
#undef hostent
#undef in_addr
#undef ip_mreq
#undef linger
#undef sockaddr
#undef sockaddr_in
#undef timeval
#undef u_char
#undef u_short
#undef u_int
#undef u_long
#undef fpos_t


#undef printf
#undef fprintf
#undef fwprintf
#undef vfprintf
#undef vfwprintf
#undef vprintf
#undef wprintf
#undef sprintf
#undef swprintf
#undef _snprintf
#if HAVE__SNWPRINTF
#undef _snwprintf
#endif  /* HAVE__SNWPRINTF */
#undef sscanf
#undef wcstod
#undef wcstol
#undef wcstoul
#undef wcscat
#undef wcscpy
#undef wcslen
#undef wcsncmp
#undef wcschr
#undef wcsrchr
#undef wsprintf
#undef swscanf
#undef wcspbrk
#undef wcsstr
#undef wcscmp
#undef wcsncat
#undef wcsncpy
#undef wcstok
#undef wcscspn
#undef iswupper
#undef iswspace
#undef towlower
#undef towupper
#undef vsprintf
#undef vswprintf
#undef _vsnprintf
#undef _vsnwprintf

#undef accept
#undef bind
#undef connect
#undef ctime
#undef getpeername
#undef getsockname
#undef inet_addr
#undef getsockopt
#undef setsockopt
#undef gethostbyname
#undef gethostbyaddr
#undef gethostname
#undef listen
#undef recv
#undef recvfrom
#undef send
#undef sendto
#undef shutdown
#undef select
#undef socket

#undef INADDR_ANY
#undef INADDR_BROADCAST

#undef SCHAR_MIN
#undef SCHAR_MAX
#undef UCHAR_MAX
#undef SHRT_MIN
#undef SHRT_MAX
#undef USHRT_MAX
#undef INT_MIN
#undef INT_MAX
#undef UINT_MAX
#undef LONG_MIN
#undef LONG_MAX
#undef ULONG_MAX
#undef RAND_MAX
#undef DBL_MAX
#undef FLT_MAX
#undef __record_type_class
#undef __real_type_class

#if HAVE_CHAR_BIT
#undef CHAR_BIT
#endif

#undef IOCPARM_MASK
#undef _IO
#undef _IOR
#undef _IOW
#undef FIONREAD
#undef FIONBIO
#undef FIOASYNC

#define FD_SETSIZE 16384

// We need a sigsetjmp prototype in rotor_pal.h for the SEH macros, but we
// can't use the "real" prototype (because we don't want to define sigjmp_buf).
// So we must rename the "real" sigsetjmp to avoid redefinition errors.
#define sigsetjmp REAL_sigsetjmp
#define siglongjmp REAL_siglongjmp
#include <setjmp.h>
#undef sigsetjmp
#undef siglongjmp

#undef _SIZE_T_DEFINED
#undef _WCHAR_T_DEFINED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <pwd.h>


/* we don't really need this header here, but by including it we make sure
   we'll catch any definition conflicts */
#include <sys/socket.h>

#if !HAVE_INFTIM
#define INFTIM  -1
#endif // !HAVE_INFTIM

#if HAVE_BROKEN_REALPATH
#define realpath(a, b) FILErealpath((a), (b))
char * FILErealpath(const char *path, char resolved[]);
#endif

#define OffsetOf(s, f) (INT)(SIZE_T)&(((s*)0)->f)

#endif /* _PAL_INTERNAL_H_ */
