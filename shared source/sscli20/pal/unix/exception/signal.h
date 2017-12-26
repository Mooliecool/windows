
/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    exception/signal.h

Abstract:
    Private signal handling utilities for SEH

--*/

#ifndef _PAL_SIGNAL_H_
#define _PAL_SIGNAL_H_

#if !HAVE_MACH_EXCEPTIONS

/*++
Function :
    SEHInitializeSignals

    Set-up signal handlers to catch signals and translate them to exceptions

    (no parameters, no return value)
--*/
void SEHInitializeSignals(void);

/*++
Function :
    SEHCleanupSignals

    Restore default signal handlers

    (no parameters, no return value)
--*/
void SEHCleanupSignals(void);

#if (__GNUC__ > 3 ||                                            \
     (__GNUC__ == 3 && __GNUC_MINOR__ > 2))
// For gcc > 3.2, sjlj exceptions semantics are no longer available
// Therefore we need to hijack out of signal handlers before second pass
#define HIJACK_ON_SIGNAL 1
#endif

#endif // !HAVE_MACH_EXCEPTIONS

#endif /* _PAL_SIGNAL_H_ */

