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
/*============================================================
**
** Class:  PrePrepareMethodAttribute
**
** Purpose: Serves as a hint to ngen that the decorated method
** (and its statically determinable call graph) should be
** prepared (as for Constrained Execution Region use). This
** is primarily useful in the scenario where the method in
** question will be prepared explicitly at runtime and the
** author of the method knows this and wishes to avoid the
** overhead of runtime preparation.
**
** Date:  December 18, 2003
**
===========================================================*/
using System.Runtime.InteropServices;

namespace System.Runtime.ConstrainedExecution
{
    [AttributeUsage(AttributeTargets.Constructor | AttributeTargets.Method, Inherited = false)]

    public sealed class PrePrepareMethodAttribute : Attribute
    {
        public PrePrepareMethodAttribute()
        {
        }
    }
}
