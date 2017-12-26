/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    socket.c

Abstract:

    Implementation of the socket API

Notes:

Unlike in the Win32 implementation, the SOCKET (PAL_SOCKET) type is not
managed by the handle manager. This means that closesocket must be used
to close sockets -- using CloseHandle will produce unexpected behavior.
The SOCKET or PAL_SOCKET type is in fact just a file descriptor (int)
cast to an unsigned int. Also INVALID_SOCKET is (~0) on win32, and -1
on Unix. In twos complement notation these are the same values (0xffffffff).

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/socket2.h"
#include "pal/critsect.h"

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/param.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if !HAVE_IN_ADDR_T
#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#else   // !(HAVE_STDINT_H || HAVE_INTTYPES_H)
#error Missing in_addr_t and uint32_t. socket.c needs a 32-bit unsigned int type.
#endif
#endif  /* !HAVE_IN_ADDR_T */

SET_DEFAULT_DEBUG_CHANNEL(SOCKET);

static const char* LOCAL_HOST = "localhost";

static BOOL SOCKConvertNativeToPALHostent(const struct hostent *hostent,
                                          struct PAL_hostent *palHostent,
                                          const char *hostname);
static BOOL SOCKAllocRTAData(PAL_SOCKET s);

extern pal_socket_list SOCK_socket_list;
extern int SOCK_table_size;
extern CRITICAL_SECTION SOCK_list_crit_section; 

