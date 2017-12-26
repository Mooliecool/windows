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
using System;

// This enumeration defines the access modes for a dynamic assembly.
// EE uses these enum values..look for m_dwDynamicAssemblyAccess in Assembly.hpp

namespace System.Reflection.Emit 
{    
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    [Flags]
    public enum AssemblyBuilderAccess
    {
        Run = 1,
        Save = 2,
        RunAndSave = Run | Save,
        ReflectionOnly = 6, // 4 | Save
    }
}
