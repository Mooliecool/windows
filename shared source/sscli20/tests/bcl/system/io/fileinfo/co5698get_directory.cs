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
public class Co5698get_Directory
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Directory()";
    public static String s_strTFName        = "Co5698get_Directory.cs";
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
            FileInfo fil2;
            DirectoryInfo dir2;
            strLoc = "Loc_2723d";
            fil2 = new FileInfo("Hello\\file.tmp");
            dir2 = fil2.Directory;
            iCountTestcases++;
            if(!dir2.FullName.Equals(Environment.CurrentDirectory + Path.DirectorySeparatorChar +"Hello")) 
            {
                iCountErrors++;
                printerr( "Error_5yb87! Incorrect name=="+dir2.FullName);
            }

            fil2 = new FileInfo("\\Directory\\File");
            dir2 = fil2.Directory;
            iCountTestcases++;
#if PLATFORM_UNIX
			if(!dir2.FullName.Equals(Path.DirectorySeparatorChar + "Directory")) 
#else
            if(!dir2.FullName.Equals(Directory.GetCurrentDirectory().Substring(0,2)+Path.DirectorySeparatorChar+"Directory")) 
#endif
            {
                iCountErrors++;
                printerr( "Error_78288! Incorrect name=="+dir2.FullName);
            }
#if !PLATFORM_UNIX
            fil2 = new FileInfo("\\\\Machine\\Directory\\File");
            dir2 = fil2.Directory;
            iCountTestcases++;
            if(!dir2.FullName.Equals("\\\\Machine\\Directory")) 
            {
                iCountErrors++;
                printerr( "Error_67y8b! Incorrect name=="+dir2.FullName);
            } 
#endif

#if PLATFORM_UNIX
			fil2 = new FileInfo("/File.tmp hello.blah");
#else
            fil2 = new FileInfo("C:\\File.tmp hello.blah");
#endif
            dir2 = fil2.Directory;
            iCountTestcases++;
#if PLATFORM_UNIX
			if(!dir2.FullName.Equals("/")) 
#else
            if(!dir2.FullName.Equals("C:\\")) 
#endif
            {
                iCountErrors++;
                printerr( "Error_2987b! Incorrect name=="+dir2.FullName);
            }

#if PLATFORM_UNIX
			fil2 = new FileInfo("/Directory/File");
#else
            fil2 = new FileInfo("C:/Directory/File");
#endif
            dir2 = fil2.Directory;
            iCountTestcases++;
#if PLATFORM_UNIX
			if(!dir2.FullName.Equals("/Directory")) 
#else
            if(!dir2.FullName.Equals("C:\\Directory")) 
#endif
            {
                iCountErrors++;
                printerr( "Error_2y78d! Incorrect name=="+dir2.FullName);
            }

#if PLATFORM_UNIX
			fil2 = new FileInfo("/Dir1/Dir2/Dir3/Dir4/File1");
#else
            fil2 = new FileInfo("C:\\Dir1\\Dir2\\Dir3\\Dir4\\File1");
#endif
            dir2 = fil2.Directory;
            iCountTestcases++;
#if PLATFORM_UNIX
			if(!dir2.FullName.Equals("/Dir1/Dir2/Dir3/Dir4"))
#else
            if(!dir2.FullName.Equals("C:\\Dir1\\Dir2\\Dir3\\Dir4"))
#endif
            {
                iCountErrors++;
                printerr( "Error_283fy! Incorrect name=="+dir2.FullName);
            }
            fil2 = new FileInfo("Dir1");
            dir2 = fil2.Directory;
            iCountTestcases++;
            if(!dir2.FullName.Equals(Environment.CurrentDirectory)) 
            {
                iCountErrors++;
                printerr( "Error_758bb! Incorrect name=="+dir2.FullName);
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
        Co5698get_Directory cbA = new Co5698get_Directory();
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
