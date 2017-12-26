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
class Co1806
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "ctor_Biibb";
    public String s_strTFName                 = "Co1806ctor_Biibb";
    public String s_strTFAbbrev               = "Co1806";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "construct stream that is not exposed, should get an exception" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream( new byte[] { Byte.MinValue, Byte.MaxValue }, 1, 1, false, false );
            try
            {
                ms.GetBuffer();
                ++iCountErrors;
                Console.WriteLine( "Err_001b, should have thrown AccessException" );
            }
            catch ( UnauthorizedAccessException )
            {
            }
            catch(Exception ex)
            {
                Console.WriteLine("Err_7452fdsf! Wrong exception thrown. " + ex);
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "construct stream that we can write to" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream( new byte[] { Byte.MinValue, Byte.MaxValue }, 1, 1, true, true );
            ms.GetBuffer();		
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002a,  Unexpected exception was thrown ex: " + ex.ToString() );
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
        Co1806 runClass = new Co1806();
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
