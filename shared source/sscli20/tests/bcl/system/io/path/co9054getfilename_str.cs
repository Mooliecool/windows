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
public class Co9054GetFileName_str
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.GetFileName(String)";
    public static String s_strTFName        = "Co9054GetFileName_str.cs";
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
            str2 = Path.GetFileName(null);
            if(str2 != null) 
            {
                iCountErrors++;
                printerr( "Error_297gb! Expected exception not thrown, str2=="+str2);
            }
            strLoc = "Loc_2723d";
            str2 = Path.GetFileName("Hello\\file.tmp");
            iCountTestcases++;
            if(!str2.Equals("file.tmp")) 
            {
                iCountErrors++;
                printerr( "Error_5yb87! Incorrect name=="+str2);
            }
            str2 = Path.GetFileName("\\Directory\\File");
            iCountTestcases++;
            if(!str2.Equals("File")) 
            {
                iCountErrors++;
                printerr( "Error_78288! Incorrect name=="+str2);
            }
#if !PLATFORM_UNIX
            str2 = Path.GetFileName("\\\\Machine\\Directory\\File");
            iCountTestcases++;
            if(!str2.Equals("File")) 
            {
                iCountErrors++;
                printerr( "Error_67y8b! Incorrect name=="+str2);
            } 
#endif
            str2 = Path.GetFileName("C:\\File.tmp hello.blah");
            iCountTestcases++;
            if(!str2.Equals("File.tmp hello.blah")) 
            {
                iCountErrors++;
                printerr( "Error_2987b! Incorrect name=="+str2);
            }
            str2 = Path.GetFileName("C://Directory//File");
            iCountTestcases++;
            if(!str2.Equals("File")) 
            {
                iCountErrors++;
                printerr( "Error_2y78d! Incorrect name=="+str2);
            }
            str2 = Path.GetFileName("C:\\Dir1\\Dir2\\");
            iCountTestcases++;
            if(!str2.Equals(String.Empty)) 
            {
                iCountErrors++;
                printerr( "Error_27yg7! Incorrect name=="+str2);
            }
            str2 = Path.GetFileName("File1");
            iCountTestcases++;
            if(!str2.Equals("File1")) 
            {
                iCountErrors++;
                printerr( "Error_8588v! Incorrect name=="+str2);
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
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
        Co9054GetFileName_str cbA = new Co9054GetFileName_str();
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
