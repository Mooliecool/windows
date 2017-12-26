//------------------------------------------------------------------------------
// <copyright file="server.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.Threading;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Messaging;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Tcp;
using System.Runtime.Remoting.Channels.Http;

namespace RemotingSamples 
{
  /// <summary>
  /// Main entry point class.
  /// </summary>
  public class Server
  {
    public const int HTTPPORT = 8085;
    public const int TCPPORT = 8086;

    private static CallingConvention m_CallConv = CallingConvention.ByRef;
    private static ChannelKind m_ChanKind = ChannelKind.TCP;
    private static WellKnownObjectMode m_ObjMode = WellKnownObjectMode.SingleCall;


    public static void Main(string [] args)
    {
      Usage();
      GetOptions();
      Type ObjType = null;

      if (m_CallConv == CallingConvention.ByRef)
        ObjType = Type.GetType("RemotingSamples.HelloServerByRef,remotingshared");
      else
        ObjType = Type.GetType("RemotingSamples.HelloServerByVal,remotingshared");

      Server.Run(m_ChanKind, m_ObjMode, ObjType);
    }//Main()

    private static void Usage()
    {
      Console.WriteLine(
        "===============================================================\n" +
        "*  This is the server remoting sample application.            *\n" +
        "*  Use with the client application to use remoting.           *\n" +
        "*                                                             *\n" +
        "*  Start with an instance of the server in one console and    *\n" +
        "*  run another instance as client in another console.         *\n" +
        "*                                                             *\n" +
        "*  You must match the type of object (ByRef or ByVal) between *\n" +
        "*  the client and server or an exception will occur           *\n" +    
        "*                                                             *\n" + 
        "*  The input values are:                                      *\n" + 
        "*     - Channel: use HTTP, TCP or both.                       *\n" +
        "*                                                             *\n" + 
        "*     - ObjectMode: SingleCall or Singleton                   *\n" +
        "*                   SingleCall destroys on each call          *\n" +
        "*                   SingleTon keeps the object alive          *\n" +
        "*                                                             *\n" + 
        "*     - Calling Convention: by reference or by value          *\n" +
        "*           The server needs to know this to register         *\n" +
        "*           the correct type.                                 *\n" +
        "===============================================================\n");
    }//Usage()

    /// <summary>
    /// Determines whether to act as client or server and executes the relevant method.
    /// </summary>
    
    private static void GetOptions()
    {
GetChannel:
      Console.Write("\nHttp (h), TCP (t) or both (b)? ");
      string Reply = Console.ReadLine();
      switch (Reply.ToLower())
      {
        case "b":
          m_ChanKind = ChannelKind.Both;
          break;
        case "h":
          m_ChanKind = ChannelKind.Http;
          break;
        case "t":
          m_ChanKind = ChannelKind.TCP;
          break;
        default:
          Console.WriteLine("Invalid option, please try again.");
          goto GetChannel;
      }//switch

GetObjectMode:
        Console.Write("\nSingleCall (sc) or Singleton (st)? ");
      Reply = Console.ReadLine();
      switch (Reply.ToLower())
      {
        case "sc":
          m_ObjMode = WellKnownObjectMode.SingleCall;
          break;
        case "st":
          m_ObjMode = WellKnownObjectMode.Singleton;
          break;
        default:
          Console.WriteLine("Invalid option, please try again.");
          goto GetObjectMode;
      }//switch

GetConvention:
      Console.Write("\nBy ref (r) or by val (v)? ");
      Reply = Console.ReadLine();
      switch (Reply.ToLower())
      {
        case "r":
          m_CallConv = CallingConvention.ByRef;
          break;
        case "v":
          m_CallConv = CallingConvention.ByVal;
          break;
        default:
          Console.WriteLine("Invalid option, please try again.");
          goto GetConvention;
      }//switch
      return;
    }//GetOptions()

    private static IServerChannelSinkProvider GetProviderChain() {
        IServerChannelSinkProvider chain = new System.Runtime.Remoting.MetadataServices.SdlChannelSinkProvider();            
        IServerChannelSinkProvider sink = chain;
        SoapServerFormatterSinkProvider soapProvider = new SoapServerFormatterSinkProvider();
        soapProvider.TypeFilterLevel =  System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;
        sink.Next = soapProvider;                    
        sink = sink.Next;
        BinaryServerFormatterSinkProvider binaryProvider = new BinaryServerFormatterSinkProvider();
        binaryProvider.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;
        sink.Next = binaryProvider;     
        return chain;
    }

    /// <summary>
    /// Runs this program as a server.
    /// </summary>
    public static void Run(ChannelKind ChanKind, WellKnownObjectMode ObjMode, Type ObjType)
    {
      TcpChannel tcpchan;
      HttpChannel httpchan;
      System.Collections.Hashtable httpproperties;
      System.Collections.Hashtable tcpproperties;

      Console.WriteLine("\nStarting server in WellKnownObjectMode {0}\n", ObjMode.ToString());

      switch (ChanKind)
      {
        case ChannelKind.Http:
          httpproperties = new System.Collections.Hashtable();
          httpproperties["port"] = Server.HTTPPORT;
          httpchan = new HttpChannel(httpproperties, null, GetProviderChain());     
          ChannelServices.RegisterChannel(httpchan);
          break;
        case ChannelKind.TCP:
          tcpproperties = new System.Collections.Hashtable();
          tcpproperties["port"] = Server.TCPPORT;
          tcpchan = new TcpChannel(tcpproperties, null, GetProviderChain());
          ChannelServices.RegisterChannel(tcpchan);
          break;
        case ChannelKind.Both:
          httpproperties = new System.Collections.Hashtable();
          httpproperties["port"] = Server.HTTPPORT;
          httpchan = new HttpChannel(httpproperties, null, GetProviderChain());     
          ChannelServices.RegisterChannel(httpchan);

          tcpproperties = new System.Collections.Hashtable();
          tcpproperties["port"] = Server.TCPPORT;
          tcpchan = new TcpChannel(tcpproperties, null, GetProviderChain());
          ChannelServices.RegisterChannel(tcpchan);
          break;
        default:
          throw new System.InvalidOperationException("Unexpected Channel kind in Server.Run()");
      }//switch
      
      RemotingConfiguration.RegisterWellKnownServiceType(ObjType, 
                                                         "SayHello", 
                                                         ObjMode);

      System.Console.WriteLine("Hit <enter> to exit...");
      System.Console.ReadLine();
      return;
    }
  }//class Server

}//namespace