/*++
Function:
  PAL_getpeername

See MSDN doc.
--*/
int
PALAPI
PAL_getpeername (
        IN PAL_SOCKET s,
        OUT struct PAL_sockaddr FAR *name,
        IN OUT int FAR * namelen)
{
    int gd;
    struct sockaddr UnixSockaddr;

    PERF_ENTRY(getpeername);
    ENTRY("PAL_getpeername(s=%#x, name=%p, namelen=%p )\n",
          s, name, namelen );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_getpeername returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getpeername);
        return SOCKET_ERROR;
    }
    
    if ( IsBadWritePtr(namelen, sizeof(sizeof(int))) || 
         (*namelen < sizeof(struct sockaddr)))    
    {
        ERROR("Parameter namelen has an invalid value\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_getpeername returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getpeername);
        return SOCKET_ERROR;
    }
    
    if (IsBadWritePtr(name, sizeof(struct sockaddr))) 
    {
        ERROR("Parameter name is an invalid memory address\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_getpeername returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getpeername);
        return SOCKET_ERROR;
    }
        
    if ((gd = getpeername(s, &UnixSockaddr, (socklen_t*) namelen)) < 0 )
    {
        SetLastError(SOCKErrorReturn(errno));
    }

    /*Copy BSD structure to Windows structure*/
    name->sa_family = UnixSockaddr.sa_family;
    memcpy(name->sa_data, UnixSockaddr.sa_data, sizeof(name->sa_data));

    LOGEXIT("PAL_getpeername returns int %d\n", gd);
    PERF_EXIT(getpeername);
    return gd;
}

/*++
Function:
  PAL_gethostname

See MSDN doc.
--*/
int 
PALAPI 
PAL_gethostname (
        OUT char FAR * name,
        IN int namelen)
{
    int ret;
    char buffer[MAXHOSTNAMELEN + 1];
    int length;

    PERF_ENTRY(gethostname);
    ENTRY("PAL_gethostname(name=%p, namelen=%d )\n",
          name, namelen );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_gethostname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(gethostname);
        return SOCKET_ERROR;
    }
    
    if ( namelen == 0 )
    {
        ERROR("namelen was too small\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_gethostname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(gethostname);
        return SOCKET_ERROR;
    }
    
    if (IsBadWritePtr(name, namelen))
    {
        ERROR("Parameter name is an invalid memory address\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_gethostname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(gethostname);
        return SOCKET_ERROR;
    }
    
    // Some platforms return success and truncate the name when given a
    // buffer that's too small to hold the hostname. We need to return
    // failure in that case, but we can't detect whether the hostname
    // has been truncated without making a second (costly and
    // non-thread-safe) call to gethostname. Instead, we get the full
    // hostname and then check to see if it can fit in the given buffer.
    buffer[MAXHOSTNAMELEN] = '\0';
    if ( (ret = gethostname(buffer, MAXHOSTNAMELEN)) < 0 )
    {
        SetLastError(SOCKErrorReturn(errno));
    }
    
    length = strlen(buffer);
    if (length + 1 > namelen)
    {
        ERROR("Insufficient space to store the hostname\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_gethostname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(gethostname);
        return SOCKET_ERROR;
    }
    strcpy(name, buffer);
    
    LOGEXIT("PAL_gethostname returns int %d\n", ret);
    PERF_EXIT(gethostname);
    return ret;
}

LONG
PALAPI
PAL_inet_addr (IN const char FAR * cp)
{
    LONG ret;
    PERF_ENTRY(inet_addr);
    ENTRY("PAL_inet_addr(cp:%p (%s))\n", cp, cp);
    
    ret= (cp && cp[0]==' ') ? 0 : inet_addr(cp);

    LOGEXIT("PAL_inet_addr returning %08x\n", ret);
    PERF_EXIT(inet_addr);
    return ret;
}

/*++
Function:
  PAL_getsockopt

See MSDN doc.
--*/
int
PALAPI
PAL_getsockopt (
        IN PAL_SOCKET s,
        IN int level,
        IN int optname,
        OUT char FAR * optval,
        IN OUT int FAR *optlen)
{
    int gd = SOCKET_ERROR;
    int nativeLevel;
    int nativeOptname;
    int socktype;
    socklen_t sockoptlen;
    socklen_t socktypelen;
    struct linger FBSD_linger;
    struct PAL_linger *Win32_linger = NULL;
    BOOL OptIsDONTLINGER = FALSE;
    BOOL *DONTLINGERoptval = NULL;
    struct timeval FBSD_SndRcvO;
    int *Win32_SndRcvO=NULL;

    PERF_ENTRY(getsockopt);
    ENTRY("PAL_getsockopt(s=%#x, level=%d, optname=%d, optval=%p, "
          "optlen=%p )\n",s, level, optname, optval, optlen );

    socktypelen = sizeof(socktype);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockopt);
        return SOCKET_ERROR;
    }

    /* Test if we are getting a valid socket */
    if ((s >= SOCK_table_size) || (s < 0) || 
        (getsockopt(s, SOL_SOCKET, SO_TYPE, &socktype, &socktypelen) < 0))
    {
        ERROR("Invalid socket!\n");
        SetLastError(WSAENOTSOCK);
        LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockopt);
        return (SOCKET_ERROR);
    }

    /* Test if the socket level is valid */
    if (level != PAL_SOL_SOCKET && level != PAL_IPPROTO_TCP)
    {
        ERROR("Invalid socket level!\n");
        SetLastError(WSAEINVAL);
        LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockopt);
        return (SOCKET_ERROR);
    }

    // Change level to its native value before we call the native API.
    nativeLevel = SOCKGetNativeSocketLevel(level);
    // If the level isn't supported, fail.
    if (nativeLevel == -1)
    {
        WARN ("getsockopt called with unimplemented level=%d\n", level);
        SetLastError(WSAEINVAL);
        LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockopt);
        return SOCKET_ERROR;
    }

    // Sanity check the incoming optlen to make sure it's a valid part of
    // the address space.
    if ( IsBadWritePtr(optlen, sizeof(sizeof(int))) || (*optlen <= 0))
    {
        ERROR("invalid optlen\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockopt);
        return SOCKET_ERROR;
    }

    /* The buffer pointer should be a valid part of the user
       address space*/
    if (IsBadWritePtr(optval, *optlen))
    {
        ERROR("Buffer pointer is not totally contained in a valid"
               "part of the user address space.\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockopt);
        return SOCKET_ERROR;
    }

    switch (optname)
    {
    case PAL_SO_REUSEADDR:
        SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
        if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
        {
            *((int*)optval) = (SOCK_socket_list.sockets[s].rt_data->opts 
                               & PAL_SO_REUSEADDR) ? 1 : 0;
        }
        else
        {
            *((int*)optval) = 0;
        }
        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

        /* Default value for SO_REUSEADDR is 0 on any platform: let's bail out 
           here. 
        */
        gd = 0;
        goto done;

        break; 

    case PAL_SO_EXCLUSIVEADDRUSE:
        /*                                                       
        */

        SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
        if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
        {
            *((int*)optval) = (SOCK_socket_list.sockets[s].rt_data->special_opts 
                               & PAL_SOCK_SSO_EXCLUSIVEADDRUSE) ? 1 : 0;
        }
        else
        {
            *((int*)optval) = 0;
        }
        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

        gd = 0;
        goto done;
        break; 

    case PAL_SO_DONTLINGER:
        // We need to do some magic, since SO_DONTLINGER is
        // supported via a boolean parameter for SO_LINGER.

        // Be sure the size is correct.
        if (*optlen < sizeof (BOOL))
        {
            ERROR("invalid optlen\n");
            SetLastError(WSAEFAULT);
            gd = SOCKET_ERROR;
            goto done;
        }

        // Construct a SO_LINGER call, and translate the results
        // after the getsockopt() call.
        OptIsDONTLINGER = TRUE;
        DONTLINGERoptval = (BOOL*)optval;
        optname = PAL_SO_LINGER;
        optval = (char *)&FBSD_linger;
        *optlen = sizeof (struct linger);

    default:
        break;
    }

    // Change optname to its native value before we call the native API.
    nativeOptname = SOCKGetNativeSocketOption(level, optname);
    
    // If the option isn't supported, fail.
    if (nativeOptname == -1)
    {
        WARN ("getsockopt called with unimplemented optname=%d\n", optname);
        SetLastError(WSAENOPROTOOPT);
        LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockopt);
        return SOCKET_ERROR;
    }
    
    // We need special support for SO_SNDTIMEO and SO_RCVTIMEO, 
    // since in Windows it uses an int (in milliseconds) vs. 
    // FreeBSD which is a struct of seconds and microseconds.
    // First, hold onto our int ptr and put in our BSD struct.
    if (nativeOptname == SO_RCVTIMEO || nativeOptname == SO_SNDTIMEO)
    {
        Win32_SndRcvO = (int*)optval;
        optval = (char*)&FBSD_SndRcvO;
        *optlen = sizeof(struct timeval);
    }
    
    // We need a special case here since struct linger in Windows
    // is different from FreeBSD. So, lets save the pointer
    // and pass a 'struct linger' of our own to FreeBSD's getsockopt().
    if (nativeOptname == SO_LINGER)
    {
        /* Be sure the size is correct */
        if (*optlen < sizeof (struct PAL_linger))
        {
            ERROR("invalid optlen\n");
            SetLastError(WSAEFAULT);
            LOGEXIT("PAL_getsockopt returns int %d\n", SOCKET_ERROR);
            PERF_EXIT(getsockopt);
            return (SOCKET_ERROR);
        }

        Win32_linger = (struct PAL_linger*)optval;
        optval = (char *)&FBSD_linger;
        *optlen = sizeof (struct linger);
    }

    // In BSD, when getsockopt is failing, it is NOT returning -1
    // like is documented; instead, it is returning 0 like it had
    // succeeded, but setting the errno(!).  So, lets clear the
    // errno, and if it gets set, then we know there was an error.
#if SO_TIMEO_NOT_SUPPORTED
    if (nativeOptname != SO_RCVTIMEO && nativeOptname != SO_SNDTIMEO)
    {
#endif
        errno=0;
        sockoptlen = *optlen;
        if ((gd = getsockopt(s, nativeLevel, nativeOptname, optval, &sockoptlen)) < 0
            || errno == EBADF || errno == ENOTSOCK)
        {
            SetLastError(SOCKErrorReturn(errno));
            gd = SOCKET_ERROR;
            goto done;
        }
        if ((sockoptlen > INT_MAX) && (*optlen > 0))
        {
            ASSERT("sockfromlen should not be greater than INT_MAX.\n");
        }
        *optlen = (int)sockoptlen;
#if SO_TIMEO_NOT_SUPPORTED
    }
#endif

    switch (nativeLevel)
    {
        case SOL_SOCKET:
            switch (nativeOptname)
            {
                case SO_LINGER:
                    // We now need to put the data from BSD's structure into the
                    // pointer we received (Win32 linger structure).
                    if (OptIsDONTLINGER)
                    {
                        /* We drop in here, because we needed to do some magic
                           to handle SO_DONTLINGER */
            
                        /* DONTLINGER True if SO_LINGER said linger is turned off.
                           DONTLINGER False if SO_LINGER said linger is turned on.
                           That is why we have this opposite assignment */
                        *DONTLINGERoptval = FBSD_linger.l_onoff ? FALSE : TRUE;
                        *optlen = sizeof(BOOL);
                    }
                    else
                    {
                        Win32_linger->l_onoff = FBSD_linger.l_onoff;
                        Win32_linger->l_linger = FBSD_linger.l_linger;
                        *optlen = sizeof (struct PAL_linger);
                    }
                    break;
                case SO_RCVTIMEO:
                case SO_SNDTIMEO:
#if SO_TIMEO_NOT_SUPPORTED
                    *optlen = sizeof(int);
                    *Win32_SndRcvO = 0;
            
                    SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
                    if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
                    {
                        if (nativeOptname == SO_RCVTIMEO)
                            *Win32_SndRcvO = SOCK_socket_list.sockets[s].rt_data->rcv_tmo;
                        else
                            *Win32_SndRcvO = SOCK_socket_list.sockets[s].rt_data->snd_tmo;
                    }
                    SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

                    gd = 0;
                    break;
#else // SO_TIMEO_NOT_SUPPORTED
                    // Convert from BSD structure to Win32 int. Some loss of
                    // precision occurs due to the fact that Windows has
                    // millisecond resolution vs. BSD's microsecond
                    // resolution.
                    *Win32_SndRcvO = (FBSD_SndRcvO.tv_sec * 1000) +
                                     (FBSD_SndRcvO.tv_usec / 1000);
                    *optlen = sizeof(int);
                    break;
#endif // SO_TIMEO_NOT_SUPPORTED

                default:
                    break;
            }
            break;
        case IPPROTO_TCP:
            switch (nativeOptname)
            {
                case TCP_NODELAY:
                    /* Windows does FALSE or TRUE, BSD does 0 or non-zero.
                       Let's fix that, just in case someone from the Windows world
                       specifically depends on it */
                    *((BOOL*)optval) = *((BOOL*)optval) ? TRUE : FALSE;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

done:
    LOGEXIT("PAL_getsockopt returns int %d\n", gd);
    PERF_EXIT(getsockopt);
    return gd;
}


/*++
Function:
  PAL_setsockopt

See MSDN doc.
--*/
int
PALAPI
PAL_setsockopt (
        IN PAL_SOCKET s,
        IN int level,
        IN int optname,
        IN const char FAR *optval,
        IN int optlen)
{
    int gd;
    int nativeLevel;
    int nativeOptname;
    socklen_t sockoptlen;
    struct linger FBSD_linger; /* FreeBSD's linger structure is different
                                  from Windows */
    struct timeval FBSD_SndRcvO; /* FreeBSD uses struct timeval for SO_SNDTIMEO,
                                   SO_RCVTIMEO */
    union  {
        WORD word;
        } OptvalRollBack;

    PERF_ENTRY(setsockopt);
    ENTRY("PAL_setsockopt(s=%#x, level=%d, optname=%d, optval=%p, "
          "optlen=%d )\n",s, level, optname, optval, optlen );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        gd = SOCKET_ERROR;
        goto done;
    }

    if ((s < 0) || (s >= SOCK_table_size))
    {
        ERROR("Invalid socket\n");
        SetLastError(WSAENOTSOCK);
        gd = SOCKET_ERROR;
        goto done;
    }

    if (IsBadReadPtr(optval, optlen))
    {
        ERROR("optval is not totally contained in a valid part of the "
               "user address space.\n");
        SetLastError(WSAEFAULT);
        gd = SOCKET_ERROR;
        goto done;
    }

    // Change level to its native value before we call the native API.
    nativeLevel = SOCKGetNativeSocketLevel(level);
    // If the level isn't supported, fail.
    if (nativeLevel == -1)
    {
        WARN ("setsockopt called with unimplemented level=%d\n", level);
        SetLastError(WSAEINVAL);
        gd = SOCKET_ERROR;
        goto done;
    }

    switch (optname)
    {
        case PAL_SO_LINGER:
            // We need a special case here since struct linger in Windows
            // is different from Unix.
            FBSD_linger.l_onoff = ((struct PAL_linger*)optval)->l_onoff;
            FBSD_linger.l_linger = ((struct PAL_linger*)optval)->l_linger;
            optval = (const char *)&FBSD_linger;
            optlen = sizeof(struct linger);
            break;
        case PAL_SO_DONTLINGER:
            // On Windows setting SO_DONTLINGER is equivalent to setting
            // SO_LINGER with l_onoff set to zero.
            optname = PAL_SO_LINGER; 
            optval = (const char *)&FBSD_linger;
            optlen = sizeof(struct linger);
    
            // First, we need to retrieve the SO_LINGER socket option,
            // because we need to keep the l_linger time constant.
            // (ie. if we disable linger with SO_DONTLINGER to TRUE,
            // then re-enable it with SO_DONTLINGER to false,
            // we need to retain our previous l_linger value, as
            // Windows does)
            if ((gd = getsockopt(s, nativeLevel, SO_LINGER, (void *) optval,
                 (socklen_t*) &optlen)) < 0)
            {
                SetLastError(SOCKErrorReturn(errno));
                goto done;                
            }
        
            // If SO_DONTLINGER is TRUE, turn off linger (l_onoff is 0).
            // if SO_DONTLINGER is FALSE, turn on linger (l_onoff is 1).
            // Leave l_linger the same.
            FBSD_linger.l_onoff = *((BOOL*)optval) ? 0 : 1;
            break;
        case PAL_SO_RCVTIMEO:
        case PAL_SO_SNDTIMEO:
#if SO_TIMEO_NOT_SUPPORTED
            if (*((int*)optval) < 0)
            {
                ERROR("Negative timeout specified as SO_RCVTIMEO or SO_SNDTIMEO\n");
                SetLastError(WSAEINVAL); 
                gd = SOCKET_ERROR;
                goto done;
            }

            /* 
            SHRT_MAX * 10 ms is a documented maximum limit for timeouts on Mac OS X and an undocumented limit on FreeBSD.
            if (*((int*)optval) >= SHRT_MAX * 10)
            {
                ERROR("Timeout is greater than or equal to SHRT_MAX * 10 ms.\n");
                SetLastError(WSAEINVAL);
                gd = SOCKET_ERROR;
                goto done;
            } 
            */

            SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
            if (SOCK_socket_list.sockets)
            {
                if (SOCK_socket_list.sockets[s].rt_data == NULL)
                {
                    if (!SOCKAllocRTAData(s))
                    {
                        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
                        ERROR("Out of memory\n");
                        SetLastError(WSAENETDOWN); // closest setsockopt's error to out of mem
                        gd = SOCKET_ERROR;
                        goto done;
                    }
                }

                if (optname == PAL_SO_RCVTIMEO)
                {
                    SOCK_socket_list.sockets[s].rt_data->rcv_tmo = *((int*)optval);
                }
                else
                {
                    SOCK_socket_list.sockets[s].rt_data->snd_tmo = *((int*)optval);
                }
            }
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

            gd = 0;
            goto done;
#endif
            // Convert from a Win32 int to the BSD structure.
            FBSD_SndRcvO.tv_sec = *((int*)optval) / 1000;
            FBSD_SndRcvO.tv_usec = ((*(int*)optval) % 1000) * 1000;
            optval = (char*)&FBSD_SndRcvO;
            optlen = sizeof(struct timeval);
            break;
        case PAL_SO_REUSEADDR:
            /* We need to keep track of the value set for this options in a way 
               to return error in case of any attempt to set both 
               PAL_SO_REUSEADDR and SO_EXCLUSIVEADDRUSE at the same time. We 
               store the value rather than just setting it to (and getting it 
               from) the native socket for the following reasons:
               1) It is in general faster, it avoids us two system calls
            */

            /* Initialize roll back value */
            OptvalRollBack.word = 0;

            SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
            if (SOCK_socket_list.sockets)
            {
                if (SOCK_socket_list.sockets[s].rt_data == NULL)
                {
                    if (!SOCKAllocRTAData(s))
                    {
                        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
                        ERROR("Out of memory\n");
                        SetLastError(WSAENETDOWN); // closest setsockopt's error to out of mem
                        gd = SOCKET_ERROR;
                        goto done;
                    }
                }

                /* Save roll back value in case native setsockopt fails */
                OptvalRollBack.word = SOCK_socket_list.sockets[s].rt_data->opts;

                if (*((int*)optval))
                {
                    if (SOCK_socket_list.sockets[s].rt_data->special_opts & 
                        PAL_SOCK_SSO_EXCLUSIVEADDRUSE)
                    {
                        /* SO_EXCLUSIVEADDRUSE and PAL_SO_REUSEADDR cannot be both set 
                           at the same time */
                        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
                        ERROR("SO_EXCLUSIVEADDRUSE and PAL_SO_REUSEADDR cannot be both set\n");
                        SetLastError(WSAEINVAL); 
                        gd = SOCKET_ERROR;
                        goto done;
                    }

                    SOCK_socket_list.sockets[s].rt_data->opts |= PAL_SO_REUSEADDR;
                }
                else
                {
                    SOCK_socket_list.sockets[s].rt_data->opts &= ~PAL_SO_REUSEADDR;
                }
            }
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

            /* The actual native SO_REUSEADDR option will be set by the native setsockopt 
               call later */
            break;

        case PAL_SO_EXCLUSIVEADDRUSE:
            /* See note above for PAL_SO_REUSEADDR */
            /*                                                               
            */

            SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
            if (SOCK_socket_list.sockets)
            {
                if (SOCK_socket_list.sockets[s].rt_data == NULL)
                {
                    if (!SOCKAllocRTAData(s))
                    {
                        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
                        ERROR("Out of memory\n");
                        SetLastError(WSAENETDOWN); // closest setsockopt's error to out of mem
                        gd = SOCKET_ERROR;
                        goto done;
                    }
                }

                /* We store the value so getsockopt can return it, 
                   but currently we don't use it */
                if (*((int*)optval))
                {
                    if (SOCK_socket_list.sockets[s].rt_data->opts & PAL_SO_REUSEADDR)
                    {
                        /* SO_EXCLUSIVEADDRUSE and PAL_SO_REUSEADDR cannot be both set 
                           at the same time */
                        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
                        ERROR("SO_EXCLUSIVEADDRUSE and PAL_SO_REUSEADDR cannot be both set\n");
                        SetLastError(WSAEINVAL); 
                        gd = SOCKET_ERROR;
                        goto done;
                    }

                    SOCK_socket_list.sockets[s].rt_data->special_opts |=
                        PAL_SOCK_SSO_EXCLUSIVEADDRUSE;
                }
                else
                {
                    SOCK_socket_list.sockets[s].rt_data->special_opts &=
                        ~PAL_SOCK_SSO_EXCLUSIVEADDRUSE;
                }
            }
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

            /* There is no native support for SO_EXCLUSIVEADDRUSE, our job is 
               done here, so we bail out */
            gd = 0;
            goto done;

            break; 

        default:
            break;
    }

    // Change optname to its native value before we call the native API.
    nativeOptname = SOCKGetNativeSocketOption(level, optname);
    
    // If the option isn't supported, fail.
    if (nativeOptname == -1)
    {
        WARN ("setsockopt called with unimplemented optname=%d or level=%d\n",
              optname, level);
        SetLastError(WSAENOPROTOOPT);
        gd = SOCKET_ERROR;
    }
    else
    {
        sockoptlen = optlen;
        if ((gd = setsockopt(s, nativeLevel, nativeOptname, optval, sockoptlen)) < 0 )
        {
            SetLastError(SOCKErrorReturn(errno));
        }
    }

    if (SOCKET_ERROR == gd)
    {
        /* Roll back */
        switch (optname)
        {
        case PAL_SO_REUSEADDR:
            SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
            if (SOCK_socket_list.sockets)
            {
                if ((SOCK_socket_list.sockets[s].rt_data->special_opts & 
                     PAL_SOCK_SSO_EXCLUSIVEADDRUSE) && 
                    (OptvalRollBack.word & PAL_SO_REUSEADDR))
                {
                    /* SO_EXCLUSIVEADDRUSE changed while we were executing:
                       another thread is racing with us setting options on
                       this socket. This is an application error. Anyway we
                       can't rollback here, let's just fail.
                    */
                    SetLastError(WSAEINPROGRESS); /* not quite the correct 
                                                     error code, but as close as
                                                     it gets among setsockopt 
                                                     error codes */
                }
                else
                {
                    SOCK_socket_list.sockets[s].rt_data->opts = OptvalRollBack.word;
                }
            }
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            break;
        default:
            break;
        }
    }

done:
    LOGEXIT("PAL_setsockopt returns int %d\n", gd);
    PERF_EXIT(setsockopt);
    return gd;
}

/*++
Function:
  PAL_connect

See MSDN doc.
--*/
int
PALAPI
PAL_connect (
        IN PAL_SOCKET s,
        IN const struct PAL_sockaddr FAR *name,
        IN int namelen)
{
    int cd;
    PERF_ENTRY(connect);
    ENTRY("PAL_connect(s=%#x, name=%p, namelen=%d )\n",
          s, name, namelen );
    
    cd = WSAConnect (s, name, namelen, NULL, NULL, NULL, NULL);

    LOGEXIT("PAL_connect returns int %d\n", cd);
    PERF_EXIT(connect);
    return cd;
}

/*++
Function:
  PAL_send

See MSDN doc.
--*/
int
PALAPI
PAL_send (
        IN PAL_SOCKET s,
        IN const char FAR * buf,
        IN int len,
        IN int flags)
{
    WSABUF ws_buff;
    DWORD dwBytesSent;
    int ret;
    
    
    PERF_ENTRY(send);
    ENTRY("PAL_send(s=%#x, buf=%p, len=%d, flags=%#x )\n",
          s, buf, len, flags );


    ws_buff.len = (u_long) len;
    ws_buff.buf = (char*) buf;

    dwBytesSent = 0;
    ret = WSASend(
            s, &ws_buff, 1, &dwBytesSent, (DWORD) flags,
            (LPWSAOVERLAPPED) NULL, 
            (LPWSAOVERLAPPED_COMPLETION_ROUTINE) NULL);

    if (SOCKET_ERROR != ret)
    {
        ret = (int) dwBytesSent;
    }
    
    LOGEXIT("PAL_send returns int %d\n", ret);
    PERF_EXIT(send);
    return ret;
}


/*++
Function:
  PAL_recv

See MSDN doc.
--*/
int
PALAPI
PAL_recv (
        IN PAL_SOCKET s,
        OUT char FAR * buf,
        IN int len,
        IN int flags)
{
    int ret;
    DWORD dwBytesRecvd = 0;
    WSABUF ws_buff;

    PERF_ENTRY(recv);
    ENTRY("PAL_recv(s=%#x, buf=%p, len=%d, flags=%#x)\n",
           s, buf, len, flags );

    ws_buff.len = (u_long) len;
    ws_buff.buf = (char*) buf;

    dwBytesRecvd = 0;
    ret = WSARecv(
            s, &ws_buff, 1, &dwBytesRecvd, (LPDWORD) &flags,
            (LPWSAOVERLAPPED) NULL,
            (LPWSAOVERLAPPED_COMPLETION_ROUTINE) NULL);

    if (SOCKET_ERROR != ret)
    {
        ret = (int) dwBytesRecvd;
    }

    LOGEXIT("PAL_recv returns int %d\n", ret);
    PERF_EXIT(recv);
    return ret;
}

/*++
Function:
  PAL_closesocket

See MSDN doc.
--*/
int
PALAPI
PAL_closesocket (
        IN PAL_SOCKET s)
{
    int ret = 0;
    int ret1 = 0;
    unsigned int i;
    const unsigned int op_idx =
#if CLOSE_BLOCKS_ON_OUTSTANDING_SYSCALLS
      1; /* Need first to remove the fd from the poll's fds, 
            otherwise close() will block: the switch-block below
            will be executed in 1,0 order */
#else
      0; /* first close the socket: : the switch-block below
            will be executed in 0,1 order  */
#endif // CLOSE_BLOCKS_ON_OUTSTANDING_SYSCALLS
    
    PERF_ENTRY(closesocket);
    ENTRY("PAL_closesocket(s=%#x)\n", s );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        ret = SOCKET_ERROR;
        goto done;
    }

    if ((s < 0) || (s >= SOCK_table_size))
    {
        ERROR("Invalid socket\n");
        SetLastError(WSAENOTSOCK);
        ret = SOCKET_ERROR;
        goto done;
    }

    for (i = op_idx; i < op_idx+2; i++)
    {
      switch (i % 2)
      {
      case 0:
        /* close the socket */
        if (( ret = close(s)) < 0 )
        {
          ret = SOCKET_ERROR;
          SetLastError(SOCKErrorReturn(errno));
          goto done;
        }
        break;

      case 1:
        /* check for and cancel any asynchronous operations */
        if ( SOCKIsAsyncSocket(s) )
        {
          ret1 = SOCKCancelAsyncOperations(s);
          /* if this is the first operation and it fails,
             we continue and close the socket anyway */
        }
        break;

      default:
        /* it should never get here */
        ASSERT("This code should never be executed\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        ret = SOCKET_ERROR;     
      }
    }

    SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
    if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
    {
        free ((void *)SOCK_socket_list.sockets[s].rt_data);
        SOCK_socket_list.sockets[s].rt_data = NULL;
        SOCK_socket_list.sockets[s].CompletionKey = 0;
        SOCK_socket_list.sockets[s].CompletionPort = NULL;
    }
    SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);


done:
    if (0 == ret)
      ret = ret1;
    LOGEXIT("PAL_closesocket returns int %d\n", ret);
    PERF_EXIT(closesocket);
    return ret;
}


