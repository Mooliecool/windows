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
public class Co8507GetNames_Type
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.GetNames(Type)";
 public static String s_strTFName        = "Co8507GetNames_Type.cs";
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
   String[] returnedValues;
   Type enumType;
   Array enums;
   Object[] enumsSingle;
   Type[] types = {typeof(MyEnum), typeof(MyEnumByte), typeof(MyEnumShort), typeof(MyEnumInt),
		   typeof(MyEnumLong), typeof(MyEnumSByte), typeof(MyEnumUShort), typeof(MyEnumUInt),
		   typeof(MyEnumULong), typeof(MyEnumFlags), typeof(SillyEnum), 
   };
   MyEnum myen1;
   try {
   strLoc="Loc_7539fd";
   iCountTestcases++;
   try{
   Enum.GetNames(null);
   iCountErrors++;
   Console.WriteLine("Err_756eg! Exception was not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_29475dg! Unexpected Exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   Enum.GetNames(typeof(String));
   iCountErrors++;
   Console.WriteLine("Err_9436dg! Exception was not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_03486sdg! Unexpected Exception thrown, " + ex.GetType().Name);
   }
   for(int iLoop = 0; iLoop<types.Length; iLoop++){
   strLoc="Loc_874sdg_" + iLoop;
   iCountTestcases++;
   enumType = types[iLoop];
   returnedValues = Enum.GetNames(enumType);
   enums = Enum.GetValues(enumType);
   Array.Sort(enums);
   enumsSingle = new Object[enums.Length];
   for(int i =0; i<returnedValues.Length; i++)
     enumsSingle[i] = Enum.Parse(enumType, returnedValues[i]);
   Array.Sort(enumsSingle);
   for(int i =0; i<returnedValues.Length; i++){
   if(!enumsSingle[i].Equals(enums.GetValue(i))){
   iCountErrors++;
   Console.WriteLine("Err_7853sg_" + iLoop + "_" + i + "! Wrong value returned, " + enumsSingle[i].GetType().Name + " " + enums.GetValue(i).GetType().Name);
   }
   }
   }
   myen1 = MyEnum.MINUS_VALUE;
   if(myen1.ToString()!="MINUS_VALUE"){
   iCountErrors++;
   Console.WriteLine("Err_@$597sfg! Unexpected value returned, {0}", MyEnum.MINUS_VALUE);
   }
   returnedValues = Enum.GetNames(typeof(MyEnum));
   Array.Sort(returnedValues);
   if(myen1.ToString()!="MINUS_VALUE"){
   iCountErrors++;
   Console.WriteLine("Err_3247tsg! Unexpected value returned, Expected: {0}, Returned: {1}", "MINUS_VALUE", myen1.ToString());
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
   Co8507GetNames_Type oCbTest = new Co8507GetNames_Type();
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
