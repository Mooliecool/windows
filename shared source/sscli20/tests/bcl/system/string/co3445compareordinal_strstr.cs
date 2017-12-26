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
public class Co3445CompareOrdinal_StrStr
{
 internal static String strName = "String.CompareOrdinal";
 internal static String strTest = "Co3445CompareOrdinal_StrStr.";
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
   String strTest1 = "WA";
   String strTest2 = "PN";
   String str1 = null;
   String str2 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       IntlStrings intl = new IntlStrings();
       String strTest3 = intl.GetString(18, true, true);
       str1 = strTest3;
       ++iCountTestcases;
       if ( String.CompareOrdinal(str1,strTest3) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strTest1;
       ++iCountTestcases;
       if ( String.CompareOrdinal(str1, strTest1) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = strTest1;
       if ( String.CompareOrdinal(str2, strTest1) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_390nb" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = strTest1;
       if ( String.CompareOrdinal(str2, strTest2) <= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_023Sd" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = strTest2;
       if ( String.CompareOrdinal(str2, strTest1) >= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_033pw" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strTest2;
       str2 = null;
       if(String.CompareOrdinal(str1, str2) <= 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_053jh" ;
	 strInfo += ", Exception not thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       str2 = strTest1;
       if(String.CompareOrdinal(str1, str2) >= 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;
	 strInfo += ", Exception not thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       str2 = null;
       if(String.CompareOrdinal(str1, str2) != 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_982ww" ;
	 strInfo += ", Exception not thrown" ;
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
   Co3445CompareOrdinal_StrStr oCbTest = new Co3445CompareOrdinal_StrStr();
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