/*++
Function:
  PAL_accept

See MSDN doc.
--*/
PAL_SOCKET
PALAPI
PAL_accept (
        IN PAL_SOCKET s,
        OUT struct PAL_sockaddr FAR *addr,
        IN OUT int FAR *addrlen)
{
    int ad;
    struct sockaddr UnixSockaddr;
    socklen_t UnixSockaddrlen;
    int socktype;
    socklen_t socktypelen;
    DWORD dwLastError = 0;

    PERF_ENTRY(accept);
    ENTRY("PAL_accept(s=%#x, addr=%p, addrlen=%p)\n", s, addr, addrlen );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_accept returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(accept);
        return SOCKET_ERROR;
    }
    socktypelen = sizeof(socktype);
    if (getsockopt(s, SOL_SOCKET, SO_TYPE, &socktype, &socktypelen) < 0)
    {
        ERROR("invalid socket\n");
        SetLastError(WSAENOTSOCK);
        LOGEXIT("PAL_accept returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(accept);
        return SOCKET_ERROR;
    }
    else if (socktype == SOCK_DGRAM ||
             socktype == SOCK_RAW   ||
             socktype == SOCK_RDM)
    {
        ERROR("invalid socket type (connection-less).\n");
        SetLastError(WSAEOPNOTSUPP);
        LOGEXIT("PAL_accept returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(accept);
        return SOCKET_ERROR;
    }

    if (NULL != addr && *addrlen != sizeof(struct PAL_sockaddr))
    {
        ERROR("Parameter addrlen has a invalid value\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_accept returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(accept);
        return SOCKET_ERROR;
    }

again:
    UnixSockaddrlen = sizeof (UnixSockaddr); 
    if ((ad = accept(s, &UnixSockaddr, &UnixSockaddrlen)) < 0)
    {
        if (errno == EINTR)
        {
            TRACE("accept() failed with EINTR; re-accepting...\n");
            goto again;
        }

        SetLastError(SOCKErrorReturn(errno));
    }
   
    /*Copy BSD structure to Windows structure*/
    if (addr)
    {
        addr->sa_family = UnixSockaddr.sa_family;
        memcpy(addr->sa_data, UnixSockaddr.sa_data, sizeof(addr->sa_data));  
    }

    /* then export the received length */
    if (addrlen)
    {
#if HAVE_SOCKADDR_SA_LEN
        *addrlen = UnixSockaddr.sa_len;
#else   // HAVE_SOCKADDR_SA_LEN
        *addrlen = sizeof(UnixSockaddr);
#endif  // HAVE_SOCKADDR_SA_LEN
    }

    /* reset FD_ACCEPT network events, if it was previously set with 
       WSAEventSelect */
    if ( (SOCKET_ERROR != ad) && SOCKIsAsyncSocket(s) )
    {
        SOCKResetNetworkEvents(s, FD_ACCEPT);

        /* Wake up worker thread since we've re-enabled FD_ACCEPT network 
           events. So the poll list needs to be refreshed. WS2_OP_EVENTSELECT
           op code will force the worker thread to do that. */
        dwLastError = SOCKSendRequestForAsyncOp(s, WS2_OP_EVENTSELECT, NULL);

        if (dwLastError)
        {
            SetLastError(dwLastError);
            close(ad);
            ad = SOCKET_ERROR;
            goto done;
        }
    }

done:   
    LOGEXIT("PAL_accept returns int %d\n", ad);
    PERF_EXIT(accept);
    return (PAL_SOCKET) ad;
}

/*++
Function:
  PAL_listen

See MSDN doc.
--*/
int
PALAPI
PAL_listen (
        IN PAL_SOCKET s,
        IN int backlog)
{
    int ld = SOCKET_ERROR;
    struct sockaddr UnixSockaddr;
    char sa_data[sizeof(UnixSockaddr.sa_data)];
    socklen_t sockaddr_len;

    PERF_ENTRY(listen);
    ENTRY("PAL_listen(s=%#x, backlog=%d)\n", s, backlog );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_listen returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(listen);
        return SOCKET_ERROR;
    }
    
    /* ensure that this socket is bind()ed */
    memset(sa_data, 0, sizeof(UnixSockaddr.sa_data));

    sockaddr_len = sizeof UnixSockaddr;
    if ( getsockname(s, &UnixSockaddr, &sockaddr_len) < 0 )
    {
        ERROR("Failed to call getsockname()! error is %d (%s)\n", 
              errno, strerror(errno));
        SetLastError(SOCKErrorReturn(errno));
        goto done;
    }
    
    /* If there is no data in sa_data, we aren't bind()ed yet -
       this is an error case in win32 */
    if (!memcmp(UnixSockaddr.sa_data, sa_data, sizeof(UnixSockaddr.sa_data)))
    {
        ERROR("Socket was not previously bound!\n");
        SetLastError(WSAEINVAL);
        goto done;
    }         

    /* now ensure that socket isn't connected, that is another 
       error case in Win32 */
    if(getpeername(s,&UnixSockaddr,&sockaddr_len)==0 || ENOTCONN!=errno)
    {
        ERROR("socket is connected, can't listen\n");
        SetLastError(WSAEISCONN);
        goto done;
    }             

    if ((ld = listen(s,backlog)) < 0)
    {
        ERROR("listen() failed; eror is %d (%s)\n", errno, strerror(errno));
        SetLastError(SOCKErrorReturn(errno));
    }

done:
    LOGEXIT("PAL_listen returns int %d\n",ld);
    PERF_EXIT(listen);
    return ld;
}


/*++
Function:
  PAL_bind

See MSDN doc.
--*/
int
PALAPI
PAL_bind (
        IN PAL_SOCKET s,
        IN const struct PAL_sockaddr FAR *addr,
        IN int namelen)
{
    int bd;
    struct sockaddr UnixSockaddr;

    PERF_ENTRY(bind);
    ENTRY("PAL_bind(s=%#x, addr=%p, namelen=%d)\n", s, addr, namelen );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_bind returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(bind);
        return SOCKET_ERROR;
    }

    if (namelen < sizeof(struct PAL_sockaddr))
    {
        ERROR("The structure size (namelen) is too small\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_bind returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(bind);
        return SOCKET_ERROR;
    }

    // Copy the PAL structure for sockaddr to the native structure.
#if HAVE_SOCKADDR_SA_LEN
    UnixSockaddr.sa_len = sizeof(UnixSockaddr);
#endif  // HAVE_SOCKADDR_SA_LEN
    UnixSockaddr.sa_family = addr->sa_family;
    memcpy(UnixSockaddr.sa_data, addr->sa_data, sizeof(addr->sa_data));
    
    
    if ((bd = bind((int)s, &UnixSockaddr, namelen)) < 0 )
    {
        SetLastError(SOCKErrorReturn(errno));
    }

    LOGEXIT("PAL_bind returns int %d\n", bd);
    PERF_EXIT(bind);
    return bd;
}


/*++
Function:
  PAL_shutdown

See MSDN doc.
--*/
int
PALAPI
PAL_shutdown (
        IN PAL_SOCKET s,
        IN int how)
{
    int sd;
    PERF_ENTRY(shutdown);
    ENTRY("PAL_shutdown(s=%#x, how=%d)\n", s, how );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        sd = SOCKET_ERROR;
        goto done;
    }

    if ((s < 0) || (s >= SOCK_table_size))
    {
        ERROR("Invalid socket\n");
        SetLastError(WSAENOTSOCK);
        sd = SOCKET_ERROR;
        goto done;
    }

    /* For some reason, shutdown() fails on an invalid how parameter
       with the wrong error code, so lets do that check manually */
    if (how < SD_RECEIVE || how > SD_BOTH)
    {
        SetLastError(WSAEINVAL);
        sd = SOCKET_ERROR;
        goto done;
    }

    /* We need to be sure that the socket is already connect()ed.
       (for connection-oriented sockets only)
       This check needs to be done so that the correct error code
       is set. */
    if (SOCKIsSocketConnected(s) <= 0)
    {
        /* LastError set in SOCKIsSocketConnected */
        sd = SOCKET_ERROR;
        goto done;
    }

    if ((sd = shutdown(s, how)) < 0 )
    {     
#if SHUTDOWN_FAILS_ON_CONNECTIONLESS_SOCKETS
        /* On some platforms (e.g. AIX) shutdown(s,...) fails with errno=ENOTCONN
           when s is a connectionless socket. On Windows shutdown works regardless 
           of the socket type, stopping further send and/or receive (depending on 
           the 'how' parameter) on the socket, and WSAENOTCONN is returned only when 
           the target socket is a connection-oriented socket currently not connected. 
           On platforms that do not allow shutdown on connectionless sockets, we 
           simulate the shutdown keeping track of it in the internal PAL socket list
        */

        if (errno == ENOTCONN)
        {
            int type, err;
            socklen_t size = sizeof(int);
 
            err = getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&type, &size);

            if ((0 == err) && (SOCK_STREAM != type)
#if defined(SOCK_SEQPACKET)
                && (SOCK_SEQPACKET != type)
#endif
#if defined(SOCK_CONN_DGRAM)
                && (SOCK_CONN_DGRAM != type)
#endif
               )
            {
                unsigned status_bits = 0;
                sd = 0;
                
                switch (how)
                {
                case SD_BOTH:
                    status_bits = PAL_SOCK_STATUS_SHUTDOWN;
                    break;
                case SD_SEND:
                    status_bits = PAL_SOCK_STATUS_SEND_SHUTDOWN;
                    break;
                case SD_RECEIVE:                  
                    status_bits = PAL_SOCK_STATUS_RECV_SHUTDOWN;
                    break;
                default:
                    /* it should never get here */
                    ASSERT("This code should never be executed\n");
                    sd = SOCKET_ERROR;
                    SetLastError(ERROR_INTERNAL_ERROR);
                    goto done;
                    break;
                }

                SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
                if (SOCK_socket_list.sockets)
                {
                    if (SOCK_socket_list.sockets[s].rt_data == NULL)
                    {
                        if (!SOCKAllocRTAData(s))
                        {
                            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
                            ERROR("Out of memory\n");
                            SetLastError(WSAENETDOWN); // closest shutdown's error to out of mem
                            sd = SOCKET_ERROR;
                            goto done;
                        }
                    }

                    /* Update status bits in runtime structure */
                    SOCK_socket_list.sockets[s].rt_data->status |= status_bits;
                }
                SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            }
            else
            {
                errno = ENOTCONN;
            }
        }
        if (sd != 0)
        {
            sd = SOCKET_ERROR;
            SetLastError(SOCKErrorReturn(errno));
        }
#else // SHUTDOWN_FAILS_ON_CONNECTIONLESS_SOCKETS
        SetLastError(SOCKErrorReturn(errno));
#endif // SHUTDOWN_FAILS_ON_CONNECTIONLESS_SOCKETS
    }

