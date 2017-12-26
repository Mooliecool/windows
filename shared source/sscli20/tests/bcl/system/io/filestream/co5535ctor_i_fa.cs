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
public class Co5535ctor_i_fa
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Copy(String)";
    public static String s_strTFName        = "Co5535ctor_i_fa.cs";
    public static String s_strTFAbbrev      = "Co5535";
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
            FileStream fs2, fs3;
            Int32 handle;
            Byte[] bReadArr;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_98v8v";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(new IntPtr(-1), FileAccess.ReadWrite);
                iCountErrors++;
                printerr( "Error_987yt! Expected exception not thrown");
            } 
            catch (ArgumentException ) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_29100! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_498yf";
            fs2 = new FileStream(filName, FileMode.Create);
            handle = (int)fs2.Handle;
            Console.WriteLine(handle);
            fs2.Write(new Byte[]{65,66,67,68,69}, 0, 5);
            fs2.Flush();
            fs2.Position = 0;
            fs3 = new FileStream(new IntPtr(handle), FileAccess.Read, false);
            bReadArr = new Byte[5];
            int read = fs3.Read(bReadArr, 0, 5);
            int i = 65;
            foreach (Byte b in bReadArr) 
            {
                iCountTestcases++;
                if(b != i) 
                {
                    iCountErrors++;
                    printerr( "Error_47f7v_"+i+"! Expected=="+i+" , got=="+b);
                }
                i++;
            }
            fs2.Close();
            fs3.Close();
            strLoc = "Loc_487ty";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(new IntPtr(2), (FileAccess)(-2));
                iCountErrors++;
                printerr( "Error_f489y! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_4t98c! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_4398g! Incorrect exception thrown, exc=="+exc.ToString());
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
        Co5535ctor_i_fa cbA = new Co5535ctor_i_fa();
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
