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
public class Co3933Sort_I
{
    public static String strName = "ArrayList.Sort";
    public static String strTest = "Co3933Sort_I";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
        String [] strHeroesUnsorted = null;
        String [] strHeroes =
        {
            "Green Arrow",
            "Atom",
            "Batman",
            "Steel",
            "Superman",
            "Wonder Woman",
            "Hawkman",
            "Flash",
            "Aquaman",
            "Green Lantern",
            "Catwoman",
            "Huntress",
            "Robin",
            "Captain Atom",
            "Wildcat",
            "Nightwing",
            "Ironman",
            "SpiderMan",
            "Black Canary",
            "Thor",
            "Cyborg",
            "Captain America",
        };
        String [] strHeroesSorted =
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
            "Huntress",
            "Ironman",
            "Nightwing",
            "Robin",
            "SpiderMan",
            "Steel",
            "Superman",
            "Thor",
            "Wildcat",
            "Wonder Woman",
        };
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Test ascending sort" );
            try
            {
                strHeroesUnsorted = new String[strHeroes.Length];
                System.Array.Copy( strHeroes, 0, strHeroesUnsorted, 0, strHeroes.Length );
                System.Array.Sort( strHeroesUnsorted, null, 0, strHeroesUnsorted.Length, new Co3933Sort_I_Assending() );
                for ( int ii = 0; ii < strHeroesUnsorted.Length; ++ii )
                {
                    if ( strHeroesSorted[ii].CompareTo( strHeroesUnsorted[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected index of hero <"+ strHeroesSorted[ii] + "> ";
                        strInfo = strInfo + "Returned index of hero <"+ strHeroesUnsorted[ii] + "> ";
                        Console.WriteLine( strTest+ "E_202: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10002: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Test decending sort" );
            try
            {
                strHeroesUnsorted = new String[strHeroes.Length];
                System.Array.Copy( strHeroes, 0, strHeroesUnsorted, 0, strHeroes.Length );
                System.Array.Sort( strHeroesUnsorted, null, 0, strHeroesUnsorted.Length, new Co3933Sort_I_Decending() );
                for ( int ii = 0; ii < strHeroesUnsorted.Length; ++ii )
                {
                    if ( strHeroesSorted[ii].CompareTo( strHeroesUnsorted[strHeroesUnsorted.Length-ii-1] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected index of hero <"+ strHeroesSorted[ii] + "> ";
                        strInfo = strInfo + "Returned index of hero <"+ strHeroesUnsorted[strHeroesUnsorted.Length-ii-1] + "> ";
                        Console.WriteLine( strTest+ "E_304: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10003: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Sort array list using default comparer" );
            try
            {
                arrList = new ArrayList( (ICollection) strHeroesUnsorted );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_404: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                arrList.Sort( null );
                for ( int ii = 0; ii < arrList.Count; ++ii )
                {
                    if ( strHeroesSorted[ii].CompareTo( (String)arrList[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected index of hero <"+ strHeroesSorted[ii] + "> ";
                        strInfo = strInfo + "Returned index of hero <"+ (String)arrList[ii] + "> ";
                        Console.WriteLine( strTest+ "E_405: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Sort array list using default comparer" );
            try
            {
                arrList = new ArrayList( (ICollection) strHeroesUnsorted );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_404: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                arrList.Sort( new Co3933Sort_I_Assending() );
                for ( int ii = 0; ii < arrList.Count; ++ii )
                {
                    if ( strHeroesSorted[ii].CompareTo( (String)arrList[ii] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected index of hero <"+ strHeroesSorted[ii] + "> ";
                        strInfo = strInfo + "Returned index of hero <"+ (String)arrList[ii] + "> ";
                        Console.WriteLine( strTest+ "E_405: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Sort array list using default comparer" );
            try
            {
                arrList = new ArrayList( (ICollection) strHeroesUnsorted );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_404: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                arrList.Sort( new Co3933Sort_I_Decending() );
                for ( int ii = 0, jj = arrList.Count-1; ii < arrList.Count; ++ii, jj-- )
                {
                    if ( strHeroesSorted[ii].CompareTo( (String)arrList[jj] ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected index of hero <"+ strHeroesSorted[jj] + "> ";
                        strInfo = strInfo + "Returned index of hero <"+ (String)arrList[ii] + "> ";
                        Console.WriteLine( strTest+ "E_405: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10004: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
        }
        while ( false );
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( strTest + " iCountTestcases==" + iCountTestcases + " paSs" );
            return true;
        }
        else
        {
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "FAiL" );
            Console.Error.WriteLine( strTest + " iCountErrors==" + iCountErrors );
            return false;
        }
    }
    internal class Co3933Sort_I_Assending : IComparer
    {
        public virtual int Compare(Object x, Object y)
        {
            return ((String)x).CompareTo( (String)y );
        }
    }
    internal class Co3933Sort_I_Decending : IComparer
    {
        public virtual int Compare(Object x, Object y)
        {
            return -((String)x).CompareTo( (String)y );
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co3933Sort_I oCbTest = new Co3933Sort_I();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "E_1000000" );
            Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( strTest+ ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
