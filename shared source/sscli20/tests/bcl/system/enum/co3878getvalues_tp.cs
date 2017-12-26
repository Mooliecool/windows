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
public class Co3878GetValues_tp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.GetValues(Type)";
 public static String s_strTFName        = "Co3878GetValues_tp.cs";
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
   MyEnum[] arrMyEnum;
   MyEnumLong[] arrMyEnumLong;
   MyEnumStrange[] arrMyEnumStrange;
   Int32[] arrInt32;
   ContentAlignment1[] arrContentAlignment1;
   ContentAlignment2[] arrContentAlignment2;
   Boolean fValue;
   try {
   arrMyEnum = (MyEnum[])Enum.GetValues(typeof(MyEnum));
   for(int i=0; i<arrMyEnum.Length; i++){
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(MyEnum), (int)arrMyEnum[i])) {
   iCountErrors++;
   Console.WriteLine("Err_648fd_" + i + "! Expected value wasn't returned");
   }						
   }
   fValue = false;
   arrMyEnum = (MyEnum[])Enum.GetValues(typeof(MyEnum));									
   for(int i=0; i<arrMyEnum.Length; i++){
   if((int)arrMyEnum[i] != 1) {
   fValue = true;
   }						
   }
   iCountTestcases++;
   if(!fValue) {
   iCountErrors++;
   Console.WriteLine("Err_257fd! Expected value wasn't returned");
   }						
   arrMyEnumLong = (MyEnumLong[])Enum.GetValues(typeof(MyEnumLong));						
   for(int i=0; i<arrMyEnumLong.Length; i++){
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(MyEnumLong), (long)arrMyEnumLong[i])) {
   iCountErrors++;
   Console.WriteLine("Err_65273cs_" + i + "! Expected value wasn't returned");
   }						
   }
   arrMyEnumStrange = (MyEnumStrange[])Enum.GetValues(typeof(MyEnumStrange));						
   for(int i=0; i<arrMyEnumStrange.Length; i++){
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(MyEnumStrange), (int)arrMyEnumStrange[i])) {
   iCountErrors++;
   Console.WriteLine("Err_74329cd_" + i + "! Expected value wasn't returned, " + (int)arrMyEnumStrange[i]);
   }						
   }
   try {
   iCountTestcases++;
   arrInt32 = (Int32[])Enum.GetValues(typeof(Int32));									
   iCountErrors++;
   Console.WriteLine("Err_47cdw! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_53598cd! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   arrInt32 = (Int32[])Enum.GetValues(null);									
   iCountErrors++;
   Console.WriteLine("Err_025dswom! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0556cfd! unexpected exception thrown, " + ex);
   }
   arrContentAlignment1 = (ContentAlignment1[])Enum.GetValues(typeof(ContentAlignment1));						
   for(int i=0; i<arrContentAlignment1.Length; i++){
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(ContentAlignment1), (int)arrContentAlignment1[i])) {
   iCountErrors++;
   Console.WriteLine("Err_648fd_" + i + "! Expected value wasn't returned");
   }						
   }
   arrContentAlignment2 = (ContentAlignment2[])Enum.GetValues(typeof(ContentAlignment2));						
   for(int i=0; i<arrContentAlignment2.Length; i++){
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(ContentAlignment2), (int)arrContentAlignment2[i])) {
   iCountErrors++;
   Console.WriteLine("Err_648fd_" + i + "! Expected value wasn't returned");
   }						
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
   Co3878GetValues_tp oCbTest = new Co3878GetValues_tp();
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
  THREE	= 3
}
public enum MyEnumByte:byte
{
 ONE = (byte)1,
   TWO = (byte)2,
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
public enum MyEnumStrange
{
  ONE	= 1,
  TWO	=	2,
  THREE	= 3,
  FOUR = TWO + TWO,
  OR = ONE | TWO,
  FOREIGN = (Int32)MyEnumLong.ONE + MyEnumShort.TWO,
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
