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
public class Co1702Dequeue
{
    internal static String strName = "Queue.Dequeue";
    internal static String strTest = "Co1702Dequeue";
    internal static String strEEVT = "\u0003";
    internal static String strEMSG = "\u001a";
    internal static String strDEBUGINFO =  "DEBUGINFO" ;
    internal static String strDEBUGINFOEEVT = strDEBUGINFO +  strEEVT ;
    internal static void debugPoint( String strPoint )
    {
        Console.Error.Write( strDEBUGINFOEEVT );
        Console.Error.Write( "POINTTOBREAK: find " );
        Console.Error.Write( strPoint );
        Console.Error.Write( " (" );
        Console.Error.Write( strTest );
        Console.Error.WriteLine( ".cs)" );
    }
    public virtual bool runTest()
    {
        bool bPass = true;
        int iCountTestcases = 0;
        int iCountErrors = 0;
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        Console.Error.Write( strTest );
        Console.Error.WriteLine( " runTest started..." );
        Console.Error.Write( strDEBUGINFOEEVT );
        Console.Error.WriteLine( "PATHTOSRC: $COM99/BVT/ClassLib/Test/vj/Queue/Co1702Dequeue.cs" );
        Queue myQueue = null;
        Console.Error.WriteLine( "Dequeue an object / Dequeue an object" );
        myQueue = new Queue();
        iCountTestcases++;
        if ( myQueue == null || myQueue.Count != 0 )
        {
            debugPoint( "E_101" );
            Console.Error.WriteLine( "Invalid queue creation" );
            iCountErrors++;
            return false;
        }
        Console.Error.WriteLine( "Dequeue 1000 objects / Dequeue 1000 objects" );
        for ( int ii = 1; ii <= 1000; ++ii )
        {
            iCountTestcases++;
            myQueue.Enqueue( (Int32)( ii ) );
            Int32 myInt = (Int32)myQueue.Dequeue();
            if ( myQueue.Count != 0 || (int)myInt != ii )
            {
                debugPoint( "E_202" );
                Console.Error.WriteLine( "Dequeue failed verification" );
                bPass = false;
                iCountErrors++;
                break;
            }
        }
        Console.Error.WriteLine( "Dequeue 1000 object / Dequeue and verify objects" );
        for ( int ii = 1; ii <= 1000; ++ii )
            myQueue.Enqueue( (Int32)( ii ) );
        iCountTestcases++;
        if ( myQueue.Count != 1000 )
        {
            debugPoint( "E_303" );
            Console.Error.WriteLine( "Queue count not equal to 1000" );
            iCountErrors++;
            bPass = false;
        }
        for ( int ii = 1; ii <= 1000; ++ii )
        {
            iCountTestcases++;
            Int32 myInt = (Int32)myQueue.Dequeue();
            if ( (int)( myInt ) != ii )
            {
                debugPoint( "E_404" );
                iCountErrors++;
                bPass = false;
                break;
            }
        }
        Console.Error.WriteLine( "Verify dequeue on an empty collection" );
        iCountTestcases++;
        try
        {
            while ( myQueue.Count != 0 )
                myQueue.Dequeue();
            myQueue.Dequeue();
            Console.WriteLine( "Should have thrown InvalidOperationException like it was in spec" );
            iCountErrors++;
            bPass = false;
        }
        catch ( InvalidOperationException)
        {}
        catch ( Exception ex )
        {
            Console.Error.Write( ex.ToString() );
            Console.Error.WriteLine(" unexpected exception" );
            iCountErrors++;
            bPass = false;
        }
        try
        {
            while ( myQueue.Count != 0 )
                myQueue.Dequeue();
            iCountTestcases++;
            myQueue.Dequeue();
            debugPoint( "E_404" );
            iCountErrors++;
            bPass = false;
        }
        catch(InvalidOperationException){}
        catch ( Exception ex )
        {
            debugPoint( "E_404" );
            bPass = false;
            iCountErrors++;
            Console.Error.Write( ex.ToString() );
            Console.Error.WriteLine(" caught" );
        }
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( bPass == true )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine( " paSs, iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine( " FAiL, iCountErrors==" + iCountErrors);
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co1702Dequeue oCbTest = new Co1702Dequeue();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine(  ": Main() Uncaught exception" );
            Console.Error.WriteLine( ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0;
        else Environment.ExitCode = 1;
    }
}
