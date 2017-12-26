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
public class Co1091IndexOf_Char
{
 static String strName = "String.IndexOf";
 static String strTest = "Co1091IndexOf_Char";
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strTest );
   Console.Out.WriteLine( " runTest started..." );
   String strFromCharArrIntInt = null;
   String strAlphabets = "abcdefghijklmnopqrstuvwxyz";
   String strAlphabetsUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   String strDigits = "0123456789";
   char[] charAlphabets = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
			    'h', 'i', 'j', 'k', 'l', 'm', 'n',
			    'o', 'p', 'q', 'r', 's', 't', 'u',
			    'v', 'w', 'x', 'y', 'z' };
   do
     {
     IntlStrings intl = new IntlStrings();
     String swrString2 = intl.GetString(19, true, true);
     String swrString3 = swrString2.Substring(0, 1);
     char[] charArray = swrString3.ToCharArray();
     if(swrString2.IndexOf(charArray[0]) != 0) {
     ++iCountErrors;
     }
     Console.Out.WriteLine( "[] Obtain index of a character with a string" ); 
     ++iCountTestcases;
     try
       {
       for ( int ii = 0; ii < charAlphabets.Length; ++ii )
	 {
	 if ( ii != strAlphabets.IndexOf( charAlphabets[ii] ) )
	   {
	   String strInfo = strTest + " error: ";
	   strInfo = strInfo + "On Item " + (ii).ToString() + " ";
	   strInfo = strInfo + "Expected index <" + (ii).ToString() + ">\n";
	   strInfo = strInfo + "Returned index <" + (strAlphabets.IndexOf( charAlphabets[ii] ) ).ToString() + ">";
	   Console.WriteLine( strTest+ "E_101" );
	   Console.WriteLine( strTest+ strInfo );
	   ++iCountErrors;
	   break;
	   }
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_202" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     iCountTestcases++;
     Console.Out.WriteLine( "[] Character not found" ); 
     ++iCountTestcases;
     try
       {
       if ( strAlphabets.IndexOf( '0' ) != -1 )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected index <-1> ";
	 strInfo = strInfo + "Returned index <" + (strAlphabets.IndexOf( '0' ) ).ToString() + ">";
	 Console.WriteLine( strTest+ "E_303" );
	 Console.WriteLine( strTest+ strInfo );
	 ++iCountErrors;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_404" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     iCountTestcases++;
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
   Co1091IndexOf_Char oCbTest = new Co1091IndexOf_Char();
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
