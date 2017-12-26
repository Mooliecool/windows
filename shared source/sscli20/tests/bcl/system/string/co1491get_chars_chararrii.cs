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
using System.Globalization;
using GenStrings;
public class Co1491get_Chars_CharArrII
{
 static String strName = "String.GetChars";
 static String strTest = "Co1491get_Chars_CharArrII";
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iTestCases = 0;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strTest );
   Console.Out.WriteLine( " runTest started..." );
   int ii = 0;
   String strAlphabets = "abcdefghijklmnopqrstuvwxyz";
   String strDigits = "0123456789";
   char[] charArray = null; 
   char[] charAlphabets = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
			    'h', 'i', 'j', 'k', 'l', 'm', 'n',
			    'o', 'p', 'q', 'r', 's', 't', 'u',
			    'v', 'w', 'x', 'y', 'z' };
   char[] charDigits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' }; 
   char[] charBuffer = new char[256]; 
   IntlStrings intl = new IntlStrings();
   char[] MyCharArr = new char[2];
   char[] CopyArr = new char[2];
   MyCharArr[0] = (intl.GetString(1, true, true))[0];
   MyCharArr[1] = (intl.GetString(1, true, true))[0];
   String intlString = new String(MyCharArr);
   CopyArr = intlString.ToCharArray();
   if((CopyArr[0] != MyCharArr[0]) || (CopyArr[1] != MyCharArr[1]) ) {
   iCountErrors++;
   }
   do
     {
     ++iTestCases;
     Console.Out.WriteLine( "[] Obtain char array for entire string" ); 
     try
       {
       strAlphabets.CopyTo( 0, charBuffer, 0, strAlphabets.Length );
       if ( charBuffer == null )
	 {
	 Console.WriteLine( strTest+ "E_101" );
	 Console.WriteLine( strTest+ "Failed to return valid buffer" );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_202" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     for ( ii = 0; ii < charAlphabets.Length; ++ii )
       {
       if ( charAlphabets[ii] != charBuffer[ii] )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "On character item " + ii + " ";
	 strInfo = strInfo + "Expected character <" + (charAlphabets[ii]).ToString() + ">\n";
	 Console.WriteLine( strTest+ "E_303" );
	 Console.WriteLine( strTest+ strInfo );
	 ++iCountErrors;
	 break;
	 }
       }
     ++iTestCases;
     Console.Out.WriteLine( "[] Obtain char array for substring" ); 
     try
       {
       strAlphabets.CopyTo(12, charBuffer,  20, 5 );
       if ( charBuffer == null )
	 {
	 Console.WriteLine( strTest+ "E_404" );
	 Console.WriteLine( strTest+ "Failed to return valid buffer" );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_505" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     for ( ii = 0; ii < 5; ++ii )
       {
       if ( charAlphabets[ii+12] != charBuffer[ii+20] )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "On character item " + ii + " ";
	 strInfo = strInfo + "Expected character <" + (charAlphabets[ii+12]).ToString() + ">\n";
	 strInfo = strInfo + "Returned character <" + (charBuffer[ii+20]).ToString() + ">";
	 Console.WriteLine( strTest+ "E_606" );
	 Console.WriteLine( strTest+ strInfo );
	 ++iCountErrors;
	 break;
	 }
       }
     ++iTestCases;
     Console.Out.WriteLine( "[] Index out of range" ); 
     try
       {
       strAlphabets.CopyTo( 12, charBuffer,  1000, 5 );
       Console.WriteLine( strTest+ "E_707" );
       Console.WriteLine( strTest+ "Failed to throw IndexOutOfRangeException" );
       ++iCountErrors;
       break;
       }
     catch (ArgumentException ex)
       {
       Console.Error.WriteLine( "IndexOutOfRangeException caught" );
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_808" );
       Console.WriteLine( strTest+ "Generic Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     ++iTestCases;
     ++iTestCases;
     Console.Out.WriteLine( "[] Null buffer expection" ); 
     try
       {
       strAlphabets.CopyTo( 12, null,  20, 5 );
       Console.WriteLine( strTest+ "E_909" );
       Console.WriteLine( strTest+ "Failed to throw ArgumentException" );
       ++iCountErrors;
       break;
       }
     catch (ArgumentException ex)
       {
       Console.Error.WriteLine( "ArgumentException caught" );
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_1010" );
       Console.WriteLine( strTest+ "Generic Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     }
   while ( false );
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co1491get_Chars_CharArrII oCbTest = new Co1491get_Chars_CharArrII();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.WriteLine( strTest+ strPath );
     Console.WriteLine( strTest+ "E_1000000" );
     Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
     Console.WriteLine( strTest+ ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
} 
