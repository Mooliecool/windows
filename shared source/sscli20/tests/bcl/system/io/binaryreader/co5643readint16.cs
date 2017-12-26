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
public class Co5643ReadInt16
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BinaryWriter.ReadInt16()";
    public static String s_strTFName        = "Co5643ReadInt16.cs";
    public static String s_strTFAbbrev      = "Co5643";
    public static String s_strTFPath        = Environment.CurrentDirectory.ToString();
    public static int iPortNumber           = 0;
    private static int iCountErrors         = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static Int16[] i16Arr = new Int16[] {
                                                    Int16.MinValue
                                                    ,Int16.MaxValue
                                                    ,0
                                                    ,-10000
                                                    ,10000
                                                    ,-50
                                                    ,50
                                                };
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
            Int16 i16a = 0;
            int ii = 0;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName)) 
                File.Delete(filName);
            strLoc = "Loc_8yfv7";
            fil2 = new FileInfo(filName);
            fs2 = fil2.Open(FileMode.Create);
            dw2 = new BinaryWriter(fs2);
            try 
            {
                for(ii = 0 ; ii < i16Arr.Length ; ii++)
                    dw2.Write(i16Arr[ii]);		   
                dw2.Flush();
                fs2.Close();
                strLoc = "Loc_987hg";
                fs2 = fil2.Open(FileMode.Open);
                dr2 = new BinaryReader(fs2);
                for(ii = 0 ; ii < i16Arr.Length ;ii++) 
                {
                    iCountTestcases++;
                    if((i16a = dr2.ReadInt16()) != i16Arr[ii]) 
                    {
                        iCountErrors++;
                        printerr( "Error_298hg_"+ii+"! Expected=="+i16Arr[ii]+" , got=="+i16a);
                    }
                }
                iCountTestcases++;
                try 
                {
                    i16a = dr2.ReadInt16();
                    iCountErrors++;
                    printerr( "Error_2389! Expected exception not thrown, i16a=="+i16a);
                } 
                catch (EndOfStreamException) 
                {
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_3298h! Unexpected exception thrown, exc=="+exc.ToString());
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_278gy! Unexpected exception, exc=="+exc.ToString());
            }
            fs2.Close();
            fil2.Delete();
            strLoc = "Loc_98yss";
            mstr = new MemoryStream();
            dw2 = new BinaryWriter(mstr);
            try 
            {
                i16Arr = new Int16[] {
                                         Int16.MinValue
                                         ,Int16.MaxValue
                                         ,0
                                         ,-10000
                                         ,10000
                                         ,-50
                                         ,50
                                     };	
                for(ii = 0 ; ii < i16Arr.Length ; ii++) 
                    dw2.Write(i16Arr[ii]);
                dw2.Flush();
                mstr.Position = 0;
                strLoc = "Loc_287y5";
                dr2 = new BinaryReader(mstr);
                for(ii = 0 ; ii < i16Arr.Length ;ii++) 
                {
                    iCountTestcases++;
                    if((i16a = dr2.ReadInt16()) != i16Arr[ii]) 
                    {
                        iCountErrors++;
                        printerr( "Error_398xu_"+ii+"! Expected=="+i16Arr[ii]+" , got=="+i16a);
                    }
                }
                iCountTestcases++;
                try 
                {
                    i16a = dr2.ReadInt16();
                    iCountErrors++;
                    printerr( "Error_2d847! Expected exception not thrown, i16a=="+i16a);
                } 
                catch (EndOfStreamException) 
                {
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_238gy! Unexpected exception thrown, exc=="+exc.ToString());
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_278gy! Unexpected exception, exc=="+exc.ToString());
            }
            mstr.Close();
            if(File.Exists(filName)) 
                File.Delete(filName);
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5643ReadInt16.StartListeningTcp));
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
            Int16 iTemp ;
            for(int i = 0 ; i < i16Arr.Length ; i++) 
            {
                iTemp = br.ReadInt16();
                if(iTemp != i16Arr[i]) 
                {
                    iCountErrors++;
                    Console.WriteLine( "Error_5453c_"+i+"! Expected=="+i16Arr[i]+", got=="+iTemp);
                }
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
        TcpClient tcpClient = new TcpClient();
        IPEndPoint epSend = null;
        IPAddress sendAddress = IPAddress.Parse(address);			
        epSend = new IPEndPoint(sendAddress, iPortNumber);
        tcpClient.Connect(epSend);
        Stream stream = tcpClient.GetStream();
        BinaryWriter bw = new BinaryWriter( stream );
        for(int i = 0 ; i < i16Arr.Length ; i++)
            bw.Write(i16Arr[i]);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5643ReadInt16 cbA = new Co5643ReadInt16();
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
