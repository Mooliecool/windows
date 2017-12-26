/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    socket2.c

Abstract:

    Implementation of the Winsock 2 API required for the PAL

Notes:

Important! The SOCKET (PAL_SOCKET) type is NOT managed by the handle
manager, therefore CloseHandle cannot be called on a socket, unlike
in win32. See socket.c.

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/file.h"
#include "pal/socket2.h"
#include "pal/critsect.h"
#include "pal/thread.h"

#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#if HAVE_POLL
#include <poll.h>
#else
#include "pal/fakepoll.h"
#endif  // HAVE_POLL
#include <sys/uio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#if HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif  // HAVE_SYS_FILIO_H
#if HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif  // HAVE_SYS_SOCKIO_H
#include <sys/resource.h>

#if defined(_XOPEN_SOURCE_EXTENDED)
#include <arpa/inet.h>
#endif

SET_DEFAULT_DEBUG_CHANNEL(SOCKET);

CRITICAL_SECTION SOCK_crit_section;
CRITICAL_SECTION SOCK_list_crit_section;

static HANDLE SOCK_worker_thread_handle = INVALID_HANDLE_VALUE;

#define SOCKWORKERTHREAD_INIT_TIMEOUT     5000

int     SOCK_startup_count = 0;

int SOCK_pipe_fd[2] = { -1, -1 };
#define SOCK_PIPE_READ  SOCK_pipe_fd[0]
#define SOCK_PIPE_WRITE SOCK_pipe_fd[1]

static void  SOCKCommonCleanup(BOOL dontwait);
static long SOCKGetCurrMaxOpenFiles(void);
static BOOL SOCKInitIOCP(pal_iocp *pIOCP);

/* This is needed for WSAIoctl */
#define IOC_TYPE_MASK           0x3
#define IOC_TYPE_UNIX           0x0
#define IOC_TYPE_WINSOCK2       0x1
#define IOC_TYPE_SPECIFICADDR   0x2
#define IOC_TYPE_VENDORDEFINED  0x3

/* List of sockets */
pal_socket_list SOCK_socket_list = {0, NULL, NULL};
int SOCK_table_size = FD_SETSIZE;

/* Global IO completion port */
pal_iocp SOCK_iocp;

/*++
Function:
  WSAStartup

See MSDN doc.
--*/
int
PALAPI
WSAStartup(
       IN WORD wVersionRequired,
       OUT LPWSADATA lpWSAData)
{
    DWORD thread_id;
    int Ret = 0;

    PERF_ENTRY(WSAStartup);
    ENTRY("WSAStartup (wVersionRequired=0x%x, lpWSAData=0x%p)\n",
          wVersionRequired, lpWSAData);

    if ( wVersionRequired != 0x202 )
    {
        ERROR("wVersionRequired is not 2.2\n");
        Ret = WSAVERNOTSUPPORTED;
        goto done;
    }

    if (lpWSAData)
    {
        WARN("lpWSAData is non-NULL, but this implementation ignores it\n");

        lpWSAData->wVersion = 0x202;
    }

    SYNCEnterCriticalSection( &SOCK_crit_section , TRUE);
    if ( SOCK_startup_count == 0 )
    {
        DWORD WaitResult;
        HANDLE hWorkerThreadInitCompleted = NULL;

        SOCK_socket_list.max_fd = SOCK_PIPE_READ;
        SOCK_socket_list.pollfds = (struct pollfd *)malloc(SOCK_table_size * sizeof(struct pollfd));
        SOCK_socket_list.sockets = (ws2_sock *)malloc(SOCK_table_size * sizeof(ws2_sock));

        if ((NULL == SOCK_socket_list.pollfds) || (NULL == SOCK_socket_list.sockets))
        {
            ERROR("Could not allocate socket internal tables\n");   

            /* WSAEPROCLIM is the error closest to out of memory 
                among those documented to be returned by WSAStartup */
            Ret = WSAEPROCLIM;
            goto error_cleanup;
        }

        memset (SOCK_socket_list.pollfds, 0, SOCK_table_size * sizeof(struct pollfd));
        memset ((void *)SOCK_socket_list.sockets, 0, SOCK_table_size * sizeof(ws2_sock));

        if ( pipe(SOCK_pipe_fd) != 0 )
        {
            ERROR("Could not create pipe for communication "
                  "with worker thread\n");
            Ret = WSASYSNOTREADY;
            goto error_cleanup;
        }
        TRACE("Created pipe, file descriptors are %d (read) and %d (write)\n",
              SOCK_PIPE_READ, SOCK_PIPE_WRITE);

        /* make the read end of the pipe non-blocking */
        if ( fcntl(SOCK_PIPE_READ, F_SETFL, O_NONBLOCK) == -1 )
        {
            ERROR("Could not make file descriptor %d non-blocking\n", 
                  SOCK_PIPE_READ);
            Ret = WSASYSNOTREADY;
            goto error_cleanup;
        }

        /* event on which the worker thread will notify us when ready */
        hWorkerThreadInitCompleted = CreateEvent( 
                                            NULL,  /* security descriptor */
                                            TRUE,  /* manual reset */
                                            FALSE, /* initial state */
                                            NULL); /* name */
       
        if (NULL == hWorkerThreadInitCompleted)
        {
            ERROR("Could not create thread init notification event object\n");
            Ret = WSASYSNOTREADY;
            goto error_cleanup;
        }

        /* start the worker thread */
        SOCK_worker_thread_handle = 
            CreateInternalThread( NULL, 0, 
                                  (LPTHREAD_START_ROUTINE)SOCKWorkerThreadMain,
                                  (PVOID) &hWorkerThreadInitCompleted, 0,
                                  &thread_id );
        if ( SOCK_worker_thread_handle == NULL )
        {
            ERROR("Could not create worker thread\n");
            if(CloseHandle(hWorkerThreadInitCompleted)== FALSE)
            {
                WARN("Unable to close worker thread handle\n");
            }
            hWorkerThreadInitCompleted = NULL;
            Ret = WSASYSNOTREADY;
            goto error_cleanup;
        }

        /* give it some setup time... */
        WaitResult = WaitForSingleObject(
                        hWorkerThreadInitCompleted,
                        SOCKWORKERTHREAD_INIT_TIMEOUT);

        if (WaitResult != WAIT_OBJECT_0)
        {
            ERROR("Unexpected failure while waiting for worker thread "
                  "initialization\n");

            if(CloseHandle(hWorkerThreadInitCompleted)== FALSE)
            {
                WARN("Unable to close worker thread handle\n");
            }
            hWorkerThreadInitCompleted = NULL;
            Ret = WSASYSNOTREADY;
            goto error_cleanup;
        }

        if(CloseHandle(hWorkerThreadInitCompleted)== FALSE)
        {
            WARN("Unable to close worker thread handle\n");
        }
        hWorkerThreadInitCompleted = NULL;

        if (SOCKInitIOCP(&SOCK_iocp) == FALSE)
        {
            ERROR("Unexpected failure while initializing "
                  "global IO completion port\n");
            Ret = WSASYSNOTREADY;
            goto error_cleanup;
        }
    }

    ++SOCK_startup_count;
    TRACE("startup count is now %d\n", SOCK_startup_count);

    SYNCLeaveCriticalSection( &SOCK_crit_section , TRUE);
    goto done;

error_cleanup:
    if (SOCK_worker_thread_handle != INVALID_HANDLE_VALUE)
    {
        SOCKStopWorkerThread(INFINITE);
    
        if ( CloseHandle( SOCK_worker_thread_handle ) == FALSE )
        {
            WARN("Unable to reclaim worker thread handle\n");
        }
    }

    if (SOCK_socket_list.pollfds) 
    {
      free (SOCK_socket_list.pollfds);
      SOCK_socket_list.pollfds = NULL;
    }
    if (SOCK_socket_list.sockets)
    {
      free ((void *)SOCK_socket_list.sockets);
      SOCK_socket_list.sockets = NULL;
    }

    {
        int i;
        /* close the pipe if it was opened */
        for( i = 0; i <= 1; ++i )
        {
            if ( SOCK_pipe_fd[i] != -1 )
            {
                if ( close(SOCK_pipe_fd[i]) != 0 )
                {
                    ERROR("Could not close file descriptor %d\n",
                          SOCK_pipe_fd[i]);
                }
            }
            SOCK_pipe_fd[i] = -1;
        }
    }

    SYNCLeaveCriticalSection( &SOCK_crit_section , TRUE);

done:
    if (Ret != 0)
    {
        // Windows implementation does set last error contrary to what MSDN says
        SetLastError(Ret);
    }

    LOGEXIT("WSAStartup returns int %d\n", Ret);
    PERF_EXIT(WSAStartup);
    return Ret;
}

/*++
Function:
  SOCKInitIOCP

  Initialize the completion status list, pthread mutex,
  and pthread condition variable of the global IO
  completion port.

Return value:
  TRUE, if initialization succeeds; FALSE, otherwise.

--*/
static
BOOL
SOCKInitIOCP(pal_iocp *pIOCP)
{
    int pthread_ret;

    InitializeListHead(&pIOCP->completion_packet_list);
    pIOCP->completion_packet_list_length = 0;
    pthread_ret = pthread_mutex_init(&pIOCP->completion_packet_list_mutex, NULL);
    if (0 != pthread_ret )
    {
        if(ENOMEM == pthread_ret)
        {
            ERROR("pthread_mutex_init() failed with ENOMEM\n");
        }
        else if( EAGAIN == pthread_ret)
        {
            ERROR("pthread_mutex_init() failed with EAGAIN\n");
        }
        else
        {
            ASSERT("pthread_mutex_init() returned %d (%s)\n", 
                   pthread_ret, strerror(pthread_ret));
        }
        return FALSE;
    }
    
    pthread_ret = pthread_cond_init(&pIOCP->completion_packet_list_not_empty, NULL);
    if (0 != pthread_ret )
    {
        if(ENOMEM == pthread_ret)
        {
            ERROR("pthread_cond_init() failed with ENOMEM\n");
        }
        else if( EAGAIN == pthread_ret)
        {
            ERROR("pthread_cond_init() failed with EAGAIN\n");
        }
        else
        {
            ASSERT("pthread_cond_init() returned %d (%s)\n", 
                   pthread_ret, strerror(pthread_ret));
        }
        pthread_mutex_destroy(&pIOCP->completion_packet_list_mutex);
        return FALSE;
    }

    return TRUE;
}


