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
public class Co3411CompareTo_Str
{
 static String strName = "String.Compare";
 static String strTest = "Co3411CompareTo_Str.";
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Int32 i2 = -123;
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
   try
     {
     do
       {
       IntlStrings intl = new IntlStrings();
       String intlString = intl.GetString(10, false, true);
       str1 = intlString;
       if ( str1.CompareTo(intlString) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 } 
       str1 = strAlphabetsLower;
       ++iCountTestcases;
       if ( str1.CompareTo(strAlphabetsLower) != 0 )
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_738ke" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       strLoc="reb8312";
       str1 = strAlphabetsLower;
       ++iCountTestcases;
       if ( str1.CompareTo(strAlphabetsUpper) >= 0 && CultureInfo.CurrentCulture.LCID != 0x41F)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_023Sd" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       strLoc="reb8313";
       str1 = strAlphabetsUpper;
       ++iCountTestcases;
       if ( str1.CompareTo(strAlphabetsLower) <= 0 && CultureInfo.CurrentCulture.LCID != 0x41F)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_033pw" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       strLoc="reb8314";
       str1 = strAlphabetsUpper;
       str2 = null;
       ++iCountTestcases;
       if(str1.CompareTo(str2) != 1)
	 {
	 ++iCountErrors;
	 strInfo = strTest + " error: ";
	 strInfo += "FAiL. E_053jh" ;
	 Console.WriteLine( strTest+ strInfo );
	 }
       strLoc="reb8315";
       str1 = null;
       str2 = strAlphabetsLower;
       ++iCountTestcases;
       try
	 {
	 i2 = str1.CompareTo(str2);
	 ++iCountErrors;
	 Console.WriteLine( "Error_reb2155: Expected NullRefExc!" );
	 }
       catch ( NullReferenceException  )
	 {}  
       strLoc = "Loc_837hd";
       str1 = "0";
       str2 = "{";
       iCountTestcases++;
       if(str1.CompareTo(str2) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine( strTest+ "Err_371eh! , result=="+str1.CompareTo(str2));
	 }
       strLoc = "Loc_192hw";
       str1 = "z";
       str2 = "{";
       iCountTestcases++;
       if(str1.CompareTo(str2) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine( strTest+ "Err_732qu! , result=="+str1.CompareTo(str2));
	 }
       strLoc = "Loc_128yr";
       str1 = "A";
       str2 = "*";
       iCountTestcases++;
       if ( str1.CompareTo( str2 ) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine( strTest+ "Err_347hw! , result=="+str1.CompareTo( str2 ));
	 }
       strLoc = "Loc_834yw";
       str1 = "0";
       str2 = "$";
       iCountTestcases++;
       if ( str1.CompareTo ( str2) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine( strTest+ "Err_289nk! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_372hu";
       str1 = "0";
       str2 = "5";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) >= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_793rh! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_373hu";
       str1 = "5";
       str2 = "0";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_794rh! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_373hu";
       str1 = "5";
       str2 = "0";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_794rh! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_373hu";
       str1 = "A";
       str2 = "a";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_894rh! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_481hu";
       str1 = "A";
       str2 = "B";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) >= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_933jw! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_473hu";
       str1 = "a";
       str2 = "A";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) >= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_812ks! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_751hu";
       str1 = "B";
       str2 = "A";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_941jd! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_573hu";
       str1 = "}";
       str2 = "<";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) >= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_094rh! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_673hu";
       str1 = "<";
       str2 = "{";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_099uh! , result=="+str1.CompareTo ( str2 ));
	 }
       strLoc = "Loc_683hu";
       str1 = "A";
       str2 = "0";
       iCountTestcases++;
       if ( str1.CompareTo ( str2 ) <= 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(strTest+ "Err_134eh! , result=="+str1.CompareTo ( str2 ));
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
   Co3411CompareTo_Str oCbTest = new Co3411CompareTo_Str();
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
