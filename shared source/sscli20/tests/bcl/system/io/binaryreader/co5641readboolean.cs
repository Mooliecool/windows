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
public class Co5641ReadBoolean
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BinaryWriter(Stream)";
    public static String s_strTFName        = "Co5641ReadBoolean.cs";
    public static String s_strTFAbbrev      = "Co5641";
    public static String s_strTFPath        = Environment.CurrentDirectory.ToString();
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
            BinaryWriter dw2 = null;
            Stream fs2 = null;
            BinaryReader dr2 = null;
            FileInfo fil2 = null;			
            MemoryStream mstr = null;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_785hv";
            fil2 = new FileInfo(filName);
            fs2 = fil2.Open(FileMode.Create);
            dw2 = new BinaryWriter(fs2);
            dw2.Write(true);
            dw2.Flush();
            fs2.Close();			
            strLoc = "Loc_987hg";
            fs2 = fil2.Open(FileMode.Open);
            dr2 = new BinaryReader(fs2);
            iCountTestcases++;
            if(!dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr("Error_287gy! Correct value not written");
            }
            fs2.Close();
            fil2.Delete();
            strLoc = "Loc_98yvh";
            mstr = new MemoryStream();
            dw2 = new BinaryWriter(mstr);
            dw2.Write(true);
            dw2.Flush();
            mstr.Position = 0;
            dr2 = new BinaryReader(mstr);
            iCountTestcases++;
            if(!dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_298yx! Incorrect value on stream");
            }
            dr2.Close();
            strLoc = "Loc_98yss";
            mstr = new MemoryStream();
            dw2 = new BinaryWriter(mstr);
            dw2.Write(false);
            dw2.Write(false);
            dw2.Write(true);
            dw2.Write(false);
            dw2.Write(true);
            dw2.Write(5);
            dw2.Write(0);		
            dw2.Flush();
            mstr.Position = 0;
            dr2 = new BinaryReader(mstr);
            iCountTestcases++;			
            if(dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_948yg! Incorrect value in stream");
            } 
            iCountTestcases++;
            if(dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_t598h! Incorrect value in stream");
            }
            iCountTestcases++;
            if(!dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_9084b! Incorrect value in stream");
            } 
            iCountTestcases++;
            if(dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_209uc! Incorrect value in stream");
            }
            iCountTestcases++;
            if(!dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_98y8c! Incorrect value in stream");
            }
            iCountTestcases++;
            if(!dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_198xy! Incorrect value in stream");
            } 
            iCountTestcases++;
            if(dr2.ReadBoolean()) 
            {
                iCountErrors++;
                printerr( "Error_39t8g! Incorrect value in stream");
            }
            dr2.Close();
            mstr.Close();		
            strLoc = "Loc_098gy";
            iCountTestcases++;
            try 
            {
                dw2.Write(true);
                iCountErrors++;
                printerr( "Error_1908u! Expected exception not thrown");
            } 
            catch (ObjectDisposedException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_309t8! Incorrect exception thrown, exc=="+exc.ToString());
            }
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5641ReadBoolean.StartListeningTcp));
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
        NetworkStream ns = null;
        BinaryReader br = null;

        try 
        {
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Socket s = listener.AcceptSocket();
            ns = new NetworkStream(s);
            br = new BinaryReader( ns );   
            bool bValue =  true ;
            for(int i = 0 ; i < 100 ; i++) 
            {
                if(bValue != br.ReadBoolean()) 
                {
                    iCountErrors++;
                    Console.WriteLine( "Error_3423_"+i+"! Expected=="+ bValue+", got=="+(! bValue));
                }
                bValue = ! bValue ;
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
            if(br != null)
            {
                br.Close();
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
        bool bValue = false ;
        TcpClient tcpClient = new TcpClient();
        IPEndPoint epSend = null;
        IPAddress sendAddress = IPAddress.Parse(address);			
        epSend = new IPEndPoint(sendAddress, iPortNumber);
        tcpClient.Connect(epSend);
        Stream stream = tcpClient.GetStream();
        BinaryWriter bw = new BinaryWriter( stream );
        for(int i = 0 ; i < 100 ; i++)
        {
            bValue =  ! bValue ;
            bw.Write(bValue);
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5641ReadBoolean cbA = new Co5641ReadBoolean();
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
