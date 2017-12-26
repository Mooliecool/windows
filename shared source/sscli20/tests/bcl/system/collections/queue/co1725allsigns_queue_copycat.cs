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
class Co1725allSigns_Queue_Copycat:IDescribeTestedMethods
{
    public const String s_strActiveBugNums = "";
    public const String s_strDtTmVer       = "";
    public const String s_strClassMethod   = "allSigns";
    public const String s_strTFName        = "Co1725allSigns_Queue_Copycat.cs";
    public const String s_strTFAbbrev      = "Co1725";
    public const String s_strTFPath        = "";
    public Boolean verbose                 = false;													
    public int m_iTickCount;				
    public int m_iRandSeed;					
    public MemberInfo[] GetTestedMethods()
    {
        Type type = typeof(Queue);
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
        "Method_Covered:  Queue( )"
        ,"Method_Covered:  Queue( int )"
        ,"Method_Covered:  Queue( int, Single )"
        ,"Method_Covered:  Queue( IEnumerator )"
        ,"Method_Covered:  Count_get"
        ,"Method_Covered:  GetEnumerator( )"
        ,"Method_Covered:  ToArray( )"
        ,"Method_Covered:  CopyTo( Array, Int32)"
        ,"Method_Covered:  IsSynchronized_get"
        ,"Method_Covered:  SyncRoot_get"
        ,"Method_Covered:  Clear( )"
        ,"Method_Covered:  Remove( Object )"
        ,"Method_Covered:  Enqueue( Object )"
        ,"Method_Covered:  Peek( )"
        ,"Method_Covered:  Dequeue( )"
        ,"Method_Covered:  Synchronized( Queue )"
    };
    public static void printoutCoveredMethods()
    {
        Console.Error.WriteLine( "" );
        Console.Error.WriteLine( "Method_Count==16  ("+ s_strMethodsCovered.Length.ToString() +"==confirm) !!" );
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
        Queue queue         = null;
        Object [] objArr    = null;
        Object objRet = null;                         
        int iCountErrors    = 0;                      
        int iCountTestcases = 0;                      
        IEnumerator ienm1;
        IEnumerator ienm2;
        Hashtable hsh1;
        Hashtable hsh2;
        Int32 iQueCount;
        Boolean fPass;
        try
        {
            strLoc = "Loc_Tst0001";
            if ( verbose ) Console.WriteLine( "try to make an empty queue and check to make sure counter is 0" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                if ( queue.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001a,  Count on newly created queue was not 0 but " + queue.Count.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0001_1";
            if ( verbose ) Console.WriteLine( "ctor with ICollection" );
            try
            {
                ++iCountTestcases;
                ArrayList alst = new ArrayList();
                for(int i=0; i<10;i++)
                {
                    alst.Add("String_" + i);
                }
                queue = new Queue(alst);
                if ( queue.Count != 10 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001a,  Count on newly created queue was not 0 but " + queue.Count.ToString() );
                }
                for(int i=0; i<10;i++)
                {
                    if ( !((String)queue.Dequeue()).Equals("String_" + i) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_756923bgd,  wrong value returned " + queue.Dequeue() );
                    }
                }
                try 
                {
                    ++iCountTestcases;
                    ICollection icol = null;
                    queue = new Queue(icol);
                }
                catch(ArgumentNullException)
                {
                }
                catch (Exception ex)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_002b,  Expected noexception but exception thrown= " + ex.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0002";
            if ( verbose ) Console.WriteLine( "test to make sure we can create a queue and by default it is not synchronized" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                if ( queue.IsSynchronized )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_002a,  Newly created queue should not be synchronized" );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_002b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0003";
            if ( verbose ) Console.WriteLine( "test to make sure we enqueue elements to queue and we get respective count back" );
            try
            {
                ++iCountTestcases;
                int iNumElementsAdded = 1975;
                queue = new Queue();
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    queue.Enqueue( new Object() );
                }
                if ( queue.Count != iNumElementsAdded )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_003a,  Counter should be " + iNumElementsAdded.ToString() + " but is " + queue.Count.ToString() );
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
                queue = new Queue();
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    queue.Enqueue( new Object() );
                }
                queue.Clear();
                if ( queue.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_004a,  Counter should be " + 0.ToString() + " but is " + queue.Count.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_004b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0005";
            if ( verbose ) Console.WriteLine( "make sure dequeue decrements counter" );
            try
            {
                ++iCountTestcases;
                int iNumElementsAdded = 25;
                queue = new Queue();
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    queue.Enqueue( i );
                }
                for ( int i = 0; i < iNumElementsAdded; i++ )
                {
                    if ( queue.Count != iNumElementsAdded - i )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_005a,  Counter should be " + (iNumElementsAdded - i).ToString() + " but is " + queue.Count.ToString() );
                    }
                    Object objTop = queue.Dequeue();
                    if ( ! objTop.Equals( i ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_005c,  Dequeued element is not what it is supposed to be, expected " + i.ToString() + " but got " + objTop.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_005b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0006";
            if ( verbose ) Console.WriteLine( "try peek on an empty queue" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                objRet = queue.Peek();
                ++iCountErrors;
                Console.WriteLine( "Err_006a,  Expected objInvalidOperationException but it was not" );
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_006b,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0007";
            if ( verbose ) Console.WriteLine( "try peek on an empty queue after elements have been removed from it" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                for ( int i = 0; i < 1000; i++ )
                {
                    queue.Enqueue( i );
                }
                for ( int i = 0; i < 1000; i++ )
                {
                    objRet = queue.Dequeue();
                }
                objRet = queue.Peek();
                ++iCountErrors;
                Console.WriteLine( "Err_007a,  Expected InvalidOperationException to be thrown but it was not" );
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_007b,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0011";
            if ( verbose ) Console.WriteLine( "by default queue should not be synchronized" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                if ( queue.IsSynchronized != false )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_011a,  issynchronized should have returned false" );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_011b,  Expected noexception but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0013";
            if ( verbose ) Console.WriteLine( "call dequeue and should cause exceptions after queue is empty" );
            try
            {
                ++iCountTestcases;
                int iNumElements = 110;
                for ( int i = 0; i < iNumElements; i ++ )
                {
                    queue.Enqueue( i );
                }
                for ( int i = 0; i < iNumElements; i ++ )
                {
                    objRet = queue.Dequeue();
                    if ( objRet == null )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_013a,  Dequeue should not have returned null at iteration " + i.ToString() );
                    }
                }
                for ( int i = 0; i < 10; i++ )
                {
                    try
                    {
                        objRet = queue.Dequeue();
                        ++iCountErrors;
                        Console.WriteLine( "Err_013b,  Dequeue should have thrown exception but did not" );
                    }
                    catch ( InvalidOperationException )
                    {}
                    catch ( Exception exin )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_013c,  Dequeue should thrown InvalidOperationException but it threw=" + exin.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_013b,  Expected ArgumentOutOfRangeException but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0014";
            if ( verbose ) Console.WriteLine( "copy to null array so we get exception" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                queue.Enqueue( "hey" );
                queue.CopyTo( null, 0 );
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
                queue = new Queue();
                objArr = new Object[0];
                queue.CopyTo( objArr, 1 );
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
                queue = new Queue();
                objArr = new Object[0];
                queue.CopyTo( objArr, Int32.MaxValue );
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
                queue = new Queue();
                objArr = new Object[0];
                queue.CopyTo( objArr, Int32.MinValue );
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
                    queue = new Queue();
                    objArr = new Object[0];
                    queue.CopyTo( objArr, random.Next(-1000, 0));
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
                queue = new Queue();
                queue.Enqueue( "MyString" );
                objArr = new Object[0];
                queue.CopyTo( objArr, 0 );
                ++iCountErrors;
                Console.WriteLine( "Err_016a,  Expected ArgumentException but no exception thrown" );
            }
            catch ( ArgumentOutOfRangeException )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_016c,  Expected ArgumentException but exception thrown=ArgumentOutOfRangeException"  );
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
                queue = new Queue();
                queue.Enqueue( "hey" );
                queue.Enqueue( "hello" );
                IEnumerator ienum = queue.GetEnumerator();
                int iCounter = 0;
                while ( ienum.MoveNext() )
                {
                    iCounter++;
                }
                if ( iCounter != queue.Count )
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
                queue = new Queue();
                queue.Enqueue( "a" );
                queue.Enqueue( "b" );
                IEnumerator ienum = queue.GetEnumerator();
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
                queue = new Queue();
                queue.Enqueue( queue );
                queue.Enqueue( new Object() );
                IEnumerator ienum = queue.GetEnumerator();
                Boolean fRet = ienum.MoveNext();
                if ( ! fRet	)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_019a,  expected MoveNext to return true at first move" );
                }
                for ( int i = 0; i < 100; i++ )
                {
                    Object objTemp1 = ienum.Current;
                    if ( ! objTemp1.Equals( queue ) )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_019f,  expected GetObject to return reference to queue since that is first object" );
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
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_019d,  Expected noexepction but exception thrown= " + ex.ToString() );
            }
            strLoc = "Loc_Tst0020";
            if ( verbose ) Console.WriteLine( "modify underlying layer on list should get exception on enumerator" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                queue.Enqueue( queue );
                IEnumerator ienum = queue.GetEnumerator();
                ienum.MoveNext();
                objRet = queue.Dequeue();
                Object obj = ienum.Current;
            }
            catch ( InvalidOperationException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_020b,  Expected InvalidOperationException but exception thrown= " + ex.ToString() );
            }
            try
            {
                Int32 iDefCap = 10;
                queue = new Queue(iDefCap);
                for(int ij=0; ij<iDefCap; ij++)
                    queue.Enqueue(ij);
                IEnumerator ienum = queue.GetEnumerator();
                int iCounter = 0;
                while ( ienum.MoveNext() )
                {
                    iCounter++;
                }
                if(iCounter != iDefCap)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_74534fsfd,  Wrong result returned, Expectecd - {0}, Returned - {1}", iDefCap, iCounter);				
                }
                queue = new Queue(iDefCap);
                for(int ij=0; ij<iDefCap-1; ij++)
                    queue.Enqueue(ij);
                ienum = queue.GetEnumerator();
                iCounter = 0;
                while ( ienum.MoveNext() )
                {
                    iCounter++;
                }
                if(iCounter != (iDefCap-1))
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_75634fsf,  Wrong result returned, Expectecd - {0}, Returned - {1}", (iDefCap-1), iCounter);				
                }
            }
            catch(Exception exO)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_97463rfg,  Unexpected exception thrown= " + exO.ToString() );				
            }
            queue = new Queue();
            hsh1 = new Hashtable();
            for(int i=0; i<100; i++)
            {
                hsh1.Add(i, null);
                queue.Enqueue(i);
            }
            ienm1 = queue.GetEnumerator();
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
            if(iQueCount!=queue.Count)
            {
                Console.WriteLine("What" + iQueCount);
                fPass = false;
            }
            queue = new Queue();
            hsh1 = new Hashtable();
            for(int i=0; i<100; i++)
            {
                hsh1.Add(i, null);
                queue.Enqueue(i);
            }
            ienm1 = queue.GetEnumerator();
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
            if(iQueCount!=queue.Count)
            {
                Console.WriteLine("What" + iQueCount);
                fPass = false;
            }
            ienm2.Reset();
            queue.Dequeue();
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
            queue = new Queue();
            for(int i=0; i<100; i++)
            {
                queue.Enqueue(i);
            }
            ienm1 = queue.GetEnumerator();
            queue.Dequeue();
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
            if ( verbose ) Console.WriteLine( "make sure that ToArray returns a 0 sized array" );
            try
            {
                ++iCountTestcases;
                queue = new Queue();
                objArr = queue.ToArray();
                if ( objArr.Length != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_021a,  returned array should have have length 0 but it was length " + objArr.Length.ToString() );
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_021b,  Expected no exception but exception thrown= " + ex.ToString() );
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
        QueueMimic			qm						= null;
        Queue					qr						= null;
        int					iCountErrors		= 0;
        int					iCountTestcases	= 0;
        LinkedList        resArray          = new LinkedList();           
        Console.WriteLine( "RANDOM SEED == " + m_iRandSeed.ToString() );
        rand = new Random(m_iRandSeed);
        resArray.Add( "Queue qr = new Queue();" );
        resArray.Add( "Queue qm = new QueueMimic();" );
        qr = new Queue();
        qm = new QueueMimic();
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
                    qr  = new Queue();
                    resArray.Clear();
                    resArray.Add( "Queue qr = null;" );
                    resArray.Add( "qr = new Queue();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { qr = new Queue(); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    qm = new QueueMimic();
                    resArray.Add( "QueueMimic qm = null;" );
                    resArray.Add( "qm = new QueueMimic();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { qm = new QueueMimic(); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ) {}" );
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
                    qr.Enqueue(objGuessArg1);
                    resArray.Add( "qr.Enqueue( GuessObject( " +  iGuessObjectArg.ToString() + ") );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { qr.Enqueue( GuessObject( " +  iGuessObjectArg.ToString() + ") ); Console.WriteLine(\"Error\"); } catch ( " + ex.GetType().Name + " ) {}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    qm.Enqueue(objGuessArg1);
                    resArray.Add( "qm.Enqueue( GuessObject( " +  iGuessObjectArg.ToString() + ") );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { qm.Enqueue( GuessObject( " +  iGuessObjectArg.ToString() + ") ); Console.WriteLine(\"Error\"); } catch ( " + ex.GetType().Name + " ) {}" );
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
                    qr.Clear();
                    resArray.Add( "qr.Clear();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( " try { qr.Clear(); Console.WriteLine( \"ERROR\" );} catch (" + ex.GetType().Name + ") {}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    qm.Clear();
                    resArray.Add( "qm.Clear();" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( " try { qm.Clear(); Console.WriteLine( \"ERROR\" );} catch (" + ex.GetType().Name + ") {}" );
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
                    gotResultReal = qr.Count;
                    resArray.Add( "Console.WriteLine( qr.Count.ToString() );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { Console.WriteLine( qr.Count.ToString() ); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ){}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    gotResultFake = qm.Count;
                    resArray.Add( "Console.WriteLine( qm.Count.ToString() );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { Console.WriteLine( qm.Count.ToString() ); Console.WriteLine(\"ERROR\"); } catch ( " + ex.GetType().Name + " ){}" );
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
            if ( rand.Next(0, 10) <= 8 )        
            {
                Object[] gotResultReal = null;
                Object[] gotResultFake = null;
                Exception gotExceptionReal = null;
                Exception gotExceptionFake = null;
                try
                {
                    ++iCountTestcases;
                    gotResultReal = qr.ToArray();
                    resArray.Add( "Console.WriteLine( qr.ToArray().Length.ToString() );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { Console.WriteLine( qr.ToArray().Length.ToString() ); Console.WriteLine(\"ERROR\"); } catch (" + ex.GetType().Name + "){}" );
                    gotExceptionReal = ex;
                }
                try
                {
                    gotResultFake = qm.ToArray();
                    resArray.Add( "Console.WriteLine( qm.ToArray().Length.ToString() );" );
                }
                catch ( Exception ex )
                {
                    resArray.Add( "try { Console.WriteLine( qm.ToArray().Length.ToString() ); Console.WriteLine(\"ERROR\"); } catch (" + ex.GetType().Name + "){}" );
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
                    Console.WriteLine( "Exceptions do not match, real:" + strRealEx + "  mimic: " + strFakeEx );
                    return resArray;
                }
                if (  (gotResultReal == null) || (gotResultFake == null) || (gotResultReal.Length != gotResultFake.Length) )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Results do not match from runs Queue.ToArray().Length=>" + gotResultReal.Length.ToString() + ", Queue.ToArray().Length=>" + gotResultFake.Length.ToString() );
                    return resArray;
                }
                else 
                {
                    Boolean fComp = Common.CompareArrays( gotResultReal, gotResultFake, true );
                    if ( fComp == false )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_Comp001, Arrays Differ" );
                        return resArray;
                    }
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
        Console.WriteLine( "USAGE: Co1725allSigns_Queue_Copycat [option option ....]" );
        Console.WriteLine( "      options:" );
        Console.WriteLine( "               /s randomseed - the random seed we should use, randomseed must be an integer" );
        Console.WriteLine( "               /r runtime    - for how long you want to run for in ticks, runtime must be an integer, default=10000 == 10sec" );
        Console.WriteLine( "               /?            - prints usage and exits" );
    }
    public static void Main( String [] args )
    {
        Boolean bolResult = false;                                  
        Co1725allSigns_Queue_Copycat cbX = new Co1725allSigns_Queue_Copycat();
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
        if ( s_strActiveBugNums != null && s_strActiveBugNums.Length != 0 )
        {
            Console.WriteLine( "ACTIVE BUGS:  " + s_strActiveBugNums );
        }
        if ( bolResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    } 
} 
class QueueMimic
{
    private Object [] m_store;
    public QueueMimic()
    {
        m_store = new Object[0];
    }
    public int Count
    {
        get { return m_store.Length; }
    }
    public void Enqueue( Object obj )
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
    public Object Dequeue( )
    {
        if ( m_store.Length == 0 )
        {
            throw new InvalidOperationException( "QueueMimic, Dequeue" );
        }
        Object retObject = m_store[0];
        Object [] objTemp = new Object[m_store.Length - 1];
        for ( int i = 0; i < objTemp.Length; i++ )
        {
            objTemp[i] = m_store[i + 1];
        }
        m_store = objTemp;
        return retObject;
    }
    public Object Peek( )
    {
        if ( m_store.Length == 0 )
        {
            throw new InvalidOperationException( "QueueMimic, Peek" );
        }
        return m_store[0];
    }
    public Object [] ToArray()
    {
        Object [] objTemp = new Object[ m_store.Length ];
        for ( int i = 0; i < m_store.Length; i++ )
        {
            objTemp[ i ] = m_store[ i ];
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
