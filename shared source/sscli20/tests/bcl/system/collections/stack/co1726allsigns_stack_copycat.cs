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
using System.Text;
using System.Reflection;
interface IDescribeTestedMethods
{
    MemberInfo[] GetTestedMethods();
}
class Co1726allSigns_Stack_Copycat:IDescribeTestedMethods
{
    public const String s_strActiveBugNums = "";
    public const String s_strDtTmVer       = "";
    public const String s_strClassMethod   = "allSigns";
    public const String s_strTFName        = "Co1726allSigns_Stack_Copycat.cs";
    public const String s_strTFAbbrev      = "Co1726";
    public const String s_strTFPath        = "";
    public Boolean verbose                 = false;													
    public int m_iTickCount;				
    public int m_iRandSeed;					
    public MemberInfo[] GetTestedMethods()
    {
        Type type = typeof(Stack);
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
    public static String[] s_strMethodsCovered = new String []
    {
        "Method_Covered:  Stack( )"
        ,"Method_Covered:  Stack( ICollection )"
        ,"Method_Covered:  Count_get"
        ,"Method_Covered:  GetEnumerator( )"
        ,"Method_Covered:  CopyTo( Array, Int32)"
        ,"Method_Covered:  IsSynchronized_get"
        ,"Method_Covered:  SyncRoot_get"
        ,"Method_Covered:  Clear( )"
        ,"Method_Covered:  Peek ( )"
        ,"Method_Covered:  Push ( Object )"
        ,"Method_Covered:  Pop ( )"
    };
    public static void printoutCoveredMethods()
    {
        Console.Error.WriteLine( "" );
        Console.Error.WriteLine( "Method_Count==11  ("+ s_strMethodsCovered.Length.ToString() +"==confirm) !!" );
        Console.Error.WriteLine( "" );
        for ( int ia = 0 ;ia < s_strMethodsCovered.Length ;ia++ )
        {
            Console.Error.WriteLine( s_strMethodsCovered[ia] );
        }
        Console.Error.WriteLine( "" );
    }
    public Boolean runTest()
    {
        Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
        String strLoc="Loc_000ooo";
        Stack stack         = null;
        Object [] objArr    = null;
        Object objRet = null;                         
        int iCountErrors    = 0;                      
        int iCountTestcases = 0;                      
        Stack q1;
        Stack q2;
        Stack q3;
        Boolean fPass;
        String[] strArr;
        Object[] oArr;
        IEnumerator ienm1;
        Object oValue;
        Int32 iCount;
        IEnumerator ienm2;
        Hashtable hsh1;
        Hashtable hsh2;
        Int32 iQueCount;
        try
        {
            strLoc = "Loc_Tst0001";
            if ( verbose ) Console.WriteLine( "try to make an empty stack and make sure counter is 0" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                if ( stack.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001a,  Count on newly created q1 was not 0 but " + stack.Count.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0002";
            if ( verbose ) Console.WriteLine( "test to make sure we can create a stack and by default it is not synchronized" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                if ( stack.IsSynchronized )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_002a,  Newly created q1 should not be synchronized" );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_002b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0003";
            if ( verbose ) Console.WriteLine( "test to make sure we push elements to stack and we get respective count back" );
            try
            {
                ++iCountTestcases;
                int iNumElementsAdded = 1975;
                stack = new Stack();
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    stack.Push( new Object() );
                }
                if ( stack.Count != iNumElementsAdded )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_003a,  Counter should be " + iNumElementsAdded.ToString() + " but is " + stack.Count.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_003b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0004";
            if ( verbose ) Console.WriteLine( "add elements then clear to make sure counter gets back to 0" );
            try
            {
                ++iCountTestcases;
                int iNumElementsAdded = 2;
                stack = new Stack();
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    stack.Push( new Object() );
                }
                stack.Clear();
                if ( stack.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_004a,  Counter should be " + 0.ToString() + " but is " + stack.Count.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_004b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0005";
            if ( verbose ) Console.WriteLine( "make sure pop decrements counter" );
            try
            {
                ++iCountTestcases;
                int iNumElementsAdded = 25;
                stack = new Stack();
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    stack.Push( i );
                }
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    if ( stack.Count != iNumElementsAdded - i )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_005a,  Counter should be " + (iNumElementsAdded - i).ToString() + " but is " + stack.Count.ToString() );
                    }
                    Object objTop = stack.Pop();
                    if ( ! objTop.Equals( iNumElementsAdded - i - 1 ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_005c,  Popped element is not what it is supposed to be, expected " + i.ToString() + " but got " + objTop.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_005b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0006";
            if ( verbose ) Console.WriteLine( "try peek on an empty stack" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                objRet = stack.Peek();
                ++iCountErrors;
                Console.WriteLine( "Err_006a,  Expected InvalidOperationException to be thrown but it was not" );
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_006b,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0007";
            if ( verbose ) Console.WriteLine( "try peek on an empty stack after elements have been removed from it" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                for ( int i = 0; i < 1000; i++ )
                {
                    stack.Push( i );
                }
                for ( int i = 0; i < 1000; i++ )
                {
                    objRet = stack.Pop();
                }
                objRet = stack.Peek();
                ++iCountErrors;
                Console.WriteLine( "Err_007a,  Expected InvalidOperationException but it was not thrown" );
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_007b,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0008";
            if ( verbose ) Console.WriteLine( "tests to make sure ICollection can be given to stack" );
            try
            {
                ++iCountTestcases;
                int iNumElements = 10000;
                objArr = new Object[iNumElements];
                for ( int i = 0; i < iNumElements; i ++ )
                {
                    objArr[i] = i;
                }
                stack  = new Stack(objArr);
                for ( int i = 0; i < iNumElements; i++ )
                {
                    objRet = stack.Pop();
                    if ( ! objRet.Equals( iNumElements - i - 1 ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_008b,  Pop should have returned " + (iNumElements - i - 1).ToString() + " but returned " + objRet.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_008b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0009";
            if ( verbose ) Console.WriteLine( "peek at an element twice" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( 1 );
                if ( ! stack.Peek().Equals( stack.Peek() ) )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_009a,  Cannot Peek propertly twice in a row" );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_009b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0012";
            if ( verbose ) Console.WriteLine( "we should be able to push pop null elements onto the Stack" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( null );
                stack.Push( -1 );
                stack.Push( null );
                if ( stack.Pop() != null || ! (-1).Equals( stack.Pop()) || stack.Pop() != null )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_012a,  stack does not push/pop nulls correctly" );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_012b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0014";
            if ( verbose ) Console.WriteLine( "copy to null array so we get exception" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( "hey" );
                stack.CopyTo( null, 0 );
                ++iCountErrors;
                Console.WriteLine( "Err_014a,  Expected ArgumentNullException but no exception thrown" );
            }
            catch ( ArgumentNullException )
            {
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_014b,  Expected ArgumentNullException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0015";
            if ( verbose ) Console.WriteLine( "copy should throw because of outofrange" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                objArr = new Object[0];
                stack.CopyTo( objArr, 1 );
                ++iCountErrors;
                Console.WriteLine( "Err_015a,  Expected ArgumentException but no exception thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_015b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
            }
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                objArr = new Object[0];
                stack.CopyTo( objArr, Int32.MaxValue );
                ++iCountErrors;
                Console.WriteLine( "Err_015a,  Expected ArgumentException but no exception thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_015b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
            }
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                objArr = new Object[0];
                stack.CopyTo( objArr, Int32.MinValue );
                ++iCountErrors;
                Console.WriteLine( "Err_015a,  Expected ArgumentException but no exception thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_015b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
            }
            if ( verbose ) Console.WriteLine( "copy should throw because of outofrange" );
            Random random = new Random();
            for(int iii=0; iii<20; iii++)
            {
                try
                {
                    ++iCountTestcases;
                    stack = new Stack();
                    objArr = new Object[0];
                    stack.CopyTo( objArr, random.Next(-1000, 0));
                    ++iCountErrors;
                    Console.WriteLine( "Err_015a,  Expected ArgumentException but no exception thrown" );
                }
                catch ( ArgumentException )
                {
                }
                catch (Exception ex)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_015b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
                }
            }
            strLoc = "Loc_Tst0015";
            if ( verbose ) Console.WriteLine( "copy should throw because of outofrange" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( "MyString" );
                objArr = new Object[0];
                stack.CopyTo( objArr, 0 );
                ++iCountErrors;
                Console.WriteLine( "Err_016a,  Expected ArgumentException but no exception thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_016b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0017";
            if ( verbose ) Console.WriteLine( "get enumerator and loop through" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( "hey" );
                stack.Push( "hello" );
                IEnumerator ienum = stack.GetEnumerator();
                int iCounter = 0;
                while ( ienum.MoveNext() )
                {
                    iCounter++;
                }
                if ( iCounter != stack.Count )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_017a,  did not enumerate though enough elements" );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_017b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0018";
            if ( verbose ) Console.WriteLine( "get before starting enumerator" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( "a" );
                stack.Push( "b" );
                IEnumerator ienum = stack.GetEnumerator();
                Object obj = ienum.Current;
                ++iCountErrors;
                Console.WriteLine( "Err_018a,  should have thrown InvalidOperationException" );
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_018b,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0019";
            if ( verbose ) Console.WriteLine( "enumerate beyond end of list then get object" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( new Object() );
                stack.Push( stack );
                IEnumerator ienum = stack.GetEnumerator();
                Boolean fRet = ienum.MoveNext();
                if ( ! fRet	)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_019a,  expected MoveNext to return true at first move" );
                }
                for ( int i = 0; i < 100; i++ )
                {
                    Object objTemp1 = ienum.Current;
                    if ( ! objTemp1.Equals( stack ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_019f,  expected Current to return reference to stack since that is last object" );
                    }
                }
                fRet = ienum.MoveNext();
                if ( ! fRet	)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_019e,  expected MoveNext to return true at second move" );
                }
                for ( int i = 0; i < 100; i++ )
                {
                    fRet = ienum.MoveNext();
                    if ( fRet )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_019b,  expected MoveNext to return false after passing last element" );
                    }
                }
                Object objTemp2 = ienum.Current;
                ++iCountErrors;
                Console.WriteLine( "Err_019d,  InvalidOperationException expected was not thrown" );
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_019e,  Expected noexepction but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0020";
            if ( verbose ) Console.WriteLine( "modify underlying layer on list should get exception on enumerator" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( stack );
                IEnumerator ienum = stack.GetEnumerator();
                ienum.MoveNext();
                objRet = stack.Pop();
                Object obj = ienum.Current;
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_020b,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0020b";
            if ( verbose ) Console.WriteLine( "modify underlying layer on list should get exception on enumerator" );
            try
            {
                ++iCountTestcases;
                stack = new Stack();
                stack.Push( stack );
                IEnumerator ienum = stack.GetEnumerator();
                ienum.MoveNext();
                stack.Push( new Stack() );
                Object obj = ienum.Current;
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_020d,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            q1 = new Stack();
            hsh1 = new Hashtable();
            for(int i=0; i<100; i++)
            {
                hsh1.Add(i, null);
                q1.Push(i);
            }
            ienm1 = q1.GetEnumerator();
            ienm2 = (IEnumerator)((ICloneable)ienm1).Clone();
            iQueCount = 0;
            fPass = true;
            hsh2 = new Hashtable();
            while(ienm2.MoveNext())
            {
                iQueCount++;
                if(!hsh1.Contains(ienm2.Current))
                {
                    Console.WriteLine(ienm2.Current);
                    fPass = false;
                }
                try
                {
                    hsh2.Add(ienm2.Current, null);
                }
                catch(Exception)
                {
                    Console.WriteLine("What");
                    fPass = false;					
                }
            }
            if(iQueCount!=q1.Count)
            {
                Console.WriteLine("What" + iQueCount);
                fPass = false;
            }
            q1 = new Stack();
            hsh1 = new Hashtable();
            for(int i=0; i<100; i++)
            {
                hsh1.Add(i, null);
                q1.Push(i);
            }
            ienm1 = q1.GetEnumerator();
            while(ienm1.MoveNext())
            {
            }
            ienm2 = (IEnumerator)((ICloneable)ienm1).Clone();
            iQueCount = 0;
            while(ienm2.MoveNext())
            {
                iQueCount++;
            }
            if(iQueCount!=0)
            {
                Console.WriteLine("What" + iQueCount);
                fPass = false;
            }
            ienm2.Reset();
            try
            {
                objRet = ienm1.Current;
                fPass = false;
            }
            catch(Exception)
            {
            }
            iQueCount = 0;
            while(ienm2.MoveNext())
            {
                iQueCount++;
            }
            if(iQueCount!=q1.Count)
            {
                Console.WriteLine("What" + iQueCount);
                fPass = false;
            }
            ienm2.Reset();
            q1.Pop();
            try
            {
                objRet = ienm2.Current;
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                fPass = false;
            }
            q1 = new Stack();
            for(int i=0; i<100; i++)
            {
                q1.Push(i);
            }
            ienm1 = q1.GetEnumerator();
            q1.Pop();
            ienm2 = (IEnumerator)((ICloneable)ienm1).Clone();
            try
            {
                ienm2.MoveNext();
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                fPass = false;
            }
            if(!fPass)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_74532fsd,  Mayday! Mayday! something horribly gone wrong!!!" );
            }
            strLoc = "Loc_Tst0021";
            if ( verbose ) Console.WriteLine( "call Stacks constructor which takes integer and pass in negative" );
            try
            {
                ++iCountTestcases;
                stack = new Stack(Int32.MinValue);
                ++iCountErrors;
                Console.WriteLine( "Err_021a,  Expected ArgumentOutOfRangeException but no exception was thrown");
            }
            catch ( ArgumentOutOfRangeException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_021b,  Expected ArgumentOutOfRangeException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0022";
            if ( verbose ) Console.WriteLine( "call Stacks constructor which takes integer and pass in 0" );
            try
            {
                ++iCountTestcases;
                stack = new Stack(0);
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_022b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0023";
            if ( verbose ) Console.WriteLine( "call Stacks constructor which takes integer and pass in valid and make sure coutn is 0" );
            try
            {
                ++iCountTestcases;
                stack = new Stack(1);
                if ( stack.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_023a,  Stack should have had a count of 0 but it had count " + stack.Count.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_023b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0024";
            if ( verbose ) Console.WriteLine( "Synchronize stack and check if the IsSynchronized property is true" );
            try
            {
                ++iCountTestcases;
                stack = Stack.Synchronized( new Stack() );
                if ( ! stack.IsSynchronized )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_024a,  Stack is synchronized but it was not marked as such" );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_024b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0025";
            if ( verbose ) Console.WriteLine( "Synchronize stack which is null should throw" );
            try
            {
                ++iCountTestcases;
                stack = Stack.Synchronized( null );
                ++iCountErrors;
                Console.WriteLine( "Err_025a,  Expected ArgumentNullException to be thrown but it was not" );
            }
            catch ( ArgumentNullException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_025b,  Expected ArgumentNullException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_452dsfg";
            ++iCountTestcases;
            fPass=true;
            q1 = new Stack();
            for(int i=0; i<10; i++)
                q1.Push("String_" + i);
            q2 = Stack.Synchronized(q1);
            if(q2.Count != q1.Count)
                fPass=false;
            q2.Clear();
            if(q2.Count != 0)
                fPass=false;
            for(int i=0; i<10; i++)
                q2.Push("String_" + i);
            for(int i=0, j=9; i<10; i++, j--)
            {
                if(!((String)q2.Peek()).Equals("String_" + j))
                {
                    Console.WriteLine(q2.Peek());
                    fPass=false;
                }
                if(!((String)q2.Pop()).Equals("String_" + j))
                {
                    Console.WriteLine(q2.Peek());
                    fPass=false;
                }
            }
            if(q2.Count != 0)
                fPass=false;
            if(!q2.IsSynchronized)
                fPass=false;
            for(int i=0; i<10; i++)
                q2.Push("String_" + i);
            q3 = Stack.Synchronized(q2);
            if(!q3.IsSynchronized || (q2.Count!=q3.Count))
                fPass = false;
            strArr = new String[10];
            q2.CopyTo(strArr, 0);
            for(int i=0, j=9; i<10; i++, j--)
            {
                if(!strArr[i].Equals("String_" + j))
                    fPass = false;
            }
            strArr = new String[10 + 10];
            q2.CopyTo(strArr, 10);
            for(int i=0, j=9; i<10; i++, j--)
            {
                if(!strArr[i+10].Equals("String_" + j))
                    fPass = false;
            }
            try
            {
                q2.CopyTo(null, 0);
                fPass = false;
            }
            catch(ArgumentNullException)
            {
            }
            catch(Exception)
            {
                fPass = false;
            }
            oArr = q2.ToArray();
            for(int i=0, j=9; i<10; i++, j--)
            {
                if(!((String)oArr [i]).Equals("String_" + j))
                {
                    fPass=false;
                }
            }
            ienm1 = q2.GetEnumerator();
            try
            {
                oValue = ienm1.Current;
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                fPass = false;
            }
            iCount = 9;
            while(ienm1.MoveNext())
            {
                if(!((String)ienm1.Current).Equals("String_" + iCount))
                {
                    fPass=false;
                }
                iCount--;
            }
            ienm1.Reset();
            iCount = 9;
            while(ienm1.MoveNext())
            {
                if(!((String)ienm1.Current).Equals("String_" + iCount))
                {
                    fPass=false;
                }
                iCount--;
            }
            ienm1.Reset();
            q2.Pop();
            try
            {
                oValue = ienm1.Current;
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                fPass = false;
            }
            try
            {
                ienm1.MoveNext();
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                fPass = false;
            }
            try
            {
                ienm1.Reset();
                fPass = false;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception)
            {
                fPass = false;
            }
            if(!fPass)
            {
                ++iCountErrors;
                Console.WriteLine( "Unexpect result returned!" );
            }
            strLoc = "Loc_Tst0026";
            if ( verbose ) Console.WriteLine( "constructor should throw if ICollection is null" );
            try
            {
                ++iCountTestcases;
                stack = new Stack( null );
                ++iCountErrors;
                Console.WriteLine( "Err_026a,  Expected ArgumentNullException to be thrown but it was not" );
            }
            catch ( ArgumentNullException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_026b,  Expected ArgumentNullException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_COPYCAT";
            Console.WriteLine( "Info:  Starting CopyCat" );
            LinkedList results = StartCopyCat();
            if ( results == null )
            {
                Console.WriteLine( "Info:  CopyCat has NOT found errors" );
            }
            else
            {
                ++iCountErrors;
                Console.WriteLine( "Error:  CopyCat has found Errors" );
                Console.WriteLine( "/////////////////////// CopyCat StackTrace ///////////////////////////////" );

                Object [] allResults = results.GetElements();
                for ( int i = 0; i < allResults.Length; i ++ )
                {
                    Console.WriteLine( ((String) allResults[i]).ToString() );
                }
            }
        }
        catch (Exception exc_general)
        {
            ++iCountErrors;
            Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc );
            Console.WriteLine( "exc_general=="+ exc_general.ToString() );
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases="+ iCountTestcases.ToString() );
            return true;
        }
        else
        {
            Console.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors="+ iCountErrors.ToString() +" ,BugNums?: "+ s_strActiveBugNums );
            return false;
        }
    }
    public LinkedList StartCopyCat()
    {
        Random				rand					= null;
        StackMimic			sm						= null;
        Stack					sr						= null;
        int					iCountErrors		= 0;
        int					iCountTestcases	= 0;
        LinkedList        resArray          = new LinkedList();           
        Console.WriteLine( "RANDOM SEED == " + m_iRandSeed.ToString() );
        rand = new Random(m_iRandSeed);
        resArray.Add( "Stack sr = new Stack();" );
        resArray.Add( "Stack sm = new StackMimic();" );
        sr = new Stack();
        sm = new StackMimic();
        int iStartTime = Environment.TickCount;
        while ( (iStartTime + m_iTickCount) > Environment.TickCount )
        {
            if ( rand.Next(0, 10) <= 0 )        
            {
                Exception gotExceptionReal = null;
                Exception gotExceptionFake = null;
                try
                {
                    ++iCountTestcases;
                    sr  = new Stack();
                    resArray.Clear();
                    resArray.Add( "Stack s = new Stack();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { sr = new Stack(); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    sm = new StackMimic();
                    resArray.Add( "StackMimic sm = new StackMimic();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { sm = new StackMimic(); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionFake = ex;
                }
                if ( ! EqualExceptions( gotExceptionReal, gotExceptionFake ) )
                {
                    ++iCountErrors;
                    String strRealEx = "";
                    String strFakeEx = "";
                    if ( gotExceptionReal == null ) { strRealEx = "NULL"; } 
                    else { strRealEx = gotExceptionReal.GetType().ToString(); }
                    if ( gotExceptionFake == null ) { strFakeEx = "NULL"; } 
                    else { strFakeEx = gotExceptionFake.GetType().ToString(); }
                    Console.WriteLine( "Exceptions do not match from Constructor, real:" + strRealEx + "  mimic: " + strFakeEx );
                    return resArray;
                }
            }
            if ( rand.Next(0, 10) <= 0 )        
            {
                int iRandCapacity = rand.Next( -10, 1000 );
                Exception gotExceptionReal = null;
                Exception gotExceptionFake = null;
                try
                {
                    ++iCountTestcases;
                    sr  = new Stack( iRandCapacity );
                    resArray.Clear();
                    resArray.Add( "Stack s = new Stack( " + iRandCapacity.ToString() + " );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { sr = new Stack(" + iRandCapacity.ToString() + "); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    sm = new StackMimic( iRandCapacity );
                    resArray.Add( "StackMimic sm = new StackMimic(" + iRandCapacity.ToString() + ");" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { sm = new StackMimic(" + iRandCapacity.ToString() + "); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionFake = ex;
                }
                if ( ! EqualExceptions( gotExceptionReal, gotExceptionFake ) )
                {
                    ++iCountErrors;
                    String strRealEx = "";
                    String strFakeEx = "";
                    if ( gotExceptionReal == null ) { strRealEx = "NULL"; } 
                    else { strRealEx = gotExceptionReal.GetType().ToString(); }
                    if ( gotExceptionFake == null ) { strFakeEx = "NULL"; } 
                    else { strFakeEx = gotExceptionFake.GetType().ToString(); }
                    Console.WriteLine( "Exceptions do not match from Constructor, real:" + strRealEx + "  mimic: " + strFakeEx );
                    return resArray;
                }
            }
            if ( rand.Next(0, 10) <= 5 )        
            {
                int iGuessObjectArg = rand.Next();
                Object objGuessArg1 = Common.GuessObject( iGuessObjectArg );
                Exception gotExceptionReal = null;
                Exception gotExceptionFake = null;
                try
                {
                    ++iCountTestcases;
                    sr.Push(objGuessArg1);
                    resArray.Add( "sr.Push( GuessObject( " +  iGuessObjectArg.ToString() + ") );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { sr.Push( GuessObject( " +  iGuessObjectArg.ToString() + ") ); Console.WriteLine(\"Error\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    sm.Push(objGuessArg1);
                    resArray.Add( "sm.Push( GuessObject( " +  iGuessObjectArg.ToString() + ") );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { sm.Push( GuessObject( " +  iGuessObjectArg.ToString() + ") ); Console.WriteLine(\"Error\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionFake = ex;
                }
                if ( ! EqualExceptions( gotExceptionReal, gotExceptionFake ) )
                {
                    ++iCountErrors;
                    String strRealEx = "";
                    String strFakeEx = "";
                    if ( gotExceptionReal == null ) { strRealEx = "NULL"; } 
                    else { strRealEx = gotExceptionReal.GetType().ToString(); }
                    if ( gotExceptionFake == null ) { strFakeEx = "NULL"; } 
                    else { strFakeEx = gotExceptionFake.GetType().ToString(); }
                    Console.WriteLine( "Exceptions do not match from Add, real:" + strRealEx + "  mimic: " + strFakeEx );
                    return resArray;
                }
            }
            if ( rand.Next(0, 10) <= 0 )        
            {
                Exception gotExceptionReal = null;
                Exception gotExceptionFake = null;
                try
                {
                    ++iCountTestcases;
                    sr.Clear();
                    resArray.Add( "sr.Clear();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( " try { sr.Clear(); Console.WriteLine( \"ERROR\" );} catch (" + ex.GetType().Name + ") {}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    sm.Clear();
                    resArray.Add( "sm.Clear();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( " try { sm.Clear(); Console.WriteLine( \"ERROR\" );} catch (" + ex.GetType().Name + ") {}" );
                    gotExceptionFake = ex;
                }
                if ( ! EqualExceptions( gotExceptionReal, gotExceptionFake ) )
                {
                    ++iCountErrors;
                    String strRealEx = "";
                    String strFakeEx = "";
                    if ( gotExceptionReal == null ) { strRealEx = "NULL"; } 
                    else { strRealEx = gotExceptionReal.GetType().ToString(); }
                    if ( gotExceptionFake == null ) { strFakeEx = "NULL"; } 
                    else { strFakeEx = gotExceptionFake.GetType().ToString(); }
                    Console.WriteLine( "Exceptions do not match from Clear, real:" + strRealEx + "  mimic: " + strFakeEx );
                    return resArray;
                }
            }
            if ( rand.Next(0, 10) <= 8 )        
            {
                int gotResultReal = 0;
                int gotResultFake = 0;
                Exception gotExceptionReal = null;
                Exception gotExceptionFake = null;
                try
                {
                    ++iCountTestcases;
                    gotResultReal = sr.Count;
                    resArray.Add( "Console.WriteLine( sr.Count.ToString() );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { Console.WriteLine( sr.Count.ToString() ); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ){}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    gotResultFake = sm.Count;
                    resArray.Add( "Console.WriteLine( sm.Count.ToString() );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { Console.WriteLine( sm.Count.ToString() ); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ){}" );
                    gotExceptionFake = ex;
                }
                if ( ! EqualExceptions( gotExceptionReal, gotExceptionFake ) )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Exceptions do not match from Count, real:" + gotExceptionReal.GetType().ToString() + "  mimic: " + gotExceptionFake.GetType().ToString() );
                    return resArray;
                }
                if (  gotResultReal != gotResultFake )
                {
                    ++iCountErrors;
                    String strRealEx = "";
                    String strFakeEx = "";
                    if ( gotExceptionReal == null ) { strRealEx = "NULL"; } 
                    else { strRealEx = gotExceptionReal.GetType().ToString(); }
                    if ( gotExceptionFake == null ) { strFakeEx = "NULL"; } 
                    else { strFakeEx = gotExceptionFake.GetType().ToString(); }
                    Console.WriteLine( "Exceptions do not match from Count, real:" + strRealEx + "  mimic: " + strFakeEx );
                    return resArray;
                }
            }
        } 
        return null;
    }
    public Boolean EqualExceptions( Exception ex1, Exception ex2 )
    {
        if ( ex1 != null && ex2 != null )
        {
            return ex1.GetType().Equals( ex2.GetType() );
        }
        else if ( ex1 == null && ex2 == null )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    public static void printUsage()
    {
        Console.WriteLine( "USAGE: Co1726allSigns_Stack_Copycat [option option ....]" );
        Console.WriteLine( "      options:" );
        Console.WriteLine( "               /s randomseed - the random seed we should use, randomseed must be an integer" );
        Console.WriteLine( "               /r runtime    - for how long you want to run for in ticks, runtime must be an integer, default=10000 == 10sec" );
        Console.WriteLine( "               /?            - prints usage and exits" );
    }
    public static void Main( String [] args )
    {
        Boolean bolResult = false;                                  
        Co1726allSigns_Stack_Copycat cbX = new Co1726allSigns_Stack_Copycat();
        if ( Common.argContains( args, "/?" ) )
        {
            printUsage();
            Environment.ExitCode =  0 ;
            return;
        }
        if ( Common.argContains( args, "/s" ) )
        {
            String strStartRandSeed = Common.argGetArg( args, "/s" );
            if ( strStartRandSeed == null )
            {
                Console.Out.WriteLine( "Incorrect rand seed specified" );
                printUsage();
                Environment.ExitCode =  0 ;
                return;
            }
            try
            {
                cbX.m_iRandSeed = Int32.Parse( strStartRandSeed );
            }
            catch ( Exception )
            {
                Console.Out.WriteLine( "Invalid random seed specified, the string specified is not a number" );
                printUsage();
                Environment.ExitCode =  0 ;
                return;
            }
        }
        else
        {
            cbX.m_iRandSeed = (new Random()).Next();
        }
        if ( Common.argContains( args, "/r" ) )
        {
            String strStartRunTime = Common.argGetArg( args, "/r" );
            if ( strStartRunTime == null )
            {
                Console.WriteLine( "Incorrect run time specified" );
                printUsage();
                Environment.ExitCode =  0 ;
                return;
            }
            try
            {
                cbX.m_iTickCount = Int32.Parse( strStartRunTime );
            }
            catch ( Exception )
            {
                Console.WriteLine( "Invalid run time specified, the string specified is not a number" );
                printUsage();
                Environment.ExitCode =  0 ;
                return;
            }
        }
        else
        {
            cbX.m_iTickCount = 10000;
        }
        try
        {
            bolResult = cbX.runTest();
        }
        catch ( Exception exc_main )
        {
            bolResult = false;
            Console.WriteLine( s_strTFAbbrev +"  FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main()" );
            Console.WriteLine( "exc_main=="+ exc_main.ToString() );
        }
        if ( ! bolResult )
        {
            Console.WriteLine( "FullerPath="+ s_strTFPath +" "+ s_strTFName );
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
            Console.WriteLine( " " );
        }
        if ( bolResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    } 
} 
class StackMimic
{
    private Object [] m_store;
    public StackMimic()
    {
        m_store = new Object[0];
    }
    public StackMimic( int size )
    {
        if ( size < 0 )
        {
            throw new ArgumentOutOfRangeException( "StackMimic:  size is less than 0" );
        }
        m_store = new Object[0];
    }
    public int Count
    {
        get { return m_store.Length; }
    }
    public void Push( Object obj )
    {
        Object [] objTemp = new Object[m_store.Length + 1 ];
        for ( int i = 0; i < m_store.Length; i++ )
        {
            objTemp[i] = m_store[i];
        }
        objTemp[m_store.Length] = obj;
        m_store = objTemp;
    }
    public void Clear()
    {
        m_store = new Object[0];
    }
    public Object Pop( )
    {
        if ( m_store.Length == 0 )
        {
            throw new InvalidOperationException( "StackMimic:  Pop" );
        }
        Object retObject = m_store[m_store.Length - 1];
        Object [] objTemp = new Object[m_store.Length - 1];
        for ( int i = 0; i < objTemp.Length; i++ )
        {
            objTemp[i] = m_store[i];
        }
        m_store = objTemp;
        return retObject;
    }
    public Object Peek( )
    {
        if ( m_store.Length == 0 )
        {
            throw new InvalidOperationException( "StackMimic:  Peek" );
        }
        return m_store[m_store.Length - 1];
    }
    public Object [] ToArray()
    {
        Object [] objTemp = new Object[ m_store.Length ];
        for ( int i = m_store.Length - 1; i <= 0; i++ )
        {
            objTemp[m_store.Length - i - 1] = m_store[i];
        }
        return objTemp;
    }
}
class Common
{
    public static String GuessString( int iRandSeed, int iLength )
    {
        Random rand = new Random( iRandSeed );
        StringBuilder sb = new StringBuilder();
        for ( int i = 0; i < iLength; i += 1 )
        {
            sb.Append( (char) rand.Next( Char.MinValue, Char.MaxValue ) );
        }
        return sb.ToString();
    }
    public static Object GuessObject( int iRandSeed )
    {
        if ( iRandSeed > (Int32.MaxValue / 1.5) )
        {
            return null;
        }
        Object obj = iRandSeed;
        return obj;
    }
    public static Boolean argContains( String [] args, String str )
    {
        for ( int i = 0; i < args.Length; i += 1 )
        {
            if ( args[i].Equals( str ) )
            {
                return true;
            }
        }
        return false;
    }
    public static String argGetArg( String [] args, String str )
    {
        for ( int i = 0; i < args.Length; i += 1 )
        {
            if ( args[i].Equals( str ) )
            {
                if ( i == (args.Length - 1) )
                {
                    return null;
                }
                else
                {
                    return args[i+1];
                }
            }
        }
        return null;
    }
    public static Boolean CompareArrays( Object [] arr1, Object [] arr2 )
    {
        return CompareArrays( arr1, arr2, false );
    }
    public static Boolean CompareArrays( Object [] arr1, Object [] arr2, Boolean inOrder )
    {
        int i, j;
        Boolean fPresent = false;
        if ( arr1.Length != arr2.Length )
        {
            return false;
        }
        if ( inOrder == true )
        {
            for ( i = 0; i < arr1.Length; i++ )
            {
                if (  ! ( ( arr1[i] == null && arr2[i] == null ) ||
                    ( arr1[i] != null && arr1[i].Equals( arr2[i] ) )
                    )                                                    )
                {
                    Console.WriteLine( "Ordered compare:  Arrays differ at position " + i.ToString() );
                    return false;
                }
            }
            return true;
        }
        for (  i = 0; i < arr1.Length; i++ )
        {
            fPresent = false;
            for (  j = 0; j < arr2.Length && ( fPresent == false ); j++ )
            {
                if (  (arr1[i] == null && arr2[j] == null )
                    ||
                    (arr1[i] != null && arr1[i].Equals(arr2[j]))   )
                {
                    fPresent = true;
                }
            }
            if ( fPresent == false )
            {
                return false;
            }
        }
        for (  i = 0; i < arr2.Length; i++ )
        {
            fPresent = false;
            for (  j = 0; j < arr1.Length && ( fPresent == false ); j++ )
            {
                if (  (arr2[i] == null && arr1[j] == null )
                    ||
                    (arr2[i] != null && arr2[i].Equals(arr1[j]))   )
                {
                    fPresent = true;
                }
            }
            if ( fPresent == false )
            {
                return false;
            }
        }
        return true;
    }
}
class LinkedList
{
    LinkedListNode        m_start;         
    public int            m_Count;         
    public class LinkedListNode
    {
        public LinkedListNode last;
        public Object obj;
        public LinkedListNode next;
        public LinkedListNode( Object obj )
        {
            this.obj = obj;
            last = null;
            next = null;
        }
    };
    public LinkedList()
    {
        m_start = null;
        m_Count = 0;
    }
    public static Boolean SelfTest()
    {
        LinkedList temp_store = new LinkedList();
        temp_store.Add( "elem1" );
        if ( ! temp_store.Remove( "elem1" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_001" );
        }
        if ( temp_store.Contains( "elem1" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_002" );
        }
        if ( temp_store.m_Count != 0 )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_002a" );
        }
        temp_store.Add( "a1" );
        temp_store.Add( "a2" );
        if ( ! temp_store.Remove( "a2" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_002b" );
        }
        if ( ! temp_store.Remove( "a1" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_002c" );
        }
        temp_store.Add( "elem1" );
        temp_store.Add( "elem2" );
        temp_store.Add( "elem3" );
        if ( ! temp_store.Remove( "elem2" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_003" );
        }
        if ( temp_store.Contains( "elem2" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_004" );
        }
        if ( ! temp_store.Remove( "elem3" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_005" );
        }
        if ( temp_store.Contains( "elem3" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_006" );
        }
        if ( temp_store.m_Count != 1 )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_007" );
        }
        temp_store = new LinkedList();
        temp_store.Add( "a" );
        temp_store.Remove( "a"  );
        temp_store.Add( "b" );
        temp_store.Add( "d" );
        temp_store.Remove( "b" );
        if ( temp_store.Contains( "b" ) )
        {
            throw new Exception( "LinkedList implementation incorrect, Err_008" );
        }
        return true;
    }
    public void Add(Object obj)
    {
        LinkedListNode node = new LinkedListNode( obj );
        LinkedListNode ptrEnd = m_start;
        if ( ptrEnd == null )
        {
            m_start = node;
        }
        else
        {
            while( ptrEnd.next != null )
            {
                ptrEnd = ptrEnd.next;
            }
            ptrEnd.next = node;
            node.last = ptrEnd;
        }
        m_Count++;
    }
    public void Set( int index, Object obj )
    {
        LinkedListNode ptrEnd = m_start;
        int iCounter          = 0;
        if ( ptrEnd == null )
        {
            throw new Exception( "INCORRECT use of LinkedList, ptrEnd is null in Set" );
        }
        while( true )
        {
            if ( index == iCounter )
            {
                ptrEnd.obj = obj;
                return;
            }
            ptrEnd = ptrEnd.next;
            iCounter ++;
            if ( ptrEnd == null )
            {
                break;
            }
        }
        throw new Exception( "INCORRECT use of LinkedList, expected valid index for Set" );
    }
    public Boolean Remove(Object obj)
    {
        LinkedListNode ptrEnd = m_start;
        if (  ptrEnd != null &&
            (
            (ptrEnd.obj == null && obj == null)
            ||
            (ptrEnd.obj != null && ptrEnd.obj.Equals( obj ) )
            )                                                        )
        {
            m_start = ptrEnd.next;
            if ( m_start != null )
            {
                m_start.last = null;
            }
            m_Count--;
            return true;
        }
        while( ptrEnd != null )
        {
            if ( (ptrEnd.obj == null && obj == null) ||
                (ptrEnd.obj != null && ptrEnd.obj.Equals( obj ) )  )
            {
                m_Count--;
                if ( m_Count == 0 )
                {
                    m_start = null;
                    return true;
                }
                if ( ptrEnd.next != null )
                {
                    ptrEnd.next.last = ptrEnd.last;
                }
                if ( ptrEnd.last != null )
                {
                    ptrEnd.last.next = ptrEnd.next;
                }
                ptrEnd.next = null;
                ptrEnd.last = null;
                ptrEnd = null;
                return true;
            }
            ptrEnd = ptrEnd.next;
        }
        return false;
    }
    public Boolean Contains(Object obj)
    {
        LinkedListNode ptrEnd = m_start;
        while( ptrEnd != null )
        {
            if ( (ptrEnd.obj == null && obj == null) ||
                (ptrEnd.obj != null && ptrEnd.obj.Equals( obj ) )  )
            {
                return true;
            }
            ptrEnd = ptrEnd.next;
        }
        return false;
    }
    public void Clear()
    {
        m_start = null;
        m_Count = 0;
    }
    public Object [] GetElements()
    {
        Object [] retArray = new Object[m_Count];
        int i = 0;
        LinkedListNode ptrEnd = m_start;
        while( ptrEnd != null )
        {
            retArray[i] = ptrEnd.obj;
            i++;
            ptrEnd = ptrEnd.next;
        }
        return retArray;
    }
}
