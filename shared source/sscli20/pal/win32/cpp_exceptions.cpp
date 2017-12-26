/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


--*/

#include "rotor_pal.h"

extern "C"
LONG PAL_RunFilters(PEXCEPTION_POINTERS pException) {
    PPAL_EXCEPTION_REGISTRATION pRegistration = PAL_GetBottommostRegistration();
    LONG filterResult = EXCEPTION_CONTINUE_SEARCH;
    BOOL foundHandler= FALSE;

    while ((pRegistration != NULL) && !foundHandler) {
        char *thrownTypeStr = NULL;

        switch (pRegistration->typeOfHandler) {
            case PALExceptFilter:
                filterResult = (pRegistration->Handler)
                    (pException, pRegistration->pvFilterParameter);

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

extern "C"
void Cpp_RaiseException(PEXCEPTION_RECORD old_record) {
    EXCEPTION_RECORD *record = new EXCEPTION_RECORD();
    
    // Copy record over
    memcpy (record, old_record, sizeof(EXCEPTION_RECORD));

    PAL_GOOD(throw) record;
}
