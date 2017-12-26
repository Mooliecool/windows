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
public class Co5626Read
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader(String, Encoding)";
    public static String s_strTFName        = "Co5626Read.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static Char[] chArr ;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            StreamReader sr2;
            StreamWriter sw2;
            MemoryStream ms2;
            FileStream fs2;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);			
            chArr = new Char[]{
                                  Char.MinValue
                                  ,Char.MaxValue
                                  ,'\t'
                                  ,' '
                                  ,'$'
                                  ,'@'
                                  ,'#'
                                  ,'\0'
                                  ,'\v'
                                  ,'\''
                                  ,'\u3190'
                                  ,'\uC3A0'
                                  ,'A'
                                  ,'5'
                                  ,'\uFE70' 
                                  ,'-'
                                  ,';'
                                  ,'\u00E6'
                              };
            try 
            {
                ms2 = new MemoryStream();
                sw2 = new StreamWriter(ms2);
                for(int i = 0 ; i < chArr.Length ; i++)
                    sw2.Write(chArr[i]);
                sw2.Flush();
                ms2.Position = 0;
                sr2 = new StreamReader(ms2);
                Int32 tmp = 0;
                for(int i = 0 ; i < chArr.Length ; i++) 
                {
                    iCountTestcases++;
                    if((tmp = sr2.Read()) != (Int32)chArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_298vc_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
                    }
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_298yg! Unexpected exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_9848v";
            try 
            {
                fs2 = new FileStream(filName, FileMode.Create);
                sw2 = new StreamWriter(fs2);
                for(int i = 0 ; i < chArr.Length ; i++)
                    sw2.Write(chArr[i]);
                sw2.Flush();
                sw2.Close();
                sr2 = new StreamReader(filName);
                Int32 tmp = 0;
                for(int i = 0 ; i < chArr.Length ; i++) 
                {
                    iCountTestcases++;
                    if((tmp = sr2.Read()) != (Int32)chArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_98yv8!_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
                    }
                }
                sr2.Close();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_698y7! Unexpected exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_958hb";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.UTF8, 4);
            sw2.Write("\u00FF\u00FE");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new StreamReader(fs2);
            iCountTestcases++;
            if(sr2.Read() != 0xFF) 
            {
                iCountErrors++;
                printerr( "Error_t8yc! Incorrect character read");
            }
            if(sr2.Read() != 0xFE) 
            {
                iCountErrors++;
                printerr("Error_8yb78! Incorrect character read");
            }
            sr2.Close();
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5626Read.StartListeningTcp));
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
        StreamReader sr = null;
        try 
        {
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Socket s = listener.AcceptSocket();
            ns = new NetworkStream(s);
            sr = new StreamReader(ns);
            try 
            {
                Int32 tmp = 0;
                for(int i = 0 ; i < chArr.Length ; i++) 
                {
                    if((tmp = sr.Read()) != (Int32)chArr[i]) 
                    {
                        iCountErrors++;
                        Console.WriteLine( "Error_9090_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
                    }
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_5555! Incorrect exception thrown, exc=="+exc.ToString());
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
            if(sr != null)
            {
                sr.Close();
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
        for(int i = 0 ; i < chArr.Length ; i++)
            sw.Write(chArr[i]);
        sw.Flush();
        stream.Close();
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5626Read cbA = new Co5626Read();
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
