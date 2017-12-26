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
public class Co3773Sort_oo
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.Sort(Object[], Object[])";
 public static String s_strTFName        = "Co3773Sort_oo";
 public static String s_strTFAbbrev      = "Co3773";
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   Object o1;
   Char chValue;
   Int16 i16Value;
   int iReturnValue;
   Object[] oArrValues;
   Object[] oArrKeyValues;
   Object[,] o2ArrValues;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       oArrKeyValues = new Object[cArr.Length];
       for(int i=0; i < cArr.Length; i++)
	 {
	 oArrKeyValues[i]=cArr[i];
	 }
       oArrValues = new Object[i16Arr.Length];
       for(int i=0; i < i16Arr.Length; i++)
	 {
	 oArrValues[i]=i16Arr[i];
	 }
       Array.Sort(oArrKeyValues, oArrValues);
       ++iCountTestcases;
       for(int i=0; i < oArrKeyValues.Length; i++)
	 {
	 o1 = cSortedArr[i];
	 if ( o1.Equals(oArrKeyValues[i]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_320ye_" + i + ", oArrKeyValues[i]==" + oArrKeyValues[i] + " o1==" + o1);
	   }
	 }
       ++iCountTestcases;
       for(int i=0; i < oArrValues.Length; i++)
	 {
	 o1 = iKeySorted16Arr[i];
	 if ( o1.Equals(oArrValues[i]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_6342sd_" + i + ", oArrValues[i]==" + oArrValues[i] + " o1==" + o1);
	   }
	 }
       oArrKeyValues = new Object[i32Arr.Length];
       for(int i=0; i < i32Arr.Length; i++)
	 {
	 oArrKeyValues[i]=i32Arr[i];
	 }
       oArrValues = new Object[gArr.Length];
       for(int i=0; i < gArr.Length; i++)
	 {
	 oArrValues[i]=gArr[i];
	 }
       Array.Sort(oArrKeyValues, oArrValues);
       ++iCountTestcases;
       for(int i=0; i < oArrKeyValues.Length; i++)
	 {
	 o1 = i32SortedArr[i];
	 if ( o1.Equals(oArrKeyValues[i]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_364dfe_" + i + ", oArrKeyValues[i]==" + oArrKeyValues[i] + " o1==" + o1);
	   }
	 }
       ++iCountTestcases;
       for(int i=0; i < oArrValues.Length; i++)
	 {
	 o1 = gKeySortedArr[i];
	 if ( o1.Equals(oArrValues[i]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_648sd_" + i + ", oArrValues[i]==" + oArrValues[i] + " o1==" + o1);
	   }
	 }
       oArrValues = new Object[i32Arr.Length];
       for(int i=0; i < i32Arr.Length; i++)
	 {
	 oArrValues[i]=i32Arr[i];
	 }
       oArrKeyValues = new Object[gArr.Length];
       for(int i=0; i < gArr.Length; i++)
	 {
	 oArrKeyValues[i]=gArr[i];
	 }
       try
	 {
	 ++iCountTestcases;
	 Array.Sort(oArrKeyValues, oArr);
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "Error E_795gd!  , Wrong Exception thrown == " + ex.ToString());
	 Console.WriteLine(ex.StackTrace);
	 }
       oArrKeyValues = new Object[i32Arr.Length];
       for(int i=0; i < i32Arr.Length; i++)
	 {
	 oArrKeyValues[i]=i32Arr[i];
	 }
       Array.Sort(oArrKeyValues, (System.Array)null);
       ++iCountTestcases;
       for(int i=0; i < oArrKeyValues.Length; i++)
	 {
	 o1 = i32SortedArr[i];
	 if ( o1.Equals(oArrKeyValues[i]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_735sfd_" + i + ", oArrKeyValues[i]==" + oArrKeyValues[i] + " o1==" + o1);
	   }
	 }
       try
	 {
	 ++iCountTestcases;
	 Array.Sort(null, oArrValues);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!" );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "Error E_972qr!  , Wrong Exception thrown == " + ex.ToString());
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev + "Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 static Char[]			cArr				 = {'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
 static Char[]			cSortedArr	 = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
 static Int16[]		i16Arr				= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 static Int16[]		iKeySorted16Arr	= {753, -563, 60, -52, 0, 565, 6, 317, 238, 19};
 static Int32[]		i32Arr				= {2, 3, 1};
 static Int32[]		i32SortedArr	= {1, 2, 3};
 static Guid[]			gArr					= {new Guid("86fd6420-1e01-11d3-b67c-00a0c9dea091"), new Guid("86fd6421-1e01-11d3-b67c-00a0c9dea091"), new Guid("86fd6422-1e01-11d3-b67c-00a0c9dea091")};
 static Guid[]			gKeySortedArr	= {new Guid("86fd6422-1e01-11d3-b67c-00a0c9dea091"), new Guid("86fd6420-1e01-11d3-b67c-00a0c9dea091"), new Guid("86fd6421-1e01-11d3-b67c-00a0c9dea091")};
 static Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 static Char[,]		c2Arr		  = {{'a', 'b', 'c', 'd', 'e'},
					     {'k', 'l', 'm', 'n', 'o'}};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3773Sort_oo oCbTest = new Co3773Sort_oo();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev + "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFAbbrev + s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