/*++
Function:
  WSACleanup


See MSDN doc.
--*/
int
PALAPI
WSACleanup(
       void)
{
    PERF_ENTRY(WSACleanup);
    ENTRY("WSACleanup(void)\n");

    SYNCEnterCriticalSection(&SOCK_crit_section, TRUE);

    if ( SOCK_startup_count == 0 )
    {
        LeaveCriticalSection(&SOCK_crit_section);
        SetLastError(WSANOTINITIALISED);
        ERROR("No preceding successful call to WSAStartup\n");
        LOGEXIT("WSACleanup returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(WSACleanup);
        return SOCKET_ERROR;
    }

    --SOCK_startup_count;
    TRACE("startup count is now %d\n", SOCK_startup_count);
    if ( SOCK_startup_count == 0 )
    {
        SOCKCommonCleanup(FALSE);
    }

    LeaveCriticalSection(&SOCK_crit_section);

    LOGEXIT("WSACleanup returns int 0\n");
    PERF_EXIT(WSACleanup);
    return 0;
}

/*++
Function:
  WSAConnect

See MSDN doc.
--*/
int
PALAPI
WSAConnect (
        IN PAL_SOCKET s,
        IN const struct PAL_sockaddr FAR *name,
        IN int namelen,
        IN LPWSABUF lpCallerData,
        IN LPWSABUF lpCalleeData,
        IN LPQOS lpSQOS,
        IN LPQOS lpGQOS)
{
    int cd = 0;
    DWORD  dwLastError = 0;
    struct sockaddr UnixSockaddr;
    socklen_t sockaddrlen;
    int socktype;
    socklen_t socktypelen;

    PERF_ENTRY(WSAConnect);
    ENTRY("WSAConnect(s=%#x, name=%p, namelen=0x%x, lpCallerData=%p, lpCalleeData=%p, lpSQOS=%p, lpGQOS=%p)\n", 
       s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No preceding successful call to WSAStartup\n");
        dwLastError = WSANOTINITIALISED;
        goto done;
    }

    if (lpCallerData || lpCalleeData || lpSQOS || lpGQOS)
    {
        ERROR ("WSAConnect: CallerData, CalleeData, SQOS and GQOS parameters are not supported\n");
        if (lpCallerData || lpCalleeData)
            dwLastError = WSAEPROTONOSUPPORT;
        else
            dwLastError = WSAEOPNOTSUPP;
        goto done;
    }

    // Copy the PAL structure for sockaddr to the native structure.
#if HAVE_SOCKADDR_SA_LEN
    UnixSockaddr.sa_len = sizeof(UnixSockaddr);
#endif  // HAVE_SOCKADDR_SA_LEN
    UnixSockaddr.sa_family = name->sa_family;
    memcpy(UnixSockaddr.sa_data, name->sa_data, sizeof(name->sa_data));
    
    sockaddrlen = namelen;
    if (connect(s, &UnixSockaddr, sockaddrlen) < 0)
    {
        if (errno == EOPNOTSUPP)
        {
            /* FreeBSD sets errno to EOPNOTSUPP if a connect() 
               has been attempted on a listening socket, 
               while Windows implementation set the 
               LastError to WSAEINVAL in that particular case.
            */
            ERROR("WSAConnect on listening socket not supported\n");
            dwLastError = WSAEINVAL;
            goto done; 
        }
        
        if (errno == EINPROGRESS)
        {
            /*  WSAEINPROGRESS is not defined in Winsock2 
                (only in Winsock 1.1!)
              
                To have Winsock2-like behavior, we must convert this error.
            */

            /*  For connectionless sockets, returning WSAEWOULDBLOCK is inappropriate. 
                Windows never returns WSAEWOULDBLOCK for a connect on a DGRAM socket.
                Furthermore, since a connect() on a connectionless socket just specifies 
                the peer address to which messages are to be sent and the call returns 
                immediately, do not return an error. 
            */
            if (getsockopt(s, SOL_SOCKET, SO_TYPE, &socktype, &socktypelen) == 0)
            {
                if(socktype == SOCK_DGRAM || socktype == SOCK_RAW)
                {
                    goto done;
                }
            }

            ERROR("WSAConnect: already trying to connect\n");
            dwLastError = WSAEWOULDBLOCK;
            goto done; 
        }

        ERROR("WSAConnect failed errno %d\n", errno);
        dwLastError = SOCKErrorReturn(errno);
        goto done; 
    }  

done:
    if (dwLastError)
    {
        cd = SOCKET_ERROR;
        SetLastError(dwLastError);
    }
   
    LOGEXIT("WSAConnect returns int %d\n", cd);
    PERF_EXIT(WSAConnect);
    return cd;
}

/*++
Function:
  WSASend

See MSDN doc.
--*/
int
PALAPI
WSASend(
    IN PAL_SOCKET s,
    IN LPWSABUF lpBuffers,
    IN DWORD dwBufferCount,
    OUT LPDWORD lpNumberOfBytesSent,
    IN DWORD dwFlags,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int ret;

    PERF_ENTRY(WSASend);
    ENTRY("WSASend (s=%#x, lpBuffers=%p, dwBufferCount=%u, lpNumberOfBytesSent=%p, "
          "dwFlags=%#x, lpOverlapped=%p, lpCompletionRoutine=%#x)\n", 
          s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,
          dwFlags, lpOverlapped, lpCompletionRoutine);

    ret = WSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,
                     dwFlags, NULL, 0, lpOverlapped, lpCompletionRoutine);

    LOGEXIT("WSASend returns int %d\n", ret);
    PERF_EXIT(WSASend);
    return ret;
}


/*++
Function:
  WSASendTo

See MSDN doc.
--*/
int
PALAPI
WSASendTo(
      IN PAL_SOCKET s,
      IN LPWSABUF lpBuffers,
      IN DWORD dwBufferCount,
      OUT LPDWORD lpNumberOfBytesSent,
      IN DWORD dwFlags,
      IN const struct PAL_sockaddr FAR *lpTo,
      IN int iToLen,
      IN LPWSAOVERLAPPED lpOverlapped,
      IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int    ret = SOCKET_ERROR;
    DWORD  dwLastError = 0;
#if SO_TIMEO_NOT_SUPPORTED
    struct timeval select_tmo = { -1, 0 };
#endif 
#if SHUTDOWN_FAILS_ON_CONNECTIONLESS_SOCKETS
    int type;
    socklen_t size;
#endif
#if BROADCAST_ALLOWED_WITHOUT_SO_BROADCAST
    struct PAL_sockaddr_in * lpToIn;
#endif

    int mstmo;

    PERF_ENTRY(WSASendTo);
    ENTRY("WSASendTo (s=%#x, lpBuffers=%p, dwBufferCount=%u, "
          "lpNumberOfBytesSent=%p, dwFlags=%#x, lpTo=%p, iTolen=%d, "
          "lpOverlapped=%p, lpCompletionRoutine=%#x)\n", 
          s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,
          dwFlags, lpTo, iToLen, lpOverlapped, lpCompletionRoutine);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        dwLastError = WSANOTINITIALISED;
        goto done;
    }

    if ( lpCompletionRoutine )
    {
        ASSERT("lpCompletionRoutine is not NULL, as it should be\n");
        dwLastError = WSAEINVAL;
        goto done;
    }

    if (IsBadWritePtr(lpNumberOfBytesSent, sizeof(DWORD)))
    {
        ERROR("lpNumberOfBytesSent is not totally contained in valid "
              "user memory space\n");
        dwLastError = WSAEFAULT;
        goto done;
    }
    if (IsBadReadPtr(lpBuffers, sizeof(WSABUF)) ||
        IsBadReadPtr(lpBuffers[0].buf, lpBuffers[0].len))
    {
        ERROR("lpBuffers is not totally contained in valid "
              "user memory space\n");
        dwLastError = WSAEFAULT;
        goto done;
    }

    if ( SOCKIsValidSocket(s) == FALSE )
    {
        ERROR("Socket %d is not a valid socket\n", s);
        dwLastError = WSAENOTSOCK;
        goto done;
    }

#if BROADCAST_ALLOWED_WITHOUT_SO_BROADCAST
    lpToIn = (struct PAL_sockaddr_in *)lpTo;
    if (lpToIn && (iToLen == sizeof(struct PAL_sockaddr_in)) && (lpToIn->sin_family == AF_INET) &&
        (lpToIn->sin_addr.S_un.S_addr == htonl(INADDR_BROADCAST)))
    {
        int val;
        socklen_t size;
        size = sizeof(int);

        if ((0 == getsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&val, &size)) && (0 == val))
        {
            /* SO_BROADCAST flag not set */
            dwLastError = WSAEACCES;
            goto done;
        }
    }
#endif

    /* Initialize the buffered timeout to -1, i.e. non-prefetched values */
    mstmo = -1;
    
#if SHUTDOWN_FAILS_ON_CONNECTIONLESS_SOCKETS
    size = sizeof(int);
    if ((0 == getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&type, &size))  && (SOCK_STREAM != type)
#if defined(SOCK_SEQPACKET)
                && (SOCK_SEQPACKET != type)
#endif
#if defined(SOCK_CONN_DGRAM)
                && (SOCK_CONN_DGRAM != type)
#endif
        )
    {
        int bTest = 0;

        SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
        if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
        {
            /* Find out if it has been shut down for send */
            bTest = SOCK_socket_list.sockets[s].rt_data->status & PAL_SOCK_STATUS_SEND_SHUTDOWN;

            /* Prefetch the timeout */
            mstmo = SOCK_socket_list.sockets[s].rt_data->snd_tmo;
        }
        else
        {
            /* Prefetch the timeout */
            mstmo = 0;
        }
        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

        if (bTest)
        {
            /* The socket has been shut down for send */
            dwLastError = WSAESHUTDOWN;
            goto done;
        }
    }
