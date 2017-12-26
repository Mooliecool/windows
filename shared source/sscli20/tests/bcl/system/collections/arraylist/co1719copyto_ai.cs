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
public class Co1719CopyTo_Ai
{
    public static String strName = "ArrayList.CopyTo( Array, int )";
    public static String strTest = "Co1719CopyTo_Ai";
    public static String strPath = "";
    public static String strActiveBugs = "";
    public Boolean runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        Console.WriteLine( "ACTIVE BUGS " + strActiveBugs );
        ArrayList arrList = null;
        String[] arrCopy = null;
        String [] strHeroes = new String[]
        {
            "Aquaman",
            "Atom",
            "Batman",
            "Black Canary",
            "Captain America",
            "Captain Atom",
            "Catwoman",
            "Cyborg",
            "Flash",
            "Green Arrow",
            "Green Lantern",
            "Hawkman",
            null,
            "Ironman",
            "Nightwing",
            "Robin",
            "SpiderMan",
            "Steel",
            null,
            "Thor",
            "Wildcat",
            null
        };
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "Normal Copy Test 1" );
            try
            {
                arrList = new ArrayList(strHeroes );
                arrCopy = new String[strHeroes.Length];
                arrList.CopyTo( arrCopy, 0 );
                for ( int i = 0; i < arrCopy.Length; i++ )
                {
                    if (   (arrCopy[i] == null && strHeroes[i] != null ) ||
                        (arrCopy[i] != null && ! arrCopy[i].Equals( strHeroes[i] ) )
                        )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_101a arrays do not match at position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_101c, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Normal Copy Test 2 - copy 0 elements" );
            try
            {
                arrList = new ArrayList();
                arrList.Add( null );
                arrList.Add( arrList );
                arrList.Add( null );
                arrList.Remove( null );
                arrList.Remove( null );
                arrList.Remove( arrList );
                if ( arrList.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_102b, count should be zero but is " + arrList.Count.ToString() );
                }
                arrCopy = new String[strHeroes.Length];
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    arrCopy[i] = strHeroes[i];
                }
                arrList.CopyTo( arrCopy, 1 );
                for ( int i = 0; i < arrCopy.Length; i++ )
                {
                    if (   (arrCopy[i] == null && strHeroes[i] != null ) ||
                        (arrCopy[i] != null && ! arrCopy[i].Equals( strHeroes[i] ) )
                        )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_102a, arrays do not match at position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_102c, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Normal Copy Test 3 - copy 0 elements from the end" );
            try
            {
                arrList = new ArrayList();
                if ( arrList.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_103b, count should be zero but is " + arrList.Count.ToString() );
                }
                arrCopy = new String[strHeroes.Length];
                for ( int i = 0; i < strHeroes.Length; i++ )
                {
                    arrCopy[i] = strHeroes[i];
                }
                arrList.CopyTo( arrCopy, arrCopy.Length - 1 );
                for ( int i = 0; i < arrCopy.Length; i++ )
                {
                    if (   (arrCopy[i] == null && strHeroes[i] != null ) ||
                        (arrCopy[i] != null && ! arrCopy[i].Equals( strHeroes[i] ) )
                        )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_103a, arrays do not match at position " + i.ToString() );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_103c, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Copy so that exception should be thrown 1 " );
            try
            {
                arrList = new ArrayList();
                arrCopy = new String[2];
                arrList.CopyTo( arrCopy, arrCopy.Length  );
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_104a, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Copy so that exception should be thrown 2" );
            try
            {
                arrList = new ArrayList();
                if ( arrList.Count != 0 )
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_105b, count should be zero but is " + arrList.Count.ToString() );
                }
                arrCopy = new String[0];
                arrList.CopyTo( arrCopy, 0  );
            }
            catch (Exception ex)
            {
                Console.WriteLine( "Err_105a, unexpected exception " + ex.ToString() );
                ++iCountErrors;
                break;
            }
        }
        while ( false );
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( strTest + " iCountTestcases==" + iCountTestcases.ToString() + " paSs" );
            return true;
        }
        else
        {
            Console.WriteLine( strTest + " FAiL");
            Console.Error.WriteLine( strTest + " iCountErrors==" + iCountErrors.ToString() );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        Boolean bResult = false;	
        Co1719CopyTo_Ai oCbTest = new Co1719CopyTo_Ai();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( strTest + " " + strPath );
            Console.WriteLine( strTest + " E_1000000" );
            Console.WriteLine( strTest + "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( strTest + " " + ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
} 
