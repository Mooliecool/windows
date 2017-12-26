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
public class Co5604Close
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BufferedStream.Close()";
    public static String s_strTFName        = "Co5604Close.cs";
    public static String s_strTFAbbrev      = "Co5604";
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
            if(File.Exists("Co5604Test.tmp"))
                File.Delete("Co5604Test.tmp");
            strLoc = "Loc_857vi";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            bs2.Write(new Byte[]{65, 66}, 0, 2);
            bs2.Close();
            iCountTestcases++;
            try 
            {
                bs2.Flush();
                iCountErrors++;
                printerr("Error_2yc94! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_298uv! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_9t85y! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_09uf4";
            iCountTestcases++;
            try 
            {
                bs2.Write(new Byte[]{1,2,3}, 0, 3);
                iCountErrors++;
                printerr( "Error_129vc! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_27b99! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "ERror_0901v! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_2099x";
            iCountTestcases++;
            try 
            {
                bs2.Read(new Byte[3], 0, 3);
                iCountErrors++;
                printerr( "Error_209cx! Expectede exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_t7587! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_20g8j! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_01990";
            iCountTestcases++;
            try 
            {
                bs2.Seek(54, SeekOrigin.Current);
                iCountErrors++;
                printerr( "Error_0190j! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_g6798! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2998y! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_0939s";
            iCountTestcases++;
            try 
            {
                bs2.SetLength(100);
                iCountErrors++;
                printerr( "Error_209gb! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_989gh! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_0190x! Incorrect exception thrown, exc=="+exc.Message);
            }
            strLoc = "Loc_209ug";
            iCountTestcases++;
            if(bs2.CanRead) 
            {
                iCountErrors++;
                printerr( "Error_20109! Canread returned true for closed stream");
            }
            strLoc = "Loc_209ug";
            iCountTestcases++;
            if(bs2.CanWrite) 
            {
                iCountErrors++;
                printerr( "Error_10v90! CanWrite returned true for closed stream");
            }
            strLoc = "Loc_5987g";
            iCountTestcases++;
            if(bs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_1099v! CanSeek returned true for closed stream");
            }
            strLoc = "Loc_5878v";
            iCountTestcases++;
            try 
            {
                Int64 i = bs2.Length;
                iCountErrors++;
                printerr( "Error_019ux! Expectede exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo ("Info_109uc! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_f587h! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_8g68g";
            iCountTestcases++;
            try 
            {
                bs2.Position = 4;
                iCountErrors++;
                printerr( "Error_109xj! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_109c9! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_10r94! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_9g8yg";
            FileStream fs = new FileStream("Co5604Test.tmp", FileMode.Create);
            bs2 = new BufferedStream(fs);
            bs2.Write(new Byte[]{65,66,67,68,69,70}, 0, 6);
            bs2.Close();
            StreamReader sr2 = new StreamReader("Co5604Test.tmp");
            iCountTestcases++;
            if(!sr2.ReadToEnd().Equals("ABCDEF")) 
            {
                iCountErrors++;
                printerr( "Error_19009! Close did not flush");
            }
            sr2.Close();
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5604Close.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
            if(File.Exists("Co5604Test.tmp"))
                File.Delete("Co5604Test.tmp");
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
            bs.Close();
            try
            {
                bs.Flush();
                iCountErrors++;
                Console.WriteLine( "Error_8989!!! Expected exception not occured");
            } 
            catch (ObjectDisposedException iexc) 
            {
                Console.WriteLine( "Info_9898! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_0000! Incorrect exception thrown, exc=="+exc.ToString());
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
        Co5604Close cbA = new Co5604Close();
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
