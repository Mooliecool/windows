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
public class Co1557SetAll
{
    public virtual bool runTest()
    {
        System.Console.Error.WriteLine( "BitArray- Co1557SetAll runTest started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        int i2 = -2;
        bool b2 = false;
        BitArray ba2 = null;
        BitArray ba4 = null;
        i2 = 6;
        ba2 = new BitArray( i2 ,false );
        ++iCountTestcases;
        if ( ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_545wi!" );
        }
        ++iCountTestcases;
        if ( ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_319hf!" );
        }
        ba2.SetAll( true );
        ++iCountTestcases;
        if ( ! ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_446hf!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_667aa!" );
        }
        i2 = 6;
        ba2 = new BitArray( i2 ,true );
        ++iCountTestcases;
        if ( ! ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_965dd!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_443sw!" );
        }
        ba2.SetAll( true );
        ++iCountTestcases;
        if ( ! ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_979uy!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_118ju!" );
        }
        i2 = 6;
        ba2 = new BitArray( i2 ,false );
        ++iCountTestcases;
        if ( ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_747tr!" );
        }
        ++iCountTestcases;
        if ( ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_235vc!" );
        }
        ba2.SetAll( false );
        ++iCountTestcases;
        if ( ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_005gh!" );
        }
        ++iCountTestcases;
        if ( ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_739wn!" );
        }
        i2 = 0x1000F;
        ba2 = new BitArray( i2 ,true );
        ++iCountTestcases;
        if ( ! ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_945wx!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_919hi!" );
        }
        ba2.SetAll( false );
        ++iCountTestcases;
        if ( ba2.Get( 0 ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_946ef!" );
        }
        ++iCountTestcases;
        if ( ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1557SetAll Error E_967ab!" );
        }
        if ( iCountErrors == 0 )
        {
            System.Console.Error.Write( "BitArray- Co1557SetAll: paSs.  iCountTestcases==" );
            System.Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            System.Console.Error.Write( "Co1557SetAll iCountErrors==" );
            System.Console.Error.WriteLine( iCountErrors );
            System.Console.Error.WriteLine( "BitArray- Co1557SetAll: FAiL!" );
            return false;
        }
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        try
        {
            Co1557SetAll o2Co1557SetAll = new Co1557SetAll();
            bResult = o2Co1557SetAll.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine( "BitArray- Co1557SetAll main caught Exception!" );
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
