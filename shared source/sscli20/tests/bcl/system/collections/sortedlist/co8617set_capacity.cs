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
public class Co8617set_Capacity
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "SortedList.Capacity";
    public static String s_strTFName        = "Co8617set_Capacity.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList list;		
        Int32 iCurrentDefaultCapacity;
        Int32 capacity;
        try
        {
            strLoc = "Loc_384sdg";
            iCountTestcases++;
            iCurrentDefaultCapacity = 0; 
            list = new SortedList();
            if(list.Capacity!=iCurrentDefaultCapacity)
            {
                iCountErrors++;
                Console.WriteLine( "Err_9347sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", iCurrentDefaultCapacity, list.Capacity);
            }
            list.Capacity = 3;
            if(list.Capacity!=3)
            {
                iCountErrors++;
                Console.WriteLine( "Err_987345sg! wrong value returned. Expected - <{0}>, Returned - <{1}>", 3, list.Capacity);
            }
            iCountTestcases++;
            list = new SortedList(0);
            capacity = 0;
            list.Capacity = capacity;
            if(list.Capacity!=capacity)
            {
                iCountErrors++;
                Console.WriteLine( "Err_2947sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", capacity, list.Capacity);
            }
            capacity = 5000;
            list.Capacity = capacity;
            if(list.Capacity!=capacity)
            {
                iCountErrors++;
                Console.WriteLine( "Err_34058wgsd! wrong value returned. Expected - <{0}>, Returned - <{1}>", capacity, list.Capacity);
            }
            iCountTestcases++;
            list = new SortedList();
            capacity = 0;
            list.Capacity = capacity;
            if(list.Capacity!=iCurrentDefaultCapacity)
            {
                iCountErrors++;
                Console.WriteLine( "Err_2947sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", iCurrentDefaultCapacity, list.Capacity);
            }
            list = new SortedList(5000);
            capacity = 0;
            list.Capacity = capacity;
            if(list.Capacity!=iCurrentDefaultCapacity)
            {
                iCountErrors++;
                Console.WriteLine( "Err_2947sdg! wrong value returned. Expected - <{0}>, Returned - <{1}>", iCurrentDefaultCapacity, list.Capacity);
            }
            iCountTestcases++;
            list = new SortedList();
            for(int i=0; i<5000; i++)
                list.Add(i, i);
            capacity = 0;
            try
            {
                list.Capacity = capacity;
                iCountErrors++;
                Console.WriteLine( "Err_2947sdg! Exception not thrown");
            }
            catch(ArgumentOutOfRangeException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_85rgw wrong excepion thrown, " + ex.GetType().Name);
            }    		
            list = new SortedList(0);
            capacity = -1;
            try
            {
                list.Capacity = capacity;
                iCountErrors++;
                Console.WriteLine( "Err_2947sdg! Exception not thrown");
            }
            catch(ArgumentOutOfRangeException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_85rgw wrong excepion thrown, " + ex.GetType().Name);
            }    		
            iCountTestcases++;
            list = new SortedList(0);
            capacity = Int32.MaxValue;
            try
            {
                list.Capacity = capacity;
                iCountErrors++;
                Console.WriteLine( "Err_2947sdg! Exception not thrown");
            }
            catch(OutOfMemoryException)
            {
            }
            catch(Exception ex)
            {
                iCountErrors++;
                Console.WriteLine("Err_85rgw wrong excepion thrown, " + ex.GetType().Name);
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
        Co8617set_Capacity cbA = new Co8617set_Capacity();
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
