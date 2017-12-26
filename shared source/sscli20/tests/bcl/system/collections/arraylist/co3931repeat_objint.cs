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
public class Co3931Repeat_ObjInt
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.Repeat(Object, Int32)";
    public static String s_strTFName        = "Co3931Repeat_ObjInt.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        ArrayList alst1;
        try 
        {
            do
            {
                strLoc = "Loc_8345vdfv";
                alst1 = ArrayList.Repeat(5, 1000);
                iCountTestcases++;
                for(int i=0; i<alst1.Count;i++)
                {
                    if((Int32)alst1[i] != 5) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_561dvs_" + i + "! Expected value not returned, " + (Int32)alst1[i]);
                    }
                }
                alst1 = ArrayList.Repeat(null, 10);
                iCountTestcases++;
                for(int i=0; i<alst1.Count;i++)
                {
                    if(alst1[i] != null) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_561dvs_" + i + "! Expected value not returned, " + alst1[i]);
                    }
                }
                alst1 = ArrayList.Repeat(5, 0);
                iCountTestcases++;
                if(alst1.Count != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4530fvgd! Expected value not returned, " + alst1.Count);
                }
                try 
                {
                    iCountTestcases++;
                    alst1 = ArrayList.Repeat(5, -1);
                    iCountErrors++;
                    Console.WriteLine("Err_8342sf! exception not thrown");
                }
                catch(ArgumentException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_135234vdf! Unexpected exception, " + ex.ToString());
                }
            } while (false);
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
            return true;
        }
        else
        {
            Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co3931Repeat_ObjInt cbA = new Co3931Repeat_ObjInt();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
    }
}
internal class Foo 
{
    internal String strValue;
    internal Foo()
    {
        strValue = "Hello World";
    }
}
