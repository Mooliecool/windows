//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------

//  File:      ProxyWithCustomCastings.cs

using System;
using System.Collections;
using System.Threading;
using System.Reflection;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Proxies;
using System.Runtime.Remoting.Messaging;
using System.Security.Permissions;

namespace Microsoft.Samples
{
    //
    // MyProxy extends the CLR Remoting RealProxy.
    // This demonstrate the RealProxy extensiblity.
    // 
    //
    public class MyProxy : RealProxy, IRemotingTypeInfo
    {
        Type[] _types;

        // This constructor forwards the call to base RealProxy.
        // RealProxy uses the Type to generate a transparent proxy
        public MyProxy(Type type)
        : base(type)
        {
        }

        public MyProxy(Type type, Type[] types)
        : base(type)
        {
            _types = types;
        }

        // Obtain the fully qualified name of the type that the proxy represents
        public String TypeName 
        {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
            get
            {
                return GetProxiedType().ToString();
            }
            
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
            set
            {
                throw new NotSupportedException();
            }
        }

        // Check whether we can cast the transparent proxy to the given type
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        public bool CanCastTo(Type castType, Object o)
        {
          Console.WriteLine("CanCastTo {0}", castType);
          bool canCast = false;

          foreach (Type type in _types)
          {
              if (castType == type)
              {
                  canCast = true;
                  break;
              }
          }

          return canCast;
        }

        public override IMessage Invoke(IMessage msg)
        {
            Console.WriteLine("MyProxy.Invoke Start");
            Console.WriteLine("");

            if (msg is IMethodCallMessage)
                Console.WriteLine("IMethodCallMessage");

            if (msg is IMethodReturnMessage)
                Console.WriteLine("IMethodReturnMessage");

            Type msgType = msg.GetType();
            Console.WriteLine("Message Type: {0}", msgType.ToString());

            Console.WriteLine("Message Properties");
            IDictionary d = msg.Properties;
            IDictionaryEnumerator e = (IDictionaryEnumerator) d.GetEnumerator();

            while (e.MoveNext())
            {
                Object key = e.Key;
                String keyName = key.ToString();
                Object value = e.Value;

                Console.WriteLine("\t{0} : {1}", keyName, e.Value);
                if (keyName == "__Args")
                {
                    Object[] args = (Object[])value;
                    for (int a = 0; a < args.Length; a++)
                        Console.WriteLine("\t\targ: {0} value: {1}", a, args[a]);
                }

                if ((keyName == "__MethodSignature") && (null != value))
                {
                    Object[] args = (Object[])value;
                    for (int a = 0; a < args.Length; a++)
                        Console.WriteLine("\t\targ: {0} value: {1}", a, args[a]);
                }

            }

            // Build Return Message
            Object ret = 5;
            Object[] outArgs = null;

            ReturnMessage retMsg = new ReturnMessage(
                                        ret,           //Object ret
                                        outArgs,       //Object[] outArgs
                                        0,             //int outArgsCount
                                        null,          //LogicalCallContext callCtx
                                        (IMethodCallMessage)msg   //IMethodCallMessage mcm
                                        );


            Console.WriteLine("MyProxy.Invoke - Finish");

            return retMsg;
        }
    }

    public interface IFaq
    {
        int MethodX(int i, String s);
    }

    public interface IBaz
    {
        int MethodY(double d);
    }

    //
    // Main class that drives the whole sample
    //
    public class ProxySample
    {

        public static int Main(String[] args)
        {
            int ret=0;
            Console.WriteLine("CLR Remoting Sample: Custom Proxy");

            Type[] types = new Type[2];
            types[0] = typeof(IFaq);
            types[1] = typeof(IBaz);

            Console.WriteLine("Generate a new MyProxy using the Type");

            MyProxy myProxy = new MyProxy(typeof(MarshalByRefObject), types );        

            Console.WriteLine("Obtain the transparent proxy from myProxy");
            MarshalByRefObject mbr = (MarshalByRefObject)myProxy.GetTransparentProxy();

            IFaq faq = (IFaq)mbr;
            int r = faq.MethodX(5, "hi");
            Console.WriteLine("{0}", r);

            IBaz baz = (IBaz)faq;
            int r2 = baz.MethodY(123.45);
            Console.WriteLine("{0}", r);

            Console.WriteLine("Sample Done");
            return ret;
        }
    }
}


