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
public class Co3777AsList
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.AsList(Object[])";
 public static String s_strTFName        = "Co3777AsList";
 public static String s_strTFAbbrev      = "Co3777";
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
   Object[,] o2ArrValues;
   ArrayList lst1;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="536df";
       lst1 = new ArrayList( (cArr) );
       lst1.Sort();
       if ( cArr.Length != lst1.Count)
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + ", Err_623sd! cArr.Length==" + cArr.Length);
	 }
       ++iCountTestcases;
       for(int i=0; i<cArr.Length; i++)
	 {
	 chValue = cArr[i];
	 if ( lst1.BinarySearch(0, lst1.Count, chValue, null) < 0 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_320ye_" + i + " chValue==" + chValue);
	   }
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
 static Int16[]		iSorted16Arr	= {-563, -52, 0, 6, 19, 60, 238, 317, 565, 753};
 static Guid[]			gArr					= {new Guid(), new Guid()};
 static Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 static Char[,]		c2Arr		  = {{'a', 'b', 'c', 'd', 'e'},
					     {'k', 'l', 'm', 'n', 'o'}};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3777AsList oCbTest = new Co3777AsList();
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
