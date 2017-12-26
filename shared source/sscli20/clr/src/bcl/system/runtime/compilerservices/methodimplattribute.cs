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
namespace System.Runtime.CompilerServices {
    
    using System;
    using System.Reflection;    
    
    // This Enum matchs the miImpl flags defined in corhdr.h. It is used to specify 
    // certain method properties.
    
    [Serializable]
    [Flags]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum MethodImplOptions
    {
        Unmanaged       =   System.Reflection.MethodImplAttributes.Unmanaged,
        ForwardRef      =   System.Reflection.MethodImplAttributes.ForwardRef,
        PreserveSig     =   System.Reflection.MethodImplAttributes.PreserveSig,
        InternalCall    =   System.Reflection.MethodImplAttributes.InternalCall,
        Synchronized    =   System.Reflection.MethodImplAttributes.Synchronized,
        NoInlining      =   System.Reflection.MethodImplAttributes.NoInlining,
        // **** If you add something, update internal MethodImplAttribute(MethodImplAttributes methodImplAttributes)! ****
	}

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum MethodCodeType
    {
        IL              =   System.Reflection.MethodImplAttributes.IL,
        Native          =   System.Reflection.MethodImplAttributes.Native,
        /// <internalonly/>
        OPTIL           =   System.Reflection.MethodImplAttributes.OPTIL,
        Runtime         =   System.Reflection.MethodImplAttributes.Runtime  
    }

    // Custom attribute to specify additional method properties.
    [Serializable, AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor, Inherited = false)] 
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class MethodImplAttribute : Attribute  
    {    
        internal MethodImplOptions  _val;
        public   MethodCodeType     MethodCodeType;

        internal MethodImplAttribute(MethodImplAttributes methodImplAttributes)
        {
            MethodImplOptions all = 
                MethodImplOptions.Unmanaged | MethodImplOptions.ForwardRef | MethodImplOptions.PreserveSig | 
                MethodImplOptions.InternalCall | MethodImplOptions.Synchronized | MethodImplOptions.NoInlining;
            _val = ((MethodImplOptions)methodImplAttributes) & all;
        }
        
        public MethodImplAttribute(MethodImplOptions methodImplOptions)
        {
            _val = methodImplOptions;
        }
        
        public MethodImplAttribute(short value)
        {
            _val = (MethodImplOptions)value;
        }
        
        public MethodImplAttribute()
        {
        }
        
        public MethodImplOptions Value { get {return _val;} }   
    }

}
