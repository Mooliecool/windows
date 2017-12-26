//  ==++==
//
//    
//     Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//    
//     The use and distribution terms for this software are contained in the file
//     named license.txt, which can be found in the root of this distribution.
//     By using this software in any fashion, you are agreeing to be bound by the
//     terms of this license.
//    
//     You must not remove this notice, or any other, from this software.
//    
//
//  ==--==
//

TEXT_SECTION

//  void __stdcall SWITCH_helper(void)
ASMFUNC(SWITCH_helper)
    lwz   r3, 0(r1)        // limit
    lwz   r4, 4(r1)	  // index
    addi  r1, r1, 8        // remove the arguments from the stack
    cmplw 0, r3, r4
    bge	  LOCAL_LABEL(UseLimit)
    mr    r4, r3
LOCAL_LABEL(UseLimit):	
    mflr  r3               // each b 'target' instruction is 4 bytes
    slwi  r4, r4, 2        // rlwinm r4, r4, 60, 0, 31
    add   r3, r3, r4	
    mtlr  r3	
    blr
