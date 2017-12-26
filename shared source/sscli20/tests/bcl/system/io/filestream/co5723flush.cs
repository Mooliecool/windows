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
public class Co5723Flush
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "FileStream.Flush";
    public static String s_strTFName        = "Co5723Flush.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
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
            String filName = s_strTFAbbrev + "TestFile.tmp";
            if(File.Exists(filName))
                File.Delete(filName);
            strLoc = "Loc_9g8yg";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Write(new Byte[]{65,66,67,68,69,70}, 0, 6);
            fs2.Flush();			
            StreamReader sr2 = new StreamReader(fs2);
            iCountTestcases++;
            fs2.Position = 0;
            if(!sr2.ReadToEnd().Equals("ABCDEF")) 
            {
                iCountErrors++;
                printerr( "Error_19009! Not flushed correctly");
            }
            fs2.Position = 3;
            fs2.Write(new Byte[]{65,66,67}, 0, 3);
            fs2.Flush();
            fs2.Position = 0;
            if(!sr2.ReadToEnd().Equals("ABCABC")) 
            {
                iCountErrors++;
                printerr( "Error_01909! Not flushed correctly");
            }
            sr2.Close();
            strLoc = "Loc_857yv";
            fs2 = new FileStream(filName, FileMode.Create);
            fs2.Write(new Byte[]{1}, 0, 1);
            fs2.Close();
            iCountTestcases++;
            try 
            {
                fs2.Flush();
                iCountErrors++;
                printerr( "Error_50039! Expected exception not thrown");
            } 
            catch (ObjectDisposedException iexc) 
            {
                printinfo( "Info_0199x! Caught expected exception, exc=="+iexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_298t8! Incorrect exception thrown, exc=="+exc.ToString());
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
        Co5723Flush cbA = new Co5723Flush();
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
