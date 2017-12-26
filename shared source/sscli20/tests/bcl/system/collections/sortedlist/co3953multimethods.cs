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
using System.Reflection;
interface IDescribeTestedMethods
{
    MemberInfo[] GetTestedMethods();
}
public class Co3953MultiMethods:IDescribeTestedMethods
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "SortedList.Assorted Methods";
    public static String s_strTFName        = "Co3953MultiMethods.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public MemberInfo[] GetTestedMethods()
    {
        Type type = typeof(SortedList);
        ArrayList list = new ArrayList();
        MethodInfo[] methods = type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static);
        for(int i=0; i<methods.Length; i++)
        {
            if (methods[i].IsPublic && (methods[i].DeclaringType == methods[i].ReflectedType)) 
            {
                list.Add(methods[i]);
            }
        }
        ConstructorInfo[] ctors = type.GetConstructors(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static);
        for(int i=0; i<ctors.Length; i++) 
        {
            if (ctors[i].IsPublic  && (ctors[i].DeclaringType == ctors[i].ReflectedType)) 
            {
                list.Add(ctors[i]);
            }
        }
        MemberInfo[] members = new MemberInfo[list.Count];
        list.CopyTo(members, 0);
        return members;
    }
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        SortedList slst1;
        Int32 iNumberOfElements;
        String strValue;
        Array ar1;
        try 
        {
            do
            {
                strLoc = "Loc_001fn";
                iNumberOfElements = 10;
                slst1 = new SortedList(iNumberOfElements);
                for(int i=iNumberOfElements-1; i>=0;i--)
                {
                    slst1.Add(50 + i, "Value_" + i);
                }
                iCountTestcases++;
                if(slst1.Count!=iNumberOfElements) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, " + slst1.Count + " " + iNumberOfElements);
                }
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i;
                    if(!strValue.Equals(slst1[slst1.GetKey(i)])) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_000oo! Expected value not returned, " + strValue + " " + slst1[slst1.GetKey(i)]);
                    }
                }
                strLoc = "Loc_756tegd";
                try
                {
                    iCountTestcases++;
                    iNumberOfElements = -5;
                    slst1 = new SortedList(iNumberOfElements);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentOutOfRangeException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_097243sdgg! Unexpected exception thrown, " + ex);
                }
                strLoc = "Loc_74dsg";
                slst1 = new SortedList();
                strLoc = "Loc_175fdg";
                try
                {
                    iCountTestcases++;
                    slst1.Add(null, 5);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0642efss! Unexpected exception thrown, " + ex);
                }
                iNumberOfElements = 10;
                slst1 = new SortedList();
                for(int i=iNumberOfElements-1; i>=0;i--)
                {
                    slst1.Add(i, null);
                }
                iCountTestcases++;
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i;
                    if(slst1.GetByIndex(i)!=null) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_7620cdg! Expected value not returned, " + slst1.GetByIndex(i));
                    }
                }
                strLoc = "Loc_07534efsdgs";
                iNumberOfElements = 10;
                slst1 = new SortedList(iNumberOfElements);
                for(int i=iNumberOfElements-1; i>=0;i--)
                {
                    slst1.Add(50 + i, "Value_" + i);
                }
                iCountTestcases++;
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i;
                    if(!slst1.Contains(50 + i)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_7423fg! Expected value not returned");
                    }
                }
                iCountTestcases++;
                if(slst1.Contains(1)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, ");
                }
                iCountTestcases++;
                if(slst1.Contains(-1)) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, ");
                }
                strLoc = "Loc_074dgf";
                try
                {
                    iCountTestcases++;
                    slst1.Contains(null);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_742fns! Unexpected exception thrown, " + ex);
                }
                strLoc = "Loc_10745dfs";
                slst1 = new SortedList();
                for(int i=iNumberOfElements-1; i>=0;i--)
                {
                    slst1[50+i] =  "Value_" + i;
                }
                iCountTestcases++;
                if(slst1.Count!=iNumberOfElements) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, " + slst1.Count + " " + iNumberOfElements);
                }
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i;
                    if(!strValue.Equals(slst1[i+50])) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_001oo! Expected value not returned, " + strValue + " " + slst1[i+50]);
                    }
                }
                strLoc = "Loc_175erffg";
                iCountTestcases++;
                strValue = "Value_1";
                if(!strValue.Equals(slst1[51])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, " + slst1[51]);
                }
                strValue = "Different value";
                slst1[51] =  strValue ;
                if(!strValue.Equals(slst1[51])) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, " + slst1[51]);
                }
                strLoc = "Loc_63rfdg";
                try
                {
                    iCountTestcases++;
                    slst1[null] = "Not a chance";
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_7653dsv! Unexpected exception thrown, " + ex);
                }
                strValue = null;
                slst1[51] =  strValue ;
                if(slst1[51] != null) 
                {
                    iCountErrors++;
                    Console.WriteLine("Err_752dsg! Expected value not returned, " + slst1[51]);
                }
                strLoc = "Loc_7435gfdsg";
                iNumberOfElements = 10;
                slst1 = new SortedList();
                for(int i=iNumberOfElements-1; i>=0;i--)
                {
                    slst1.Add(50 + i, "Value_" + i);
                }
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i + 50;
                    slst1.SetByIndex(i, strValue);
                }
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i + 50;
                    if(!strValue.Equals(slst1.GetByIndex(i))) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_003oo! Expected value not returned, " + strValue + " " + slst1.GetByIndex(i));
                    }
                }
                strLoc = "Loc_7645cfxgd";
                try
                {
                    iCountTestcases++;
                    slst1.SetByIndex(-1, strValue);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentOutOfRangeException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0155234sfdg! Unexpected exception thrown, " + ex);
                }
                try
                {
                    iCountTestcases++;
                    slst1.SetByIndex(slst1.Count, strValue);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentOutOfRangeException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_7320678dfv! Unexpected exception thrown, " + ex);
                }
                strLoc = "Loc_0734tefgd";
                iCountTestcases++;
                iNumberOfElements = 10;
                slst1 = new SortedList();
                for(int i=iNumberOfElements-1; i>=0;i--)
                {
                    slst1.Add(50 + i, "Value_" + i);
                }
                ar1 = Array.CreateInstance(typeof(DictionaryEntry), iNumberOfElements);
                slst1.CopyTo(ar1, 0);
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i;
                    if(!strValue.Equals(((DictionaryEntry)ar1.GetValue(i)).Value)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_004oo! Expected value not returned, " + strValue + " " + ((DictionaryEntry)ar1.GetValue(i)).Value);
                    }
                }
                strLoc = "Loc_7645tgbfgg";
                try
                {
                    iCountTestcases++;
                    ar1 = Array.CreateInstance(typeof(String), iNumberOfElements);
                    slst1.CopyTo(ar1, 0);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(InvalidCastException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_2078sdfd! Unexpected exception thrown, " + ex);
                }
                try
                {
                    iCountTestcases++;
                    slst1.CopyTo(null, 0);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentNullException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_9173sdf! Unexpected exception thrown, " + ex);
                }
                try
                {
                    iCountTestcases++;
                    ar1 = Array.CreateInstance(typeof(DictionaryEntry), iNumberOfElements);
                    slst1.CopyTo(ar1, -1);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentOutOfRangeException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_0175xvg! Unexpected exception thrown, " + ex);
                }
                try
                {
                    iCountTestcases++;
                    ar1 = Array.CreateInstance(typeof(String), iNumberOfElements);
                    slst1.CopyTo(ar1, 1);
                    iCountErrors++;
                    Console.WriteLine("Err_7439dg! Exception not thrown");
                }
                catch(ArgumentException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_723-2sfs! Unexpected exception thrown, " + ex);
                }
                strLoc = "Loc_00047fgfdgg";
                ar1 = Array.CreateInstance(typeof(DictionaryEntry), 2*iNumberOfElements);
                slst1.CopyTo(ar1, iNumberOfElements);
                for(int i=0; i<slst1.Count;i++)
                {
                    strValue = "Value_" + i;
                    if(!strValue.Equals(((DictionaryEntry)ar1.GetValue(iNumberOfElements + i)).Value)) 
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_005oo! Expected value not returned, <" + strValue + "> <" + ((DictionaryEntry)ar1.GetValue(iNumberOfElements + i)).Value + "><" + ((DictionaryEntry)ar1.GetValue(i)).Value + ">");
                    }
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
        Co3953MultiMethods cbA = new Co3953MultiMethods();
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
