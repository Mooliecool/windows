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
public class Co5625CurrentEncoding
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader(String, Encoding)";
    public static String s_strTFName        = "Co5625CurrentEncoding.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static Encoding enc = Encoding.ASCII ;
    private int iCountTestcases = 0;
    private static String strValue = "This is a testing string";
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        try
        {
            StreamReader sr2;
            StreamWriter sw2;
            String str2;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);			
            strLoc = "Loc_98yv7";
            iCountTestcases++;
            sw2 = new StreamWriter(filName);
            sw2.Close();
            try 
            {
                sr2 = new StreamReader((String)null, Encoding.UTF8);
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
            iCountTestcases++;
            try 
            {
                sr2= new StreamReader(filName, null);
                iCountErrors++;
                printerr( "Error_t890y! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo( "Error_t98yb! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_948yb! Incorrect exception thrown, exc=="+exc.Message);
            }
            strLoc = "Loc_98y8x";
            sw2 = new StreamWriter("Co5597Test.tmp", false, Encoding.ASCII);
            sw2.Write("HelloThere\u00FF");
            sw2.Close();
            sr2 = new StreamReader("Co5597Test.tmp", Encoding.ASCII);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_298xw! Incorrect String interpreted");
            }
            iCountTestcases++;
            if(sr2.CurrentEncoding != Encoding.ASCII) 
            {
                iCountErrors++;
                printerr( "Error_32987! Incorrect encoding");
            }
            sr2.Close();
            strLoc = "Loc_4747u";
            sw2 = new StreamWriter("Co5597Test.tmp", false, Encoding.UTF8);
            sw2.Write("This is UTF8\u00FF");
            sw2.Close();
            sr2 = new StreamReader("Co5597Test.tmp", Encoding.UTF8);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF8\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_1y8xx! Incorrect string on stream");
            }
            iCountTestcases++;
            if(sr2.CurrentEncoding != Encoding.UTF8) 
            {
                iCountErrors++;
                printerr( "Error_8t8gn! Incorrect encoding");
            }
            sr2.Close();
            strLoc = "Loc_28y7c";
            sw2 = new StreamWriter("Co5597Test.tmp", false, Encoding.UTF7);
            sw2.Write("This is UTF7\u00FF");
            sw2.Close();
            sr2 = new StreamReader("Co5597Test.tmp", Encoding.UTF7);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF7\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_2091x! Incorrect string on stream=="+str2);
            }
            iCountTestcases++;
            if(sr2.CurrentEncoding != Encoding.UTF7) 
            {
                iCountErrors++;
                printerr( "Error_92y76! Incorrect encoding");
            }
            sr2.Close();
            strLoc = "Loc_98hcf";
            sw2 = new StreamWriter("Co5597Test.tmp", false, Encoding.BigEndianUnicode);
            sw2.Write("This is BigEndianUnicode\u00FF");
            sw2.Close();
            sr2 = new StreamReader("Co5597Test.tmp", Encoding.BigEndianUnicode);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is BigEndianUnicode\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_2g88t! Incorrect string on stream=="+str2);
            }
            iCountTestcases++;
            if(sr2.CurrentEncoding.EncodingName != Encoding.BigEndianUnicode.EncodingName) 
            {
                iCountErrors++;
                printerr( "Error_t987y! Incorrect encoding");
            }
            sr2.Close();
            strLoc = "Loc_48y8d";
            sw2 = new StreamWriter("Co5597Test.tmp", false, Encoding.Unicode);
            sw2.Write("This is Unicode\u00FF");
            sw2.Close();
            sr2 = new StreamReader("Co5597Test.tmp", Encoding.Unicode);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is Unicode\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_2g88t! Incorrect string on stream=="+str2);
            }
            iCountTestcases++;
            if(sr2.CurrentEncoding.EncodingName != Encoding.Unicode.EncodingName) 
            {
                iCountErrors++;
                printerr( "Error_5y7gd! Incorrect encoding");
            }
            sr2.Close();			
            enc = Encoding.ASCII ;
            NetworkStreamTest();
            enc = Encoding.BigEndianUnicode ;
            NetworkStreamTest();
            enc = Encoding.UTF7 ;
            NetworkStreamTest();
            enc = Encoding.UTF8 ;
            NetworkStreamTest();
            enc = Encoding.Unicode ;
            NetworkStreamTest();
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
    private void NetworkStreamTest()
    {
        iCountTestcases++;
        m_PortSetEvent.Reset();
        Thread tcpListenerThread = new Thread(new ThreadStart(Co5625CurrentEncoding.StartListeningTcp));
        tcpListenerThread.Start();
        Console.WriteLine("Listening");
        Thread.Sleep( 1000 );
        m_PortSetEvent.WaitOne();
        Teleport("127.0.0.1");
        Thread.Sleep( 1000 );
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
        TcpThreadListener listener = null;
        Socket s = null;
        NetworkStream ns = null;
        StreamReader sr = null;

        try 
        {
            listener = new TcpThreadListener(0);
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Console.WriteLine("Using port number: {0}", iPortNumber);
            s = listener.AcceptSocket();
            ns = new NetworkStream(s);
            sr = new StreamReader(ns);
            try 
            {
                String line = sr.ReadLine();
                if( line != strValue )
                {
                    iCountErrors++;
                    Console.WriteLine( "Error_0000! Expected exception not thrown");
                }
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
        }  

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
        StreamWriter sw = new StreamWriter( stream );
        sw.WriteLine( strValue );
        stream.Flush();
        sw.Close();
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5625CurrentEncoding cbA = new Co5625CurrentEncoding();
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
