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
public class Co4345TrimToSize
    :
    IComparer
{
    internal static String strName = "SortedList.TrimToSize";
    internal static String strTest = "Co4345TrimToSize";
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
        Console.Error.WriteLine( "Co4345TrimToSize  runTest() started." );
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
                for (i=0; i<32; i++) 
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
                ++iCountTestcases;
                sl2.Add ("key_32", "val_32");
                ++iCountTestcases;
                if (sl2.Count  != 33) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_gh40! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                for (i=0; i < 10; i++) 
                {
                    ++iCountTestcases;
                    sl2.Remove ("key_" + i.ToString()); 
                }
                strLoc="Loc_141aa";
                ++iCountTestcases;
                if (sl2.Count  != 23) 
                {
                    ++iCountErrors;
                    sblMsg.Length =  0 ;
                    sblMsg.Append( "POINTTOBREAK: Error Err_gh45! - Count == " );
                    sblMsg.Append( sl2.Count  );
                    Console.Error.WriteLine(  sblMsg.ToString()  );
                }
                ++iCountTestcases;
                sl2.TrimToSize ();
                ++iCountTestcases;
                sl2.Clear();
                ++iCountTestcases;
                sl2.TrimToSize ();
            } while ( false );
        }
        catch( Exception exc_general )
        {
            ++iCountErrors;
            Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4345TrimToSize) exc_general==" + exc_general  );
            Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( "paSs.   SortedList\\Co4345TrimToSize.cs   iCountTestcases==" + iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine( "FAiL!   SortedList\\Co4345TrimToSize.cs   iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        StringBuilder sblMsg = new StringBuilder( 99 );
        Co4345TrimToSize cbA = new Co4345TrimToSize ();
        try
        {
            bResult = cbA.runTest();
        }
        catch ( Exception exc_main )
        {
            bResult = false;
            Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4345TrimToSize) Uncaught Exception caught in main(), exc_main==" + exc_main  );
        }
        if ( ! bResult )
            Console.Error.WriteLine(  "Co4345TrimToSize.cs   FAiL!"  );
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
