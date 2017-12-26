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
public class Co9052GetDirectoryName_str
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.GetDirectoryName(String)";
    public static String s_strTFName        = "Co9052GetDirectoryName_str.cs";
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
            String str2;
            strLoc = "Loc_2798f";
            iCountTestcases++;
            str2 =	Path.GetDirectoryName(null);
            if(str2 != null) 
            {
                iCountErrors++;
                printerr( "Error_297gb! Expected exception not thrown");
            }
            strLoc = "Loc_2723d";
            str2 = Path.GetDirectoryName("Hello" + Path.DirectorySeparatorChar + "file.tmp");
            iCountTestcases++;
            if(!str2.Equals("Hello")) 
            {
                iCountErrors++;
                printerr( "Error_5yb87! Incorrect name=="+str2);
            }

            iCountTestcases++;
#if PLATFORM_UNIX
			str2 = Path.GetDirectoryName("/Directory/File");
			if(!str2.Equals("/Directory"))
#else
            str2 = Path.GetDirectoryName("\\Directory\\File");
            if(!str2.Equals("\\Directory"))
#endif
            {
                iCountErrors++;
                printerr( "Error_78288! Incorrect name=="+str2);
            }
#if !PLATFORM_UNIX
            str2 = Path.GetDirectoryName("\\\\Machine\\Directory\\File");
            iCountTestcases++;
            if(!str2.Equals("\\\\Machine\\Directory")) 
            {
                iCountErrors++;
                printerr( "Error_67y8b! Incorrect name=="+str2);
            } 
#endif
            iCountTestcases++;
#if PLATFORM_UNIX
			str2 = Path.GetDirectoryName("/File.tmp hello.blah");
			if(!str2.Equals("/")) 
#else
            str2 = Path.GetDirectoryName("C:\\File.tmp hello.blah");
            if(!str2.Equals("C:\\")) 
#endif
            {
                iCountErrors++;
                printerr( "Error_2987b! Incorrect name=="+str2);
            }

            iCountTestcases++;
#if PLATFORM_UNIX
			str2 = Path.GetDirectoryName("/Directory/File");
			if(!str2.Equals(@"/Directory"))
#else
            str2 = Path.GetDirectoryName("C:/Directory/File");
            if(!str2.Equals(@"C:\Directory"))
#endif
            {
                iCountErrors++;
                printerr( "Error_2y78d! Incorrect name=="+str2);
            }

            iCountTestcases++;
#if PLATFORM_UNIX
			str2 = Path.GetDirectoryName("/Dir1/Dir2/");
			if(!str2.Equals("/Dir1/Dir2"))
#else
            str2 = Path.GetDirectoryName("C:\\Dir1\\Dir2\\");
            if(!str2.Equals("C:\\Dir1\\Dir2"))
#endif
            {
                iCountErrors++;
                printerr( "Error_287gy! Incorrect name=="+str2);
            } 

            iCountTestcases++;
#if PLATFORM_UNIX
			str2 = Path.GetDirectoryName("/Dir1/Dir2/Dir3/Dir4/File1");
			if(!str2.Equals("/Dir1/Dir2/Dir3/Dir4"))
#else
            str2 = Path.GetDirectoryName("C:\\Dir1\\Dir2\\Dir3\\Dir4\\File1");
            if(!str2.Equals("C:\\Dir1\\Dir2\\Dir3\\Dir4"))
#endif
            {
                iCountErrors++;
                printerr( "Error_283fy! Incorrect name=="+str2);
            }
#if !PLATFORM_UNIX
            str2 = Path.GetDirectoryName("c:\\");
            iCountTestcases++;
            if(str2 != null) 
            {
                iCountErrors++;
                printerr( "Error_t721b! Incorrect name=="+str2);
            }

            str2 = Path.GetDirectoryName("\\\\fxqasql\\TestDrivers\\FxBcl\\IO");
            iCountTestcases++;
            if(!str2.Equals("\\\\fxqasql\\TestDrivers\\FxBcl")) 
            {
                iCountErrors++;
                printerr( "Error_758bb! Incorrect name=="+str2 );
            }
#endif
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
        Co9052GetDirectoryName_str cbA = new Co9052GetDirectoryName_str();
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
