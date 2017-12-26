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
public class Co3761IndexOf_oo
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Array.IndexOf(Object[], Object)";
 public static readonly String s_strTFName        = "Co3761IndexOf_oo";
 public static readonly String s_strTFAbbrev      = "Co3761";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="Loc_top123";
   Int32 iIdx;
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   Object o1;
   int iReturnValue;
   Object[] oArrValues;
   Int32[,] iArr2D;
   Int32 iFoundIdx = -2;
   Array aGeneralArray;
   Int32 aa ,bb;
   Boolean bTemp;
   Boolean[] bBooleansArray = {true ,false}; 
   try
     {
     do
       {
       strLoc="Loc_nub478";
       bTemp = false;  
       iArr2D = new Int32[2,3];  
       for ( aa=0 ;aa<2 ;aa++ )
	 {
	 for ( bb=0 ;bb<3 ;bb++ )
	   {
	   iArr2D[aa,bb] = aa*bb;
	   }
	 }
       aGeneralArray = (Array)iArr2D;
       try
	 {
	 iFoundIdx = Array.IndexOf( aGeneralArray ,(Int32)3 );
	 }
       catch ( RankException  )
	 { bTemp = !bTemp; }
       if ( ! bTemp )
	 {
	 ++iCountErrors;
	 Console.WriteLine( "Error_nub782:  We expected a RankException but did not get one." );
	 }
       strLoc="Loc_bob443";
       iIdx = Array.IndexOf( bBooleansArray ,true );
       if ( iIdx != 0 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( "Error_dof589:  Booleans array problem, maybe see old ." );
	 }
       oArrValues = new Object[cArr.Length];
       for(int i=0; i < cArr.Length; i++)
	 {
	 oArrValues[i]=cArr[i];
	 }
       for(int i=0; i < cArr.Length-3; i++)
	 {
	 o1 = cArr[i];
	 iReturnValue = Array.IndexOf(oArrValues, o1);
	 ++iCountTestcases;
	 if ( iReturnValue != i )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + ", Err_320ye_" + i + ", iReturnValue=="+ iReturnValue );
	   }
	 }
       oArrValues = new Object[i16Arr.Length];
       for(int i=0; i < i16Arr.Length; i++)
	 {
	 oArrValues[i]=i16Arr[i];
	 }
       for(int i=0; i < oArrValues.Length-3; i++)
	 {
	 o1 = i16Arr[i];
	 iReturnValue = Array.IndexOf(oArrValues, o1);
	 ++iCountTestcases;
	 if ( iReturnValue != i )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev + "Err_947ps_" + i + ", iReturnValue=="+ iReturnValue );
	   }
	 }
       try
	 {
	 o1 = 16;
	 iReturnValue = Array.IndexOf(null, o1);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev + "Err_835xr!  iReturnValue=="+ iReturnValue );
	 }
       catch (ArgumentException ) 
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
       iReturnValue = Array.IndexOf(oArrValues, null);
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
 internal static readonly Char[]			cArr	  = {'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a', 'j', 'i', 'h'};
 internal static readonly Int16[]		i16Arr	= {19, 238, 317, 6, 565, 0, -52, 60, -563, 753, 19, 238, 317};
 internal static readonly Object[]	oArr			= {true, 'k', SByte.MinValue, Byte.MinValue, (short)2, 634, (long)436, (float)1.1, 1.23, "Hello World"};
 internal static readonly Char[,]		c2Arr		  = {{'a', 'b', 'c', 'd', 'e'},
								     {'k', 'l', 'm', 'n', 'o'}};
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3761IndexOf_oo oCbTest = new Co3761IndexOf_oo();
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
