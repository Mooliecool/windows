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
public class Co9016FullName 
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.OpenText(String)";
    public static String s_strTFName        = "co9016fullname .cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    String strLoc = "Loc_000oo";
    String strValue = String.Empty;
    int iCountErrors = 0;
    int iCountTestcases = 0;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        try
        {		
            FileInfo fil2;
            String strFileName = "" ;			
            strLoc = "Loc_2723d";
            strFileName = "Hello" + Path.DirectorySeparatorChar + "file.tmp";
            fil2 = new FileInfo(strFileName);
            iCountTestcases++;
            if(!fil2.FullName.Equals(s_strTFPath + Path.DirectorySeparatorChar + strFileName)) 
            {
                iCountErrors++;
                printerr( "Error_5yb87! Incorrect name=="+fil2.FullName);
            }
#if !PLATFORM_UNIX
            strFileName = Path.DirectorySeparatorChar + "Directory" + Path.DirectorySeparatorChar + "File" ; 
            fil2 = new FileInfo( strFileName );
            iCountTestcases++;
            if(fil2.FullName.IndexOf(strFileName) != 2) 
            {
                iCountErrors++;
                printerr( "Error_78288! Incorrect name=="+fil2.FullName);
            }

            strFileName = "\\\\Machine\\Directory\\File" ;
            fil2 = new FileInfo( strFileName );
            iCountTestcases++;
            if(!fil2.FullName.Equals(strFileName)) 
            {
                iCountErrors++;
                printerr( "Error_67y8b! Incorrect name=="+fil2.FullName);
            } 
#endif
#if PLATFORM_UNIX
                        strFileName = "/File.tmp hello.blah" ;
#else
            strFileName = "C:\\File.tmp hello.blah" ;
#endif
            fil2 = new FileInfo( strFileName);
            iCountTestcases++;
            if(!fil2.FullName.Equals(strFileName)) 
            {
                iCountErrors++;
                printerr( "Error_2987b! Incorrect name=="+fil2.FullName);
            }
#if PLATFORM_UNIX
                        strFileName = "/Directory/File";
#else
            strFileName = "C://Directory//File";
#endif
            fil2 = new FileInfo( strFileName);
            iCountTestcases++;
            Console.WriteLine( strFileName );
#if PLATFORM_UNIX
			if(!fil2.FullName.Equals(strFileName)) {
#else
            if(!fil2.FullName.Equals("C:\\Directory\\File")) 
            {
#endif
                iCountErrors++;
                printerr( "Error_2y78d! Incorrect name=="+fil2.FullName);
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
        Co9016FullName  cbA = new Co9016FullName ();
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
