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

//  File:      ProxyWithReturnMessage.cs


using System;
using System.Collections;
using System.Threading;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Proxies;
using System.Runtime.Remoting.Messaging;

namespace Microsoft.Samples
{
    //
    // MyProxy extends the CLR Remoting RealProxy.
    // This demonstrate the RealProxy extensiblity.
    // 
    //
    public class MyProxy : RealProxy
    {
        public MyProxy(Type type)
        : base(type)
        {
            // This constructor forwards the call to base RealProxy.
            // RealProxy uses the Type to generate a transparent proxy
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

    //
    // The class used to obtain Metadata
    //
    public class Test : MarshalByRefObject
    {
        public int Method1(String str, double f, int i)
        {
            Console.WriteLine("Test.Method1 {0} {1} {2}", str, f, i);
            return 0;
        }
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

            Console.WriteLine("Generate a new MyProxy using the Type");
            MyProxy myProxy = new MyProxy(typeof(Test));        

            Console.WriteLine("Obtain the transparent proxy from myProxy");
            Test test = (Test)myProxy.GetTransparentProxy();

            Console.WriteLine("Calling the Proxy");
            int i2 = test.Method1("String1", 1.2, 6);

            Console.WriteLine("Checking result");
            if (5 == i2)
            {
                Console.WriteLine("Test.Method1 PASSED : returned {0}", i2);
                ret = 0;
            }
            else
            {
                Console.WriteLine("Test.Method1 FAILED : returned {0}", i2);
                ret = -1;
            }

            Console.WriteLine("Sample Done");
            return ret;
        }
    }

}