#endif

    /* MSG_OOB is not supported for UDP and FreeBSD ignores the flags
       and proceeds with the operation instead of reporting an unsupported
       operation
    */
    if (dwFlags & PAL_MSG_OOB)
    {
        socklen_t sizeof_int  = sizeof(int);
        int       socket_type = 0;
    
        /*
           First, we need to get the socket type...
        */
        if ( (getsockopt(s, SOL_SOCKET, SO_TYPE, &socket_type, &sizeof_int )) < 0)
        {
            ERROR("getsockopt failed for socket %d\n", s);
            dwLastError = SOCKErrorReturn(errno);
            goto done;
        }
       
        if ((socket_type == SOCK_DGRAM) || (socket_type == SOCK_RAW))
        {
            ERROR("Unsupported operation for non-stream socket\n");
            dwLastError = WSAEOPNOTSUPP;
            goto done;
        }
    }
    
    /* non-overlapped mode OR overlapped mode but the overlapped send queue 
       is empty - e.g. maybe we can process this operation right away.

       Here we access the queue without the queues critical section.
       There is no need to hold the critical section here since we are
       certain the worker thread won't add anything to the queue - all
       the operations are queued in the application threads (see 
       SOCKQueuePushBackOperation call below.

       There is a special case where two threads might be trying to 
       queue an operation simultaneously and that is effectively a race...
       but it's first and foremost an application design issue!
    */
    if (!lpOverlapped || 
         (lpOverlapped && SOCKIsQueueEmpty (s, WS2_OP_SENDTO) ) )
    {
        /* for overlapped, we need to initialize the WSAOVERLAPPED structure
           Internal fields */
        if (lpOverlapped)
        {
            struct pollfd fds;

            fds.fd = s;
            fds.events = POLLOUT;
           
            lpOverlapped->InternalHigh = 0;
            lpOverlapped->Internal     = 0;
            
            /* We are possibly dealing with blocking sockets.
               Because this is an overlapped socket (lpOverlapped != NULL),  
               we don't want to block but rather add the send operation 
               to the queue if the operation cannot be carried right away. 
               To verify whether we can write on the socket without blocking,
               we use poll in POLLOUT mode
            */
poll_again:
            if (-1 == poll(&fds, 1, 0))
            {
                if (errno == EINTR)
                {
                    TRACE("poll() failed with EINTR; re-polling\n");
                    goto poll_again;
                }
                dwLastError = SOCKErrorReturn(errno);
                goto done;
            }
            else if (fds.revents & POLLERR)
            {
                ERROR("Exceptional condition has occured on the socket\n");
                dwLastError = WSAENOTCONN;
                goto done;
            }
            else if (fds.revents & POLLHUP)
            {
                socklen_t ssize = sizeof(int);
                int stype = 0;
                if (0 == getsockopt(s, SOL_SOCKET, SO_TYPE, &stype, &ssize))
                {
                    if(stype == SOCK_DGRAM || stype == SOCK_RAW)
                    {
                        if(shutdown(s, SD_SEND) == -1 && errno == ENOTCONN)
                        {
                            dwLastError = WSAESHUTDOWN;
                            goto done;
                        }
                    }
                }

                ERROR("Socket was disconnected\n");
                dwLastError = WSAENOTCONN;
                goto done;
            }
            else if (fds.revents & POLLNVAL)
            {
                ERROR("Invalid socket\n");
                dwLastError = WSAENOTSOCK;
                goto done;
            }
            else if (!(fds.revents & POLLOUT))
            {
                goto overlapped;
            }
        }

#if SO_TIMEO_NOT_SUPPORTED
        if (!lpOverlapped)
        {
            /* Accordingly to Windows behavior, send and receive timeouts 
               specified through setsockopt are used only for synchronous 
               I/O and ignored in case of overlapped I/O */
            if (mstmo < 0)
            {
                /* mstmo < 0 means that mstmo has not been prefetched
                   therefore we need to get the critical section and
                   access it now */
                SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
                if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
                {
                    mstmo = SOCK_socket_list.sockets[s].rt_data->snd_tmo;
                }
                SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            }

            if (mstmo > 0)
            {
                select_tmo.tv_sec = (long)mstmo / 1000;
                select_tmo.tv_usec = ((long)mstmo % 1000) * 1000;
            }
        }
#endif

send_again:

#if SO_TIMEO_NOT_SUPPORTED
        if (select_tmo.tv_sec >= 0)
        {
            fd_set writefds;
            FD_ZERO(&writefds);
            FD_SET((int)s, &writefds);
            ret = select (s+1, NULL, &writefds, NULL, &select_tmo);
            if (ret < 0)
            {
                if (errno == EINTR)
                {
                    TRACE("select() in sendto() failed with EINTR; re-selecting\n");
                    goto send_again;
                }
                else
                {
                    TRACE("Unexpected error %d.", errno);
                    dwLastError = SOCKErrorReturn(errno);
                    goto done;
                }
            }
            else if (ret == 0)
            {
                TRACE("WSASendTo timed out\n");
                ret = SOCKET_ERROR;
                dwLastError = WSAETIMEDOUT;
                goto done;
            }
        }
#endif

        if (lpTo)
        {
            /* sendto */
            struct sockaddr UnixSockaddr;

            // Copy the PAL structure for sockaddr to the native structure.
#if HAVE_SOCKADDR_SA_LEN
            UnixSockaddr.sa_len = sizeof(UnixSockaddr);
#endif  // HAVE_SOCKADDR_SA_LEN
            UnixSockaddr.sa_family = lpTo->sa_family;
            memcpy(UnixSockaddr.sa_data, lpTo->sa_data, sizeof(lpTo->sa_data));

            ret = sendto((int)s, 
                          lpBuffers[0].buf, lpBuffers[0].len, 
                          (int) dwFlags,
                          &UnixSockaddr, iToLen);
        }
        else
        {
            /* send */
            ret = send((int)s,
                        lpBuffers[0].buf, lpBuffers[0].len,
                        (int) dwFlags); 
        }

        /* we could not process this request right now, queue it */
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                TRACE("send() failed with EINTR; re-sending\n");
                goto send_again;
            }

            if (errno == EAGAIN && lpOverlapped)
            {
                TRACE("Send returned EAGAIN, queuing send operation\n");
                goto overlapped;
            }
            else if (errno == EPIPE)
            {
                /* we should be getting a SIGPIPE signal here, but since
                   the default behavior for SIGPIPE is process termination,
                   we preferred to tell the system that we want to ignore 
                   SIGPIPE signals and be notified through EPIPE error message
                   instead. (see exception/signal.c::SEHInitializeSignals) 
                   
                   In the context of sockets, SIGPIPE is issued when the
                   connection was dropped.
                */
                TRACE("Connection has been dropped.\n");
                dwLastError = WSAESHUTDOWN;
            }
            else
            {
                TRACE("Error happened (%d) and it wasn't EAGAIN.", errno);
                dwLastError = SOCKErrorReturn(errno);
            }
        }
        else 
        {
            TRACE("Send operation completed succesfully. Bytes sent %d\n", ret);
            *lpNumberOfBytesSent = ret;

            if (lpOverlapped)
            {        
                pal_iocp_completion_packet *lpIOCPCompletionPacket;

                lpOverlapped->InternalHigh = ret;
                if (SOCKAllocateCompletionPacket(s, lpOverlapped, &lpIOCPCompletionPacket))
                {
                    SOCKProcessOverlappedCompletion(s, lpOverlapped, lpIOCPCompletionPacket);
                }
                else
                {
                    dwLastError = ERROR_NOT_ENOUGH_MEMORY;
                }
            }
            ret = 0;
        }
    }
    else
overlapped:
    {
        ws2_op_sendto *sendto_op = NULL;
        
        /* We should never attempt to overlap an operation if no peer is 
           associated with the stream socket */
        if ( SOCKIsSocketConnected(s) <= 0)
        {
            dwLastError = GetLastError();
            goto done;
        }

        sendto_op = malloc( sizeof(ws2_op_sendto) );
        if ( !sendto_op ) 
        {   
            ERROR("Couldn't allocate sendto operation\n");
            
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            goto done;
        }


        /* 
        This memory is freed in SOCKQUEURemoveBack, called by 
        SOCKProcessOverlappedSend or SOCKDeleteAsyncSocket

        Also, there's ALWAYS only 1 WSABUF structure (see rotor_pal.doc) 
        */
        sendto_op->lpBuffers = (WSABUF*) malloc( sizeof(WSABUF) );
        if ( !sendto_op->lpBuffers)
        {
            ERROR("Couldn't allocate internal copy of WSABUF structure\n");
            
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            free(sendto_op);
            goto done;
        }

        /* initialize Internal and InternalHigh fields of
           WSAOVERLAPPED */
        lpOverlapped->Internal     = 0;
        lpOverlapped->InternalHigh = 0;
    
        /* NB: we don't need to keep dwBufferCount, it's always 1 */
        sendto_op->lpBuffers[0].buf    = lpBuffers[0].buf;
        sendto_op->lpBuffers[0].len    = lpBuffers[0].len;
        sendto_op->dwFlags             = dwFlags;
        sendto_op->lpTo                = lpTo;
        sendto_op->iToLen              = iToLen;
        sendto_op->lpOverlapped        = lpOverlapped;

        if (!SOCKAllocateCompletionPacket(s, lpOverlapped, &sendto_op->lpIOCPCompletionPacket))
        {
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            free(sendto_op->lpBuffers);
            free(sendto_op);
            goto done;
        }
        
        SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);

        if (FALSE == 
            SOCKQueuePushBackOperation((int)s, WS2_OP_SENDTO, (LPVOID) sendto_op))
        {
            ERROR("couldn't push back operation in overlapped send queue\n");
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            free(sendto_op->lpBuffers);
            free(sendto_op);
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            goto done;
        }
        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

        dwLastError = SOCKSendRequestForAsyncOp(s, WS2_OP_SENDTO, NULL);

        if (dwLastError)
        {
            ret = SOCKET_ERROR;
            SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
            if (FALSE == 
                SOCKQueueRemoveOperation((int)s, WS2_OP_SENDTO, sendto_op))
            {
                ERROR("couldn't remove operation from queue (memory leak)\n");
            }
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            goto done;
        }

        dwLastError = WSA_IO_PENDING;
        ret = SOCKET_ERROR;
    }

done:
    if (dwLastError)
    {
#if SENDTO_RETURNS_UNREACH_ON_WRONG_AF
        if ((dwLastError == WSAEHOSTUNREACH) && lpTo && (lpTo->sa_family == AF_UNIX))
        {
            // This is intended to prevent WSAEHOSTUNREACH when sendto'ing with a
            // lpTo which does not match the socket address family.
            // AF_UNIX sockets are local, WSAEHOSTUNREACH, returned by some platforms
            // in such a case, does not make sense
            dwLastError = WSAEAFNOSUPPORT;
        }
#endif

        if (lpOverlapped && dwLastError != WSA_IO_PENDING)
        {
            lpOverlapped->Internal     = dwLastError;
            lpOverlapped->InternalHigh = 0;
        }
        SetLastError(dwLastError);
    }

    LOGEXIT("WSASendTo returns int %d\n", ret);
    PERF_EXIT(WSASendTo);
    return ret;
}


/*++
Function:
  WSARecv

See MSDN doc.
--*/
int
PALAPI
WSARecv(
    IN PAL_SOCKET s,
    IN OUT LPWSABUF lpBuffers,
    IN DWORD dwBufferCount,
    OUT LPDWORD lpNumberOfBytesRecvd,
    IN OUT LPDWORD lpFlags,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int ret;

    PERF_ENTRY(WSARecv);
    ENTRY("WSARecv (s=%#x, lpBuffers=%p, dwBufferCount=%u, lpNumberOfBytesRecvd=%p,"
          "lpFlags=%#x, lpOverlapped=%p, lpCompletionRoutine=%#x)\n", 
          s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd,
          *lpFlags, lpOverlapped, lpCompletionRoutine);

    ret = WSARecvFrom( s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd,
                       lpFlags, NULL, 0, lpOverlapped, lpCompletionRoutine );

    LOGEXIT("WSARecv returns int %d\n", ret);
    PERF_EXIT(WSARecv);
    return ret;
}


