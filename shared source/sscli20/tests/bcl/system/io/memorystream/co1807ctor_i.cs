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
class Co1807
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "ctor_i";
    public String s_strTFName                 = "Co1807ctor_i";
    public String s_strTFAbbrev               = "Co1807";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "construct MemoryStream with diff capacities and check against spec" );
        try
        {
            for ( int i = 0; i < 2000; i++ )
            {
                ++iCountTestcases;
                MemoryStream ms = new MemoryStream(i);
                if ( ms.Length != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001a, length should be 0 but is " + ms.Length + ", note setting capacity to high value does not increase length, calling Write methods increases Lenght" );
                }
                ++iCountTestcases;
                if ( ! ms.CanSeek )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001b, CanSeek should return true but returns false" );
                }
                ++iCountTestcases;		
                if ( ! ms.CanWrite )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001c, CanWrite should return true but returns false" );
                }
                ++iCountTestcases;		
                if ( ! ms.CanRead )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001d, CanRead should return true but returns false" );
                }
                ++iCountTestcases;		
                if ( ms.Capacity != i )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001e, Initial capacity should be " + i + " + but it is: " + ms.Capacity );		
                }
                ++iCountTestcases;
                if ( ms.GetBuffer() == null)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001f, GetBuffer should NOT be null!" );      
                }
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001g,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "Construct really huge MemoryStream" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream( Int32.MaxValue );
            ++iCountErrors;
            Console.WriteLine( "Err_002a,  Expected exception OutOfMemoryException was not thrown." );      
        }
        catch ( OutOfMemoryException )
        {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        for ( int i = -1; i > -50; i-- )
        {
            if ( verbose ) Console.WriteLine( "Construct MemoryStream with capacity " + i + " should throw" );
            try
            {
                ++iCountTestcases;		
                MemoryStream ms = new MemoryStream( i );
                ++iCountErrors;
                Console.WriteLine( "Err_003a,  Expected exception ArgumentException was not thrown." );      
            }
            catch ( ArgumentException )
            {}
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_003b,  Unexpected exception was thrown ex: " + ex.ToString() );
            }
        }
        if ( verbose ) Console.WriteLine( "Construct MemoryStream with capacity " + Int32.MinValue + " should throw" );
        try
        {
            ++iCountTestcases;		
            MemoryStream ms = new MemoryStream( Int32.MinValue );
            ++iCountErrors;
            Console.WriteLine( "Err_003a,  Expected exception ArgumentException was not thrown." );      
        }
        catch ( ArgumentException )
        {}
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_003b,  Unexpected exception was thrown ex: " + ex.ToString() );
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
        Co1807 runClass = new Co1807();
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
