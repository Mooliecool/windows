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
    using System.Runtime.InteropServices;

    /*
    NGenHint is not supported in Whidbey     

    [Serializable]
    public enum NGenHint
    {    
        Default             = 0x0000, // No preference specified
        
        Eager               = 0x0001, // NGen at install time
        Lazy                = 0x0002, // NGen after install time
        Never               = 0x0003, // Assembly should not be ngened      
    }
    */

    [Serializable]
    public enum LoadHint
    {
        Default             = 0x0000, // No preference specified
        
        Always              = 0x0001, // Dependency is always loaded
        Sometimes           = 0x0002, // Dependency is sometimes loaded
        //Never               = 0x0003, // Dependency is never loaded
    }

    /*
    NGenAttribute is not supported in Whidbey    

    [Serializable, AttributeUsage(AttributeTargets.Assembly)]  
    public sealed class NGenAttribute : Attribute 
    {
        private NGenHint hint;
    
        public NGenAttribute (
            NGenHint hintArgument
            )
        {
            hint = hintArgument;
        }
        
        public NGenHint NGenHint
        {
            get
            {
                return hint;
            }
        }       
    }
    */

    [Serializable, AttributeUsage(AttributeTargets.Assembly)]  
    public sealed class DefaultDependencyAttribute : Attribute 
    {
        private LoadHint loadHint;
    
        public DefaultDependencyAttribute (
            LoadHint loadHintArgument
            )
        {
            loadHint = loadHintArgument;
        }  
    
        public LoadHint LoadHint
        {
            get
            {
                return loadHint;
            }
        }       
    } 


    [Serializable, AttributeUsage(AttributeTargets.Assembly, AllowMultiple = true)]  
    public sealed class DependencyAttribute : Attribute 
    {
        private String                dependentAssembly;
        private LoadHint              loadHint;

        public DependencyAttribute (
            String   dependentAssemblyArgument,
            LoadHint loadHintArgument
            )
        {
            dependentAssembly     = dependentAssemblyArgument;
            loadHint              = loadHintArgument;
        }
        
        public String DependentAssembly
        {
            get
            {
                return dependentAssembly;
            }
        }       

        public LoadHint LoadHint
        {
            get
            {
                return loadHint;
            }
        }       
    }
}

