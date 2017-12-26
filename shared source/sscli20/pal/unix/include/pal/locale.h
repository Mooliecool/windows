/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    locale.h

Abstract:

    Prototypes for codepage initialization, and control of the readwrite locks
    for systems that use them.

Revision History:

--*/

#ifndef _PAL_LOCALE_H_
#define _PAL_LOCALE_H_

#if HAVE_LOWERCASE_ISO_NAME
#define ISO_NAME(region, encoding, part)  region ".iso" encoding part
#elif HAVE_UNDERSCORE_ISO_NAME
#define ISO_NAME(region, encoding, part)  region ".ISO_" encoding "-" part
#else
#define ISO_NAME(region, encoding, part)  region ".ISO" encoding "-" part
#endif

#if HAVE_CFSTRING
#include <CoreFoundation/CoreFoundation.h>
#endif  // HAVE_CFSTRING

#if HAVE_CFSTRING
typedef
struct _CP_MAPPING
{
    UINT                nCodePage;      /* Code page identifier. */
    CFStringEncoding    nCFEncoding;    /* The equivalent CFString encoding. */
    UINT                nMaxByteSize;   /* The max byte size of any character. */
    BYTE                LeadByte[ MAX_LEADBYTES ];  /* The lead byte array. */
} CP_MAPPING;
#elif HAVE_PTHREAD_RWLOCK_T
typedef 
struct _CP_MAPPING
{
    UINT    nCodePage;                  // Code page identifier.
    LPSTR   lpBSDEquivalent;            // The equivalent BSD locale identifier.
    UINT    nMaxByteSize;               // The max byte size of any character.
    BYTE    LeadByte[ MAX_LEADBYTES ];  // The lead byte array.
} CP_MAPPING;
#else
#error Insufficient platform support for text encodings
#endif

BOOL CODEPAGEInit( void );
void CODEPAGECleanup( void );

const CP_MAPPING * CODEPAGEGetData( IN UINT CodePage );

#if HAVE_CFSTRING
CFStringEncoding CODEPAGECPToCFStringEncoding(UINT codepage);
#else /* HAVE_CFSTRING */
BOOL CODEPAGEAcquireReadLock(void);
BOOL CODEPAGEAcquireWriteLock(void);
BOOL CODEPAGEReleaseLock(void);
#endif  /* HAVE_CFSTRING */

#endif /* _PAL_LOCALE_H_ */
