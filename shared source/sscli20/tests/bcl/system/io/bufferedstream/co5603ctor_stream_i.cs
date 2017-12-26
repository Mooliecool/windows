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
public class Co5603ctor_stream_i
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BufferedStream(Stream, Int32)";
    public static String s_strTFName        = "Co5603ctor_stream_i.cs";
    public static String s_strTFAbbrev      = "Co5602";
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
            if(File.Exists("Co5602Test.tmp"))
                File.Delete("Co5602Test.tmp");
            strLoc = "Loc_98yv7";
            iCountTestcases++;
            try 
            {
                bs2 = new BufferedStream((Stream)null, 1);
                iCountErrors++;
                printerr( "Error_2yc83! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo("Info_287c7! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_984yv! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_758xy";
            memstr2 = new MemoryStream();
            iCountTestcases++;
            try 
            {
                bs2 = new BufferedStream(memstr2, 0);
                iCountErrors++;
                printerr( "Error_90838! Expected exception not thrown");
            } 
            catch (ArgumentOutOfRangeException aexc) 
            {
                printinfo( "Info_598by! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_198hg! Incorrect exception thrown, exc=="+exc.ToString());
            }
            memstr2.Close();
            strLoc = "Loc_567g7";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2, 2);
            bs2.Write(new Byte[]{65,66,67}, 0, 3);
            Byte[] b = new Byte[3];
            bs2.Position = 0;
            bs2.Read(b, 0, 3);
            for(int i = 0 ; i < b.Length ; i++) 
            {
                iCountTestcases++;
                if(b[i] != i+65) 
                {
                    iCountErrors++;
                    printerr( "Error_2958v! Expected=="+i+", got=="+b[i]);
                }
            }
            StreamReader sr2 = new StreamReader(memstr2);
            memstr2.Position = 0;
            iCountTestcases++;
            if(!sr2.ReadToEnd().Equals("ABC")) 
            {
                iCountTestcases++;
                printerr( "ERror_42987! Unexpected string on stream");
            }
            strLoc = "Loc_277gy";
            memstr2 = new MemoryStream();
            memstr2.Close();
            iCountTestcases++;
            try 
            {
                bs2 = new BufferedStream(memstr2, 10);
                iCountErrors++;
                printerr("Error_2g8yb! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_578yg! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_3477c! Incorrect exception thrown, exc=="+exc.ToString());
            }
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5603ctor_stream_i.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
            if(File.Exists("Co5602Test.tmp"))
                File.Delete("Co5602Test.tmp");
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
        BufferedStream bs = null;

        try 
        {
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Socket s = listener.AcceptSocket();
            ns = new NetworkStream(s);
            bs = new BufferedStream( ns );   
            Byte[] b = new Byte[3];
            bs.Read(b, 0, 3);
            for(int i = 0 ; i < b.Length ; i++) 
            {
                if(b[i] != i) 
                {
                    iCountErrors++;
                    Console.WriteLine( "Error_4324! Expected=="+i+", got=="+b[i]);
                }
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
            if(bs != null)
            {
                bs.Close();
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
        bs.Write(new Byte[]{0,1,2}, 0, 3);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5603ctor_stream_i cbA = new Co5603ctor_stream_i();
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