done:
    LOGEXIT("PAL_shutdown returns int %d\n", sd);
    PERF_EXIT(shutdown);
    return sd;
}


/*++
Function:
  PAL_sendto

See MSDN doc.
--*/
int
PALAPI
PAL_sendto (
        IN PAL_SOCKET s,
        IN const char FAR * buf,
        IN int len,
        IN int flags,
        IN const struct PAL_sockaddr FAR *to,
        IN int tolen)
{
    WSABUF ws_buff;
    DWORD dwBytesSent;
    int ret;

    PERF_ENTRY(sendto);
    ENTRY("PAL_sendto(s=%#x, buf=%p, len=%d, flags=%#x, "
          "to=%p, tolen=%d)\n",s, buf, len, flags, to, tolen );

    ws_buff.len = (u_long) len;
    ws_buff.buf = (char*) buf;

    dwBytesSent = 0;
    ret = WSASendTo(
            s, &ws_buff, 1, &dwBytesSent, (DWORD) flags,
            to, tolen,
            (LPWSAOVERLAPPED) NULL, 
            (LPWSAOVERLAPPED_COMPLETION_ROUTINE) NULL);

    if (SOCKET_ERROR != ret)
    {
        ret = (int) dwBytesSent;
    }
   
    LOGEXIT("PAL_sendto returns int %d\n", ret);
    PERF_EXIT(sendto);
    return ret;
}


