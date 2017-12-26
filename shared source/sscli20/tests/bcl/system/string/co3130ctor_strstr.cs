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
using System.Globalization;
using GenStrings;
using System;
public class Co3130ctor_StrStr
{
 internal static String strName = "String.ConstructFromStrStrToBytes";
 internal static String strTest = "Co3130ctor_StrStr";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strTest );
   Console.Out.WriteLine( " runTest started..." );
   String strFromStrStr = null;
   String strAlphabets = "abcdefghijklmnopqrstuvwxyz";
   String strDigits = "0123456789";
   do
     {
     IntlStrings intl = new IntlStrings();
     String testString = intl.GetString(12, true, true);
     String testString2 = intl.GetString(3, true, true);
     Console.Out.WriteLine( "[] Construct a string from two strings" );
     try
       {
       strFromStrStr =  testString + testString2 ;
       if ( strFromStrStr == null )
	 {
	 Console.WriteLine( strTest+ "E_101" );
	 Console.WriteLine( strTest+ "Failed to construct valid string" );
	 ++iCountErrors;
	 break;
	 }
       if ( strFromStrStr.Equals( testString + testString2 ) != true )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected string<" + strAlphabets + strDigits + ">\n";
	 strInfo = strInfo + "Returned character <" + strFromStrStr + ">";
	 Console.WriteLine( strTest+ "E_202" );
	 Console.WriteLine( strTest+ strInfo );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_303" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     strFromStrStr =  strAlphabets+ strDigits ;
     }
   while ( false );
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3130ctor_StrStr oCbTest = new Co3130ctor_StrStr();
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
