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
using System.Reflection;
public class Co8825IConvertible
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "IConvertible methods in enum";
 public static String s_strTFName        = "Co8825IConvertible.cs";
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
   Enum en1;
   try {
   strLoc="Loc_642cdf";
   iCountTestcases++;
   en1 = MyEnum.ONE;			
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3487sg! Wrong value returned");
   }
   en1 = MyEnumByte.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_34sg! Wrong value returned");
   }
   en1 = MyEnumSByte.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
   }
   en1 = MyEnumShort.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
   }
   en1 = MyEnumInt.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
   }
   en1 = MyEnumLong.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
   }
   en1 = MyEnumUShort.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
   }
   en1 = MyEnumUInt.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
   }
   en1 = MyEnumULong.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
   }
   en1 = MyEnumULong.ONE;
   if(!CallInterfaceMethods(en1)){
   iCountTestcases++;
   Console.WriteLine("Err_3947tsg! Wrong value returned");
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
 private Boolean CallInterfaceMethods(Enum en)
   {
   Boolean pass = true;
   IConvertible icon1 = (IConvertible)en;
   if(!icon1.ToBoolean(null)){
   pass = false;
   }
   if(icon1.ToByte(null)!=1){
   pass = false;
   }
   if(icon1.ToChar(null)!=(Char)1){
   pass = false;
   }
   try{
   icon1.ToDateTime(null);
   pass = false;
   }catch(InvalidCastException){
   }catch(Exception){
   pass = false;
   }
   if(icon1.ToDecimal(null)!=1){
   pass = false;
   }
   if(icon1.ToDouble(null)!=1){
   pass = false;
   }
   if(icon1.ToInt16(null)!=1){
   pass = false;
   }
   if(icon1.ToInt32(null)!=1){
   pass = false;
   }
   if(icon1.ToInt64(null)!=1){
   pass = false;
   }
   if(icon1.ToSByte(null)!=1){
   pass = false;
   }
   if(icon1.ToSingle(null)!=1f){
   pass = false;
   }
   if(icon1.ToString(null)!="ONE"){
   pass = false;
   }
   if(icon1.ToUInt16(null)!=1){
   pass = false;
   }
   if(icon1.ToUInt32(null)!=1){
   pass = false;
   }
   if(icon1.ToUInt64(null)!=1){
   pass = false;
   }
   return pass;
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co8825IConvertible oCbTest = new Co8825IConvertible();
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
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
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
 ONE	= (ushort)1,
   TWO	=	(ushort)2,
   THREE	= (ushort)3
   }
public enum MyEnumUInt:uint
{
 ONE	= (uint)1,
   TWO	=	(uint)2,
   THREE	= (uint)3
   }
public enum MyEnumULong:ulong
{
 ONE	= (ulong)1,
   TWO	=	(ulong)2,
   THREE	= (ulong)3
   }
