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
using System.Globalization;
class Co1657CopyTo_ai
{
    public String s_strActiveBugNums                 = "";
    public static String s_strDtTmVer                = "";
    public static String s_strComponentBeingTested   = "CopyTo( Array, int)";
    public String s_strTFName                        = "Co1657CopyTo_ai.cs";
    public String s_strTFAbbrev                      = "Co1657";
    public String s_strTFPath                        = "";
    public Boolean verbose                           = false;
    public Boolean runTest()
    {
        Console.Error.WriteLine( s_strTFPath + " " + s_strTFName + " , for " + s_strComponentBeingTested + "  ,Source ver " + s_strDtTmVer );
        String strLoc        = "Loc_000ooo";
        int iCountTestcases = 0;
        int iCountErrors    = 0;
        Hashtable hash       = null;        
        Object [] objArr     = null;        
        Object [] objArr2    = null;        
        Object [][] objArrMDim = null;		
        Object [,] objArrRMDim = null;		
        Object [] keys       = new Object [] {
                                                 new Object(),
                                                 "Hello" ,
                                                 "my array" ,
                                                 new DateTime(),
                                                 new JulianCalendar(),
                                                 typeof( System.Environment ),
                                                 5
                                             };
        Object [] values     = new Object[] {
                                                "SomeString" ,
                                                new Object(),
                                                new int [] { 1, 2, 3, 4, 5 },
                                                new Hashtable(),
                                                new Exception(),
                                                new Co1657CopyTo_ai(),
                                                null
                                            };
        if ( verbose ) Console.WriteLine( "test normal conditions, array is large enough to hold all elements" );
        try
        {
            strLoc = "Err_0001a";
            ++iCountTestcases;
            hash = new Hashtable();
            for ( int i = 0; i < values.Length; i++ )
            {
                hash.Add( keys[i], values[i] );
            }
            objArr = new Object [values.Length + 2];
            objArr[0] =  "startstring" ;
            objArr[values.Length+1] =  "endstring" ;
            hash.Values.CopyTo( (Array)objArr, 1 );
            if ( ! "startstring".Equals( objArr[0] ) )
            {
                ++iCountErrors;
                Console.WriteLine( "Location: Err_0001b" );
                Console.WriteLine( "StartSentinal was overwritten by something" );
            }
            else if ( ! "endstring".Equals( objArr[values.Length+1] ) )
            {
                ++iCountErrors;
                Console.WriteLine( "Location: Err_0001c" );
                Console.WriteLine( "EndSentinal was overwritten by something" );
            }
            objArr2 = new Object[ values.Length ];
            Array.Copy( objArr, 1, objArr2, 0, values.Length );
            objArr = objArr2;
            if ( ! CompareArrays( objArr, values ) )
            {
                ++iCountErrors;
                Console.WriteLine( "Location: Err_0001d" );
                Console.WriteLine( "values do not match values which were inserted" );
            }
            try
            {
                ++iCountTestcases;
                hash = new Hashtable();
                objArr = new Object[0];
                hash.CopyTo( objArr, Int32.MaxValue );
                ++iCountErrors;
                Console.WriteLine( "Err_015a,  Expected ArgumentException but no exception thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_015b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
            }
            try
            {
                ++iCountTestcases;
                hash = new Hashtable();
                objArr = new Object[0];
                hash.CopyTo( objArr, Int32.MinValue );
                ++iCountErrors;
                Console.WriteLine( "Err_015a,  Expected ArgumentException but no exception thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch (Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_015b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
            }
            if ( verbose ) Console.WriteLine( "copy should throw because of outofrange" );
            Random random = new Random();
            for(int iii=0; iii<20; iii++)
            {
                try
                {
                    ++iCountTestcases;
                    hash = new Hashtable();
                    objArr = new Object[0];
                    hash.CopyTo( objArr, random.Next(-1000, 0));
                    ++iCountErrors;
                    Console.WriteLine( "Err_015a,  Expected ArgumentException but no exception thrown" );
                }
                catch ( ArgumentException )
                {
                }
                catch (Exception ex)
                {
                    ++iCountErrors;
                    Console.WriteLine( "Err_015b,  Expected ArgumentException but exception thrown= " + ex.ToString() );
                }
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when array is to small to hold all values hashtable has more values then array can hold" );
        try
        {
            strLoc = "Err_0002a";
            ++iCountTestcases;
            hash = new Hashtable();
            for ( int i = 0; i < values.Length; i++ )
            {
                hash.Add( keys[i], values[i] );
            }
            objArr = new Object [values.Length - 1];
            try
            {
                hash.Values.CopyTo( (Array) objArr, 0 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " method to CopyTo should throw an exception when array is smaller then num of elements in hashtable, no exception was thrown here" );
            }
            catch ( ArgumentException )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() + " and expected ArgumentException" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when array is size 0" );
        try
        {
            strLoc = "Err_0003a";
            ++iCountTestcases;
            hash = new Hashtable();
            for ( int i = 0; i < values.Length; i++ )
            {
                hash.Add( keys[i], values[i] );
            }
            objArr = new Object [0];
            try
            {
                hash.Values.CopyTo( (Array) objArr, 0 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " method to CopyTo should throw an exception when array is size 0, no exception was thrown here" );
            }
            catch ( ArgumentException )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() + " and expected ArgumentException" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when array is null" );
        try
        {
            strLoc = "Err_0004a";
            ++iCountTestcases;
            hash = new Hashtable();
            for ( int i = 0; i < values.Length; i++ )
            {
                hash.Add( keys[i], values[i] );
            }
            try
            {
                hash.Values.CopyTo( null, 0 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " method to CopyTo should throw an exception when array is null, no exception was thrown here" );
            }
            catch ( ArgumentNullException )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() + " and expected ArgumentNullException" );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when hashtable has no elements in it" );
        try
        {
            strLoc = "Err_0005a";
            ++iCountTestcases;
            hash = new Hashtable();
            try
            {
                strLoc = "Err_0005c";
                objArr = new Object[100];
                hash.Values.CopyTo( objArr, 0 );
                strLoc = "Err_0005d";
                objArr = new Object[100];
                hash.Values.CopyTo( objArr, 99 );
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when hashtable has no elements in it and index is out of range" );
        try
        {
            strLoc = "Err_0006a";
            ++iCountTestcases;
            hash = new Hashtable();
            try
            {
                strLoc = "Err_0006b";
                objArr = new Object[100];
                hash.Values.CopyTo( objArr, 100 );
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
            }
        }
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when hashtable has no elements in it trying to copy to array of size 0" );
        try
        {
            strLoc = "Err_0007a";
            ++iCountTestcases;
            hash = new Hashtable();
            try
            {
                strLoc = "Err_0007b";
                objArr = new Object[0];
                hash.Values.CopyTo( objArr, 0 );
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " expected ArgumentOutOfRangeException but got exception, we got "+ ex.ToString() );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when hashtable has no elements in it and index is out of range (negative)" );
        strLoc = "Err_0007aa";
        try
        {
            ++iCountTestcases;
            hash = new Hashtable();
            try
            {
                strLoc = "Err_0007bb";
                objArr = new Object[0];
                hash.Values.CopyTo( objArr, -1 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " expected ArgumentOutOfRange exception but no exception was thrown" );
            }
            catch ( ArgumentOutOfRangeException )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
            }
        }
        catch ( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when array is multi dimensional and array is large enough" );
        try
        {
            strLoc = "Err_0008a";
            ++iCountTestcases;
            hash = new Hashtable();
            for ( int i = 0; i < 100; i++ )
            {
                hash.Add( i.ToString(), i.ToString() );
            }
            try
            {
                strLoc = "Err_0008b1";
                objArrMDim = new Object[100][];
                for ( int i = 0; i < 100; i++ )
                {
                    objArrMDim[i] = new Object[i+1];
                }
                hash.Values.CopyTo( objArrMDim, 0 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " expected ArgumentRange exception but no exception was thrown" );
            }
            catch ( InvalidCastException )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
            }
            try
            {
                strLoc = "Err_0008b";
                objArrRMDim = new Object[100,100];
                hash.Values.CopyTo( objArrRMDim, 0 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " expected ArgumentRange exception but no exception was thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test when array is multi dimensional and array is small" );
        try
        {
            strLoc = "Err_0009a";
            ++iCountTestcases;
            hash = new Hashtable();
            for ( int i = 0; i < 100; i++ )
            {
                hash.Add( i.ToString(), i.ToString() );
            }
            try
            {
                strLoc = "Err_0009b";
                objArrMDim = new Object[99][];
                for ( int i = 0; i < 99; i++ )
                {
                    objArrMDim[i] = new Object[i+1];
                }
                hash.Values.CopyTo( objArrMDim, 0 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " expected ArgumentException but no exception was thrown" );
            }
            catch ( ArgumentException )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " unexpected exception, we got "+ ex.ToString() );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( verbose ) Console.WriteLine( "test to see if CopyTo throws correct exception" );
        try
        {
            strLoc = "Err_00010a";
            ++iCountTestcases;
            hash = new Hashtable();
            try
            {
                String [] str = new String[100];
                hash.Values.CopyTo( str, 101 );
                ++iCountErrors;
                Console.WriteLine( strLoc + " expected ArgumentException but no exception was thrown" );
            }
            catch ( ArgumentException  )
            {
            }
            catch ( Exception ex )
            {
                ++iCountErrors;
                Console.WriteLine( strLoc + " expected ArgumentException exception, we got "+ ex.ToString() );
            }
        }
        catch (Exception ex)
        {
            ++iCountErrors;
            Console.WriteLine( "Location: " + strLoc );
            Console.WriteLine( "Unexpected exception was thrown ex: " + ex.ToString() );
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine("paSs.  " + s_strTFPath + s_strTFName + "  iCountTestcases==" + iCountTestcases.ToString ());
            return true;
        }
        else
        {
            Console.WriteLine("Related Bugs: " + s_strActiveBugNums );
            Console.WriteLine("FAiL!   " + s_strTFPath + s_strTFName + "  iCountErrors==" + iCountErrors.ToString ());
            return false;
        }
    }
    public static Boolean CompareArrays( Object [] arr1, Object [] arr2 )
    {
        if ( arr1.Length != arr2.Length )
        {
            Console.WriteLine( "Arrays do not mach in length" );
            return false;
        }
        int i, j;
        Boolean fPresent = false;
        for (  i = 0; i < arr1.Length; i++ )
        {
            fPresent = false;
            for (  j = 0; j < arr2.Length && ( fPresent == false ); j++ )
            {
                if (  (arr1[i] == null && arr2[j] == null )
                    ||
                    (arr1[i] != null && arr1[i].Equals(arr2[j]))   )
                {
                    fPresent = true;
                }
            }
            if ( fPresent == false )
            {
                Console.WriteLine("Element " + i.ToString() + " from arr1 does not exist in arr2");
                return false;
            }
        }
        for (  i = 0; i < arr2.Length; i++ )
        {
            fPresent = false;
            for (  j = 0; j < arr1.Length && (fPresent == false); j++ )
            {
                if (  (arr2[i] == null && arr1[j] == null )
                    ||
                    (arr2[i] != null && arr2[i].Equals(arr1[j]))   )
                {
                    fPresent = true;
                }
            }
            if ( fPresent == false )
            {
                Console.WriteLine("Element " + i.ToString() + " from arr2 does not exist in arr1");
                Console.WriteLine("  this element is of type " + arr2[i].GetType().Name );
                return false;
            }
        }
        return true;
    }
    public static void Main( String [] args )
    {
        Co1657CopyTo_ai Co1657 = new Co1657CopyTo_ai();
        Boolean bResult = Co1657.runTest();
        if ( ! bResult )
        {
            Console.WriteLine( Co1657.s_strTFPath + Co1657.s_strTFName );
            Console.Error.WriteLine( " " );
            Console.Error.WriteLine( "FAiL!  "+ Co1657.s_strTFAbbrev );  
            Console.Error.WriteLine( " " );
            Console.Error.WriteLine( "ACTIVE BUGS: " + Co1657.s_strActiveBugNums ); 
        }
        if ( bResult == true ) Environment.ExitCode = 0;
        else Environment.ExitCode = 1; 
    }
}  
