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
public class Co1552Xor
{
    public virtual bool runTest()
    {
        System.Console.Error.WriteLine( "BitArray- Co1552Xor runTest started." );
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
        ba4 = ba2.Xor( ba3 );
        ++iCountTestcases;
        if ( ! ba4.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_572wb!" );
        }
        ++iCountTestcases;
        if ( ba4.Get( 1 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_733dl!" );
        }
        ++iCountTestcases;
        if ( ! ba4.Get( 2 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_678uf!" );
        }
        ++iCountTestcases;
        if ( ba4.Get( 4 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_628us!" );
        }
        ba2 = new BitArray( 0x1000F ,false );
        ba3 = new BitArray( 0x1000F ,false );
        ba2.Set( 0x10000 ,true ); 
        ba2.Set( 0x10001 ,true ); 
        ba3.Set( 0x10001 ,true ); 
        ba4 = ba2.Xor( ba3 );
        ++iCountTestcases;
        if ( ! ba4.Get( 0x10000 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_834at!" );
        }
        ++iCountTestcases;
        if ( ba4.Get( 0x10001 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_487ks!" );
        }
        ++iCountTestcases;
        if ( ba4.Get( 0x10002 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_312hr!" );
        }
        ++iCountTestcases;
        if ( ba4.Get( 0x10004 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_805mx!" );
        }
        ba2 = new BitArray( 11 ,false );
        ba3 = new BitArray( 6 ,false );
        try
        {
            ++iCountTestcases;
            ba4 = ba2.Xor( ba3 );
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_909sq!" );
        }
        catch ( System.ArgumentException exc ) {}
        ba2 = new BitArray( 6 ,false );
        ba3 = null;
        try
        {
            ++iCountTestcases;
            ba4 = ba2.Xor( ba3 );
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1552Xor Error E_151zb!" );
        }
        catch ( System.ArgumentException exc ) {}
        if ( iCountErrors == 0 )
        {
            System.Console.Error.Write( "BitArray- Co1552Xor: paSs.  iCountTestcases==" );
            System.Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            System.Console.Error.Write( "Co1552Xor iCountErrors==" );
            System.Console.Error.WriteLine( iCountErrors );
            System.Console.Error.WriteLine( "BitArray- Co1552Xor: FAiL!" );
            return false;
        }
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        try
        {
            Co1552Xor o2Co1552Xor = new Co1552Xor();
            bResult = o2Co1552Xor.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine( "BitArray- Co1552Xor main caught Exception!" );
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
