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
public class Co5605Flush
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BufferedStream.Flush()";
    public static String s_strTFName        = "Co5605Flush.cs";
    public static String s_strTFAbbrev      = "Co5605";
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
            if(File.Exists("Co5605Test.tmp"))
                File.Delete("Co5605Test.tmp");
            strLoc = "Loc_9g8yg";
            FileStream fs = new FileStream("Co5605Test.tmp", FileMode.Create);
            bs2 = new BufferedStream(fs);
            bs2.Write(new Byte[]{65,66,67,68,69,70}, 0, 6);
            bs2.Flush();			
            StreamReader sr2 = new StreamReader(fs);
            iCountTestcases++;
            bs2.Position = 0;
            if(!sr2.ReadToEnd().Equals("ABCDEF")) 
            {
                iCountErrors++;
                printerr( "Error_19009! Not flushed correctly");
            }
            bs2.Position = 3;
            bs2.Write(new Byte[]{65,66,67}, 0, 3);
            bs2.Flush();
            bs2.Position = 0;
            if(!sr2.ReadToEnd().Equals("ABCABC")) 
            {
                iCountErrors++;
                printerr( "Error_01909! Not flushed correctly");
            }
            sr2.Close();
            strLoc = "Loc_857yv";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            bs2.Write(new Byte[]{1}, 0, 1);
            memstr2.Close();
            iCountTestcases++;
            try 
            {
                bs2.Flush();
                iCountErrors++;
                printerr( "Error_50039! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_0199x! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_298t8! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_20987";
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5605Flush.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
            if(File.Exists("Co5605Test.tmp"))
                File.Delete("Co5605Test.tmp");
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
            Byte[] b = new Byte[6];
            bs.Read(b, 0, b.Length);
            int iExpected = 65 ;
            for(int i = 0 ; i < b.Length ; i++) 
            {
                if(b[i] != iExpected++) 
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
        BufferedStream bs = new BufferedStream( stream );
        bs.Write(new Byte[]{65,66,67,68,69,70}, 0, 6);
        bs.Flush();			
        StreamReader sr = new StreamReader(stream);
        sr.Close();
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5605Flush cbA = new Co5605Flush();
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