/*++
Function:
  PAL_recvfrom

See MSDN doc.
--*/
int
PALAPI
PAL_recvfrom (
        IN PAL_SOCKET s,
        OUT char FAR * buf,
        IN int len,
        IN int flags,
        OUT struct PAL_sockaddr FAR *from,
        IN OUT int FAR * fromlen)
{
    int ret;
    DWORD dwBytesRecvd = 0;
    WSABUF ws_buff;

    PERF_ENTRY(recvfrom);
    ENTRY("PAL_recvfrom(s=%#x, buf=%p, len=%d, flags=%#x, "
          "from=%p, fromlen=%p)\n",s, buf, len, flags, from, fromlen );

    ws_buff.len = (u_long) len;
    ws_buff.buf = (char*) buf;

    dwBytesRecvd = 0;
    
    ret = WSARecvFrom(
            s, &ws_buff, 1, &dwBytesRecvd, (LPDWORD) &flags,
            from, (LPINT) fromlen,
            (LPWSAOVERLAPPED) NULL,
            (LPWSAOVERLAPPED_COMPLETION_ROUTINE) NULL);

    if (SOCKET_ERROR != ret)
    {
        ret = (int) dwBytesRecvd;
    }

    LOGEXIT("PAL_recvfrom returns int %d\n", ret);
    PERF_EXIT(recvfrom);
    return ret;
}


