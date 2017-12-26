//------------------------------------------------------------------------------
// <copyright file="client.cs" company="Microsoft">
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
using RemotingSamples;

namespace RemotingSamples 
{
  /// <summary>
  /// Main entry point class.
  /// </summary>
  public class ClientMain
  {
    //These statics will contain the results of the user input.
    private static InvokeKind m_InvKind = InvokeKind.Sequential;
    private static CallingConvention m_CallConv = CallingConvention.ByRef;
    private static ChannelKind m_ChanKind = ChannelKind.TCP;
    private static Type m_ObjType = null;
    
    private static string m_Host = "localhost";

    public static void Main(string [] args)
    {      
      if (args.Length > 0)
        m_Host = args[0];

      Usage();
      GetOptions();    

      Client.RegisterBackPortChannels();
      InvokeServer();
      Client.UnregisterBackPortChannels();
    }//Main()

    /// <summary>
    /// Creates instances of the Client object and invokes methods.
    /// If the user specified to use both HTTP and TCP via threads then 
    /// creates threads and waits for them to terminate.
    /// 
    /// Allows the user to execute again.  This allows seeing the effects 
    /// in the server when running either SingleCall or Singleton.
    /// </summary>
    private static void InvokeServer()
    {
      try
      {
        //Use threads to do this.  
        //Each thread has its own channel registered so they don't conflict.
        if (m_ChanKind == ChannelKind.Both)
        {
          Client CliObj1 = new Client(ChannelKind.Http, m_ObjType, m_CallConv, m_InvKind, m_Host);
          Thread thread1 = new Thread(new ThreadStart(CliObj1.Run));
          CliObj1.ThreadNumber = 1;
  
          Client CliObj2 = new Client(ChannelKind.TCP, m_ObjType, m_CallConv, m_InvKind, m_Host);
          Thread thread2 = new Thread(new ThreadStart(CliObj2.Run));
          CliObj2.ThreadNumber = 2;
          thread1.Start();
          thread2.Start();
          //Block on the threads until they both terminate.
          thread1.Join();
          thread2.Join();
        }//if
        else  //Not threaded.
        {
          Client CliObj = new Client(m_ChanKind, m_ObjType, m_CallConv, m_InvKind, m_Host);
          CliObj.Run();
        }//else
      }//try
      catch (Exception e)
      {
        Console.WriteLine("Exception in InvokeServer: {0}", e.ToString());
        return;
      }//catch

      Console.Write("\nRun again (y)? ");
      string Reply = Console.ReadLine();
      Console.WriteLine();
      if (Reply.ToLower() == "y" || Reply.ToLower() == "yes")
      {
        InvokeServer();
      }//if

    }//InvokeServer()

    /// <summary>
    /// Usage.
    /// </summary>
    private static void Usage()
    {
      Console.WriteLine(
        "===============================================================\n" +
        "*  This is the client remoting sample application.            *\n" +
        "*  Use with the server application to use remoting.           *\n" +
        "*                                                             *\n" +
        "*  Start with an instance of the server in one console and    *\n" +
        "*  run another instance as client in another console.         *\n" +
        "*                                                             *\n" +
        "*  You must match the type of object (ByRef or ByVal) between *\n" +
        "*  the client and server or an exception will occur           *\n" +    
        "*                                                             *\n" + 
        "*  The input values are:                                      *\n" + 
        "*     - Channel: use HTTP, TCP or both.                       *\n" +
        "*                If both, then threads will be used.          *\n" +
        "*                                                             *\n" + 
        "*     - Calling Convention: by reference or by value          *\n" +
        "*                                                             *\n" + 
        "*     - Invocation type: sequential or asynchronous           *\n" +
        "===============================================================\n");
    }//Usage()
    
