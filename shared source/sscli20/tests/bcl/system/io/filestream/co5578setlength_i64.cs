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
public class Co5578SetLength_i64
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.SetLength(Int64)";
    public static String s_strTFName        = "Co5578SetLength_i64.cs";
    public static String s_strTFAbbrev      = "Co5578";
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
        int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue/2, Int32.MaxValue/10, 10000, 100000 , Int32.MaxValue/20, Int32.MaxValue/100 , Int32.MaxValue/1000 };
        try
        {
            FileStream fs2;
            StreamWriter sw2;
            String filName = s_strTFAbbrev+"Test.tmp";
            try 
            {
                new FileInfo(filName).Delete();
            } 
            catch (Exception) {} 
            strLoc = "Loc_98yc8";
            for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++)
            {
                fs2 = new FileStream(filName, FileMode.Create);
                iCountTestcases++;
                try 
                {
                    fs2.SetLength(iArrInvalidValues[iLoop]);
                    iCountErrors++;
                    printerr( "Error_23890! Expected exception not thrown, position=="+fs2.Position);
                } 
                catch (ArgumentOutOfRangeException aexc) 
                {
                    printinfo( "Info_328xj! Caught expected exception, aexc=="+aexc.Message);
                } 
                catch (Exception exc) 
                {
                    iCountErrors++;
                    printerr( "Error_289ux! Incorrect exception thrown, exc=="+exc.ToString());
                }
                fs2.Close();
            }
            strLoc = "Loc_27yxc";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.SetLength(50);
            fs2.Position = 50;
            sw2 = new StreamWriter(fs2);
            for(char c = 'a' ; c < 'f' ; c++) 
                sw2.Write(c);
            sw2.Flush();
            iCountTestcases++;
            if(fs2.Length != 55) 
            {
                iCountErrors++;
                printerr( "Error_389xd! Incorrect stream length=="+fs2.Length);
            }
            fs2.SetLength(30);
            iCountTestcases++;
            if(fs2.Length != 30) 
            {
                iCountErrors++;
                printerr( "Error_28xye! Incorrect length=="+fs2.Length);
            }
            iCountTestcases++;
            if(fs2.Position != 30) 
            {
                iCountErrors++;
                printerr( "Error_3989a! Incorrect position=="+fs2.Position);
            }
            fs2.SetLength(100);
            iCountTestcases++;
            if(fs2.Length != 100) 
            {
                iCountErrors++;
                printerr( "Error_2090x! Incorrect length=="+fs2.Length);
            }
            fs2.Close();
            strLoc = "Loc_7572i";
            for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++)
            {
                fs2 = new FileStream(filName, FileMode.Create);
                try 
                {
                    fs2.SetLength(iArrLargeValues[iLoop]);
                    iCountTestcases++;
                    if(fs2.Length != iArrLargeValues[iLoop]) 
                    {
                        iCountErrors++;
                        printerr( "Error_298xh! Incorrect position=="+fs2.Position);
                    }
                } 
                catch (IOException iexc) 
                {
                    printinfo("Info_g98yb! Not enough disk space, iexc=="+iexc.Message);
                }
                fs2.Close();  
            }
            strLoc = "Loc_99189";
            fs2 = new FileStream(filName, FileMode.Create);
            iCountTestcases++;
            try 
            {
                fs2.SetLength(Int64.MaxValue);
                iCountErrors++;
                printerr( "Error_209xu! Expected exception not thrown, fs2.Length=="+fs2.Length);
            } 
            catch (IOException iexc) 
            {
                printinfo( "Info_24889! Caught expected exception, iexc=="+iexc.Message);
            } 
            catch (ArgumentOutOfRangeException e) 
            {
                printinfo( "Info_5u765! Caught expected exception, e=="+e.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_200fu! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();
            new FileInfo(filName).Delete();
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
        Co5578SetLength_i64 cbA = new Co5578SetLength_i64();
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
