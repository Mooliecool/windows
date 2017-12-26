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
public class Co5726ctor_fm_fa_fs_i
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileStream(String, FileMode, FileAccess, FileShare, Int32)";
    public static String s_strTFName        = "Co5726ctor_fm_fa_fs_i.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        try
        {
            String filName = s_strTFAbbrev+"TestFile";
            FileInfo fil2;
            Stream fs2, fs3;
            if(File.Exists(filName)) 
                File.Delete(filName);
            strLoc = "Loc_498vw";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(filName, FileMode.Create, FileAccess.ReadWrite, FileShare.None, -1);
                iCountErrors++;
                printerr( "Error_100aa! Expected exception not thrown");
                fs2.Close();
            } 
            catch (ArgumentOutOfRangeException aexc) 
            {
                printinfo( "Info_100bb! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_100cc! Incorrect exception thrown, exc=="+exc.ToString());
            }
            for(int counter = 1 ; counter < 100 ; counter++) 
            {
                strLoc = "Loc_2498V";
                fs2 = new FileStream(filName, FileMode.Create, FileAccess.ReadWrite, FileShare.None, counter);
                iCountTestcases++;
                try 
                {
                    fs3 = new FileStream(filName, FileMode.Open);
                    iCountErrors++;
                    printerr( "Error_209uv! Shouldn't be able to open an open file");
                    fs3.Close();
                } 
                catch (IOException iexc) 
                {
                } 
                catch (Exception exc) 
                {			   
                    iCountErrors++;
                    printerr( "Error_287gv! Incorrect exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
                strLoc = "Loc_f5498";
                fil2 = new FileInfo(filName);
                fs2 = new FileStream(filName, FileMode.Create, FileAccess.ReadWrite, FileShare.Read, counter);
                iCountTestcases++;			
                fs3 = new FileStream(filName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite, counter);
                fs2.Write(new Byte[]{10}, 0, 1);
                fs2.Flush();
                fs3.Read(new Byte[1], 0, 1);
                iCountTestcases++;
                try 
                {
                    fs3.Write(new Byte[]{10}, 0, 1);
                    iCountErrors++;
                    printerr( "Error_958vc! Expected exception not thrown");
                } 
                catch (NotSupportedException iexc) 
                {
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_20939! Incorrect exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
                fs3.Close();
                strLoc = "Loc_2498x";
                fil2 = new FileInfo(filName);
                fs2 = new FileStream(filName, FileMode.Create, FileAccess.ReadWrite, FileShare.Write, counter);
                iCountTestcases++;
                try 
                {
                    fs3 = new FileStream(filName, FileMode.Open, FileAccess.Write, FileShare.ReadWrite, counter);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr("Error_2980x! Unexpected exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
                fs3.Close();
                strLoc = "Loc_4897y";
                fil2 = new FileInfo(filName);
                fs2 = new FileStream(filName, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite, counter);
                iCountTestcases++;
                try 
                {
                    fs3 = new FileStream(filName, FileMode.Open, FileAccess.Write, FileShare.ReadWrite, counter);
                    fs3.Close();
                    fs3 = new FileStream(filName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite, counter);
                    fs3.Close();
                    fs3 = new FileStream(filName, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite, counter);
                    fs3.Close();
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_287g9! Unexpected exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
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
        Co5726ctor_fm_fa_fs_i cbA = new Co5726ctor_fm_fa_fs_i();
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
            Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