    /// <summary>
    /// Gets user options.
    /// </summary>
    private static void GetOptions()
    {
 GetChannel:
      Console.Write("\nHttp (h), TCP (t) or both (b) using threads? ");
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

GetInvoke:
      Console.Write("\nSerial (s) or async (a)? ");
      Reply = Console.ReadLine();
      switch (Reply.ToLower())
      {
        case "a":
          m_InvKind = InvokeKind.Async;
          break;
        case "s":
          m_InvKind = InvokeKind.Sequential;
          break;
        default:
          Console.WriteLine("Invalid option, please try again.");
          goto GetInvoke;
    }//switch

      //Setup the correct Type object to pass to the Client object for object creation.
      switch (m_CallConv)
      {
        case CallingConvention.ByRef:
          m_ObjType = Type.GetType("RemotingSamples.HelloServerByRef,remotingshared");
          break;
        case CallingConvention.ByVal:
          m_ObjType = Type.GetType("RemotingSamples.HelloServerByVal,remotingshared");
          break;
        default:
          throw new System.InvalidOperationException("Invalid Calling Convention in Main()");
      }//switch
      Console.WriteLine();
      return;
    }//GetOptions()
    //  }//class Client
  }//class ClientMain


  /// <summary>
  /// Class that registers the remoting channels.
  /// It has methods for both sychchonous and async method invocation.
  /// </summary>
  public class Client
  {
    public const int HTTPPORT = 8085;
    public const int TCPPORT = 8086;

    //These are only used in the ByRef case as the back channel to pass the ForwardByRef object back.
    public const int HTTPBACKPORT = 8087;
    public const int TCPBACKPORT = 8088;
    
    private string m_Host;

    private static HttpChannel m_HttpChannel = null;
    private static TcpChannel m_TcpChannel = null;

    //This is the wait object for the async method invocation.
    private ManualResetEvent m_ResetEvent;

    //These are the delegates for async.
    public delegate string ByRefDelegate(string text, ForwardByRef Forwarded);
    public delegate ForwardByVal ByValDelegate(string text, ForwardByVal Forwarded);

    //Options set in constructor.
    private ChannelKind m_ChanKind;
    private Type m_ObjType = null;
    private CallingConvention m_CallConv;
    private InvokeKind m_InvKind;
    private int m_ThreadNumber = 0;

    /// <summary>
    /// Only constructor.
    /// </summary>
    /// <param name="ChanKind"></param>
    /// <param name="ObjType"></param>
    /// <param name="CallConv"></param>
    /// <param name="InvKind"></param>
    public Client(ChannelKind ChanKind, 
                  Type ObjType, 
                  CallingConvention CallConv, 
                  InvokeKind InvKind,
                  string Host)
    {
      m_ChanKind = ChanKind;
      m_ObjType = ObjType;
      m_CallConv = CallConv;
      m_InvKind = InvKind;
      m_Host = Host;

      
    }//constructor  

    static public void RegisterBackPortChannels() {
        
        // We only need to register the channels once.
        if (m_HttpChannel == null) {  
            Console.WriteLine("RegisterChannel m_HttpChannel");
            m_HttpChannel = new HttpChannel(HTTPBACKPORT);
            ChannelServices.RegisterChannel(m_HttpChannel);
        }
  
        if (m_TcpChannel == null) {
            Console.WriteLine("RegisterChannel m_TcpChannel");
            m_TcpChannel = new TcpChannel(TCPBACKPORT);
            ChannelServices.RegisterChannel(m_TcpChannel);
        }
    }

    static public void UnregisterBackPortChannels() {
        
        if (m_HttpChannel != null)
        {
          Console.WriteLine("UnregisterChannel m_HttpChannel");
          ChannelServices.UnregisterChannel(m_HttpChannel);
          m_HttpChannel = null;
        }

        if (m_TcpChannel != null)
        {
          Console.WriteLine("UnregisterChannel m_TcpChannel");
          ChannelServices.UnregisterChannel(m_TcpChannel);
          m_TcpChannel = null;
        }
    }

    //Allows getting and setting the thread number.
    public int ThreadNumber 
    {
      get 
      {
        return m_ThreadNumber;
      }//get
      set
      {
        m_ThreadNumber = value;
      }//set
    }//property CurrentThread

