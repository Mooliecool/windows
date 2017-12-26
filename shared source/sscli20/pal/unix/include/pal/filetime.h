/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/filetime.h

Abstract:
    Header file for utility functions having to do with file times.

Revision History:

--*/

#ifndef _PAL_FILETIME_H_
#define _PAL_FILETIME_H_

/* Provide consistent access to nanosecond fields, if they exist. */
#if HAVE_STAT_TIMESPEC

#define ST_ATIME_NSEC(statstruct) ((statstruct)->st_atimespec.tv_nsec)
#define ST_MTIME_NSEC(statstruct) ((statstruct)->st_mtimespec.tv_nsec)
#define ST_CTIME_NSEC(statstruct) ((statstruct)->st_ctimespec.tv_nsec)

#else /* HAVE_STAT_TIMESPEC */
#if HAVE_STAT_NSEC

#define ST_ATIME_NSEC(statstruct) ((statstruct)->st_atimensec)
#define ST_MTIME_NSEC(statstruct) ((statstruct)->st_mtimensec)
#define ST_CTIME_NSEC(statstruct) ((statstruct)->st_ctimensec)

#else /* HAVE_STAT_NSEC */

#define ST_ATIME_NSEC(statstruct) 0
#define ST_MTIME_NSEC(statstruct) 0
#define ST_CTIME_NSEC(statstruct) 0

#endif /* HAVE_STAT_NSEC */
#endif /* HAVE_STAT_TIMESPEC */


FILETIME FILEUnixTimeToFileTime( time_t sec, long nsec );
time_t FILEFileTimeToUnixTime( FILETIME FileTime, long *nsec );

#endif /* _PAL_FILE_H_ */





