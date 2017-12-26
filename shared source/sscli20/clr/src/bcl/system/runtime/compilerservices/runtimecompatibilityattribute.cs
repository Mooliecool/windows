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
/*=============================================================================
**
** Class: RuntimeCompatibilityAttribute
**
**
** Purpose: Mark up the program to indicate various legacy or new opt-in behaviors.
**
**
=============================================================================*/

namespace System.Runtime.CompilerServices 
{

    using System;

    [Serializable, AttributeUsage(AttributeTargets.Assembly, Inherited=false, AllowMultiple=false)]
    public sealed class RuntimeCompatibilityAttribute : Attribute 
    {
        // fields
        private bool m_wrapNonExceptionThrows;

        // constructors
        public RuntimeCompatibilityAttribute() {
            // legacy behavior is the default, and m_wrapNonExceptionThrows is implicitly
            // false thanks to the CLR's guarantee of zeroed memory.
        }

        // properties

        // If a non-CLSCompliant exception (i.e. one that doesn't derive from System.Exception) is
        // thrown, should it be wrapped up in a System.Runtime.CompilerServices.RuntimeWrappedException
        // instance when presented to catch handlers?
        public bool WrapNonExceptionThrows { 
            get { 
                return m_wrapNonExceptionThrows; 
            }
            set {
                m_wrapNonExceptionThrows = value;
            }
        }
    }
}
