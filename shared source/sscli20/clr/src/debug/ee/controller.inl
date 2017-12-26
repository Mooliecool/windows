// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
//*****************************************************************************
// File: controller.inl
//
// Inline definitions for the Left-Side of the CLR debugging services
// This is logically part of the header file. 
//
//*****************************************************************************

#ifndef CONTROLLER_INL_
#define CONTROLLER_INL_

inline BOOL DebuggerControllerPatch::IsBreakpointPatch()
{
    return (controller->GetDCType() == DEBUGGER_CONTROLLER_BREAKPOINT);
}

inline BOOL DebuggerControllerPatch::IsStepperPatch() 
{ 
    return (controller->IsStepperDCType()); 
}

inline DebuggerPatchKind DebuggerControllerPatch::GetKind()
{
    return kind;
}
inline BOOL DebuggerControllerPatch::IsILMasterPatch()
{
    LEAF_CONTRACT;

    return (kind == PATCH_KIND_IL_MASTER);
}

inline BOOL DebuggerControllerPatch::IsILSlavePatch()
{
    LEAF_CONTRACT;

    return (kind == PATCH_KIND_IL_SLAVE);
}

inline BOOL DebuggerControllerPatch::IsManagedPatch()
{
    return (IsILMasterPatch() || IsILSlavePatch() || kind == PATCH_KIND_NATIVE_MANAGED);

}
inline BOOL DebuggerControllerPatch::IsNativePatch()
{
    return (kind == PATCH_KIND_NATIVE_MANAGED || kind == PATCH_KIND_NATIVE_UNMANAGED || (IsILSlavePatch() && !offsetIsIL));

}

#endif  // CONTROLLER_INL_
