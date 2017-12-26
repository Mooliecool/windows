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
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace System.Runtime.CompilerServices 
{

    using System;

    /// IMPORTANT: Keep this in sync with corhdr.h
    [Serializable, Flags]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum CompilationRelaxations : int
    { 
        NoStringInterning       = 0x0008, // Start in 0x0008, we had other non public flags in this enum before,
                                          // so we'll start here just in case somebody used them. This flag is only
                                          // valid when set for Assemblies.
    };
        
    [Serializable, AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Module | AttributeTargets.Class | AttributeTargets.Method)]  
[System.Runtime.InteropServices.ComVisible(true)]
    public class CompilationRelaxationsAttribute : Attribute 
    {
        private int m_relaxations;      // The relaxations.
        
        public CompilationRelaxationsAttribute (
            int relaxations) 
        { 
            m_relaxations = relaxations; 
        }
        
        public CompilationRelaxationsAttribute (
            CompilationRelaxations relaxations) 
        { 
            m_relaxations = (int) relaxations; 
        }
        
        public int CompilationRelaxations
        { 
            get 
            { 
                return m_relaxations; 
            } 
        }
    }
    
}
