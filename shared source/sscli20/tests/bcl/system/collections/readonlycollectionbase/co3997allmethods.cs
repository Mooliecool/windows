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
public class Co3997AllMethods
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ReadOnlyCollectionBase.AllMethods";
    public static String s_strTFName        = "Co3997AllMethods.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        MyReadOnlyCollectionBase mycol1;
        MyReadOnlyCollectionBase mycol2;
        Foo f1;
        Foo[] arrF1;
        Foo[] arrF2;
        IEnumerator enu1;
        Int32 iCount;
        Object obj1;
        try 
        {
            do
            {
                iCountTestcases++;
                iCountTestcases++;
                strLoc = "Loc_001oo";
                arrF1 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF1[i]=new Foo();
                mycol1 = new MyReadOnlyCollectionBase(arrF1);
                if(mycol1.Count!=100)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_2075sg! Expected value not returned, " + mycol1.Count);
                }
                iCountTestcases++;
                strLoc = "Loc_002oo";
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                mycol1 = new MyReadOnlyCollectionBase(arrF2);
                arrF1 = new Foo[100];
                mycol1.CopyTo(arrF1, 0);
                for(int i=0; i<100; i++)
                {
                    if((arrF1[i].IValue != i) || (arrF1[i].SValue!= i.ToString()) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_2874sf_" + i + "! Expected value not returned");
                    }
                }
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                mycol1 = new MyReadOnlyCollectionBase(arrF2);
                arrF1 = new Foo[100];
                try
                {
                    mycol1.CopyTo(arrF1, 50);
                    iCountErrors++;
                    Console.WriteLine("Err_2075dfgv! Exception not thrown");
                }
                catch(ArgumentException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_854732f! Unexception not thrown, " + ex.GetType().Name);
                }
                try
                {
                    mycol1.CopyTo(arrF1, -1);
                    iCountErrors++;
                    Console.WriteLine("Err_2075dfgv! Exception not thrown");
                }
                catch(ArgumentException)
                {
                }
                catch(Exception ex)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_854732f! Unexception not thrown, " + ex.GetType().Name);
                }
                arrF1 = new Foo[200];
                mycol1.CopyTo(arrF1, 100);
                for(int i=0; i<100; i++)
                {
                    if((arrF1[100+i].IValue != i) || (arrF1[100+i].SValue!= i.ToString()) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_2874sf_" + i + "! Expected value not returned");
                    }
                }
                iCountTestcases++;
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                mycol1 = new MyReadOnlyCollectionBase(arrF2);
                enu1 = mycol1.GetEnumerator();
                try
                {
                    f1 = (Foo)enu1.Current;
                    iCountErrors++;
                    Console.WriteLine("Err_87543! Exception not thrown");
                }
                catch(InvalidOperationException)
                {
                }
                catch(Exception ex)
                {
                    Console.WriteLine("fail, should throw InvalidOperationException, thrown, " + ex.GetType().Name);
                }
                iCount=0;
            while( enu1.MoveNext() )
            {			
                f1 = (Foo)enu1.Current;
                if((f1.IValue != iCount) || (f1.SValue!= iCount.ToString()) )
                {
                    iCountErrors++;
                    Console.WriteLine("Err_87543! does not match, " + f1.IValue);
                }
                iCount++;
            }
                if(iCount!=100)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_87543! doesnot match");
                }
                try
                {
                    f1 = (Foo)enu1.Current;
                    iCountErrors++;
                    Console.WriteLine("Err_438fsfd! Exception not thrown");
                }
                catch(InvalidOperationException)
                {
                }
                catch(Exception ex)
                {
                    Console.WriteLine("fail, should throw InvalidOperationException, thrown, " + ex.GetType().Name);
                }				
                strLoc = "Loc_004oo";
                iCountTestcases++;
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                if(((ICollection)mycol1).IsSynchronized)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_275eg! Expected value not returned, " + ((ICollection)mycol1).IsSynchronized);
                }
                iCountTestcases++;
                strLoc = "Loc_005oo";
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                obj1 = mycol1.SyncRoot;
                mycol2 = mycol1;
                if(obj1 != mycol2.SyncRoot)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_9745sg! Expected value not returned");
                }
                strLoc = "Loc_006oo";
                iCountTestcases++;
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                mycol1 = new MyReadOnlyCollectionBase(arrF2);
                for(int i=0; i<100; i++)
                {
                    if((mycol1[i].IValue != i) || (mycol1[i].SValue!= i.ToString()) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_2974swsg_" + i + "! Expected value not returned");
                    }
                    if((mycol1.IndexOf(new Foo(i, i.ToString()))!= i) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_205sg_" + i + "! Expected value not returned");
                    }
                    if((!mycol1.Contains(new Foo(i, i.ToString()))) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_975wg_" + i + "! Expected value not returned");
                    }
                }
                strLoc = "Loc_006oo";
                iCountTestcases++;
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                mycol1 = new MyReadOnlyCollectionBase(arrF2);
                if(!mycol1.IsFixedSize)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_9753sfg! Expected value not returned, " + mycol1.IsFixedSize);
                }
                if(!mycol1.IsReadOnly)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_834sg! Expected value not returned, " + mycol1.IsReadOnly);
                }
                arrF2 = new Foo[100];
                for(int i=0; i<100; i++)
                    arrF2[i]=new Foo(i, i.ToString());
                mycol1 = new MyReadOnlyCollectionBase(arrF2);
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
        Co3997AllMethods cbA = new Co3997AllMethods();
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
public class MyReadOnlyCollectionBase:ReadOnlyCollectionBase
{
    public MyReadOnlyCollectionBase(Foo[] values)
    {
        InnerList.AddRange(values);
    }
    public Foo this[int indx]
    {
        get{return (Foo)InnerList[indx];}
    }
    public void CopyTo(Array array, Int32 index)
    {
        ((ICollection)InnerList).CopyTo(array, index);
    }
    public virtual Object SyncRoot
    {
        get {return ((ICollection)InnerList).SyncRoot;}
    }
    public Int32 IndexOf(Foo f)
    {
        return ((IList)InnerList).IndexOf(f);
    }
    public Boolean Contains(Foo f)
    {
        return ((IList)InnerList).Contains(f);
    }
    public Boolean IsFixedSize
    {
        get{return true;}
    }
    public Boolean IsReadOnly
    {
        get{return true;}
    }
}
public class Foo
{
    private Int32 iValue;
    private String strValue;
    public Foo()
    {
    }
    public Foo(Int32 i, String str)
    {
        iValue = i;
        strValue = str;
    }
    public Int32 IValue
    {
        get{return iValue;}
        set{iValue=value;}
    }
    public String SValue
    {
        get{return strValue;}
        set{strValue=value;}
    }
    public override Boolean Equals(Object obj)
    {		
        if(obj==null)
            return false;
        if(!(obj is Foo))
            return false;
        if((((Foo)obj).IValue==iValue) && (((Foo)obj).SValue==strValue))
            return true;
        return false;
    }
    public override Int32 GetHashCode()
    {
        return iValue;
    }
}
