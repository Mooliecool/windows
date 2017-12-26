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
public class Co3762IndexOf_ooi
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Array.IndexOf(Object[], Object, int StartIndex)";
 public static readonly String s_strTFName        = "Co3762IndexOf_ooi";
 public static readonly String s_strTFAbbrev      = "Co3762";
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
   int iStartIndex;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       oArrValues = new Object[cArr.Length];
       for(int i=0; i < cArr.Length; i++)
	 {
	 oArrValues[i]=cArr[i];
	 }
       ++iCountTestcases;
       for(int i=0; i < cArr.Length; i++)
	 {
	 o1 = cArr[i];
	 iReturnValue = Array.IndexOf(oArrValues, o1, i);
	 if ( iReturnValue != i )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_320ye_" + i + ", iReturnValue=="+ iReturnValue );
	   }
	 }
       oArrValues = new Object[i16Arr.Length];
       for(int i=0; i < i16Arr.Length; i++)
	 {
	 oArrValues[i]=i16Arr[i];
	 }
       ++iCountTestcases;
       for(int i=0; i < oArrValues.Length; i++)
	 {
	 o1 = i16Arr[i];
	 iReturnValue = Array.IndexOf(oArrValues, o1, i);
	 if ( iReturnValue != i )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_947ps_" + i + ", iReturnValue=="+ iReturnValue );
	   }
	 }
       try
	 {
	 o1 = 16;
	 ++iCountTestcases;
	 iReturnValue = Array.IndexOf(null, o1, 0);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!  iReturnValue=="+ iReturnValue );
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
       oArrValues = new Object[i16Arr.Length];
       for(int i=0; i < i16Arr.Length; i++)
	 {
	 oArrValues[i]=i16Arr[i];
	 }
       try
	 {
	 o1 = 16;
	 ++iCountTestcases;
	 iReturnValue = Array.IndexOf(oArrValues, o1, -1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_735ds!  iReturnValue=="+ iReturnValue );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(" Error E_975em!  , Wrong Exception thrown == " + ex.ToString());
	 Console.WriteLine(ex.StackTrace);
	 }
       oArrValues = new Object[i16Arr.Length];
       for(int i=0; i < i16Arr.Length; i++)
	 {
	 oArrValues[i]=i16Arr[i];
	 }
       iReturnValue = Array.IndexOf(oArrValues, null, 0);
       ++iCountTestcases;
       if ( iReturnValue != -1 )	
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_784dr! iReturnValue=="+ iReturnValue );
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
 internal static readonly Char[]			cArr	  = {'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a', 'i', 'i'};
 internal static readonly Int16[]		i16Arr	= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753, 238, 238};
 internal static readonly Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 internal static readonly Char[,]		c2Arr		  = {{'a', 'b', 'c', 'd', 'e'},
								     {'k', 'l', 'm', 'n', 'o'}};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3762IndexOf_ooi oCbTest = new Co3762IndexOf_ooi();
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
