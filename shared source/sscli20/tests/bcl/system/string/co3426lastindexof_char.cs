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
public class Co3426LastIndexOf_Char
{
 internal static String strName = "String.LastIndexOf";
 internal static String strTest = "Co3426LastIndexOf_Char.";
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
   Char ch1 = '\0';
   try
     {
     LABEL_860_GENERAL:
     do
       {
       IntlStrings intl = new IntlStrings();
       String swrString2 = intl.GetString(4, true, true);
       String swrString3 = intl.GetString(1, true, true);
       String swrString4 = String.Concat(swrString2, swrString3);
       swrString4 = String.Concat(swrString4, swrString2);
       swrString4 = String.Concat(swrString4, swrString3);
       swrString4 = String.Concat(swrString4, swrString2);
       int n2 = swrString4.LastIndexOf(swrString3[0]);
       if(n2 < 4) 
	 ++iCountErrors;
       ch1 = 'G';
       ++iCountTestcases;
       if ( str1.LastIndexOf(ch1) != 14 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_362gg" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       ch1 = 'g';
       if ( str1.LastIndexOf(ch1) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_672pf" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       ch1 = 'X';
       if ( str1.LastIndexOf(ch1) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       ch1 = '\0';
       if ( str1.LastIndexOf(ch1) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_839pl " ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       ch1 = 'G';
       try
	 {
	 str1.LastIndexOf(ch1);
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
   Co3426LastIndexOf_Char oCbTest = new Co3426LastIndexOf_Char();
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
