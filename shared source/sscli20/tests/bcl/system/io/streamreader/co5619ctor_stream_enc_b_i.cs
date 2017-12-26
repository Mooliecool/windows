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
public class Co5619ctor_stream_enc_b_i
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader(Stream)";
    public static String s_strTFName        = "Co5619ctor_stream_enc_b_i.cs";
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
                sr2 = new StreamReader((Stream)null, Encoding.UTF8,false, 1);
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
                sr2 = new StreamReader(new MemoryStream(), null,false, 1);
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
            strLoc = "Loc_299xj";
            iCountTestcases++;
            try 
            {
                ms2 = new MemoryStream();
                sr2 = new StreamReader(ms2, Encoding.UTF8,false, -2);
                iCountErrors++;
                printerr( "Error_1908x! Expected exception not thrown");
            } 
            catch (ArgumentOutOfRangeException aexc) 
            {
                printinfo( "Info_199xu! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_56y8v! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_8974v";
            iCountTestcases++;
            ms2 = new MemoryStream();
            ms2.Close();
            iCountTestcases++;
            try 
            {
                sw2 = new StreamWriter(ms2, Encoding.UTF8, 2);
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
            strLoc = "Loc_287g8";
            iCountTestcases++;
            try 
            {
                ms2 = new MemoryStream();
                sr2 = new StreamReader(ms2, Encoding.UTF8, false, Int32.MaxValue);
                iCountErrors++;
                printerr( "Error_498g7! Expected exception not thrown");
            } 
            catch (OutOfMemoryException aexc) 
            {
                printinfo( "Info_857yb! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_90d89! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_98y8x";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.ASCII, 9);
            sw2.Write("HelloThere\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.ASCII, false, 9);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_298xw! Incorrect String interpreted");
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false, Encoding.ASCII, "HelloThere\u00FF", "HelloThere?");
            VerifyNetworkStreamWorks( true, Encoding.ASCII, "HelloThere\u00FF", "HelloThere?");
            strLoc = "Loc_4747u";
            strValue = "This is UTF8\u00FF" ;
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.UTF8, 9);
            sw2.Write( strValue );
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF8, false, 9);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals( strValue)) 
            {
                iCountErrors++;
                printerr( "Error_1y8xx! Incorrect string on stream");
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false, Encoding.UTF8, strValue, strValue);
            VerifyNetworkStreamWorks( true, Encoding.UTF8, strValue, strValue);
            strLoc = "Loc_28y7c";
            strValue = "This is UTF7\u00FF" ;
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.UTF7, 11);
            sw2.Write( strValue );
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.UTF7, false, 10);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(strValue)) 
            {
                iCountErrors++;
                printerr( "Error_2091x! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false, Encoding.UTF7, strValue, strValue);
            VerifyNetworkStreamWorks( true, Encoding.UTF7, strValue, strValue);
            strLoc = "Loc_98hcf";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.BigEndianUnicode, 20);
            strValue = "This is BigEndianUnicode\u00FF";
            sw2.Write(strValue);
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            Console.WriteLine(fs2.Length);
            sr2 = new StreamReader(fs2, Encoding.BigEndianUnicode, false, 15);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(strValue)) 
            {
                iCountErrors++;
                printerr( "Error_2g88t! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            VerifyNetworkStreamWorks( false, Encoding.BigEndianUnicode, strValue, strValue);
            VerifyNetworkStreamWorks( true, Encoding.BigEndianUnicode, strValue, strValue);
            strLoc = "Loc_48y8d";
            fs2 = new FileStream(filName, FileMode.Create);	
            sw2 = new StreamWriter(fs2, Encoding.Unicode, 50000);
            strValue = "This is Unicode\u00FF";
            sw2.Write(strValue);
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2, Encoding.Unicode, false, 16);
            str2 = sr2.ReadToEnd();
            iCountTestcases++;
            if(!str2.Equals(strValue)) 
            {
                iCountErrors++;
                printerr( "Error_9877i! Incorrect string on stream=="+str2);
            }
            sr2.Close();			
            VerifyNetworkStreamWorks( false, Encoding.Unicode, strValue, strValue);
            VerifyNetworkStreamWorks( true, Encoding.Unicode, strValue, strValue);
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
    private void VerifyNetworkStreamWorks( bool bValue, Encoding encValue, String str1, String str2)
    {
        iCountTestcases++;
        detectEncodingFromByteOrderMarks = bValue ;
        enc = encValue ;
        strWrite = str1 ;
        strResult = str2;
        m_PortSetEvent.Reset();
        Thread tcpListenerThread = new Thread(new ThreadStart(Co5619ctor_stream_enc_b_i.StartListeningTcp));
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
            sr = new StreamReader(ns , enc, detectEncodingFromByteOrderMarks, 100);
            String strRead = sr.ReadLine();
            Console.WriteLine("read string..." + strRead );
            if(strRead != strResult) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_66677! Incorrect string read");
            }
            ns.Close();
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
        StreamWriter sr = new StreamWriter( stream , enc, 5);
        sr.WriteLine( strWrite );
        sr.Flush();
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5619ctor_stream_enc_b_i cbA = new Co5619ctor_stream_enc_b_i();
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
