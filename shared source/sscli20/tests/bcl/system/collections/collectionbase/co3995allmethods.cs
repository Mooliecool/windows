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
public class Co3995AllMethods
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "CollectionBase.AllMethods";
    public static String s_strTFName        = "Co3995AllMethods.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        MyCollectionBase mycol1;
        MyCollectionBase mycol2;
        Foo f1;
        Foo[] arrF1;
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
                mycol1 = new MyCollectionBase();
                if(mycol1.Count!=0)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_234dnvf! Expected value not returned, " + mycol1.Count);
                }
                for(int i=0; i<100; i++)
                    mycol1.Add(new Foo());
                if(mycol1.Count!=100)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_2075sg! Expected value not returned, " + mycol1.Count);
                }
                iCountTestcases++;
                strLoc = "Loc_002oo";
                mycol1 = new MyCollectionBase();
                for(int i=0; i<100; i++)
                    mycol1.Add(new Foo(i, i.ToString()));
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
                mycol1 = new MyCollectionBase();
                for(int i=0; i<100; i++)
                    mycol1.Add(new Foo(i, i.ToString()));
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
                iCountTestcases++;
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
                mycol1 = new MyCollectionBase();
                for(int i=0; i<100; i++)
                    mycol1.Add(new Foo(i, i.ToString()));
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
                mycol1 = new MyCollectionBase();
                if(((ICollection)mycol1).IsSynchronized)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_275eg! Expected value not returned, " + ((ICollection)mycol1).IsSynchronized);
                }
                iCountTestcases++;
                strLoc = "Loc_005oo";
                mycol1 = new MyCollectionBase();
                obj1 = mycol1.SyncRoot;
                mycol2 = mycol1;
                if(obj1 != mycol2.SyncRoot)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_9745sg! Expected value not returned");
                }
                strLoc = "Loc_006oo";
                iCountTestcases++;
                mycol1 = new MyCollectionBase();
                for(int i=0; i<100; i++)
                    mycol1.Add(new Foo(i, i.ToString()));
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
                }
                for(int i=99; i>=0; i--)
                {
                    mycol1.RemoveAt(0);
                    if((mycol1.Count != (i)) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_2975sg_" + i + "! Expected value not returned");
                    }
                    if(i>=50)
                    {
                        if((!mycol1.Contains(new Foo(i, i.ToString()))) )
                        {
                            iCountErrors++;
                            Console.WriteLine("Err_975wg_" + i + "! Expected value not returned");
                        }
                    }
                    else
                    {
                        if((mycol1.Contains(new Foo(i, i.ToString()))) )
                        {
                            iCountErrors++;
                            Console.WriteLine("Err_975wg_" + i + "! Expected value not returned");
                        }
                    }
                }				
                strLoc = "Loc_006oo";
                iCountTestcases++;
                mycol1 = new MyCollectionBase();
                if(((IList)mycol1).IsFixedSize)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_9753sfg! Expected value not returned, " + ((IList)mycol1).IsFixedSize);
                }
                if(((IList)mycol1).IsReadOnly)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_834sg! Expected value not returned, " + ((IList)mycol1).IsReadOnly);
                }
                for(int i=0; i<100; i++)
                {
                    mycol1.Insert(i, new Foo(i, i.ToString()));
                    if(!(mycol1.Contains(new Foo(i, i.ToString()))) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_753sg_" + i + "! Expected value not returned");
                    }
                }
                for(int i=0; i<100; i++)
                {
                    mycol1.Remove(new Foo(i, i.ToString()));
                    if((mycol1.Contains(new Foo(i, i.ToString()))) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_2085sg_" + i + "! Expected value not returned");
                    }
                }
                mycol1 = new MyCollectionBase();
                for(int i=0; i<100; i++)
                    mycol1.Add(new Foo(i, i.ToString()));
                mycol1.Clear();
                if(mycol1.Count!=0)
                {
                    iCountErrors++;
                    Console.WriteLine("Err_847tfgdg! Expected value not returned, " + mycol1.Count);
                }
                mycol1 = new MyCollectionBase();
                for(int i=0; i<100; i++)
                    mycol1.Add(new Foo(i, i.ToString()));
                for(int i=0, j=100; i<100; i++, j--)
                    mycol1[i] = new Foo(j, j.ToString());
                for(int i=0, j=100; i<100; i++, j--)
                {
                    if((mycol1.IndexOf(new Foo(j, j.ToString()))!= i) )
                    {
                        iCountErrors++;
                        Console.WriteLine("Err_7342rfg_" + i + "! Expected value not returned");
                    }
                }
                mycol1 = new MyCollectionBase();
                mycol1.Add(new Foo());
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
        Co3995AllMethods cbA = new Co3995AllMethods();
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
public class MyCollectionBase:CollectionBase
{
    public int Add(Foo f1)
    {
        return List.Add(f1);
    }
    public Foo this[int indx]
    {
        get{return (Foo)List[indx];}
        set{List[indx] = value;}
    }
    public void CopyTo(Array array, Int32 index)
    {
        ((ICollection)List).CopyTo(array, index);
    }
    public virtual Object SyncRoot
    {
        get {return ((ICollection)List).SyncRoot;}
    }
    public Int32 IndexOf(Foo f)
    {
        return ((IList)List).IndexOf(f);
    }
    public Boolean Contains(Foo f)
    {
        return ((IList)List).Contains(f);
    }
    public void Insert(Int32 index, Foo f)
    {
        ((IList)List).Insert(index, f);
    }
    public void Remove(Foo f)
    {
        ((IList)List).Remove(f);
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
