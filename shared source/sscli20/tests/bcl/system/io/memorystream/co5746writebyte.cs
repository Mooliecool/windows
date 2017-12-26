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
public class Co5746WriteByte
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "MemoryStream.WriteByte";
    public static String s_strTFName        = "Co5746WriteByte.cs";
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
            MemoryStream ms2;
            Int32 ii = 0;
            Byte[] bytArr;
            Int32 i32;
            bytArr = new Byte[] {
                                    Byte.MinValue
                                    ,Byte.MaxValue
                                    ,100
                                    ,Byte.MaxValue-100
                                };
            strLoc = "Loc_398yc";
            ms2 = new MemoryStream();
            for(ii = 0 ; ii < bytArr.Length ; ii++)
                ms2.WriteByte(bytArr[ii]);
            ms2.Flush();
            ms2.Position = 0;
            ms2.Flush();
            for(ii = 0 ; ii < bytArr.Length ; ii++) 
            {
                iCountTestcases++;
                if((i32 = ms2.ReadByte()) != bytArr[ii]) 
                {
                    iCountErrors++;
                    printerr( "Error_38yv8_"+ii+"! Expected=="+bytArr[ii]+", got=="+i32);
                }
            }
            i32 = ms2.ReadByte();
            if(i32 != -1) 
            {
                iCountErrors++;
                printerr( "Error_238v8! -1 return expected, i32=="+i32);
            }
            ms2.Position = 0;
            for(ii = 0 ; ii < bytArr.Length; ii++)
                ms2.WriteByte(bytArr[ii]);
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
        Co5746WriteByte cbA = new Co5746WriteByte();
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
