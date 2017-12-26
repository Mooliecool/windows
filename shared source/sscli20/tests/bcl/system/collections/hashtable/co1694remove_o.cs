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
public class Co1694
{
    static String strName = "Hashtable.Remove( Object )";
    static String strTest = "Co1694Remove.cs";
    static String strPath = "";
    public Boolean runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Out.Write( strPath );
        Console.Out.Write( strTest );
        Console.Out.WriteLine( " runTest() started..." );
        Hashtable hash = null;
        int ii;
        String[] strSuperHeroes = new String[]
        {
            "Captain Marvel" ,		
            "Batgirl" ,			
            "Nightwing" ,			
            "Green Lantern" ,		
            "Robin" ,				
            "Superman" ,			
            "Black Canary" ,		
            "Spiderman" ,			
            "Iron Man" ,			
            "Wonder Girl" ,		
            "Batman" ,				
            "Flash" ,				
            "Green Arrow" ,		
            "Atom" ,				
            "Steel" ,				
            "Powerman" ,			
        };
        String[] strSecretIdentities = new String[]
        {
            "Batson, Billy" ,		
            "Gordan, Barbara" ,	
            "Grayson, Dick" ,		
            "Jordan, Hal" ,		
            "Drake, Tim" ,			
            "Kent, Clark" ,		
            "Lance, Dinah" ,		
            "Parker, Peter" ,		
            "Stark, Tony" ,		
            "Troy, Donna" ,		
            "Wayne, Bruce" ,		
            "West, Wally" ,		
            "Queen, Oliver" ,		
            "Palmer, Ray" ,		
            "Irons, John Henry" ,	
            "Cage, Luke" ,			
        };
        hash = new Hashtable();
        for ( ii = 0; ii < strSuperHeroes.Length; ++ii )
        {
            iCountTestcases++;
            try
            {
                hash.Add( strSuperHeroes[ii], strSecretIdentities[ii] );
            }
            catch ( Exception ex )
            {
                Console.WriteLine( strTest, " E_101, unexpected exception when adding elements, exception = " +  ex.ToString() );
                ++iCountErrors;
            }
        }
        iCountTestcases++;
        if ( hash.Count != strSuperHeroes.Length )
        {
            Console.WriteLine( strTest, " E_202, hash.Count " + hash.Count.ToString() + " does not equal strSuperHeroes.Length " + strSuperHeroes.Length.ToString() );
            ++iCountErrors;
        }
        iCountTestcases++;
        Console.Out.WriteLine( "Remove: Attempt to remove a bogus key entry from table" );
        try
        {
            hash.Remove( "THIS IS A BOGUS KEY"  );
            if ( hash.Count != strSuperHeroes.Length )
            {
                Console.WriteLine( strTest, " E_303, hash.Count " + hash.Count.ToString() + " does not equal strSuperHeroes.Length " + strSuperHeroes.Length.ToString() );
                ++iCountErrors;
            }
        }
        catch ( Exception ex )
        {
            Console.WriteLine( strTest, " E_404, unexpected exception when removing bogus element, exception = " +  ex.ToString() );
            ++iCountErrors;
        }
        iCountTestcases++;
        Console.Out.WriteLine( "Remove: Attempt to remove a null key entry from table" );
        try
        {
            hash.Remove( null );
            Console.WriteLine( strTest, " E_505, expected ArgumentNullException but did not throw any exception ");
            ++iCountErrors;
        }
        catch ( ArgumentNullException )
        {
        }
        catch ( Exception ex )
        {
            Console.WriteLine( strTest, " E_606, throw unexpected exception, exception="+ex.ToString());
            ++iCountErrors;
        }
        Console.Out.WriteLine( "Remove: Add key/value pair to Hashtable and remove items" );
        for ( ii = 0; ii < strSuperHeroes.Length; ++ii )
        {
            iCountTestcases++;
            try
            {
                hash.Remove( strSuperHeroes[ii] );
                if ( hash.Count != strSuperHeroes.Length - ii - 1 )
                {
                    Console.WriteLine( strTest, " E_707, hash.Count " + hash.Count.ToString() + " does not equal strSuperHeroes.Length " + (strSuperHeroes.Length - ii - 1).ToString());
                    ++iCountErrors;
                }
            }
            catch ( Exception ex )
            {
                Console.WriteLine( strTest, " E_808, throw unexpected exception, exception="+ex.ToString());
                ++iCountErrors;
            }
        }
        for ( ii = 0; ii < strSuperHeroes.Length; ++ii )
        {
            iCountTestcases++;
            try
            {
                hash.Add( strSuperHeroes[ii], strSecretIdentities[ii] );
            }
            catch ( Exception ex )
            {
                Console.WriteLine( strTest, " E_909, unexpected exception when adding elements FOR SECOND TIME and removing them, exception = " +  ex.ToString() );
                ++iCountErrors;
            }
        }
        for ( ii = 0; ii < strSuperHeroes.Length; ++ii )
        {
            iCountTestcases++;
            try
            {
                hash.Remove( strSuperHeroes[ii] );
                if ( hash.Count != strSuperHeroes.Length - ii - 1 )
                {
                    Console.WriteLine( strTest, " E_111, hash.Count " + hash.Count.ToString() + " does not equal strSuperHeroes.Length " + (strSuperHeroes.Length - ii - 1).ToString());
                    ++iCountErrors;
                }
            }
            catch ( Exception ex )
            {
                Console.WriteLine( strTest, " E_222, unexpected exception when adding elements FOR SECOND TIME and removing them, exception = " +  ex.ToString() );
                ++iCountErrors;
            }
        }
        hash.Clear();
        for(int iAnnoying = 0; iAnnoying<10; iAnnoying++)
        {
            for ( ii = 0; ii < strSuperHeroes.Length; ++ii )
            {
                iCountTestcases++;
                try
                {
                    hash.Remove( strSuperHeroes[ii] );
                    if ( hash.Count != 0 )
                    {
                        Console.WriteLine( strTest, " E_6534sg, hash.Count ");
                        ++iCountErrors;
                    }
                }
                catch ( Exception ex )
                {
                    Console.WriteLine( strTest, " E_222, unexpected exception when adding elements FOR SECOND TIME and removing them, exception = " +  ex.ToString() );
                    ++iCountErrors;
                }
            }
        }
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine( " paSs, iCountTestcases=="  + iCountTestcases);
            return true;
        }
        else
        {
            Console.Write( strTest + strPath );
            Console.WriteLine( strTest + "FAiL" );
            Console.Error.Write( strTest );
            Console.Error.Write( " iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            return false;
        }
    }
    public static void Main(String[] args)
    {
        Boolean bResult = false;	
        Co1694 oCbTest = new Co1694();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( "E_999!  Caught Exception " + ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
} 
