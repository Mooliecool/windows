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
public class Co3709Compare_StrStrBoolLoc
{
 static String strName = "String.Compare";
 static String strTest = "Co3709Compare_StrStrBoolLoc";
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   Console.Error.Write( strTest );
   Console.Error.WriteLine( " runTest started..." );
   Console.Error.WriteLine(  "CAUTION" +"POINTTOBREAK:  C_358uc.  ReCheck validity of Japanese testcases."  );
   String strAlphabetsLower = "abcdefghijklmnopqrstuvwxyz";
   String strAlphabetsUpper = "ABCDEFGHGIJKLMNOPQRSTUVWXYZ";
   String strDigits = "0123456789";
   String str1 = null;
   String str2 = null;
   CultureInfo locale = null; 
   CultureInfo locUSEnglish = null; 
   CultureInfo locGDR = null;  
   CultureInfo locJPN = null;  
   do
     {
     Console.Error.WriteLine( "[] Create locales" ); 
     ++iCountTestcases;
     try
       {
       locale = new CultureInfo("en-us"); 
       if ( locale == null )
	 {
	 Console.WriteLine( strTest+ "E_101" );
	 Console.WriteLine( strTest+ "Failed to construct default locale object" );
	 ++iCountErrors;
	 break;
	 }
       locUSEnglish = new CultureInfo( 1033 ); 
       if ( locUSEnglish == null )
	 {
	 Console.WriteLine( strTest+ "E_202" );
	 Console.WriteLine( strTest+ "Failed to construct U.S. English locale object" );
	 ++iCountErrors;
	 break;
	 }
       locGDR = new CultureInfo( 1031 ); 
       if ( locGDR == null )
	 {
	 Console.WriteLine( strTest+ "E_303" );
	 Console.WriteLine( strTest+ "Failed to construct German locale object" );
	 ++iCountErrors;
	 break;
	 }
       locJPN = new CultureInfo( 1041 ); 
       if ( locJPN == null )
	 {
	 Console.WriteLine( strTest+ "E_404" );
	 Console.WriteLine( strTest+ "Failed to construct Japanese locale object" );
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
     Console.Error.WriteLine( "[] Compare string using default locale" ); 
     try
       {
       ++iCountTestcases;
       str1 = strDigits;
       if ( String.Compare(str1, strDigits, false, locale ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1010" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsLower;
       if ( String.Compare( str1, strAlphabetsLower, false, locale ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1111" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsUpper;
       if ( String.Compare( str1, strAlphabetsUpper, false, locale ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1212" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.WriteLine( strTest+ "E_1313" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       break;
       }
     Console.Error.WriteLine( "[] Compare strings using German locale object" ); 
     try
       {
       ++iCountTestcases;
       str1 = strDigits;
       if ( String.Compare( str1, strDigits, false, locGDR ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1515" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsLower;
       if ( String.Compare( str1, strAlphabetsLower,  false, locGDR ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1616" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsUpper;
       if ( String.Compare( str1, strAlphabetsUpper,  false, locGDR ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_1717" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.WriteLine( strTest+ "E_1818" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       break;
       }
     Console.Error.WriteLine( "[] Compare strings using U.S.English locale object" ); 
     try
       {
       ++iCountTestcases;
       str1 = strDigits;
       if ( String.Compare( str1, strDigits,  false, locUSEnglish ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_2020" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsLower;
       if ( String.Compare( str1, strAlphabetsLower,  false, locUSEnglish ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_2121" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsUpper;
       if ( String.Compare( str1, strAlphabetsUpper,  false, locUSEnglish ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_2222" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       }
     catch (Exception ex)
       {
       ++iCountErrors;
       Console.WriteLine( strTest+ "E_2323" );
       Console.WriteLine( strTest+ "Unexpected Exception: " + ex.ToString() );
       break;
       }
     Console.Error.WriteLine( "[] Compare strings using Japanese locale object" ); 
     try
       {
       ++iCountTestcases;
       str1 = strDigits;
       if ( String.Compare( str1, strDigits,  false, locJPN ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_2525" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsLower;
       if ( String.Compare( str1, strAlphabetsLower,  false, locJPN ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_2626" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       ++iCountTestcases;
       str1 = strAlphabetsUpper;
       if ( String.Compare( str1, strAlphabetsUpper,  false, locJPN ) != 0 )
	 {
	 ++iCountErrors;
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "String 1 = <" + str1 + ">\n";
	 strInfo = strInfo + "String 2 = <" + str2 + ">";
	 Console.WriteLine( strTest+ "E_2727" );
	 Console.WriteLine( strTest+ strInfo );
	 break;
	 }
       }
     catch ( Exception  )
       {
       Console.Error.WriteLine(  "CAUTION" +"POINTTOBREAK:  Should not throw Exception! RECHECK Later "  );
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
   Co3709Compare_StrStrBoolLoc oCbTest = new Co3709Compare_StrStrBoolLoc();
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
