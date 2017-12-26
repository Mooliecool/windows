//------------------------------------------------------------------------------
// <copyright file="udpchat.cs" company="Microsoft">
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
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;

public class Chat 
{

  private static UdpClient m_Client;

  private static int ListenerPort = 8080;
  private static int SenderPort = 8080;
  private static int LocalPort;
  private static int RemotePort;

  private static string m_szHostName;

  private static IPAddress m_GroupAddress;
  private static IPHostEntry m_LocalHost;
  private static IPEndPoint m_RemoteEP;

  private static bool m_Done = false;

  public static void Usage() 
  {
    Console.WriteLine(
      "*----------------------------------------------------------------------------*\n" +
      "*                         UDP Multicast Chat Utility                         *\n" +
      "*                                   Usage:                                   *\n" +
      "*                                 udpchat.exe                                *\n" +
      "*                                                                            *\n" +
      "* You can only execute one instance of chat on the same system.              *\n" +
      "* To test this sample, copy the executable to another system run from there. *\n" +
      "* To exit this sample, enter a line starting with the '@' symbol.            *\n" +
      "*----------------------------------------------------------------------------*\n" 
      );
  }//Usage()

  public static void Main(string [] args ) 
  {

    LocalPort = SenderPort;
    RemotePort = ListenerPort;

    Usage();
    
    m_szHostName = Dns.GetHostName();
    m_LocalHost = Dns.GetHostByName(m_szHostName);

    Console.WriteLine("Local Port: {0}, Remote: {1}", LocalPort, RemotePort);
    Console.WriteLine("Initializing...");

    Initialize();

    Console.WriteLine("Starting Listener thread...");

    Thread workerThread = new Thread(new ThreadStart(Listener));
    workerThread.Start();

    byte [] buffer = null;

    Encoding ASCII = Encoding.ASCII;

    bool ShuttingDown = false;

    while(!ShuttingDown) 
    {
      string InputString = Console.ReadLine();

      if ( InputString.Length == 0 )
        continue;

      //Look for exit flag from user.
      if (InputString.StartsWith("@"))
      {
        lock(typeof(Chat))
        {
          m_Done = true;
          //
          // Send a terminator to ourselves,
          // so that the receiving thread can shut down.
          //
          InputString = m_szHostName + ":@";
        }//lock
        ShuttingDown = true;
      } 
      else 
      {
        lock(typeof(Chat))
        {
          InputString = m_szHostName + ":" + InputString;
        }//lock
      }//else

      buffer = new byte[InputString.Length + 1];
      //
      // send data to remote peer
      //

      int len = ASCII.GetBytes( InputString.ToCharArray(), 0, InputString.Length, buffer, 0);
      int ecode;
      lock(typeof(Chat))
      {
        ecode = m_Client.Send(buffer, len, m_RemoteEP);
      }//lock

      if(ecode <= 0) 
      {
        Console.WriteLine("Error in send : " + ecode);
      }//if

    }//while

    workerThread.Abort();
    workerThread.Join();

    Console.WriteLine("Closing connection...");
    Terminate();

  } // Main

  public static void Terminate() 
  {
    lock(typeof(Chat))
    {
      m_Client.DropMulticastGroup(m_GroupAddress);
    }//lock
  }//Terminate()

  public static void Initialize() 
  {
    //
    // instantiate UdpCLient
    //
    
    m_Client = new UdpClient(LocalPort);

    //
    // Create an object for Multicast Group
    //

    m_GroupAddress = IPAddress.Parse("224.0.0.1");
    //
    // Join Group
    //
    try 
    {
      m_Client.JoinMulticastGroup(m_GroupAddress, 100);
    }//try
    catch(Exception e) 
    {
      Console.WriteLine("Unable to join multicast group: {0}", e.ToString());
    }//catch

    //
    // Create Endpoint for peer
    //
    m_RemoteEP = new IPEndPoint(m_GroupAddress, RemotePort);

  }//Initialize()

  public static void Listener() 
  {
    bool Done = false;
    string stringData;
    byte [] data;

    // The listener waits for data to come and buffers it

    Thread.Sleep(2000); // Make sure client2 is receiving

    Encoding    ASCII = Encoding.ASCII;
    Console.WriteLine("\nReady to transmit input...\n");
    
    lock(typeof(Chat))
    {
      Done = m_Done;
    }//lock

    //while(!m_Done) 
    while(!Done)
    {
      IPEndPoint endpoint = null;

      //Can't lock here because thread waits on the Receive before continuing.
      data = m_Client.Receive(ref endpoint);

      string strData = ASCII.GetString(data);

      if( strData.IndexOf(":@") > 0 ) 
      {
        //
        // We received a termination indication
        // now we have to decide if it is from
        // our main thread shutting down, or
        // from someone else.
        //
        char [] separators = {':'};
        string [] vars = strData.Split(separators);

        lock(typeof(Chat))
        {
          stringData = m_szHostName;
        }//lock
        if( vars[0] == stringData ) 
        {
          //
          // This is from ourselves, therefore we end now.
          //
          Console.WriteLine("Shutting down Listener thread...");

          //
          // This should have been done by main thread, but we
          // do it again for safety.
          //
          Done = true;
          lock(typeof(Chat))
          {
            m_Done = true;
          }//lock
        }//if
        else 
        {
          //
          // This is a termination from someone else
          //
          Console.WriteLine("{0} has left the conversation", vars[0]);
        }//else
      }//if
      else 
      {
        //
        // this is normal data received from others
        // as well as ourselves
        // check to see if it is from ourselves before
        // we print
        //
        if(strData.IndexOf(":") > 0) 
        {
          char [] separators = {':'};
          string [] vars = strData.Split(separators);

          lock(typeof(Chat))
          {
            stringData = m_szHostName;
          }//lock
          
          if( vars[0] != stringData ) 
          {
            Console.WriteLine(strData);
          }//if
        }//if
      }//else
    }//while

    Console.WriteLine("Listener thread finished...");
    return;
  }//Listener()
}//class Chat
