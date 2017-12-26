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
using System.Text;
using System;
using System.Collections;
public class Co1550And
{
    public virtual bool runTest()
    {
        Console.Out.WriteLine( "Plain\\System_NewCollections\\BitArray\\Co1550And.cs  runTest() started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        BitArray ba2 = null;
        BitArray ba3 = null;
        BitArray ba4 = null;
        ba2 = new BitArray( 6 ,false );
        ba3 = new BitArray( 6 ,false );
        ba2.Set( 0 ,true ); 
        ba2.Set( 1 ,true ); 
        ba3.Set( 1 ,true ); 
        ba3.Set( 2 ,true ); 
        ba4 = ba2.And( ba3 );
        ++iCountTestcases;
        if ( ba4.Get( 0 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_09pv (Co1550And)"  );
            Console.Error.WriteLine(  "EXTENDEDINFO: 1 & 0 (E_09pv ,Co1550And)"  );
        }
        ++iCountTestcases;
        if ( ! ba4.Get( 1 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_96hf (Co1550And)"  );
        }
        ++iCountTestcases;
        if ( ba4.Get( 2 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_84si (Co1550And)"  );
        }
        ++iCountTestcases;
        if ( ba4.Get( 4 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_79kn (Co1550And)"  );
        }
        ba2 = new BitArray( 0x1000F ,false );
        ba3 = new BitArray( 0x1000F ,false );
        ba2.Set( 0x10000 ,true );
        ba2.Set( 0x10001 ,true );
        ba3.Set( 0x10001 ,true );
        ba3.Set( 0x10002 ,true );
        ba4 = ba2.And( ba3 );
        ++iCountTestcases;
        if ( ba4.Get( 0x10000 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_61ah (Co1550And)"  );
        }
        ++iCountTestcases;
        if ( ! ba4.Get( 0x10001 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_52xd (Co1550And)"  );
        }
        ++iCountTestcases;
        if ( ba4.Get( 0x10002 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_47op (Co1550And)"  );
        }
        ++iCountTestcases;
        if ( ba4.Get( 0x10004 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_54ja (Co1550And)"  );
        }
        try
        {
            BitArray b1 = new BitArray( 0 );
            BitArray b2 = new BitArray( 0 );
            b1.And( b2 );
        }
        catch ( Exception exc )
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001, Exception was not expected " + exc );
        }
        ba2 = new BitArray( 11 ,false );
        ba3 = new BitArray( 6 ,false );
        try
        {
            ++iCountTestcases;
            ba4 = ba2.And( ba3 );
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_66vc (Co1550And)"  );
        }
        catch ( ArgumentException ) {}
        catch ( Exception )  
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_58jq (Co1550And)"  );
        }
        ba2 = new BitArray( 6 ,false );
        ba3 = new BitArray( 11 ,false );
        try
        {
            ++iCountTestcases;
            ba4 = ba2.And( ba3 );
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_432k (Co1550And)"  );
        }
        catch ( ArgumentException ) {}
        catch ( Exception )  
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_452a (Co1550And)"  );
        }
        ba2 = new BitArray( 6 ,false );
        ba3 = null;
        try
        {
            ++iCountTestcases;
            ba4 = ba2.And( ba3 );
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_72dg (Co1550And)"  );
        }
        catch ( ArgumentNullException ) {}
        catch ( Exception )  
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find error E_93kx (Co1550And)"  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "BitArray\\Co1550And.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.Write( "Co1550And.cs iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine(  "PATHTOSOURCE: BitArray\\Co1550And.cs   FAiL !"  );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblW = null;
        Co1550And cbA = new Co1550And();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc )
        {
            bResult = false;
            Console.Error.WriteLine(  "EXTENDEDINFO: FAiL!  Find E_999zzz, Uncaught Exception caught in main()! Co1550And.cs"  );
            sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
            sblW.Append( exc.ToString() );
            Console.Error.WriteLine(  sblW.ToString()  );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