/*++
Function:
  WSARecvFrom

See MSDN doc.
--*/
int
PALAPI
WSARecvFrom(
        IN PAL_SOCKET s,
        IN OUT LPWSABUF lpBuffers,
        IN DWORD dwBufferCount,
        OUT LPDWORD lpNumberOfBytesRecvd,
        IN OUT LPDWORD lpFlags,
        OUT struct PAL_sockaddr FAR *lpFrom,
        IN OUT LPINT lpFromLen,
        IN LPWSAOVERLAPPED lpOverlapped,
        IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int    ret = SOCKET_ERROR;
    DWORD  dwLastError = 0;

    struct sockaddr UnixSockaddr;
    char sa_data[sizeof(UnixSockaddr.sa_data)];
    socklen_t sockaddr_len;
    socklen_t sockfromlen;
#if SO_TIMEO_NOT_SUPPORTED
    struct timeval select_tmo = { -1, 0 };
#endif 
#if SHUTDOWN_FAILS_ON_CONNECTIONLESS_SOCKETS
    int type;
    socklen_t size;
#endif
    int mstmo;
    DWORD SavedFlags = 0;
    WSABUF SavedWASBUF;
    char TmpBuf[1];

    PERF_ENTRY(WSARecvFrom);
    ENTRY("WSARecvFrom (s=%#x, lpBuffers=%p, dwBufferCount=%u, "
          "lpNumberOfBytesRecvd=%p, lpFlags=%#x, lpFrom=%p, lpFromLen=%d, "
          "lpOverlapped=%p, lpCompletionRoutine=%#x)\n", 
          s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd,
          *lpFlags, lpFrom, lpFrom?*lpFromLen:0, lpOverlapped, 
          lpCompletionRoutine);
          
    /* When the len field of WASBUF is 0, recv, recvfrom, WSARecv, and WSARecvFrom
       in WinSock will block when the socket's receive queue is empty.
       However recv and recvfrom on UNIX will not block if the length of receive
       buffer is 0 even when the socket's receive queue is empty.
       In order to match Windows behavior, WSABUF and lpFlags are changed so that
       recv and recvfrom will peek one byte of incoming message.
       The original WSABUF and lpFlags are saved here and
       to be restored before this function exits. */
    SavedWASBUF.len = lpBuffers->len;
    if (SavedWASBUF.len == 0 && !lpOverlapped)
    {
        SavedWASBUF.buf = lpBuffers->buf;
        lpBuffers->len = 1;
        lpBuffers->buf = TmpBuf;
        SavedFlags = *lpFlags;
        if ((SavedFlags & PAL_MSG_PEEK) == 0)
        {
            *lpFlags = SavedFlags | PAL_MSG_PEEK;
        }
    }          

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        dwLastError = WSANOTINITIALISED;
        goto done;
    }

    if ( lpCompletionRoutine )
    {
        ASSERT("lpCompletionRoutine is not NULL, as it should be\n");
        dwLastError = WSAEINVAL;
        goto done;
    }

    if ( SOCKIsValidSocket(s) == FALSE )
    {
        ERROR("Socket %d is not a valid socket\n", s);
        dwLastError = WSAENOTSOCK;
        goto done;
    }
   
    if (IsBadWritePtr(lpNumberOfBytesRecvd, sizeof(DWORD)))
    {
        ERROR("lpNumberOfBytesRecvd is not totally contained in valid "
              "user memory space\n");
        dwLastError = WSAEFAULT;
        goto done;
    }

    if (IsBadWritePtr(lpBuffers, sizeof(WSABUF)))
    {
        ERROR("lpBuffers is not totally contained in valid user "
              "memory space\n");
        dwLastError = WSAEFAULT;
        goto done;
    }
    
    if (lpBuffers[0].len == -1)
    {
        ERROR("Recv buffer length is invalid (-1)");
        dwLastError = WSAEINVAL;
        goto done;
    }
    
    if (IsBadWritePtr(lpBuffers[0].buf, lpBuffers[0].len))
    {
        ERROR("lpBuffers is not totally contained in valid "
              "user memory space\n");
        dwLastError = WSAEFAULT;
        goto done;
    }

    /* Initialize the buffered timeout to -1, i.e. non-prefetched */
    mstmo = -1;

#if SHUTDOWN_FAILS_ON_CONNECTIONLESS_SOCKETS
    size = sizeof(int);
    if ((0 == getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&type, &size))  && (SOCK_STREAM != type)
#if defined(SOCK_SEQPACKET)
        && (SOCK_SEQPACKET != type)
#endif
#if defined(SOCK_CONN_DGRAM)
        && (SOCK_CONN_DGRAM != type)
#endif
        )
    {
        int bTest = 0;
        SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
        if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
        {
            /* Find out if it has been shut down for receive */
            bTest = SOCK_socket_list.sockets[s].rt_data->status & PAL_SOCK_STATUS_RECV_SHUTDOWN;

            /* Prefetch the timeout */
            mstmo = SOCK_socket_list.sockets[s].rt_data->rcv_tmo;
        }
        else
        {
            /* Prefetch the timeout */
            mstmo = 0;
        }
        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

        if (bTest)
        {
            /* The socket has been shut down for receive */
            dwLastError = WSAESHUTDOWN;
            goto done;
        }
    }
#endif
    
    /* We need to be sure that the socket is already connect()ed.
       (for connection-oriented sockets only)
       This check needs to be done so that the correct error code
       is set. */
    if (SOCKIsSocketConnected(s) <= 0)
    {
        /* LastError set in SOCKIsSocketConnected */
        dwLastError = GetLastError();
        goto done;
    }

    /* MSG_OOB is not supported for UDP and FreeBSD ignores the flags
       and proceeds with the operation instead of reporting an unsupported
       operation
    */
    if (*lpFlags & PAL_MSG_OOB)
    {
        socklen_t sizeof_int  = sizeof(int);
        int       socket_type = 0;
    
        /*
           First, we need to get the socket type...
        */
        if ( (getsockopt(s, SOL_SOCKET, SO_TYPE, &socket_type, &sizeof_int )) < 0)
        {
            SetLastError(SOCKErrorReturn(errno));
            ERROR("getsockopt failed for socket %d\n", s);
            PERF_EXIT(WSARecvFrom);
            return(-1);
        }
       
        if ((socket_type == SOCK_DGRAM) || (socket_type == SOCK_RAW))
        {
            SetLastError(WSAEOPNOTSUPP);
            ERROR("Unsupported operation for non-stream socket\n");
            PERF_EXIT(WSARecvFrom);
            return(-1);
        }
    }

    /* ensure that this socket is bind()ed */
    memset(sa_data, 0, sizeof(UnixSockaddr.sa_data));

    sockaddr_len = sizeof UnixSockaddr;
    if ( getsockname(s, &UnixSockaddr, &sockaddr_len) < 0 )
    {
        ERROR("Failed to call getsockname()! error is %d (%s)\n", 
              errno, strerror(errno));
        dwLastError = WSAEINVAL;
        goto done;
    }
    
    /* If there is no data in sa_data, we aren't bind()ed yet -
       this is an error case in win32 */
    if (!memcmp(UnixSockaddr.sa_data, sa_data, sizeof(UnixSockaddr.sa_data)))
    {
        ERROR("Socket was not previously bound!\n");
        dwLastError = WSAEINVAL;
        goto done;
    }

    /* non-overlapped mode OR overlapped mode but 
       the overlapped recv queue is empty. 

       Here we access the queue without the queues critical section.
       There is no need to hold the critical section here since we are
       certain the worker thread won't add anything to the queue - all
       the operations are queued in the application threads (see 
       SOCKQueuePushBackOperation call below.

       There is a special case where two threads might be trying to 
       queue an operation simultaneously and that is effectively a race...
       but it's first and foremost an application design issue!
    */

    if ( (!lpOverlapped) ||
          (lpOverlapped && SOCKIsQueueEmpty(s, WS2_OP_RECVFROM) ) )
    {
        /* for overlapped, we need to initialize the WSAOVERLAPPED structure
           Internal fields */
        if (lpOverlapped)
        {
            struct pollfd fds;

            fds.fd = s;
            fds.events = POLLRDNORM;
            
            lpOverlapped->InternalHigh = 0;
            lpOverlapped->Internal     = 0;

            /* We are possibly dealing with blocking sockets.
               Because this is an overlapped socket (lpOverlapped != NULL),  
               we don't want to block but rather add the recv operation 
               to the queue if the operation cannot be carried right away. 
               To verify whether we can write on the socket without blocking,
               we use poll in POLLOUT mode
            */
poll_again:
            if (-1 == poll(&fds, 1, 0))
            {
                if (errno == EINTR)
                {
                    TRACE("poll() failed with EINTR; re-polling\n");
                    goto poll_again;
                }

                dwLastError = SOCKErrorReturn(errno);
                goto done;
            }
            else if (fds.revents & POLLERR)
            {
                ERROR("Exceptional condition has occured on the socket\n");
                dwLastError = WSAENOTCONN;
                goto done;
            }
            else if (fds.revents & POLLHUP)
            {
                ERROR("Socket was disconnected\n");
                dwLastError = WSAENOTCONN;
                goto done;
            }
            else if (fds.revents & POLLNVAL)
            {
                ERROR("Invalid socket\n");
                dwLastError = WSAENOTSOCK;
                goto done;
            }
            else if (!(fds.revents & POLLRDNORM))
            {
                goto overlapped;
            }
        }

#if SO_TIMEO_NOT_SUPPORTED
        if (!lpOverlapped)
        {
            /* Accordingly to Windows behavior, send and receive timeouts 
               specified through setsockopt are used only for synchronous 
               I/O and ignored in case of overlapped I/O */
            if (mstmo < 0)
            {
                /* mstmo < 0 means that mstmo has not been prefetched
                   therefore we need to get the critical section and
                   access it now */
                SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
                if (SOCK_socket_list.sockets && SOCK_socket_list.sockets[s].rt_data)
                {
                    mstmo = SOCK_socket_list.sockets[s].rt_data->rcv_tmo;
                }
                SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            }

            if (mstmo > 0)
            {
                select_tmo.tv_sec = (long)mstmo / 1000;
                select_tmo.tv_usec = ((long)mstmo % 1000) * 1000;
            }
        }
#endif

recv_again:
#if SO_TIMEO_NOT_SUPPORTED
        if (select_tmo.tv_sec >= 0)
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET((int)s, &readfds);
            
            ret = select (s+1, &readfds, NULL, NULL, &select_tmo);
            if (ret < 0)
            {
                if (errno == EINTR)
                {
                    TRACE("select() in recvfrom() failed with EINTR; re-selecting\n");
                    goto recv_again;
                }
                else
                {
                    TRACE("Unexpected error %d.", errno);
                    dwLastError = SOCKErrorReturn(errno);
                    goto done;
                }
            }
            else if (ret == 0)
            {
                TRACE("WSARecvFrom timed out\n");
                ret = SOCKET_ERROR;
                dwLastError = WSAETIMEDOUT;
                goto done;
            }
        }