    /// <summary>
    /// The main execution code for invoking the server.
    /// - Opens the specified channel. 
    /// - Creates the proxy object.
    /// - Calls ClientInvokeMethod or ClientInvokeMethodAsync to invoke the output method on the proxy.
    /// </summary>
    public void Run()
    {
      string PortStr = null;
      object RemoteObj = null;

      try
      {
        lock(this)
        {
          switch (m_ChanKind)
          {
            case ChannelKind.Http:
            switch (m_CallConv)
            {
              case CallingConvention.ByRef:
                PortStr = "http://" + m_Host + ":" + HTTPPORT.ToString() + "/SayHello";
                RemoteObj = (HelloServerByRef)Activator.GetObject(
                  typeof(RemotingSamples.HelloServerByRef), 
                  PortStr);
                break;
              case CallingConvention.ByVal:
                PortStr = "http://" + m_Host + ":" + HTTPPORT.ToString() + "/SayHello";
                RemoteObj = (HelloServerByVal)Activator.GetObject(
                  typeof(RemotingSamples.HelloServerByVal), 
                  PortStr);
                break;
              default:
                throw new System.InvalidOperationException(
                  "Unexpected Calling Convention kind in Server.Run()");
            }//switch
              break;
            case ChannelKind.TCP:
            switch (m_CallConv)
            {
              case CallingConvention.ByRef:
               
                PortStr = "tcp://" + m_Host + ":" + TCPPORT.ToString() + "/SayHello";
                RemoteObj = (HelloServerByRef)Activator.GetObject(
                  typeof(RemotingSamples.HelloServerByRef), 
                  PortStr);
                break;
              case CallingConvention.ByVal:
                PortStr = "tcp://" + m_Host + ":" + TCPPORT.ToString() + "/SayHello";
                RemoteObj = (HelloServerByVal)Activator.GetObject(
                  typeof(RemotingSamples.HelloServerByVal), 
                  PortStr);
                break;
              default:
                throw new System.InvalidOperationException(
                  "Unexpected Calling Convention kind in Server.Run()");
            }//switch
              break;
            default:
              throw new System.InvalidOperationException("Unexpected Channel kind in Server.Run()");
          }//switch

          //Invoke using either sequential or async methods.
          switch (m_InvKind)
          {
            case InvokeKind.Async:
              ClientInvokeAsync(RemoteObj);
              break;
            case InvokeKind.Sequential:
              ClientInvoke(RemoteObj);
              break;
            default:
              throw new System.InvalidOperationException("Invalid invoke kind in Run()");
          }//switch
        }//end lock(this)
      }//try
      catch (Exception e)
      {
        Console.WriteLine("Exception in Client.Run: {0}", e.ToString());
        System.Diagnostics.Process.GetCurrentProcess().Kill();
        throw e;
      }//catch
    }//Run()

    /// <summary>
    /// Sequential invocation method.  
    /// </summary>
    /// <param name="RemoteObj"></param>
    public void ClientInvoke(object RemoteObj)
    //public void ClientInvoke()
    {
      try
      {
        if (RemoteObj == null)
        {
          Console.WriteLine("Could not locate server.");
          return;
        }//if

        lock (this)
        {
          switch (m_CallConv)
          {
            case CallingConvention.ByRef:
            {
              HelloServerByRef hsbr = (HelloServerByRef) RemoteObj;
              ForwardByRef fbr = new ForwardByRef();
              string str = hsbr.HelloMethod("Calling ByRef sequential", fbr);
              Console.WriteLine("Output from HelloMethod(): {0}", str);

              if (m_ThreadNumber > 0)
              {
                Console.WriteLine("From thread {0}", m_ThreadNumber.ToString());
              }//if
              break;
            }
            case CallingConvention.ByVal:
            {
              HelloServerByVal hsbv = (HelloServerByVal) RemoteObj;
              ForwardByVal fbv = new ForwardByVal();
              ForwardByVal returnfbv = hsbv.HelloMethod("Calling ByVal sequential", fbv);
              Console.WriteLine("Output from forwarded object: {0}", returnfbv.Message);
              if (m_ThreadNumber > 0)
              {
                Console.WriteLine("From thread {0}", m_ThreadNumber.ToString());
              }//if
              break;
            }
            default:
              throw new System.InvalidOperationException("Invalid calling convention in Run()");
          }//switch
        }//end lock(this)
      }//try
      catch (Exception e)
      {
        Console.WriteLine("Exception in Client.ClientInvoke(): {0}", e.ToString());
        System.Diagnostics.Process.GetCurrentProcess().Kill();
      }//catch
      return;

    }//ClientInvoke

