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
** File:    IMethodMessage.cs
**
**
** Purpose: Defines the message object interface
**
**
===========================================================*/
namespace System.Runtime.Remoting.Messaging {
    using System;
    using System.Reflection;
    using System.Security.Permissions;
    using IList = System.Collections.IList;
    
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IMethodMessage : IMessage
    {
        String Uri                      
        {
             [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]      
             get;
        }
        String MethodName               
        {
             [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]      
             get;
        }
        String TypeName     
        {
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
        get;
        }
        Object MethodSignature
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
            get; 
        }
       
        int ArgCount
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
            get;
        }
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
        String GetArgName(int index);
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
        Object GetArg(int argNum);
        Object[] Args
        {
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
         get;
        }

        bool HasVarArgs
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
             get;
        }
        LogicalCallContext LogicalCallContext
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
            get;
        }

        // This is never actually put on the wire, it is
        // simply used to cache the method base after it's
        // looked up once.
        MethodBase MethodBase           
        {
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
         get;
        }
    }
    
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IMethodCallMessage : IMethodMessage
    {
        int InArgCount
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
            get;
        }
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
        String GetInArgName(int index);
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
        Object GetInArg(int argNum);
        Object[] InArgs
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
            get;
        }
    }

[System.Runtime.InteropServices.ComVisible(true)]
    public interface IMethodReturnMessage : IMethodMessage
    {
        int OutArgCount                
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
             get;
        }
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
        String GetOutArgName(int index);
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
        Object GetOutArg(int argNum);
        Object[]  OutArgs         
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
             get;
        }
        
        Exception Exception        
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
             get;
        }
        Object    ReturnValue 
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]       
             get;
        }
    }

}
