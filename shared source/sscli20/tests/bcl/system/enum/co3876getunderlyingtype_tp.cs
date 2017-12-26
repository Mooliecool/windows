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
public class Co3876GetUnderlyingType_tp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.UnderlyingType";
 public static String s_strTFName        = "Co3876GetUnderlyingType_tp.cs";
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
   try {
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(typeof(MyEnum)).Equals(typeof(Int32))) {
   iCountErrors++;
   Console.WriteLine("Err_1056cds! Underlying type is different, expected, " + typeof(Int32) + ", returned, " + Enum.GetUnderlyingType(typeof(MyEnum)));
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(typeof(MyEnumShort)).Equals(typeof(Int16))) {
   iCountErrors++;
   Console.WriteLine("Err_5739fd! Underlying type is different, expected, " + typeof(Int16) + ", returned, " + Enum.GetUnderlyingType(typeof(MyEnumShort)));
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(typeof(MyEnumInt)).Equals(typeof(Int32))) {
   iCountErrors++;
   Console.WriteLine("Err_6y429cds! Underlying type is different, expected, " + typeof(Int32) + ", returned, " + Enum.GetUnderlyingType(typeof(MyEnumInt)));
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(typeof(MyEnumByte)).Equals(typeof(Byte))) {
   iCountErrors++;
   Console.WriteLine("Err_6538cvd! Underlying type is different, expected, " + typeof(Int32) + ", returned, " + Enum.GetUnderlyingType(typeof(MyEnumByte)));
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(typeof(MyEnumLong)).Equals(typeof(Int64))) {
   iCountErrors++;
   Console.WriteLine("Err_6538cvd! Underlying type is different, expected, " + typeof(Int64) + ", returned, " + Enum.GetUnderlyingType(typeof(MyEnumLong)));
   }						
   try {
   iCountTestcases++;
   Enum.GetUnderlyingType(null);
   iCountErrors++;
   Console.WriteLine("Err_47cdw! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_53598cd! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.GetUnderlyingType(typeof(Int32));
   iCountErrors++;
   Console.WriteLine("Err_5673cd! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_6534fd! unexpected exception thrown, " + ex);
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
   Co3876GetUnderlyingType_tp oCbTest = new Co3876GetUnderlyingType_tp();
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
