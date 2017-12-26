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
public class Co5614Seek_i64_so
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BufferedStream.DataAvailable";
    public static String s_strTFName        = "Co5614Seek_i64_so.cs";
    public static String s_strTFAbbrev      = "Co5614";
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
            Int64 pos;
            if(File.Exists("Co5614Test.tmp"))
                File.Delete("Co5614Test.tmp");
            strLoc = "Loc_95vy8";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            bs2.Write(new Byte[]{1,2,3,4}, 0, 4);
            bs2.Flush();
            iCountTestcases++;
            try 
            {
                bs2.Seek(-2, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_98yvc! Expected exception not thrown");
            } 
            catch (IOException aexc) 
            {
                printinfo("Info_20u90! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_f3099! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(bs2.Position != 4) 
            {
                iCountErrors++;
                printerr( "Error_298hx! Position set=="+bs2.Position);
            }
            bs2.Position = 1;
            iCountTestcases++;
            try 
            {
                bs2.Seek(-2, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_0190cj! Expected exception not thrown");
            } 
            catch (IOException aexc) 
            {
                printinfo("Info_98yg9! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_98t8b! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(bs2.Position != 1) 
            {
                iCountErrors++;
                printerr( "Error_29887! Position set=="+bs2.Position);
            }
            bs2.Close();
            strLoc = "Loc_98yvh";
            fs2 = new FileStream("Co5614Test.tmp", FileMode.Create);
            bs2 = new BufferedStream(fs2);
            bs2.Write(new Byte[]{1,2,3,4},0,4);
            bs2.Flush();
            iCountTestcases++;
            try 
            {
                bs2.Seek(-2, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_019uc! Expected exception not thrown");
            } 
            catch (IOException aexc) 
            {
                printinfo("Info_78y7g! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_099gn! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            iCountTestcases++;
            if(bs2.Position != 4) 
            {
                iCountErrors++;
                printerr( "Error_9t8yb! Position set=="+bs2.Position);
            }
            bs2.Position = 1;
            iCountTestcases++;
            try 
            {
                bs2.Seek(-2, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_98ycn! Expected exception not thrown");
            } 
            catch (IOException aexc) 
            {
                printinfo("Info_t6598! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2896g! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(bs2.Position != 1) 
            {
                iCountErrors++;
                printerr("Error_2989b! POsition set=="+bs2.Position);
            }
            fs2.Close();
            strLoc = "Loc_9t87y";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            bs2.Write(new Byte[]{1,2,3,4}, 0,4);
            bs2.Flush();
            iCountTestcases++;
            try 
            {
                bs2.Seek(2, (SeekOrigin)(-1));
                iCountErrors++;
                printerr( "Error_9857! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_t86yv! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_9838e! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(bs2.Position != 4) 
            {
                iCountErrors++;
                printerr( "Error_98v8b! Position set=="+bs2.Position);
            } 
            iCountTestcases++;
            try 
            {
                bs2.Seek(2, (SeekOrigin)(1000));
                iCountErrors++;
                printerr( "Error_928ch! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_7t87y! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t98bn! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(bs2.Position != 4) 
            {
                iCountErrors++;
                printerr( "Error_989bj! Position set=="+bs2.Position);
            }
            bs2.Close();
            strLoc = "loc_8ythb";
            fs2 = new FileStream("Co5614Test.tmp", FileMode.Create);
            bs2 = new BufferedStream(fs2);
            bs2.Write(new Byte[]{1,2,3,4}, 0,4);
            bs2.Flush();
            iCountTestcases++;
            try 
            {
                bs2.Seek(2, (SeekOrigin)(-1));
                iCountErrors++;
                printerr( "Error_98y8c! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_900vj! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr("Error_098y8! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(bs2.Position != 4) 
            {
                iCountErrors++;
                printerr( "Error_9g7b7! Position set=="+bs2.Position);
            }
            iCountTestcases++;
            try 
            {
                bs2.Seek(2, (SeekOrigin)1000);
                iCountErrors++;
                printerr( "Error_t98bh! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_t87yb! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_1987v Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(bs2.Position != 4) 
            {
                iCountErrors++;
                printerr( "ERror_g7y7b! Position setc=="+bs2.Position);
            }
            bs2.Close();
            strLoc = "Loc_900ub";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            bs2.Write(new Byte[]{1,2,3,4,5,6,7,8},0, 8);
            bs2.Flush();
            bs2.Position = 2;
            pos = bs2.Seek(-1, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 1) 
            {
                iCountErrors++;
                printerr( "Error_78hbv! Return incorrect=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 1) 
            {
                iCountErrors++;
                printerr( "Error_958uv! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(5, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 6) 
            {
                iCountErrors++;
                printerr( "Error_8ghbh! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 6) 
            {
                iCountErrors++;
                printerr( "Error_t98yb! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(7, SeekOrigin.Begin);
            iCountTestcases++;
            if(pos != 7) 
            {
                iCountErrors++;
                printerr( "Error_t57h7! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 7) 
            {
                iCountErrors++;
                printerr( "Error_08998! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(-5, SeekOrigin.End);
            iCountTestcases++;
            if(pos != 3) 
            {
                iCountErrors++;
                printerr( "Errror_g87hv! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 3) 
            {
                iCountErrors++;
                printerr( "Error_98hbb! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(2, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 5) 
            {
                iCountErrors++;
                printerr( "ERror_287hg! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 5) 
            {
                iCountErrors++;
                printerr( "Error_f48y8! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(0, SeekOrigin.End);
            iCountTestcases++;
            if(pos != 8) 
            {
                iCountErrors++;
                printerr( "ERror_8970l! Incorrect return=="+pos);
            } 
            iCountTestcases++;
            if(bs2.Position != 8) 
            {
                iCountErrors++;
                printerr( "Error_787tb! Incorrect position=="+bs2.Position);
            }
            bs2.Close();
            strLoc = "Loc_900gu";
            fs2 = new FileStream("Co5614Test.tmp", FileMode.Create);
            bs2 = new BufferedStream(fs2);
            bs2.Write(new Byte[]{1,2,3,4,5,6,7,8},0, 8);
            bs2.Flush();
            bs2.Position = 2;
            pos = bs2.Seek(-1, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 1) 
            {
                iCountErrors++;
                printerr( "Error_4897b! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 1) 
            {
                iCountErrors++;
                printerr( "Error_g8uy8! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(5, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 6) 
            {
                iCountErrors++;
                printerr( "Error_87yg7! Incorrec return, pos=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 6) 
            {
                iCountErrors++;
                printerr( "Error_g498y! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(7, SeekOrigin.Begin);
            iCountTestcases++;
            if(pos != 7) 
            {
                iCountErrors++;
                printerr( "ERror_78h91! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 7) 
            {
                iCountErrors++;
                printerr( "Error_10978! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(-5, SeekOrigin.End);
            iCountTestcases++;
            if(pos != 3) 
            {
                iCountErrors++;
                printerr( "Error_2908y! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 3) 
            {
                iCountErrors++;
                printerr( "Error_984yb! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(2, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 5) 
            { 
                iCountErrors++;
                printerr( "Error_09utg! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 5) 
            {
                iCountErrors++;
                printerr( "Error_98g87! Incorrect position=="+bs2.Position);
            }
            bs2.SetLength(105);
            pos = bs2.Seek(100, SeekOrigin.Current);
            if(pos != 105) 
            {
                iCountErrors++;
                printerr( "Error_b858j! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 105) 
            {
                iCountErrors++;
                printerr( "Error_48yvb! Incorrect position=="+bs2.Position);
            }
            pos = bs2.Seek(0, SeekOrigin.Current);
            if(pos != 105) 
            {
                iCountErrors++;
                printerr( "Error_g987y! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(bs2.Position != 105) 
            {
                iCountErrors++;
                printerr( "ERror_g587h! Incorrec position=="+bs2.Position);
            }
            bs2.Close();
            strLoc = "Loc_897hg";
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            bs2.Close();
            iCountTestcases++;
            try 
            {
                bs2.Seek(0,SeekOrigin.Current);
                iCountErrors++;
                printerr( "Error_249yb! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_2897b! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_g439y! Incorrect exception thrown, exc=="+exc.ToString());
            }
            memstr2 = new MemoryStream();
            bs2 = new BufferedStream(memstr2);
            memstr2.Close();
            iCountTestcases++;
            try 
            {
                bs2.Seek(0, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_g9h8b! Expected exception not thrown");
            } 
            catch (NotSupportedException iexc) 
            {
                printinfo("Info_8g7hb! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_989vh! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_98hvg";
            fs2 = new FileStream("Co5614Test.tmp", FileMode.Create);
            bs2 = new BufferedStream(fs2);
            fs2.Close();
            iCountTestcases++;
            try 
            {
                bs2.Seek(0, SeekOrigin.Current);
                iCountErrors++;
                printerr( "Error_9h8g8! Expected exception not thrown");
            } 
            catch (NotSupportedException iexc) 
            {
                printinfo("Info_g98b8! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr("Error_09u19! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2 = new FileStream("Co5614Test.tmp", FileMode.Create);
            bs2 = new BufferedStream(fs2);
            bs2.Close();
            iCountTestcases++;
            try 
            {
                bs2.Seek(0, SeekOrigin.End);
                iCountErrors++;
                printerr( "Error_t87yb! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_g897h! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_98f8h! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5614Seek_i64_so.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );
            if(File.Exists("Co5614Test.tmp"))
                File.Delete("Co5614Test.tmp");
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
        bs.Write(new Byte[]{0,1,2}, 0, 3);
        try
        {
            bs.Seek( 1, SeekOrigin.Begin);
            iCountErrors++;
            Console.WriteLine( "Error_8888! Expecte exception not occured ");
        } 
        catch( NotSupportedException iexc) 
        {
            Console.WriteLine("INFO: Expected exception occured, iexc == " + iexc.Message );
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
        Co5614Seek_i64_so cbA = new Co5614Seek_i64_so();
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
