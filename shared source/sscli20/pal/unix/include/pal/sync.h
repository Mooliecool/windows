/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/sync.h

Abstract:
    
    Header file for the event functions.
    
--*/

#ifndef _PAL_SYNC_H_
#define _PAL_SYNC_H_

/*++
Function:
  EventLocalToRemote

Parameters, 
    IN handle_data         : structure associated with this handle
    IN remote_handle_data  : structure associated with the remote handle

Return value : TRUE if functions succeeded, FALSE otherwise.

--*/
BOOL EventLocalToRemote(HOBJSTRUCT* handle_data, 
                        HREMOTEOBJSTRUCT *remote_handle_data);
/*++
Function :
    EventRemoteToLocal

    Creates a even handle using a Remote Handle from another process

Parameters, 
    IN remote_handle_data  : structure associated with the remote handle
    
Returns
    Handle to the instantiated event object

--*/
HANDLE EventRemoteToLocal(HREMOTEOBJSTRUCT *remote_handle_data);

#endif // _PAL_SYNC_H_
