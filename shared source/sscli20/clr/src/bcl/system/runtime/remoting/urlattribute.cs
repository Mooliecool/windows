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
** File:    UrlAttribute.cs
**
**                                             
**
** Purpose: Defines an attribute which can be used at the callsite to
**          specify the URL at which the activation will happen.
**
** Date:    March 30, 2000
**
===========================================================*/
namespace System.Runtime.Remoting.Activation {
    using System.Runtime.Remoting;
    using System.Runtime.Remoting.Contexts;
    using System.Runtime.Remoting.Messaging;
    using System.Security.Permissions;	
    using System;
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class UrlAttribute : ContextAttribute
    {
        private String url;
        private static String propertyName = "UrlAttribute";

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
        public UrlAttribute(String callsiteURL) :base(propertyName)
        {
            if(null == callsiteURL)
            {
                // Invalid arg
                throw new ArgumentNullException("callsiteURL");
            }
            url = callsiteURL;
        }        


#pragma warning disable 688  // link demand on override        
        // Object::Equals
        // Override the default implementation which just compares the names
        public override bool Equals(Object o)
        {
            return (o is IContextProperty) && (o is UrlAttribute) && 
                   (((UrlAttribute)o).UrlValue.Equals(url));
        }

        public override int GetHashCode()
        {
            return this.url.GetHashCode();
        }
#pragma warning disable
        
        // Override ContextAttribute's implementation of IContextAttribute::IsContextOK
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
[System.Runtime.InteropServices.ComVisible(true)]
        public override bool IsContextOK(Context ctx, IConstructionCallMessage msg)
        {
            return false;
        }
    
        // Override ContextAttribute's impl. of IContextAttribute::GetPropForNewCtx
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
[System.Runtime.InteropServices.ComVisible(true)]
        public override void GetPropertiesForNewContext(IConstructionCallMessage ctorMsg)
        {
            // We are not interested in contributing any properties to the
            // new context since the only purpose of this property is to force
            // the creation of the context and the server object inside it at
            // the specified URL.
            return;
        }
        
        public String UrlValue
        {
	    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
            get { return url; }            
        }
    }
} // namespace