    /// <summary>
    /// Used by AsyncClient.Run() to invoke the proxy method repeatedly.
    /// </summary>
    /// <param name="obj"></param>
    public void ClientInvokeAsync(object RemoteObj)
    {
      try
      {
        if (RemoteObj == null)
        {
          Console.WriteLine("Could not locate server.");
          return;
        }//if

        switch (m_CallConv)
        {
          case CallingConvention.ByRef:
          {
            AsyncCallback cb = new AsyncCallback(ByRefCallBack);
            HelloServerByRef hsbr = (HelloServerByRef)RemoteObj;
            ByRefDelegate del = new ByRefDelegate(hsbr.HelloMethod);
            ForwardByRef fbr = new ForwardByRef();
            IAsyncResult ar = del.BeginInvoke("Calling ByRef async", fbr, cb, null);
            lock(this)
            {      
              m_ResetEvent = new ManualResetEvent(false);
            }//lock
            m_ResetEvent.WaitOne();
            if (m_ThreadNumber > 0)
            {
              Console.WriteLine("From thread {0}", m_ThreadNumber.ToString());
            }//if
            break;
          }//case
          case CallingConvention.ByVal:
          {
            AsyncCallback cb = new AsyncCallback(ByValCallBack);
            HelloServerByVal hsbv = (HelloServerByVal)RemoteObj;
            ByValDelegate del = new ByValDelegate(hsbv.HelloMethod);
            ForwardByVal fbv = new ForwardByVal();
            IAsyncResult ar = del.BeginInvoke("Calling ByVal async", fbv, cb, null);
            lock(this)
            {      
              m_ResetEvent = new ManualResetEvent(false);
            }//lock
            m_ResetEvent.WaitOne();
            if (m_ThreadNumber > 0)
            {
              Console.WriteLine("From thread {0}", m_ThreadNumber.ToString());
            }//if
            break;
          }//case
        }//switch
      }//try
      catch (Exception e)
      {
        Console.WriteLine("Exception in Client.ClientInvokeAsync(): {0}", e.ToString());
        System.Diagnostics.Process.GetCurrentProcess().Kill();
      }//catch
      return;
    }//ClientInvokeAsync()

    /// <summary>
    /// Callback handler used in ClientInvokeMethod().
    /// </summary>
    /// <param name="ar"></param>
    public void ByRefCallBack(IAsyncResult ar)
    {
      try
      {
        ByRefDelegate d = (ByRefDelegate)((AsyncResult)ar).AsyncDelegate;
        Console.WriteLine(d.EndInvoke(ar));
        m_ResetEvent.Set();
      }//try
      catch (Exception e)
      {
        Console.WriteLine("Exception in ByRefCallBack(): {0}", e.ToString());
        System.Diagnostics.Process.GetCurrentProcess().Kill();
      }//catch
    }//ByRefCallBack()

    public void ByValCallBack(IAsyncResult ar)
    {
      try
      {
        ByValDelegate d = (ByValDelegate)((AsyncResult)ar).AsyncDelegate;
        ForwardByVal fbv = d.EndInvoke(ar);
        Console.WriteLine("Forwarded message: {0}", fbv.Message);
        m_ResetEvent.Set();
      }//try
      catch (Exception e)
      {
        Console.WriteLine("Exception in ByValCallBack(): {0}", e.ToString());
        System.Diagnostics.Process.GetCurrentProcess().Kill();
      }//catch
    }//ByRefCallBack()
  }//class Client
}//namespace

