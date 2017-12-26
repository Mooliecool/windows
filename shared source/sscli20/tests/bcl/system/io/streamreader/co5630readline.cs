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
public class Co5630ReadLine
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "StreamReader.ReadToEnd()";
    public static String s_strTFName        = "Co5630ReadLine.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static string strValue ="Some string to test the network streams" ;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        try
        {
            StreamReader sr2;
            StreamWriter sw2;
            MemoryStream ms2;
            FileStream fs2;
            String str2;
            Char[] chArr2;
            String filName = s_strTFAbbrev+"Test.tmp";
            Char[] chArrValues = new Char[]{
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
                                               ,'\r'
                                               ,'\uFE70' 
                                               ,'-'
                                               ,';'
                                               ,'\r'
                                               ,'\n'
                                               ,'T'
                                               ,'3'
                                               ,'\n'
                                               ,'K'
                                               ,'\u00E6'
                                           };
            String valueString = new String(chArrValues);
            if(File.Exists(filName))
                File.Delete(filName);			
            strLoc = "Loc_t78yv";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            sr2 = new StreamReader(filName);
            sr2.Close();
            iCountTestcases++;
            try 
            {
                sr2.ReadLine();
                iCountErrors++;
                printerr( "Error_958yb! Expected exception not thrown");
            } 
            catch (ObjectDisposedException aexc) 
            {
                printinfo( "Info_8697y! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_6y787! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            ms2 = new MemoryStream();
            sr2 = new StreamReader(ms2);
            sr2.Close();
            iCountTestcases++;
            try 
            {
                sr2.ReadLine();
                iCountErrors++;
                printerr( "Error_938yb! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo ("Info_049vu! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_1757y! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_t87yb";
            fs2 = new FileStream(filName, FileMode.Create);
            sr2 = new StreamReader(fs2);
            fs2.Close();
            iCountTestcases++;
            try 
            {
                sr2.ReadLine();
                iCountErrors++;
                printerr( "Error_t67yg! Expected exception not thrown");
            } 
            catch (ObjectDisposedException nexc) 
            {
                printinfo( "Info_t7800! Caught expected exception, nexc=="+nexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t78y1! Incorrect exception thrown, exc=="+exc.ToString());
            }
            ms2 = new MemoryStream();
            sr2 = new StreamReader(ms2);
            ms2.Close();
            iCountTestcases++;
            try 
            {
                sr2.ReadLine();
                iCountErrors++;
                printerr( "Error_17857! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_6998g! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_76r87! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_5948c";
            ms2 = new MemoryStream();
            sr2 = new StreamReader(ms2);
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(str2 != null) 
            {
                iCountErrors++;
                printerr( "Error_109g8! Incorrect string read=="+str2);
            }
            strLoc = "Loc_290by";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2);
            sw2.Write(chArrValues, 0, chArrValues.Length);
            sw2.Close();
            sr2 = new StreamReader(filName);
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(0, valueString.IndexOf('\r')))) 
            {
                iCountErrors++;
                printerr( "Error_918t7! Unexpected string read=="+str2);
            }
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(valueString.IndexOf('\r')+1, 3))) 
            {
                iCountErrors++;
                printerr( "Error_t7y8y! Unexpected string read=="+str2);
            }
            sr2.Read();
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(valueString.IndexOf('\n')+2, 1))) 
            {
                iCountErrors++;
                printerr( "Error_t8789! Unexpected string read=="+str2);
            } 
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(valueString.LastIndexOf('\n')+1))) 
            {
                iCountErrors++;
                printerr( "Error_78yt7! Unexpected string read=="+str2);
            }
            sr2.Close();
            strLoc = "Loc_3244e";
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            sw2.Write(chArrValues, 0, chArrValues.Length);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new StreamReader(ms2);
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(0, valueString.IndexOf('\r')))) 
            {
                iCountErrors++;
                printerr( "Error_918t7! Unexpected string read=="+str2);
            }
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(valueString.IndexOf('\r')+1, 3))) 
            {
                iCountErrors++;
                printerr( "Error_t7y8y! Unexpected string read=="+str2);
            }
            sr2.Read();
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(valueString.IndexOf('\n')+2, 1))) 
            {
                iCountErrors++;
                printerr( "Error_t8789! Unexpected string read=="+str2);
            } 
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(valueString.LastIndexOf('\n')+1))) 
            {
                iCountErrors++;
                printerr( "Error_78yt7! Unexpected string read=="+str2);
            }
            sr2.Close();
            strLoc = "Loc_9y08y";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2);
            sw2.Write(chArrValues, 0, 5);
            sw2.Close();
            sr2 = new StreamReader(filName);
            chArr2 = new Char[chArrValues.Length];
            sr2.Read(chArr2, 0, 3);
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(3, 2))) 
            {
                iCountErrors++;
                printerr( "Error_9489b! Incorrect string returned=="+str2);
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            sw2.Write(chArrValues, 4, 6);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new StreamReader(ms2);
            chArr2 = new Char[chArrValues.Length];
            sr2.Read(chArr2, 0, 5);
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString[9].ToString())) 
            {
                iCountErrors++;
                printerr( "Error_3298c! Incorrect string returned=="+str2);
            }
            sr2.Close();
            strLoc = "Loc_982yb";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2);
            sw2.Write(chArrValues, 0, 5);
            sw2.Close();
            sr2 = new StreamReader(filName);
            chArr2 = new Char[5];
            chArr2[3] = (Char)sr2.Read();
            chArr2[4] = (Char)sr2.Read();
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(valueString.Substring(2, 3))) 
            {
                iCountErrors++;
                printerr( "ERror_9867g! Incorrect string returned=="+str2);
            }
            sr2.Close();
            strLoc = "Loc_093hb";
            fs2 = new FileStream(filName, FileMode.Create);
            sw2 = new StreamWriter(fs2);
            sw2.Write("\n\n\r\r\n");
            sw2.Close();
            sr2 = new StreamReader(filName);
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(String.Empty)) 
            {
                iCountErrors++;
                printerr( "ERror_8920c! Incorrect string returned=="+str2);
            } 
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(String.Empty)) 
            {
                iCountErrors++;
                printerr( "Error_90668! Incorrect string returned=="+str2);
            } 
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(String.Empty)) 
            {
                iCountErrors++;
                printerr( "Error_09577! Incorrect string returned=="+str2);
            }
            str2 = sr2.ReadLine();
            iCountTestcases++;
            if(!str2.Equals(String.Empty)) 
            {
                iCountErrors++;
                printerr( "Error_1090! Incorrect string returned=="+str2);
            }
            iCountTestcases++;
            str2 = sr2.ReadLine();
            if(str2 != null) 
            {
                iCountErrors++;
                printerr( "Error_09809! Non-null returned for end of stream");
            }
            sr2.Close();
            iCountTestcases++;
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5630ReadLine.StartListeningTcp));
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
                String text = sr.ReadLine();
                if(text != strValue) 
                {
                    iCountErrors++;
                    Console.WriteLine( "Error_54543! Incorrect string read " + strValue);
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
        StreamWriter sw = new StreamWriter(stream);
        sw.Write( strValue);
        sw.Flush();
        sw.Close();
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5630ReadLine cbA = new Co5630ReadLine();
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
