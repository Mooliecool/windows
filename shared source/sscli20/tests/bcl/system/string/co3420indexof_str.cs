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
public class Co3420IndexOf_Str
{
 internal static String strName = "String.IndexOf";
 internal static String strTest = "Co3420IndexOf_Str.";
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
   String str2 = "Dill";
   try
     {
     do
       {
       ++iCountTestcases;
       if ( str1.IndexOf(str2) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke " ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       IntlStrings intl = new IntlStrings();
       String myIntlString = intl.GetString(5, true, true);
       String myIntlString2 = intl.GetString(13, true, true);
       myIntlString = String.Concat(myIntlString, myIntlString2);
       if(myIntlString.IndexOf(myIntlString2) != 5) {
       ++iCountErrors;
       }
       str2 = "Dill Guv Dill Guv Dill" ;
       if ( str1.IndexOf(str2) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_892dds " ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = "ll" ;
       if ( str1.IndexOf(str2) != 2 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_739cp" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = "Bill" ;
       if ( str1.IndexOf(str2) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_720mr" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = " " ;
       if (str1.IndexOf(str2)!= 4)
	 {
	 ++iCountErrors;
	 strInfo += "FAiL. E_240hg " ;
	 strInfo += ", Exception not thrown ==" + (str1.IndexOf(str2)).ToString() ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = "" ;
       if (str1.IndexOf(str2)!= 0)
	 {
	 ++iCountErrors;
	 strInfo += "FAiL. E_235jf " ;
	 strInfo += ", Index ==" +  "< " + (str1.IndexOf(str2)).ToString() + " >" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       try
	 {
	 str2 = null ;
	 str1.IndexOf(str2);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_248ko" ;
	 strInfo += ", Exception not thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       catch (ArgumentException )
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
	 str1.IndexOf(str2);
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_015qp " ;
	 strInfo += ", Exception not thrown" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       catch ( NullReferenceException  ) 
	 {}
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
   bool bResult = false;	
   Co3420IndexOf_Str oCbTest = new Co3420IndexOf_Str();
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
