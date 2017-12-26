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
public class Co4346ctor_dictionary_IComparer
    :
    IComparer
{
    internal static String strName = "SortedList.Constructor_dictionary_IComparer";
    internal static String strTest = "Co4346ctor_dictionary_IComparer";
    internal static String strPath = "";
    public virtual int Compare  
        (
        Object p_obj1
        ,Object p_obj2
        )
    {
        return String.Compare( p_obj1.ToString() ,p_obj2.ToString() );
    }
    public virtual bool runTest()
    {
        Console.Error.WriteLine( "Co4346ctor_dictionary_IComparer  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        Hashtable ht = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        int[] in4a = new int[9];
        try
        {
            do
            {
                iCountTestcases++;
                ht = new Hashtable();
                ht.Add ("key_2", "val_2"); 
                ht.Add ("key_0", "val_0"); 
                ht.Add ("key_1", "val_1"); 
                strLoc="100cc";
                iCountTestcases++;
                try 
                {
                    sl2 = new SortedList( ht, this );
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_l30r! (Co4346ctor_dictionary_IComparer) exc ==" + exc  );
                }
                strLoc="101cc";
                iCountTestcases++;
                if ( sl2 == null )
                {
                    Console.WriteLine( strTest+ "E_101" );
                    Console.WriteLine( strTest+ "SortedList creation failure" );
                    ++iCountErrors;
                    break;
                }
                strLoc="102cc";
                iCountTestcases++;
                if ( sl2.Count  != 3 )
                {
                    Console.WriteLine( strTest+ "E_102" );
                    Console.WriteLine( strTest+ "New SortedList is not empty" );
                    ++iCountErrors;
                }
                strLoc="103cc";
                iCountTestcases++;
                if (!((String)sl2["key_0"]).Equals("val_0")) 
                {
                    Console.WriteLine( strTest+ "E_103" );
                    Console.WriteLine( strTest+ ("SortedList order not correct - index 0: "+(String)sl2["key_0"]) );
                    ++iCountErrors;
                }
                strLoc="104cc";
                iCountTestcases++;
                if (!((String)sl2["key_1"]).Equals("val_1")) 
                {
                    Console.WriteLine( strTest+ "E_104" );
                    Console.WriteLine( strTest+ "SortedList order not correct - index 1" );
                    ++iCountErrors;
                }
                strLoc="105cc";
                iCountTestcases++;
                if (!((String)sl2["key_2"]).Equals("val_2")) 
                {
                    Console.WriteLine( strTest+ "E_105" );
                    Console.WriteLine( strTest+ "SortedList order not correct - index 2" );
                    ++iCountErrors;
                }
                strLoc="106cc";
                iCountTestcases++;
                try 
                {
                    sl2 = new SortedList( ht, null );
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_001dt! (Co4346ctor_dictionary_IComparer) exc ==" + exc  );
                }
                strLoc="107cc";
                iCountTestcases++;
                try 
                {
                    sl2 = new SortedList( null, this );
                    ++iCountErrors;
                    Console.Error.WriteLine( "Err_002dt,  expected ArgumentNullException to be thrown but it was not" );
                }
                catch (ArgumentNullException)
                {}
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_003dt! (Co4346ctor_dictionary_IComparer) exc ==" + exc  );
                }
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4346ctor_dictionary_IComparer) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4346ctor_dictionary_IComparer.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4346ctor_dictionary_IComparer.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4346ctor_dictionary_IComparer cbA = new Co4346ctor_dictionary_IComparer();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4346ctor_dictionary_IComparer) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4346ctor_dictionary_IComparer.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
