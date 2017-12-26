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
using System.Threading;
using System.Collections;
class Co6078Synchronized_Queue 
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer = "";
    public static String s_strComponentBeingTested = "Queue.Synchronized( Queue )";
    public static String s_strTFName = "Co6078Synchronized_Queue.cs";
    public static String s_strTFAbbrev = "Co6078";
    public static String s_strTFPath = "";
    public Boolean verbose = true;
    public Queue m_Queue;
    public Int32 iCountTestcases = 0;
    public Int32 iCountErrors = 0;
    public Int32 m_ThreadsToUse = 8;
    public Int32 m_ThreadAge = 5; 
    public Int32 m_ThreadCount;
    public void StartEnThread() 
    {
        Int32 t_age = m_ThreadAge;
        while( t_age > 0 ) 
        {      
            try 
            {
                m_Queue.Enqueue( t_age );
                iCountTestcases++;
                if ( verbose ) Console.Write( "a" );
            } 
            catch ( Exception e ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_EnThread : " + e.GetType().FullName );
            }
            Interlocked.Decrement(ref t_age);
        }
        Interlocked.Decrement(ref m_ThreadCount);
    }
    public void StartDeThread() 
    {
        Int32 t_age = m_ThreadAge;
        while( t_age > 0 ) 
        {      
            try 
            {
                m_Queue.Dequeue();
                iCountTestcases++;
                if ( verbose ) Console.Write( "b" );
            } 
            catch ( Exception e ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_DeThread : " + e.GetType().FullName );
            }
            Interlocked.Decrement(ref t_age);
        }
        Interlocked.Decrement(ref m_ThreadCount);
    }
    public void StartDeEnThread() 
    {
        Int32 t_age = m_ThreadAge;
        while( t_age> 0 ) 
        {      
            try 
            {
                m_Queue.Dequeue();
                m_Queue.Enqueue( "DeEn" );
                iCountTestcases++;
                if ( verbose ) Console.Write( "c" );
            } 
            catch ( Exception e ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_DeEnThread : " + e.GetType().FullName );
            }
            Interlocked.Decrement(ref t_age);
        }
        Interlocked.Decrement(ref m_ThreadCount);
    }
    public void StartEnDeThread() 
    {
        Int32 t_age = m_ThreadAge;
        while( t_age > 0) 
        {      
            try 
            {
                m_Queue.Dequeue();
                m_Queue.Enqueue( "EnDe" );
                iCountTestcases++;
                if ( verbose ) Console.Write( "d" );
            } 
            catch ( Exception e ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_EnDeThread : " + e.GetType().FullName );
            }
            Interlocked.Decrement(ref t_age);
        }
        Interlocked.Decrement(ref m_ThreadCount);
    }
    public Boolean runTest() 
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        String strLoc = "Loc_000ooo";
        Queue q1;
        Queue q2;
        Queue q3;
        Boolean fPass;
        String[] strArr;
        Object[] oArr;
        IEnumerator ienm1;
        Object oValue;
        Int32 iCount;
        if ( verbose ) Console.WriteLine( "Testing Method: Queue.Synchronized( Queue )" );
        try 
        {
            String[] expectedExceptions = {
                                              "System.ArgumentNullException",
            };
            Queue[]   errorValues = {
                                        null,
            };
            for( int i = 0; i < expectedExceptions.Length; i++ ) 
            {
                iCountTestcases++;
                try 
                {
                    if( verbose ) Console.WriteLine( " Exception Testing: " + expectedExceptions[i] );
                    Queue result = Queue.Synchronized( errorValues[i] );
                    iCountErrors++;
                    strLoc = "Err_NE1," + i;
                    Console.WriteLine( strLoc + " Exception not Thrown!" );
                } 
                catch( Exception e ) 
                {
                    if ( !e.GetType().FullName.Equals( expectedExceptions[i] ) ) 
                    {
                        iCountErrors++;
                        strLoc = "Err_WE1," + i;
                        Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
                    }
                }
            }
        } 
        catch( Exception e ) 
        {
            Console.WriteLine( "Uncaught Exception in Queue Queue.Synchronized( Queue )");
            Console.WriteLine( "Exception->" + e.GetType().FullName );
        }
        m_Queue = new Queue();
        m_Queue = Queue.Synchronized( m_Queue );
        Thread[] ths = new Thread[m_ThreadsToUse];
        if ( verbose ) Console.WriteLine( m_Queue.Count );
        for ( int i = 0; i < m_ThreadsToUse; i++ ) 
        {
            ths[i] = new Thread( new ThreadStart( this.StartEnThread ) );
        }
        try 
        {
            if ( verbose ) Console.WriteLine( "Beginning threads now..." );
            m_ThreadCount = m_ThreadsToUse;
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Start();
            }
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Join();
            }
            Int32 expected = m_ThreadsToUse * m_ThreadAge;
            Int32 result = m_Queue.Count;
            if ( !expected.Equals( result ) ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_ENQUEUE Race.  Expected = " + expected + " Result = " + result );
            }
        } 
        catch ( Exception e ) 
        {
            iCountErrors++;
            Console.WriteLine( "Err_THREAD threw " + e.GetType().FullName );
        }
        if ( verbose ) Console.WriteLine( m_Queue.Count );
        m_ThreadsToUse = (m_ThreadsToUse - 2);     
        for ( int i = 0; i < m_ThreadsToUse; i++ ) 
        {
            ths[i] = new Thread( new ThreadStart( this.StartDeThread ) );
        }
        try 
        {
            if ( verbose ) Console.WriteLine( "Beginning threads now..." );
            m_ThreadCount = m_ThreadsToUse;
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Start();
            }
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Join();
            }
            Int32 expected = 2 * m_ThreadAge;
            Int32 result = m_Queue.Count;
            if ( !expected.Equals( result ) ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_DEQUEUE Race.  Expected = " + expected + " Result = " + result );
            }
        } 
        catch ( Exception e ) 
        {
            iCountErrors++;
            Console.WriteLine( "Err_THREAD threw " + e.GetType().FullName );
        }
        if ( verbose ) Console.WriteLine( m_Queue.Count );
        m_ThreadsToUse = (m_ThreadsToUse + 2);     
        for ( int i = 0; i < m_ThreadsToUse; i++ ) 
        {
            ths[i] = new Thread( new ThreadStart( this.StartDeEnThread ) );
        }
        try 
        {
            if ( verbose ) Console.WriteLine( "Beginning threads now..." );
            m_ThreadCount = m_ThreadsToUse;
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Start();
            }
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Join();
            }
            Int32 expected = 2 * m_ThreadAge;
            Int32 result = m_Queue.Count;
            if ( !expected.Equals( result ) ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_DEQUEUE Race.  Expected = " + expected + " Result = " + result );
            }
        } 
        catch ( Exception e ) 
        {
            iCountErrors++;
            Console.WriteLine( "Err_THREAD threw " + e.GetType().FullName );
        }
        if ( verbose ) Console.WriteLine( m_Queue.Count );
        for ( int i = 0; i < m_ThreadsToUse; i++ ) 
        {
            ths[i] = new Thread( new ThreadStart( this.StartEnDeThread ) );
        }
        try 
        {
            if ( verbose ) Console.WriteLine( "Beginning threads now..." );
            m_ThreadCount = m_ThreadsToUse;
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Start();
            }
            for ( int i = 0; i < m_ThreadsToUse; i++ ) 
            {
                ths[i].Join();
            }
            Int32 expected = 2 * m_ThreadAge;
            Int32 result = m_Queue.Count;
            if ( !expected.Equals( result ) ) 
            {
                iCountErrors++;
                Console.WriteLine( "Err_DEQUEUE Race.  Expected = " + expected + " Result = " + result );
            }
        } 
        catch ( Exception e ) 
        {
            iCountErrors++;
            Console.WriteLine( "Err_THREAD threw " + e.GetType().FullName );
        }
        strLoc = "Loc_452dsfg";
        iCountTestcases++;
        fPass=true;
        q1 = new Queue();
        for(int i=0; i<10; i++)
            q1.Enqueue("String_" + i);
        q2 = Queue.Synchronized(q1);
        if(q2.Count != q1.Count)
            fPass=false;
        q2.Clear();
        if(q2.Count != 0)
            fPass=false;
        for(int i=0; i<10; i++)
            q2.Enqueue("String_" + i);
        for(int i=0; i<10; i++)
        {
            if(!((String)q2.Peek()).Equals("String_" + i))
            {
                Console.WriteLine(q2.Peek());
                fPass=false;
            }
            q2.Dequeue();
        }
        if(q2.Count != 0)
            fPass=false;
        if(!q2.IsSynchronized)
            fPass=false;
        for(int i=0; i<10; i++)
            q2.Enqueue("String_" + i);
        q3 = Queue.Synchronized(q2);
        if(!q3.IsSynchronized || (q2.Count!=q3.Count))
            fPass = false;
        strArr = new String[10];
        q2.CopyTo(strArr, 0);
        for(int i=0; i<10; i++)
        {
            if(!strArr[i].Equals("String_" + i))
                fPass = false;
        }
        strArr = new String[10 + 10];
        q2.CopyTo(strArr, 10);
        for(int i=0; i<10; i++)
        {
            if(!strArr[i+10].Equals("String_" + i))
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
        for(int i=0; i<10; i++)
        {
            if(!((String)oArr [i]).Equals("String_" + i))
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
        iCount = 0;
        while(ienm1.MoveNext())
        {
            if(!((String)ienm1.Current).Equals("String_" + iCount))
            {
                fPass=false;
            }
            iCount++;
        }
        ienm1.Reset();
        iCount = 0;
        while(ienm1.MoveNext())
        {
            if(!((String)ienm1.Current).Equals("String_" + iCount))
            {
                fPass=false;
            }
            iCount++;
        }
        ienm1.Reset();
        q2.Dequeue();
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
            iCountErrors++;
            Console.WriteLine( "Unexpect result returned!" );
        }
        Console.Error.Write( s_strTFName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 ) 
        {
            Console.Error.WriteLine( " inCountTestcases==" + iCountTestcases + " paSs" );
            return true;
        } 
        else 
        {
            Console.Error.WriteLine( s_strTFPath + s_strTFName + ".cs" );
            Console.Error.WriteLine( " inCountTestcases==" + iCountTestcases );
            Console.Error.WriteLine( "FAiL" );
            Console.Error.WriteLine( " inCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args ) 
    {
        Boolean bResult = false; 
        Co6078Synchronized_Queue cbX = new Co6078Synchronized_Queue();
        try { if ( args[0].Equals( "-v" ) ) cbX.verbose = true; } 
        catch( Exception ) {}
        try 
        {
            bResult = cbX.runTest();
        } 
        catch ( Exception exc_main ) 
        {
            bResult = false;
            Console.WriteLine( "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
        }
        if ( ! bResult ) 
        {
            Console.WriteLine( s_strTFPath + s_strTFName );
            Console.Error.WriteLine( " " );
            Console.Error.WriteLine( "Try '" + s_strTFName + ".exe -v' to see tests..." );
            Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
            Console.Error.WriteLine( " " );
        }
        if ( bResult == true ) Environment.ExitCode = 0;
        else Environment.ExitCode = 1; 
    }
}  