/*++
Function:
  PAL_getsockname

See MSDN doc.
--*/
int
PALAPI
PAL_getsockname (
        IN PAL_SOCKET s,
        OUT struct PAL_sockaddr FAR *name,
        IN OUT int FAR * namelen)
{
    int gd;
    struct sockaddr UnixSockaddr;
    char sa_data[sizeof(UnixSockaddr.sa_data)];

    PERF_ENTRY(getsockname);
    ENTRY("PAL_getsockname(s=%#x, name=%p, namelen=%p )\n",
          s, name, namelen );

    memset(sa_data, 0, sizeof(UnixSockaddr.sa_data));
    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_getsockname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockname);
        return SOCKET_ERROR;
    }

    /* Sanity check -> the incoming name pointer must be at least
       the size of PAL_sockaddr */
    if (IsBadWritePtr(namelen, sizeof(sizeof(int))) ||
        (*namelen < sizeof(struct PAL_sockaddr)))
    {
        ERROR("Parameter namelen has an invalid value\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_getsockname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockname);
        return SOCKET_ERROR;
    }

    /* The buffer pointer should be a valid part of the user
       address space*/
    if (IsBadWritePtr(name,*namelen))
    {
        ERROR("Buffer pointer is not totally contained in a valid"
               "part of the user address space.\n");
        SetLastError(WSAEFAULT);
        LOGEXIT("PAL_getsockname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockname);
        return SOCKET_ERROR;
    }

    if ((gd = getsockname(s, &UnixSockaddr, (socklen_t*) namelen)) < 0 )
    {
        ERROR("getsockname failed!\n");
        SetLastError(SOCKErrorReturn(errno));
        LOGEXIT("PAL_getsockname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockname);
        return SOCKET_ERROR;
    }

    /* If there is no data in sa_data, we aren't bind()ed yet -
       this is an error case in win32 */
    if (!memcmp(UnixSockaddr.sa_data, sa_data, sizeof(UnixSockaddr.sa_data)))
    {
        ERROR("Socket was not previously bound!\n");
        SetLastError(WSAEINVAL);
        LOGEXIT("PAL_getsockname returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(getsockname);
        return SOCKET_ERROR;
    }

    /*Copy BSD structure to Windows structure*/
    name->sa_family = UnixSockaddr.sa_family;
    memcpy(name->sa_data, UnixSockaddr.sa_data, sizeof(name->sa_data));

    LOGEXIT("PAL_getsockname returns int %d\n", gd);
    PERF_EXIT(getsockname);
    return gd;
}


/*++
Function:
  PAL_select

See MSDN doc.
--*/
int
PALAPI
PAL_select (
        IN int nfds,
        IN OUT PAL_fd_set FAR *readfds,
        IN OUT PAL_fd_set FAR *writefds,
        IN OUT PAL_fd_set FAR *exceptfds,
        IN const struct PAL_timeval FAR *timeout)
{
    int     sd = SOCKET_ERROR;
    int     i;
    int     nCount;
    int     UnixNfds = 0;
    fd_set  UnixReadfds;
    fd_set  UnixWritefds;
    fd_set  UnixExceptfds;
    struct timeval stimeout;

    PERF_ENTRY(select);
    ENTRY("PAL_select(nfds=%d, readfds=%p, writefds=%p, "
          "exceptfds=%p, timeout=%p )\n",
          nfds, readfds, writefds, exceptfds, timeout );

    if ((readfds == NULL) && (writefds == NULL) && (exceptfds == NULL)) 
    {
        ERROR("Invalid NULL parameters\n");
        SetLastError(WSAEINVAL);
        goto EXIT;
    }
    
    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        goto EXIT;
    }

    /*Initalize the descriptors*/
    FD_ZERO(&UnixReadfds);
    FD_ZERO(&UnixWritefds);
    FD_ZERO(&UnixExceptfds);

    /*Copy the set of sockets in the Unix Structure*/
    /*Copy readfds*/
    if (readfds == NULL)
    {
       TRACE("readfds is NULL\n");
    }
    else if ((readfds->fd_count >= 0) && (readfds->fd_count <= FD_SETSIZE))
    {
        for (i=0; i < readfds->fd_count; i++)
        {
            if (FD_SET_BOOL(readfds->fd_array[i], &UnixReadfds) == FALSE)
            {
                ERROR("Invalid socket value\n");
                SetLastError(WSAENOTSOCK);
                goto EXIT;
            }
            
            if (UnixNfds < (readfds->fd_array[i] + 1 ))
            {
                UnixNfds = readfds->fd_array[i] + 1;
            }
        }
        TRACE("PAL_fd_set *readfds converted to BSD structure\n");
    }
    else
    {
        ERROR("readfds is invalid\n");
        SetLastError(WSAEFAULT);
        return (0);
    }

    /*Copy writefds*/
    if (writefds == NULL)
    {
        TRACE("writefds is NULL\n");
    }
    else if ((writefds->fd_count >= 0) && (writefds->fd_count <= FD_SETSIZE))
    {
        for (i=0; i < writefds->fd_count; i++)
        {
            if (FD_SET_BOOL(writefds->fd_array[i], &UnixWritefds) == FALSE)
            {
                ERROR("Invalid socket value\n");
                SetLastError(WSAENOTSOCK);
                goto EXIT;
            }
            if (UnixNfds < ( writefds->fd_array[i] + 1 ))
            {
                UnixNfds = writefds->fd_array[i] + 1;
            }
        }
        TRACE("PAL_fd_set *writefds converted to BSD structure\n");
    }
    else
    {
        ERROR("writefds is invalid\n");
        SetLastError(WSAEFAULT);
        goto EXIT;
    }
    
    /*Copy exceptfds*/
    if (exceptfds == NULL)
    {
        TRACE("exceptfds is NULL\n");
    }
    else if ((exceptfds->fd_count >= 0) && (exceptfds->fd_count <= FD_SETSIZE))
    {
        for (i=0; i < exceptfds->fd_count; i++)
        {
            if (FD_SET_BOOL(exceptfds->fd_array[i], &UnixExceptfds) == FALSE)
            {
                ERROR("Invalid socket value\n");
                SetLastError(WSAENOTSOCK);
                goto EXIT;
            }
            if (UnixNfds < ( exceptfds->fd_array[i] + 1 ))
            {
                UnixNfds = exceptfds->fd_array[i] + 1;
            }
        }
        TRACE("PAL_fd_set *exceptfds converted to BSD structure\n");
    }
    else
    {
        ERROR("exceptfds is invalid\n");
        SetLastError(WSAEFAULT);
        goto EXIT;
    }

again:

    if (timeout)
    {
        stimeout.tv_sec = timeout->tv_sec;
        stimeout.tv_usec = timeout->tv_usec;
    }
   
    if ((sd = select(UnixNfds, &UnixReadfds, &UnixWritefds, &UnixExceptfds, 
        (struct timeval *)( timeout ? &stimeout : NULL))) < 0 )
    {
        if (errno == EINTR)
        {
            TRACE("select() failed with EINTR; re-selecting...\n");
            goto again;
        }
    
        SetLastError(SOCKErrorReturn(errno));
    }

    /*Set Windows fd_set structure with socket that ready*/
    /*See if UnixReadfds is ready*/
    if (readfds != NULL)
    {
        for (nCount=0, i=0;i < readfds->fd_count; i++)
        {
            if (FD_ISSET_BOOL(readfds->fd_array[i], &UnixReadfds))
            {
                readfds->fd_array[nCount] = readfds->fd_array[i];
                nCount++;
            }
        }
        readfds->fd_count = nCount;
        TRACE("fd_set *readfds converted to PAL structure\n");
    }

    /*See if UnixWritefds is ready*/
    if (writefds != NULL)
    {
        for (nCount=0, i=0;i < writefds->fd_count; i++)
        {
            if (FD_ISSET_BOOL(writefds->fd_array[i], &UnixWritefds))
            {
                writefds->fd_array[nCount] = writefds->fd_array[i];
                nCount++;
            }
        }
        writefds->fd_count = nCount;
        TRACE("fd_set *writefds converted to PAL structure\n");
    }

    /*See if UnixExceptfds is ready*/
    if (exceptfds != NULL)
    {
        for (nCount=0, i=0;i < exceptfds->fd_count; i++)
        {
            if (FD_ISSET_BOOL(exceptfds->fd_array[i], &UnixExceptfds))
            {
                exceptfds->fd_array[nCount] = exceptfds->fd_array[i];
                nCount++;
            }
        }
        exceptfds->fd_count = nCount;
        TRACE("fd_set *exceptfds converted to PAL structure\n");
    }
EXIT:
    LOGEXIT("PAL_select returns int %d\n", sd);
    PERF_EXIT(select);
    return sd;
}


/*++
Function:
  PAL_socket

See MSDN doc.
--*/
PAL_SOCKET
PALAPI
PAL_socket (
        IN int af,
        IN int type,
        IN int protocol)
{
    int sd;
    int nativeType;

    PERF_ENTRY(socket);
    ENTRY("PAL_socket(af=%d, type=%d, protocol=%d )\n",
          af, type, protocol );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        LOGEXIT("PAL_socket returns PAL_SOCKET %d\n", INVALID_SOCKET);
        PERF_EXIT(socket);
        return INVALID_SOCKET;
    }

    // Convert to the native socket type.
    nativeType = SOCKGetNativeSocketType(type);
    if (nativeType == -1)
    {
        LOGEXIT("PAL_socket returns PAL_SOCKET %x\n", SOCKET_ERROR);
        PERF_EXIT(socket);
        return (SOCKET_ERROR);
    }

    // SOCK_RDM is only supported by address family AF_APPLETALK
    // (according to "Network Programming for MS Windows", MS Press,
    // p. 130) Since BSD doesn't even implement this type specification,
    // let's return the easy error code.
    if (nativeType == SOCK_RDM && af != AF_APPLETALK)
    {
        SetLastError(WSAESOCKTNOSUPPORT);
        LOGEXIT("PAL_socket returns PAL_SOCKET %x\n", SOCKET_ERROR);
        PERF_EXIT(socket);
        return SOCKET_ERROR;
    }
    

    if ((sd = socket(af, nativeType, protocol)) < 0 )
    {
        SetLastError(SOCKErrorReturn(errno));
    }
    else
    {
        /* make sockets close-on-exec so they don't get inherited by child 
           processes */
        if(-1 == fcntl(sd,F_SETFD,1))
        {
            ASSERT("Can't set close-on-exec flag; fcntl() failed. errno is %d "
                   "(%s)\n", errno, strerror(errno));
            if (close(sd) == -1)
            {
                ERROR("Unable to close newly created socket after fcntl failure!\n");
            }
            sd = -1;
            SetLastError(ERROR_INTERNAL_ERROR);
        }
    }

    /*                                                                          
    */
    if (sd != SOCKET_ERROR)
    {
        int one = 1;
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &one, 
            sizeof(one)) == -1 )
        {
            ERROR("setsockopt failed on the new socket\n");
            if (close(sd) == -1)
            {
                ERROR("Unable to close newly created socket after setsockopt failure!\n");
            }
            sd = -1;
            SetLastError(ERROR_INTERNAL_ERROR);
        }
    }

    LOGEXIT("PAL_socket returns PAL_SOCKET %x\n", sd);
    PERF_EXIT(socket);
    return (PAL_SOCKET) sd;
}


/*++
Function:
  PAL_gethostbyname

Must convert between win32 and BSD hostent structure.

Also, on BSD, gethostbyname and gethostbyaddr are NOT thread-safe, since
they use static data structures. MSDN says that "one copy of this
structure is allocated by thread", so that is implemented using the
Tls functions. The Tls index is allocated in the first call to 
--*/
PALIMPORT struct PAL_hostent FAR * PALAPI PAL_gethostbyname(
    IN const char FAR * name )
{
    struct hostent     *result;
    struct PAL_hostent *ret = NULL;
    char host_name[_MAX_FNAME];
    THREAD              *lpThread;

    PERF_ENTRY(gethostbyname);
    ENTRY("PAL_gethostbyname( name=%p (%s) )\n", name ? name : "NULL" , name ? name : "NULL" );

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        goto done;
    }

    SYNCEnterCriticalSection( &SOCK_crit_section , TRUE);
    
    if (name == NULL)
    {
        if (gethostname(host_name, _MAX_FNAME) == -1)
        {
            ERROR("gethostname() failed errno:%d (%s)\n",
                    errno, strerror(errno));
            SetLastError(SOCKErrorReturn(errno));
            goto leave;
        }
        
        name = host_name;
    }
    result = gethostbyname(name);
    
    if (result)
    {
        lpThread = PROCGetCurrentThreadObject();
        if (lpThread == NULL)
        {
            ASSERT("Unable to access the current thread\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto leave;
        }

        ret = &lpThread->hostentBuffer;

        // Copy between the two structures.
        if (!SOCKConvertNativeToPALHostent(result, ret, NULL))
        {
            // SOCKConvertNativeToPALHostent calls SetLastError for us.
            ERROR("SOCKConvertNativeToPALHostent failed\n");
            ret = NULL;
            goto leave;
        }
    }
    else
    {
        // If we drop into this case, gethostbyname() failed.
        // If h_errno is set to a value other than the constants
        // listed in the man page for gethostbyname(), call
        // SOCKErrorReturn() on errno.
        switch (h_errno)
        {
            case HOST_NOT_FOUND:
                SetLastError(WSAHOST_NOT_FOUND);
                break;
            case TRY_AGAIN:
                SetLastError(WSATRY_AGAIN);
                break;
            case NO_RECOVERY:
                SetLastError(WSANO_RECOVERY);
                break;
            case NO_DATA:
                SetLastError(WSANO_DATA);
                break;
            default:
                SetLastError(SOCKErrorReturn(errno));
                break;
        }
    }

leave:
    SYNCLeaveCriticalSection( &SOCK_crit_section , TRUE);

done:
    LOGEXIT("PAL_gethostbyname returns struct PAL_hostent FAR * %p\n", ret);
    PERF_EXIT(gethostbyname);
    return ret;
}


/*++
Function:
  PAL_gethostbyaddr

Must convert between win32 and BSD hostent structure.
--*/
PALIMPORT struct PAL_hostent FAR * PALAPI PAL_gethostbyaddr(
    IN const char FAR * addr,
    IN int len,
    IN int type )
{
    struct hostent     *result;
    struct PAL_hostent *ret = NULL;
    THREAD             *lpThread;
   
    PERF_ENTRY(gethostbyaddr);
    ENTRY("PAL_gethostbyaddr( addr=%p, len=%d, type=%d )\n", 
          addr, len, type);
    
    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        SetLastError(WSANOTINITIALISED);
        goto done;
    }

