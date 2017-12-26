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
public class Co3881CompareTo_o
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.CompareTo()";
 public static String s_strTFName        = "Co3881CompareTo_o.cs";
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
   MyEnum myn2;
   MyEnumByte mbt1;
   try {
   strLoc="Loc_7539fd";
   myn1 = MyEnum.ONE;
   myn2 = MyEnum.ONE;
   iCountTestcases++;
   if(myn1.CompareTo(myn2)!=0) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + myn1.CompareTo(myn2));
   }	
   myn1 = MyEnum.ONE;
   myn2 = MyEnum.TWO;
   iCountTestcases++;
   if(myn1.CompareTo(myn2)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_7583fcd! Expected value wasn't returned, " + myn1.CompareTo(myn2));
   }	
   myn1 = MyEnum.TWO;
   myn2 = MyEnum.ONE;
   iCountTestcases++;
   if(myn1.CompareTo(myn2)!=1) {
   iCountErrors++;
   Console.WriteLine("Err_925cde! Expected value wasn't returned, " + myn1.CompareTo(myn2));
   }	
   try{
   myn1 = MyEnum.TWO;
   mbt1 = MyEnumByte.ONE;
   iCountTestcases++;
   myn1.CompareTo(mbt1);
   iCountErrors++;
   Console.WriteLine("Err_04561! Exception wasn't thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_562lqm! Unexpected exception thrown, " + ex);
   }
   try{
   myn1 = MyEnum.TWO;
   iCountTestcases++;
   myn1.CompareTo(4);
   iCountErrors++;
   Console.WriteLine("Err_6724ds! Exception wasn't thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0264fsd! Unexpected exception thrown, " + ex);
   }
   try{
   myn1 = MyEnum.TWO;
   iCountTestcases++;
   if(myn1.CompareTo(null)!=1){				
   iCountErrors++;
   Console.WriteLine("Err_05572xs! Exception wasn't thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_6523c-s! Unexpected exception thrown, " + ex);
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
   Co3881CompareTo_o oCbTest = new Co3881CompareTo_o();
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
[Flags]
  public enum MyEnumFlags
{
  ONE	= 1,
  TWO	=	2,
  FIVE = 5,
  EIGHT = 8,
  SIXTEEN = 16,
}
