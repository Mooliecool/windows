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
public class Co5531CopyTo_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.CopyTo(String)";
    public static String s_strTFName        = "co5531copyto_str.cs";
    public static String s_strTFAbbrev      = "co5531";
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
            FileInfo fil2 = null;
            FileInfo fil1 = null;
            Char[] cWriteArr, cReadArr;
            StreamWriter sw2;
            StreamReader sr2;
            FileStream fs2;
            String filName = s_strTFAbbrev+"Test.tmp";
            try 
            {
                new FileInfo(filName).Delete();
            } 
            catch (Exception) {}
            strLoc = "Loc_498yg";
            new FileStream(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            try 
            {
                fil1 = fil2.CopyTo(null);
                iCountErrors++;
                printerr( "Error_209uz! Expected exception not thrown, fil2=="+fil1.FullName);
                fil1.Delete();
            } 
            catch (ArgumentNullException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_21x99! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fil2.Delete();
            strLoc = "Loc_298vy";
            FileStream fs = new FileStream(filName, FileMode.Create);
            fs.Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            try 
            {
                fil1 = fil2.CopyTo(String.Empty);
                iCountErrors++;
                printerr( "Error_092u9! Expected exception not thrown, fil2=="+fil1.FullName);
                fil1.Delete();
            } 
            catch (ArgumentException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_109uc! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fil2.Delete();
            strLoc = "Loc_289vy";
            new FileStream(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            try 
            {
                fil1 = fil2.CopyTo(".");
                iCountErrors++;
                printerr( "Error_301ju! Expected exception not thrown, fil2=="+fil1.FullName);
                fil1.Delete();
            } 
            catch (IOException iexc) 
            {
                printinfo("Info_2389! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_545345! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fil2.Delete();
            strLoc = "Loc_r7yd9";
            new FileStream(filName, FileMode.Create).Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            try 
            {
                fil1 = fil2.CopyTo(fil2.Name);
                iCountErrors++;
                printerr( "Error_54534! Expected exception not thrown, fil2=="+fil1.FullName);
                fil1.Delete();
            } 
            catch (IOException iexc) 
            {
                printinfo("Info_298xh! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_f588y! Unexpected exception thrown, exc=="+exc.ToString());
            } 
            fil2.Delete();
            strLoc = "Loc_f548y";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            try 
            {
                fil1 = fil2.CopyTo(Environment.CurrentDirectory + Path.DirectorySeparatorChar + "TestFile.tmp1");
                if(!fil1.Exists) 
                {
                    iCountErrors++;
                    printerr( "Error_2978y! File not copied");
                }
                if(!fil2.Exists) 
                {
                    iCountErrors++;
                    printerr( "Error_239vr! Source file gone");
                } 
                fil1.Delete();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2987v! Unexpected exception, exc=="+exc.ToString());
            }
            fil2.Delete();
            fil1.Delete();
            strLoc = "Loc_984hg";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            try 
            {
                fil1 = fil2.CopyTo("**");
                iCountErrors++;
                printerr( "Error_298xh! Expected exception not thrown, fil2=="+fil1.FullName);
                fil1.Delete();
            } 
            catch (ArgumentException) 
            {
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_2091s! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fil2.Delete();
            strLoc = "Loc_t98ck";
            try 
            {
                File.Delete("TestTest.tmp");
                sw2 = new StreamWriter(filName, true);
                cWriteArr = new Char[26];
                int j = 0;
                for(Char i = 'A' ; i <= 'Z' ; i++) 
                    cWriteArr[j++] = i;
                sw2.Write(cWriteArr, 0, cWriteArr.Length);
                sw2.Flush();
                sw2.Close();
                fil2 = new FileInfo(filName);
                fil1 = fil2.CopyTo("TestTest.tmp");
                iCountTestcases++;
                if(!fil1.Exists) 
                {
                    iCountErrors++;
                    printerr( "Error_9u99s! File not copied correctly: "+fil1.FullName);
                }
                if(!fil2.Exists) 
                {
                    iCountErrors++;
                    printerr( "Error_29h7b! Source file gone");
                }			
                sr2 = new StreamReader("TestTest.tmp");
                cReadArr = new Char[cWriteArr.Length];
                sr2.Read(cReadArr, 0, cReadArr.Length);
                iCountTestcases++;
                for(int i = 0 ; i < cReadArr.Length ; i++) 
                {
                    iCountTestcases++;
                    if( cReadArr[i] != cWriteArr[i]) 
                    {
                        iCountErrors++;
                        printerr( "Error_98yv7! Expected=="+cWriteArr[i]+", got value=="+cReadArr[i]);
                    }
                }
                sr2.Close();
                fil1.Delete();
                fil2.Delete();
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_28vc8! Unexpected exception, exc=="+exc.ToString());
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
        Co5531CopyTo_str cbA = new Co5531CopyTo_str();
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
