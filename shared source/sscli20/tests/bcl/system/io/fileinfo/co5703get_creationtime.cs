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
public class Co5703get_CreationTime
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Directory()";
    public static String s_strTFName        = "Co5703get_CreationTime.cs";
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
            String filName = s_strTFAbbrev+"TestFile";			
            FileInfo fil2;
            iCountTestcases++;
            strLoc = "Loc_r8r7j";
            DirectoryInfo newDirectory = new DirectoryInfo(".");
            fil2 = new FileInfo(newDirectory + filName);
            iCountTestcases++;
            try 
            {
                if((fil2.CreationTime-DateTime.Now).Seconds > 0) 
                {
                    iCountErrors++;
                    printerr( "Error_20hjx! Creation time cannot be correct");
                }
            } catch (IOException) { 
       	    } catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_20fhd! Unexpected exceptiont thrown: "+exc.ToString());
                printinfo( "");
            }
            fil2.Delete();
            strLoc = "Loc_20yxc";
            fil2 = new FileInfo(s_strTFPath + "\\" + filName);
            FileStream fs = fil2.Create();
            Thread.Sleep(2000);
            iCountTestcases++;
            try 
            {
                Console.WriteLine( DateTime.Now );
                Console.WriteLine( DateTime.Now - fil2.CreationTime);
                Console.WriteLine( fil2.CreationTime);
                if( (DateTime.Now - fil2.CreationTime).Minutes > 1) 
                {
                    iCountErrors++;
                    printerr( "Eror_209x9! Creation time is off");
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_209jx! Unexpected exception thrown: "+exc.ToString());
            } 
            fs.Close();
            fil2.Delete();
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
        Co5703get_CreationTime cbA = new Co5703get_CreationTime();
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
