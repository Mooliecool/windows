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
public class Co3924Synchronized
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "SortedList.Synchronized(SortedList)";
    public static String s_strTFName        = "Co3924Synchronized.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    private SortedList slst2;
    private Int32 iNumberOfElements = 20;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        SortedList slst1;
        String strValue;
        Thread[] workers;
        ThreadStart ts1;
        Int32 iNumberOfWorkers = 15;
        Boolean fLoopExit;
        try 
        {
            do
            {
                strLoc = "Loc_8345vdfv";
                Console.WriteLine("Starting adding to the list");
                slst1 = new SortedList();
                slst2 = SortedList.Synchronized(slst1);
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
                Console.WriteLine("Finished adding to the list");
                iCountTestcases++;
                if(slst2.Count != iNumberOfElements*iNumberOfWorkers) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_75630fvbdf! Expected value not returned, " + slst2.Count);
                }
                iCountTestcases++;
                for(int i=0; i<iNumberOfWorkers; i++)
                {
                    for(int j=0; j<iNumberOfElements; j++)
                    {
                        strValue = "Thread worker " + i + "_" + j;
                        if(!slst2.Contains(strValue))
                        {
                            iCountErrors++;
                            Console.WriteLine("Err_452dvdf_" + i + "_" + j + "! Expected value not returned, " + strValue);
                        }
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
                if(slst2.Count != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_6720fvdg! Expected value not returned, " + slst2.Count);
                }
                iCountTestcases++;
                if(slst1.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4820fdf! Expected value not returned, " + slst1.IsSynchronized);
                }
                iCountTestcases++;
                if(!slst2.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4820fdf! Expected value not returned, " + slst2.IsSynchronized);
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
        Thread thrd1 = Thread.CurrentThread;
        String strName = thrd1.Name;
        Console.WriteLine(strName + " started");
        for(int i=0;i<iNumberOfElements;i++)
        {
            slst2.Add(strName + "_" + i, "String_" + i);
        }
        Console.WriteLine(strName + " Finished");
    }
    void RemoveElements()
    {
        Thread thrd1 = Thread.CurrentThread;
        String strName = thrd1.Name;
        for(int i=0;i<iNumberOfElements;i++)
        {
            slst2.Remove(strName + "_" + i);
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co3924Synchronized cbA = new Co3924Synchronized();
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
