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
public class Co3923get_IsSynchronized
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Hashtable.Synchronized(Hashtable)";
    public static String s_strTFName        = "Co3923get_IsSynchronized.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    private Hashtable hsh2;
    private Int32 iNumberOfElements = 20;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        Hashtable hsh1;
        Hashtable hsh3;
        String strValue;
        Thread[] workers;
        ThreadStart ts1;
        Int32 iNumberOfWorkers = 15;
        Boolean fLoopExit;
        try 
        {
            do
            {
                iCountTestcases++;
                hsh1 = new Hashtable();
                hsh2 = Hashtable.Synchronized(hsh1);
                if(hsh1.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4820fdf! Expected value not returned, " + hsh1.IsSynchronized);
                }
                if(!hsh2.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4820fdf! Expected value not returned, " + hsh2.IsSynchronized);
                }
                hsh3 = Hashtable.Synchronized(hsh2);
                if(!hsh3.IsSynchronized) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_4820fdf! Expected value not returned, " + hsh2.IsSynchronized);
                }
                strLoc = "Loc_8345vdfv";
                iCountTestcases++;
                hsh1 = new Hashtable();
                hsh2 = Hashtable.Synchronized(hsh1);
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
                if(hsh2.Count != iNumberOfElements*iNumberOfWorkers) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_75630fvbdf! Expected value not returned, " + hsh2.Count);
                }
                for(int i=0; i<iNumberOfWorkers; i++)
                {
                    for(int j=0; j<iNumberOfElements; j++)
                    {
                        strValue = "Thread worker " + i + "_" + j;
                        if(!hsh2.Contains(strValue))
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
                if(hsh2.Count != 0) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_6720fvdg! Expected value not returned, " + hsh2.Count);
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
        for(int i=0;i<iNumberOfElements;i++)
        {
            hsh2.Add(strName + "_" + i, "String_" + i);
        }
    }
    void RemoveElements()
    {
        Thread thrd1 = Thread.CurrentThread;
        String strName = thrd1.Name;
        for(int i=0;i<iNumberOfElements;i++)
        {
            hsh2.Remove(strName + "_" + i);
        }
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co3923get_IsSynchronized cbA = new Co3923get_IsSynchronized();
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
