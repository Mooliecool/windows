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
public class Co5595Close
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamWriter.GetBaseStream";
    public static String s_strTFName        = "Co5595Close.cs";
    public static String s_strTFAbbrev      = "Co5595";
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static string strValue = "This is a testing string" ;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        try
        {
            StreamWriter sw2;
            StreamReader sr2;
            strLoc = "loc_t458x";
            sw2 = new StreamWriter(new MemoryStream());
            sw2.Close();
            iCountTestcases++;
            try 
            {
                sw2.Write('A');
                iCountErrors++;
                printerr( "Error_498yc! Was able to write to a closed stream");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_298yx! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2987b! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                sw2.WriteLine("hello");
                iCountErrors++;
                printerr( "Error_289yx! Was able to write to a closed stream");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_e999s! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_98ysy! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                sw2.Flush();
                iCountErrors++;
                printerr( "Error_109ux! Was able to write to a closed stream");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_198xj! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_298yy! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(sw2.BaseStream != null) 
            {
                iCountErrors++;
                printerr( "Error_289c8! Expected null stream on closed writer");
            }
            iCountTestcases++;
            try 
            {
                sw2.AutoFlush = true;
                iCountErrors++;
                printerr( "Error_3yg88! Was able to set flush on a closed stream");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_1994u! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_29s98! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_548c8";
            if(File.Exists("Co5595Test.tmp"))
                File.Delete("Co5595Test.tmp");
            sw2 = new StreamWriter("Co5595Test.tmp");
            sw2.Write("hello");
            sw2.Close();
            sr2 = new StreamReader("Co5595Test.tmp");
            String tmp = sr2.ReadToEnd();
            iCountTestcases++;
            if(!tmp.Equals("hello")) 
            {
                iCountErrors++;
                printerr( "Error_298b7! Close did not flush the stream");
            }
            sr2.Close();
            if(File.Exists("Co5595Test.tmp"))
                File.Delete("Co5595Test.tmp");
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5595Close.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
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
        StreamWriter sw = new StreamWriter( stream );
        sw.Close();
        try
        {
            sw.Write( strValue );
        } 
        catch( ObjectDisposedException ex)
        {
            Console.WriteLine( "Info_1111!!!! Expected exception occured... msg" + ex.Message );
        } 
        catch( Exception e)
        {
            iCountErrors++;
            Console.WriteLine( "Error_0000! Incorrect exception occured... msg" + e.Message);
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5595Close cbA = new Co5595Close();
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
