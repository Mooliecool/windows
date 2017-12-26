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
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.Net;
using System.Runtime.Serialization.Formatters.Binary;
public class Co5632ctor_stream_enc
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader(Stream)";
    public static String s_strTFName        = "Co5632ctor_stream_enc.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static Encoding enc              = Encoding.ASCII ;
    public static int iPortNumber           = 0 ;
    public static int iCountErrors          = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            MemoryStream ms2;
            FileStream fs2;
            BinaryReader sr2;
            StreamWriter sw2;
            String str2;
            if(File.Exists("Co5632Test.tmp"))
                File.Delete("Co5632Test.tmp");			
            strLoc = "Loc_98yv7";
            iCountTestcases++;
            try 
            {
                sr2 = new BinaryReader((Stream)null, Encoding.UTF8);
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
                sr2 = new BinaryReader(new MemoryStream(), null);
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
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.ASCII);
            sw2.Write("HelloThere\u00FF");
            sw2.Close();
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2, Encoding.ASCII);
            str2 = new String(sr2.ReadChars(11));
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_298xw! Incorrect String interpreted");
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.ASCII);
            sw2.Write("HelloThere\u00FF");
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2, Encoding.ASCII);
            str2 = new String(sr2.ReadChars(11));
            iCountTestcases++;
            if(!str2.Equals("HelloThere?")) 
            {
                iCountErrors++;
                printerr( "Error_298xw! Incorrect String interpreted");
            }
            sr2.Close();
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5632ctor_stream_enc.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            //Allow abort to complete.
            tcpListenerThread.Join();
            strLoc = "Loc_4747u";
            if(File.Exists("Co5632Test.tmp"))
                File.Delete("Co5632Test.tmp");			
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.UTF8);
            sw2.Write("This is UTF8\u00FF");
            sw2.Close();
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2, Encoding.UTF8);
            str2 = new String(sr2.ReadChars(14));
            str2 = str2.Substring( 1 );
            iCountTestcases++;
            if(!str2.Equals("This is UTF8\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_1y8xx! Incorrect string on stream" + str2 );
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.UTF8);
            sw2.Write("This is UTF8\u00FF");
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2, Encoding.UTF8);
            str2 = new String(sr2.ReadChars(14));
            str2 = str2.Substring( 1 );
            iCountTestcases++;
            if(!str2.Equals("This is UTF8\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_1y843! Incorrect string on stream" + str2 );
            }
            sr2.Close();
            iCountTestcases++;
            enc = Encoding.UTF8 ;
            m_PortSetEvent.Reset();
            tcpListenerThread = new Thread(new ThreadStart(Co5632ctor_stream_enc.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 ); 
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            //Allow abort to complete.
            tcpListenerThread.Join();
            strLoc = "Loc_28y7c";
            if(File.Exists("Co5632Test.tmp"))
                File.Delete("Co5632Test.tmp");			
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.UTF7);
            sw2.Write("This is UTF7\u00FF");
            sw2.Flush();
            sw2.Close();
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2, Encoding.UTF7);
            str2 = new String(sr2.ReadChars(13));
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
            sr2 = new BinaryReader(ms2, Encoding.UTF7);
            str2 = new String(sr2.ReadChars(13));
            iCountTestcases++;
            if(!str2.Equals("This is UTF7\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_5f98y! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            iCountTestcases++;
            enc = Encoding.UTF7 ;
            m_PortSetEvent.Reset();
            tcpListenerThread = new Thread(new ThreadStart(Co5632ctor_stream_enc.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            //Allow abort to complete
            tcpListenerThread.Join();
            strLoc = "Loc_98hcf";
            if(File.Exists("Co5632Test.tmp"))
                File.Delete("Co5632Test.tmp");			
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.BigEndianUnicode);
            sw2.Write("This is BigEndianUnicode\u00FF");
            sw2.Close();
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2, Encoding.BigEndianUnicode);
            str2 = new String(sr2.ReadChars(26));
            str2 = str2.Substring( 1 );
            iCountTestcases++;
            if(!str2.Equals("This is BigEndianUnicode\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_2g88t! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.BigEndianUnicode);
            sw2.Write("This is BigEndianUnicode\u00FF");
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2, Encoding.BigEndianUnicode);
            str2 = new String(sr2.ReadChars(26));
            str2 = str2.Substring( 1 );
            iCountTestcases++;
            if(str2 != "This is BigEndianUnicode\u00FF") 
            {
                iCountErrors++;
                printerr( "Error_8f7yv! Incorrect string on stream=="+str2);
            }
            sr2.Close();
            iCountTestcases++;
            enc = Encoding.BigEndianUnicode ;
            m_PortSetEvent.Reset();
            tcpListenerThread = new Thread(new ThreadStart(Co5632ctor_stream_enc.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            //Allow abort to complete
            tcpListenerThread.Join();
            strLoc = "Loc_48y8d";
            if(File.Exists("Co5632Test.tmp"))
                File.Delete("Co5632Test.tmp");			
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.Unicode);
            sw2.Write("This is Unicode\u00FF");
            sw2.Close();
            fs2 = new FileStream("Co5632Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2, Encoding.Unicode);
            str2 = new String(sr2.ReadChars(17));
            str2 = str2.Substring( 1 );
            iCountTestcases++;
            if(!str2.Equals("This is Unicode\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_2g88t! Incorrect string on stream=="+str2);
            }
            sr2.Close();			
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2, Encoding.Unicode);
            sw2.Write("This is Unicode\u00FF");
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2, Encoding.Unicode);
            str2 = new String(sr2.ReadChars(17));
            str2 = str2.Substring( 1 );
            iCountTestcases++;
            if(!str2.Equals("This is Unicode\u00FF")) 
            {
                iCountErrors++;
                printerr( "Error_f897h! Incorrect string on stream=="+str2);
            }
            sr2.Close();			
            iCountTestcases++;
            enc = Encoding.Unicode ;
            m_PortSetEvent.Reset();
            tcpListenerThread = new Thread(new ThreadStart(Co5632ctor_stream_enc.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            //Allow abort to complete
            tcpListenerThread.Join();
            if(File.Exists("Co5632Test.tmp"))
                File.Delete("Co5632Test.tmp");			 
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString() );
            return false;
        }
    }
    public static void StartListeningTcp() 
    {
        TcpThreadListener listener = null;
        Socket s = null;
        NetworkStream ns = null;
        BinaryReader bs = null;
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
            bs = new BinaryReader( ns , enc);   
            Console.WriteLine( enc.ToString() );
            int iNumBytesRead = bs.Read(new Byte[1024], 0, 1024) ;
            if ( iNumBytesRead != 1024 )
                iCountErrors++ ;
        }

        catch(Exception e) 
        {
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
        Byte[] bts = new Byte[1024];
        for(int iLoop = 0 ; iLoop < 1024 ; iLoop++ )
            bts[iLoop] =  65 ;
        stream.Write( bts , 0, 1024 );
    }
    public void printerr ( String err )
    {
        Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
    }
    public void printinfo ( String info )
    {
        Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5632ctor_stream_enc cbA = new Co5632ctor_stream_enc();
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
