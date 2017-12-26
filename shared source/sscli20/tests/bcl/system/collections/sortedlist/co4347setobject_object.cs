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
public class Co4347SetObject_object
    :
    IComparer
{
    internal static String strName = "SortedList.Set_object_object";
    internal static String strTest = "Co4347SetObject_object";
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
        Console.Error.WriteLine( "Co4347SetObject_object  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        String s1 = null;
        String s2 = null;
        String s3 = null;
        int[] in4a = new int[9];
        bool bol = false;
        int i = 0;
        try
        {
            do
            {
                strLoc="100cc";
                iCountTestcases++;
                sl2 = new SortedList( this );
                iCountTestcases++;
                if ( sl2 == null )
                {
                    Console.WriteLine( strTest+ "E_101" );
                    Console.WriteLine( strTest+ "SortedList creation failure" );
                    ++iCountErrors;
                    break;
                }
                iCountTestcases++;
                if ( sl2.Count  != 0 )
                {
                    Console.WriteLine( strTest+ "E_102" );
                    Console.WriteLine( strTest+ "New SortedList is not empty" );
                    ++iCountErrors;
                }
                strLoc="Loc_100aa";
                ++iCountTestcases;
                try 
                {
                    sl2[null] = "first value" ; 
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_2764! - must 've thrown ArgExc" );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                catch (ArgumentException) {}
                ++iCountTestcases;
                if (sl2.Count  != 0) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_48ff! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_110aa";
                ++iCountTestcases;
                try 
                {
                    sl2["first key"] = (String) null ; 
                }
                catch (ArgumentException ) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_34fj! - must 've thrown ArgExc" );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                ++iCountTestcases;
                if (sl2.Count  != 1) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_60ff! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_120aa";
                ++iCountTestcases;
                sl2[(int) 0] = "first value" ;
                if (sl2.Count  != 2) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_2i20bc! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_130aa";
                ++iCountTestcases;
                bol = sl2.ContainsKey ("first key");
                if (!bol) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_213h! - bol  == " );
                    sblMsg.Append( bol );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_140aa";
                ++iCountTestcases;
                sl2["first key"] = "first value";
                s2 = (String) sl2["first key"] ;
                if (!s2.Equals("first value")) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_1uid! - s2.Equals(first value)  == " );
                    sblMsg.Append( s2.Equals("first value") );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_150aa";
                ++iCountTestcases;
                sl2["first key"] = "second value";
                if (sl2.Count  != 2) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_58fj! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_160aa";
                ++iCountTestcases;
                s2 = (String) sl2["first key"] ;
                if (!s2.Equals("second value")) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_49d! - s2.Equals(second value)  == " );
                    sblMsg.Append( s2.Equals("second value") );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                sl2.Clear();
                strLoc="Loc_141aa";
                for (i=0; i<50; i++) 
                {
                    ++iCountTestcases;
                    sblMsg.Length =  0 ;
                    sblMsg.Append("key_");
                    sblMsg.Append(i);
                    s1 = sblMsg.ToString();
                    sblMsg.Length =  0 ;
                    sblMsg.Append("val_");
                    sblMsg.Append(i);
                    s2 = sblMsg.ToString();
                    sl2.Add (s1, s2);
                }
                strLoc="Loc_145aa";
                for (i=0; i<50; i++) 
                {
                    ++iCountTestcases;
                    ++iCountTestcases;
                    sblMsg.Length =  0 ;
                    sblMsg.Append("key_");
                    sblMsg.Append(i);
                    s1 = sblMsg.ToString();
                    sblMsg.Length =  0 ;
                    sblMsg.Append("new_val_");
                    sblMsg.Append(i);
                    s2 = sblMsg.ToString();
                    sl2[s1] = s2;
                }
                strLoc="Loc_125aa";
                ++iCountTestcases;
                if (sl2.Count  != 50) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_294y! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_145aa";
                for (i=0; i<50; i++) 
                {
                    ++iCountTestcases;
                    sblMsg.Length =  0 ;
                    sblMsg.Append("key_");
                    sblMsg.Append(i);
                    s1 = sblMsg.ToString();
                    sblMsg.Length =  0 ;
                    sblMsg.Append("new_val_");
                    sblMsg.Append(i);
                    s2 = sblMsg.ToString();
                    s3 = (String) sl2[s1] ;
                    if (!s3.Equals(s2)) 
                    {
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_49d! - s3.Equals(s2)  == " );
                        sblMsg.Append( s2.Equals(s2) );
                        sblMsg.Append( " for key == " );
                        sblMsg.Append( s1 + "; returned Val == " + s3 );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                    }
                }
            } while ( false );
        }
        catch( Exception exc_general ) 
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4347SetObject_object) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4347SetObject_object.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4347SetObject_object.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4347SetObject_object cbA = new Co4347SetObject_object();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4347SetObject_object) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4347SetObject_object.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
