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
public class Co8506GetName_TypeObj
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.GetName(Type, Object)";
 public static String s_strTFName        = "Co8506GetName_TypeObj.cs";
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
   iCountTestcases++;
   try{
   Enum.GetName(null, MyEnum.ONE);
   iCountErrors++;
   Console.WriteLine("Err_756eg! Exception was not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_29475dg! Unexpected Exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   Enum.GetName(typeof(MyEnum), null);
   iCountErrors++;
   Console.WriteLine("Err_987643sdg! Exception was not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_20875sg! Unexpected Exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   Enum.GetName(typeof(String), MyEnum.ONE);
   iCountErrors++;
   Console.WriteLine("Err_9436dg! Exception was not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_03486sdg! Unexpected Exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   Enum.GetName(typeof(MyEnum), "This is not an Enum value");
   iCountErrors++;
   Console.WriteLine("Err_9745sdg! Exception was not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_93745sdg! Unexpected Exception thrown, " + ex.GetType().Name);
   }
   myn1 = MyEnum.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), myn1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), myn1));
   }
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), 1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_9475gs! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), 1));
   }
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), 100) != null) {
   iCountErrors++;
   Console.WriteLine("Err_9475gs! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), 100));
   }
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), (MyEnum)1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_874fg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), (MyEnum)1));
   }
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumByte), 1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_874fg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumByte), 1));
   }
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), 100) != null) {
   iCountErrors++;
   Console.WriteLine("Err_9475gs! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), 100));
   }
   myn1 = MyEnum.SIXTEEN;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), myn1) != "SIXTEEN") {
   iCountErrors++;
   Console.WriteLine("Err_3292756tsdg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), myn1));
   }
   myn1 = MyEnum.MINUS_VALUE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), myn1) != "MINUS_VALUE") {
   iCountErrors++;
   Console.WriteLine("Err_874325sdg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), myn1));
   }
   myn1 = MyEnum.TWO | MyEnum.EIGHT;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), myn1) != null) {
   iCountErrors++;
   Console.WriteLine("Err_02754sf! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), myn1));
   }
   myn1 = MyEnum.MINUS_VALUE | MyEnum.EIGHT;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), myn1) != "MINUS_VALUE") {
   iCountErrors++;
   Console.WriteLine("Err_329756sdg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), myn1));
   }
   myn1 = MyEnum.SEVENTEEN | MyEnum.EIGHTEEN;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnum), myn1) != "NINETEEN") {
   iCountErrors++;
   Console.WriteLine("Err_374sdg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnum), myn1));
   }
   enflg1 = MyEnumFlags.ONE | MyEnumFlags.TWO;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumFlags), enflg1) != null) {
   iCountErrors++;
   Console.WriteLine("Err_329756sdg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumFlags), enflg1));
   }
   mbt1 = MyEnumByte.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumByte), mbt1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_827435dsf! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumByte), mbt1));
   }
   ms1 = MyEnumShort.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumShort), ms1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_29475dg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumShort), ms1));
   }
   mi1 = MyEnumInt.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumInt), mi1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_24tsdg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumInt), mi1));
   }
   ml1 = MyEnumLong.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumLong), ml1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_97423rsgt! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumLong), ml1));
   }
   msb1 = MyEnumSByte.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumSByte), msb1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_974dg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumSByte), msb1));
   }
   mus1 = MyEnumUShort.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumUShort), mus1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_274tgs! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumUShort), mus1));
   }
   mui1 = MyEnumUInt.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumUInt), mui1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_27345dsg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumUInt), mui1));
   }
   mul1 = MyEnumULong.ONE;
   iCountTestcases++;
   if(Enum.GetName(typeof(MyEnumULong), mul1) != "ONE") {
   iCountErrors++;
   Console.WriteLine("Err_2075wg! Expected value wasn't returned, " + Enum.GetName(typeof(MyEnumULong), mul1));
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
   Co8506GetName_TypeObj oCbTest = new Co8506GetName_TypeObj();
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
