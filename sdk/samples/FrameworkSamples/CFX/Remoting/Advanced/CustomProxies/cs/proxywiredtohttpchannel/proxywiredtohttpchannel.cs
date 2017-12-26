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

//  File:      ProxyWiredToHttpChannel.cs


using System;
using System.Collections;
using System.Threading;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Http;
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
    public class MyProxy : RealProxy
    {
		String _url;
		String _objectURI;
		IMessageSink _messageSink;

        public MyProxy(Type type, String url)
        : base(type)
        {
            // This constructor forwards the call to base RealProxy.
            // RealProxy uses the Type to generate a transparent proxy
			_url = url;

            IChannel[] registeredChannels = ChannelServices.RegisteredChannels;

			foreach (IChannel channel in registeredChannels )
			{
			  if (channel is IChannelSender)
			  {
				IChannelSender channelSender = (IChannelSender)channel;

				_messageSink = channelSender.CreateMessageSink(_url, null, out _objectURI);
				if (_messageSink != null)
					break;
			  }
			}

			if (_messageSink == null)
			{
				throw new Exception("A supported channel could not be found for url:"+ _url);
			}
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
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

			Console.WriteLine("url {0} object URI{1}",
							  _url,
							  _objectURI);

			d["__Uri"] = _url;
			Console.WriteLine("URI {0}", d["__URI"]);

			IMessage retMsg = _messageSink.SyncProcessMessage(msg);

			if (retMsg is IMethodReturnMessage)
			{
				IMethodReturnMessage mrm = (IMethodReturnMessage)retMsg;
			}


            Console.WriteLine("MyProxy.Invoke - Finish");

            return retMsg;
        }
    }

    //
    // Main class that drives the whole sample
    //
    public static class ProxySample
    {
        public static int Main(String[] args)
        {
			ChannelServices.RegisterChannel(new HttpChannel(), false /*ensureSecurity*/);

            int ret=0;
            Console.WriteLine("CLR Remoting Sample: Custom Proxy");

            Console.WriteLine("Generate a new MyProxy using the Type");
		Type type = typeof(HelloService);
		String url = "http://localhost/RemotingHello/HelloService.soap";
            MyProxy myProxy = new MyProxy(type, url);        

            Console.WriteLine("Obtain the transparent proxy from myProxy");
            HelloService helloService = (HelloService)myProxy.GetTransparentProxy();

            Console.WriteLine("Calling the Proxy");
            try
            {
                String str = helloService.ReturnGreeting("bill");

                Console.WriteLine("Checking result : {0}", str);

                if (str == "Hi there bill, you are using .NET Remoting")
                {
                    Console.WriteLine("helloService.HelloMethod PASSED : returned {0}", str);
                    ret = 0;
                }
                else
                {
                    Console.WriteLine("helloService.HelloMethod FAILED : returned {0}", str);
                    ret = -1;
                }
            }

            catch ( System.Net.WebException e)
            {
                 Console.WriteLine("\nHandled Exception!!!\nPlease view the sample documentation in order to properly configure IIS to run this sample.\n");
                 Console.WriteLine(e.Message);
            }
            catch( System.Runtime.Remoting.RemotingException e)
            {
                Console.WriteLine("\nHandled Exception!!!\nPlease view the sample documentation in order to properly configure IIS to run this sample.\n");
                Console.WriteLine(e.Message);
            }

            Console.WriteLine("Sample Done");
            return ret;
        }
    }
}

