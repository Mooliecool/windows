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
public class Co1558set_Length
{
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co1558set_Length.cs  runTest() started." );
        String strWrite = null;
        int iCountErrors = 0;
        int iCountTestcases = 0;
        int i2 = -2;
        bool b2 = false;
        BitArray ba2 = null;
        BitArray ba4 = null;
        i2 = 16;
        ba2 = new BitArray( i2 ,true );
        ba2.Length = ( (i2 * 3) );
        ++iCountTestcases;
        if ( ba2.Get( (i2 * 2) ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_451fr (Co1558set_Length.cs)"  );
        }
        i2 = 16;
        ba2 = new BitArray( i2 ,true );
        ba2.Length = ( (i2 / 2) );
        try
        {
            ++iCountTestcases;
            ba2.Get( (i2 * 2) );
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_132re (Co1558set_Length.cs)"  );
        }
        catch ( ArgumentException exc ) {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.Error.WriteLine( "Error E_643aw!" );
        }
        ++iCountTestcases;
        if ( ! ba2.Get( (i2 / 2) - 2 ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_427dc (Co1558set_Length.cs)"  );
        }
        i2 = 16;
        ba2 = new BitArray( i2 ,true );
        ba2.Length = ( 0 );
        try
        {
            ++iCountTestcases;
            ba2.Get( 0 );
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_208ux (Co1558set_Length.cs)"  );
        }
        catch ( ArgumentException exc ) {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.Error.WriteLine( "Error E_763sm!" );
        }
        try
        {
            ++iCountTestcases;
            ba2.Length = -5;
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_208ux (Co1558set_Length.cs)"  );
        }
        catch ( ArgumentException exc ) {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.Error.WriteLine( "Error E_763sm!" );
        }
        ba2.Length = ( i2 );
        ++iCountTestcases;
        if ( ba2.Get( (i2 - 1) ) )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: find E_278fj (Co1558set_Length.cs)"  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "BitArray- Co1558set_Length: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.Write( "Co1558set_Length iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "BitArray- Co1558set_Length: FAiL!" );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblW = null;
        try
        {
            Co1558set_Length o2Co1558set_Length = new Co1558set_Length();
            bResult = o2Co1558set_Length.runTest();
        }
        catch ( Exception exc )
        {
            bResult = false;
            sblW = new StringBuilder( "EXTENDEDINFO:  BitArray\\Co1558set_Length.cs main caught Exception!  exc.ToString()==" );
            sblW.Append( exc.ToString() );
            Console.Error.WriteLine(  sblW.ToString()  );
        }
        if ( ! bResult )
        {
            Console.Error.WriteLine(  "Co1558set_Length.cs"  );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
} 
