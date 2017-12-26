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
public class Co1554get_Length
{
    public virtual bool runTest()
    {
        System.Console.Error.WriteLine( "BitArray- Co1554get_Length runTest started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        int i2 = -2;
        BitArray ba2 = null;
        BitArray ba4 = null;
        i2 = 6;
        ba2 = new BitArray( i2 ,false );
        ++iCountTestcases;
        if ( ba2.Length != i2 )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1554get_Length Error E_545wi!" );
        }
        i2 = 0;
        ba2 = new BitArray( i2 ,false );
        ++iCountTestcases;
        if ( ba2.Length != i2 )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1554get_Length Error E_759oj!" );
        }
        i2 = -3;
        ++iCountTestcases;		
        try
        {
            ba2 = new BitArray( i2 ,false );
            System.Console.Error.WriteLine( "Co1554get_Length Error E_163fr!" );
        }
        catch ( System.ArgumentException exc ) {}
        i2 = 0x1000F;
        ba2 = new BitArray( i2 ,false );
        ++iCountTestcases;
        if ( ba2.Length != i2 )
        {
            ++iCountErrors;
            System.Console.Error.WriteLine( "Co1554get_Length Error E_608il!" );
        }
        if ( iCountErrors == 0 )
        {
            System.Console.Error.Write( "BitArray- Co1554get_Length: paSs.  iCountTestcases==" );
            System.Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            System.Console.Error.Write( "Co1554get_Length iCountErrors==" );
            System.Console.Error.WriteLine( iCountErrors );
            System.Console.Error.WriteLine( "BitArray- Co1554get_Length: FAiL!" );
            return false;
        }
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        try
        {
            Co1554get_Length o2Co1554get_Length = new Co1554get_Length();
            bResult = o2Co1554get_Length.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine( "BitArray- Co1554get_Length main caught Exception!" );
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
} 