#endif

        if (lpFrom)
        {
            /* recvfrom */
            sockfromlen = *lpFromLen;
            ret = recvfrom((int)s, lpBuffers[0].buf, lpBuffers[0].len, 
                           (int) *lpFlags, &UnixSockaddr, &sockfromlen);

            /* 
            While there might be a concern about truncating sockfromlen, which is
            of type socklen_t, to an int, we note that it should not be a problem.
            In case it is, an ASSERT will report it.
            */

            if ((sockfromlen > INT_MAX) && (*lpFromLen > 0))
            {
                ASSERT("sockfromlen should not be greater than INT_MAX.\n");
            }

            *lpFromLen = (int)sockfromlen;

            /*Copy BSD structure to Windows structure*/
            lpFrom->sa_family = UnixSockaddr.sa_family;
            memcpy(lpFrom->sa_data, UnixSockaddr.sa_data, sizeof(UnixSockaddr.sa_data));
        }
        else
        {
            /* recv */
            ret = recv((int)s,
                        lpBuffers[0].buf, lpBuffers[0].len,
                        (int) *lpFlags);
        }

        if (ret < 0)
        {
            if (errno == EINTR)
            {
                TRACE("recv() failed with EINTR; re-recving\n");
                goto recv_again;
            }

            /* we could not process this request in one pass, queue the
               remaining of that request */
            if (errno == EAGAIN && lpOverlapped)
            {
                TRACE("Recv returned EAGAIN, queuing recv operation\n");
                goto overlapped;
            }

            TRACE("Error happened and it wasn't EAGAIN!\n");
            *lpNumberOfBytesRecvd = 0;
            dwLastError = SOCKErrorReturn(errno);
        }
        else
        {
            TRACE("Recv operation completed succesfully. Bytes recvd %d\n", ret);
            if (lpOverlapped)
            {
                pal_iocp_completion_packet *lpIOCPCompletionPacket;

                lpOverlapped->InternalHigh = ret;
                if (SOCKAllocateCompletionPacket(s, lpOverlapped, &lpIOCPCompletionPacket))
                {
                    SOCKProcessOverlappedCompletion(s, lpOverlapped, lpIOCPCompletionPacket);
                }
                else
                {
                    dwLastError = ERROR_NOT_ENOUGH_MEMORY;
                }
            }
            *lpNumberOfBytesRecvd = ret;
            ret = 0;
        }
    }
    else
overlapped:
    {
        ws2_op_recvfrom *recvfrom_op = NULL;

        /* We should never attempt to overlap an operation if no peer is 
           associated with the stream socket */
        if ( SOCKIsSocketConnected(s) <= 0)
        {
            dwLastError = GetLastError();
            goto done;
        }

        recvfrom_op = malloc( sizeof(ws2_op_recvfrom) );
        if ( !recvfrom_op ) 
        {   
            ERROR("Couldn't allocate sendto operation");
            
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            goto done;
        }

        /* 
        This memory is freed in SOCKQUEURemoveBack, called by 
        SOCKProcessOverlappedSend or SOCKDeleteAsyncSocket

        Also, there's ALWAYS only 1 WSABUF structure (see rotor_pal.doc) 
        */
        recvfrom_op->lpBuffers = (WSABUF*) malloc( sizeof(WSABUF) );
        if ( !recvfrom_op->lpBuffers)
        {
            ERROR("Couldn't allocate internal copy of WSABUF structure\n");
            
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            free(recvfrom_op);
            goto done;
        }

        /* initialize Internal and InternalHigh fields of
           WSAOVERLAPPED */
        lpOverlapped->Internal     = 0;
        lpOverlapped->InternalHigh = 0;
    
        /* NB: we don't need to keep dwBufferCount, it's always 1 */
        recvfrom_op->lpBuffers[0].buf     = lpBuffers[0].buf;
        recvfrom_op->lpBuffers[0].len     = lpBuffers[0].len;
        recvfrom_op->dwFlags              = *lpFlags;
        recvfrom_op->lpFrom               = lpFrom;
        recvfrom_op->lpFromLen            = lpFromLen;
        recvfrom_op->lpOverlapped         = lpOverlapped;

        if (!SOCKAllocateCompletionPacket(s, lpOverlapped, &recvfrom_op->lpIOCPCompletionPacket))
        {
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            free(recvfrom_op->lpBuffers);
            free(recvfrom_op);
            goto done;
        }

        SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);

        if (FALSE == 
            SOCKQueuePushBackOperation((int)s, WS2_OP_RECVFROM,
                                        (LPVOID) recvfrom_op ))
        {
            ERROR("couldn't push back operation in overlapped recv queue\n");
           
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            free(recvfrom_op->lpBuffers);
            free(recvfrom_op);
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            goto done;
        }
        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
        
        dwLastError = SOCKSendRequestForAsyncOp(s, WS2_OP_RECVFROM, NULL);

        if (dwLastError)
        {
            ret = SOCKET_ERROR;
            SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
            if (FALSE == 
                SOCKQueueRemoveOperation((int)s, WS2_OP_RECVFROM, recvfrom_op))
            {
                ERROR("couldn't remove operation from queue (memory leak)\n");
            }
            SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
            goto done;
        }

        dwLastError = WSA_IO_PENDING;
        ret = SOCKET_ERROR;
    }

done:
    if (dwLastError) 
    {
        if (lpOverlapped && dwLastError != WSA_IO_PENDING)
        {
            lpOverlapped->Internal     = dwLastError;
            lpOverlapped->InternalHigh = 0;
        }
        SetLastError(dwLastError); 
    }

    /* Restore the original WSABUF and lpFlags. */
    if (SavedWASBUF.len == 0 && !lpOverlapped)
    {
        *lpBuffers = SavedWASBUF;
        if ((SavedFlags & PAL_MSG_PEEK) == 0)
        {
            *lpFlags &= (~PAL_MSG_PEEK);
        }
        if (dwLastError == 0 && *lpNumberOfBytesRecvd == 1)
        {
            *lpNumberOfBytesRecvd = 0;
        }
    }
    LOGEXIT("WSARecvFrom returns int %d\n", ret);
    PERF_EXIT(WSARecvFrom);
    return ret;
}


/*++
Function:
  WSAEventSelect

See MSDN doc.
--*/
int
PALAPI
WSAEventSelect(
           IN PAL_SOCKET s,
           IN WSAEVENT hEventObject,
           IN int lNetworkEvents)
{
    int ret = SOCKET_ERROR;
    DWORD dwLastError = 0;
    

    PERF_ENTRY(WSAEventSelect);
    ENTRY("WSAEventSelect (s=%#x, hEventObject=%p, lNetworkEvent=%#x)\n",
          s, hEventObject, lNetworkEvents);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        SetLastError(WSANOTINITIALISED);
        goto done;
    }

    if(lNetworkEvents && (hEventObject == NULL))
    {
        ERROR("The event object is NULL\n");
        SetLastError(WSAEINVAL);
        goto done;
    }

    /* make the socket non-blocking */
    if ( fcntl( (int)s, F_SETFL, O_NONBLOCK ) == -1 )
    {
        ERROR("Failed to make file descriptor %d non-blocking\n", (int)s);
        if ( errno == EBADF )
        {
            ERROR("%d is a bad file descriptor\n", (int)s);
        }
        SetLastError( WSAENOTSOCK);
        goto done;
    }


    SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);
    
    if (FALSE == 
        SOCKAssignEventObjectToNetworkEvents(
            (int)s, hEventObject, lNetworkEvents))
    {
        SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);
        
        ERROR("Could not assign an event object for network events!\n");
        goto done;
    }
    SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

    TRACE("writing to file descriptor %d to wake up worker thread\n",
          SOCK_PIPE_WRITE);

    dwLastError = SOCKSendRequestForAsyncOp(s, WS2_OP_EVENTSELECT, NULL);

    if (dwLastError)
    {
        SetLastError(dwLastError);
        goto done;
    }

    ret = 0;

done:
    LOGEXIT("WSAEventSelect returns int %d\n", ret);
    PERF_EXIT(WSAEventSelect);
    return ret;
}

/*++
Function:
  WSAEnumNetworkEvents

See MSDN doc.
--*/
int
PALAPI
WSAEnumNetworkEvents(
        IN PAL_SOCKET s,
        IN WSAEVENT hEventObject,
        OUT LPWSANETWORKEVENTS lpNetworkEvents)
{
    DWORD dwLastError = 0;
    struct pollfd fds;
    int    *lpNetEventError;
    int nPollCount;

    PERF_ENTRY(WSAEnumNetworkEvents);
    ENTRY("WSAEnumNetworkEvents (s=%#x, hEventObject=%p,"
          "lpNetworkEvent=%p)\n",
          s, hEventObject, lpNetworkEvents);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        dwLastError = WSANOTINITIALISED;
        goto done;
    }

    if ( !SOCKIsValidSocket(s))
    {
        dwLastError = WSAENOTSOCK;
        goto done;
    }

    if ( IsBadWritePtr(lpNetworkEvents, sizeof(WSANETWORKEVENTS)))
    {
        ERROR("The network event structure is not in a valid part of the user"
              "address space\n");

        if(lpNetworkEvents == NULL)
        {
            dwLastError = WSAEINVAL;
        }
        else
        {
            dwLastError = WSAEFAULT;
        }
        goto done;
    }

    if ( hEventObject != NULL )
    {
        if (FALSE == ResetEvent(hEventObject))
        {
            WARN("Could not reset event %p\n", hEventObject);
        }
    }

    fds.fd      = s;
    fds.events  = POLLOUT;

again:
    nPollCount = poll(&fds, 1, 0);
    lpNetEventError = & (lpNetworkEvents->iErrorCode[FD_CONNECT_BIT]);

    TRACE("poll returned %d fds.revents %d\n", nPollCount, fds.revents);
    if (nPollCount < 0)
    {
        if ( EINTR == errno)
        {
            TRACE("poll() failed with EINTR; re-polling\n");
            goto again;
        }
        dwLastError = SOCKErrorReturn(errno);
        goto done;
    }

    memset(lpNetworkEvents, 0,
           sizeof(WSANETWORKEVENTS));

    if (fds.revents & POLLERR)
    {
        TRACE("Exceptional condition has occured on the socket\n");
        *lpNetEventError = WSAECONNREFUSED;
        lpNetworkEvents->lNetworkEvents |= FD_CONNECT;
        goto done;
    }
    else if (fds.revents & POLLHUP)
    {
        TRACE("Socket was disconnected\n");
        *lpNetEventError = WSAENETUNREACH;
        lpNetworkEvents->lNetworkEvents |= FD_CONNECT;
        goto done;
    }
    else if (fds.revents & POLLNVAL)
    {
        ERROR("Invalid socket. This should not happen since we"
               "checked the validity of the socket before polling\n");
        dwLastError = WSAENOTSOCK;
        goto done;
    }
    else if (fds.revents & POLLOUT)
    {
        if ( SOCKIsSocketConnected(s) > 0)
        {
            /* socket is connected */
            *lpNetEventError = 0;
        }
        else
        {
            /* socket is NOT connected */
            *lpNetEventError = WSAECONNREFUSED;
        }

        lpNetworkEvents->lNetworkEvents |= FD_CONNECT;
    }

