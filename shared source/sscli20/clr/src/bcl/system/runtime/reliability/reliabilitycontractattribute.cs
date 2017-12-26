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
** Class:  ReliabilityContractAttribute
**
**
** Purpose: Defines a publically documentable contract for 
** reliability between a method and its callers, expressing
** what state will remain consistent in the presence of 
** failures (ie async exceptions like thread abort) and whether
** the method needs to be called from within a CER.
**
**
===========================================================*/
using System.Runtime.InteropServices;

namespace System.Runtime.ConstrainedExecution {

    // **************************************************************************************************************************
    //
    // Note that if you change either of the enums below or the constructors, fields or properties of the custom attribute itself
    // you must also change the logic and definitions in vm\ConstrainedExecutionRegion.cpp to match.
    //
    // **************************************************************************************************************************

    [Serializable]
    public enum Consistency : int
    {
        MayCorruptProcess   = 0,
        MayCorruptAppDomain = 1,
        MayCorruptInstance  = 2,
        WillNotCorruptState = 3,
	}

    [Serializable]
    public enum Cer : int
    {
        None                = 0,
        MayFail             = 1,  // Might fail, but the method will say it failed
        Success             = 2,
    }

    [AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Constructor | AttributeTargets.Method | AttributeTargets.Interface /* | AttributeTargets.Delegate*/, Inherited = false)]
    public sealed class ReliabilityContractAttribute : Attribute
    {
        private Consistency _consistency;
        private Cer _cer;

        public ReliabilityContractAttribute(Consistency consistencyGuarantee, Cer cer)
        {
            _consistency = consistencyGuarantee;
            _cer = cer;
        }

        public Consistency ConsistencyGuarantee {
            get { return _consistency; }
        }

        public Cer Cer {
            get { return _cer; }
        }
    }
}
