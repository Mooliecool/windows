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
using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.Net;
public class Co5623Close
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader.Close()";
    public static String s_strTFName        = "Co5623Close.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            StreamReader sr2;
            StreamWriter sw2;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);			
            strLoc = "Loc_9577b";
            sw2 = new StreamWriter(filName);
            sw2.WriteLine("Dette er sjef");
            sw2.Close();
            sr2 = new StreamReader(filName);
            sr2.Close();
            sr2.Close();
            sr2.Close();
            iCountTestcases++;
            try 
            {
                sr2.Peek();
                iCountErrors++;
                printerr( "Error_687yv! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_y67y9! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_9y7g7! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                sr2.Read();
                iCountErrors++;
                printerr( "ERror_y658b! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_7y857! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t87yg! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                sr2.Read(new Char[2], 0, 2);
                iCountErrors++;
                printerr( "Error_2398j! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_t877b! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_787yb! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                sr2.ReadLine();
                iCountErrors++;
                printerr( "Error_8g7bp! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_g87y7! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t8g7l! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                sr2.ReadToEnd();
                iCountErrors++;
                printerr( "Error_17t98! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_876yb! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_18788! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_g9yg8";
            sw2 = new StreamWriter(filName);
            sw2.Close();
            sr2 = new StreamReader(filName);
            sr2.Close();
            sr2 = new StreamReader(filName);
            sr2.Close();
            strLoc = "Loc_98yff";
            sw2 = new StreamWriter(filName);
            sw2.Close();
            sw2.Close();
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5623Close.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
            if(File.Exists(filName))
                File.Delete(filName);
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
            return true;
        }
        else
        {
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public void printerr ( String err )
    {
        Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
    }
    public void printinfo ( String info )
    {
        Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
    }
    public static void StartListeningTcp() 
    {
        TcpThreadListener listener = new TcpThreadListener(0);
        NetworkStream ns = null;
        StreamReader sr = null;
        try 
        {
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Socket s = listener.AcceptSocket();
            ns = new NetworkStream(s);
            sr = new StreamReader(ns);
            ns.Close();
            try 
            {
                sr.Read();
                iCountErrors++;
                Console.WriteLine( "Error_0000! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                Console.WriteLine( "INFO !!!!! Caught expected exception, exc=="+iexc.Message + "You are right man");
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_5555! Incorrect exception thrown, exc=="+exc.ToString());
            }
            Console.WriteLine("We are done with the listening");
        }
        catch(Exception e) 
        {
            iCountErrors++ ;
            Console.WriteLine("Exception receiving Teleportation: " + e.Message + Environment.NewLine + e.StackTrace);
            m_PortSetEvent.Set();
        }
        finally
        {
            if (listener != null)
            {
                listener.Stop();
            }
            if (ns != null)
            {
                ns.Close();
            }
            if(sr != null)
            {
                sr.Close();
            }
        } //finally
    }
    internal class TcpThreadListener : TcpListener 
    {
        public TcpThreadListener(int port) : base(port) 
        {
        }
        internal void Shutdown() 
        {
            if ( Server != null )
                Server.Close();
        }	
    }
    public static void Teleport(string address) 
    {
        TcpClient tcpClient = new TcpClient();
        IPEndPoint epSend = null;
        IPAddress sendAddress = IPAddress.Parse(address);			
        epSend = new IPEndPoint(sendAddress, iPortNumber);
        tcpClient.Connect(epSend);
        Stream stream = tcpClient.GetStream();
        stream.Write(new Byte[]{65,66,67,68}, 0, 4);
        stream.Flush();
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5623Close cbA = new Co5623Close();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
