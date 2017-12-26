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
public class Co1704Enqueue
{
    internal static System.String strName = "Queue.Enqueue";
    internal static System.String strTest = "Co1704Enqueue";
    internal static System.String strEEVT = "\u0003";
    internal static System.String strEMSG = "\u001a";
    internal static System.String strDEBUGINFO =  "DEBUGINFO" ;
    internal static System.String strDEBUGINFOEEVT = strDEBUGINFO +  strEEVT ;
    internal static void debugPoint( String strPoint )
    {
        Console.Error.Write( strDEBUGINFOEEVT );
        Console.Error.Write( "POINTTOBREAK: find " );
        Console.Error.Write( strPoint );
        Console.Error.Write( " (" );
        Console.Error.Write( strTest );
        Console.Error.Write( ".cs)" );
        Console.Error.WriteLine( strEMSG );
    }
    public virtual bool runTest()
    {
        bool bPass = true;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        System.String strCheckPoint = "CHECKPOINT";
        Console.Out.Write( strName );
        Console.Out.Write( ": " );
        Console.Out.Write( strTest );
        Console.Out.WriteLine( " runTest started..." );
        Console.Error.Write( strDEBUGINFOEEVT );
        Console.Error.Write( "PATHTOSRC: $COM99/BVT/ClassLib/Test/vj/Queue/Co1704Enqueue.cs" );
        Console.Error.WriteLine( strEMSG );
        Queue myQueue = null;
        System.String strResult = "Queue.Enqueue: Co1704Enqueue iCountErrors==";
        System.Console.Out.WriteLine( "Enqueue objects into the queue" );
        myQueue = new Queue();
        iCountTestcases++;
        if ( myQueue == null || myQueue.Count != 0 )
        {
            debugPoint( "E_101" );
            Console.Error.WriteLine( "Invalid queue creation" );
            iCountErrors++;
            return false;
        }
        System.Console.Out.WriteLine( "Enqueue 1000 integers onto the queue" );
        for ( int ii = 1; ii <= 1000; ++ii )
        {
            iCountTestcases++;
            myQueue.Enqueue( ii );
            if ( myQueue.Count != ii )
            {
                debugPoint( "E_202" );
                bPass = false;
                iCountErrors++;
                break;
            }
        }
        System.Console.Out.WriteLine( "Verify that 1000 object are on the stack" );
        iCountTestcases++;
        if ( myQueue.Count != 1000 )
        {
            debugPoint( "E_303" );
            Console.Error.WriteLine( "Count not verified as 1000" );
            iCountErrors++;
            bPass = false;
        }
        iCountTestcases++;
        System.Console.Out.WriteLine( "Enqueue null object into the stack" );
        try
        {
            myQueue.Enqueue( null );
        }
        catch ( Exception ex )
        {
            Console.Error.WriteLine( "Err_404,  noexception expected but got "+ ex.ToString() );
            bPass = false;
            iCountErrors++;
        }
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( bPass == true )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine( " paSs, iCountTestcases==" + iCountTestcases);
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
        Co1704Enqueue oCbTest = new Co1704Enqueue();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( System.Exception ex )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine(  ": Main() Uncaught exception" );
            Console.Error.WriteLine( ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
