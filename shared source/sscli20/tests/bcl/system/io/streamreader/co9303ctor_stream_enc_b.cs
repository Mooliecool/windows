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
public class Co9303ctor_stream_enc_b
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader(Stream, encoding, boolean)";
    public static String s_strTFName        = "Co9303ctor_stream_enc_b.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static Encoding enc = Encoding.Default ;
    private static String strWrite = String.Empty , strResult = String.Empty;
    private static bool detectEncodingFromByteOrderMarks = false ;
    int iCountTestcases = 0;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
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
                sr2 = new StreamReader((Stream)null, Encoding.UTF8,false);
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
            strLoc = "Loc_98g77";
            iCountTestcases++;
            try 
            {
                sr2 = new StreamReader(new MemoryStream(), null,false);
                iCountErrors++;
                printerr( "Error_t8786! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo("Info_2989x! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_27gy7! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_8974v";
            iCountTestcases++;
            ms2 = new MemoryStream();
            ms2.Close();
            iCountTestcases++;
            try 
            {
                sr2 = new StreamReader(ms2, Encoding.UTF8, false);
                iCountErrors++;
                printerr( "Error_198xy! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_9yg75! CAught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr("Error_76y7b! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_98y8x";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.ASCII, 9);
            sw2.Write("HelloThere\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.ASCII, false);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_298xw! Incorrect String interpreted");
            }
            sr2.Close();
            strLoc = "Loc_98y8x";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.ASCII, 9);
            sw2.Write("HelloThere\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.ASCII, true);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_5345! Incorrect String interpreted");
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false , Encoding.ASCII, "HelloThere\u00FF", "HelloThere?");
            VerifyNetworkStreamWorks( true , Encoding.ASCII, "HelloThere\u00FF", "HelloThere?");
            strLoc = "Loc_4747u";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.UTF8, 9);
            sw2.Write("This is UTF8\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF8, false);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF8\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_1y8xx! Incorrect string on stream");
            }
            sr2.Close();
            strLoc = "Loc_4747u";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.UTF8, 9);
            sw2.Write("This is UTF8\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF8, true);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF8\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_1543! Incorrect string on stream");
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false , Encoding.UTF8, "This is UTF8\u00FF", "This is UTF8\u00FF");
            VerifyNetworkStreamWorks( true , Encoding.UTF8, "This is UTF8\u00FF", "This is UTF8\u00FF");
            strLoc = "Loc_28y7c";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.UTF7, 11);
            sw2.Write("This is UTF7\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF7, false);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF7\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_2091x! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            strLoc = "Loc_28y7c";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.UTF7, 11);
            sw2.Write("This is UTF7\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF7, true);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("This is UTF7\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_98879! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false , Encoding.UTF7, "This is UTF7\u00FF", "This is UTF7\u00FF");
            VerifyNetworkStreamWorks( false , Encoding.UTF7, "This is UTF7\u00FF", "This is UTF7\u00FF");
            strLoc = "Loc_98hcf";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.BigEndianUnicode, 20);
            String s = "This is BigEndianUnicode\u00FF";
            sw2.Write(s);
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.BigEndianUnicode, false);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(s)) 
            {
                iCountErrors++;
                printerr( "Error_56546! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            strLoc = "Loc_98hcf";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.BigEndianUnicode, 20);
            sw2.Write(s);
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.BigEndianUnicode, true);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(s)) 
            {
                iCountErrors++;
                printerr( "Error_80685! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false , Encoding.BigEndianUnicode, s, s);
            VerifyNetworkStreamWorks( true , Encoding.BigEndianUnicode, s, s);
            strLoc = "Loc_48y8d";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.Unicode, 50000);
            s = "This is Unicode\u00FF";
            sw2.Write(s);
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.Unicode, false);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(s)) 
            {
                iCountErrors++;
                printerr( "Error_9877i! Incorrect string on stream=="+str2);
            }
            sr2.Close();	
            strLoc = "Loc_48y8d";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.Unicode, 50000);
            s = "This is Unicode\u00FF";
            sw2.Write(s);
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.Unicode, true);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(s)) 
            {
                iCountErrors++;
                printerr( "Error_9877i! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false , Encoding.BigEndianUnicode, s, s);
            VerifyNetworkStreamWorks( true , Encoding.BigEndianUnicode, s, s);
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
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
    private void VerifyNetworkStreamWorks( bool bValue,  Encoding encValue, String str1, String str2)
    {
        iCountTestcases++;
        detectEncodingFromByteOrderMarks = bValue ;
        enc = encValue ;
        strWrite = str1 ;
        strResult = str2;
        m_PortSetEvent.Reset();
        Thread tcpListenerThread = new Thread(new ThreadStart(Co9303ctor_stream_enc_b.StartListeningTcp));
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
            sr = new StreamReader(ns , enc, detectEncodingFromByteOrderMarks);
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
        Co9303ctor_stream_enc_b cbA = new Co9303ctor_stream_enc_b();
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
