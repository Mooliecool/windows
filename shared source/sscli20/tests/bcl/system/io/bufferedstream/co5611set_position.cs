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
public class Co5611set_Position
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BufferedStream.Flush()";
    public static String s_strTFName        = "Co5611set_Position.cs";
    public static String s_strTFAbbrev      = "Co5611";
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
            FileStream fs2;
            StreamWriter sw2;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_9289x";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            iCountTestcases++;
            try 
            {
                bs2.Position = -2;
                iCountErrors++;
                printerr( "Error_190x9! Expected exception not thrown");
            } 
            catch (ArgumentOutOfRangeException aexc) 
            {
                printinfo("Info_9199x! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t589v! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_27yxc";
            fs2 = new FileStream(filName, FileMode.Create);
            bs2 = new BufferedStream(fs2);
            bs2.SetLength(50);
            bs2.Position = 50;
            sw2 = new StreamWriter(bs2);
            for(char c = 'a' ; c < 'f' ; c++) 
                sw2.Write(c);
            sw2.Flush();
            iCountTestcases++;
            if(fs2.Length != 55) 
            {
                iCountErrors++;
                printerr( "Error_389xd! Incorrect stream length=="+fs2.Length);
            }
            iCountTestcases++;
            if(bs2.Position != 55) 
            {
                iCountErrors++;
                printerr( "Error_3y8t3! Incorrect position=="+bs2.Position);
            }
            bs2.SetLength(30);
            iCountTestcases++;
            if(bs2.Length != 30) 
            {
                iCountErrors++;
                printerr( "Error_28xye! Incorrect length=="+bs2.Length);
            }
            iCountTestcases++;
            if(bs2.Position != 30) 
            {
                iCountErrors++;
                printerr( "Error_3989a! Incorrect position=="+bs2.Position);
            }
            bs2.SetLength(100);
            bs2.Position = 100;
            iCountTestcases++;
            if(bs2.Length != 100) 
            {
                iCountErrors++;
                printerr( "Error_2090x! Incorrect length=="+bs2.Length);
            }
            iCountTestcases++;
            if(bs2.Position != 100) 
            {
                iCountErrors++;
                printerr( "Error_1987t! Incorrect position=="+bs2.Position);
            }
            bs2.Flush();
            bs2.SetLength(50);
            iCountTestcases++;
            if(bs2.Length != 50) 
            {
                iCountErrors++;
                printerr( "Error_10x88! Incorrect length=="+bs2.Length);
            }
            iCountTestcases++;
            if(bs2.Position != 50) 
            {
                iCountErrors++;
                printerr( "Error_738gb! Incorrect position=="+bs2.Position);
            }
            bs2.Position = 25;
            iCountTestcases++;
            if(bs2.Position != 25) 
            {
                iCountErrors++;
                printerr( "Error_298yx! Incorrect position=="+bs2.Position);
            }
            bs2.Close();
            strLoc = "Loc_99189";
            fs2 = new FileStream(filName, FileMode.Create);
            bs2 = new BufferedStream(fs2);
            iCountTestcases++;
            try 
            {
                bs2.Position = Int64.MaxValue;
                bs2.Write(new Byte[]{1}, 0, 1);
            } 
            catch (IOException exc) 
            {
                iCountErrors++;
                printerr( "Error_4443! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200fu! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5611set_Position.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
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
        BufferedStream bs = new BufferedStream( stream);
        try
        {
            bs.Position = 100;
            iCountErrors++;
            Console.WriteLine( "Error_8888! Expecte exception not occured ");
        } 
        catch( NotSupportedException ) 
        {
        } 
        catch( Exception )
        {
            iCountErrors++;
            Console.WriteLine( "Error_434234! Able to set the Network stream position ");
        } 
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5611set_Position cbA = new Co5611set_Position();
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
