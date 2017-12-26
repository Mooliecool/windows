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
public class Co1700ctor_int_float
{
    internal static System.String strName = "Queue.constructor";
    internal static System.String strTest = "Co1700ctor_int_float";
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
        Console.Out.Write( strName );
        Console.Out.Write( ": " );
        Console.Out.Write( strTest );
        Console.Out.WriteLine( " runTest started..." );
        Console.Error.Write( strDEBUGINFOEEVT );
        Console.Error.Write( "PATHTOSRC: $COM99/BVT/ClassLib/Test/vj/Queue/Co1700ctor_int_float.cs" );
        Console.Error.WriteLine( strEMSG );
        Queue myQueue = null;
        int nDefaultCapacity = 32;
        float nDefaultGrowFactor = (float) 2.0;
        iCountTestcases++;
        System.Console.Out.WriteLine( "Construct queue using specified defaults" );
        myQueue = new Queue( nDefaultCapacity, nDefaultGrowFactor );
        if ( myQueue == null )
        {
            iCountErrors++;
            debugPoint( "E_101" );
            Console.Error.WriteLine( "Invalid queue creation" );
            return false;
        }
        iCountTestcases++;
        System.Console.Out.WriteLine( "Construct queue using invalid parameters" );
        try
        {
            myQueue = new Queue( 0, (float) 0.0 );
            debugPoint( "E_202" );
            iCountErrors++;
            Console.Error.WriteLine( "Error: Microsoft.Runtime.ArgumentException expected" );
        }
        catch( System.ArgumentException ex )
        {
            System.Console.Out.WriteLine( "ArgumentException caught" );
        }
        iCountTestcases++;
        try
        {
            myQueue = new Queue( -1, nDefaultGrowFactor );
            debugPoint( "E_303" );
            iCountErrors++;
            Console.Error.WriteLine( "Error: Microsoft.Runtime.ArgumentException expected" );
        }
        catch( System.ArgumentException ex )
        {
            System.Console.Out.WriteLine( "ArgumentException caught" );
        }
        iCountTestcases++;
        try
        {
            myQueue = new Queue( nDefaultCapacity, (float) 30.0 );
            debugPoint( "E_404" );
            iCountErrors++;
            Console.Error.WriteLine( "Error: Microsoft.Runtime.ArgumentException expected" );
        }
        catch( System.ArgumentException ex )
        {
            System.Console.Out.WriteLine( "ArgumentException caught" );
        }
        iCountTestcases++;
        try
        {
            myQueue = new Queue( nDefaultCapacity, (float) -30.0 );
            debugPoint( "E_404" );
            iCountErrors++;
            Console.Error.WriteLine( "Error: Microsoft.Runtime.ArgumentException expected" );
        }
        catch( System.ArgumentException ex )
        {
            System.Console.Out.WriteLine( "ArgumentException caught" );
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
        Co1700ctor_int_float oCbTest = new Co1700ctor_int_float();
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
