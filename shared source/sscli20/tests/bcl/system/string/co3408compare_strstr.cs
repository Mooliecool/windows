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
using System.Threading;
using GenStrings;
public class Co3408Compare_StrStr
{
 static String strName = "String.Compare";
 static String strTest = "Co3408Compare_StrStr"; 
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 1;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   String strAlphabetsLower = "abcdefghijklmnopqrstuvwxyz";
   String strAlphabetsUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   String str1 = null;
   String str2 = null;
   CultureInfo ciCultInfo;
   CompareInfo ciCompInfo;
   Int32 iAnswer;
   Thread tCurrentThread;
   try
     {
     tCurrentThread = Thread.CurrentThread;
     ciCultInfo = tCurrentThread.CurrentCulture;
     ciCompInfo = ciCultInfo.CompareInfo;
     iAnswer = ciCompInfo.Compare( "a" ,"-" ,CompareOptions.StringSort );
     if ( iAnswer != 1 )
       {
       ++iCountErrors;
       Console.WriteLine( "Error_pim750:  ("+ iAnswer.ToString() +") .StringSort bad, a -to- -" );
       }
     iAnswer = ciCompInfo.Compare( "a" ,"-a" ,CompareOptions.StringSort );
     if ( iAnswer != 1 )
       {
       ++iCountErrors;
       Console.WriteLine( "Error_pim761:  ("+ iAnswer.ToString() +") .StringSort bad, a -to- -a" );
       }
     iAnswer = ciCompInfo.Compare( "a" ,"-" );
     if ( iAnswer != 1 )
       {
       ++iCountErrors;
       Console.WriteLine( "Error_ran346:  ("+ iAnswer.ToString() +") bad, a -to- -" );
       }
     iAnswer = ciCompInfo.Compare( "a" ,"-a" );
     if ( iAnswer != -1 )
       {
       ++iCountErrors;
       Console.WriteLine( "Error_ran337:  ("+ iAnswer.ToString() +")  bad, a -to- -a" );
       }
     iAnswer = ciCompInfo.Compare( "coop" ,"co-op" );
     if ( iAnswer != -1 )
       {
       ++iCountErrors;
       Console.WriteLine( "Error_ran328:  ("+ iAnswer.ToString() +") bad, coop -to- co-op" );
       }
     do
       {
       str1 = strAlphabetsLower;
       if ( String.Compare(str1, strAlphabetsLower) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       IntlStrings intl = new IntlStrings();
       String intlString = intl.GetString(10, false, true);
       str1 = intlString;
       if ( String.Compare(str1, intlString) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = strAlphabetsLower;
       if ( String.Compare(str2, strAlphabetsLower) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_390nb" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = strAlphabetsLower;
       Console.WriteLine( "Info_wat481:  a -to- A == "+ String.Compare("a", "A"));
       if ( String.Compare(str2, strAlphabetsUpper, false, CultureInfo.InvariantCulture) >= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_023Sd   , str1=="+str2+" , str2=="+strAlphabetsUpper ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str2 = strAlphabetsUpper;
       if ( String.Compare(str2, strAlphabetsLower, false, CultureInfo.InvariantCulture) <= 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_033pw" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = strAlphabetsUpper;
       str2 = null;
       if ( String.Compare(str1, str2) != 1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_053jh" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       str2 = strAlphabetsLower;
       if ( String.Compare(str1, str2) != -1 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_304_jk" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       str1 = null;
       str2 = null;
       if ( String.Compare(str1, str2) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_982ww" ;
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
   Co3408Compare_StrStr oCbTest = new Co3408Compare_StrStr();
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