done:
    if (dwLastError)
    {
        SetLastError(dwLastError);

        LOGEXIT("WSAEnumNetworkEvents returns int %d\n", SOCKET_ERROR);
        PERF_EXIT(WSAEnumNetworkEvents);
        return SOCKET_ERROR;
    }

    LOGEXIT("WSAEnumNetworkEvents returns int 0\n");
    PERF_EXIT(WSAEnumNetworkEvents);
    return 0;
}

/*++
Function:
  WSASocketA

See MSDN doc.
--*/
PAL_SOCKET
PALAPI
WSASocketA(
       IN int af,
       IN int type,
       IN int protocol,
       LPWSAPROTOCOL_INFOA lpProtocolInfo,
       IN GROUP g,
       IN DWORD dwFlags)
{
    PAL_SOCKET ret = INVALID_SOCKET;

    PERF_ENTRY(WSASocketA);
    ENTRY("WSASocketA (af=%#x, type=%#x, protocol=%#x, lpProtocolInfo=%p, "
          "g=%#x, dwFlags=%#x)\n",
          af, type, protocol, lpProtocolInfo, g, dwFlags);

    if ( lpProtocolInfo )
    {
        ASSERT("lpProtocolInfo is not NULL as it should be\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    ret = WSASocketW(af, type, protocol, NULL, g, dwFlags);

done:
    LOGEXIT("WSASocketA returns SOCKET %d\n", ret);
    PERF_EXIT(WSASocketA);
    return ret;
}

PAL_SOCKET
PALAPI
WSASocketW(
       IN int af,
       IN int type,
       IN int protocol,
       LPWSAPROTOCOL_INFOW lpProtocolInfo,
       IN GROUP g,
       IN DWORD dwFlags)
{
    PAL_SOCKET ret = INVALID_SOCKET;

    PERF_ENTRY(WSASocketW);
    ENTRY("WSASocketW (af=%#x, type=%#x, protocol=%#x, lpProtocolInfo=%p, "
          "g=%#x, dwFlags=%#x)\n",
          af, type, protocol, lpProtocolInfo, g, dwFlags);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        SetLastError(WSANOTINITIALISED);
        goto done;
    }

    if ( dwFlags != WSA_FLAG_OVERLAPPED )
    {
        ASSERT("dwFlags is 0x%x, it should be 0x%x\n", dwFlags,
               WSA_FLAG_OVERLAPPED);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    if ( lpProtocolInfo )
    {
        ASSERT("lpProtocolInfo is not NULL as it should be\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    if ( g )
    {
        ASSERT("GROUP argument g should be 0\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    ret = PAL_socket(af, type, protocol);

done:
    LOGEXIT("WSASocketW returns SOCKET %d\n", ret);
    PERF_EXIT(WSASocketW);
    return ret;
}


/*++
Function:
  WSAIoctl

See MSDN documentation.

Note: This function accepts only the Unix ioctl codes (FIONBIO, FIONREAD,
and SIOCATMARK). Currently none of the winsock2 codes are exposed through
the BCL.
--*/
int
PALAPI
WSAIoctl(
     IN PAL_SOCKET s,
     IN DWORD dwIoControlCode,
     IN LPVOID lpvInBuffer,
     IN DWORD cbInBuffer,
     OUT LPVOID lpvOutBuffer,
     IN DWORD cbOutBuffer,
     OUT LPDWORD lpvbBytesReturned,
     IN LPWSAOVERLAPPED lpOverlapped,
     IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int ret = SOCKET_ERROR;
    BOOL bBehaveLikeNonOverlapped;
    
    PERF_ENTRY(WSAIoctl);
    ENTRY("WSAIoctl (s=%#x, dwIoControlCode=%#x, lpvInBuffer=%p, cbInBuffer=%u,"
          "lpvOutBuffer=%p, cbOutBuffer=%u, lpvbBytesReturned=%p, "
          "lpOverlapped=%p, lpCompletionRoutine=%#x)\n",
          s, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer, cbOutBuffer, 
          lpvbBytesReturned, lpOverlapped, lpCompletionRoutine);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        SetLastError(WSANOTINITIALISED);
        goto done;
    }
   
    /* Notice that sanity tests are always done on every parameter,
       regardless of operation requested.
    */

    /* test input buffer */
    if (IsBadReadPtr(lpvInBuffer, cbInBuffer))
    {
        ERROR("lpvInBuffer is not totally contained in a valid part of the "
               "user address space.\n");
        SetLastError(WSAEFAULT);
        goto done;
    }
  
    /* test output buffer */
    if (IsBadWritePtr(lpvOutBuffer, cbOutBuffer))
    {
       ERROR("lpvOutBuffer is not totally contained in a valid part of the "
              "user address space.\n");
       SetLastError(WSAEFAULT);
       goto done;
    }
    
    /* test output bytes returned */
    if (IsBadWritePtr(lpvbBytesReturned, sizeof(DWORD))) 
    {
       ERROR("lpvbBytesReturned is not totally contained in a valid part "
              "of the user address space.\n");
       SetLastError(WSAEFAULT);
       goto done;
    }
    
    /* test if lpOverlapped argument is valid */
    if (lpOverlapped)
    {
        if (IsBadReadPtr(lpOverlapped, sizeof(WSAOVERLAPPED)))
        {
            ERROR("lpOverlapped parameter is not totally contained in a valid "
                   "part of the user address space\n");
            SetLastError(WSAEFAULT);
            goto done;
        }
    }
    
    /* behave like nonoverlapped when there no notification mechanism 
       available */
    if (!lpCompletionRoutine && 
        (!lpOverlapped || !lpOverlapped->hEvent))
    {
        bBehaveLikeNonOverlapped = TRUE;
    }
    else if (IsBadCodePtr((FARPROC) lpCompletionRoutine))
    {
        ERROR("lpCompletionRoutine parameter is not totally contained "
               "in a valid part of the user address space\n");
        SetLastError(WSAEINVAL);
        goto done;
    }
    else
    {
        bBehaveLikeNonOverlapped = FALSE;
    }
     
    /* test if IOC_VOID is set ( code has no parameters ) */ 
    if ( dwIoControlCode & PAL_IOC_VOID )
    {
        if (dwIoControlCode & PAL_IOC_IN)
        {
            ERROR("code can't be of type IOC_VOID and IOC_IN\n");
            SetLastError(WSAEINVAL);
            goto done;
        }
        
        if (dwIoControlCode & PAL_IOC_OUT )
        {
            ERROR("code can't be of type IOC_VOID and IOC_OUT\n");
            SetLastError(WSAEINVAL);
            goto done;
        }
    }


    /* Check to see if we are receiving a BSD ioctl request, if so,
       it can be handled by ioctlsocket() */
    if ( ((dwIoControlCode >> 27) & IOC_TYPE_MASK) == IOC_TYPE_UNIX)
    {
        /* make sure we pass the good buffer to ioctlsocket.

           FreeBSD ioctl() man page isn't clear whether an operation
           can be both in and out. However, the enumerated operations
           are not in and out. However, this could change in the future */
        if (dwIoControlCode & PAL_IOC_IN)
        {
            ret = ioctlsocket( (int)s, 
                               (unsigned long) dwIoControlCode, 
                               lpvInBuffer );
        }
        else if (dwIoControlCode & PAL_IOC_OUT)
        {
            ret = ioctlsocket( (int)s,
                               (unsigned long) dwIoControlCode,
                               lpvOutBuffer );
        }
        else
        {
            ERROR("UNIX ioctl socket operations of type IOC_VOID not "
                  "supported.\n");
            SetLastError(WSAEINVAL);
            goto done;
        }
    }
    /* If we get here, the IOCTL is of type IOC_WS2, IOC_PROTOCOL or
       IOC_VENDOR. These need to each be handled in a special way - and
       only a small subset is currently supported at all. */
    else if (dwIoControlCode == SIO_MULTIPOINT_LOOPBACK)
    {
        /* This is a request to set the loopback status. We will use 
           setsockopt(IP_MULTICAST_LOOPBACK) */
        if (cbInBuffer < sizeof(BOOL))
        {
            /* If we don't have a properly sized In buffer,
               it's an error! */
            ERROR ("Invalid sized cbInBuffer for SIO_MULTIPOINT_LOOPBACK\n");
            SetLastError(WSAEINVAL);
            goto done;
        }
            
        /* Implement SIO_MULTIPOINT_LOOPBACK using 
           setsockopt(IP_MULTICAST_LOOP) */
        ret = setsockopt (s, IPPROTO_IP, IP_MULTICAST_LOOP, 
                          (u_char*) lpvInBuffer, cbInBuffer);
        if (ret == SOCKET_ERROR)
        {
            /* Last error will be set by setsockopt() */
            ERROR("Error calling setsockopt for SIO_MULTIPOINT_LOOPBACK\n");
            goto done;
        }
    }
    else if(dwIoControlCode == SIO_MULTICAST_SCOPE)
    {
        /* This is a request to set the scope for a broadcast message. 
           This is the same as sock option IP_MULTICAST_TTL */

        if (cbInBuffer < sizeof(int))
        {
            /* If we don't have a valid In buffer or it is not properly 
               sized, it's an error! */
            ERROR ("Invalid or invalid sized lpvInBuffer for SIO_MULTICAST_SCOPE\n");
            SetLastError(WSAEINVAL);
            goto done;
        }
            
        /* Implement SIO_MULTICAST_SCOPE using 
           setsockopt(IP_MULTICAST_TTL) */
        ret = setsockopt (s, IPPROTO_IP, IP_MULTICAST_TTL, 
                          (u_char*) lpvInBuffer, cbInBuffer);
        if (ret == SOCKET_ERROR)
        {
            /* Last error will be set by setsockopt() */
            ERROR("Error calling setsockopt for SIO_MULTICAST_SCOPE errno=%d <%s> %d\n", errno, strerror(errno),
                  *((int*)lpvInBuffer));
            goto done;
        }
        *lpvbBytesReturned = cbInBuffer;
    }
    else if (dwIoControlCode == SIO_GET_BROADCAST_ADDRESS)
    {
        struct ifreq tmpIfReq;
        struct sockaddr *UnixSockaddr;

        if (!lpvOutBuffer || cbOutBuffer < sizeof(struct PAL_sockaddr))
        {
            /* If we don't have a valid Out buffer or it is not properly 
               sized, it's an error! */
            ERROR ("Invalid or invalid sized lpvOutBuffer for SIO_GET_BROADCAST_ADDRESS\n");
            SetLastError(WSAEINVAL);
            goto done;
        }

        ret = ioctl(s, SIOCGIFBRDADDR, &tmpIfReq, sizeof(tmpIfReq));
        if (ret == SOCKET_ERROR)
        {
            /* Last error will be set by setsockopt() */
            ERROR("Error calling ioctl for SIO_BROADCAST_ADDRESS\n");
            goto done;
        }

        /* Convert from the Unix sockaddr type returned to a PAL sockaddr */
        UnixSockaddr = &tmpIfReq.ifr_broadaddr;
        ((struct PAL_sockaddr*)lpvOutBuffer)->sa_family = UnixSockaddr->sa_family;
        memcpy(((struct PAL_sockaddr*)lpvOutBuffer)->sa_data, 
               UnixSockaddr->sa_data, sizeof(((struct PAL_sockaddr*)lpvOutBuffer)->sa_data));

    }
    else if (dwIoControlCode == SIO_KEEPALIVE_VALS)
    {
        /* NOTE: This implementation of SIO_KEEPALIVE_VALS is only approximate -
           it only sets Keepalive for a socket on or off, it doesn't modify
           the keepalivetime or keepaliveinterval */
        BOOL KeepAliveStatus;
        int BoolSize;
        BoolSize = sizeof(BOOL);

        if (cbInBuffer < sizeof(struct tcp_keepalive))
        {
            /* If we don't have a properly sized In buffer,
               it's an error! */
            ERROR ("Invalid sized cbInBuffer for SO_KEEPALIVE\n");
            SetLastError(WSAEINVAL);
            goto done;
        }
            
        /* Implement SIO_KEEPALIVE_VALS using 
           setsockopt(SO_KEEPALIVE) */
        KeepAliveStatus = ((struct tcp_keepalive*)lpvInBuffer)->onoff;   
        ret = setsockopt (s, SOL_SOCKET, SO_KEEPALIVE, 
                          &KeepAliveStatus, BoolSize);
        if (ret == SOCKET_ERROR)
        {
            /* Last error will be set by setsockopt() */
            ERROR("Error calling setsockopt for SO_KEEPALIVE\n");
            goto done;
        }
    }
    else if (dwIoControlCode == SIO_FLUSH)
    {
        if (FALSE == SOCKFlushOverlappedOperationQueues(s, TRUE)) 
        {
           ret = SOCKET_ERROR;
           ERROR("Error flushing pending overlapped operations on socket\n");
           goto done;
        }
    }
    else
    {
        /* This is a request for an IOCTL that we don't support - return
           an error */
        ERROR("dwIoControlCode of %#x is not supported.\n", dwIoControlCode);
        SetLastError(WSAEINVAL);
        goto done;
    }
    
    
done:
    LOGEXIT("WSAIoctl returns int %d\n", ret);
    PERF_EXIT(WSAIoctl);
    return ret;
}


/*++
Function:
  ioctlsocket

See MSDN documentation.
--*/
int
PALAPI
ioctlsocket(
        IN PAL_SOCKET s,
        IN int cmd,
        IN OUT PAL_u_long FAR *argp)
{
    int ret = SOCKET_ERROR;

    PERF_ENTRY(ioctlsocket);
    ENTRY("ioctlsocket (s=%#x, cmd=%#x, argp=%p)\n",
          s, cmd, argp);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        SetLastError(WSANOTINITIALISED);
        goto done;
    }

    switch (cmd)
    {
        case PAL_FIONREAD:
            cmd = FIONREAD;
            break;
        case PAL_FIONBIO:
            cmd = FIONBIO;
            break;
        default:
            ASSERT("cmd %d invalid\n", cmd);
            SetLastError(WSAEFAULT);
            goto done;
            break;
    }

    /*check if the argp param is a valid part
      of user address space*/
    if (IsBadWritePtr(argp,sizeof(PAL_u_long)))
    {
        ERROR("arg pointer is not totally contained in a valid"
               "part of the user address space.\n");
        SetLastError(WSAEFAULT);
        goto done;
    }

    /* MSDN docs don't say that ioctlsocket will SetLastError the
       same as WSAIoctl, but tests show that it will */
    ret = ioctl( s, (DWORD)cmd, (LPVOID)argp);
    if ( ret == -1 )
    {
        ERROR("ioctl failed errno %d\n", errno);
        ret = SOCKET_ERROR;

        switch(errno)
        {
        case EBADF:
            SetLastError(WSAENOTSOCK);
            break;
        case EFAULT:
            SetLastError(WSAEFAULT);
            break;
        case EINVAL:
            /* fall through */
        default:
            SetLastError(WSAEINVAL);
            break;
        }
    }

done:
    LOGEXIT("ioctlsocket returns int %d\n", ret);
    PERF_EXIT(ioctlsocket);
    return ret;
}

/*++
Function:
  WSAGetOverlappedResult

See MSDN doc.
--*/
BOOL
PALAPI
WSAGetOverlappedResult(
    IN PAL_SOCKET s,
    IN LPWSAOVERLAPPED lpOverlapped,
    OUT LPDWORD lpcbTransfer,
    IN BOOL fWait,
    OUT LPDWORD lpdwFlags)
{
    BOOL Ret = FALSE;

    PERF_ENTRY(WSAGetOverlappedResult);
    ENTRY("WSAGetOverlappedResult(s=%p, lpOverlapped=%p, "
           "lpcbTransfer=%p, fWait=%d, lpdwFlags=%p)\n", s, 
           lpOverlapped, lpcbTransfer, fWait, lpdwFlags);

    if ( SOCK_startup_count == 0 )
    {
        ERROR("No successful WSAStartup call\n");
        SetLastError(WSANOTINITIALISED);
        goto WSAGetOverlappedResultExit;
    }

    if ( SOCKIsValidSocket(s) == FALSE )
    {
        ERROR("Socket %d is not a valid socket\n", s);
        SetLastError(WSAENOTSOCK);
        goto WSAGetOverlappedResultExit;
    }

    if (fWait)
    {
        if (lpOverlapped->hEvent != NULL)
        {
            DWORD waitResult;

            waitResult = WaitForSingleObject(lpOverlapped->hEvent, INFINITE);

        

            if (waitResult != WAIT_OBJECT_0)

            {
                ASSERT("Event was not signalled on socket %d\n", s);
                goto WSAGetOverlappedResultExit;
            }
            if (lpOverlapped->Internal == 0)
            {
                *lpcbTransfer = lpOverlapped->InternalHigh;
                Ret = TRUE;
            }
            else
            {
                SetLastError(lpOverlapped->Internal);
            }
        }
        else
        {
            SetLastError(ERROR_INVALID_HANDLE);
        }
    }
    else
    {
        if (lpOverlapped->Internal == 0 && lpOverlapped->InternalHigh == 0)
        {
            SetLastError(WSA_IO_INCOMPLETE);
        }
        else if (lpOverlapped->Internal != 0)
        {
            SetLastError(lpOverlapped->Internal);
        }
        else
        {
            *lpcbTransfer = lpOverlapped->InternalHigh;
            Ret = TRUE;
        }
    }
WSAGetOverlappedResultExit:
    LOGEXIT("WSAGetOverlappedResult returns BOOL %d\n", Ret);
    PERF_EXIT(WSAGetOverlappedResult);
    return Ret;
}

/*++
Function:
  SOCKInitWinSock

Initialization function called from PAL_Initialize.
Initializes the global critical section and allocates a TLS index.
--*/
BOOL
SOCKInitWinSock(void)
{
    int ret;
    TRACE("Initializing WinSock2...\n");
    if (0 != SYNCInitializeCriticalSection( &SOCK_crit_section))
    {
        ERROR("sockets critical section initialization failed\n");
        return FALSE;
    }
    
    if (0 != SYNCInitializeCriticalSection(&SOCK_list_crit_section))
    {
        ERROR("Sockets lists and queues critical section initialization failed\n");
        return FALSE;
    }

    ret = SOCKGetCurrMaxOpenFiles();
    if (ret > 0)
    {
        SOCK_table_size = ret;
        TRACE("SOCK_table_size initialized to %d\n", SOCK_table_size);
    }
    else
    {
        /* Failed to get current max number of open files: let's keep 
           the default initialization value  */
        WARN("Failed to get current max number of open files: "
             "keeping the defualt value %d for SOCK_table_size\n",
             SOCK_table_size);
    }

    return TRUE;
}

/*++
Function:
  SOCKTerminateWinSock

Termination function called from PAL_Terminate to delete the global
critical section.
--*/
void SOCKTerminateWinSock(void)
{
    TRACE("Terminating WinSock2...\n");
    SYNCEnterCriticalSection( &SOCK_crit_section , TRUE);
    if ( SOCK_startup_count > 0 )
    {
        WARN("Not enough calls to WSACleanup\n");
        /* Reset anyway SOCK_startup_count to 0 so any 
           further socket API call will fail */
        SOCK_startup_count = 0;
        SOCKCommonCleanup(TRUE);
    }

    SYNCLeaveCriticalSection( &SOCK_crit_section , TRUE);
    DeleteCriticalSection( &SOCK_crit_section );
}



/*++
Function:
  SOCKCommonCleanup

Utility function to cleanup worker thread, etc., when the startup count
reaches 0. Called from WSACleanup and SOCKTerminateWinSock. The calling
function should lock the global critical section before calling this
function and unlock it afterwards. If the parameter dontwait is TRUE, 
SOCKCommonCleanup does not wait for worker thread shutdown to be 
completed.
--*/
static void SOCKCommonCleanup(BOOL dontwait)
{
    int i;
    BOOL trouble = FALSE;
    BOOL stopped;
    
    stopped = SOCKStopWorkerThread(dontwait ? 0 : INFINITE);

    if (!stopped)
    {
        ERROR("Failed stopping worker thread\n");
        trouble = TRUE;
    }

    if ( CloseHandle( SOCK_worker_thread_handle ) == FALSE )
    {
        WARN("Unable to reclaim worker thread handle\n");
        trouble = TRUE;
        SOCK_worker_thread_handle = INVALID_HANDLE_VALUE;
    }

    /* close the pipe */
    for( i = 0; i <= 1; ++i )
    {
        if ( close(SOCK_pipe_fd[i]) != 0 )
        {
            WARN("Could not close file descriptor %d\n", SOCK_pipe_fd[i]);
            trouble = TRUE;
        }
        SOCK_pipe_fd[i] = -1;
    }

    SYNCEnterCriticalSection(&SOCK_list_crit_section, TRUE);

    while (SOCK_socket_list.max_fd != -1) 
    {
        SOCKDeleteAsyncSocket(SOCK_socket_list.max_fd, TRUE);
    }    
    
    if (stopped)
    {
        ws2_sock * socks = NULL;

        if (SOCK_socket_list.pollfds) 
        {
            free (SOCK_socket_list.pollfds);
            SOCK_socket_list.pollfds = NULL;
        }

        socks = (ws2_sock *)SOCK_socket_list.sockets;
        SOCK_socket_list.sockets = NULL;

        if (socks)
        {
            for (i=0; i<SOCK_table_size; i++)
            {
                if (socks[i].rt_data)
                {
                    free ((void *)socks[i].rt_data);
                    socks[i].rt_data = NULL;
                }
            }
            free (socks);
        }
    }

    SYNCLeaveCriticalSection(&SOCK_list_crit_section, TRUE);

    if (stopped)
    {
        int rtn;

        DeleteCriticalSection( &SOCK_list_crit_section );

        rtn = pthread_mutex_lock(&SOCK_iocp.completion_packet_list_mutex);
        if (rtn != 0)
        {
            WARN("pthread_mutex_lock failed with %d\n", rtn);
            trouble = TRUE;
            goto SOCKCommonCleanupExit;
        }

        while (!IsListEmpty(&SOCK_iocp.completion_packet_list))
        {
            LIST_ENTRY *pEntry;
            pal_iocp_completion_packet *pStatus;
            pEntry = RemoveHeadList(&SOCK_iocp.completion_packet_list);
            pStatus = CONTAINING_RECORD(pEntry, pal_iocp_completion_packet, Link);
            free(pStatus);
            SOCK_iocp.completion_packet_list_length--;
        }

        rtn = pthread_mutex_unlock(&SOCK_iocp.completion_packet_list_mutex);
        if (rtn != 0)
        {
            WARN("pthread_mutex_unlock failed with %d\n", rtn);
            trouble = TRUE;
            goto SOCKCommonCleanupExit;
        }

        pthread_mutex_destroy(&SOCK_iocp.completion_packet_list_mutex);
        pthread_cond_destroy(&SOCK_iocp.completion_packet_list_not_empty);
    }

SOCKCommonCleanupExit:
    if ( trouble )
    {
        WARN("Socket cleanup was messy\n");
    }
}


/*++
Function:
  SOCKCancelAsyncOperations

Send a message down the pipe to cancel all asynchronous operations on
the given socket. Return value is the same as closesocket -- 0 if
successful, SOCKET_ERROR otherwise.
--*/
int SOCKCancelAsyncOperations( PAL_SOCKET s )
{
    int ret = 0;
    HANDLE hEvent = NULL;
    
    /* allocate an event which will be signalled when all operations
       have been cancelled */
    hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if ( hEvent == NULL )
    {
        ERROR("CreateEvent failed, unable to cancel any overlapped or "
              "asynchronous operations on socket %d\n", s);
        ret = SOCKET_ERROR;
        goto done;
    }
    
    if (SOCKSendRequestForAsyncOp(s, WS2_OP_CLOSESOCKET, hEvent))
    {
        ret = SOCKET_ERROR;
        goto done;
    }

    if ( WaitForSingleObject( hEvent, 10000 ) != WAIT_OBJECT_0 )
    {
        ASSERT("Event was not signalled, unable to cancel any overlapped "
               "or asynchronous operations on socket %d\n", s);
        ret = SOCKET_ERROR;
    }

done:
    
    if ( hEvent && CloseHandle(hEvent) == FALSE )
    {
        WARN("Could not free event\n");
    }

    return ret;
}

/*++
Function:
  SOCKStopWorkerThread

Send a message down the pipe to tell the worker thread to terminate.

Parameters:
  tmo: timeout in milliseconds waiting for the worker thread to exit
Return value:
  TRUE in case of success,
  FALSE if it fails or it times out and tmo is > 0
--*/
BOOL SOCKStopWorkerThread( DWORD tmo )
{
    BOOL ret = TRUE;
    DWORD err = 0;

    if (SOCKSendRequestForAsyncOp(0, WS2_OP_STOPTHREAD, NULL))
    {
        WARN("Could not stop worker thread\n");
        ret = FALSE;
    }
    else
    {
        /* write successful; now wait for thread to terminate itself. */
        err = WaitForSingleObject( SOCK_worker_thread_handle, tmo );
        if ((WAIT_OBJECT_0 != err) &&
            ((WAIT_TIMEOUT != err) || (0 != tmo)))
        {
            ASSERT("Failed to wait on worker thread's handle (%p)\n",
                   SOCK_worker_thread_handle);
            ret = FALSE;
        }
    }
    
    /* SOCK_worker_thread_handle is closed in SOCKCommonCleanup */
    return ret;
}


/*++
Function:
  SOCKIsValidSocket

Return true if the given socket is valid, false if not.
--*/
BOOL SOCKIsValidSocket( PAL_SOCKET s )
{
#ifdef _DEBUG
    fd_set test_set;
    int    res;
    struct timeval time = { 0, 0 };
#endif

    if ((INVALID_SOCKET == s) || (s < 0) || (s >= SOCK_table_size))
    {
        return FALSE;
    }

#ifdef _DEBUG
    FD_ZERO(&test_set);
    if (FD_SET_BOOL((int)s, &test_set) == FALSE)
    {
        ERROR("Invalid socket value\n");
        return FALSE;
    }

again:

    res = select( (int)s + 1, NULL, &test_set, NULL, &time );
    if ( res == -1 )
    {
        if (errno == EINTR)
        {
            TRACE("select() failed with EINTR; re-selecting\n");
            goto again;
        }
        else if ( errno == EBADF )
        {
            ERROR("Socket %d is an invalid file descriptor\n", s);
        }
        else
        {
            ASSERT("Unknown error from select(errno=%d)\n", errno);
        }
        return FALSE;
    }
#endif

    return TRUE;
}

/*++
Function:
  SOCKFreePALHostentFields

Free the fields of a PAL_hostent.  This does not free the hostent itself.
--*/
void SOCKFreePALHostentFields(struct PAL_hostent *hostent)
{
    int i;
    
    free(hostent->h_name);
    hostent->h_name = NULL;
    if (hostent->h_aliases != NULL)
    {
        for(i = 0; hostent->h_aliases[i] != NULL; i++)
        {
            free(hostent->h_aliases[i]);
        }
        free(hostent->h_aliases);
    }
    hostent->h_aliases = NULL;
    if (hostent->h_addr_list != NULL)
    {
        for(i = 0; hostent->h_addr_list[i] != NULL; i++)
        {
            free(hostent->h_addr_list[i]);
        }
        free(hostent->h_addr_list);
    }
    hostent->h_addr_list = NULL;
}

/*++
Function:
  SOCKIsSocketConnected

Detects if socket s is actually connect()ed or not,
by using an 'innocent' function.

1  = connect()ed or N/A
0  = Not
-1 = Error
--*/
int SOCKIsSocketConnected( PAL_SOCKET s )
{
    struct sockaddr sock;
#if HAVE_SOCKLEN_T
    socklen_t sockaddr_size;
#else   /* HAVE_SOCKLEN_T */
    int sockaddr_size;
#endif  /* HAVE_SOCKLEN_T */
    int socket_type;
    socklen_t sizeof_int;
    
    sockaddr_size = sizeof(struct sockaddr);
    sizeof_int = sizeof(int);

    /*
       First, we need to get the socket type...
    */
    if ( (getsockopt(s, SOL_SOCKET, SO_TYPE, &socket_type, &sizeof_int)) < 0)
    {
        SetLastError(SOCKErrorReturn(errno));
        ERROR("getsockopt failed for socket %d\n", s);
        return(-1);
    }
    
    /* We only need to check if the socket is connected first if
       we AREN'T either SOCK_RAW or SOCK_DGRAM 
       
       NOTE: This list could change
    */
  
    if (socket_type != SOCK_RAW && socket_type != SOCK_DGRAM)
    {
        /* Finally, our innocent function. This one should work ONLY
           if we are connected, otherwise error. */
        if ( (getpeername(s, &sock, &sockaddr_size)) < 0)
        {
            SetLastError(SOCKErrorReturn(errno));
            TRACE("Socket %d is not connect()ed\n", s);
            return(0);
        }
    }
    
    
    /* Either we are connect()ed, or are an inappropriate socket type.
       Return 1. (for connected / N/A) */
    return (1); 
}



/*++
Function:
  SOCKSendRequestForAsyncOp

Send a request to the worker thread.


Return value:
    if error: the "LastError" is returned
    otherwise: 0 is returned.

--*/
DWORD SOCKSendRequestForAsyncOp( PAL_SOCKET s, enum ws2_opcode opcode, 
                                 HANDLE hEvent)
{
    ws2_op sock_op;

    DWORD dwLastError = 0;

    sock_op.opcode = opcode;
    sock_op.s      = s;
    sock_op.event  = hEvent;
    
    /* write the request to the pipe */
    if ( write(SOCK_PIPE_WRITE, &sock_op, sizeof(sock_op)) != 
         sizeof(sock_op) )
    {
        ERROR("failed to write %d bytes to pipe to wake up worker "
              "thread\n", sizeof(sock_op));
        dwLastError = ERROR_BROKEN_PIPE;
        goto done;
    }

done:
    return dwLastError;
}

/*++
Function:
  SOCKGetCurrMaxOpenFiles

  Returns the current max number of file descriptors 
  for the current process 

Return value:
  Max number of open files, or -1 in case of error

--*/
static long SOCKGetCurrMaxOpenFiles(void)
{
    long ret = -1;
    int grlret;
    long scret;
    struct rlimit rl;

    /* Both sysconf and getrlimit are somehow unreliable depending on the 
       specific Unix systems. Sysconf is POSIX, getrlimit is not guarantee to 
       be portable.
       On AIX the initial rlim_max is equal to 0x7FFFFFFF. Since PAL 
       initialization anyway increases rlim_cur to rlim_max, rlim_cur becomes 
       0x7FFFFFFF too. Anyway the real max number of open files appears to be 
       MIN(rlim_cur,sysconf(_SC_OPEN_MAX)), while the values stored in rlim_cur
       and rlim_max are allowed to be greater than that limit.
       On Hp-UX the max number of open files is limited also by maxfiles_lim 
       (hard file limit per process, other than rlim_max), nfile (max number of 
       open files, other than rlim_cur) and ninode (max number of open inodes);
       the value returned by SOCKGetCurrMaxOpenFiles on Hp-UX may be not 
       accurate, but it is anyway greater or equal to the real limit.
       On MacOSX the value returned by getrlimit for rlim_max is initialized to
       a very large number until the first time setrlimit is called, then it 
       gets reset to the real limit. 
       Both on FreeBSD and MacOSX sysconf(_SC_OPEN_MAX) is documented to return
       the maximum number of open files per user id, rather than per process.

       The best portable guess across systems seems to be 
       MIN(rlim_cur, sysconf(_SC_OPEN_MAX))
    */

    scret = sysconf(_SC_OPEN_MAX);
    grlret = getrlimit(RLIMIT_NOFILE, &rl);
    
    if ((grlret == 0) && (scret > 0))
    {
        /* Both getrlimit and sysconf succeeded 
           Let's use the smaller value */
        long rlimcur = (long)rl.rlim_cur;
        if (rlimcur <= 0)
            ret = scret;
        else
            ret = (scret < rlimcur) ? scret : rlimcur;
    }
    else if ((grlret != 0) && (scret <= 0))
    {
        /* Both getrlimit and sysconf failed */
        ERROR("Failed to determine max number of open files");
    }
    else if (grlret != 0)
    {
        /* getrlimit failed: use sysconf result */
        ret = scret;
    }
    else if (scret <= 0)
    {
        /* sysconf failed or returned 0: use getrlimit result */
        ret = (long)rl.rlim_cur;
        if (ret <= 0)
            ret = -1;
    }

    return ret;
}
