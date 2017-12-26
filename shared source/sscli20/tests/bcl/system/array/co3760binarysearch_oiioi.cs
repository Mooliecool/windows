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
internal class TestICmp : System.Collections.IComparer
{
 public virtual int Compare(Object x, Object y)
   {
   if ((int)x > (int)y)
     return 1;
   else if((int)x < (int)y)
     return -1;
   else
     return 0;
   }
}
public class Co3760BinarySearch_oiioi
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Array.BinarySearch(Object[], int Index, int Count, Object, Icomparer)";
 public static readonly String s_strTFName        = "Co3760BinarySearch_oiioi";
 public static readonly String s_strTFAbbrev      = "Co3760";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
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
   int iIndex;
   int iCount;
   TestICmp icmp1 = new TestICmp();
   try
     {
     LABEL_860_GENERAL:
     do
       {
       oArrValues = new Object[i32Arr.Length];
       for(int i=0; i < i32Arr.Length; i++)
	 {
	 oArrValues[i]=i32Arr[i];
	 }
       Array.Sort(oArrValues);
       strLoc="375yr";
       ++iCountTestcases;
       iCount=6;
       for(int i=2; i < 8; i++)
	 {
	 strLoc="863gq_"+i;
	 o1 = oArrValues[i];
	 iReturnValue = Array.BinarySearch(oArrValues, i, iCount, o1, icmp1);
	 if ( iReturnValue != i )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_947ps_" + i + ", iReturnValue=="+ iReturnValue );
	   }
	 iCount--;
	 }
       try
	 {
	 o1 = 1;
	 ++iCountTestcases;
	 iReturnValue = Array.BinarySearch(null, 0, 2, o1, icmp1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!  iReturnValue==" + iReturnValue );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_972qr!  , Wrong Exception thrown == " + ex.ToString() );
	 }
       oArrValues = new Object[i32Arr.Length];
       for(int i=0; i < i32Arr.Length; i++)
	 {
	 oArrValues[i]=i32Arr[i];
	 }
       try
	 {
	 o1 = 1;
	 ++iCountTestcases;
	 iReturnValue = Array.BinarySearch(oArrValues, -2, 2, o1, icmp1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_407hr!  iReturnValue==" + iReturnValue );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_024df!  , Wrong Exception thrown == " + ex.ToString() );
	 }
       try
	 {
	 o1 = 1;
	 ++iCountTestcases;
	 iReturnValue = Array.BinarySearch(oArrValues, 2, -2, o1, icmp1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_948ds!  iReturnValue==" + iReturnValue );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_902sa!  , Wrong Exception thrown == " + ex.ToString() );
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
 internal static readonly Char[]			cArr	  = {'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
 internal static readonly Int32[]		i32Arr	= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753};
 internal static readonly Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 internal static readonly Char[,]		c2Arr		  = {{'a', 'b', 'c', 'd', 'e'},
								     {'k', 'l', 'm', 'n', 'o'}};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3760BinarySearch_oiioi oCbTest = new Co3760BinarySearch_oiioi();
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
