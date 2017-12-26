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
public class Co5608get_CanSeek
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BufferedStream.Flush()";
    public static String s_strTFName        = "Co5608get_CanSeek.cs";
    public static String s_strTFAbbrev      = "Co5608";
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
            BufferedStream bs2;
            MemoryStream memstr2;
            FileStream fs2;
            if(File.Exists("Co5608Test.tmp"))
                File.Delete("Co5608Test.tmp");
            strLoc = "Loc_02u9c";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            bs2.Close();
            iCountTestcases++;
            if(bs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_1099c! CanSeek returned true for closed stream");
            } 
            strLoc = "Loc_20987";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            memstr2.Close();
            iCountTestcases++;
            if(bs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_898c8! True when underlying stream is closed");
            }
            strLoc = "Loc_029uc";
            fs2 = new FileStream("Co5608Test.tmp", FileMode.Create, FileAccess.Write);
            bs2 = new BufferedStream(fs2);
            iCountTestcases++;
            if(!bs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_0uyt4! True when stream is write only");
            }
            bs2.Close();
            strLoc = "Loc_091uc";
            fs2 = new FileStream("Co5608Test.tmp", FileMode.Open, FileAccess.ReadWrite);
            bs2 = new BufferedStream(fs2);
            iCountTestcases++;
            if(!bs2.CanSeek) 
            {
                iCountErrors++;
                printerr("Error_298xj! False when stream is ReadWrite");
            } 
            fs2.Close();
            strLoc = "Loc_09u9v";
            fs2 = new FileStream("Co5608Test.tmp", FileMode.Open, FileAccess.Read);
            bs2 = new BufferedStream(fs2);
            iCountTestcases++;
            if(!bs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_f8u89! False when stream is Read");
            }
            fs2.Close();
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5608get_CanSeek.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
            if(File.Exists("Co5608Test.tmp"))
                File.Delete("Co5608Test.tmp");
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
        try 
        {
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Socket s = listener.AcceptSocket();
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
        BufferedStream bs = new BufferedStream( stream , 3);
        if(bs.CanSeek) 
        {
            iCountErrors++;
            Console.WriteLine( "Error_434234! CanSeek returned true");
        } 
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5608get_CanSeek cbA = new Co5608get_CanSeek();
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
