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
class Co1812
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "Close";
    public String s_strTFName                 = "Co1812Close";
    public String s_strTFAbbrev               = "Co1812";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "Close stream and see what Properties do, boolean Properties should return false, otherwise others should throw" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream( new byte[] { Byte.MaxValue, Byte.MinValue, Byte.MaxValue } );
            ms.Close();
            if ( ms.CanRead )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001b,  CanRead, Should have returned false" );					
            }
            if ( ms.CanSeek )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001c,  CanSeek, Should have returned false" );
            }
            if ( ms.CanWrite )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001d,  CanWrite, Should have returned false" );		
            }
            try
            {
                long iTemp = ms.Length;
                ++iCountErrors;
                Console.WriteLine( "Err_001f,  get_Length should have thrown" );					
            }
            catch ( ObjectDisposedException )
            {		
            }
            try
            {
                long iTemp = ms.Position;
                ++iCountErrors;
                Console.WriteLine( "Err_001g,  get_Position should have thrown" );					
            }
            catch ( ObjectDisposedException )
            {		
            }
            try
            {
                ms.Position = 0;
                ++iCountErrors;
                Console.WriteLine( "Err_001h,  get_Position should have thrown" );					
            }
            catch ( ObjectDisposedException )
            {		
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "we should be able to Close the stream multiple times" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream( );
            for ( int i = 0; i < 10; i++ )
            {
                ms.Close();
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "we should be able to use Flush after the stream is Closed" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream( );
            ms.Close();
            for ( int i = 0; i < 10; i++ )
            {
                ms.Flush();
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_003a,  Unexpected exception was thrown ex: " + ex.ToString() );
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
        Co1812 runClass = new Co1812();
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
