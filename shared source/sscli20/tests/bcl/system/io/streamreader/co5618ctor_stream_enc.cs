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
public class Co5618ctor_stream_enc
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader(Stream)";
    public static String s_strTFName        = "Co5618ctor_stream_enc.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static Encoding enc = Encoding.Default ;
    private static String strWrite = String.Empty , strResult = String.Empty;
    int iCountTestcases = 0;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            MemoryStream ms2;
            FileStream fs2;
            StreamReader sr2;
            StreamWriter sw2;
            String str2;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);			
            strLoc = "Loc_98yv7";
            iCountTestcases++;
            try 
            {
                sr2 = new StreamReader((Stream)null, Encoding.UTF8);
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
            strLoc = "Loc_24098";
            iCountTestcases++;
            try 
            {
                sr2 = new StreamReader(new MemoryStream(), null);
                iCountErrors++;
                printerr( "Error_209x7! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo("Info_277v7! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_298x7! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_98y8x";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.ASCII);
            sw2.Write("HelloThere\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.ASCII);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_298xw! Incorrect String interpreted, str2=="+str2);
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.ASCII);
            sw2.Write("HelloThere\u00FF");
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new StreamReader(ms2, Encoding.ASCII);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_298xw! Incorrect String interpreted, str2=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( Encoding.ASCII, "HelloThere\u00FF", "HelloThere?");
            strLoc = "Loc_4747u";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.UTF8);
            sw2.Write("This is UTF8\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF8);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF8\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_1y8xx! Incorrect string on stream, str2=="+str2);
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.UTF8);
            sw2.Write("This is UTF8\u00FF");
            sw2.Flush();
            ms2.Position = 0;			
            sr2 = new StreamReader(ms2, Encoding.UTF8);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF8\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_1y8xx! Incorrect string on stream, str2=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( Encoding.UTF8, "This is UTF8\u00FF", "This is UTF8\u00FF");
            strLoc = "Loc_28y7c";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.UTF7);
            sw2.Write("This is UTF7\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF7);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF7\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_2091x! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.UTF7);
            sw2.Write("This is UTF7\u00FF");
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new StreamReader(ms2, Encoding.UTF7);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF7\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_5f98y! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( Encoding.UTF7, "This is UTF7\u00FF", "This is UTF7\u00FF");    
            strLoc = "Loc_98hcf";
            strValue = "This is BigEndianUnicode\u00FF" ;
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.BigEndianUnicode);
            sw2.Write( strValue );
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.BigEndianUnicode);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals( strValue )) 
            {
                iCountErrors++;
                printerr( "Error_2g88t! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.BigEndianUnicode);
            sw2.Write(strValue);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new StreamReader(filName, Encoding.BigEndianUnicode);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(strValue)) 
            {
                iCountErrors++;
                printerr( "Error_8f7yv! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( Encoding.BigEndianUnicode, strValue, strValue);
            strLoc = "Loc_48y8d";
            strValue = "This is Unicode\u00FF" ;
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.Unicode);
            sw2.Write(strValue);
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.Unicode);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals( strValue )) 
            {
                iCountErrors++;
                printerr( "Error_2g88t! Incorrect string on stream=="+str2);
            }
            sr2.Close();			
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.Unicode);
            sw2.Write( strValue);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new StreamReader(ms2, Encoding.Unicode);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(strValue)) 
            {
                iCountErrors++;
                printerr( "Error_f897h! Incorrect string on stream=="+str2);
            }
            sr2.Close();			
            VerifyNetworkStreamWorks( Encoding.BigEndianUnicode, strValue, strValue);
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
    private void VerifyNetworkStreamWorks( Encoding encValue, String str1, String str2)
    {
        iCountTestcases++;
        enc = encValue ;
        strWrite = str1 ;
        strResult = str2;
        m_PortSetEvent.Reset();
        Thread tcpListenerThread = new Thread(new ThreadStart(Co5618ctor_stream_enc.StartListeningTcp));
        tcpListenerThread.Start();
        Thread.Sleep( 1000 );
        m_PortSetEvent.WaitOne();
        Teleport("127.0.0.1");
        tcpListenerThread.Join(); 
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

            s = listener.AcceptSocket();
            ns = new NetworkStream(s);
            sr = new StreamReader(ns , enc);
            String strRead = sr.ReadLine();
            Console.WriteLine("read string..." + strRead );
            if(strRead != strResult) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_66677! Incorrect string read");
            }
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
        StreamWriter sr = new StreamWriter( stream , enc);
        sr.WriteLine( strWrite );
        sr.Flush();
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5618ctor_stream_enc cbA = new Co5618ctor_stream_enc();
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
