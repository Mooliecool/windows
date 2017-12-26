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
using System;
using System.IO;
using System.Collections;
public class Co4832ctor_char_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String(char, int)";
 public static String s_strTFName        = "Co4832ctor_char_int";
 public static String s_strTFAbbrev      = "Co5504";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     char ch = 'a';
     String str1 = String.Empty, str2 = String.Empty;
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
     int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/100 , Int32.MaxValue/1000 };
     iCountTestcases++;
     String strNewString = String.Empty ;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       strNewString = new String('a' , iArrInvalidValues[iLoop]);
       iCountErrors++;
       } catch ( ArgumentException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     if ( strNewString != String.Empty )
       {
       iCountErrors++;
       Console.Error.WriteLine( "Error_3333!!!! Incorrect string content.... Expected...{0},  Actual...{1}", String.Empty, strNewString );
       }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       strNewString = new String('a' , iArrLargeValues[iLoop]);
       if ( strNewString.Length != iArrLargeValues[iLoop] )
	 {
	 iCountErrors++;
	 Console.Error.WriteLine( "Error_5555!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrLargeValues[iLoop], strNewString.Length );
	 }
       } catch ( OutOfMemoryException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_4444!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
     try
       {                            
       strNewString = new String('a' , iArrValidValues[iLoop]);
       if ( strNewString.Length != iArrValidValues[iLoop] ){
       iCountErrors++;
       Console.Error.WriteLine( "Error_6666!!!! Incorrect string length.... Expected...{0},  Actual...{1}", iArrValidValues[iLoop], strNewString.Length );
       }
       } catch ( Exception ex ){
       Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     strLoc = "Loc_498hv";
     iCountTestcases++;
     ch = ' ';
     str2 = new String(ch, 4);
     if(!str2.Equals("    "))
       {
       iCountErrors++;
       printerr( "Error_498ch! incorrect string returned for null argument=="+str2);
       } 
     strLoc = "Loc_498hv.2";
     iCountTestcases++;
     ch = '\0';
     str2 = new String(ch, 4);
     if(!str2.Equals("\0\0\0\0"))
       {
       iCountErrors++;
       printerr( "Error_498ch.2! incorrect string returned for null argument=="+str2);
       printinfo ("str2 is good == " + str2 + " is good");
       } 
     strLoc = "Loc_982hc";
     iCountTestcases++;
     ch = 'k';
     str2 = new String (ch, 3);
     if(!str2.Equals("kkk"))
       {
       iCountErrors++;
       printerr( "Error_49hcx! incorrect string constructed=="+str2);
       }
     IntlStrings intl = new IntlStrings();
     str2 = intl.GetString(1, true, true);
     String testString = new String(str2[0], 3);
     str2 = String.Concat(str2[0], str2[0], str2[0]);
     if(!str2.Equals(testString))
       iCountErrors++;
     strLoc = "Loc_9fhc1";
     ch = '\t';
     str2 = new String(ch, 2);
     iCountTestcases++;
     if(!str2.Equals("\t\t"))
       {
       iCountErrors++;
       printerr( "Error_89hfd! incorrect string constructed=="+str2);
       }
     strLoc = "Loc_93286.2";
     iCountTestcases++;
     try {
     str2 = new String ('a', -1);
     iCountErrors++;
     printerr( "Error_209xu.2! Expected ArgExc instead Returned string =="+str2);
     }
     catch (ArgumentOutOfRangeException) {
     }
     strLoc = "Loc_93286.3";
     iCountTestcases++;
     try {
     str2 = new String ('a', 1000); 
     if (str2.Length != 1000) {
     iCountErrors++;
     printerr( "Error_209xu.3! String length mismatch; Expected: " + 1000 + "; Returned: " + str2.Length);
     }
     }
     catch (Exception exc) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_777yyy!  strLoc=="+ strLoc +", unexpected exc=="+exc.ToString());
     }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co4832ctor_char_int cbA = new Co4832ctor_char_int();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
