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
namespace System.Runtime.Serialization.Formatters.Binary
{
    using System;
    using System.Collections;
    using System.Runtime.Remoting.Messaging;
    using System.Reflection;


    [Serializable()]     
    internal sealed class BinaryMethodCallMessage
    {
        Object[] _inargs = null;
        String _methodName = null;
        String _typeName = null;
        Object _methodSignature = null;

        Type[] _instArgs = null;
        Object[] _args = null;
        LogicalCallContext _logicalCallContext = null;

        Object[] _properties = null;

        internal BinaryMethodCallMessage(String uri, String methodName, String typeName, Type[] instArgs, Object[] args, Object methodSignature, LogicalCallContext callContext, Object[] properties)
        {
            _methodName = methodName;
            _typeName = typeName;
            //_uri = uri;
            if (args == null)
                args = new Object[0];

            _inargs = args;
            _args = args;
            _instArgs = instArgs;
            _methodSignature = methodSignature;
            if (callContext == null)
                _logicalCallContext = new LogicalCallContext();
            else
                _logicalCallContext = callContext;

            _properties = properties;

        }

        public String MethodName
        {
            get {return _methodName;}
        }

        public String TypeName
        {
            get {return _typeName;}
        }


        public Type[] InstantiationArgs
        {
            get {return _instArgs;}
        }
        
        public Object MethodSignature
        {
            get {return _methodSignature;}
        }

        public Object[] Args
        {
            get {return _args;}
        }

        public LogicalCallContext LogicalCallContext
        {
            get {return _logicalCallContext;}
        }

        public bool HasProperties
        {
            get {return (_properties != null);}
        }

        internal void PopulateMessageProperties(IDictionary dict)
        {
            foreach (DictionaryEntry de in _properties)
            {
                dict[de.Key] = de.Value;
            }
        }

    }


    [Serializable()]     
    internal class BinaryMethodReturnMessage
    {
        Object[] _outargs = null;
        Exception _exception = null;
        Object _returnValue = null;

        Object[] _args = null;
        LogicalCallContext _logicalCallContext = null;

        Object[] _properties = null;

        internal BinaryMethodReturnMessage(Object returnValue, Object[] args, Exception e, LogicalCallContext callContext, Object[] properties)
        {
            _returnValue = returnValue;
            if (args == null)
                args = new Object[0];

            _outargs = args;
            _args= args;
            _exception = e;

            if (callContext == null)
                _logicalCallContext = new LogicalCallContext();
            else
                _logicalCallContext = callContext;
            
            _properties = properties;
        }

        public Exception Exception
        {
            get {return _exception;}
        }

        public Object  ReturnValue
        {
            get {return _returnValue;}
        }
        
        public Object[] Args
        {
            get {return _args;}
        }

        public LogicalCallContext LogicalCallContext
        {
            get {return _logicalCallContext;}
        }

        public bool HasProperties
        {
            get {return (_properties != null);}
        }

        internal void PopulateMessageProperties(IDictionary dict)
        {
            foreach (DictionaryEntry de in _properties)
            {
                dict[de.Key] = de.Value;
            }
        }
    }
}
    
