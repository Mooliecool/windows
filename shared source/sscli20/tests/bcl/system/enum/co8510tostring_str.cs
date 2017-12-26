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
public class Co8510ToString_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.ToString(String)";
 public static String s_strTFName        = "Co8510ToString_str.cs";
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
   String str1;
   try {
   strLoc="Loc_7539fd";
   myn1 = MyEnum.MINUSONE;
   strLoc="Loc_7539fd";
   iCountTestcases++;
   if(!myn1.ToString("G").Equals("MINUSONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D").Equals("-1")) {
   iCountErrors++;
   Console.WriteLine("Err_8329fddf! Expected value wasn't returned, " + myn1.ToString("D"));
   }	
   strLoc="Loc_57234df";
   iCountTestcases++;
   if(!myn1.ToString("X").Equals("FFFFFFFF")) {
   iCountErrors++;
   Console.WriteLine("Err_7439gdf! Expected value wasn't returned, " + myn1.ToString("X"));
   }	
   myn1 = MyEnum.ONE | MyEnum.TWO;
   iCountTestcases++;
   if(!myn1.ToString("G").Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D").Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_738cfdg! Expected value wasn't returned, " + myn1.ToString( "D"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X").Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_24975sdg! Expected value wasn't returned, " + myn1.ToString("X"));
   }	
   en1 = MyEnum.ONE | MyEnum.TWO;
   iCountTestcases++;
   if(!en1.ToString("G").Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_743fd! Expected value wasn't returned, " + en1.ToString("G"));
   }	
   strLoc="Loc_412ds";
   myn1 = MyEnum.ONE & MyEnum.TWO;
   iCountTestcases++;
   if(!myn1.ToString("G").Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D").Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_4532rgt! Expected value wasn't returned, " + myn1.ToString("D"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X").Equals("00000000")) {
   iCountErrors++;
   Console.WriteLine("Err_3976wsg! Expected value wasn't returned, " + myn1.ToString("X"));
   }	
   myn1 = MyEnum.FOUR ^ MyEnum.EIGHT;
   iCountTestcases++;
   if(!myn1.ToString("G").Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D").Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_5834fbdtg! Expected value wasn't returned, " + myn1.ToString("D"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X").Equals("0000000C")) {
   iCountErrors++;
   Console.WriteLine("Err_93846dg! Expected value wasn't returned, " + myn1.ToString("X"));
   }	
   myn1 = ~MyEnum.FOUR;
   iCountTestcases++;
   if(!myn1.ToString("G").Equals((~4).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D").Equals((~4).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_$25dsfg! Expected value wasn't returned, " + myn1.ToString("D"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X").Equals("FFFFFFFB")) {
   iCountErrors++;
   Console.WriteLine("Err_3947sd! Expected value wasn't returned, " + myn1.ToString("X"));
   }	
   strLoc="Loc_429ds";
   myn1 = MyEnum.FOUR;
   myn1++;
   iCountTestcases++;
   if(!myn1.ToString("G").Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D").Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_74352fdg! Expected value wasn't returned, " + myn1.ToString("D"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X").Equals("00000005")) {
   iCountErrors++;
   Console.WriteLine("Err_9376sdg! Expected value wasn't returned, " + myn1.ToString("X"));
   }	
   strLoc="Loc_5423ecsd";
   myn1 = MyEnum.FOUR;
   myn1--;
   iCountTestcases++;
   if(!myn1.ToString("G").Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("D").Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_97823dv! Expected value wasn't returned, " + myn1.ToString("D"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("X").Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_89576sdg! Expected value wasn't returned, " + myn1.ToString("X"));
   }	
   strLoc="Loc_4562fewd";
   myn2 = MyEnumFlags.ONE | MyEnumFlags.TWO;
   iCountTestcases++;
   if(!myn2.ToString("G").Equals("ONE, TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_3476gs! Expected value wasn't returned, " + myn2.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn2.ToString("D").Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_703dv! Expected value wasn't returned, " + myn2.ToString("D"));
   }	
   iCountTestcases++;
   if(!myn2.ToString("X").Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_9756wsdg! Expected value wasn't returned, " + myn2.ToString("X"));
   }	
   strLoc="Loc_53fd";
   myen_b = MyEnumByte.ONE;
   iCountTestcases++;
   if(!myen_b.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_b.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_b.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_1352fdg! Expected value wasn't returned, " + myen_b.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_b.ToString("X").Equals("01")) {
   iCountErrors++;
   Console.WriteLine("Err_9376dsg! Expected value wasn't returned, " + myen_b.ToString("X"));
   }	
   myen_sb = MyEnumSByte.ONE;
   iCountTestcases++;
   if(!myen_sb.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_sb.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_sb.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_7893dvf! Expected value wasn't returned, " + myen_sb.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_sb.ToString("X").Equals("01")) {
   iCountErrors++;
   Console.WriteLine("Err_983476sdg! Expected value wasn't returned, " + myen_sb.ToString("X"));
   }	
   myen_sob = MyEnumShort.ONE;
   iCountTestcases++;
   if(!myen_sob.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_sob.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_sob.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_0734cfdf! Expected value wasn't returned, " + myen_sob.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_sob.ToString("X").Equals("0001")) {
   iCountErrors++;
   Console.WriteLine("Err_893746dg! Expected value wasn't returned, " + myen_sob.ToString("X"));
   }	
   myen_i = MyEnumInt.ONE;
   iCountTestcases++;
   if(!myen_i.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_i.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_i.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_345svf! Expected value wasn't returned, " + myen_i.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_i.ToString("X").Equals("00000001")) {
   iCountErrors++;
   Console.WriteLine("Err_974326sdg! Expected value wasn't returned, " + myen_i.ToString("X"));
   }	
   myen_l = MyEnumLong.ONE;
   iCountTestcases++;
   if(!myen_l.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_l.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_l.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_097834dg! Expected value wasn't returned, " + myen_l.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_l.ToString("X").Equals("0000000000000001")) {
   iCountErrors++;
   Console.WriteLine("Err_975623wqfsdg! Expected value wasn't returned, " + myen_l.ToString("X"));
   }	
   myen_us = MyEnumUShort.ONE;
   iCountTestcases++;
   if(!myen_us.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_us.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_us.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_456dsf! Expected value wasn't returned, " + myen_us.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_us.ToString("X").Equals("0001")) {
   iCountErrors++;
   Console.WriteLine("Err_2976wdg! Expected value wasn't returned, " + myen_us.ToString("X"));
   }	
   myen_ui = MyEnumUInt.ONE;
   iCountTestcases++;
   if(!myen_ui.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_ui.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_ui.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + myen_ui.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_ui.ToString("X").Equals("00000001")) {
   iCountErrors++;
   Console.WriteLine("Err_10746wtadg! Expected value wasn't returned, " + myen_ui.ToString("X"));
   }	
   myen_ul = MyEnumULong.ONE;
   iCountTestcases++;
   if(!myen_ul.ToString("G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myen_ul.ToString("G"));
   }	
   iCountTestcases++;
   if(!myen_ul.ToString("D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + myen_ul.ToString("D"));
   }	
   iCountTestcases++;
   if(!myen_ul.ToString("X").Equals("0000000000000001")) {
   iCountErrors++;
   Console.WriteLine("Err_945asgf! Expected value wasn't returned, " + myen_ul.ToString("X"));
   }	
   myn1 = MyEnum.ONE;
   iCountTestcases++;
   str1 = null;
   if(!myn1.ToString(str1).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_89723refadg! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   myn1 = MyEnum.ONE;
   iCountTestcases++;
   str1 = String.Empty;
   if(!myn1.ToString(str1).Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_2056sg! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   try {
   iCountTestcases++;
   myn1.ToString("H");
   iCountErrors++;
   Console.WriteLine("Err_2346wsdg! Exception not thrown");
   }catch(FormatException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   myn1 = MyEnum.MINUSONE;
   strLoc="Loc_7539fd";
   iCountTestcases++;
   if(!myn1.ToString("g").Equals("MINUSONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString("G"));
   }	
   iCountTestcases++;
   if(!myn1.ToString("d").Equals("-1")) {
   iCountErrors++;
   Console.WriteLine("Err_8329fddf! Expected value wasn't returned, " + myn1.ToString("D"));
   }	
   strLoc="Loc_57234df";
   iCountTestcases++;
   if(!myn1.ToString("x").Equals("FFFFFFFF")) {
   iCountErrors++;
   Console.WriteLine("Err_7439gdf! Expected value wasn't returned, " + myn1.ToString("X"));
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
   Co8510ToString_str oCbTest = new Co8510ToString_str();
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
