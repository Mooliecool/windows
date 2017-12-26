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
class Co1814
{
    public String s_strActiveBugNums          = "";
    public String s_strDtTmVer                = "";
    public String s_strComponentBeingTested   = "get_Position";
    public String s_strTFName                 = "Co1814get_Position";
    public String s_strTFAbbrev               = "Co1814";
    public String s_strTFPath                 = "";
    public Boolean verbose                    = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        if ( verbose ) Console.WriteLine( "set the Position to a valid value and get it after" );
        try
        {
            ++iCountTestcases;
            int iArrLen = 100;
            byte [] bArr = new byte[iArrLen];
            MemoryStream ms = new MemoryStream( bArr );
            for ( int i = 0; i < iArrLen; i++ )
            {
                ms.Position = i;	
                if ( ms.Position != i )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001b,  the position is incorrect ms.Position " + ms.Position + " but should be " + i );
                }		
            }
            for ( int i = iArrLen - 1; i >= 0; i-- )
            {
                ms.Position = i;
                if ( ms.Position != i )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_001b,  the position is incorrect ms.Position " + ms.Position + " but should be " + i );
                }			
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_001a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "set the Position to invalid values check to make sure position stayed at correctly set value" );
        try
        {
            ++iCountTestcases;
            int iArrLen = 100;
            byte [] bArr = new byte[iArrLen];
            MemoryStream ms = new MemoryStream( bArr );
            try
            {
                ms.Position = iArrLen;
                if(ms.Position != iArrLen) 
                {
                    iCountErrors++;
                    Console.WriteLine("Error_798hj! Position incorrect");
                }
            }
            catch( ArgumentOutOfRangeException )
            {}
            ms.Position = 66;
            for ( int i = iArrLen + 1; i < iArrLen + 5; i++ )
            {
                try
                {
                    ms.Position = i;							
                    if ( ms.Position != i )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_002d,  the position is incorrect ms.Position " + ms.Position + " but should be "+ i );
                    }  
                } 
                catch ( IOException e)
                {
                    iCountErrors++ ;
                    Console.WriteLine("Unexpected exception occured... " + e.ToString() );
                }
            }
            long iCurrentPos = ms.Position;
            for ( int i = -1; i > -6; i-- )
            {
                try
                {
                    ms.Position = i;			
                    ++iCountErrors;
                    Console.WriteLine( "Err_002c,  should have thrown for i: " + i );
                }
                catch ( ArgumentOutOfRangeException )
                {}
                if ( ms.Position !=iCurrentPos)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_002e,  the position is incorrect ms.Position " + ms.Position + " but should be " + i );
                }			
            }	
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Err_002a,  Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "position should start at 0 after MemoryStream has been constructed" );
        try
        {
            ++iCountTestcases;
            int iArrLen = 100;
            byte [] bArr = new byte[iArrLen];
            MemoryStream ms = new MemoryStream( bArr );
            if ( ms.Position != 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_003b,  position should start at 0 after MemoryStream has been constructed but is " + ms.Position );			
            }
            ms = new MemoryStream( bArr, 10, 0 );
            if ( ms.Position != 0 )
            {
                ++iCountErrors;
                Console.WriteLine( "Err_003c,  position should start at 0 after MemoryStream has been constructed but is " + ms.Position );			
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
        Co1814 runClass = new Co1814();
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
