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
class Co1821
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "Write_Bii";
    public String s_strTFName                 = "Co1821Write_Bii";
    public String s_strTFAbbrev               = "Co1821";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "use the Write method to write a correctly specified bytes" );
        try
        {
            ++iCountTestcases;
            byte[] byteArr = new byte[ 100 ];		
            for ( int i = 0; i < byteArr.Length; i++ )
            {
                byteArr[i] = (byte) 6;
            }
            byte [] byteWriteArr = new byte[ 10 ];
            for ( int i = 0; i < byteWriteArr.Length; i++ )
            {
                byteWriteArr[i] = (byte) 5;
            }
            MemoryStream ms = new MemoryStream( byteArr );
            ms.Position = 10;
            int iWriteSize = byteWriteArr.Length - 2;
            ms.Write( byteWriteArr, 0, iWriteSize );
            if ( ms.Position != 10 + iWriteSize )
            {
                Console.WriteLine( "Err_002e,  Should have written " +  iWriteSize + " bytes, but Position was wrong." );
                ++iCountErrors;
            }
            for ( int i = 0; i < byteArr.Length; i++ )
            {
                if ( (i < 10 || i > 10 + iWriteSize - 1) )
                {
                    if ( byteArr[i] != (byte) 6 )
                    {
                        Console.WriteLine( "Err_001b,  Expected 6 at position " + i + " but it was " + byteArr[i] );
                        ++iCountErrors;
                    }
                }
                else 
                {
                    if ( byteArr[i] != (byte) 5 )
                    {
                        Console.WriteLine( "Err_001c,  Expected 5 at position " + i + " but it was " + byteArr[i] );				
                        ++iCountErrors;
                    }
                }
            }
            if ( ms.Position != 10 + iWriteSize )
            {
                Console.WriteLine( "Err_001d,  Position should have been " + 10 + iWriteSize + " but is " + ms.Position );				
                ++iCountErrors;		
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "use the Write method to write a 0 length arrays" );
        try
        {
            ++iCountTestcases;
            byte[] byteArr = new byte[ 100 ];		
            for ( int i = 0; i < byteArr.Length; i++ )
            {
                byteArr[i] = (byte) 6;
            }
            byte [] byteWriteArr = new byte[ 10 ];
            for ( int i = 0; i < byteWriteArr.Length; i++ )
            {
                byteWriteArr[i] = (byte) 5;
            }
            MemoryStream ms = new MemoryStream( byteArr );
            ms.Position = 10;
            ms.Write( new byte[]{}, 0, 0 );
            if ( ms.Position != 10 )
            {
                Console.WriteLine( "Err_002c,  Position should have been " + 10 + " but is " + ms.Position );				
                ++iCountErrors;		
            }
            ms.Position = 30;
            ms.Write( byteWriteArr, 0, 0 );
            if ( ms.Position != 30 )
            {
                Console.WriteLine( "Err_002d,  Position should have been " + 30 + " but is " + ms.Position );				
                ++iCountErrors;		
            }
            for ( int i = 0; i < byteArr.Length; i++ )
            {
                if ( byteArr[i] != (byte) 6 )
                {
                    Console.WriteLine( "Err_002b,  Expected 6 at position " + i + " but it was " + byteArr[i] );				
                    ++iCountErrors;
                }	
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "write a null array" );
        try
        {
            ++iCountTestcases;
            byte[] byteArr = new byte[ 100 ];		
            MemoryStream ms = new MemoryStream( byteArr );
            ms.Position = 10;
            try
            {		
                ms.Write( null, 0, 0 );
                ++iCountErrors;
                Console.WriteLine( "Err_003b,  Expected ArgumentNullException was not thrown" );						
            }
            catch ( ArgumentNullException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_003a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "try writing where we are not allowed to write" );
        try
        {
            ++iCountTestcases;
            byte[] byteArr = new byte[ 100 ];		
            MemoryStream ms = new MemoryStream( byteArr, false );
            try
            {		
                ms.Write( new byte[] {Byte.MaxValue}, 0, 1 );
                ++iCountErrors;
                Console.WriteLine( "Err_004b,  Expected NotSupportedException was not thrown" );						
            }
            catch ( NotSupportedException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_004a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "try writing to offset which is less then 0" );
        try
        {
            ++iCountTestcases;
            byte[] byteArr = new byte[ 100 ];		
            MemoryStream ms = new MemoryStream( byteArr );
            try
            {		
                ms.Write( new byte[] {Byte.MaxValue}, -1, 1 );
                ++iCountErrors;
                Console.WriteLine( "Err_005b,  Expected ArgumentOutOfRangeException was not thrown" );						
            }
            catch ( ArgumentOutOfRangeException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_005a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "try writing count less then 0" );
        try
        {
            ++iCountTestcases;
            byte[] byteArr = new byte[ 100 ];		
            MemoryStream ms = new MemoryStream( byteArr );
            try
            {		
                ms.Write( new byte[] {Byte.MaxValue}, 0, -1 );
                ++iCountErrors;
                Console.WriteLine( "Err_006b,  Expected ArgumentOutOfRangeException was not thrown" );						
            }
            catch ( ArgumentOutOfRangeException )
            {
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_006a,  Unexpected exception was thrown ex: " + ex.ToString() );
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
        Co1821 runClass = new Co1821();
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
