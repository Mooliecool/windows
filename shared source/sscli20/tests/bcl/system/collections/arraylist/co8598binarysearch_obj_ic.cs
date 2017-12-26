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
using System.Collections;
using System.IO;
public class Co8598BinarySearch_obj_ic : IComparer
{
    public static String s_strActiveBugNums = "";
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "ArrayList.BinarySearch(Object)";
    public static String s_strTFName        = "Co8598BinarySearch_obj_ic";
    public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public Boolean verbose = false;
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( s_strClassMethod + ": " + s_strTFName + " runTest started..." );
        ArrayList list = null;
        String strLoc = "Loc_000oo";
        String [] strHeroes =
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
        String [] strFindHeroes =
            {
                "Batman",
                "Superman",
                "SpiderMan",
                "Wonder Woman",
                "Green Lantern",
                "Flash",
                "Steel"
            };
        try 
        {
            strLoc = "Loc_001o1";
            ++iCountTestcases;
            try
            {
                list = new ArrayList();
                if ( list == null )
                {
                    Console.WriteLine( s_strTFName+ "E_101: Failed to construct new ArrayList" );
                    ++iCountErrors;
                }
                for ( int ii = 0; ii < strHeroes.Length; ++ii )
                    list.Add( strHeroes[ii] );
                if ( list.Count != strHeroes.Length )
                {
                    Console.WriteLine( "E_202: ");
                    ++iCountErrors;
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( s_strTFName+ "E_10001: Unexpected Exception: " + ex.ToString() );
            }
            ++iCountTestcases;
            try
            {
                for ( int ii = 0; ii < strFindHeroes.Length; ++ii )
                {
                    int ndx = list.BinarySearch(strFindHeroes[ii], new Co8598BinarySearch_obj_ic());
                    if ( ndx >= strHeroes.Length )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_753fg unexpected value returned" );
                    }
                    if ( strHeroes[ndx].CompareTo( strFindHeroes[ii] ) != 0 )
                    {
                        ++iCountErrors;
                        Console.WriteLine( "Err_853deg! unexpected value returned");
                    }
                }
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( s_strTFName+ "E_10002: Unexpected Exception: " + ex.ToString() );
            }
            strLoc = "Loc_00437sg";
            iCountTestcases++;
            list = new ArrayList();
            for(int i=0; i<100; i++)
                list.Add(i);
            list.Sort();
            if(~list.BinarySearch(150, new Co8598BinarySearch_obj_ic())!= 100)
            {
                iCountErrors++;
                Console.WriteLine( "Err_437sdg! wrong value returned, " + list.BinarySearch(150) );
            }
            strLoc = "Loc_347sg";
            iCountTestcases++;
            if(list.BinarySearch(null, new Co8598BinarySearch_obj_ic())!= -1)
            {
                iCountErrors++;
                Console.WriteLine( "Err_247sg! wrong value returned, " + list.BinarySearch(null, new Co8598BinarySearch_obj_ic()) );
            }
            strLoc = "Loc_87245dsg";
            iCountTestcases++;
            list.Add(null);
            list.Sort();
            if(list.BinarySearch(null, new CompareWithNullEnabled())!= 0)
            {
                iCountErrors++;
                Console.WriteLine( "Err_983475dsg! wrong value returned, " + list.BinarySearch(null, new Co8598BinarySearch_obj_ic()) );
            }
            strLoc = "Loc_9437tsdfg";
            iCountTestcases++;
            list = new ArrayList();
            for(int i=0; i<100; i++)
                list.Add(5);
            list.Sort();
            if(list.BinarySearch(5, new Co8598BinarySearch_obj_ic())!= 49)
            {
                iCountErrors++;
                Console.WriteLine( "Err_3846tsdg! wrong value returned, " + list.BinarySearch(5) );
            }
            strLoc = "Loc_9437tsdfg";
            iCountTestcases++;
            list = new ArrayList();
            for(int i=0; i<100; i++)
                list.Add(5);
            list.Sort();
            if(list.BinarySearch(5, null)!= 49)
            {
                iCountErrors++;
                Console.WriteLine( "Err_3846tsdg! wrong value returned, " + list.BinarySearch(5) );
            }
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.ToString());
        }
        Console.Error.Write( s_strClassMethod );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( s_strTFName + " iCountTestcases==" + iCountTestcases + " paSs" );
            return true;
        }
        else
        {
            Console.WriteLine( s_strTFName+ s_strTFPath );
            Console.WriteLine( s_strTFName+ "FAiL" );
            Console.Error.WriteLine( s_strTFName + " iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public virtual int Compare(Object x, Object y)
    {
        if(x is Int32)
            return ((int)x).CompareTo( y );
        if(x is String)
            return ((String)x).CompareTo( y );
        return -1;    		
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co8598BinarySearch_obj_ic oCbTest = new Co8598BinarySearch_obj_ic();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( s_strTFName+ s_strTFPath );
            Console.WriteLine( s_strTFName+ "E_1000000" );
            Console.WriteLine( s_strTFName+ "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( s_strTFName+ ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
class CompareWithNullEnabled:IComparer
{
    public int Compare(Object a, Object b)
    {
        if (a == b) return 0;
        if (a == null) return -1;
        if (b == null) return 1;
        IComparable ia = a as IComparable;
        if (ia != null)
            return ia.CompareTo(b);
        IComparable ib = b as IComparable;
        if (ib != null)
            return -ib.CompareTo(a);
        throw new ArgumentException("Wrong stuff");
    }
}