#if HAVE_IN_ADDR_T    
    if (len < sizeof(in_addr_t))
#else   /* HAVE_IN_ADDR_T */
    if (len < sizeof(uint32_t))
#endif  /* HAVE_IN_ADDR_T */
    {
        SetLastError(WSAEFAULT);
        goto done;
    }
    
    SYNCEnterCriticalSection( &SOCK_crit_section , TRUE);
    result = gethostbyaddr(addr, len, type);
    
    if (result)
    {
        char hostname[_MAX_FNAME];
    
        lpThread = PROCGetCurrentThreadObject();
        if (lpThread == NULL)
        {
            ASSERT("Unable to access the current thread\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto leave;
        }

        ret = &lpThread->hostentBuffer;

        /* copy between the two structures */
        if (strcmp(result->h_name, LOCAL_HOST) == 0) 
        {
            if (gethostname(hostname, _MAX_FNAME) == -1)
            {
                ERROR("gethostname() failed errno:%d (%s)\n",
                       errno, strerror(errno));
                SetLastError(SOCKErrorReturn(errno));
                ret = NULL;
                goto leave;
            }
        }
        else
        {
            strncpy(hostname, result->h_name, _MAX_FNAME);
            hostname[_MAX_FNAME - 1] = '\0';
        }
        
        if (!SOCKConvertNativeToPALHostent(result, ret, hostname))
        {
            // SOCKConvertNativeToPALHostent calls SetLastError for us.
            ERROR("SOCKConvertNativeToPALHostent failed!\n");
            ret = NULL;
            goto leave;
        }
    }
    else
    {
        // If we drop into this case gethostbyaddr() failed.
        // If h_errno is set to a value other than the constants
        // listed in the man page for gethostbyaddr(), call
        // SOCKErrorReturn() on errno.
        switch (h_errno)
        {
            case HOST_NOT_FOUND:
                SetLastError(WSAHOST_NOT_FOUND);
                break;
            case TRY_AGAIN:
                SetLastError(WSATRY_AGAIN);
                break;
            case NO_RECOVERY:
                SetLastError(WSANO_RECOVERY);
                break;
            case NO_DATA:
                SetLastError(WSANO_DATA);
                break;
            default:
                SetLastError(SOCKErrorReturn(errno));
                break;
        }
    }

leave:
    SYNCLeaveCriticalSection( &SOCK_crit_section , TRUE);

done:
    LOGEXIT("PAL_gethostbyaddr returns struct PAL_hostent FAR * NULL\n");
    PERF_EXIT(gethostbyaddr);
    return ret;
}



/*++
Function:
    SOCKErrorReturn

Abstract:
    This function returns the specific error code that would be 
    returned from WSAGetLastError.
    
Parameters:
    int errno : The error code that is returned called function.
    
Returns:
    The specific error code for the given error.    
    
--*/
DWORD 
SOCKErrorReturn( IN int errnumber )
{
    DWORD dwLastError = 0;

    switch( errnumber )
        {
            case ( EBADF ):
                dwLastError = WSAENOTSOCK;
                break;
            case ( ECONNRESET ):
                dwLastError = WSAECONNRESET; 
                break;
            case ( ENOTSOCK ):
                dwLastError = WSAENOTSOCK; 
                break;
            case ( ENOTCONN ):
            case ( EDESTADDRREQ ):
                dwLastError = WSAENOTCONN; 
                break;
            case ( ENOBUFS ):
                dwLastError = WSAENOBUFS; 
                break;
            case ( EFAULT ):
            case ( ENOMEM ):        
                dwLastError = WSAEFAULT; 
                break;
            case ( EACCES ):
                dwLastError = WSAEACCES; 
                break;
            case ( EMSGSIZE ):
                dwLastError = WSAEMSGSIZE; 
                break;
            case ( EHOSTUNREACH ):
                dwLastError = WSAEHOSTUNREACH; 
                break;
            case ( EHOSTDOWN ):
                dwLastError = WSAEHOSTDOWN; 
                break;
            case ( EINTR ):
                dwLastError = WSAEINTR; 
                break;
            case ( EMFILE ):
                dwLastError = WSAEMFILE; 
                break;
            case ( EINVAL ):
            case ( EDOM ):
                dwLastError = WSAEINVAL; 
                break;
            case ( EWOULDBLOCK ):
                dwLastError = WSAEWOULDBLOCK; 
                break;
            case ( ECONNABORTED ):
                dwLastError = WSAECONNABORTED; 
                break;
            case ( EOPNOTSUPP ):
                dwLastError = WSAEOPNOTSUPP; 
                break;
            case ( EADDRNOTAVAIL ):
                dwLastError = WSAEADDRNOTAVAIL; 
                break;
            case ( EADDRINUSE ):
                dwLastError = WSAEADDRINUSE; 
                break;
            case ( EPROTONOSUPPORT ):
                dwLastError = WSAEPROTONOSUPPORT;
                break;
            case ( EPROTOTYPE ):
                dwLastError = WSAEPROTOTYPE;
                break;
            case ( ENOPROTOOPT ):
                dwLastError = WSAENOPROTOOPT;
                break;
            case ( EAFNOSUPPORT ):
                dwLastError = WSAEAFNOSUPPORT; 
                break;
            case ( EISCONN ):
                dwLastError = WSAEISCONN; 
                break;
            case ( ETIMEDOUT ):
                dwLastError = WSAETIMEDOUT; 
                break;
            case ( ECONNREFUSED ):
                dwLastError = WSAECONNREFUSED; 
                break;
            case ( ENETUNREACH ):
                dwLastError = WSAENETUNREACH; 
                break;
            case ( EINPROGRESS ):
                dwLastError = WSAEINPROGRESS; 
                break;
            case ( EALREADY ):
                dwLastError = WSAEALREADY; 
                break;
            case ( ESHUTDOWN ):
                dwLastError = WSAESHUTDOWN; 
                break;
            default:
                WARN("Unrecognized error code (%d) returned from socket"
                     " function\n",errnumber);
                dwLastError = -1;
        }
    return (dwLastError);
}

/*++
Function:
    SOCKGetNativeSocketLevel

Abstract:
    Returns the native socket level that corresponds to the given PAL
    socket level.
    
Parameters:
    int palLevel: A PAL socket level.
    
Returns:
    The native socket level that corresponds to palLevel, or -1 if palLevel
    doesn't have a corresponding native socket level. If -1 is returned,
    this function will call SetLastError() before returning.
    
--*/
#define TRANSLATE(level)   case PAL_##level: nativeLevel = level; break

int SOCKGetNativeSocketLevel(int palLevel)
{
    int nativeLevel;
    switch (palLevel)
    {
        TRANSLATE(SOL_SOCKET);
        TRANSLATE(IPPROTO_IP);
        TRANSLATE(IPPROTO_ICMP);
        TRANSLATE(IPPROTO_IGMP);
#ifdef IPPROTO_GGP
        TRANSLATE(IPPROTO_GGP);
#endif  // IPPROTO_GGP
        TRANSLATE(IPPROTO_TCP);
        TRANSLATE(IPPROTO_PUP);
        TRANSLATE(IPPROTO_UDP);
        TRANSLATE(IPPROTO_IDP);
#ifdef IPPROTO_ND
        TRANSLATE(IPPROTO_ND);
#endif  // IPPROTO_ND
        TRANSLATE(IPPROTO_RAW);
#ifdef NSPROTO_IPX
        TRANSLATE(NSPROTO_IPX);
#endif  // NSPROTO_IPX
#ifdef NSPROTO_SPX
        TRANSLATE(NSPROTO_SPX);
#endif  // NSPROTO_SPX
#ifdef NSPROTO_SPXII
        TRANSLATE(NSPROTO_SPXII);
#endif  // NSPROTO_SPXII
        default:
            ERROR("Unknown socket level %d!\n", palLevel);
            SetLastError(WSAESOCKTNOSUPPORT);
            nativeLevel = -1;
            break;
    }
    return nativeLevel;
}

#undef TRANSLATE

/*++
Function:
    SOCKGetNativeSocketType

Abstract:
    Returns the native socket type that corresponds to the given PAL
    socket type.
    
Parameters:
    int palType: A PAL socket type.
    
Returns:
    The native socket type that corresponds to palType, or -1 if palType
    doesn't have a corresponding native socket type. If -1 is returned,
    this function will call SetLastError() before returning.
    
--*/
int SOCKGetNativeSocketType(int palType)
{
    int nativeType;
    switch (palType)
    {
        case PAL_SOCK_STREAM: nativeType = SOCK_STREAM; break;
        case PAL_SOCK_DGRAM: nativeType = SOCK_DGRAM; break;
        case PAL_SOCK_RAW: nativeType = SOCK_RAW; break;
        case PAL_SOCK_RDM: nativeType = SOCK_RDM; break;
        case PAL_SOCK_SEQPACKET: nativeType = SOCK_SEQPACKET; break;
        default:
            ERROR("Unknown socket type %d!\n", palType);
            SetLastError(WSAESOCKTNOSUPPORT);
            nativeType = -1;
            break;
    }
    return nativeType;
}

/*++
Function:
    SOCKGetPALSocketType

Abstract:
    Returns the PAL socket type that corresponds to the given native
    socket type.
    
Parameters:
    int nativeType: A native socket type.
    
Returns:
    The PAL socket type that corresponds to nativeType.
    
--*/
int SOCKGetPALSocketType(int nativeType)
{
    int palType;
    switch (nativeType)
    {
        case SOCK_STREAM: palType = PAL_SOCK_STREAM; break;
        case SOCK_DGRAM: palType = PAL_SOCK_DGRAM; break;
        case SOCK_RAW: palType = PAL_SOCK_RAW; break;
        case SOCK_RDM: palType = PAL_SOCK_RDM; break;
        case SOCK_SEQPACKET: palType = PAL_SOCK_SEQPACKET; break;
        default:
            ASSERT("Unknown socket type %d!\n", nativeType);
            palType = 0;
            break;
    }
    return palType;
}

