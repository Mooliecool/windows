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
using System.Text;
using System;
using System.Collections;
public class Co4324ctor_dictionary : Object, IComparable
{
    internal String str = null; 
    internal static String strName = "SortedList.Constructor_dictionary";
    internal static String strTest = "Co4324ctor_dictionary";
    internal static String strPath = "";
    public Co4324ctor_dictionary () {}
    public Co4324ctor_dictionary (String tstr) 
    {
        this.str = tstr;
    }
    public virtual int CompareTo (Object p_obj2) 
    { 
        return str.CompareTo ( p_obj2.ToString() );
    }
    public override bool Equals(Object obj) 
    {
        return str.Equals (obj.ToString());
    }
    public override int GetHashCode() 
    {
        return str.GetHashCode();
    }
    public override String ToString () 
    {
        return str.ToString();
    }
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co4324ctor_dictionary  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        Hashtable ht = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        String str5 = null;
        String str6 = null;
        String str7 = null;
        int[] in4a = new int[9];
        int nCapacity = 100;
        try
        {
        LABEL_860_GENERAL:
            do
            {
                strLoc="100cc";
                iCountTestcases++;
                ht = new Hashtable();
                Co4324ctor_dictionary k0 = new Co4324ctor_dictionary ("cde");
                Co4324ctor_dictionary k1 = new Co4324ctor_dictionary ("abc");
                Co4324ctor_dictionary k2 = new Co4324ctor_dictionary ("bcd");
                ht.Add (k0, null);
                ht.Add (k1, null);
                ht.Add (k2, null);
                iCountTestcases++;
                sl2 = new SortedList(ht);
                iCountTestcases++;
                if ( sl2 == null )
                {
                    Console.WriteLine( strTest+ "E_101" );
                    Console.WriteLine( strTest+ "SortedList creation failure" );
                    ++iCountErrors;
                    break;
                }
                iCountTestcases++;
                if ( sl2.Count  != 3 )
                {
                    Console.WriteLine( strTest+ "E_202" );
                    Console.WriteLine( strTest+ "New SortedList is not empty" );
                    ++iCountErrors;
                }
                iCountTestcases++;
                if (sl2.IndexOfKey(k0) != 2) 
                {
                    Console.Error.WriteLine(  "POINTTOBREAK: (E_12ys.1) IndexOfKey(k0)==" +
                        sl2.IndexOfKey(k0) );
                    ++iCountErrors;
                }
                iCountTestcases++;
                if (sl2.IndexOfKey(k1) != 0) 
                {
                    Console.Error.WriteLine( "POINTTOBREAK: (E_12ys.2) IndexOfKey(k1)=="
                        + sl2.IndexOfKey(k1) ) ;
                    ++iCountErrors;
                }
                iCountTestcases++;
                if (sl2.IndexOfKey(k2) != 1) 
                {
                    Console.Error.WriteLine( "POINTTOBREAK: (E_12ys.3) IndexOfKey(k2)=="
                        + sl2.IndexOfKey(k2) ) ;
                    ++iCountErrors;
                }
                iCountTestcases++;
                if ( !((Co4324ctor_dictionary)sl2.GetKey(0)).ToString().Equals ("abc"))
                {
                    Console.Error.WriteLine(  "POINTTOBREAK: (E_14dj.1) Equals(k0)=="
                        + ((Co4324ctor_dictionary)sl2.GetKey(0)).ToString().Equals ("abc") ) ;
                    ++iCountErrors;
                }
                iCountTestcases++;
                if ( !((Co4324ctor_dictionary)sl2.GetKey(1)).ToString().Equals ("bcd"))
                {
                    Console.Error.WriteLine( "POINTTOBREAK: (E_14dj.1) Equals(k1)=="
                        + ((Co4324ctor_dictionary)sl2.GetKey(1)).ToString().Equals ("bcd") ) ;
                    ++iCountErrors;
                }
                iCountTestcases++;
                if ( !((Co4324ctor_dictionary)sl2.GetKey(2)).ToString().Equals ("cde"))
                {
                    Console.Error.WriteLine(  "POINTTOBREAK: (E_14dj.1) Equals(k2)=="
                        + ((Co4324ctor_dictionary)sl2.GetKey(2)).ToString().Equals ("cde") ) ;
                    ++iCountErrors;
                }
                iCountTestcases++;
                ht = new Hashtable();
                sl2 = new SortedList(ht);
                if(sl2.Count!=0)
                {
                    ++iCountErrors;
                    Console.WriteLine("Err_3947tsg! wrong value returned");
                }
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4324ctor_dictionary) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4324ctor_dictionary.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4324ctor_dictionary.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4324ctor_dictionary cbA = new Co4324ctor_dictionary("abc");
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4324ctor_dictionary) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4324ctor_dictionary.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
