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
using GenStrings;
using System.Globalization;
public class Co3417IndexOfAny_CharArr
{
 static String strName = "String.IndexOfAny";
 static String strTest = "Co3417IndexOfAny_CharArr.";
 static String strPath = "";
 public Boolean runTest()
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
   Char[] chArr1 = {'G', 'M', 'X', 'Y', 'Z'}; 
   Char[] chArr2 = null; 
   try
     {
     LABEL_860_GENERAL:
     do
       {
       IntlStrings intl = new IntlStrings();
       Char[] myCharArr = {'!', '@'};
       String myIntlString = intl.GetString(10, true, true);
       myIntlString = String.Concat(myIntlString, "!@!!@");
       if(myIntlString.IndexOfAny(myCharArr) > 15) {
       ++iCountErrors;
       }
       chArr1[3]='\0';
       ++iCountTestcases;
       if ( str1.IndexOfAny(chArr1) != 5 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke " ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       chArr1[3]='Y';
       chArr1[0]='X';
       chArr1[3]='D';
       ++iCountTestcases;
       if ( str1.IndexOfAny(chArr1) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_892dds " ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       chArr1[0]='G'; 
       chArr1[3]='Y';
       chArr1[0]='X';
       ++iCountTestcases;
       if ( str1.IndexOfAny(chArr1) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_739cp" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       chArr1[0]='G'; 
       chArr1[0]='G';
       chArr1[3]='D';
       ++iCountTestcases;
       if ( str1.IndexOfAny(chArr1) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_720mr" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       chArr1[0]='G'; 
       chArr1[3]='Y';
       chArr1[0]='\0';
       ++iCountTestcases;
       if (str1.IndexOfAny(chArr1)!= -1)
	 {
	 ++iCountErrors;
	 strInfo += "FAiL. E_240hg " ;
	 strInfo += ", Exception not thrown ==" + (str1.IndexOfAny(chArr1)).ToString() ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       try
	 {
	 ++iCountTestcases;
	 str1.IndexOfAny(chArr2);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_248ko" ;
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
	 strInfo = "FAiL. E_034fl" ;
	 strInfo += ", Wrong Exception thrown == " + ex.ToString();
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       try
	 {
	 ++iCountTestcases;
	 str1.IndexOfAny(chArr1);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_015qp " ;
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
	 strInfo = "FAiL. E_104nu" ;
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
   Boolean bResult = false;	
   Co3417IndexOfAny_CharArr oCbTest = new Co3417IndexOfAny_CharArr();
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
