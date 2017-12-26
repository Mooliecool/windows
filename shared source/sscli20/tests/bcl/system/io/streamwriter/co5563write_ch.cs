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
public class Co5563Write_ch
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "StreamWriter.Write(Char)";
	public static String s_strTFName        = "Co5563Write_ch.cs";
	public static String s_strTFAbbrev      = "Co5563";
	public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static char[] chArr ;
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
		int iCountTestcases = 0;
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
		try {
			MemoryStream ms = new MemoryStream();
			StreamWriter sw = new StreamWriter(ms);
			StreamReader sr;
			for(int i = 0 ; i < chArr.Length ; i++)
				sw.Write(chArr[i]);
			sw.Flush();
			ms.Position = 0;
			sr = new StreamReader(ms);
			Int32 tmp = 0;
			for(int i = 0 ; i < chArr.Length ; i++) {
				iCountTestcases++;
				if((tmp = sr.Read()) != (Int32)chArr[i]) {
					iCountErrors++;
					printerr( "Error_298vc_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
				}
			}
		} catch (Exception exc) {
			iCountErrors++;
			printerr( "Error_298yg! Unexpected exception thrown, exc=="+exc.ToString());
		}
        iCountTestcases++;
        m_PortSetEvent.Reset();
        Thread tcpListenerThread = new Thread(new ThreadStart(Co5563Write_ch.StartListeningTcp));
        tcpListenerThread.Start();
        Console.WriteLine("Listening");
        Thread.Sleep( 1000 );
        m_PortSetEvent.WaitOne();
        Teleport("127.0.0.1");
        Thread.Sleep( 1000 );
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
    public static void StartListeningTcp() {
        TcpThreadListener listener = new TcpThreadListener(0);
        NetworkStream ns = null;
        StreamReader sr = null;

        try {
            listener.Start();
            IPEndPoint ipe = (IPEndPoint) listener.LocalEndpoint;
            Interlocked.Exchange(ref iPortNumber, ipe.Port);
            Console.WriteLine("Using port: {0}", iPortNumber);
            m_PortSetEvent.Set();

            Socket s = listener.AcceptSocket();
            ns = new NetworkStream(s);
			sr = new StreamReader(ns);
			try {
                Int32 tmp = 0;
                for(int i = 0 ; i < chArr.Length ; i++) {
                    if((tmp = sr.Read()) != (Int32)chArr[i]) {
                        iCountErrors++;
                        Console.WriteLine( "Error_34324_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
                    }
                }
			} catch (Exception exc) {
				iCountErrors++;
				Console.WriteLine( "Error_5555! Incorrect exception thrown, exc=="+exc.ToString());
			}
            Console.WriteLine("We are done with the listening");
        }
        catch(Exception e) {
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
    internal class TcpThreadListener : TcpListener {
        public TcpThreadListener(int port) : base(port) {
        }
        internal void Shutdown() {
            if ( Server != null )
                Server.Close();
        }	
    }
    public static void Teleport(string address) {
        TcpClient tcpClient = new TcpClient();
        IPEndPoint epSend = null;
        IPAddress sendAddress = IPAddress.Parse(address);			
        epSend = new IPEndPoint(sendAddress, iPortNumber);
        tcpClient.Connect(epSend);
        Stream stream = tcpClient.GetStream();
        StreamWriter sw = new StreamWriter( stream );
        for(int i = 0 ; i < chArr.Length ; i++)
            sw.Write(chArr[i]);
        stream.Flush();
        sw.Close();
        stream.Close();
    }
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co5563Write_ch cbA = new Co5563Write_ch();
		try {
			bResult = cbA.runTest();
		} catch (Exception exc_main){
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
