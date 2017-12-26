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
public class Co5734Seek_i64_so
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "MemoryStream.Seek(Int64, SeekOrigin)";
    public static String s_strTFName        = "Co5734Seek_i64_so.cs";
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
            Int64 pos;
            strLoc = "Loc_95vy8";
            strLoc = "Loc_98yvh";
            ms2 = new MemoryStream();
            ms2.Write(new Byte[]{1,2,3,4},0,4);
            ms2.Flush();
            iCountTestcases++;
            try 
            {
                ms2.Seek(-2, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_019uc! Expected exception not thrown");
            } 
            catch (IOException aexc) 
            {
                printinfo("Info_78y7g! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_099gn! Incorrect exception thrown, exc=="+exc.ToString());
            } 
            iCountTestcases++;
            if(ms2.Position != 4) 
            {
                iCountErrors++;
                printerr( "Error_9t8yb! Position set=="+ms2.Position);
            }
            ms2.Position = 1;
            iCountTestcases++;
            try 
            {
                ms2.Seek(-2, SeekOrigin.Begin);
                iCountErrors++;
                printerr( "Error_98ycn! Expected exception not thrown");
            } 
            catch (IOException aexc) 
            {
                printinfo("Info_t6598! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2896g! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(ms2.Position != 1) 
            {
                iCountErrors++;
                printerr("Error_2989b! POsition set=="+ms2.Position);
            }
            ms2.Close();
            strLoc = "Loc_9t87y";
            ms2 = new MemoryStream();
            ms2.Write(new Byte[]{1,2,3,4}, 0,4);
            ms2.Flush();
            iCountTestcases++;
            try 
            {
                ms2.Seek(2, (SeekOrigin)(-1));
                iCountErrors++;
                printerr( "Error_98y8c! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_900vj! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr("Error_098y8! Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(ms2.Position != 4) 
            {
                iCountErrors++;
                printerr( "Error_9g7b7! Position set=="+ms2.Position);
            }
            iCountTestcases++;
            try 
            {
                ms2.Seek(2, (SeekOrigin)1000);
                iCountErrors++;
                printerr( "Error_t98bh! Expected exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_t87yb! Caught expected exception, exc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_1987v Incorrect exception thrown, exc=="+exc.ToString());
            }
            iCountTestcases++;
            if(ms2.Position != 4) 
            {
                iCountErrors++;
                printerr( "ERror_g7y7b! Position setc=="+ms2.Position);
            }
            ms2.Close();
            strLoc = "Loc_900ub";
            ms2 = new MemoryStream();
            ms2.Write(new Byte[]{1,2,3,4,5,6,7,8},0, 8);
            ms2.Flush();
            ms2.Position = 2;
            pos = ms2.Seek(-1, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 1) 
            {
                iCountErrors++;
                printerr( "Error_4897b! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(ms2.Position != 1) 
            {
                iCountErrors++;
                printerr( "Error_g8uy8! Incorrect position=="+ms2.Position);
            }
            pos = ms2.Seek(5, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 6) 
            {
                iCountErrors++;
                printerr( "Error_87yg7! Incorrec return, pos=="+pos);
            }
            iCountTestcases++;
            if(ms2.Position != 6) 
            {
                iCountErrors++;
                printerr( "Error_g498y! Incorrect position=="+ms2.Position);
            }
            pos = ms2.Seek(7, SeekOrigin.Begin);
            iCountTestcases++;
            if(pos != 7) 
            {
                iCountErrors++;
                printerr( "ERror_78h91! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(ms2.Position != 7) 
            {
                iCountErrors++;
                printerr( "Error_10978! Incorrect position=="+ms2.Position);
            }
            pos = ms2.Seek(-5, SeekOrigin.End);
            iCountTestcases++;
            if(pos != 3) 
            {
                iCountErrors++;
                printerr( "Error_2908y! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(ms2.Position != 3) 
            {
                iCountErrors++;
                printerr( "Error_984yb! Incorrect position=="+ms2.Position);
            }
            pos = ms2.Seek(2, SeekOrigin.Current);
            iCountTestcases++;
            if(pos != 5) 
            { 
                iCountErrors++;
                printerr( "Error_09utg! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(ms2.Position != 5) 
            {
                iCountErrors++;
                printerr( "Error_98g87! Incorrect position=="+ms2.Position);
            }
            ms2.SetLength(120);
            ms2.Position = 5;
            pos = ms2.Seek(100, SeekOrigin.Current);
            if(pos != 105) 
            {
                iCountErrors++;
                printerr( "Error_b858j! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(ms2.Position != 105) 
            {
                iCountErrors++;
                printerr( "Error_48yvb! Incorrect position=="+ms2.Position);
            }
            pos = ms2.Seek(0, SeekOrigin.Current);
            if(pos != 105) 
            {
                iCountErrors++;
                printerr( "Error_g987y! Incorrect return=="+pos);
            }
            iCountTestcases++;
            if(ms2.Position != 105) 
            {
                iCountErrors++;
                printerr( "ERror_g587h! Incorrec position=="+ms2.Position);
            }
            ms2.Close();
            strLoc = "Loc_0387c";
            ms2 = new MemoryStream();
            ms2.Close();
            iCountTestcases++;
            try 
            {
                ms2.Seek(0, SeekOrigin.End);
                iCountErrors++;
                printerr( "Error_t87yb! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo("Info_g897h! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_98f8h! Incorrect exception thrown, exc=="+exc.ToString());
            }
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
        Co5734Seek_i64_so cbA = new Co5734Seek_i64_so();
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
