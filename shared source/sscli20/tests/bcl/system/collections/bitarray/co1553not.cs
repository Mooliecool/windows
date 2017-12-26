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
public class Co1553Not
{
    public virtual bool runTest()
    {
        System.Console.Error.WriteLine( "BitArray- Co1553Not runTest started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        BitArray ba2 = null;
        BitArray ba4 = null;
        System.Console.Out.WriteLine( "Standard cases (1,1) (1,0) (0,0)." );
        ba2 = new BitArray( 6 ,false );
        ba2.Set( 0 ,true );
        ba2.Set( 1 ,true );
        ba4 = ba2.Not(); 
        ++iCountTestcases;
        if ( ba2 != ba4 )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1553Not Error E_545wi!" );
        }
        for ( int aa=0 ;aa<ba2.Length ;aa++ )
        {
            if ( aa <= 1 )
            {
                if ( ba2.Get( aa ) )
                {
                    ++iCountErrors;
                    System.Console.Error.WriteLine( "Co1553Not Error E_837rt!" );
                }
            }
            else
            {
                if ( ! ba2.Get( aa ) )
                {
                    ++iCountErrors;
                    System.Console.Error.WriteLine( "Co1553Not Error E_859yb!" );
                }
            }
        }
        System.Console.Out.WriteLine( "Size stress cases." );
        ba2 = new BitArray( 0x1000F ,false );
        ba2.Set( 0 ,true );
        ba2.Set( 1 ,true );
        ba2.Set( 0x10000 ,true );
        ba2.Set( 0x10001 ,true );
        ba4 = ba2.Not();
        ++iCountTestcases;
        if ( ba4.Get(1) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1553Not Error E_067xs!" );
        }
        ++iCountTestcases;
        if ( ! ba4.Get(2) )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1553Not Error E_244tm!" );
        }
        for ( int aa=0x10000 ;aa<ba2.Length ;aa++ )
        {
            if ( aa <= 0x10001 )
            {
                if ( ba2.Get( aa ) )
                {
                    ++iCountErrors;
                    System.Console.Error.Write( "Co1553Not Error E_073wq!  aa==" );
                    System.Console.Error.WriteLine( aa );
                    break;
                }
            }
            else
            {
                if ( ! ba2.Get( aa ) )
                {
                    ++iCountErrors;
                    System.Console.Error.Write( "Co1553Not Error E_246ko!  aa==" );
                    System.Console.Error.WriteLine( aa );
                    break;
                }
            }
        }
        if ( iCountErrors == 0 )
        {
            System.Console.Error.Write( "BitArray- Co1553Not: paSs.  iCountTestcases==" );
            System.Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            System.Console.Error.Write( "Co1553Not iCountErrors==" );
            System.Console.Error.WriteLine( iCountErrors );
            System.Console.Error.WriteLine( "BitArray- Co1553Not: FAiL!" );
            return false;
        }
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        Co1553Not o2Co1553Not = new Co1553Not();
        try
        {
            bResult = o2Co1553Not.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine( "BitArray- Co1553Not main caught Exception!" );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
} 
