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
using System.IO;
using System;
using System.Collections;
class Co3966WrapperTests
{
    private Boolean fVerbose = false;
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.Wrapper Tests";
    public static String s_strTFName        = "Co3935ToArray.cs";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    private Boolean runTest()
    {
        Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        ArrayList alst = null;
        ArrayList tst = null;
        IList ilst1 = null;
        IList ilst2 = null;
        String strLoc = null;
        Hashtable hsh1 = null;
        IDictionaryEnumerator idic = null;
        try
        {
            strLoc = "Loc_04872dsf";
            iCountTestcases++;
            alst = new ArrayList();
            tst = ArrayList.Adapter(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            iCountTestcases++;
            if(hsh1.Count>2 
                || ((String)hsh1["Capacity"] != "get") 
                || ((String)hsh1["TrimToSize"] != "Monekeyed, 301 301") 
                )
            {
                iCountErrors++;
                Console.WriteLine("Err_742dsf! Adapter");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<" + idic.Key + "><" + idic.Value + ">");
                }
            }
            strLoc = "Loc_210742wdsfg";
            iCountTestcases++;
            alst = new ArrayList();
            for(int i=0; i<100;i++)
                alst.Add(i);
            tst = ArrayList.Adapter(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            if(hsh1.Count>2 
                || ((String)hsh1["Capacity"] != "get") 
                || ((String)hsh1["TrimToSize"] != "Monekeyed, 301 301") 
                )
            {
                iCountErrors++;
                Console.WriteLine("Err_75629ewf! Adapter");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_6927csf";
            iCountTestcases++;
            alst = new ArrayList();
            tst = ArrayList.FixedSize(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("Fixed"))
            {
                iCountErrors++;
                Console.WriteLine("Err_0371dsf! Adapter");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_79231";
            alst = new ArrayList();
            for(int i=0; i<100;i++)
                alst.Add(i);
            tst = ArrayList.FixedSize(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            iCountTestcases++;
            if(hsh1.Count>1 || !hsh1.ContainsKey("Fixed"))
            {
                iCountErrors++;
                Console.WriteLine("Err_8427efs! Adapter");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_6792dsf";
            iCountTestcases++;
            ilst1 = new ArrayList();
            ilst2 = ArrayList.FixedSize(ilst1);
            hsh1 = new Hashtable();
            DoIListTests(ilst1, ilst2, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("Fixed"))
            {
                iCountErrors++;
                Console.WriteLine("Err_127we! FixedSize, IList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_7432adf";
            ilst1 = new ArrayList();
            for(int i=0; i<100; i++)
            {
                ilst1.Add(i);
            }
            ilst2 = ArrayList.FixedSize(ilst1);
            hsh1 = new Hashtable();
            DoIListTests(ilst1, ilst2, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("Fixed"))
            {
                iCountErrors++;
                Console.WriteLine("Err_67438dsafsf! FixedSize, IList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_0452fgf";
            iCountTestcases++;
            alst = new ArrayList();
            tst = ArrayList.ReadOnly(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            if(hsh1.Count>2 
                || !hsh1.ContainsKey("IsReadOnly")
                || ((String)hsh1["BinarySearch"] != "Exception not thrown, (Object, IComparer)")  
                )
            {
                iCountErrors++;
                Console.WriteLine("Err_752rw342! ReadOnly, ArrayList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_8342asdf";
            iCountTestcases++;
            alst = new ArrayList();
            for(int i=0; i<100;i++)
                alst.Add(i);
            tst = ArrayList.ReadOnly(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            if(hsh1.Count>2
                || !hsh1.ContainsKey("IsReadOnly")
                || ((String)hsh1["BinarySearch"] != "Exception not thrown, (Object, IComparer)")  
                )
            {
                iCountErrors++;
                Console.WriteLine("Err_03482sagfdg! ReadOnly - ArrayList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_67294sf";
            iCountTestcases++;
            ilst1 = new ArrayList();
            ilst2 = ArrayList.ReadOnly(ilst1);
            hsh1 = new Hashtable();
            DoIListTests(ilst1, ilst2, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("IsReadOnly"))
            {
                iCountErrors++;
                Console.WriteLine("Err_9723dvsf! ReadOnly, IList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_7432adf";
            iCountTestcases++;
            ilst1 = new ArrayList();
            for(int i=0; i<100; i++)
            {
                ilst1.Add(i);
            }
            ilst2 = ArrayList.ReadOnly(ilst1);
            hsh1 = new Hashtable();
            DoIListTests(ilst1, ilst2, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("IsReadOnly"))
            {
                iCountErrors++;
                Console.WriteLine("Err_97213sdfs! ReadOnly, IList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }			
            strLoc = "Loc_945wsd";
            iCountTestcases++;
            alst = new ArrayList();
            tst = ArrayList.Synchronized(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("IsSynchronized"))
            {
                iCountErrors++;
                Console.WriteLine("Err_0327sdf! Synchronized, ArrayList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_8342asdf";
            alst = new ArrayList();
            for(int i=0; i<100;i++)
                alst.Add(i);
            tst = ArrayList.Synchronized(alst);
            hsh1 = new Hashtable();
            CompareObjects(alst, tst, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("IsSynchronized"))
            {
                iCountErrors++;
                Console.WriteLine("Err_2874s! Synchronized, ArrayList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_307ens";
            iCountTestcases++;
            ilst1 = new ArrayList();
            ilst2 = ArrayList.Synchronized(ilst1);
            hsh1 = new Hashtable();
            DoIListTests(ilst1, ilst2, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("IsSynchronized"))
            {
                iCountErrors++;
                Console.WriteLine("Err_735sx! Synchronized, IList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_7432adf";
            ilst1 = new ArrayList();
            for(int i=0; i<100; i++)
            {
                ilst1.Add(i);
            }
            ilst2 = ArrayList.Synchronized(ilst1);
            hsh1 = new Hashtable();
            DoIListTests(ilst1, ilst2, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("IsSynchronized"))
            {
                iCountErrors++;
                Console.WriteLine("Err_9934sds! Synchronized, IList");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_0732fgf";
            iCountTestcases++;
            alst = new ArrayList();
            tst = alst.GetRange(0, 0);
            hsh1 = new Hashtable();
            CompareRangeObjects(alst, tst, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("TrimToSize")
                )
            {
                Console.WriteLine("Err_0784ns! Range");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_045an";
            alst = new ArrayList();
            for(int i=0; i<100;i++)
                alst.Add(i);
            tst = alst.GetRange(0, 100);
            hsh1 = new Hashtable();
            CompareRangeObjects(alst, tst, hsh1);
            if(hsh1.Count>1 || !hsh1.ContainsKey("TrimToSize")
                )
            {
                iCountErrors++;
                Console.WriteLine("Err_297cs! Range");
                idic = hsh1.GetEnumerator();
                while(idic.MoveNext())
                {
                    Console.WriteLine("<<" + idic.Key + " <<" + idic.Value + ">>");
                }
            }
            strLoc = "Loc_47yv7";
            iCountTestcases++;
            Int16 i16;
            Int32 i32;
            Int64 i64;
            UInt16 ui16;
            UInt32 ui32;
            UInt64 ui64;
            Int32 iValue;
            Boolean fPass;
            i16 = 1;
            i32 = 2;
            i64 = 3;
            ui16 = 4;
            ui32 = 5;
            ui64 = 6;
            alst = new ArrayList();
            alst.Add(i16);
            alst.Add(i32);
            alst.Add(i64);
            alst.Add(ui16);
            alst.Add(ui32);
            alst.Add(ui64);
            iCountTestcases++;
            fPass = true;
            for(int i=0; i<alst.Count; i++)
            {
                if(alst.Contains(i) && i!=2)
                    fPass = false;
            }
            if(!fPass)
            {
                iCountErrors++;
                Console.WriteLine("Err_7423dsf! Unexpected value returned");
            }
            iCountTestcases++;
            fPass = true;
            iValue = 1;
            for(int i=0; i<alst.Count; i++)
            {
                if((alst.IndexOf(i)==-1)  && (i==2))
                {
                    Console.WriteLine(iValue + " " + i);
                    fPass = false;
                }
                if((alst.IndexOf(i)!=-1)  && (i!=2))
                {
                    Console.WriteLine(iValue + " " + i);
                    fPass = false;
                }
            }
            if(!fPass)
            {
                iCountErrors++;
                Console.WriteLine("Err_7423dsf! Unexpected value returned");
            }
            iCountTestcases++;
            fPass = true;
            try
            {
                alst.Sort();
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception ex)
            {
                fPass = false;
                Console.WriteLine(ex);
            }
            if(!fPass)
            {
                iCountErrors++;
                Console.WriteLine("Err_7423dsf! Unexpected value returned");
            }
        }
        catch(Exception ex)
        {
            iCountErrors++;
            Console.WriteLine("Err_3452dfsd! Unexpected exception caught! " + strLoc + " Exception, " + ex);
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
    private void CompareObjects(ArrayList good, ArrayList bad, Hashtable hsh1)
    {
        DoIListTests(good, bad, hsh1);
        good.Clear();
        for(int i=0; i<100; i++)
            good.Add(i);
        if(fVerbose)
            Console.WriteLine("CopyTo()");
        Int32[] iArr1 = null;
        Int32[] iArr2 = null;
        iArr1 = new Int32[100];
        iArr2 = new Int32[100];
        good.CopyTo(iArr1);
        bad.CopyTo(iArr2);
        for(int i=0; i<100; i++)
        {
            if(iArr1[i] != iArr2[i])
                hsh1["CopyTo"] = "()";
        }
        iArr1 = new Int32[100];
        iArr2 = new Int32[100];
        good.CopyTo(0, iArr1, 0, 100);
        try
        {
            bad.CopyTo(0, iArr2, 0, 100);
            for(int i=0; i<100; i++)
            {
                if(iArr1[i] != iArr2[i])
                    hsh1["CopyTo"] = "()";
            }
        }
        catch
        {
            hsh1["CopyTo"] = "(Int32, Array, Int32, Int32)";
        }
        iArr1 = new Int32[200];
        iArr2 = new Int32[200];
        for(int i=0; i<200; i++)
        {
            iArr1[i]=50;
            iArr2[i]=50;
        }
        good.CopyTo(50, iArr1, 100, 20);
        try
        {
            bad.CopyTo(50, iArr2, 100, 20);
            for(int i=0; i<200; i++)
            {
                if(iArr1[i] != iArr2[i])
                    hsh1["CopyTo"] = "(Array, Int32, Int32)";
            }
        }
        catch
        {
            hsh1["CopyTo"] = "(Int32, Array, Int32, Int32)";
        }
        if(fVerbose)
            Console.WriteLine("Clone()");
        ArrayList alstClone = (ArrayList)bad.Clone();
        if(alstClone.Count != bad.Count)
            hsh1["Clone"] = "Count";
        for(int i=0; i<bad.Count; i++)
        {
            if(alstClone[i] != bad[i])
                hsh1["Clone"] = "[]";
        }
        if(fVerbose)
            Console.WriteLine("GetEnumerator()");
        IEnumerator ienm1 = null;
        IEnumerator ienm2 = null;
        ienm1 = good.GetEnumerator(0, 100);
        try
        {
            ienm2 = bad.GetEnumerator(0, 100);
            DoIEnumerableTest(ienm1, ienm2, hsh1, false);
        }
        catch
        {
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        ienm1 = good.GetEnumerator(50, 50);
        try
        {
            ienm2 = bad.GetEnumerator(50, 50);
            DoIEnumerableTest(ienm1, ienm2, hsh1, false);
        }
        catch
        {
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        try
        {
            bad.GetEnumerator(50, 150);
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        catch(Exception)
        {
        }
        ienm1 = good.GetEnumerator(0, 100);
        try
        {
            ienm2 = bad.GetEnumerator(0, 100);
            good.RemoveAt(0);
            DoIEnumerableTest(ienm1, ienm2, hsh1, true);
        }
        catch
        {
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        good.Clear();
        for(int i=0; i<100; i++)
            good.Add(i);
        if(fVerbose)
            Console.WriteLine("GetRange()");
        ArrayList alst1 = good.GetRange(0, good.Count);
        try
        {
            ArrayList alst2 = bad.GetRange(0, good.Count);
            for(int i=0; i<good.Count; i++)
            {
                if(alst1[i] != alst2[i])
                    hsh1["GetRange"] = i;
            }
        }
        catch
        {
            hsh1["Range"] = "(Int32, Int32)";
        }
        if(bad.Count>0)
        {
            if(fVerbose)
                Console.WriteLine("IndexOf()");
            for(int i=0; i<good.Count; i++)
            {
                if(good.IndexOf(good[i], 0) != bad.IndexOf(good[i], 0))
                {
                    Console.WriteLine(good .Count + " " + bad.Count + " " + good[i] + " " + bad[i] + " " + good.IndexOf(good[i], 0) + " " + bad.IndexOf(good[i], 0));
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(good.IndexOf(good[i], i) != bad.IndexOf(good[i], i))
                {
                    Console.WriteLine("2" + good.IndexOf(good[i], i) + " " + bad.IndexOf(good[i], i));
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.IndexOf(good[i], i+1) != bad.IndexOf(good[i], i+1))
                    {
                        Console.WriteLine("3" + good.IndexOf(good[i], i+1) + " " + bad.IndexOf(good[i], i+1));
                        hsh1["IndexOf"] = "(Object, Int32)";
                    }
                }			
            }
            try
            {
                bad.IndexOf(1, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            try
            {
                bad.IndexOf(1, bad.Count);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            for(int i=0; i<good.Count; i++)
            {
                if(good.IndexOf(good[i], 0, good.Count-1) != bad.IndexOf(good[i], 0, good.Count-1))
                {
                    hsh1["IndexOf"] = "(Object, Int32, Int32)";
                }
                if(good.IndexOf(good[i], i, good.Count-i) != bad.IndexOf(good[i], i, good.Count-i))
                {
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(good.IndexOf(good[i], i, 0) != bad.IndexOf(good[i], i, 0))
                {
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.IndexOf(good[i], i+1, good.Count-(i+1)) != bad.IndexOf(good[i], i+1, good.Count-(i+1)))
                    {
                        hsh1["IndexOf"] = "(Object, Int32)";
                    }
                }
            }
            try
            {
                bad.IndexOf(1, 0, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            try
            {
                bad.IndexOf(1, 0, bad.Count);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            try
            {
                bad.IndexOf(1, bad.Count-1, bad.Count-2);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            if(fVerbose)
                Console.WriteLine("LastIndexOf(), " + good.Count + " " + bad.Count);
            for(int i=0; i<good.Count; i++)
            {
                if(good.LastIndexOf(good[i]) != bad.LastIndexOf(good[i]))
                {
                    hsh1["LastIndexOf"] = "(Object)";
                }
                if(good.LastIndexOf(i+1000) != bad.LastIndexOf(i+1000))
                {
                    hsh1["LastIndexOf"] = "(Object)";
                }
            }
            try
            {
                bad.LastIndexOf(null);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            if(fVerbose)
                Console.WriteLine("LastIndexOf(Object, Int32)");
            for(int i=0; i<good.Count; i++)
            {
                if(good.LastIndexOf(good[i], good.Count-1) != bad.LastIndexOf(good[i], good.Count-1))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(good.LastIndexOf(good[i], 0) != bad.LastIndexOf(good[i], 0))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(good.LastIndexOf(good[i], i) != bad.LastIndexOf(good[i], i))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.LastIndexOf(good[i], i+1) != bad.LastIndexOf(good[i], i+1))
                    {
                        hsh1["LastIndexOf"] = "(Object, Int32)";
                    }
                }			
            }
            try
            {
                bad.LastIndexOf(1, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            try
            {
                bad.LastIndexOf(1, bad.Count);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            for(int i=0; i<good.Count; i++)
            {
                if(good.LastIndexOf(good[i], good.Count-1, 0) != bad.LastIndexOf(good[i], good.Count-1, 0))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32, Int32)";
                }
                if(good.LastIndexOf(good[i], good.Count-1, i) != bad.LastIndexOf(good[i], good.Count-1, i))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(good.LastIndexOf(good[i], i, i) != bad.LastIndexOf(good[i], i, i))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.LastIndexOf(good[i], good.Count-1, i+1) != bad.LastIndexOf(good[i], good.Count-1, i+1))
                    {
                        hsh1["LastIndexOf"] = "(Object, Int32)";
                    }
                }
            }
            try
            {
                bad.LastIndexOf(1, 1, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            try
            {
                bad.LastIndexOf(1, bad.Count-2, bad.Count-1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
        }
        if(fVerbose)
            Console.WriteLine("ReadOnly()");
        ArrayList alst3 = ArrayList.ReadOnly(bad);
        if(!alst3.IsReadOnly)
            hsh1["ReadOnly"] = "Not";
        IList ilst1 = ArrayList.ReadOnly((IList)bad);
        if(!ilst1.IsReadOnly)
            hsh1["ReadOnly"] = "Not";
        if(fVerbose)
            Console.WriteLine("Synchronized()");
        alst3 = ArrayList.Synchronized(bad);
        if(!alst3.IsSynchronized)
            hsh1["Synchronized"] = "Not";
        ilst1 = ArrayList.Synchronized((IList)bad);
        if(!ilst1.IsSynchronized)
            hsh1["Synchronized"] = "Not";
        if(good.Count == bad.Count)
        {
            if(fVerbose)
                Console.WriteLine("ToArray()");
            Object[] oArr1 = good.ToArray();
            Object[] oArr2 = bad.ToArray();
            for(int i=0; i<good.Count; i++)
            {
                if((Int32)oArr1[i] != (Int32)oArr2[i])
                    hsh1["ToArray"] = "()";
            }
            iArr1 = (Int32[])good.ToArray(typeof(Int32));
            iArr2 = (Int32[])bad.ToArray(typeof(Int32));
            for(int i=0; i<good.Count; i++)
            {
                if(iArr1[i] != iArr2[i])
                    hsh1["ToArray"] = "(Type)";
            }
        }
        if(fVerbose)
            Console.WriteLine("Capacity()");
        if(good.Capacity != bad.Capacity)
        {
            hsh1["Capacity"] = "get";
        }
        if(!hsh1.ContainsKey("IsReadOnly"))
        {
            good.Clear();
            for(int i=100; i>0; i--)
                good.Add(i);
            if(fVerbose)
                Console.WriteLine("Sort() & BinarySearch()");
            bad.Sort();
            for(int i=0; i<bad.Count-1; i++)
            {
                if((Int32)bad[i] > (Int32)bad[i+1])
                    hsh1["Sort"] = "()";
            }			
            for(int i=0; i<bad.Count; i++)
            {
                if(bad.BinarySearch(bad[i]) != i)
                    hsh1["BinarySearch"] = "(Object)";
            }
            bad.Reverse();
            if(bad.Count>0)
            {
                for(int i=0; i<99; i++)
                {
                    if((Int32)bad[i] < (Int32)bad[i+1])
                        hsh1["Reverse"] = "()";
                }
                good.Clear();
                for(int i=100; i>0; i--)
                    good.Add(i.ToString());
            }
            good.Clear();
            for(int i=90; i>64; i--)
                good.Add(((Char)i).ToString());
            try
            {
                bad.Sort(new CaseInsensitiveComparer());
                if(bad.Count>0)
                {
                    for(int i=0; i<(bad.Count-1); i++)
                    {
                        if(((String)bad[i]).CompareTo(((String)bad[i+1])) >= 0)
                            hsh1["Sort"] = "(IComparer)";
                    }			
                    for(int i=0; i<bad.Count; i++)
                    {
                        if(bad.BinarySearch(bad[i], new CaseInsensitiveComparer()) != i)
                            hsh1["BinarySearch"] = "(Object, IComparer)";
                    }
                }
                bad.Reverse();			
                good.Clear();
                for(int i=65; i<91; i++)
                    good.Add(((Char)i).ToString());
                if(bad.Count>0)
                {
                    for(int i=0; i<good.Count; i++)
                    {
                        if(bad.BinarySearch(0, bad.Count, bad[i], new CaseInsensitiveComparer()) != i)
                            hsh1["BinarySearch"] = "(Int32, Int32, Object, IComparer)";
                    }
                }
            }
            catch(Exception)
            {
            }
            good.Clear();
            for(int i=0; i<100; i++)
                good.Add(i);
            if(fVerbose)
                Console.WriteLine("SetRange()");
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            try
            {
                bad.SetRange(0, que);
            }
            catch(Exception ex)
            {
                hsh1["SetRange"] = "Copy_ExceptionType, " + ex.GetType().Name;
            }
            for(int i=bad.Count; i<bad.Count; i++)
            {
                if((Int32)bad[i] != (i + 5000))
                {
                    hsh1["SetRange"] = i;
                }
            }
        }
        else
        {
            good.Clear();
            for(int i=100; i>0; i--)
                good.Add(i);
            try
            {
                bad.Sort();
                hsh1["Sort"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
            try
            {
                bad.Reverse();
                hsh1["Reverse"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["Reverse"] = "Copy_ExceptionType, " + ex.GetType().Name;
            }
            try
            {
                bad.Sort(new CaseInsensitiveComparer());
                hsh1["Sort"] = "Copy - Icomparer";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
            try
            {
                bad.Sort(0, 0, new CaseInsensitiveComparer());
                hsh1["Sort"] = "Copy - Int32, Int32, IComparer";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
            try
            {
                for(int i=0; i<bad.Count; i++)
                {
                    if(bad.BinarySearch(bad[i]) != i)
                        hsh1["BinarySearch"] = "(Object)";
                }
                hsh1["BinarySearch"] = "(Object)";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["BinarySearch"] = ex.GetType().Name;
            }
            try
            {
                for(int i=0; i<bad.Count; i++)
                {
                    if(bad.BinarySearch(bad[i], new CaseInsensitiveComparer()) != i)
                        hsh1["BinarySearch"] = "(Object)";
                }
                hsh1["BinarySearch"] = "Exception not thrown, (Object, IComparer)";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["BinarySearch"] = ex.GetType().Name;
            }
            try
            {
                for(int i=0; i<bad.Count; i++)
                {
                    if(bad.BinarySearch(0, bad.Count, bad[i], new CaseInsensitiveComparer()) != i)
                        hsh1["BinarySearch"] = "(Object)";
                }
                hsh1["BinarySearch"] = "Exception not thrown, (Object, IComparer)";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["BinarySearch"] = ex.GetType().Name;
            }
            good.Clear();
            for(int i=0; i<100; i++)
                good.Add(i);
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            try
            {
                bad.SetRange(0, que);
                hsh1["Sort"] = "Copy - Int32, Int32, IComparer";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
        }
        if(!hsh1.ContainsKey("IsReadOnly") && !hsh1.ContainsKey("Fixed"))
        {
            good.Clear();
            for(int i=0; i<100; i++)
                good.Add(i);
            if(fVerbose)
                Console.WriteLine("InsertRange()");
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            bad.InsertRange(0, que);
            for(int i=0; i<100; i++)
            {
                if((Int32)bad[i] != i + 5000)
                {
                    hsh1["InsertRange"] = i;
                }
            }
            if(fVerbose)
                Console.WriteLine("AddRange()");
            que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+2222);
            bad.AddRange(que);
            for(int i=bad.Count-100; i<bad.Count; i++)
            {
                if((Int32)bad[i] != (i-(bad.Count-100)) + 2222)
                {
                    hsh1["AddRange"] = i + " " + (Int32)bad[i];
                }
            }
            if(fVerbose)
                Console.WriteLine("RemoveRange()");
            bad.RemoveRange(0, que.Count);
            for(int i=0; i<100; i++)
            {
                if((Int32)bad[i] != i)
                {
                    hsh1["RemoveRange"] = i + " " + (Int32)bad[i];
                }
            }
            try
            {
                bad.Capacity = bad.Capacity*2;
            }
            catch(Exception ex)
            {
                hsh1["Capacity"] = ex.GetType().Name;
            }
            try
            {
                bad.Capacity = -1;
                hsh1["Capacity"] = "No_Exception_Thrown, -1";
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["Capacity"] = ex.GetType().Name;
            }
            Int32 iMakeSureThisDoesNotCause = 0;
            while(bad.Capacity == bad.Count)
            {
                if(iMakeSureThisDoesNotCause++>100)
                    break;
                bad.Add(bad.Count);
            }
            if(iMakeSureThisDoesNotCause>100)
                hsh1["TrimToSize"] = "Monekeyed, " + bad.Count + " " + bad.Capacity;
            try
            {
                bad.TrimToSize();
                if(bad.Capacity != bad.Count)
                {
                    hsh1["TrimToSize"] = "Problems baby";
                }
            }
            catch(Exception ex)
            {
                hsh1["TrimToSize"] = ex.GetType().Name;
            }
        }
        else
        {
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            try
            {
                bad.AddRange(que);
                hsh1["AddRange"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["AddRange"] = "Copy_ExceptionType";
            }
            try
            {
                bad.InsertRange(0, que);
                hsh1["InsertRange"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["InsertRange"] = "Copy_ExceptionType";
            }
            good.Clear();
            for(int i=0; i<10; i++)
                good.Add(i);
            try
            {
                bad.RemoveRange(0, 10);
                hsh1["RemoveRange"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["RemoveRange"] = "Copy_ExceptionType, " + ex.GetType().Name;
            }
            try
            {
                bad.Capacity = bad.Capacity*2;
                hsh1["Capacity"] = "No_Exception_Thrown, bad.Capacity*2";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["Capacity"] = ex.GetType().Name;
            }
            try
            {
                bad.TrimToSize();
                hsh1["TrimToSize"] = "No_Exception_Thrown";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["TrimToSize"] = ex.GetType().Name;
            }
        }
    }
    private void DoIListTests(IList good, IList bad, Hashtable hsh1)
    {
        DoICollectionTests(good, bad, hsh1);		
        if(bad.IsReadOnly)
            hsh1["IsReadOnly"] = "yea";
        try
        {
            for(int i=0; i<good.Count; i++)
            {
                if(!bad.Contains(good[i]))
                    hsh1["Contains"] = i;
                if(bad[i] != good[i])
                    hsh1["Item"] = "get";
                if(bad.IndexOf(good[i]) != i)
                    hsh1["IndexOf"] = i;
            }				
            if(!hsh1.ContainsKey("IsReadOnly"))
            {
                for(int i=100; i<200; i++)
                {
                    try
                    {
                        good.Add(i);
                        bad.Add(i);
                    }
                    catch(Exception)
                    {
                        hsh1["Fixed"] = null;
                    }
                }
                if(!hsh1.ContainsKey("Fixed") && (bad.Count != bad.Count))
                    hsh1["Count"] =  "ReadWrite";
                if(!hsh1.ContainsKey("Fixed"))
                {
                    try
                    {
                        bad.Clear();
                    }
                    catch(Exception)
                    {
                        hsh1["Clear"] = null;
                    }
                    for(int i=0; i<100; i++)
                    {
                        bad.Insert(0, i);
                    }
                    for(int i=0; i<100; i++)
                    {
                        bad.RemoveAt(0);
                    }
                    if(bad.Count != 0)
                        hsh1["Count"] =  "Expected 0";
                    for(int i=0; i<100; i++)
                    {
                        bad.Add(i.ToString());
                    }
                    if(bad.Count != 100)
                        hsh1["Count"] =  "Expected 100, " + bad.Count;
                    if(good.Count != 100)
                        hsh1["this"] =  "Not the same objects, " + good.Count;
                    for(int i=0; i<100; i++)
                    {
                        if(!bad[i].Equals(i.ToString()))
                            hsh1["Item"] =  "String";
                    }
                    for(int i=0; i<100; i++)
                        bad.Remove(i.ToString());
                    if(bad.Count != 0)
                        hsh1["Count"] =  "Expected 0, " + bad.Count;
                    for(int i=0; i<100; i++)
                    {
                        bad.Add(i.ToString());
                    }
                    for(int i=99; i>0; i--)
                        bad[i] = i.ToString();
                    if(bad.Count != 100)
                        hsh1["Count"] =  "Expected 100, " + bad.Count;
                    for(int i=99; i>0; i--)
                    {
                        if(!bad[i].Equals(i.ToString()))
                            hsh1["Item"] =  "String";
                    }
                    bad.Clear();
                    if(bad.Count != 0)
                        hsh1["Count"] =  "Expected 0, " + bad.Count;			
                }
            }
            if(hsh1.ContainsKey("IsReadOnly") || hsh1.ContainsKey("Fixed"))
            {
                try
                {
                    good.Clear();
                }
                catch(Exception)
                {
                    hsh1["Clear"] = "original";
                }
                for(int i=0; i<100; i++)
                {
                    good.Insert(0, i);
                }
                if(bad.Count != 100)
                    hsh1["Count"] = "Not equal to original";
                for(int i=0; i<100; i++)
                {
                    good.RemoveAt(0);
                }
                if(bad.Count != 0)
                    hsh1["Count"] =  "Expected 0";
                for(int i=0; i<100; i++)
                {
                    good.Add(i.ToString());
                }
                if(bad.Count != 100)
                    hsh1["Count"] =  "Expected 100, " + bad.Count;
                if(good.Count != 100)
                    hsh1["this"] =  "Not the same objects";
                for(int i=0; i<100; i++)
                {
                    if(!bad[i].Equals(i.ToString()))
                        hsh1["Item"] =  "String";
                }
                for(int i=0; i<100; i++)
                    good.Remove(i.ToString());
                if(bad.Count != 0)
                    hsh1["Count"] =  "Expected 0, " + bad.Count;
                for(int i=0; i<100; i++)
                {
                    good.Add(i.ToString());
                }
                for(int i=99; i>0; i--)
                    good[i] = i.ToString();
                if(bad.Count != 100)
                    hsh1["Count"] =  "Expected 100, " + bad.Count;
                for(int i=99; i>0; i--)
                {
                    if(!bad[i].Equals(i.ToString()))
                        hsh1["Item"] =  "String";
                }
                good.Clear();
                if(bad.Count != 0)
                    hsh1["Count"] =  "Expected 0, " + bad.Count;
                good.Clear();
                for(int i=0; i<100; i++)
                {
                    good.Add(i);
                }
                try
                {
                    bad.Clear();
                    hsh1["Clear"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["Clear"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.Insert(0, 1);
                    hsh1["Insert"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["Insert"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.RemoveAt(0);
                    hsh1["RemoveAt"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["RemoveAt"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.Add(1);
                    hsh1["Add"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["RemoveAt"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.Remove(1);
                    hsh1["Remove"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["Remove"] = "Copy_ExceptionType";
                }
            }		
        }
        catch(Exception ex)
        {
            hsh1["DoIListTests"] = ex.GetType().Name;
        }
    }
    private void DoICollectionTests(ICollection good, ICollection bad, Hashtable hsh1)
    {
        Int32[] iArr1 = null;
        Int32[] iArr2 = null;
        if(good.Count != bad.Count)
            hsh1.Add("Count", null);
        if(good.IsSynchronized != bad.IsSynchronized)
            hsh1.Add("IsSynchronized", null);
        if(good.SyncRoot != bad.SyncRoot)
            hsh1.Add("SyncRoot", null);
        iArr1 = new Int32[good.Count];
        iArr2 = new Int32[good.Count];
        good.CopyTo(iArr1, 0);
        bad.CopyTo(iArr2, 0);
        for(int i=0; i<iArr1.Length; i++)
        {
            if(iArr1[i]!=iArr2[i])
                hsh1["CopyTo"] = "vanila";
        }		
        iArr1 = new Int32[good.Count+5];
        iArr2 = new Int32[good.Count+5];
        good.CopyTo(iArr1, 5);
        bad.CopyTo(iArr2, 5);
        for(int i=5; i<iArr1.Length; i++)
        {
            if(iArr1[i]!=iArr2[i])
                hsh1["CopyTo"] = "5";
        }
        DoIEnumerableTest(good.GetEnumerator(), bad.GetEnumerator(), hsh1, false);		
        IEnumerator ienm1 = good.GetEnumerator();
        IEnumerator ienm2 = bad.GetEnumerator();
        if(good.Count>0)
        {
            ((IList)good).RemoveAt(0);
            DoIEnumerableTest(ienm1, ienm2, hsh1, true);		
        }
    }
    private void DoIEnumerableTest(IEnumerator ienm1, IEnumerator ienm2, Hashtable hsh1, Boolean fExpectToThrow)
    {
        if(!fExpectToThrow)
        {
            while(ienm1.MoveNext())
            {
                Boolean bb = ienm2.MoveNext();
                if(ienm1.Current != ienm2.Current)
                    hsh1["Enumerator"] = "Current";
            }
            ienm1.Reset();
            ienm2.Reset();
            while(ienm1.MoveNext())
            {
                ienm2.MoveNext();
                if(ienm1.Current != ienm2.Current)
                    hsh1["Enumerator"] = "Reset";
            }
            ienm1.Reset();
            ienm2.Reset();
            IEnumerator ienm1Clone = null;
            IEnumerator ienm2Clone = null;
            Boolean fPastClone = true;
            try
            {
                ienm1Clone = (IEnumerator)((ICloneable)ienm1).Clone();
            }
            catch(Exception ex)
            {
                fPastClone = false;
                Console.WriteLine("LOOK_HERE! Enumerator clone " + ex.GetType().Name);
            }
            try
            {
                ienm2Clone = (IEnumerator)((ICloneable)ienm2).Clone();
            }
            catch(Exception ex)
            {
                fPastClone = false;
                Console.WriteLine("LOOK_HERE! Enumerator clone " + ex.GetType().Name);
            }
            if(fPastClone)
            {
                while(ienm1Clone.MoveNext())
                {
                    Boolean bb = ienm2Clone.MoveNext();
                    if(ienm1Clone.Current != ienm2Clone.Current)
                        hsh1["Enumerator"] = "Current";
                }
                ienm1Clone.Reset();
                ienm2Clone.Reset();
                while(ienm1Clone.MoveNext())
                {
                    ienm2Clone.MoveNext();
                    if(ienm1Clone.Current != ienm2Clone.Current)
                        hsh1["Enumerator"] = "Reset";
                }
            }
        }
        else
        {
            try
            {
                ienm2.MoveNext();
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                hsh1["Enumerator"] = "MoveNext";
            }
            try
            {
                ienm2.Reset();
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                hsh1["Enumerator"] = "Reset";
            }
        }
    }
    private void CompareRangeObjects(ArrayList good, ArrayList bad, Hashtable hsh1)
    {
        DoRangeIListTests(good, bad, hsh1);
        bad.Clear();
        for(int i=0; i<100; i++)
            bad.Add(i);
        if(fVerbose)
            Console.WriteLine("CopyTo()");
        Int32[] iArr1 = null;
        Int32[] iArr2 = null;
        Int32 goodCount = good.Count;
        iArr1 = new Int32[goodCount];
        iArr2 = new Int32[goodCount];
        good.CopyTo(iArr1);
        bad.CopyTo(iArr2);
        for(int i=0; i<goodCount; i++)
        {
            if(iArr1[i] != iArr2[i])
                hsh1["CopyTo"] = "()";
        }
        iArr1 = new Int32[goodCount];
        iArr2 = new Int32[goodCount];
        good.CopyTo(0, iArr1, 0, goodCount);
        try
        {
            bad.CopyTo(0, iArr2, 0, goodCount);
            for(int i=0; i<goodCount; i++)
            {
                if(iArr1[i] != iArr2[i])
                    hsh1["CopyTo"] = "()";
            }
        }
        catch
        {
            hsh1["CopyTo"] = "(Int32, Array, Int32, Int32)";
        }
        iArr1 = new Int32[200];
        iArr2 = new Int32[200];
        for(int i=0; i<200; i++)
        {
            iArr1[i]=50;
            iArr2[i]=50;
        }
        good.CopyTo(50, iArr1, 100, 20);
        try
        {
            bad.CopyTo(50, iArr2, 100, 20);
            for(int i=0; i<200; i++)
            {
                if(iArr1[i] != iArr2[i])
                    hsh1["CopyTo"] = "(Array, Int32, Int32)";
            }
        }
        catch
        {
            hsh1["CopyTo"] = "(Int32, Array, Int32, Int32)";
        }
        if(fVerbose)
            Console.WriteLine("Clone()");
        ArrayList alstClone = (ArrayList)bad.Clone();
        if(alstClone.Count != bad.Count)
            hsh1["Clone"] = "Count";
        for(int i=0; i<bad.Count; i++)
        {
            if(alstClone[i] != bad[i])
                hsh1["Clone"] = "[]";
        }
        if(fVerbose)
            Console.WriteLine("GetEnumerator()");
        IEnumerator ienm1 = null;
        IEnumerator ienm2 = null;
        ienm1 = good.GetEnumerator(0, 100);
        try
        {
            ienm2 = bad.GetEnumerator(0, 100);
            DoIEnumerableTest(ienm1, ienm2, hsh1, false);
        }
        catch
        {
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        ienm1 = good.GetEnumerator(50, 50);
        try
        {
            ienm2 = bad.GetEnumerator(50, 50);
            DoIEnumerableTest(ienm1, ienm2, hsh1, false);
        }
        catch
        {
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        try
        {
            bad.GetEnumerator(50, 150);
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        catch(Exception)
        {
        }
        ienm1 = good.GetEnumerator(0, 100);
        try
        {
            ienm2 = bad.GetEnumerator(0, 100);
            bad.RemoveAt(0);
            DoIEnumerableTest(ienm1, ienm2, hsh1, true);
        }
        catch
        {
            hsh1["GetEnumerator"] = "(Int32, Int32)";
        }
        bad.Clear();
        for(int i=0; i<100; i++)
            bad.Add(i);
        if(fVerbose)
            Console.WriteLine("GetRange()");
        ArrayList alst1 = good.GetRange(0, good.Count);
        try
        {
            ArrayList alst2 = bad.GetRange(0, good.Count);
            for(int i=0; i<good.Count; i++)
            {
                if(alst1[i] != alst2[i])
                    hsh1["GetRange"] = i;
            }
        }
        catch
        {
            hsh1["Range"] = "(Int32, Int32)";
        }
        if(bad.Count>0)
        {
            if(fVerbose)
                Console.WriteLine("IndexOf()");
            for(int i=0; i<good.Count; i++)
            {
                if(good.IndexOf(good[i], 0) != bad.IndexOf(good[i], 0))
                {
                    Console.WriteLine(good .Count + " " + bad.Count + " " + good[i] + " " + bad[i] + " " + good.IndexOf(good[i], 0) + " " + bad.IndexOf(good[i], 0));
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(good.IndexOf(good[i], i) != bad.IndexOf(good[i], i))
                {
                    Console.WriteLine("2" + good.IndexOf(good[i], i) + " " + bad.IndexOf(good[i], i));
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.IndexOf(good[i], i+1) != bad.IndexOf(good[i], i+1))
                    {
                        Console.WriteLine("3" + good.IndexOf(good[i], i+1) + " " + bad.IndexOf(good[i], i+1));
                        hsh1["IndexOf"] = "(Object, Int32)";
                    }
                }			
            }
            try
            {
                bad.IndexOf(1, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            try
            {
                bad.IndexOf(1, bad.Count);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            for(int i=0; i<good.Count; i++)
            {
                if(good.IndexOf(good[i], 0, good.Count-1) != bad.IndexOf(good[i], 0, good.Count-1))
                {
                    hsh1["IndexOf"] = "(Object, Int32, Int32)";
                }
                if(good.IndexOf(good[i], i, good.Count-i) != bad.IndexOf(good[i], i, good.Count-i))
                {
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(good.IndexOf(good[i], i, 0) != bad.IndexOf(good[i], i, 0))
                {
                    hsh1["IndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.IndexOf(good[i], i+1, good.Count-(i+1)) != bad.IndexOf(good[i], i+1, good.Count-(i+1)))
                    {
                        hsh1["IndexOf"] = "(Object, Int32)";
                    }
                }
            }
            try
            {
                bad.IndexOf(1, 0, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            try
            {
                bad.IndexOf(1, 0, bad.Count);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            try
            {
                bad.IndexOf(1, bad.Count-1, bad.Count-2);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["IndexOf"] = ex;
            }
            if(fVerbose)
                Console.WriteLine("LastIndexOf(), " + good.Count + " " + bad.Count);
            for(int i=0; i<good.Count; i++)
            {
                if(good.LastIndexOf(good[i]) != bad.LastIndexOf(good[i]))
                {
                    hsh1["LastIndexOf"] = "(Object)";
                }
                if(good.LastIndexOf(i+1000) != bad.LastIndexOf(i+1000))
                {
                    hsh1["LastIndexOf"] = "(Object)";
                }
            }
            try
            {
                bad.LastIndexOf(null);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            if(fVerbose)
                Console.WriteLine("LastIndexOf(Object, Int32)");
            for(int i=0; i<good.Count; i++)
            {
                if(good.LastIndexOf(good[i], good.Count-1) != bad.LastIndexOf(good[i], good.Count-1))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(good.LastIndexOf(good[i], 0) != bad.LastIndexOf(good[i], 0))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(good.LastIndexOf(good[i], i) != bad.LastIndexOf(good[i], i))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.LastIndexOf(good[i], i+1) != bad.LastIndexOf(good[i], i+1))
                    {
                        hsh1["LastIndexOf"] = "(Object, Int32)";
                    }
                }			
            }
            try
            {
                bad.LastIndexOf(1, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            try
            {
                bad.LastIndexOf(1, bad.Count);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            for(int i=0; i<good.Count; i++)
            {
                if(good.LastIndexOf(good[i], good.Count-1, 0) != bad.LastIndexOf(good[i], good.Count-1, 0))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32, Int32)";
                }
                if(good.LastIndexOf(good[i], good.Count-1, i) != bad.LastIndexOf(good[i], good.Count-1, i))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(good.LastIndexOf(good[i], i, i) != bad.LastIndexOf(good[i], i, i))
                {
                    hsh1["LastIndexOf"] = "(Object, Int32)";
                }
                if(i<(good.Count-1))
                {
                    if(good.LastIndexOf(good[i], good.Count-1, i+1) != bad.LastIndexOf(good[i], good.Count-1, i+1))
                    {
                        hsh1["LastIndexOf"] = "(Object, Int32)";
                    }
                }
            }
            try
            {
                bad.LastIndexOf(1, 1, -1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
            try
            {
                bad.LastIndexOf(1, bad.Count-2, bad.Count-1);
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["LastIndexOf"] = ex;
            }
        }
        if(fVerbose)
            Console.WriteLine("ReadOnly()");
        ArrayList alst3 = ArrayList.ReadOnly(bad);
        if(!alst3.IsReadOnly)
            hsh1["ReadOnly"] = "Not";
        IList ilst1 = ArrayList.ReadOnly((IList)bad);
        if(!ilst1.IsReadOnly)
            hsh1["ReadOnly"] = "Not";
        if(fVerbose)
            Console.WriteLine("Synchronized()");
        alst3 = ArrayList.Synchronized(bad);
        if(!alst3.IsSynchronized)
            hsh1["Synchronized"] = "Not";
        ilst1 = ArrayList.Synchronized((IList)bad);
        if(!ilst1.IsSynchronized)
            hsh1["Synchronized"] = "Not";
        if(good.Count == bad.Count)
        {
            if(fVerbose)
                Console.WriteLine("ToArray()");
            Object[] oArr1 = good.ToArray();
            Object[] oArr2 = bad.ToArray();
            for(int i=0; i<good.Count; i++)
            {
                if((Int32)oArr1[i] != (Int32)oArr2[i])
                    hsh1["ToArray"] = "()";
            }
            iArr1 = (Int32[])good.ToArray(typeof(Int32));
            iArr2 = (Int32[])bad.ToArray(typeof(Int32));
            for(int i=0; i<good.Count; i++)
            {
                if(iArr1[i] != iArr2[i])
                    hsh1["ToArray"] = "(Type)";
            }
        }
        if(fVerbose)
            Console.WriteLine("Capacity()");
        if(good.Capacity != bad.Capacity)
        {
            hsh1["Capacity"] = "get";
        }
        if(!hsh1.ContainsKey("IsReadOnly"))
        {
            bad.Clear();
            for(int i=100; i>0; i--)
                bad.Add(i);
            if(fVerbose)
                Console.WriteLine("Sort() & BinarySearch()");
            bad.Sort();
            for(int i=0; i<bad.Count-1; i++)
            {
                if((Int32)bad[i] > (Int32)bad[i+1])
                    hsh1["Sort"] = "()";
            }			
            for(int i=0; i<bad.Count; i++)
            {
                if(bad.BinarySearch(bad[i]) != i)
                    hsh1["BinarySearch"] = "(Object)";
            }
            bad.Reverse();
            if(bad.Count>0)
            {
                for(int i=0; i<99; i++)
                {
                    if((Int32)bad[i] < (Int32)bad[i+1])
                        hsh1["Reverse"] = "()";
                }
                bad.Clear();
                for(int i=100; i>0; i--)
                    bad.Add(i.ToString());
            }
            bad.Clear();
            for(int i=90; i>64; i--)
                bad.Add(((Char)i).ToString());
            try
            {
                bad.Sort(new CaseInsensitiveComparer());
                if(bad.Count>0)
                {
                    for(int i=0; i<(bad.Count-1); i++)
                    {
                        if(((String)bad[i]).CompareTo(((String)bad[i+1])) >= 0)
                            hsh1["Sort"] = "(IComparer)";
                    }			
                    for(int i=0; i<bad.Count; i++)
                    {
                        if(bad.BinarySearch(bad[i], new CaseInsensitiveComparer()) != i)
                            hsh1["BinarySearch"] = "(Object, IComparer)";
                    }
                }
                bad.Reverse();			
                bad.Clear();
                for(int i=65; i<91; i++)
                    bad.Add(((Char)i).ToString());
                if(bad.Count>0)
                {
                    for(int i=0; i<good.Count; i++)
                    {
                        if(bad.BinarySearch(0, bad.Count, bad[i], new CaseInsensitiveComparer()) != i)
                            hsh1["BinarySearch"] = "(Int32, Int32, Object, IComparer)";
                    }
                }
            }
            catch(Exception)
            {
            }
            bad.Clear();
            for(int i=0; i<100; i++)
                bad.Add(i);
            if(fVerbose)
                Console.WriteLine("SetRange()");
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            try
            {
                bad.SetRange(0, que);
            }
            catch(Exception ex)
            {
                hsh1["SetRange"] = "Copy_ExceptionType, " + ex.GetType().Name;
            }
            for(int i=bad.Count; i<bad.Count; i++)
            {
                if((Int32)bad[i] != (i + 5000))
                {
                    hsh1["SetRange"] = i;
                }
            }
        }
        else
        {
            bad.Clear();
            for(int i=100; i>0; i--)
                bad.Add(i);
            try
            {
                bad.Sort();
                hsh1["Sort"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
            try
            {
                bad.Reverse();
                hsh1["Reverse"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["Reverse"] = "Copy_ExceptionType, " + ex.GetType().Name;
            }
            try
            {
                bad.Sort(new CaseInsensitiveComparer());
                hsh1["Sort"] = "Copy - Icomparer";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
            try
            {
                bad.Sort(0, 0, new CaseInsensitiveComparer());
                hsh1["Sort"] = "Copy - Int32, Int32, IComparer";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
            try
            {
                for(int i=0; i<bad.Count; i++)
                {
                    if(bad.BinarySearch(bad[i]) != i)
                        hsh1["BinarySearch"] = "(Object)";
                }
                hsh1["BinarySearch"] = "(Object)";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["BinarySearch"] = ex.GetType().Name;
            }
            try
            {
                for(int i=0; i<bad.Count; i++)
                {
                    if(bad.BinarySearch(bad[i], new CaseInsensitiveComparer()) != i)
                        hsh1["BinarySearch"] = "(Object)";
                }
                hsh1["BinarySearch"] = "Exception not thrown, (Object, IComparer)";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["BinarySearch"] = ex.GetType().Name;
            }
            try
            {
                for(int i=0; i<bad.Count; i++)
                {
                    if(bad.BinarySearch(0, bad.Count, bad[i], new CaseInsensitiveComparer()) != i)
                        hsh1["BinarySearch"] = "(Object)";
                }
                hsh1["BinarySearch"] = "Exception not thrown, (Object, IComparer)";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["BinarySearch"] = ex.GetType().Name;
            }
            bad.Clear();
            for(int i=0; i<100; i++)
                bad.Add(i);
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            try
            {
                bad.SetRange(0, que);
                hsh1["Sort"] = "Copy - Int32, Int32, IComparer";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["Sort"] = "Copy_ExceptionType";
            }
        }
        if(!hsh1.ContainsKey("IsReadOnly") && !hsh1.ContainsKey("Fixed"))
        {
            bad.Clear();
            for(int i=0; i<100; i++)
                bad.Add(i);
            if(fVerbose)
                Console.WriteLine("InsertRange()");
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            bad.InsertRange(0, que);
            for(int i=0; i<100; i++)
            {
                if((Int32)bad[i] != i + 5000)
                {
                    hsh1["InsertRange"] = i;
                }
            }
            if(fVerbose)
                Console.WriteLine("AddRange()");
            que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+2222);
            bad.AddRange(que);
            for(int i=bad.Count-100; i<bad.Count; i++)
            {
                if((Int32)bad[i] != (i-(bad.Count-100)) + 2222)
                {
                    hsh1["AddRange"] = i + " " + (Int32)bad[i];
                }
            }
            if(fVerbose)
                Console.WriteLine("RemoveRange()");
            bad.RemoveRange(0, que.Count);
            for(int i=0; i<100; i++)
            {
                if((Int32)bad[i] != i)
                {
                    hsh1["RemoveRange"] = i + " " + (Int32)bad[i];
                }
            }
            try
            {
                bad.Capacity = bad.Capacity*2;
            }
            catch(Exception ex)
            {
                hsh1["Capacity"] = ex.GetType().Name;
            }
            try
            {
                bad.Capacity = -1;
                hsh1["Capacity"] = "No_Exception_Thrown, -1";
            }
            catch(ArgumentException)
            {
            }
            catch(Exception ex)
            {
                hsh1["Capacity"] = ex.GetType().Name;
            }
            Int32 iMakeSureThisDoesNotCause = 0;
            while(bad.Capacity == bad.Count)
            {
                if(iMakeSureThisDoesNotCause++>100)
                    break;
                bad.Add(bad.Count);
            }
            if(iMakeSureThisDoesNotCause>100)
                hsh1["TrimToSize"] = "Monekeyed, " + bad.Count + " " + bad.Capacity;
            try
            {
                bad.TrimToSize();
                if(bad.Capacity != bad.Count)
                {
                    hsh1["TrimToSize"] = "Problems baby";
                }
            }
            catch(Exception ex)
            {
                hsh1["TrimToSize"] = ex.GetType().Name;
            }
        }
        else
        {
            Queue que = new Queue();
            for(int i=0; i<100; i++)
                que.Enqueue(i+5000);
            try
            {
                bad.AddRange(que);
                hsh1["AddRange"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["AddRange"] = "Copy_ExceptionType";
            }
            try
            {
                bad.InsertRange(0, que);
                hsh1["InsertRange"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception)
            {
                hsh1["InsertRange"] = "Copy_ExceptionType";
            }
            bad.Clear();
            for(int i=0; i<10; i++)
                bad.Add(i);
            try
            {
                bad.RemoveRange(0, 10);
                hsh1["RemoveRange"] = "Copy";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["RemoveRange"] = "Copy_ExceptionType, " + ex.GetType().Name;
            }
            try
            {
                bad.Capacity = bad.Capacity*2;
                hsh1["Capacity"] = "No_Exception_Thrown, bad.Capacity*2";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["Capacity"] = ex.GetType().Name;
            }
            try
            {
                bad.TrimToSize();
                hsh1["TrimToSize"] = "No_Exception_Thrown";
            }
            catch(NotSupportedException)
            {
            }
            catch(Exception ex)
            {
                hsh1["TrimToSize"] = ex.GetType().Name;
            }
        }							
    }
    private void DoRangeIListTests(IList good, IList bad, Hashtable hsh1)
    {
        DoRangeICollectionTests(good, bad, hsh1);		
        if(bad.IsReadOnly)
            hsh1["IsReadOnly"] = "yea";
        try
        {
            for(int i=0; i<good.Count; i++)
            {
                if(!bad.Contains(good[i]))
                    hsh1["Contains"] = i;
                if(bad[i] != good[i])
                    hsh1["Item"] = "get";
                if(bad.IndexOf(good[i]) != i)
                    hsh1["IndexOf"] = i;
            }				
            if(hsh1.ContainsKey("IsReadOnly") || hsh1.ContainsKey("Fixed"))
            {
                try
                {
                    good.Clear();
                }
                catch(Exception)
                {
                    hsh1["Clear"] = "original";
                }
                for(int i=0; i<100; i++)
                {
                    good.Insert(0, i);
                }
                if(bad.Count != 100)
                    hsh1["Count"] = "Not equal to original";
                for(int i=0; i<100; i++)
                {
                    good.RemoveAt(0);
                }
                if(bad.Count != 0)
                    hsh1["Count"] =  "Expected 0";
                for(int i=0; i<100; i++)
                {
                    good.Add(i.ToString());
                }
                if(bad.Count != 100)
                    hsh1["Count"] =  "Expected 100, " + bad.Count;
                if(good.Count != 100)
                    hsh1["this"] =  "Not the same objects";
                for(int i=0; i<100; i++)
                {
                    if(!bad[i].Equals(i.ToString()))
                        hsh1["Item"] =  "String";
                }
                for(int i=0; i<100; i++)
                    good.Remove(i.ToString());
                if(bad.Count != 0)
                    hsh1["Count"] =  "Expected 0, " + bad.Count;
                for(int i=0; i<100; i++)
                {
                    good.Add(i.ToString());
                }
                for(int i=99; i>0; i--)
                    good[i] = i.ToString();
                if(bad.Count != 100)
                    hsh1["Count"] =  "Expected 100, " + bad.Count;
                for(int i=99; i>0; i--)
                {
                    if(!bad[i].Equals(i.ToString()))
                        hsh1["Item"] =  "String";
                }
                good.Clear();
                if(bad.Count != 0)
                    hsh1["Count"] =  "Expected 0, " + bad.Count;
                good.Clear();
                for(int i=0; i<100; i++)
                {
                    good.Add(i);
                }
                try
                {
                    bad.Clear();
                    hsh1["Clear"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["Clear"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.Insert(0, 1);
                    hsh1["Insert"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["Insert"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.RemoveAt(0);
                    hsh1["RemoveAt"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["RemoveAt"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.Add(1);
                    hsh1["Add"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["RemoveAt"] = "Copy_ExceptionType";
                }
                try
                {
                    bad.Remove(1);
                    hsh1["Remove"] = "Copy";
                }
                catch(NotSupportedException)
                {
                }
                catch(Exception)
                {
                    hsh1["Remove"] = "Copy_ExceptionType";
                }
            }		
        }
        catch(Exception ex)
        {
            hsh1["DoIListTests"] = ex.GetType().Name;
        }
    }
    private void DoRangeICollectionTests(ICollection good, ICollection bad, Hashtable hsh1)
    {
        Int32[] iArr1 = null;
        Int32[] iArr2 = null;
        if(good.Count != bad.Count)
            hsh1.Add("Count", null);
        if(good.IsSynchronized != bad.IsSynchronized)
            hsh1.Add("IsSynchronized", null);
        if(good.SyncRoot != bad.SyncRoot)
            hsh1.Add("SyncRoot", null);
        iArr1 = new Int32[good.Count];
        iArr2 = new Int32[good.Count];
        good.CopyTo(iArr1, 0);
        bad.CopyTo(iArr2, 0);
        for(int i=0; i<iArr1.Length; i++)
        {
            if(iArr1[i]!=iArr2[i])
                hsh1["CopyTo"] = "vanila";
        }		
        iArr1 = new Int32[good.Count+5];
        iArr2 = new Int32[good.Count+5];
        good.CopyTo(iArr1, 5);
        bad.CopyTo(iArr2, 5);
        for(int i=5; i<iArr1.Length; i++)
        {
            if(iArr1[i]!=iArr2[i])
                hsh1["CopyTo"] = "5";
        }
        DoIEnumerableTest(good.GetEnumerator(), bad.GetEnumerator(), hsh1, false);		
    }
    public static void Main()
    {
        Boolean bResult = new Co3966WrapperTests().runTest();
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
