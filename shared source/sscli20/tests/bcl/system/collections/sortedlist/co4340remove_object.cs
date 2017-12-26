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
public class Co4340Remove_object
    :
    IComparer
{
    internal static String strName = "SortedList.Remove_object";
    internal static String strTest = "Co4340Remove_object";
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
        Console.Error.WriteLine( "Co4340Remove_object  runTest() started." );
        String strLoc="Loc_000oo";
        StringBuilder sblMsg = new StringBuilder( 99 );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        SortedList sl2 = null;
        StringBuilder sbl3 = new StringBuilder( 99 );
        StringBuilder sbl4 = new StringBuilder( 99 );
        StringBuilder sblWork1 = new StringBuilder( 99 );
        String str5 = null;
        String str6 = null;
        String str7 = null;
        String s1 = null;
        String s2 = null;
        String s3 = null;
        int[] in4a = new int[9];
        int nCapacity = 100;
        bool bol = false;
        int i = 0;
        try
        {
        LABEL_860_GENERAL:
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
                if (s3 != null) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_101bc! " );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                strLoc="Loc_141aa";
                for (i=0; i<100; i++) 
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
                iCountTestcases++;
                if (sl2.Count  != 100) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_hbdw! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                for (i=0; i < 100; i++) 
                {
                    ++iCountTestcases;
                    sblMsg.Length =  0 ;
                    sblMsg.Append("key_");
                    sblMsg.Append(i);
                    s1 = sblMsg.ToString();
                    sl2.Remove (s1); 
                    if (sl2.Count  != (100-(i+1))) 
                    {
                        ++iCountErrors;
                        sblMsg.Length =  0 ;
                        sblMsg.Append( "POINTTOBREAK: Error Err_hf49! - Count == " );
                        sblMsg.Append( sl2.Count  );
                        Console.Error.WriteLine(  sblMsg.ToString()  );
                    }
                }
                ++iCountTestcases;
                sblMsg.Length =  0 ;
                sblMsg.Append("No_Such_Key");
                s1 = sblMsg.ToString();
                try 
                {
                    sl2.Remove (s1); 
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_38db! - "  + exc.ToString() );
                }
                ++iCountTestcases;
                try 
                {
                    sl2.Remove ((String) null);
                }
                catch (ArgumentException aexc) 
                {
                }
                catch (Exception exc) 
                {
                    ++iCountErrors;
                    Console.Error.WriteLine(  "POINTTOBREAK: Error Err_vh40! - "  + exc.ToString() );
                }
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4340Remove_object) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4340Remove_object.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4340Remove_object.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4340Remove_object cbA = new Co4340Remove_object();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4340Remove_object) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4340Remove_object.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
