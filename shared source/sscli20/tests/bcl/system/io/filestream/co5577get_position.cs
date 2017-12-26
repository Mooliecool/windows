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
public class Co5577get_Position
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.get_Position";
    public static String s_strTFName        = "Co5577get_Position.cs";
    public static String s_strTFAbbrev      = "Co5577";
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
            StreamWriter sw2;
            String filName = s_strTFAbbrev+"Test.tmp";
            try 
            {
                new FileInfo(filName).Delete();
            } 
            catch (Exception) {} 
            strLoc = "Loc_98yc8";
            fs2 = new FileStream(filName, FileMode.Create);
            iCountTestcases++;
            if(fs2.Position != 0) 
            {
                iCountErrors++;
                printerr( "Error_288c8! Incorrect starting position=="+fs2.Position);
            }
            iCountTestcases++;
            try 
            {
                fs2.Position = -2;
                iCountErrors++;
                printerr( "Error_23890! Expected exception not thrown, position=="+fs2.Position);
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_328xj! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_289ux! Incorrect exception thrown, exc=="+exc.ToString());
            }
            fs2.Close();
            strLoc = "Loc_27yxc";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.SetLength(50);
            fs2.Position = 50;
            Console.WriteLine(fs2.Position);
            sw2 = new StreamWriter(fs2);
            for(char c = 'a' ; c < 'f' ; c++) 
                sw2.Write(c);
            sw2.Flush();
            iCountTestcases++;
            if(fs2.Position != 55) 
            {
                iCountErrors++;
                printerr( "Error_2398x! Incorrect position=="+fs2.Position);
            }
            iCountTestcases++;
            if(fs2.Length != 55) 
            {
                iCountErrors++;
                printerr( "Error_389xd! Incorrect stream length=="+fs2.Length);
            }
            fs2.Position = 0;
            iCountTestcases++;
            if(fs2.Position != 0) 
            {
                iCountErrors++;
                printerr( "Error_398yx! Incorrect position=="+fs2.Position);
            }
            fs2.Position = 30;
            iCountTestcases++;
            if(fs2.Position != 30) 
            {
                iCountErrors++;
                printerr( "Error_3989a! Incorrect position=="+fs2.Position);
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
        Co5577get_Position cbA = new Co5577get_Position();
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
