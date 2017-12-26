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
public class Co5718Close
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileStream.Handle";
    public static String s_strTFName        = "Co5718Close.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            FileStream fs2;
            String filName = s_strTFAbbrev + "TestFile.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_10000";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            iCountTestcases++;
            if(fs2.CanRead) 
            {
                iCountErrors++;
                printerr( "Error_1000a! CanRead returned true for closed stream");
            }
            iCountTestcases++;
            if(fs2.CanWrite) 
            {
                iCountErrors++;
                printerr( "Error_1000b! CanWrite returned true for closed stream");
            }
            iCountTestcases++;
            if(fs2.CanSeek) 
            {
                iCountErrors++;
                printerr( "Error_1000c! CanSeek returned true for closed stream");
            }
            strLoc = "Loc_20000";
            fs2 = new FileStream(filName, FileMode.Create);                        
            fs2.Close();
            iCountTestcases++;
            try 
            {
                Console.WriteLine(fs2.Length);
                iCountErrors++;
                printerr( "Error_2000a! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000b! Caught expected exception , iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr("Error_2000c! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                fs2.SetLength(5);
                iCountErrors++;
                printerr( "Error_2000d! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000e! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch ( Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2000f! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                Console.WriteLine(fs2.Position);
                iCountErrors++;
                printerr("Error_2000g! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000h! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2000i! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                fs2.Position = 1;
                iCountErrors++;
                printerr( "Error_2000j! Expected exception not thrown");
            } 
            catch ( ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000k! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2000l! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                fs2.Write(new Byte[]{1}, 0, 1);
                iCountErrors++;
                printerr( "Error_2000m! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000n! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2000o! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                fs2.Read(new Byte[1], 0, 1);
                iCountErrors++;
                printerr( "Error_2000p! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000q! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2000r! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                fs2.EndWrite(fs2.BeginWrite(new Byte[]{1}, 0, 1, null, null));
                iCountErrors++;
                printerr( "Error_2000s! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000t! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr("Error_2000u! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                fs2.EndRead(fs2.BeginRead(new Byte[1], 0, 1, null, null));
                iCountErrors++;
                printerr("Error_2000v! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000w! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch( Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2000x! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            iCountTestcases++;
            try 
            {
                fs2.Flush();
                iCountErrors++;
                printerr( "Error_2000y! Expected exception not thrwon");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_2000z! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200aa! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            try 
            {
                fs2.Lock(1,1);
                iCountErrors++;
                printerr( "Error_200ae! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_200af! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200ag! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            iCountTestcases++;
            try 
            {
                fs2.ReadByte();
                iCountErrors++;
                printerr( "Error_200ah! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_200ai! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200aj! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            iCountTestcases++;
            try 
            {
                fs2.Seek(1, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_200ak! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_200al! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200am! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            iCountTestcases++;
            try 
            {
                fs2.Unlock(1,1);
                iCountErrors++;
                printerr( "Error_200an! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_200ao! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200ap! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            strLoc = "Loc_40000";
            iCountTestcases++;
            try 
            {
                if(fs2.Handle.ToInt32() != -1) 
                {
                    iCountErrors++;
                    printerr( "ERror_3000a! Incorrect value returned by GetHandle()=="+fs2.Handle);
                }                                        
            }
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_200ao! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200ap! Incorrect exception thrown, exc=="+exc.ToString());
            } 
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
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co5718Close cbA = new Co5718Close();
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
