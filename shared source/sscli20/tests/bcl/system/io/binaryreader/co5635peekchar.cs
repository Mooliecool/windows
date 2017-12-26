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
public class Co5635PeekChar
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BinaryReader.Peek()";
    public static String s_strTFName        = "Co5635PeekChar.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    public static int iCountErrors = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    public static Char[] chArr = new Char[]{
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
                                               ,'\n'
                                               ,'\r'
                                               ,'6'
                                               ,'T'
                                               ,';'
                                               ,'\u00E6'
                                           };
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            BinaryReader sr2;
            StreamWriter sw2;
            MemoryStream ms2;
            FileStream fs2;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);			
            try 
            {
                ms2 = new MemoryStream();
                sw2 = new StreamWriter(ms2);
                for(int i = 0 ; i < chArr.Length ; i++)
                    sw2.Write(chArr[i]);
                sw2.Flush();
                ms2.Position = 0;
                sr2 = new BinaryReader(ms2);
                Int32 tmp = 0;
                for(int i = 0 ; i < chArr.Length ; i++) 
                {
                    iCountTestcases++;
                    tmp = sr2.PeekChar();
                    if(tmp != (Int32)chArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_298vc_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
                    }
                    sr2.Read();
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
                fs2 = new FileStream(filName, FileMode.Open);
                sr2 = new BinaryReader(fs2);
                Int32 tmp = 0;
                for(int i = 0 ; i < chArr.Length ; i++) 
                {
                    iCountTestcases++;
                    if((tmp = sr2.PeekChar()) != (Int32)chArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_98yv8!_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+tmp);
                    }
                    sr2.Read();
                }
                sr2.Close();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_698y7! Unexpected exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_958hb";
            int temp = 0;
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2, Encoding.UTF8, 4);
            sw2.Write("\u00FF\u00FF");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new BinaryReader(fs2);
            temp = sr2.PeekChar(); 
            sr2.Read();
            iCountTestcases++;
            if((temp = sr2.PeekChar()) != 0xFF) 
            {
                iCountErrors++;
                printerr( "Error_t8yc! Incorrect character read" + temp);
            }
            sr2.Read();
            if((temp = sr2.PeekChar()) != 0xFF) 
            {
                iCountErrors++;
                printerr("Error_8yb78! Incorrect character read" + temp);
            }
            sr2.Read();
            iCountTestcases++;
            if(( temp = sr2.PeekChar()) != -1) 
            {
                iCountErrors++;
                printerr( "Error_987yg! Incorrect character" + temp);
            }
            sr2.Close();
            strLoc = "Loc_388yv";
            fs2 = new FileStream(filName, FileMode.Create);
            sr2 = new BinaryReader(fs2);
            iCountTestcases++;
            if(sr2.PeekChar() != -1) 
            {
                iCountErrors++;
                printerr( "Error_398yv! Incorrect return value at end of stream");
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sr2 = new BinaryReader(ms2);
            iCountTestcases++;
            if(sr2.PeekChar() != -1) 
            {
                iCountErrors++;
                printerr( "Error_38vh8! Incorrect return value at the end of stream");
            }
            sr2.Close();
            strLoc = "Loc_438yv";
            fs2 = new FileStream(filName, FileMode.Create);
            sr2 = new BinaryReader(fs2);
            fs2.Close();
            iCountTestcases++;
            if(sr2.PeekChar() != -1) 
            {
                iCountErrors++;
                printerr( "Error_48v88! Incorrect return value at end of stream");
            } 
            sr2.Close();
            ms2 = new MemoryStream();
            sr2 = new BinaryReader(ms2);
            ms2.Close();
            iCountTestcases++;
            if(sr2.PeekChar() != -1) 
            {
                iCountErrors++;
                printerr( "Error_34987b! Incorrect return value at end of stream");
            }
            sr2.Close();
            strLoc = "Loc_348yv";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Write(new Byte[]{1, 2, 3, 4, 5}, 0, 5);
            fs2.Flush();
            fs2.Position = 0;
            sr2 = new BinaryReader(fs2);
            iCountTestcases++;
            Console.WriteLine(sr2.PeekChar());
            if(sr2.PeekChar() != 1) 
            {
                iCountErrors++;
                printerr( "Error_398v8! PeekChar returned incorrect value");
            }
            fs2.Write(new Byte[]{6}, 0, 1);
            fs2.Flush();
            iCountTestcases++;
            if(sr2.PeekChar() != 2) 
            {
                iCountErrors++;
                printerr( "Error_3498y! Expected==2, PeekChar=="+sr2.PeekChar());
            }
            fs2.Position = 0;
            iCountTestcases++;
            if(sr2.PeekChar() != 6) 
            {
                iCountErrors++;
                printerr( "Error_38yv8! Expected==6, PeekChar=="+sr2.PeekChar());
            }
            fs2.Close();
            strLoc = "Loc_34958";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Write(new Byte[]{1, 2, 3, 4, 5}, 0, 5);
            fs2.Flush();
            fs2.Position = 0;
            sr2 = new BinaryReader(fs2);
            fs2.Read(new Byte[2], 0, 2);
            iCountTestcases++;
            if(sr2.PeekChar() != 3) 
            {
                iCountErrors++;
                printerr( "Error_48908! Expected==3, PeekChar=="+sr2.PeekChar());
            }
            fs2.Close();
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5635PeekChar.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 ); // wait for the listener to initialize
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
            Int32 tmp = 0;
            for(int i = 0 ; i < chArr.Length ; i++) 
            {
                tmp = br.PeekChar();
                if(tmp != -1) 
                {
                    iCountErrors++;
                    Console.WriteLine( "It's a network stream.... We always expect -1... but the real value..." + tmp);
                }
                br.Read();
            }
            Console.WriteLine("We are done with the listening");
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
        Random rand = new Random( (int)DateTime.Now.Ticks );
        TcpClient tcpClient = new TcpClient();
        IPEndPoint epSend = null;
        IPAddress sendAddress = IPAddress.Parse(address);			
        epSend = new IPEndPoint(sendAddress, iPortNumber);
        tcpClient.Connect(epSend);
        Stream stream = tcpClient.GetStream();
        BinaryWriter bw = new BinaryWriter( stream );
        for(int i = 0 ; i < chArr.Length ; i++)
            bw.Write(chArr[i]);
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
        Co5635PeekChar cbA = new Co5635PeekChar();
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
