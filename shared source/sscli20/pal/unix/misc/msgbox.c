/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    msgbox.c

Abstract:

    Implementation of Message Box.

--*/

#include <syslog.h>
#include "pal/palinternal.h"
#include "pal/critsect.h"
#include "pal/dbgmsg.h"


SET_DEFAULT_DEBUG_CHANNEL(MISC);

CRITICAL_SECTION msgbox_critsec;


/*++
Function :
    MsgBoxInitialize

    Initialize the critical sections.

Return value:
    TRUE if initialize succeeded
    FALSE otherwise

--*/
BOOL
MsgBoxInitialize( void )
{
    TRACE( "Initialising the critical section.\n" );
    if (0 != SYNCInitializeCriticalSection( &msgbox_critsec ))
    {
        ERROR("Could not initialize MsgBox critical section!\n");
        return FALSE;
    }

    return TRUE;
}

/*++
Function :
    MsgBoxCleanup

    Deletes the critical sections.

--*/
void MsgBoxCleanup( void )
{
    TRACE( "Deleting the critical section.\n" );
    DeleteCriticalSection( &msgbox_critsec );
}





/*++
Function:
  MessageBoxW

This is a small subset of MessageBox that simply logs a message to the
system logging facility and returns. A typical log entry will look
like:

May 23 15:48:10 rice example1: MessageBox: Caption: Error Text

Note:
  hWnd should always be NULL.

See MSDN doc.
--*/
int
PALAPI
MessageBoxW(
	    IN LPVOID hWnd,
	    IN LPCWSTR lpText,
	    IN LPCWSTR lpCaption,
	    IN UINT uType)
{
    CHAR *text = NULL;
    CHAR *caption = NULL;
    INT len = 0;
    INT rc = 0;

    PERF_ENTRY(MessageBoxW);
    ENTRY( "MessageBoxW (hWnd=%p, lpText=%p (%S), lpCaption=%p (%S), uType=%#x)\n",
           hWnd, lpText?lpText:W16_NULLSTRING, lpText?lpText:W16_NULLSTRING,
           lpCaption?lpCaption:W16_NULLSTRING,
           lpCaption?lpCaption:W16_NULLSTRING, uType );

    if (hWnd != NULL)
    {
        ASSERT("hWnd != NULL");
    }
    
    if(lpText)
    {
        len = WideCharToMultiByte(CP_ACP, 0, lpText, -1, NULL, 0, NULL, NULL);
        if(len)
        {
            text = (LPSTR)malloc(len);
            if(!text)
            {
                ERROR("malloc() failed!\n");
                SetLastError( ERROR_NOT_ENOUGH_MEMORY );
                goto error;
            }
            if( !WideCharToMultiByte( CP_ACP, 0, lpText, -1, text, len, 
                                      NULL, NULL))
            {
                ASSERT("WideCharToMultiByte failure\n");
                SetLastError( ERROR_INTERNAL_ERROR );
                goto error;
            }
        }
        else
        {
            ASSERT("WideCharToMultiByte failure\n");
            SetLastError( ERROR_INTERNAL_ERROR );
            goto error;
        }
    }
    else
    {
        WARN("No message text\n");

        if (NULL == (text = strdup("(no message text)")))
        {
            ASSERT("strdup() failed\n");
            SetLastError( ERROR_INTERNAL_ERROR );
            goto error;
        }
    }
    if (lpCaption)
    {
        len = WideCharToMultiByte( CP_ACP, 0, lpCaption, -1, NULL, 0, 
                                   NULL, NULL);
        if(len)
        {
            caption = (CHAR*)malloc(len);
            if(!caption)
            {
                ERROR("malloc() failed!\n");
                SetLastError( ERROR_NOT_ENOUGH_MEMORY );
                goto error;
            }
            if( !WideCharToMultiByte( CP_ACP, 0, lpCaption, -1, caption, len,
                                      NULL, NULL))
            {
                ASSERT("WideCharToMultiByte failure\n");
                SetLastError( ERROR_INTERNAL_ERROR );
                goto error;
            }
        }
        else
        {
            ASSERT("WideCharToMultiByte failure\n");
            SetLastError( ERROR_INTERNAL_ERROR );
            goto error;
        }
    }
    else
    {
        if (NULL == (caption = strdup("Error")))
        {
            ERROR("strdup() failed\n");
            SetLastError( ERROR_NOT_ENOUGH_MEMORY );
            goto error;
        }
    }

    SYNCEnterCriticalSection( &msgbox_critsec, TRUE );

    fprintf ( stderr, "MessageBox: %s: %s", caption, text );
    syslog(LOG_USER|LOG_ERR, "MessageBox: %s: %s", caption, text);

    SYNCLeaveCriticalSection( &msgbox_critsec, TRUE );
error:
    free(caption);
    free(text);

    /* return a status based on the type of button */
    switch(uType & MB_TYPEMASK)
    {
    case MB_OK:
        rc = IDOK;
        break;

    case MB_ABORTRETRYIGNORE:
        rc = IDABORT;
        break;

    case MB_YESNO:
        rc = IDNO;
        break;

    case MB_OKCANCEL :
        rc = IDCANCEL;
        break;

    case MB_RETRYCANCEL :
        rc = IDCANCEL;
        break;

    default:
        ASSERT("Bad uType");
        rc = IDOK;
        break;
    }

    LOGEXIT("MessageBoxW returns %d\n", rc);
    PERF_EXIT(MessageBoxW);
    return rc;
}
