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
using System.Text;
using System;
using System.IO;
public class Co5472CopyTo_i_chArr_ii
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.CopyTo(Int32, Char[], Int32, Int32)";
 public static String s_strTFName        = "Co5472CopyTo_i_chArr_ii.";
 public static String s_strTFAbbrev      = "Co5472";
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
     String str1;
     Char[] chArr;                       
     int iResult = 0;
     int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
     int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
     int[] iArrValidValues = new Int32[]{ 10000, 5000 , 1000 , 0 };
     IntlStrings intl = new IntlStrings();
     String intlString = intl.GetString(20, true, true);
     Char[] chrArr = intlString.ToCharArray();  
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       intlString.CopyTo(iArrInvalidValues[iLoop], chrArr, 5 , 5);
       iCountErrors++;
       Console.Error.WriteLine( "Error_0000!!!! Expected exception not occured" );
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       intlString.CopyTo(5, chrArr, iArrInvalidValues[iLoop], 5);
       iCountErrors++;
       Console.Error.WriteLine( "Error_4444!!!! Expected exception not occured" );
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_5555!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
     try
       {
       intlString.CopyTo(5, chrArr, 5, iArrInvalidValues[iLoop] );
       iCountErrors++;
       Console.Error.WriteLine( "Error_7777!!!! Expected exception not occured" );
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_8888!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       intlString.CopyTo(iArrLargeValues[iLoop], chrArr, 5 , 5);
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_2134!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       intlString.CopyTo(5, chrArr, iArrLargeValues[iLoop], 5);
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_8787!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
     try
       {
       intlString.CopyTo(5, chrArr, 5, iArrLargeValues[iLoop]);
       } catch ( ArgumentOutOfRangeException ){
       } catch ( Exception ex )
	 {
	 Console.Error.WriteLine( "Error_43433!!! Unexpected exception " + ex.ToString() );
	 iCountErrors++ ;
	 }
     }
     intlString = intl.GetString( 100000, true , true );
     str1 = intlString ;
     iCountTestcases++;
     for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
     try
       {                            
       chrArr = new Char[iArrValidValues[iLoop]];
       intlString.CopyTo(iLoop*100, chrArr , 0, iArrValidValues[iLoop]);
       if ( iResult != 0 ){
       iCountErrors++;
       Console.Error.WriteLine( "Error_8787!!!! CopyTo returned incorrect value.... Expected...{0},  Actual...{1}", 0, iResult );
       }
       } catch ( Exception ex ){
       Console.Error.WriteLine( "Error_4564!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
     }
     strLoc = "Loc_0939v";
     str1 = String.Empty;
     iCountTestcases++;
     try {
     str1.CopyTo(0, null, 0, 0);
     iCountErrors++;
     printerr( "Error_1908c! ArgumentNullException expected");
     } catch (ArgumentNullException aExc) {
     Console.WriteLine("Info_18f84! Caught Expected Exception , exc=="+aExc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr("Error_0129x! ArgumentNullException expected, got exc=="+exc.ToString());
     }
     strLoc = "Loc_09ucd";
     str1 = String.Empty;
     chArr = new Char[0];
     iCountTestcases++;
     try {
     str1.CopyTo(-1, chArr, 0, 0);
     iCountErrors++;
     printerr( "Error_109uc! ArgumentOutOfRangeException expected");
     } catch (ArgumentOutOfRangeException aExc) {
     Console.WriteLine("Info_109fa! Caught expected exception , exc=="+aExc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_1098u! ArgumentOutOfRangeException expected, got exc=="+exc.ToString());
     }
     strLoc = "Loc_09f8v";
     str1 = String.Empty;
     chArr = new Char[0];
     iCountTestcases++;
     try {
     str1.CopyTo(1, chArr, 0, 0);
     iCountErrors++;
     printerr( "Error_1908v! ArgumentOutOfRangeException expected");
     } catch ( ArgumentOutOfRangeException aExc) {
     Console.WriteLine("Info_09t95! Caught expected exception , exc=="+aExc.Message);
     } catch ( Exception exc) {
     iCountErrors++;
     printerr( "Error_228ch! ArgumentOutOfRangeException expected, got exc=="+exc.ToString());
     }
     strLoc = "Loc_98yt5";
     str1 = "Hello";
     chArr = new Char[0];
     iCountTestcases++;
     try {
     str1.CopyTo(3, chArr, -1, 1);
     iCountErrors++;
     printerr( "Error_498gb! ArgumentOutOfRangeException expected");
     } catch (ArgumentOutOfRangeException aExc) {
     Console.WriteLine("Info_298vh! Caught expected exception , exc=="+aExc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_20g9f! ArgumentOutOfRangeException expected, got exc=="+exc.ToString());
     }
     strLoc = "Loc_2998c";
     str1 = "Hello";
     chArr = new Char[0];
     iCountTestcases++;
     try {
     str1.CopyTo(2, chArr, 0, 1);
     iCountErrors++;
     printerr("Error_28gh8! ArgumentOutOfRangeException expected");
     } catch ( ArgumentOutOfRangeException aExc) {
     Console.WriteLine("Info_298gb! Caught expected excpetion , exc=="+aExc.Message);
     } catch ( Exception exc ) {
     iCountErrors++;
     printerr( "Error_298hb! ArgumentOutOfRangeException expected, got exc=="+exc.ToString());
     }
     strLoc = "Loc_209uc";
     str1 = "HelloWorld";
     chArr = new Char[2];
     str1.CopyTo(4, chArr, 0, 2);
     iCountTestcases++;
     if(chArr[0] != 'i' && chArr[1] != 'W')
       {
       iCountErrors++;
       printerr( "Error_287fv! Incorrect chars in array [0]=="+chArr[0].ToString()+" , [1]=="+chArr[1].ToString());
       }
     intl = new IntlStrings();
     str1 = intl.GetString(8, true, true);
     chArr = new Char[2];
     str1.CopyTo(0, chArr, 0, 2);
     iCountTestcases++;
     if(chArr[0] != str1[0] && chArr[1] != str1[1])
       {
       iCountErrors++;
       printerr( "Error_287fv! Incorrect chars in array [0]=="+chArr[0].ToString()+" , [1]=="+chArr[1].ToString());
       }
     strLoc = "Loc_298hg";
     str1 = "HelloWorld";
     chArr = new Char[15];
     str1.CopyTo(4, chArr, 5, 2);
     iCountTestcases++;
     if(chArr[5] != 'i' && chArr[6] != 'W')
       {
       iCountErrors++;
       printerr( "Error_7yg73! Incorrect chars in array [5]=="+chArr[5].ToString()+" , [6]=="+chArr.ToString());
       }
     strLoc = "Loc_298hg";
     str1 = "Bug";
     chArr = new Char[3];
     str1.CopyTo(0, chArr, 0, 3);
     iCountTestcases++;
     if(chArr[0] != 'B' && chArr[1] != 'u' && chArr[2] != 'g')
       {
       iCountErrors++;
       printerr( "Error_27ggb! Incorrect chars in array");
       }
     str1 = "Bug";
     chArr = new Char[7];
     str1.CopyTo(0, chArr, 2, 3);
     iCountTestcases++;
     if(chArr[2] != 'B' && chArr[3] != 'u' && chArr[4] != 'g')
       {
       iCountErrors++;
       printerr( "Error_958vh! Incorrect chars in array");
       }
     strLoc = "Loc_27hgt";
     str1 = "Bug";
     chArr = new Char[10];
     iCountTestcases++;
     try {
     str1.CopyTo(2, chArr, 3, 2);
     Console.WriteLine(chArr[2]);
     iCountErrors++;
     printerr( "Error_2gh88! ArgumentOutOfRangeException expected");
     } catch ( ArgumentOutOfRangeException aExc) {
     Console.WriteLine("Info_498gh! Caught expected Exception, exc=="+aExc.Message);
     } catch ( Exception exc) {
     iCountErrors++;
     printerr( "Error_209u9! ArgumentException expected , got exc=="+exc.ToString());
     }
     str1 = "Bug";
     chArr = new Char[4];
     iCountTestcases++;
     try {
     str1.CopyTo(1, chArr, 3, 3);
     iCountErrors++;
     printerr( "Error_29hvg! ArgumentException expected");
     } catch ( ArgumentException aExc) {
     Console.WriteLine("Info_287gb! Caught expected Exception, exc=="+aExc.Message);
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_298g8! ArgumentException expected, got exc=="+exc.ToString());
     }
     StringBuilder sb1 = new StringBuilder("");
     for(int i = 0 ; i < 100000 ; i++)
       sb1.Append('o');
     str1 = sb1.ToString();
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
   Co5472CopyTo_i_chArr_ii cbA = new Co5472CopyTo_i_chArr_ii();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
