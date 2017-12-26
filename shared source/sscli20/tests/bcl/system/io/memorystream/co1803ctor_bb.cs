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
class Co1803
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "ctor_Bb";
    public String s_strTFName                 = "Co1803ctor_Bb";
    public String s_strTFAbbrev               = "Co1803";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "construct MemoryStream with byte[] of different lengths, and CanWrite set to false" );
        try
        {
            byte [][] byteArrs = new byte[100][];
            for ( int i = 0; i < byteArrs.Length; i++ )
            {
                byteArrs[i] = new byte[i];		
            }
            for ( int i = 0; i < byteArrs.Length; i++ )
            {
                ++iCountTestcases;
                MemoryStream ms = new MemoryStream(byteArrs[i], false );
                if ( ms.Length != byteArrs[i].Length )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001a, length should be " + byteArrs[i].Length + " but is " + ms.Length + ", calling Write methods increases Lenght, setting certain sized buffer increases Length." );
                }
                ++iCountTestcases;
                if ( ! ms.CanSeek )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001b, CanSeek should return true but returns false" );
                }
                ++iCountTestcases;		
                if ( ms.CanWrite != false )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001c, CanWrite should return false but returns true" );
                }
                ++iCountTestcases;		
                if ( ! ms.CanRead )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001d, CanRead should return true but returns false" );
                }
                ++iCountTestcases;		
                if ( ms.Capacity != byteArrs[i].Length )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001e, Initial capacity should be " + byteArrs[i].Length + " + but it is: " + ms.Capacity );		
                }
                ++iCountTestcases;
                try
                {
                    ms.GetBuffer();
                    ++iCountErrors;
                    Console.WriteLine( "Err_001f, GetBuffer should throw AccessException for byteArray of size: " + byteArrs[i].Length );      
                }
                catch ( UnauthorizedAccessException )
                {			
                }
                catch(Exception ex)
                {
                    Console.WriteLine("Err_7452fdsf! Wrong exception thrown. " + ex);
                }
                ++iCountTestcases;
                try
                {
                    ms.Capacity = (ms.Capacity);
                }
                catch ( Exception exin )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001g, Unexpected Exception " + exin );      
                }
                ++iCountTestcases;
                try
                {
                    ms.Capacity = ( ms.Capacity + 1 );
                    ++iCountErrors;
                    Console.WriteLine( "Err_001h, should have thrown NotSupportedException when setting capacity to something greater" );
                }
                catch ( NotSupportedException )
                {			
                }
                catch(Exception ex)
                {
                    Console.WriteLine("Err_7563wrdfsg! Wrong exception thrown. " + ex);
                }
                ++iCountTestcases;
                try
                {
                    ms.Capacity = ( ms.Capacity - 1 );
                    ++iCountErrors;
                    Console.WriteLine( "Err_001i, should have thrown NotSupportedException when setting capacity to something less then" );
                }
                catch ( NotSupportedException )
                {			
                }			
                catch(Exception ex)
                {
                    Console.WriteLine("Err_872wsf! Wrong exception thrown. " + ex);
                }
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001j,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "Construct MemoryStream with null byte[] and not writable" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream( null, false );
            ++iCountErrors;
            Console.WriteLine( "Err_002a,  Expected exception ArgumentNullException was not thrown." );      
        }
        catch ( ArgumentNullException )
        {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "try to see if stream can by truncated using SetLength where writable is false" );
        try
        {
            byte [][] byteArrs = new byte[100][];
            for ( int i = 0; i < byteArrs.Length; i++ )
            {
                byteArrs[i] = new byte[i];		
            }
            for ( int i = 0; i < byteArrs.Length; i++ )
            {		
                if ( verbose ) Console.WriteLine( "Trying test on byte[" + i + "]" );
                ++iCountTestcases;
                MemoryStream ms = new MemoryStream(byteArrs[i], false);
                try
                {
                    for ( int j = byteArrs[i].Length; j >= 0; j-- )
                    {					
                        ms.SetLength( j );
                        ++iCountErrors;
                        Console.WriteLine( "Err_003b,  expected NotSupportedException was not thrown" );					
                    }
                }
                catch ( NotSupportedException )
                {
                }
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_003a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "try to see if stream can be expanded with SetLength, it should not be" );
        try
        {
            byte [][] byteArrs = new byte[100][];
            for ( int i = 0; i < byteArrs.Length; i++ )
            {
                byteArrs[i] = new byte[i];		
            }
            for ( int i = 0; i < byteArrs.Length; i++ )
            {		
                if ( verbose ) Console.WriteLine( "Trying test on byte[" + i + "]" );
                ++iCountTestcases;
                MemoryStream ms = new MemoryStream(byteArrs[i], false );
                for ( int j = byteArrs[i].Length + 1; j <= byteArrs[i].Length; j++ )
                {					
                    try
                    {
                        ms.SetLength( j );
                        ++iCountErrors;
                        Console.WriteLine( "Err_004b,  Expected ArgumentException was not thrown by SetLength(" + j + ")" );
                    }
                    catch ( NotSupportedException )
                    {
                    }					
                }
                try
                {
                    ms.SetLength( Int32.MaxValue );
                    ++iCountErrors;
                    Console.WriteLine( "Err_004c,  Expected ArgumentException was not thrown by SetLength(" + Int32.MaxValue + ")" );
                }
                catch ( NotSupportedException )
                {
                }
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_004a,  Unexpected exception was thrown ex: " + ex.ToString() );
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
        Co1803 runClass = new Co1803();
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
        if ( bResult == true ) Environment.ExitCode = (0);
        else Environment.ExitCode = (1); 
    }
}  
