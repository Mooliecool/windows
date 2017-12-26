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
public class Co8505GetHashCode
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.GetHashCode()";
 public static String s_strTFName        = "Co8505GetHashCode.cs";
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
   MyEnumFlags enflg1;
   MyEnumByte mbt1;		
   MyEnumShort ms1;
   MyEnumInt mi1;
   MyEnumLong ml1;
   MyEnumSByte msb1;
   MyEnumUShort mus1;
   MyEnumUInt mui1;
   MyEnumULong mul1;
   try {
   strLoc="Loc_7539fd";
   myn1 = MyEnum.ONE;
   iCountTestcases++;
   if(myn1.GetHashCode() != 1) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.GetHashCode());
   }
   myn1 = MyEnum.SIXTEEN;
   iCountTestcases++;
   if(myn1.GetHashCode() != 16) {
   iCountErrors++;
   Console.WriteLine("Err_8765sg! Expected value wasn't returned, " + myn1.GetHashCode());
   }
   myn1 = MyEnum.MINUS_VALUE;
   iCountTestcases++;
   if(myn1.GetHashCode() != -5) {
   iCountErrors++;
   Console.WriteLine("Err_9736sdg! Expected value wasn't returned, " + myn1.GetHashCode());
   }
   myn1 = MyEnum.TWO | MyEnum.EIGHT;
   iCountTestcases++;
   if(myn1.GetHashCode() != 10) {
   iCountErrors++;
   Console.WriteLine("Err_9347dsg! Expected value wasn't returned, " + myn1.GetHashCode());
   }
   myn1 = MyEnum.MINUS_VALUE | MyEnum.EIGHT;
   iCountTestcases++;
   if(myn1.GetHashCode() != -5) {
   iCountErrors++;
   Console.WriteLine("Err_89sdg! Expected value wasn't returned, " + myn1.GetHashCode());
   }
   enflg1 = MyEnumFlags.ONE | MyEnumFlags.TWO;
   iCountTestcases++;
   if(enflg1.GetHashCode() != 3) {
   iCountErrors++;
   Console.WriteLine("Err_8374sdg! Expected value wasn't returned, " + enflg1.GetHashCode());
   }
   mbt1 = MyEnumByte.ONE;
   iCountTestcases++;
   if(mbt1.GetHashCode() != 1) {
   iCountErrors++;
   Console.WriteLine("Err_97346gs! Expected value wasn't returned, " + mbt1.GetHashCode());
   }	
   ms1 = MyEnumShort.ONE;
   iCountTestcases++;
   if(ms1.GetHashCode() != ((short)1).GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_27dg4! Expected value wasn't returned, " + ms1.GetHashCode());
   }	
   mi1 = MyEnumInt.ONE;
   iCountTestcases++;
   if(mi1.GetHashCode() != 1) {
   iCountErrors++;
   Console.WriteLine("Err_2075sg! Expected value wasn't returned, " + mi1.GetHashCode());
   }	
   ml1 = MyEnumLong.ONE;
   iCountTestcases++;
   if(ml1.GetHashCode() != 1) {
   iCountErrors++;
   Console.WriteLine("Err_7295sg! Expected value wasn't returned, " + ml1.GetHashCode());
   }	
   msb1 = MyEnumSByte.ONE;
   iCountTestcases++;
   if(msb1.GetHashCode() != ((SByte)1).GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_207sg! Expected value wasn't returned, " + msb1.GetHashCode());
   }	
   mus1 = MyEnumUShort.ONE;
   iCountTestcases++;
   if(mus1.GetHashCode() != 1) {
   iCountErrors++;
   Console.WriteLine("Err_3072sgv! Expected value wasn't returned, " + mus1.GetHashCode());
   }	
   mui1 = MyEnumUInt.ONE;
   iCountTestcases++;
   if(mui1.GetHashCode() != 1) {
   iCountErrors++;
   Console.WriteLine("Err_072wfd! Expected value wasn't returned, " + mui1.GetHashCode());
   }	
   mul1 = MyEnumULong.ONE;
   iCountTestcases++;
   if(mul1.GetHashCode() != 1) {
   iCountErrors++;
   Console.WriteLine("Err_2065sg! Expected value wasn't returned, " + mul1.GetHashCode());
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
   Co8505GetHashCode oCbTest = new Co8505GetHashCode();
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
  MINUS_VALUE = -5,
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
public enum MyEnumShort:short
{
 ONE = 1,
   TWO = 2,
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
public enum MyEnumSByte:sbyte
{
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
