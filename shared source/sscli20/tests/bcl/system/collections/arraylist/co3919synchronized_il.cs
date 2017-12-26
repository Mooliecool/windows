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
using System.Threading;
using System.IO;
using System.Collections;
public class Co3919Synchronized_IL
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.FixedSize(IList)";
    public static String s_strTFName        = "Co3919Synchronized_IL.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    private IList ilst1;
    private Int32 iNumberOfElements = 10;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        ArrayList alst1;
        Int32 iNumberOfTimes;
        String strValue;
        Thread[] workers;
        ThreadStart ts1;
        Int32 iNumberOfWorkers = 10;
        Boolean fLoopExit;
        try 
        {
            do
            {
                strLoc = "Loc_8345vdfv";
                alst1 = new ArrayList();
                ilst1 = ArrayList.Synchronized((IList)alst1);
                workers = new Thread[iNumberOfWorkers];
                ts1 = new ThreadStart(AddElements);
                for(int i=0; i<workers.Length; i++)
                {
                    workers[i] = new Thread(ts1);
                    workers[i].Name = "Thread worker " + i;
                    workers[i].Start();
                }
            while(true)
            {
                fLoopExit=false;
                for(int i=0; i<iNumberOfWorkers;i++)
                {
                    if(workers[i].IsAlive)
                        fLoopExit=true;
                }
                if(!fLoopExit)
                    break;
            }
                iCountTestcases++;
                if(ilst1.Count != iNumberOfElements*iNumberOfWorkers) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_75630fvbdf! Expected value not returned, " + ilst1.Count);
                }
                iCountTestcases++;
                for(int i=0; i<iNumberOfElements; i++)
                {
                    iNumberOfTimes = 0;
                    strValue = "String_" + i;
                    for(int j=0; j<ilst1.Count; j++)
                    {
                        if(((String)ilst1[j]).Equals(strValue))
                            iNumberOfTimes++;
                    }
                    if(iNumberOfTimes!=iNumberOfWorkers)
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_452dvdf_" + i + "! Expected value not returned, " + iNumberOfTimes + " " + strValue);
                    }
                }
                workers = new Thread[iNumberOfWorkers];
                ts1 = new ThreadStart(RemoveElements);
                for(int i=0; i<workers.Length; i++)
                {
                    workers[i] = new Thread(ts1);
                    workers[i].Name = "Thread worker " + i;
                    workers[i].Start();
                }
            while(true)
            {
                fLoopExit=false;
                for(int i=0; i<iNumberOfWorkers;i++)
                {
                    if(workers[i].IsAlive)
                        fLoopExit=true;
                }
                if(!fLoopExit)
                    break;
            }
                iCountTestcases++;
                if(ilst1.Count != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_6720fvdg! Expected value not returned, " + ilst1.Count);
                }
                iCountTestcases++;
                if(alst1.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4820fdf! Expected value not returned, " + alst1.IsSynchronized);
                }
                iCountTestcases++;
                if(!ilst1.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4820fdf! Expected value not returned, " + ilst1.IsSynchronized);
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
    void AddElements()
    {
        for(int i=0;i<iNumberOfElements;i++)
        {
            ilst1.Add("String_" + i);
        }
    }
    void RemoveElements()
    {
        for(int i=0;i<iNumberOfElements;i++)
        {
            ilst1.Remove("String_" + i);
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co3919Synchronized_IL cbA = new Co3919Synchronized_IL();
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
