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
public class Co5637Read_chArr_ii
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "BinaryReader.Read(Char[], int, int)";
    public static String s_strTFName        = "Co5637Read_chArr_ii.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public static int iPortNumber           = 0;
    private static int iCountErrors = 0;
    public static ManualResetEvent m_PortSetEvent = new ManualResetEvent(false);

    Random rand = new Random( (int) DateTime.Now.Ticks);
    int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue, Int16.MinValue};
    int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
    int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/3000 , Int32.MaxValue/10000, Int16.MaxValue };
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
        try
        {
            BinaryReader sr2;
            StreamWriter sw2;
            MemoryStream ms2;
            FileStream fs2;
            Char[] chArr2;
            Int32 i32;
            if(File.Exists("Co5637Test.tmp"))
                File.Delete("Co5637Test.tmp");			
            strLoc = "Loc_487vy";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            fs2.Close();
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2);
            iCountTestcases++;
            try 
            {
                sr2.Read((Char[])null, 0, 0);
                iCountErrors++;
                printerr( "Error_87ty7! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo( "Info_ty78g! Caught expectede exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_867yc! Incorrect exception thrown, exc=="+exc.ToString());
            }
            sr2.Close();
            strLoc = "Loc_985yc";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            fs2.Close();
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2);
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ )
            {
                try 
                {
                    sr2.Read(new Char[0], iArrInvalidValues[iLoop], 0);
                    iCountErrors++;
                    printerr( "Error_8y9cx! Expected exception not thrown");
                } 
                catch (ArgumentOutOfRangeException aexc) 
                {
                    printinfo( "Info_8747v! Caught expected exception, aexc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_59csy! Incorrect exception thrown, exc=="+exc.ToString());
                }
            }
            sr2.Close();
            strLoc = "Loc_0720g";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            fs2.Close();
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2);
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ )
            {
                try 
                {
                    sr2.Read(new Char[0], 0, iArrInvalidValues[iLoop]);
                    iCountErrors++;
                    printerr( "Error_t97gh! Expectede exception not thrown");
                } 
                catch (ArgumentOutOfRangeException aexc) 
                {
                    printinfo( "Info_8g9bx! caught expectede exception, aexc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_9000f! Incorrect exception thrown, exc=="+exc.ToString());
                }
            }
            sr2.Close();
            strLoc = "Loc_209x9";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            fs2.Close();
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2);
            iCountTestcases++;
            try 
            {
                sr2.Read(new Char[0], 2, 0);
                iCountErrors++;
                printerr( "Error_968yg! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_87ytg! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_t018c! Incorrect exception thrown, exc=="+exc.ToString());
            }
            sr2.Close();
            strLoc = "Loc_t78yv";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            fs2.Close();
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2);
            sr2.Close();
            iCountTestcases++;
            try 
            {
                sr2.Read(new Char[0], 0, 0);
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
            strLoc = "Loc_t87yb";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            sr2 = new BinaryReader(fs2);
            fs2.Close();
            iCountTestcases++;
            try 
            {
                sr2.Read(new Char[1], 0, 1);
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
            strLoc = "Loc_5948c";
            ms2 = new MemoryStream();
            sr2 = new BinaryReader(ms2);
            chArr2 = new Char[10];
            iCountTestcases++;
            i32 = sr2.Read(chArr2, 0, 10);
            if(i32 != 0) 
            {
                iCountErrors++;
                printerr( "Error_209ji! Values read from empty stream=="+i32);
            }
            sr2.Close();
            strLoc = "Loc_290by";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            sw2 = new StreamWriter(fs2);
            sw2.Write(chArr, 0, chArr.Length);
            sw2.Close();
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2);
            chArr2 = new Char[chArr.Length+1];
            i32 = sr2.Read(chArr2, 0, chArr2.Length );
            iCountTestcases++;
            if(i32 != chArr.Length ) 
            {
                iCountErrors++;
                printerr( "Error_8602c! Incorrect number of chars read, read=="+i32);
            }
            for(int i = 0 ; i < chArr.Length ; i++) 
            {
                iCountTestcases++;
                if(chArr2[i] != chArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_0190v_"+i+"! Incorrect character, expected=="+chArr[i]+", got=="+chArr2[i]);
                }
            }
            sr2.Close();
            strLoc = "Loc_44324";
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            sw2.Write(chArr, 0, chArr.Length);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            chArr2 = new Char[chArr.Length+1];  
            i32 = sr2.Read(chArr2, 0, chArr2.Length);
            iCountTestcases++;
            if(i32 != chArr.Length ) 
            {
                iCountErrors++;
                printerr( "Error_19087! Incorrect number of chars read, read=="+i32);
            } 
            for(int i = 0 ; i < chArr.Length ; i++) 
            { 
                iCountTestcases++;
                if(chArr2[i] != chArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_58byc_"+i+"! Incorrect character, expected=="+chArr[i]+", got=="+chArr2[i]);
                }
            }
            sr2.Close();
            m_PortSetEvent.Reset();
            Thread tcpListenerThread = new Thread(new ThreadStart(Co5637Read_chArr_ii.StartListeningTcp));
            tcpListenerThread.Start();
            Console.WriteLine("Listening");
            Thread.Sleep( 1000 );
            m_PortSetEvent.WaitOne();
            Teleport("127.0.0.1");
            Thread.Sleep( 1000 );   
            strLoc = "Loc_9y08y";
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Create);
            sw2 = new StreamWriter(fs2);
            sw2.Write(chArr, 0, 5);
            sw2.Close();
            fs2 = new FileStream("Co5637Test.tmp", FileMode.Open);
            sr2 = new BinaryReader(fs2);
            chArr2 = new Char[chArr.Length + 1];
            i32 = sr2.Read(chArr2, 4, 4);
            iCountTestcases++;
            if(i32 != 4) 
            {
                iCountErrors++;
                printerr( "Error_984ya! Incorrect number of chars read, read=="+i32);
            }
            for(int i = 0 ; i < 3 ; i++) 
            {
                iCountTestcases++;
                if(chArr2[i+4] != chArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_98y77b_"+i+"! Incorrect character, expected=="+chArr[i]+", got=="+chArr2[i+4]);
                }
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            sw2.Write(chArr, 4, 6);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            chArr2 = new Char[chArr.Length+1];
            i32 = sr2.Read(chArr2, 4, 7);
            iCountTestcases++;
            if(i32 != 6) 
            {
                iCountErrors++;
                printerr("Error_2987b! Incorrect number of chars read, read=="+i32);
                for(int i = 4 ; i < 10 ; i++) 
                {
                    Console.WriteLine("Expected=="+chArr[i]);
                    Console.WriteLine("Got=="+chArr2[i]);
                }
            }
            for(int i = 4 ; i < 10 ; i++) 
            {
                iCountTestcases++;
                if(chArr2[i ] != chArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_9878t_"+i+"! Incorrect character, expected=="+chArr[i]+", got=="+chArr2[i]);
                }
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            sw2.Write(chArr, 4, 6);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            chArr2 = new Char[chArr.Length+1];
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ )
            {    			
                try
                {
                    i32 = sr2.Read(chArr2, iArrLargeValues[iLoop], 7);
                    iCountErrors++ ;
                    Console.WriteLine("Error_1234!!!! Expected exception not occured...");
                } 
                catch( ArgumentException)
                {
                } 
                catch( Exception e)
                {
                    iCountErrors++;
                    printerr( "Error_0988!!!! Incorrect exception occured... Msg:" + e.Message );
                }
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            sw2.Write(chArr, 4, 6);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            chArr2 = new Char[chArr.Length+1];
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ )
            {    			
                try
                {
                    i32 = sr2.Read(chArr2, 0, iArrLargeValues[iLoop]);
                    iCountErrors++ ;
                    Console.WriteLine("Error_4354!!!! Expected exception not occured...");
                } 
                catch( ArgumentException)
                {
                } 
                catch( Exception e)
                {
                    iCountErrors++;
                    printerr( "Error_0988!!!! Incorrect exception occured... Msg:" + e.Message );
                }
            }
            sr2.Close();
            ms2 = new MemoryStream();
            sw2 = new StreamWriter(ms2);
            sw2.Write(chArr, 4, 6);
            sw2.Flush();
            ms2.Position = 0;
            sr2 = new BinaryReader(ms2);
            chArr2 = new Char[Int32.MaxValue/2000];
            for(int iLoop = 0 ; iLoop < chArr2.Length ; iLoop++ )
                chArr2[iLoop] = Convert.ToChar(rand.Next( Byte.MinValue, Byte.MaxValue ));
            sw2.Write( chArr2, 0, chArr2.Length );
            iCountTestcases++;
            for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ )
            {    			
                try
                {
                    ms2.Position = 0;
                    i32 = sr2.Read(chArr2, 0, iArrValidValues[iLoop]);
                    if( i32 != iArrValidValues[iLoop] )
                    {
                        iCountErrors++ ;
                        Console.WriteLine("Error_0877!!!! Expected exception not occured...Expected...{0}, Actual....{1}", iArrValidValues[iLoop], i32 );
                    }
                } 
                catch( Exception e)
                {
                    iCountErrors++;
                    printerr( "Error_7284!!!! Incorrect exception occured... Msg:" + e.Message );
                }
            }
            sr2.Close();
            if(File.Exists("Co5637Test.tmp"))
                File.Delete("Co5637Test.tmp");
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
            Char[] charArr = new Char[chArr.Length];  
            int i32 = br.Read(charArr, 0, charArr.Length);
            if(i32 != chArr.Length ) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_4353! Incorrect number of chars read, read=="+i32);
            } 
            for(int i = 0 ; i < chArr.Length ; i++) 
            { 
                if(charArr[i] != chArr[i]) 
                {
                    iCountErrors++;
                    Console.WriteLine( "Error_58byc_"+i+"! Incorrect character, expected=="+chArr[i]+", got=="+charArr[i]);
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
        for(int i = 0 ; i < chArr.Length ; i++)
        {
            bw.Write(chArr[i]);
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5637Read_chArr_ii cbA = new Co5637Read_chArr_ii();
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
