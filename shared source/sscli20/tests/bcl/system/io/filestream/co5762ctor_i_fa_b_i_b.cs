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
public class Co5762ctor_i_fa_b_i_b
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileStream(Int32, FileAccess,Boolean, Boolean, Int32)";
    public static String s_strTFName        = "Co5762ctor_i_fa_b_i_b.cs";
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
            FileStream fs2, fs3;
            Byte[] bReadArr;
            String filName = s_strTFAbbrev+"Test.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_498yv";
            iCountTestcases++;
            fs2 = new FileStream(filName, FileMode.Create);
            try 
            {
                fs2 = new FileStream(fs2.Handle, FileAccess.ReadWrite, true, -1 ,false );
                iCountErrors++;
                printerr( "Error_38g88! Expected exception not thrown");
            } 
            catch (ArgumentOutOfRangeException aexc) 
            {
                printinfo( "Info_8gy8g! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_3908v! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();
            strLoc = "Loc_98v8v";
            iCountTestcases++;
            try 
            {
                fs2 = new FileStream(new IntPtr(-1), FileAccess.ReadWrite, true, 10, false );
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
            fs2.Write(new Byte[]{65,66,67,68,69}, 0, 5);
            fs2.Flush();
            fs2.Position = 0;
            fs3 = new FileStream(fs2.Handle, FileAccess.Read, false, 10, false );
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
                fs2 = new FileStream(new IntPtr(2), (FileAccess)(-2), true, 10, false );
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
            strLoc = "Loc_399f9";
            fs2 = new FileStream(filName, FileMode.Create);
            fs3 = new FileStream(fs2.Handle, FileAccess.Read, true, 10, false );
            fs3.Close();
            iCountTestcases++;
            try 
            {
                fs2.Write(new Byte[]{1}, 0, 1);
                fs2.Flush();
                iCountErrors++;
                printerr( "Error_3f8vc! Expected exception not thrown");
            } 
            catch (IOException iexc) 
            {
                printinfo( "Info_398fc! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_98gyg! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();			
            strLoc = "Loc_99f99";
            fs2 = new FileStream(filName, FileMode.Create);
            fs3 = new FileStream(fs2.Handle, FileAccess.Read, false, 10, false );
            fs3.Close();
            try 
            {
                fs2.Write(new Byte[]{1}, 0, 1);
                fs2.Flush();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_3989c! Unexpected exception, exc=="+exc.ToString());
            }
            fs2.Close();
            strLoc = "Loc_498vy";
            fs2 = new FileStream("Bug.txt", FileMode.Create);
            fs3 = new FileStream(fs2.Handle, FileAccess.ReadWrite, false, 10, false );
            Byte[] bArr = new Byte[100*1024];
            for(int ii = 0 ; ii < bArr.Length ;ii++)
                bArr[i] = (Byte)ii;
            IAsyncResult iar = fs3.BeginWrite(bArr, 0, bArr.Length, null, null);
            fs3.EndWrite(iar);
            fs3.Close();
            fs2.Close();
            File.Delete("Bug.txt");
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
        Co5762ctor_i_fa_b_i_b cbA = new Co5762ctor_i_fa_b_i_b();
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
