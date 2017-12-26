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
public class Co4309ctor_int_float
{
    internal static String strName = "Hashtable.Contructor";
    internal static String strTest = "Co4309ctor_int_float";
    internal static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Out.Write( strPath );
        Console.Out.Write( strTest );
        Console.Out.Write( ".cs" );
        Console.Out.WriteLine( " runTest() started..." );
        Hashtable hash = null;
        int nCapacity = 100;
        float fltLoadFactor = (float) .5;
        do
        {
            iCountTestcases++;
            hash = new Hashtable( nCapacity, fltLoadFactor );
            if ( hash == null )
            {
                Console.WriteLine( strTest+ "E_101" );
                Console.WriteLine( strTest+ "Hashtable creation failure" );
                ++iCountErrors;
                break;
            }
            iCountTestcases++;
            if ( hash.Count != 0 )
            {
                Console.WriteLine( strTest+ "E_202" );
                Console.WriteLine( strTest+ "New hash table is not empty" );
                ++iCountErrors;
            }
            iCountTestcases++;
            Console.Out.WriteLine( "Create Hashtable using a invalid capacity and valid load factor" );
            try
            {
                hash = new Hashtable( -1, fltLoadFactor );
                Console.WriteLine( strTest+ "E_403" );
                Console.WriteLine( strTest+ "Expected ArgumentException" );
                ++iCountErrors;
            }
            catch( ArgumentOutOfRangeException aex ) {}
            catch( Exception ex )
            {
                Console.WriteLine( strTest+ "E_404" );
                Console.WriteLine( strTest+ "Generic exception caught" );
                Console.WriteLine( strTest+ ex.ToString() );
                ++iCountErrors;
            }
            iCountTestcases++;
            Console.Out.WriteLine( "Create Hashtable using a valid capacity and invalid load factor" );
            try
            {
                hash = new Hashtable( nCapacity, .09f ); 
                Console.WriteLine( strTest+ "E_605" );
                Console.WriteLine( strTest+ "Expected ArgumentOutOfRangeException" );
                ++iCountErrors;
            }
            catch( ArgumentOutOfRangeException aex ) {}
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_606" );
                Console.WriteLine( strTest+ "Generic exception caught" );
                Console.WriteLine( strTest+ ex.ToString() );
                ++iCountErrors;
            }
            iCountTestcases++;
            try
            {
                hash = new Hashtable( nCapacity, 1.1f );
                Console.WriteLine( strTest+ "E_707" );
                Console.WriteLine( strTest+ "Expected ArgumentOutOfRangeException" );
                ++iCountErrors;
            }
            catch( ArgumentOutOfRangeException ex )
            {}
            catch( Exception ex )
            {
                Console.WriteLine( strTest+ "E_808" );
                Console.WriteLine( strTest+ "Generic exception caught" );
                Console.WriteLine( strTest+ ex.ToString() );
                ++iCountErrors;
            }
            iCountTestcases++;
            try
            {
                hash = new Hashtable( -1, -1f );
                Console.WriteLine( strTest+ "E_707" );
                Console.WriteLine( strTest+ "Expected ArgumentOutOfRangeException" );
                ++iCountErrors;
            }
            catch( ArgumentOutOfRangeException ex )
            {}
            catch( Exception ex )
            {
                Console.WriteLine( strTest+ "E_808" );
                Console.WriteLine( strTest+ "Generic exception caught" );
                Console.WriteLine( strTest+ ex.ToString() );
                ++iCountErrors;
            }
            iCountTestcases++;
            try 
            {
                hash = new Hashtable((int)100000000, .5f);
            }
            catch (OutOfMemoryException mexc) 
            {
                Console.Out.WriteLine( "OutOfMemoryException caught" );
            }
        }
        while (false);
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine( " paSs, iCountTestcases==" + iCountTestcases);
            return true;
        }
        else
        {
            System.String strFailMsg = null;
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "FAiL" );
            Console.Error.Write( strTest );
            Console.Error.Write( " iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            return false;
        }
    }
    public static void Main(System.String[] args)
    {
        bool bResult = false;	
        Co4309ctor_int_float oCbTest = new Co4309ctor_int_float();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "E_1000000" );
            Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( strTest+ ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
} 
