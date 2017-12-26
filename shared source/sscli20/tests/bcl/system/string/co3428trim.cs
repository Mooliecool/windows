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
public class Co3428Trim
{
 internal static String strName = "String.Trim";
 internal static String strTest = "Co3428Trim.";
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
   String str1 = "  Hello          World         ";
   try
     {
     LABEL_860_GENERAL:
     do
       {
       try { 
       IntlStrings intl = new IntlStrings();
       String str6 = intl.GetString(8, true, true);
       String str5 = String.Concat(str6, "        ");
       str5 = String.Concat("             ", str5);
       str6 = str6.Trim();
       if(String.Compare(str5.Trim(), str6) != 0) {
       Console.WriteLine("!" + str5 + "!" + str6 + "!");
       ++iCountErrors;
       strInfo = strTest + " error: ";
       strInfo += "FAiL. E_208bt" ;
       Console.WriteLine( strTest+ strInfo );
       }
       strLoc="1993_er";
       if ( String.Compare(str1.Trim(), "Hello          World") != 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1="             ";
       strLoc="19388_er";
       if ( String.Compare(str1.Trim(), String.Empty) != 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_295mp" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       } catch(Exception e) { Console.WriteLine(e.ToString());}
       str1="Hello             ";
       strLoc="128_er";
       if ( String.Compare(str1.Trim(), "Hello") != 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_047j[" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1="             Hello";
       ++iCountTestcases;
       if ( String.Compare(str1.Trim(), "Hello") != 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_023jt" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1="";
       ++iCountTestcases;
       if ( String.Compare(str1.Trim(), String.Empty) != 0)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_250dk" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       try
	 {
	 str1.Trim();
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;
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
	 strInfo = "FAiL. E_278pw" ;
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
   Co3428Trim oCbTest = new Co3428Trim();
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
