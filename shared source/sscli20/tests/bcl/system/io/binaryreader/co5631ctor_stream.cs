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
public class Co5631ctor_stream
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BinaryReader(Stream)";
    public static String s_strTFName        = "Co5631ctor_stream.cs";
    public static String s_strTFAbbrev      = "Co5631";
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            MemoryStream ms2;
            FileStream fs2;
            BinaryReader sr2;
            String str2;
            if(File.Exists("Co5631Test.tmp"))
                File.Delete("Co5631Test.tmp");			
            strLoc = "Loc_1989x";
            iCountTestcases++;
            try 
            {
                sr2 = new BinaryReader((Stream)null);
                iCountErrors++;
                printerr( "Error_298vb! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo("Info_7887g! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t98bh! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_g37jb";
            fs2 = new FileStream("Co5631Test.tmp", FileMode.Create, FileAccess.Write);
            iCountTestcases++;
            try 
            {
                sr2 = new BinaryReader(fs2);
                iCountErrors++;
                printerr("Error_07t8b! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_09877! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_9t798! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();
            strLoc = "Loc_gfu8g";
            ms2 = new MemoryStream();
            ms2.Close();
            iCountTestcases++;
            try 
            {
                sr2 = new BinaryReader(ms2);
                iCountErrors++;
                printerr( "Error_9578b! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_6t877! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_7698b! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_787yg";
            iCountTestcases++;
            ms2 = new MemoryStream();
            ms2.Write(new Byte[]{65,66,67,68}, 0, 4);
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            str2 = new String(sr2.ReadChars(4));
            if(!str2.Equals("ABCD")) 
            {
                iCountErrors++;
                printerr( "Error_6t897! Incorrect string read");
            }
            sr2.Close();
            strLoc = "Loc_48yvd";
            iCountTestcases++;
            fs2 = new FileStream("Co5631Test.tmp", FileMode.Create);
            fs2.Write(new Byte[]{65,66,67,68},0,4);
            fs2.Position = 0;
            sr2 = new BinaryReader(fs2);
            str2 = new String(sr2.ReadChars(4));
            if(!str2.Equals("ABCD")) 
            {
                iCountErrors++;
                printerr( "Error_9688y! Incorrect string read");
            }
            fs2.Close();				
            if(File.Exists("Co5631Test.tmp"))
                File.Delete("Co5631Test.tmp");
            strLoc = "Loc_44444";
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5631ctor_stream.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );  // wait for listener to start
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 3000 );                      
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
    public static void StartListeningTcp() 
    {
        //Use 0 to cause socket to use a free port number.
        TcpThreadListener listener = new TcpThreadListener(0);
        NetworkStream ns = null;
        BinaryReader bs = null;
        try 
        {
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Socket s = listener.AcceptSocket();
            ns = new NetworkStream(s);
            bs = new BinaryReader( ns );
            Console.WriteLine( bs.Read(new Byte[1024], 0, 1024) );
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
        Co5631ctor_stream cbA = new Co5631ctor_stream();
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
