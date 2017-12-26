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
public class Co1551Or
{
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co1551Or.cs  runTest() started." );
        String strWrite = null;
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
        ba4 = ba2.Or( ba3 );
        ++iCountTestcases;
        if ( ! ba4.Get( 0 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_936hf (Co1551Or.cs)" );
        }
        ++iCountTestcases;
        if ( ! ba4.Get( 1 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_894fc (Co1551Or.cs)" );
        }
        ++iCountTestcases;
        if ( ! ba4.Get( 2 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_755jd (Co1551Or.cs)" );
        }
        ++iCountTestcases;
        if ( ba4.Get( 4 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_667sz (Co1551Or.cs)" );
        }
        ba2 = new BitArray( 0x1000F ,false );
        ba3 = new BitArray( 0x1000F ,false );
        ba2.Set( 0x10000 ,true ); 
        ba2.Set( 0x10001 ,true ); 
        ba3.Set( 0x10001 ,true ); 
        ba4 = ba2.Or( ba3 );
        ++iCountTestcases;
        if ( ! ba4.Get( 0x10000 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_534xu (Co1551Or.cs)" );
        }
        ++iCountTestcases;
        if ( ! ba4.Get( 0x10001 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_400ki (Co1551Or.cs)" );
        }
        ++iCountTestcases;
        if ( ba4.Get( 0x10002 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_357nl (Co1551Or.cs)" );
        }
        ba2 = new BitArray( 11 ,false );
        ba3 = new BitArray( 6 ,false );
        try
        {
            ++iCountTestcases;
            ba4 = ba2.Or( ba3 );
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_289wx (Co1551Or.cs)" );
        }
        catch ( ArgumentException exc ) {}
        ba2 = new BitArray( 6 ,false );
        ba3 = null;
        try
        {
            ++iCountTestcases;
            ba4 = ba2.Or( ba3 );
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_112gy (Co1551Or.cs)" );
        }
        catch ( ArgumentException exc ) {}
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "BitArray\\Co1551Or.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.Write( "Co1551Or iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "BitArray\\Co1551Or.cs: FAiL!" );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblW = null;
        try
        {
            Co1551Or o2Co1551Or = new Co1551Or();
            bResult = o2Co1551Or.runTest();
        }
        catch ( Exception exc )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_999zz (Co1551Or.cs)" );
            sblW = new StringBuilder( "EXTENDEDINFO: (E_999zz ,Co1551Or)  exc.ToString()==" );
            sblW.Append( exc.ToString() );
            Console.Error.WriteLine(  sblW.ToString()  );
        }
        if ( ! bResult )
        {
            Console.Error.WriteLine(  "Co1551Or.cs"  );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
