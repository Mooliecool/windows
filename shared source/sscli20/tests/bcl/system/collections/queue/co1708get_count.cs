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
public class Co1708get_Count
{
    internal static System.String strName = "Queue.GetCount";
    internal static System.String strTest = "Co1708get_Count";
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
        System.String strCheckPoint = "CHECKPOINT";
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Out.Write( strName );
        Console.Out.Write( ": " );
        Console.Out.Write( strTest );
        Console.Out.WriteLine( " runTest started..." );
        Console.Error.Write( strDEBUGINFOEEVT );
        Console.Error.Write( "PATHTOSRC: $COM99/BVT/ClassLib/Test/vj/Queue/Co1708get_Count.cs" );
        Console.Error.WriteLine( strEMSG );
        Queue myQueue = null;
        int nDefaultCapacity = 32;
        float nDefaultGrowFactor = (float) 2.0;
        System.Console.Out.WriteLine( "Validate size of new Queue is zero" );
        iCountTestcases++;
        myQueue = new Queue();
        if ( myQueue == null )
        {
            debugPoint( "E_101" );
            Console.Error.WriteLine( "Invalid queue creation" );
            iCountErrors++;
            return false;
        }
        if ( myQueue.Count != 0 || myQueue.Count != 0 )
        {
            debugPoint( "E_202" );
            iCountErrors++;
            Console.Error.WriteLine( "Invalid queue creation" );
            bPass = false;
        }
        System.Console.Out.WriteLine( "GetCount objects onto Queue and get size" );
        for ( int ii = 0; ii < nDefaultCapacity; ++ii )
            myQueue.Enqueue( ii );
        iCountTestcases++;
        if ( myQueue.Count == 0 || myQueue.Count != nDefaultCapacity )
        {
            iCountErrors++;
            debugPoint( "E_303" );
            Console.Error.WriteLine( "Invalid count" );
            bPass = false;
        }
        System.Console.Out.WriteLine( "Dequeue objects from Queue and get size" );
        iCountTestcases++;
        for ( int ii = 0; ii < nDefaultCapacity / 2; ++ii )
            myQueue.Dequeue();
        if ( myQueue.Count == 0 || myQueue.Count != nDefaultCapacity / 2 )
        {
            iCountErrors++;
            debugPoint( "E_404" );
            Console.Error.WriteLine( "Count not equal to expected queue capacity" );
            bPass = false;
        }
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs. "+strTest+" ,iCountTestcases=="+iCountTestcases.ToString());
            return true;
        }
        else
        {
            Console.WriteLine("FAiL! "+strTest+" ,iCountErrors=="+iCountErrors.ToString());
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co1708get_Count oCbTest = new Co1708get_Count();
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
