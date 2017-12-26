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
public class Co8619Clone
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Stack.Clone()";
    public static String s_strTFName        = "Co8619Clone.cs";
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
        Stack stkClone;		
        A a1;
        A a2;
        try
        {
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            stk = new Stack();
            for(int i=0; i<100; i++)
                stk.Push(i);
            stkClone = (Stack)stk.Clone();
            if(stkClone.Count!=100)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned");
            }
            for(int i=0; i<100; i++)
            {
                if(!stkClone.Contains(i))
                {
                    iCountErrors++;
                    Console.WriteLine( "Err_93475sdg! wrong value returned");
                }
            }
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            stk = new Stack();
            stk.Push(new A(10));
            stkClone = (Stack)stk.Clone();
            if(stkClone.Count!=1)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned");
            }
            a1 = (A)stk.Pop();
            a1.I=50;
            if(stkClone.Count!=1)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned");
            }
            a2 = (A)stkClone.Pop();
            if(a2.I!=50)
            {
                iCountErrors++;
                Console.WriteLine( "Err_93745sdg! wrong value returned, " + a2.I);
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
        Co8619Clone cbA = new Co8619Clone();
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
class A
{
    int i;
    public A(int i)
    {
        this.i=i;
    }
    internal Int32 I
    {
        set{i=value;}
        get{return i;}
    }
}
