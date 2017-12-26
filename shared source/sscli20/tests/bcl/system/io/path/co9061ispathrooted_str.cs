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
public class Co9061IsPathRooted
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.IsPathRooted";
    public static String s_strTFName        = "Co9061IsPathRooted";
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
            Boolean bo3;      			
            strLoc = "Loc_27y08";
            iCountTestcases++;
            try 
            {
                bo3 = Path.IsPathRooted("<|>");
                iCountErrors++;
                printerr( "Error_94821! Expected exception not thrown, bo=="+bo3);
            } 
            catch (ArgumentException aexc) 
            {
                printinfo( "Info_6109d! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_0950v! Incorrect exception thrown, exc=="+exc.ToString());
            }
#if !PLATFORM_UNIX
            strLoc = "Loc_2478b";
            iCountTestcases++;
            if(!Path.IsPathRooted("\\\\public")) 
            {
                iCountErrors++;
                printerr( "Error_09t8b! Incorrect return");
            }
#endif
            strLoc = "Loc_276t8";
            iCountTestcases++;
            if ( Path.IsPathRooted(null) )
            {
                iCountErrors++;
                printerr( "Error_0688b! Expected exception not thrown");
            } 
            iCountTestcases++;
            if(!Path.IsPathRooted("\\public")) 
            {
                iCountErrors++;
                printerr( "Error_9276b! Incorrect return");
            }
#if PLATFORM_UNIX
			iCountTestcases++;
			if(!Path.IsPathRooted("/this/is/a/test")) {
				iCountErrors++;
				printerr( "Error_2808g! Incorrect return");
			}
			iCountTestcases++;
			if(!Path.IsPathRooted("/")) {
				iCountErrors++;
				printerr("Error_209d2! Incorrect return");
			}
#else
            iCountTestcases++;
            if(!Path.IsPathRooted("C:\\this\\is\\a\\test")) 
            {
                iCountErrors++;
                printerr( "Error_2808g! Incorrect return");
            }
            iCountTestcases++;
            if(!Path.IsPathRooted("A:\\")) 
            {
                iCountErrors++;
                printerr("Error_209d2! Incorrect return");
            }
#endif
            iCountTestcases++;
            if(!Path.IsPathRooted("////HEllo//World")) 
            {
                iCountErrors++;
                printerr( "Error_6949g! Incorrect return");
            }
            iCountTestcases++;
            if(!Path.IsPathRooted("\\")) 
            {
                iCountErrors++;
                printerr( "Error_5y68s! Incorrect return");
            }
            strLoc = "Loc_29f08";
            iCountTestcases++;
            if(Path.IsPathRooted("Bogus")) 
            {
                iCountErrors++;
                printerr( "Error_9487b! Incorrect return");
            } 
            iCountTestcases++;
            if(Path.IsPathRooted("hello\\C:\\::::")) 
            {
                iCountErrors++;
                printerr( "Error_157gb! Incorrect return");
            }
            iCountTestcases++;
            if(Path.IsPathRooted("AAAAAAAAAAAAAAAAAAAAAAA:\\")) 
            {
                iCountErrors++;
                printerr( "Error_029g7! Incorrect return");
            }
            iCountTestcases++;
            if(Path.IsPathRooted("aaaaaaaaaa\\aaaaaaaaaaaaaa\\\\\\")) 
            {
                iCountErrors++;
                printerr( "Error_96y67y! Incorrect return");
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
        Co9061IsPathRooted cbA = new Co9061IsPathRooted();
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
