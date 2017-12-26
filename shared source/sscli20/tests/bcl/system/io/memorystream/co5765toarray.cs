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
public class Co5765ToArray
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "MemoryStream.ToArray()";
    public static String s_strTFName        = "Co5765ToArray.cs";
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
            MemoryStream ms2;
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
            strLoc = "Loc_00001";
            ms2 = new MemoryStream();
            bytArrRet = ms2.ToArray();
            iCountTestcases++;
            if(bytArrRet.Length != 0) 
            {
                iCountErrors++;
                printerr( "Error_00002! Unexpected length of array=="+bytArrRet.Length);
            }
            ms2.Close();
            strLoc = "Loc_00003";
            ms2 = new MemoryStream();
            ms2.Write(bytArr, 0, bytArr.Length);
            ms2.Flush();
            bytArrRet = ms2.ToArray();
            iCountTestcases++;
            if(bytArrRet.Length != bytArr.Length) 
            {
                iCountErrors++;
                printerr( "Error_00004! Unexpected length, expected=="+bytArr.Length+", got=="+bytArrRet.Length);
            }
            for(int i = 0 ; i < bytArr.Length ; i++) 
            {
                iCountTestcases++;
                if(bytArrRet[i] != bytArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_00005_"+i+"! Expected=="+bytArr[i]+", got=="+bytArrRet[i]);
                }
            }
            ms2.Close();
            strLoc = "Loc_00006";
            ms2 = new MemoryStream();
            ms2.Write(bytArr, 0, bytArr.Length);
            ms2.SetLength(5);
            ms2.Flush();
            bytArrRet = ms2.ToArray();
            iCountTestcases++;
            if(bytArrRet.Length != 5) 
            {
                iCountErrors++;
                printerr( "Error_00007! Expected==5, got=="+bytArrRet.Length);
            }
            for(int i = 0 ; i < 5 ; i++) 
            {
                iCountTestcases++;
                if(bytArrRet[i] != bytArr[i]) 
                {
                    iCountErrors++;
                    printerr( "Error_00008_"+i+"! Expected=="+bytArr[i]+", got=="+bytArrRet[i]);
                }
            }
            ms2.Close();
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
        Co5765ToArray cbA = new Co5765ToArray();
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
