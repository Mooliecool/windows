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
public class Co1556Set
{
    public virtual bool runTest()
    {
        System.Console.Error.WriteLine( "BitArray- Co1556Set runTest started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        int i2 = -2;
        bool b2 = false;
        BitArray ba2 = null;
        BitArray ba4 = null;
        i2 = 6;
        ba2 = new BitArray( i2 ,true );
        ba2.Set( 0 ,true );
        ba2.Set( 2 ,true );
        ba2.Set( 5 ,true );
        ++iCountTestcases;
        if ( ! ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_794fn!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( 1 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_200rp!" );
        }
        i2 = 6;
        ba2 = new BitArray( i2 ,false );
        ba2.Set( 0 ,true );
        ba2.Set( 1 ,false );
        ba2.Set( 2 ,true );
        ba2.Set( 5 ,true );
        ++iCountTestcases;
        if ( ! ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_545wi!" );
        }
        ++iCountTestcases;
        if ( ba2.Get( 1 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_319hf!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( 2 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_063na!" );
        }
        ++iCountTestcases;
        if ( ba2.Get( 3 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_624cd!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( 5 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_446xt!" );
        }
        try
        {
            ++iCountTestcases;
            ba2.Set( -3, false );
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_876jv!" );
        }
        catch ( ArgumentException exc ) {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.Error.WriteLine( "Co1556Set Error E_643aw!" );
        }
        try
        {
            ++iCountTestcases;
            ba2.Set( (i2 + 3), false );
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_748io!" );
        }
        catch ( ArgumentException exc ) {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.Error.WriteLine( "Co1556Set Error E_052be!" );
        }
        try
        {
            ++iCountTestcases;
            ba2.Set( i2, false );
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_748io!" );
        }
        catch ( ArgumentException exc ) {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.Error.WriteLine( "Co1556Set Error E_052be!" );
        }
        i2 = 0x1000F;
        ba2 = new BitArray( i2 ,false );
        ba2.Set( 0 ,true );
        ba2.Set( 2 ,true );
        ba2.Set( 0x10001 ,true );
        ++iCountTestcases;
        if ( ! ba2.Get( 2 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_333yy!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( 0x10001 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1556Set Error E_088ht!" );
        }
        if ( iCountErrors == 0 )
        {
            System.Console.Error.Write( "BitArray- Co1556Set: paSs.  iCountTestcases==" );
            System.Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            System.Console.Error.Write( "Co1556Set iCountErrors==" );
            System.Console.Error.WriteLine( iCountErrors );
            System.Console.Error.WriteLine( "BitArray- Co1556Set: FAiL!" );
            return false;
        }
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        try
        {
            Co1556Set o2Co1556Set = new Co1556Set();
            bResult = o2Co1556Set.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine( "BitArray- Co1556Set main caught Exception!" );
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