/*++
Function:
    SOCKGetNativeSocketOption

Abstract:
    Returns the native socket option that corresponds to the given
    protocol level and PAL socket option.
    
Parameters:
    int level: A PAL protocol level
    int palOption: A PAL socket option.
    
Returns:
    The native socket option, or -1 if no such option or level exists.
    If the function returns -1, it will call SetLastError() to set the
    appropriate error based on whether the level or option is invalid.
    
--*/

#define TRANSLATE(option)   case PAL_##option: nativeOption = option; break

int SOCKGetNativeSocketOption(int palLevel, int palOption)
{
    int nativeOption = -1;
    switch (palLevel)
    {
        case PAL_SOL_SOCKET:
            switch (palOption)
            {
                TRANSLATE(SO_DEBUG);
                TRANSLATE(SO_ACCEPTCONN);
                TRANSLATE(SO_REUSEADDR);
                TRANSLATE(SO_KEEPALIVE);
                TRANSLATE(SO_DONTROUTE);
                TRANSLATE(SO_BROADCAST);
#ifdef SO_USELOOPBACK
                TRANSLATE(SO_USELOOPBACK);
#endif  // SO_USELOOPBACK
                TRANSLATE(SO_LINGER);
                TRANSLATE(SO_OOBINLINE);
#ifdef SO_DONTLINGER
                TRANSLATE(SO_DONTLINGER);
#endif  // SO_DONTLINGER
#ifdef SO_EXCLUSIVEADDRUSE
                TRANSLATE(SO_EXCLUSIVEADDRUSE);
#endif  // SO_EXCLUSIVEADDRUSE
                TRANSLATE(SO_SNDBUF);
                TRANSLATE(SO_RCVBUF);
                TRANSLATE(SO_SNDLOWAT);
                TRANSLATE(SO_RCVLOWAT);
                TRANSLATE(SO_SNDTIMEO);
                TRANSLATE(SO_RCVTIMEO);
                TRANSLATE(SO_ERROR);
                TRANSLATE(SO_TYPE);
                default: SetLastError(WSAENOPROTOOPT); break;
            }
            break;
        case PAL_IPPROTO_IP:
            switch (palOption)
            {
                TRANSLATE(IP_OPTIONS);
                TRANSLATE(IP_HDRINCL);
                TRANSLATE(IP_TOS);
                TRANSLATE(IP_TTL);
                TRANSLATE(IP_MULTICAST_IF);
                TRANSLATE(IP_MULTICAST_TTL);
                TRANSLATE(IP_MULTICAST_LOOP);
                TRANSLATE(IP_ADD_MEMBERSHIP);
                TRANSLATE(IP_DROP_MEMBERSHIP);
#ifdef IP_DONTFRAGMENT
                TRANSLATE(IP_DONTFRAGMENT);
#endif  // IP_DONTFRAGMENT
#ifdef IP_ADD_SOURCE_MEMBERSHIP
                TRANSLATE(IP_ADD_SOURCE_MEMBERSHIP);
#endif  // IP_ADD_SOURCE_MEMBERSHIP
#ifdef IP_DROP_SOURCE_MEMBERSHIP
                TRANSLATE(IP_DROP_SOURCE_MEMBERSHIP);
#endif  // IP_DROP_SOURCE_MEMBERSHIP
#ifdef IP_BLOCK_SOURCE
                TRANSLATE(IP_BLOCK_SOURCE);
#endif  // IP_BLOCK_SOURCE
#ifdef IP_UNBLOCK_SOURCE
                TRANSLATE(IP_UNBLOCK_SOURCE);
#endif  // IP_UNBLOCK_SOURCE
#ifdef IP_PKTINFO
                TRANSLATE(IP_PKTINFO);
#endif  // IP_PKTINFO
                default: SetLastError(WSAENOPROTOOPT); break;
            }
            break;
        case PAL_IPPROTO_TCP:
            switch (palOption)
            {
                TRANSLATE(TCP_NODELAY);
#ifdef TCP_BSDURGENT
                TRANSLATE(TCP_BSDURGENT);
#endif  // TCP_BSDURGENT
                default: SetLastError(WSAENOPROTOOPT); break;
            }
            break;
        case PAL_IPPROTO_UDP:
            switch (palOption)
            {
                TRANSLATE(UDP_NOCHECKSUM);
                TRANSLATE(UDP_CHECKSUM_COVERAGE);
                default: SetLastError(WSAENOPROTOOPT); break;
            }
            break;
        default:
            // Unknown level.
            SetLastError(WSAEINVAL);
            break;
    }
    return nativeOption;
}

#undef TRANSLATE

/*++
Function:
    SOCKConvertNativeToPALHostent

Abstract:
    Copies a native struct hostent into a PAL struct hostent.
    
Parameters:
    const struct hostent *hostent: A native hostent
    struct PAL_hostent *palHostent: A PAL hostent
    const char *hostname: An optional hostname to copy into h_name in
                          place of hostent->h_name
    
Returns:
    TRUE if hostent was successfully copied into PAL_hostent.  If the
    function returns FALSE, it will call SetLastError() with an
    appropriate error code.
    
--*/

static BOOL SOCKConvertNativeToPALHostent(const struct hostent *hostent,
                                          struct PAL_hostent *palHostent,
                                          const char *hostname)
{
    int i;
    int numItems;
    
    // Deallocate the previous fields of the PAL hostent first.
    SOCKFreePALHostentFields(palHostent);

    // Now copy the native hostent to the PAL hostent.
    if (hostname != NULL)
    {
        palHostent->h_name = strdup(hostname);
    }
    else
    {
        palHostent->h_name = strdup(hostent->h_name);
    }
    if (palHostent->h_name == NULL)
    {
        ERROR("Failed to allocate h_name\n");
        goto error;
    }

    numItems = 0;
    while (hostent->h_aliases[numItems] != NULL)
    {
        numItems++;
    }
    numItems++; // For the NULL item at the end of the list
    palHostent->h_aliases = (char **) malloc(numItems * sizeof(char *));
    if (palHostent->h_aliases == NULL)
    {
        ERROR("Failed to allocate h_aliases\n");
        goto error;
    }
    // Copy everything up to the last entry, but not that one because it's NULL.
    for(i = 0; i < numItems - 1; i++)
    {
        palHostent->h_aliases[i] = strdup(hostent->h_aliases[i]);
        if (palHostent->h_aliases[i] == NULL)
        {
            ERROR("Failed to allocate an h_alias\n");
            goto error;
        }
    }
    palHostent->h_aliases[i] = NULL;
    
    palHostent->h_addrtype = hostent->h_addrtype;
    palHostent->h_length = hostent->h_length;
    
    numItems = 0;
    while (hostent->h_addr_list[numItems] != NULL)
    {
        numItems++;
    }
    numItems++; // For the NULL item at the end of the list
    palHostent->h_addr_list = (char **) malloc(numItems * sizeof(char *));
    if (palHostent->h_addr_list == NULL)
    {
        ERROR("Failed to allocate h_aliases\n");
        goto error;
    }
    // Copy everything up to the last entry, but not that one because it's NULL.
    for(i = 0; i < numItems - 1; i++)
    {
        palHostent->h_addr_list[i] = malloc(palHostent->h_length);
        if (palHostent->h_addr_list[i] == NULL)
        {
            ERROR("Failed to allocate an h_alias\n");
            goto error;
        }
        memcpy(palHostent->h_addr_list[i], hostent->h_addr_list[i], palHostent->h_length);
    }
    palHostent->h_addr_list[i] = NULL;

    return TRUE;

error:
    SOCKFreePALHostentFields(palHostent);
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return FALSE;
}

/*++
Function:
    FD_SET_BOOL
Abstract:
   Calls FD_SET macro.   
Parameters:
    PAL_SOCKET sockId: socket ID
    fd_set* fd: fd_set structure.    
Returns:
   returns FALSE when socket value is invalid, TRUE otherwise.
--*/
BOOL FD_SET_BOOL(PAL_SOCKET sockId, fd_set* fd)
{
    // calls FD_SET  macro only if socket value < FD_SETSIZE.
    if (sockId >= FD_SETSIZE) 
    {
        return FALSE;
    }

    FD_SET(sockId, fd);
    return TRUE;
}

/*++
Function:
    SOCKAllocRTAData
Abstract:
    Allocates RTA data structure for the given s socket.
Parameters:
    PAL_SOCKET s: target socket;    
Returns:
    Returns TRUE if it succeeds, FALSE otherwise (out of memory).
Note:
    This function MUST be called holding the SOCK_list_crit_section 
    critical section
--*/
static BOOL SOCKAllocRTAData(PAL_SOCKET s)
{
    BOOL ret = FALSE;
    SOCK_socket_list.sockets[s].rt_data = 
        (runtime_socket_data *)malloc(sizeof(runtime_socket_data));
    if (SOCK_socket_list.sockets[s].rt_data)
    {
        memset ((void *)SOCK_socket_list.sockets[s].rt_data, 0, sizeof(runtime_socket_data));
        ret = TRUE;
    }

    return ret;
}

/*++
Function:
    FD_CLR_BOOL
Abstract:
   Calls FD_CLR macro.
Parameters:
    PAL_SOCKET sockId: socket ID
    fd_set* fd: fd_set structure.    
Returns:
    returns FALSE when socket value is invalid, TRUE otherwise.
--*/
BOOL FD_CLR_BOOL(PAL_SOCKET sockId, fd_set* fd)
{
    // Call FD_CLR macro only if socket value < FD_SETSIZE
    if (sockId >= FD_SETSIZE) 
    {
        return FALSE;
    }

    FD_CLR(sockId, fd);
    return TRUE;
}

