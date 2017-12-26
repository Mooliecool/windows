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
/*=============================================================================
**
** File: AppDomainAttributes
**
**
** Purpose: For AppDomain-related custom attributes.
**
**
=============================================================================*/

namespace System {

    [Serializable()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum LoaderOptimization 
    {
        NotSpecified            = 0,
        SingleDomain            = 1,
        MultiDomain             = 2,
        MultiDomainHost         = 3,

        [Obsolete("This method has been deprecated. Please use Assembly.Load() instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        DomainMask              = 3,

        [Obsolete("This method has been deprecated. Please use Assembly.Load() instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        DisallowBindings        = 4           
    }

    [AttributeUsage (AttributeTargets.Method)]  
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class LoaderOptimizationAttribute : Attribute
    {
        internal byte _val;

        public LoaderOptimizationAttribute(byte value)
        {
            _val = value;
        }
        public LoaderOptimizationAttribute(LoaderOptimization value)
        {
            _val = (byte) value;
        }
        public LoaderOptimization Value 
        {  get {return (LoaderOptimization) _val;} }
    }
}

