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
using System.Collections;
using System.Globalization;
using System.IO;
public class Co8620Contains_obj
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Stack.Contains(Object)";
    public static String s_strTFName        = "Co8620Contains_obj.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Stack stk;		
        try
        {
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            stk = new Stack();
            for(int i=0; i<100; i++)
                stk.Push(i);
            if(stk.Count!=100)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned");
            }
            for(int i=0; i<100; i++)
            {
                if(!stk.Contains(i))
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_93475sdg! wrong value returned");
                }
            }
            iCountTestcases++;
            if(stk.Contains(150))
            {
                iCountErrors++;
                Console.WriteLine( "Err_93475sdg! wrong value returned");
            }
            if(stk.Contains("Hello World"))
            {
                iCountErrors++;
                Console.WriteLine( "Err_93475sdg! wrong value returned");
            }
            iCountTestcases++;
            if(stk.Contains(null))
            {
                iCountErrors++;
                Console.WriteLine( "Err_3276sfg! wrong value returned");
            }
            stk.Push(null);
            if(!stk.Contains(null))
            {
                iCountErrors++;
                Console.WriteLine( "Err_9734sg! wrong value returned");
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
            Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co8620Contains_obj cbA = new Co8620Contains_obj();
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
