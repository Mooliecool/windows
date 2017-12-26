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
using System;
using System.IO;
using System.Collections;
using System.Globalization;
public class Co3901ToString_StrIFP
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.ToString(String, IServiceObjectProvider)";
 public static String s_strTFName        = "Co3901ToString_StrIFP.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   MyEnum myn1;
   MyEnumFlags myn2;
   Enum en1;
   MyEnumByte myen_b;
   MyEnumSByte myen_sb;
   MyEnumShort myen_sob;
   MyEnumInt myen_i;
   MyEnumLong myen_l;
   MyEnumUShort myen_us;
   MyEnumUInt myen_ui;
   MyEnumULong myen_ul;
   NumberFormatInfo numInfo;
   CultureInfo culInfo;
   DateTimeFormatInfo dateInfo;
   try {
   strLoc="Loc_7539fd";
   numInfo = new NumberFormatInfo();
   numInfo.NegativeSign = "^";  
   numInfo.NumberDecimalDigits = 3;    
   myn1 = MyEnum.MINUSONE;
   strLoc="Loc_7539fd";
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals("MINUSONE")) {
   iCountErrors++;
   Console.WriteLine("Err_43295sdg! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals("-1")) {
   iCountErrors++;
   Console.WriteLine("Err_8329fddf! Expected value wasn't returned, " + myn1.ToString("D", numInfo));
   }	
   strLoc="Loc_57234df";
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("FFFFFFFF")) {
   iCountErrors++;
   Console.WriteLine("Err_7439gdf! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   myn1 = MyEnum.ONE | MyEnum.TWO;
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_32497fsdg! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_738cfdg! Expected value wasn't returned, " + myn1.ToString( "D", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_0724wef! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   en1 = MyEnum.ONE | MyEnum.TWO;
   iCountTestcases++;
   if(!en1.ToString("G", numInfo).Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_743fd! Expected value wasn't returned, " + en1.ToString("G", numInfo));
   }	
   strLoc="Loc_412ds";
   myn1 = MyEnum.ONE & MyEnum.TWO;
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_239756sdg! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_4532rgt! Expected value wasn't returned, " + myn1.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("00000000")) {
   iCountErrors++;
   Console.WriteLine("Err_29745wdsg! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   myn1 = MyEnum.FOUR ^ MyEnum.EIGHT;
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_24967dsg! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_5834fbdtg! Expected value wasn't returned, " + myn1.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("0000000C")) {
   iCountErrors++;
   Console.WriteLine("Err_2976dsg! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   myn1 = ~MyEnum.FOUR;
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals((~4).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_3976dsg! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals((~4).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_$25dsfg! Expected value wasn't returned, " + myn1.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("FFFFFFFB")) {
   iCountErrors++;
   Console.WriteLine("Err_2756sfg! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   strLoc="Loc_429ds";
   myn1 = MyEnum.FOUR;
   myn1++;
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_29475dsg! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_74352fdg! Expected value wasn't returned, " + myn1.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("00000005")) {
   iCountErrors++;
   Console.WriteLine("Err_239476sdg! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   strLoc="Loc_5423ecsd";
   myn1 = MyEnum.FOUR;
   myn1--;
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_1935dsg! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_97823dv! Expected value wasn't returned, " + myn1.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_20467dsg! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   strLoc="Loc_4562fewd";
   myn2 = MyEnumFlags.ONE | MyEnumFlags.TWO;
   iCountTestcases++;
   if(!myn2.ToString("G", numInfo).Equals("ONE, TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_2947wsdg! Expected value wasn't returned, " + myn2.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn2.ToString("D", numInfo).Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_703dv! Expected value wasn't returned, " + myn2.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myn2.ToString("X", numInfo).Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_2349067sdg! Expected value wasn't returned, " + myn2.ToString("X", numInfo));
   }	
   strLoc="Loc_53fd";
   myen_b = MyEnumByte.ONE;
   iCountTestcases++;
   if(!myen_b.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_3497sdfgf! Expected value wasn't returned, " + myen_b.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_b.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_1352fdg! Expected value wasn't returned, " + myen_b.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_b.ToString("X", numInfo).Equals("01")) {
   iCountErrors++;
   Console.WriteLine("Err_2057sgd! Expected value wasn't returned, " + myen_b.ToString("X", numInfo));
   }	
   myen_sb = MyEnumSByte.ONE;
   iCountTestcases++;
   if(!myen_sb.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_3497dsg! Expected value wasn't returned, " + myen_sb.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_sb.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_7893dvf! Expected value wasn't returned, " + myen_sb.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_sb.ToString("X", numInfo).Equals("01")) {
   iCountErrors++;
   Console.WriteLine("Err_2945gfw! Expected value wasn't returned, " + myen_sb.ToString("X", numInfo));
   }	
   myen_sob = MyEnumShort.ONE;
   iCountTestcases++;
   if(!myen_sob.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_2497sdg! Expected value wasn't returned, " + myen_sob.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_sob.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_0734cfdf! Expected value wasn't returned, " + myen_sob.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_sob.ToString("X", numInfo).Equals("0001")) {
   iCountErrors++;
   Console.WriteLine("Err_9765erg! Expected value wasn't returned, " + myen_sob.ToString("X", numInfo));
   }	
   myen_i = MyEnumInt.ONE;
   iCountTestcases++;
   if(!myen_i.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_3497fgsdf! Expected value wasn't returned, " + myen_i.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_i.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_345svf! Expected value wasn't returned, " + myen_i.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_i.ToString("X", numInfo).Equals("00000001")) {
   iCountErrors++;
   Console.WriteLine("Err_347sdg! Expected value wasn't returned, " + myen_i.ToString("X", numInfo));
   }	
   myen_l = MyEnumLong.ONE;
   iCountTestcases++;
   if(!myen_l.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_275sgg! Expected value wasn't returned, " + myen_l.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_l.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_097834dg! Expected value wasn't returned, " + myen_l.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_l.ToString("X", numInfo).Equals("0000000000000001")) {
   iCountErrors++;
   Console.WriteLine("Err_34967sdg! Expected value wasn't returned, " + myen_l.ToString("X", numInfo));
   }	
   myen_us = MyEnumUShort.ONE;
   iCountTestcases++;
   if(!myen_us.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_2975rsfg! Expected value wasn't returned, " + myen_us.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_us.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_456dsf! Expected value wasn't returned, " + myen_us.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_us.ToString("X", numInfo).Equals("0001")) {
   iCountErrors++;
   Console.WriteLine("Err_290745sdg! Expected value wasn't returned, " + myen_us.ToString("X", numInfo));
   }	
   myen_ui = MyEnumUInt.ONE;
   iCountTestcases++;
   if(!myen_ui.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_2076fsg! Expected value wasn't returned, " + myen_ui.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_ui.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + myen_ui.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_ui.ToString("X", numInfo).Equals("00000001")) {
   iCountErrors++;
   Console.WriteLine("Err_294765sdg! Expected value wasn't returned, " + myen_ui.ToString("X", numInfo));
   }	
   myen_ul = MyEnumULong.ONE;
   iCountTestcases++;
   if(!myen_ul.ToString("G", numInfo).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_276tsdg! Expected value wasn't returned, " + myen_ul.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myen_ul.ToString("D", numInfo).Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + myen_ul.ToString("D", numInfo));
   }	
   iCountTestcases++;
   if(!myen_ul.ToString("X", numInfo).Equals("0000000000000001")) {
   iCountErrors++;
   Console.WriteLine("Err_2367sfg! Expected value wasn't returned, " + myen_ul.ToString("X", numInfo));
   }	
   myn1 = MyEnum.ONE;
   try {
   iCountTestcases++;
   myn1.ToString(null, new NumberFormatInfo());
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   myn1.ToString("H", new NumberFormatInfo());
   iCountErrors++;
   Console.WriteLine("Err_4325fds! Exception not thrown");
   }catch(FormatException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   numInfo = null;
   myn1 = MyEnum.MINUSONE;
   strLoc="Loc_7539fd";
   iCountTestcases++;
   if(!myn1.ToString("G", numInfo).Equals("MINUSONE")) {
   iCountErrors++;
   Console.WriteLine("Err_23975dgs! Expected value wasn't returned, " + myn1.ToString("G", numInfo));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D", numInfo).Equals("-1")) {
   iCountErrors++;
   Console.WriteLine("Err_8329fddf! Expected value wasn't returned, " + myn1.ToString("D", numInfo));
   }	
   strLoc="Loc_57234df";
   iCountTestcases++;
   if(!myn1.ToString("X", numInfo).Equals("FFFFFFFF")) {
   iCountErrors++;
   Console.WriteLine("Err_7439gdf! Expected value wasn't returned, " + myn1.ToString("X", numInfo));
   }	
   culInfo = CultureInfo.CurrentCulture;
   myn1 = MyEnum.MINUSONE;
   iCountTestcases++;
   if(!myn1.ToString("D", culInfo).Equals("-1")) {
   iCountErrors++;
   Console.WriteLine("Err_3745346! Expected value wasn't returned, " + myn1.ToString("D", culInfo));
   }	
   dateInfo = culInfo.DateTimeFormat;
   myn1 = MyEnum.MINUSONE;
   iCountTestcases++;
   if(!myn1.ToString("G", dateInfo).Equals("MINUSONE")) {
   iCountErrors++;
   Console.WriteLine("Err_234967sdg! Expected value wasn't returned, " + myn1.ToString("G", dateInfo));
   }	
   }catch (Exception exc_general){
   ++iCountErrors;
   Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
   }
   if ( iCountErrors == 0 ){
   Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
   return true;
   } else {
   Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
   return false;
   }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3901ToString_StrIFP oCbTest = new Co3901ToString_StrIFP();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev +"FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFName +s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode=0; else Environment.ExitCode=1; 
   }	
}
public enum MyEnum
{
  MINUSONE	= -1,
  ONE	= 1,
  TWO	=	2,
  FOUR	= 4,
  EIGHT = 8,
  SIXTEEN = 16,
}
public enum MyEnumByte:byte
{
 ONE = (byte)1,
   TWO = (byte)2,
   }
public enum MyEnumSByte:sbyte
{
 ONE = (sbyte)1,
   TWO = (sbyte)2,
   }
public enum MyEnumShort:short
{
 ONE = (short)1,
   TWO = (short)2,
   }
public enum MyEnumInt:int
{
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
   }
public enum MyEnumLong:long
{
 ONE	= (long)1,
   TWO	=	(long)2,
   THREE	= (long)3
   }
public enum MyEnumUShort:ushort
{
 ONE = 1,
   TWO = 2,
   }
public enum MyEnumUInt:uint
{
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
   }
public enum MyEnumULong:ulong
{
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
   }
[Flags]
  public enum MyEnumFlags
{
  ONE	= 1,
  TWO	=	2,
  FOUR	= 4,
  EIGHT = 8,
  SIXTEEN = 16,
}
