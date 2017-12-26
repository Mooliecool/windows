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
class Co1811
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "get_CanWrite";
    public String s_strTFName                 = "Co1811get_CanWrite";
    public String s_strTFAbbrev               = "Co1811";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "make sure we can write to an open MemoryStream" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream();
            if ( ! ms.CanWrite )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_001c,  newly created MemoryStream should be Writable" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "make sure we canNOT write to a closed MemoryStream" );
        try
        {
            ++iCountTestcases;
            MemoryStream ms = new MemoryStream();
            ms.Close();
            if ( ms.CanWrite )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_002c,  closed MemoryStream should NOT be Writable" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002b,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        try 
        {
            MemoryStream ms = new MemoryStream(new Byte[10], 0, 5, false, true);
            iCountTestcases++;
            if(ms.CanWrite) 
            {
                iCountErrors++;
                Console.WriteLine( "Error_3298v! CanWrite was true for readonly stream");
            } 
            ms.Close();
        } 
        catch (Exception exc) 
        {
            iCountErrors++;
            Console.WriteLine( "Error_398vy! Unexpected exception thrown, exc=="+exc);
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
        Co1811 runClass = new Co1811();
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
