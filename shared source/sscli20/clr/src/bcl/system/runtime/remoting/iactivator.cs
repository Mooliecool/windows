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
** File:    IActivator.cs
**
**
** Purpose: Defines the interface provided by activation services
**          
**
**
===========================================================*/
namespace System.Runtime.Remoting.Activation {

    using System;
    using System.Runtime.Remoting.Messaging;
    using System.Collections;
    using System.Security.Permissions;
    
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IActivator
    {
        // return the next activator in the chain
        IActivator NextActivator 
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	    get; 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	    set;
	}
        
        // New method for activators.
	[SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
        IConstructionReturnMessage Activate(IConstructionCallMessage msg);     

           // Returns the level at which this activator is active ..
           // Should return one of the ActivatorLevels below
        ActivatorLevel Level 
	    { 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	    get;
	    }

           
    }

	[Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum ActivatorLevel
    {
        Construction = 4,
        Context = 8,
        AppDomain = 12,
        Process = 16,
        Machine = 20
    }

    [System.Runtime.InteropServices.ComVisible(true)]
    public interface IConstructionCallMessage : IMethodCallMessage
    {
        IActivator Activator                   
        { 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	    get;
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	    set;
	}
        Object[] CallSiteActivationAttributes  
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	     get;
	}
        String ActivationTypeName               
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	    get;
        }
        Type ActivationType                     
        { 
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	    get;
        }
        IList ContextProperties                
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
	     get;
        }
    }
    
    [System.Runtime.InteropServices.ComVisible(true)]
    public interface IConstructionReturnMessage : IMethodReturnMessage
    {
    }
    
}
