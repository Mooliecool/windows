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
using System;
using System.IO;
class Co1804
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "ctor_Bii";
    public String s_strTFName                 = "Co1804ctor_Bii";
    public String s_strTFAbbrev               = "Co1804";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "create a Memory stream, write to all position expect for before beg and aft end, check ends" );
        try
        {
            ++iCountTestcases;
            byte [] byteArr = new byte[200 + 1000 + 200];
            for ( int i = 0; i < byteArr.Length; i++ )
            {
                byteArr[i] = Byte.MaxValue;
            }
            MemoryStream ms = new MemoryStream( byteArr, 200, 1000 );
            byte [] byteDataWrite = new byte[ 100 ];
            for ( int i = 0; i < byteDataWrite.Length; i++ )
            {
                byteDataWrite[i] = Byte.MinValue;
            }
            for ( int i = 0; i < 10; i++ )
            {
                ms.Position = i * 100;
                ms.Write( byteDataWrite, 0, byteDataWrite.Length );
            }
            ms.Close();
            for ( int i = 0; i < 200; i++ )
            {
                if ( byteArr[i] != Byte.MaxValue )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001c, byteArr[" + i + "] should have been " + Byte.MaxValue + " but was " + byteArr[i] );
                }
            }
            for ( int i = 200; i < 1000; i++ )
            {
                if ( byteArr[i] != Byte.MinValue )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001d, byteArr[" + i + "] should have been " + Byte.MinValue + " but was " + byteArr[i] );
                }
            }
            for ( int i = 1200; i < 200; i++ )
            {
                if ( byteArr[i] != Byte.MaxValue )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001e, byteArr[" + i + "] should have been " + Byte.MaxValue + " but was " + byteArr[i] );
                }
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "create mem stream have first int be a incorrect index (neg) into byte[]" );	
        try
        {
            ++iCountTestcases;
            byte[] byteArr = new byte[ 666 ];
            try
            {
                MemoryStream ms = new MemoryStream( byteArr, -1, Int32.MaxValue );
                ++iCountErrors;
                Console.WriteLine( "Err_002b,  expected ArgumentOutOfRangeException was not thrown." );
            }
            catch ( ArgumentOutOfRangeException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "create mem stream have second int be an incorrect index (neg) into byte[]" );
        try
        {
            ++iCountTestcases;	
            byte[] byteArr = new byte[ 666 ];
            try
            {
                MemoryStream ms = new MemoryStream( byteArr, 6, -1 );
                ++iCountErrors;
                Console.WriteLine( "Err_003b,  expected ArgumentOutOfRangeException was not thrown." );
            }
            catch ( ArgumentOutOfRangeException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_003a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "create mem stream have count is 0 into byte[]" );
        try
        {
            ++iCountTestcases;	
            byte[] byteArr = new byte[ 7 ];
            MemoryStream ms = new MemoryStream( byteArr, 5, 0 );
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_004a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "create mem stream have first int and second int be both invalid into byte[]" );
        try
        {
            ++iCountTestcases;	
            byte[] byteArr = new byte[ 7 ];
            try
            {
                MemoryStream ms = new MemoryStream( byteArr, 8, 2 );
                ++iCountErrors;
                Console.WriteLine( "Err_005b,  should have thrown ArgumentExcpetion" );
            }
            catch ( ArgumentException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_005a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "create mem stream where byte[] arr is null" );
        try
        {
            ++iCountTestcases;	
            byte[] byteArr = new byte[ 7 ];
            try
            {
                MemoryStream ms = new MemoryStream( null, 5, 2 );
                ++iCountErrors;
                Console.WriteLine( "Err_006b,  should have thrown ArgumentExcpetion" );			
            }
            catch ( ArgumentNullException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_006a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "create mem stream where byte[] arr is 0 length, both indecies are 0" );
        try
        {
            ++iCountTestcases;	
            byte[] byteArr = new byte[ 0 ];
            MemoryStream ms = new MemoryStream( byteArr, 0, 0 );
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_007a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases="+ iCountTestcases.ToString() );
            return true;
        }
        else
        {
            Console.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors="+ iCountErrors.ToString() +" ,BugNums?: "+ s_strActiveBugNums );
            return false;
        }
    }
    public static void Main( String [] args )
    {
        Co1804 runClass = new Co1804();
        if ( args.Length > 0 )
        {
            Console.WriteLine( "Verbose ON!" );
            runClass.verbose = true;
        }	
        Boolean bResult = runClass.runTest();
        if ( ! bResult )
        {
            Console.WriteLine( runClass.s_strTFPath + runClass.s_strTFName );
            Console.Error.WriteLine( " " );
            Console.Error.WriteLine( "FAiL!  "+ runClass.s_strTFAbbrev );  
            Console.Error.WriteLine( " " );
            Console.Error.WriteLine( "ACTIVE BUGS: " + runClass.s_strActiveBugNums ); 
        }
        if ( bResult == true ) Environment.ExitCode = 0;
        else Environment.ExitCode = 1; 
    }
}  
