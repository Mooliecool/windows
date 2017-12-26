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
public class Co8509ToString
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.ToString()";
 public static String s_strTFName        = "Co8509ToString.cs";
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
   MyEnumByte myen_b;
   MyEnumSByte myen_sb;
   MyEnumShort myen_sob;
   MyEnumInt myen_i;
   MyEnumLong myen_l;
   MyEnumUShort myen_us;
   MyEnumUInt myen_ui;
   MyEnumULong myen_ul;
   try {
   strLoc="Loc_7539fd";
   myn1 = MyEnum.MINUSONE;			
   iCountTestcases++;
   if(!myn1.ToString().Equals("MINUSONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_927435dg";
   myn1 = MyEnum.ONE | MyEnum.TWO;
   iCountTestcases++;
   if(!myn1.ToString().Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_973245werg! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_412ds";
   myn1 = MyEnum.ONE & MyEnum.TWO;
   iCountTestcases++;
   if(!myn1.ToString().Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_2107856wsdg! Expected value wasn't returned, " + myn1.ToString());
   }	
   myn1 = MyEnum.FOUR ^ MyEnum.EIGHT;
   iCountTestcases++;
   if(!myn1.ToString().Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_2075sg! Expected value wasn't returned, " + myn1.ToString());
   }	
   myn1 = ~MyEnum.FOUR;
   iCountTestcases++;
   if(!myn1.ToString().Equals("-5")) {
   iCountErrors++;
   Console.WriteLine("Err_234076wedg! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_429ds";
   myn1 = MyEnum.FOUR;
   myn1++;
   iCountTestcases++;
   if(!myn1.ToString().Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_20756tsdg! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_5423ecsd";
   myn1 = MyEnum.FOUR;
   myn1--;
   iCountTestcases++;
   if(!myn1.ToString().Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_234967sd! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_4562fewd";
   myn2 = MyEnumFlags.ONE | MyEnumFlags.TWO;
   iCountTestcases++;
   if(!myn2.ToString().Equals(("ONE, TWO").ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_5827eag! Expected value wasn't returned, " + myn2.ToString());
   }	
   strLoc="Loc_53fd";
   myen_b = MyEnumByte.ONE;
   iCountTestcases++;
   if(!myen_b.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_3496dfg! Expected value wasn't returned, " + myen_b.ToString());
   }	
   myen_sb = MyEnumSByte.ONE;
   iCountTestcases++;
   if(!myen_sb.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_435dg! Expected value wasn't returned, " + myen_sb.ToString());
   }	
   myen_sb = MyEnumSByte.MINUS_FIVE;
   iCountTestcases++;
   if(!myen_sb.ToString().Equals("MINUS_FIVE")) {
   iCountErrors++;
   Console.WriteLine("Err_4376tg! Expected value wasn't returned, " + myen_sb.ToString());
   }	
   myen_sob = MyEnumShort.ONE;
   iCountTestcases++;
   if(!myen_sob.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_46534wrsdg! Expected value wasn't returned, " + myen_sob.ToString());
   }	
   myen_sob = MyEnumShort.MINUS_FIVE;
   iCountTestcases++;
   if(!myen_sob.ToString().Equals("MINUS_FIVE")) {
   iCountErrors++;
   Console.WriteLine("Err_2465dsg! Expected value wasn't returned, " + myen_sob.ToString());
   }	
   myen_i = MyEnumInt.ONE;
   iCountTestcases++;
   if(!myen_i.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_245sdg! Expected value wasn't returned, " + myen_i.ToString());
   }	
   myen_i = MyEnumInt.MINUS_FIVE;
   iCountTestcases++;
   if(!myen_i.ToString().Equals("MINUS_FIVE")) {
   iCountErrors++;
   Console.WriteLine("Err_2345rd! Expected value wasn't returned, " + myen_i.ToString());
   }	
   myen_l = MyEnumLong.ONE;
   iCountTestcases++;
   if(!myen_l.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_2345rsdfgd! Expected value wasn't returned, " + myen_l.ToString());
   }	
   myen_l = MyEnumLong.MINUS_FIVE;
   iCountTestcases++;
   if(!myen_l.ToString().Equals("MINUS_FIVE")) {
   iCountErrors++;
   Console.WriteLine("Err_393453gs! Expected value wasn't returned, " + myen_l.ToString());
   }	
   myen_us = MyEnumUShort.ONE;
   iCountTestcases++;
   if(!myen_us.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_23qasf! Expected value wasn't returned, " + myen_us.ToString());
   }	
   myen_ui = MyEnumUInt.ONE;
   iCountTestcases++;
   if(!myen_ui.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_346efg! Expected value wasn't returned, " + myen_ui.ToString());
   }	
   myen_ul = MyEnumULong.ONE;
   iCountTestcases++;
   if(!myen_ul.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_245sdg! Expected value wasn't returned, " + myen_ul.ToString());
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
   Co8509ToString oCbTest = new Co8509ToString();
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
  MINUSONE = -1,
  ONE	= 1,
  TWO	=	2,
  FOUR	= 4,
  EIGHT = 8,
  SIXTEEN = 16,
  SEVENTEEN = 17,
  EIGHTEEN = 18,
  NINETEEN = 19,
}
public enum MyEnumByte:byte
{
 ONE = (byte)1,
   TWO = (byte)2,
   }
public enum MyEnumShort:short
{
 MINUS_FIVE = -5,
   ONE = 1,
   TWO = 2,
   }
public enum MyEnumInt:int
{
 MINUS_FIVE = -5,
   ONE	= 1,
   TWO	=	2,
   THREE	= 3
   }
public enum MyEnumLong:long
{
 MINUS_FIVE = -5,
   ONE	= (long)1,
   TWO	=	(long)2,
   THREE	= (long)3
   }
public enum MyEnumSByte:sbyte
{
 MINUS_FIVE = -5,
   ONE = 1,
   TWO = -2,
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
 ONE	= (long)1,
   TWO	=	(long)2,
   THREE	= (long)3
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
[Flags]
  public enum SillyEnum{
    Jack = 1,
    Jill = 1,
  }
