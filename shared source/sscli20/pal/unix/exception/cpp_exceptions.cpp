/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


--*/

extern "C" {
#include "pal/palinternal.h"

extern void SEHSetSafeState(BOOL state);
extern EXCEPTION_RECORD * SEHGetCurrentException(void);

LONG PAL_RunFilters(PEXCEPTION_POINTERS pException) {
    PPAL_EXCEPTION_REGISTRATION pRegistration = PAL_GetBottommostRegistration();
    LONG filterResult = EXCEPTION_CONTINUE_SEARCH;
    BOOL foundHandler= FALSE;

    while ((pRegistration != NULL) && !foundHandler) {
        char *thrownTypeStr = NULL;

        switch (pRegistration->typeOfHandler) {
            case PALExceptFilter:
                // If we already called this filter (because of CppEHUnwindFilter),
                // no need to call it again.
                if (pRegistration->dwFlags & PAL_EXCEPTION_FLAGS_FILTERCALLED) {
                    break;
                }

                /* since we're calling the application handler,
                   let's reset to safe state so exception in
                   the filter could be handled properly */
                SEHSetSafeState(TRUE);

                filterResult = (pRegistration->Handler)
                    (pException, pRegistration->pvFilterParameter);

                SEHSetSafeState(FALSE);

                switch (filterResult) {
                    case EXCEPTION_EXECUTE_HANDLER:
                        pRegistration->dwFlags |= PAL_EXCEPTION_FLAGS_UNWINDTARGET;
                        foundHandler = TRUE;
                        break;

                    case EXCEPTION_CONTINUE_SEARCH:
                        pRegistration->dwFlags &= ~PAL_EXCEPTION_FLAGS_UNWINDTARGET;
                        break;

                    case EXCEPTION_CONTINUE_EXECUTION:
			            foundHandler = true;
			            break;
                }
                break;

            case PALExcept:
		        filterResult = (LONG)(SIZE_T)pRegistration->Handler;

                switch (filterResult) {
                    case EXCEPTION_EXECUTE_HANDLER:
                        pRegistration->dwFlags |= PAL_EXCEPTION_FLAGS_UNWINDTARGET;
                        foundHandler = TRUE;
                        break;

                    case EXCEPTION_CONTINUE_SEARCH:
                        pRegistration->dwFlags &= ~PAL_EXCEPTION_FLAGS_UNWINDTARGET;
                        break;

                    case EXCEPTION_CONTINUE_EXECUTION:
			            foundHandler = TRUE;
			            break;
                 }
                break;

            case PALFinally:
                break;

            case CatchTyped:
	            // The exception code must be right
		        if (pException->ExceptionRecord->ExceptionCode != PAL_CPP_EXCEPTION_CODE)
		            break;

                // Type thrown must end in "Exception *"
                thrownTypeStr = (char *) pException->ExceptionRecord->ExceptionInformation[0];
                
                if (strcmp(thrownTypeStr + strlen(thrownTypeStr) - strlen("Exception *"),
                           "Exception *") != 0) {
                    pRegistration->dwFlags &= ~PAL_EXCEPTION_FLAGS_UNWINDTARGET;
                    break;
                }

                // fallthru

            case CatchAll:
                pRegistration->dwFlags |= PAL_EXCEPTION_FLAGS_UNWINDTARGET;
                foundHandler = TRUE;
		        filterResult = EXCEPTION_EXECUTE_HANDLER;
                break;
        }

        if (!foundHandler)
            pRegistration = pRegistration->Next;
    }

    return filterResult;
};

void Cpp_RaiseException() {
    EXCEPTION_RECORD *record = new EXCEPTION_RECORD();
    EXCEPTION_RECORD *old_record = SEHGetCurrentException();
    
    // Copy record over
    memcpy (record, old_record, sizeof(EXCEPTION_RECORD));

    PAL_GOOD(throw) record;
}

} // extern "C"
