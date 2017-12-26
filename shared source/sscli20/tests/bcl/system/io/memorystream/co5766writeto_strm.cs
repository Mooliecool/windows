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
public class Co5766WriteTo_strm
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "MemoryStream.WriteTo(Stream)";
    public static String s_strTFName        = "Co5766WriteTo_strm.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
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
            MemoryStream ms2, ms3;
            FileStream fs2;
            BufferedStream bs2;
            String filName = s_strTFAbbrev+"Test.tmp";
            Byte[] bytArrRet;
            Byte[] bytArr = new Byte[] {
                                           Byte.MinValue
                                           ,Byte.MaxValue
                                           ,1
                                           ,2
                                           ,3
                                           ,4
                                           ,5
                                           ,6
                                           ,128
                                           ,250
                                       };
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_00001";
            ms2 = new MemoryStream();
            iCountTestcases++;
            try 
            {
                ms2.WriteTo(null);
                iCountErrors++;
                printerr( "Error_00002! Expected exception not thrown");
            } 
            catch (ArgumentNullException aexc) 
            {
                printinfo( "Info_00003! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_00004! Incorrect exception thrown, exc=="+exc.ToString());
            }
            ms2.Close();
            strLoc = "Loc_00005";
            ms2 = new MemoryStream();
            ms2.Write(new Byte[]{1}, 0, 1);
            fs2 = new FileStream(filName, FileMode.OpenOrCreate, FileAccess.Read);
            iCountTestcases++;
            try 
            {
                ms2.WriteTo(fs2);
                iCountErrors++;
                printerr( "Error_00006! Expected exception not thrown");
            } 
            catch (NotSupportedException iexc) 
            {
                printinfo( "Info_00007! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_00008! Incorrect exception thrown, exc=="+exc.ToString());
            }
            ms2.Close();
            fs2.Close();
            strLoc = "Loc_00009";
            ms2 = new MemoryStream();
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            iCountTestcases++;
            try 
            {
                ms2.WriteTo(fs2);
                iCountErrors++;
                printerr( "Error_00010! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_00011! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_00012! Incorrect exception thrown, exc=="+exc.ToString());
            }
            ms2.Close();
            strLoc = "Loc_00013";
            ms2 = new MemoryStream();
            ms2.Write(bytArr, 0, bytArr.Length);
            fs2 = new FileStream(filName, FileMode.Create);
            ms2.WriteTo(fs2);
            fs2.Flush();
            fs2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            bytArrRet = new Byte[(Int32)fs2.Length];
            fs2.Read(bytArrRet, 0, (Int32)fs2.Length);
            for(int i = 0 ; i < bytArr.Length ; i++) 
            {
                iCountTestcases++;
                if(bytArrRet[i] != bytArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_00014! Expected=="+bytArr[i]+", got=="+bytArrRet[i]);
                }
            }		
            fs2.Close();
            ms2.Close();
            strLoc = "Loc_00015";
            ms2 = new MemoryStream();
            ms2.Write(bytArr, 0, bytArr.Length);
            ms3 = new MemoryStream();
            ms2.WriteTo(ms3);
            ms3.Position = 0;
            bytArrRet = new Byte[(Int32)ms3.Length];
            ms3.Read(bytArrRet, 0, (Int32)ms3.Length);
            for(int i = 0 ; i < bytArr.Length ; i++) 
            {
                iCountTestcases++;
                if(bytArrRet[i] != bytArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_00016! Expected=="+bytArr[i]+", got=="+bytArrRet[i]);
                }
            }					
            ms2.Close();
            ms3.Close();
            strLoc = "Loc_00017";
            ms2 = new MemoryStream();
            ms2.Write(bytArr, 0, bytArr.Length);
            fs2 = new FileStream(filName, FileMode.Create);
            bs2 = new BufferedStream(fs2);
            ms2.WriteTo(bs2);
            bs2.Flush();
            bs2.Close();
            fs2 = new FileStream(filName, FileMode.Open);
            bytArrRet = new Byte[(Int32)fs2.Length];
            fs2.Read(bytArrRet, 0, (Int32)fs2.Length);
            for(int i = 0 ; i < bytArr.Length ; i++) 
            {
                iCountTestcases++;
                if(bytArrRet[i] != bytArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_00018! Expected=="+bytArr[i]+", got=="+bytArrRet[i]);
                }
            }				
            fs2.Close();
            ms2.Close();
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
        Co5766WriteTo_strm cbA = new Co5766WriteTo_strm();
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
