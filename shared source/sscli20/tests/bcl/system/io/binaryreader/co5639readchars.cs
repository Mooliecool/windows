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
public class Co5639ReadChars
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BinaryReader.ReadChars(Int32)";
    public static String s_strTFName        = "Co5639ReadChars.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    private static Char[] chArr = new Char[]{
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
        Random rand = new Random( (int) DateTime.Now.Ticks );
        int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue, Int16.MinValue};
        int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
        int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/2000 , Int32.MaxValue/10000, Int16.MaxValue };
        try
        {
            BinaryReader sr2;
            StreamWriter sw2;
            MemoryStream ms2;
            FileStream fs2;
            Char[] chArrReturn;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);					
            strLoc = "Loc_948yv";
            ms2 = new MemoryStream();
            sr2 = new BinaryReader(ms2);
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ )
            {
                try 
                {
                    sr2.ReadChars(iArrInvalidValues[iLoop]);
                    iCountErrors++;
                    printerr( "Error_1098g! Expected exception not thrown");
                } 
                catch (ArgumentOutOfRangeException aexc) 
                {
                    printinfo( "Info_7587b! Caught expected exception, exc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_789gy! Incorrect exception thrown, exc=="+exc.ToString());
                } 
            }
            sr2.Close();
            strLoc = "Loc_948yv";
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            for(int i = 0 ; i < chArr.Length ; i++)
                sw2.Write(chArr[i]);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            iCountTestcases++;
            char[] cNewArray = null ;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ )
            {
                try 
                {
                    cNewArray = sr2.ReadChars(iArrLargeValues[iLoop]);
                    if(!( cNewArray.Length == 0 || cNewArray.Length == 22))
                    {
                        iCountErrors++;
                        printerr( "Error_5543! Unexpected bytes are read from the stream... Length:" + cNewArray.Length);
                    }
                } 
                catch (OutOfMemoryException aexc) 
                {
                    printinfo( "Info_7342! Caught expected exception, exc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_0843! Incorrect exception thrown, exc=="+exc.ToString());
                } 
            }
            sr2.Close(); 
            strLoc = "Loc_7t09b";
            ms2 = new MemoryStream();
            ms2.Write(new Byte[]{1}, 0, 1);
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            iCountTestcases++;
            chArrReturn = sr2.ReadChars(3);
            if(chArrReturn.Length != 1) 
            {
                iCountErrors++;
                printerr( "Error_2098uv! Read from empty stream, read=="+chArrReturn.Length);
            }
            sr2.Close();
            strLoc = "Loc_2698b";
            try 
            {
                ms2 = new MemoryStream();
                sw2 = new StreamWriter(ms2);
                for(int i = 0 ; i < chArr.Length ; i++)
                    sw2.Write(chArr[i]);
                sw2.Flush();
                ms2.Position = 0;
                sr2 = new BinaryReader(ms2);
                chArrReturn = sr2.ReadChars(chArr.Length );
                iCountTestcases++;
                if(chArrReturn.Length != chArr.Length ) 
                {
                    iCountErrors++;
                    printerr( "Error_1900c! Incorrect number of chars read");
                }
                for(int i = 0 ; i < chArr.Length ; i++) 
                {
                    iCountTestcases++;
                    if(chArrReturn[i] != chArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_298vc_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+(Int32)chArrReturn[i]);
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
                fs2 = new FileStream(filName, FileMode.Open);
                sr2 = new BinaryReader(fs2);
                chArrReturn = sr2.ReadChars(chArr.Length );
                iCountTestcases++;
                if(chArrReturn.Length != chArr.Length ) 
                {
                    iCountErrors++;
                    printerr( "Error_0901x! Incorrect number of chars read");
                }
                for(int i = 0 ; i < chArr.Length ; i++) 
                {
                    iCountTestcases++;
                    if(chArrReturn[i] != chArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_98yv8!_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+(Int32)chArrReturn[i]);
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
            sw2.Write("\u00FA\u00FB\u00FC\u00FD\u00FE");
            sw2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            sr2 = new BinaryReader(fs2);
            iCountTestcases++;
            sr2.ReadChar() ; 
            if(sr2.ReadChar() != (Char)0xFA) 
            {
                iCountErrors++;
                printerr( "Error_t8yc! Incorrect character read");
            }
            if(sr2.ReadChar() != (Char)0xFB) 
            {
                iCountErrors++;
                printerr("Error_8yb78! Incorrect character read");
            }
            chArrReturn = sr2.ReadChars(2);
            iCountTestcases++;
            if(chArrReturn.Length != 2) 
            {
                iCountErrors++;
                printerr( "Error_tg777! Incorrect number of chars read");
            }
            iCountTestcases++;
            if(chArrReturn[0] != (Char)0xFC) 
            {
                iCountErrors++;
                printerr( "Error_9t8yv! Incorrect char read");
            }
            iCountTestcases++;
            if(chArrReturn[1] != (Char)0xFD) 
            {
                iCountErrors++;
                printerr( "Error_2098b! Incorrect char read");
            }
            iCountTestcases++;
            Int32 tmp;		  
            if((tmp = sr2.Read()) != 0xFE) 
            {
                iCountErrors++;
                printerr( "Error_928yb! Incorrect value read, tmp=="+tmp);
            }
            iCountTestcases++;
            if((tmp = sr2.Read()) != -1) 
            {
                iCountErrors++;
                printerr( "Error_t8753! Incorrect position in stream, tmp=="+tmp);
            }
            sr2.Close();
            strLoc = "Loc_48vy7";
            ms2 = new MemoryStream();
            sr2 = new BinaryReader(ms2);
            iCountTestcases++;
            if(sr2.ReadChars(1).Length != 0) 
            {
                iCountErrors++;
                printerr( "Error_398yc! Incorrect number of characters read");
            }
            ms2.Close();
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5639ReadChars.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 ); 
            strLoc = "Loc_9066";
            chArr = new Char[Int32.MaxValue/2000];
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            for(int i = 0 ; i < chArr.Length ; i++)
                sw2.Write(rand.Next(Char.MinValue, Char.MaxValue));
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ )
            {
                try 
                {
                    ms2.Position = 0 ;
                    cNewArray = sr2.ReadChars(iArrValidValues[iLoop]);
                    if(cNewArray.Length !=iArrValidValues[iLoop])
                    {
                        iCountErrors++;
                        printerr( "Error_4379! Unexpected bytes are read from the stream... Length:" + cNewArray.Length);
                    }
                } 
                catch (OutOfMemoryException aexc) 
                {
                    printinfo( "Info_0844! Caught expected exception, exc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_3848! Incorrect exception thrown, exc=="+exc.ToString());
                } 
            }
            sr2.Close();
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
            char[] chArrReturn = br.ReadChars(chArr.Length );
            if(chArrReturn.Length != chArr.Length ) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_5345! Incorrect number of chars read");
            }
            for(int i = 0 ; i < chArr.Length ; i++) 
            {
                if(chArrReturn[i] != chArr[i]) 
                {
                    iCountErrors++;
                    Console.WriteLine( "Error_5453c_"+i+"! Expected=="+(Int32)chArr[i]+", got=="+(Int32)chArrReturn[i]);
                }
            }
            Console.WriteLine("We are done with the listening");
        }
        catch(Exception e) 
        {
            iCountErrors++ ;
            Console.WriteLine("Exception receiving Teleportation: " + e.Message + Environment.NewLine + e.StackTrace);
            m_PortSetEvent.Set();
        } //catch
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
        for(int i = 0 ; i < chArr.Length ; i++)
            bw.Write(chArr[i]);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5639ReadChars cbA = new Co5639ReadChars();
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
