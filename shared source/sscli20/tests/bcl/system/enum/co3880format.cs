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
public class Co3880Format
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.ToString()";
 public static String s_strTFName        = "Co3880Format.cs";
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
   ContentAlignment1 con1;
   ContentAlignment2	con2;			
   try {
   strLoc="Loc_7539fd";
   myn1 = MyEnum.ONE;
   Console.WriteLine("MyEnum.ONE, " + myn1.ToString());
   iCountTestcases++;
   if(!myn1.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_57234df";
   myn1 = MyEnum.ONE | MyEnum.TWO;
   Console.WriteLine("MyEnum.ONE | MyEnum.TWO, " + myn1.ToString());
   iCountTestcases++;
   if(!myn1.ToString().Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_653fd! Expected value wasn't returned, " + myn1.ToString());
   }	
   en1 = MyEnum.ONE | MyEnum.TWO;
   iCountTestcases++;
   if(!en1.ToString().Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_743fd! Expected value wasn't returned, " + en1.ToString());
   }	
   strLoc="Loc_412ds";
   myn1 = MyEnum.ONE & MyEnum.TWO;
   Console.WriteLine("MyEnum.ONE & MyEnum.TWO, " + myn1.ToString());
   iCountTestcases++;
   if(!myn1.ToString().Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_106cd! Expected value wasn't returned, " + myn1.ToString());
   }	
   myn1 = MyEnum.FOUR ^ MyEnum.EIGHT;
   Console.WriteLine("MyEnum.FOUR ^ MyEnum.EIGHT, " + myn1.ToString());
   iCountTestcases++;
   if(!myn1.ToString().Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_520xa! Expected value wasn't returned, " + myn1.ToString());
   }	
   myn1 = ~MyEnum.FOUR;
   iCountTestcases++;
   if(!myn1.ToString().Equals((~4).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_6539fd! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_429ds";
   myn1 = MyEnum.FOUR;
   myn1++;
   iCountTestcases++;
   if(!myn1.ToString().Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_6349cd! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_5423ecsd";
   myn1 = MyEnum.FOUR;
   myn1--;
   iCountTestcases++;
   if(!myn1.ToString().Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_203nms! Expected value wasn't returned, " + myn1.ToString());
   }	
   strLoc="Loc_4562fewd";
   myn2 = MyEnumFlags.ONE | MyEnumFlags.TWO;
   iCountTestcases++;
   if(!myn2.ToString().Equals("ONE, TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_0468fd! Expected value wasn't returned, " + myn2.ToString());
   }										
   strLoc="Loc_5329dw";
   myn2 = MyEnumFlags.ONE | MyEnumFlags.FOUR;
   Console.WriteLine("MyEnumFlags.ONE | MyEnumFlags.FOUR, " + myn2.ToString());
   iCountTestcases++;
   if(!myn2.ToString().Equals("ONE, FOUR")) {
   iCountErrors++;
   Console.WriteLine("Err_97436! Expected value wasn't returned, " + myn1.ToString());
   }										
   strLoc="Loc_4527ds";
   myn2 = MyEnumFlags.ONE ^ MyEnumFlags.TWO;
   Console.WriteLine("MyEnumFlags.ONE ^ MyEnumFlags.TWO, " + myn2.ToString());
   iCountTestcases++;
   if(!myn2.ToString().Equals("ONE^TWO") && !myn2.ToString().Equals("3") && !myn2.ToString().Equals("ONE, TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_6538fd! Expected value wasn't returned, " + myn2.ToString());
   }										
   strLoc="Loc_53fd";
   myen_b = MyEnumByte.ONE;
   iCountTestcases++;
   if(!myen_b.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_b.ToString());
   }	
   strLoc="Loc_75432vd";
   myen_sb = MyEnumSByte.ONE;
   iCountTestcases++;
   if(!myen_sb.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_sb.ToString());
   }	
   myen_sob = MyEnumShort.ONE;
   iCountTestcases++;
   if(!myen_sob.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_sob.ToString());
   }	
   strLoc="Loc_63fgd";
   myen_i = MyEnumInt.ONE;
   iCountTestcases++;
   if(!myen_i.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_i.ToString());
   }	
   myen_l = MyEnumLong.ONE;
   iCountTestcases++;
   if(!myen_l.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_l.ToString());
   }	
   strLoc="Loc_1207563gf";
   myen_us = MyEnumUShort.ONE;
   iCountTestcases++;
   if(!myen_us.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_us.ToString());
   }	
   myen_ui = MyEnumUInt.ONE;
   iCountTestcases++;
   if(!myen_ui.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_ui.ToString());
   }	
   strLoc="Loc_21367-gdf";
   myen_ul = MyEnumULong.ONE;
   iCountTestcases++;
   if(!myen_ul.ToString().Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + myen_ul.ToString());
   }	
   con1 = ContentAlignment1.BottomRight;
   iCountTestcases++;
   if(!con1.ToString().Equals("BottomRight")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + con1.ToString());
   }	
   con2 = ContentAlignment2.BottomCenter;
   iCountTestcases++;
   if(!con2.ToString().Equals("BottomCenter")) {
   iCountErrors++;
   Console.WriteLine("Err_7423fds! Expected value wasn't returned, " + con2.ToString());
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
   Co3880Format oCbTest = new Co3880Format();
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
[Flags]
  public enum ContentAlignment1 {
    Left    = 0x0001,
    Center  = 0x0002,
    Right   = 0x0004,
    Middle  = 0x0010,
    Top     = 0x0020,
    Bottom  = 0x0040,
    TopLeft = Top | Left,
    TopCenter = Top | Center,
    TopRight = Top | Right,
    MiddleLeft = Middle | Left,
    MiddleCenter = Middle | Center,
    MiddleRight = Middle | Right,
    BottomLeft = Bottom | Left,
    BottomCenter = Bottom | Center,
    BottomRight = Bottom | Right,
  }
[Flags]
  public enum ContentAlignment2 {
    Right   = 0x0004,
    Left    = 0x0001,
    Center  = 0x0002,
    MiddleCenter = Middle | Center,
    Middle  = 0x0010,
    Top     = 0x0020,
    Bottom  = 0x0040,
    TopLeft = Top | Left,
    TopCenter = Top | Center,
    MiddleLeft = Middle | Left,
    MiddleRight = Middle | Right,
    BottomLeft = Bottom | Left,
    BottomCenter = Bottom | Center,
    BottomRight = Bottom | Right,
    TopRight = Top | Right,
  }
[Flags]
  public enum ContentAlignment3 {
    Right = 1
  }
[Flags]
  public enum ContentAlignment4 {
    Right = 1,
    Left = 0
  }
