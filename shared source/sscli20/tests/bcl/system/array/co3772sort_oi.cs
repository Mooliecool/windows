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
class ICmpImplementor : System.Collections.IComparer
{
 public int Compare(Object x, Object y)
   {
   Object v1 = x;
   Object v2 = y;
   if( (int)v1 > (int)v2)
     return 1;
   else if((int)v1 < (int)v2)
     return -1;
   else return 0;
   }
}
public class Co3772Sort_oi
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.Sort(Object[], IComparer)";
 public static String s_strTFName        = "Co3772Sort_oi";
 public static String s_strTFAbbrev      = "Co3772";
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
   Object[,] o2ArrValues;
   ICmpImplementor icmp1 = new ICmpImplementor();
   try
     {
     LABEL_860_GENERAL:
     do
       {
       oArrValues = new Object[vArr.Length];
       for(int i=0; i < vArr.Length; i++)
	 {
	 oArrValues[i]=vArr[i];
	 }
       Array.Sort(oArrValues, icmp1);
       ++iCountTestcases;
       ++iCountTestcases;
       for(int i=0; i < oArrValues.Length; i++)
	 {
	 o1 = iSortedvArr[i];
	 if ( o1.Equals(oArrValues[i]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_246sdf_" + i + ", oArrValues[i]==" + oArrValues[i] + " o1==" + o1);
	   }
	 }
       oArrValues = new Object[cArr.Length];
       for(int i=0; i < cArr.Length; i++)
	 {
	 oArrValues[i]=cArr[i];
	 }
       Array.Sort(oArrValues, (System.Collections.IComparer)null);
       ++iCountTestcases;
       for(int i=0; i < oArrValues.Length; i++)
	 {
	 o1 = cSortedArr[i];
	 if ( o1.Equals(oArrValues[i]) != true )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_246sdf_" + i + ", oArrValues[i]==" + oArrValues[i] + " o1==" + o1);
	   }
	 }
       oArrValues = new Object[gArr.Length];
       for(int i=0; i < gArr.Length; i++)
	 {
	 oArrValues[i]=gArr[i];
	 }
       try
	 {
	 ++iCountTestcases;
	 Array.Sort(oArr, (System.Collections.IComparer)null);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_984fd!" );
	 }
       catch(InvalidOperationException ex)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "Error E_795gd!  , Wrong Exception thrown == " + ex.ToString());
	 Console.WriteLine(ex.StackTrace);
	 }
       oArrValues = new Object[cArr.Length];
       for(int i=0; i < cArr.Length; i++)
	 {
	 oArrValues[i]=cArr[i];
	 }
       try
	 {
	 ++iCountTestcases;
	 Array.Sort(null, icmp1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!");
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(" Error E_972qr!  , Wrong Exception thrown == " + ex.ToString());
	 Console.WriteLine(ex.StackTrace);
	 }
       oArrValues = new Object[cArr.Length];
       for(int i=0; i < cArr.Length; i++)
	 {
	 oArrValues[i]=cArr[i];
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
 static Object[]	vArr				= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 static Object[]	iSortedvArr	= {-563, -52, 0, 6, 19, 60, 238, 317, 565, 753};
 static Guid[]			gArr					= {new Guid(), new Guid()};
 static Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 static Char[,]		c2Arr		  = {{'a', 'b', 'c', 'd', 'e'},
					     {'k', 'l', 'm', 'n', 'o'}};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3772Sort_oi oCbTest = new Co3772Sort_oi();
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
