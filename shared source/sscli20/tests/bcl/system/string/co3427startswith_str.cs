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
using System.Globalization;
using GenStrings;
using System.IO; 
using System;
public class Co3427StartsWith_Str
{
 internal static String strName = "String.StartsWith";
 internal static String strTest = "Co3427StartsWith_Str.";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   String str1 = "Dill Guv Dill Guv Dill" ;
   String str2 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       IntlStrings intl = new IntlStrings();
       String str5 = intl.GetString(8, true, true);
       String str6 = intl.GetString(3, true, true);
       str6 = String.Concat(str5, str6);
       if(str6.StartsWith(str5) == false) {
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_208bt" ;
       Console.WriteLine( strTest+ strInfo );
       }
       ++iCountTestcases;
       str2="Dill ";
       if ( str1.StartsWith(str2) == false)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2="DILL ";
       if ( str1.StartsWith(str2) == true)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_532dh" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2=" ";
       if ( str1.StartsWith(str2) == true)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_053kd" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       strLoc="E_620pf";
       str2=String.Empty; 
       if ( str1.StartsWith(str2) == false)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_260is" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = null;
       try
	 {
	 str1.StartsWith(str2);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;
	 strInfo += ", Exception not thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo = "FAiL. E_278pw" ;
	 strInfo += ", Wrong Exception thrown == " + ex.ToString();
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = " ";
       str1 = null;
       try
	 {
	 str1.StartsWith(str2);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_935ep " ;
	 strInfo += ", Exception not thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       catch (NullReferenceException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo = "FAiL. E_267mo" ;
	 strInfo += ", Wrong Exception thrown == " + ex.ToString();
	 Console.WriteLine( strTest+ strInfo );
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.Error.WriteLine ( "POINTTOBREAK: Error Err_103! strLoc=="+ strLoc + " ,exc_general=="+exc_general  );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3427StartsWith_Str oCbTest = new Co3427StartsWith_Str();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
