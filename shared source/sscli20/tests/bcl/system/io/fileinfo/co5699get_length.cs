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
public class Co5699get_Length
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "File.Directory()";
    public static String s_strTFName        = "Co5699get_Length.cs";
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
            FileStream fs2;		
            StreamWriter sw2;
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_98yc8";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Close();
            fil2 = new FileInfo(filName);
            iCountTestcases++;
            if(fil2.Length != 0) 
            {
                iCountErrors++;
                printerr( "Error_2898t! Incorrect length=="+fs2.Length);
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
            fil2 = new FileInfo(filName);
            fs2.Close();
            if(fil2.Length != 55) 
            {
                iCountErrors++;
                printerr( "Error_389xd! Incorrect stream length=="+fs2.Length+", filelength=="+fil2.Length);
            }
            strLoc = "Loc_f47yv";
            fs2 = new FileStream(filName, FileMode.Open);
            fs2.SetLength(30);
            fs2.Flush();
            iCountTestcases++;
            fs2.Close();
            fil2.Refresh();
            if(fil2.Length != 30) 
            {
                iCountErrors++;
                printerr( "Error_28xye! Incorrect, filelength=="+fil2.Length);
            }
            strLoc = "Loc_487yv";
            fs2 = new FileStream(filName, FileMode.Open);
            fs2.SetLength(100);
            fs2.Flush();
            fs2.Close();
            fil2.Refresh();
            iCountTestcases++;
            if(fil2.Length != 100) 
            {
                iCountErrors++;
                printerr( "Error_2090x! Incorrect filelength=="+fil2.Length);
            }			
            fs2.Close();
            fil2.Refresh();
            iCountTestcases++;
            if(fil2.Length != 100) 
            {
                iCountErrors++;
                printerr( "Error_297ty! Incorrect length=="+fs2.Length);
            }
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
        Co5699get_Length cbA = new Co5699get_Length();
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
