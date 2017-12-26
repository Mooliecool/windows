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
public class Co3440Replace_CharChar
{
 static String strName = "String.Replace";
 static String strTest = "Co3440Replace_CharChar.";
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
   char chOld = '\0';
   char chNew = '\0';
   String str1 = "Dill Bill Guv Dill";
   String str2 = null;
   try
     {
     ++iCountTestcases;
     do
       {
       for(int i =0 ; i < 100 ; i++ ){
       IntlStrings intl = new IntlStrings();
       String str3 = intl.GetString(1, true, true);
       String str4 = intl.GetString(1, true, true);
       String str5 = intl.GetString(6, true, true);
       str5 = str5.Replace(str3[0], Convert.ToChar( Convert.ToInt32(str3[0]) + 1));
       String str6 = String.Concat(str3, str5);
       str5 = String.Concat(str4, str5);  
       str6 = str6.Replace(str3[0], str4[0]);                                
       if(String.Compare(str5, str6) !=0) {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_9745r!" + str5 + str6);
       }
       }
       chOld = 'G';
       chNew = 'X';
       str2 = str1.Replace(chOld, chNew);
       if ( String.Compare(str2, "Dill Bill Xuv Dill") != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  str2==" + str2  );
	 }
       chOld = 'D';
       chNew = ' ';
       str2 = str1.Replace(chOld, chNew);
       iCountTestcases++ ;
       if ( String.Compare(str2, " ill Bill Guv  ill") != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_027gr!  str2==" + str2  );
	 }
       chOld = 'l';
       chNew = 'i';
       str2 = str1.Replace(chOld, chNew);
       ++iCountTestcases;
       if ( String.Compare(str2, "Diii Biii Guv Diii") != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_715tr!  str2==" + str2  );
	 }
       chOld = 'l';
       chNew = 'l';
       str2 = str1.Replace(chOld, chNew);
       ++iCountTestcases;
       if ( String.Compare(str2, str1) != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_702sd!  str2==" + str2  );
	 }
       chOld = 'B';
       chNew = '\n';
       str2 = str1.Replace(chOld, chNew);
       ++iCountTestcases;
       if ( String.Compare(str2, "Dill \nill Guv Dill") != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_403fj!  str2==" + str2  );
	 }
       str1 = null;
       chOld = 'B';
       chNew = '\n';
       try
	 {
	 ++iCountTestcases;
	 str2 = str1.Replace(chOld, chNew);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;
	 strInfo += ", Exception not thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       catch (NullReferenceException )
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
   Boolean bResult = false;	
   Co3440Replace_CharChar oCbTest = new Co3440Replace_CharChar();
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
