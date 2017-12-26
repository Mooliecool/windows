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
public class Co3877IsDefined_tp_o
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.IsDefined";
 public static String s_strTFName        = "Co3877IsDefined_tp_o.cs";
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
   if(!Enum.IsDefined(typeof(MyEnum), 1)) {
   iCountErrors++;
   Console.WriteLine("Err_1056cds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(typeof(MyEnum), 4)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(MyEnum), "ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_025df! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(typeof(MyEnum), "One")) {
   iCountErrors++;
   Console.WriteLine("Err_04556ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(MyEnum), MyEnum.ONE)) {
   iCountErrors++;
   Console.WriteLine("Err_47cdw! expected exception not thrown");
   }
   try {
   iCountTestcases++;
   Enum.IsDefined(null, MyEnum.ONE);
   iCountErrors++;
   Console.WriteLine("Err_025dswom! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0556cfd! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.IsDefined(typeof(MyEnum), null);
   iCountErrors++;
   Console.WriteLine("Err_6824sfd! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_104ds! unexpected exception thrown, " + ex);
   }
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(MyEnumByte), (byte)1)) {
   iCountErrors++;
   Console.WriteLine("Err_04687fcd! Expected value wasn't returned");
   }						
   try {
   iCountTestcases++;
   Enum.IsDefined(typeof(MyEnumByte), 1);
   iCountErrors++;
   Console.WriteLine("Err_5632sd! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_543012d! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.IsDefined(typeof(Int32), 1);
   iCountErrors++;
   Console.WriteLine("Err_7653cd! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_5738cvd! unexpected exception thrown, " + ex);
   }
   iCountTestcases++;
   if(!Enum.IsDefined(typeof(MyEnumByte), "ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_6520d! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(typeof(MyEnumByte), "One")) {
   iCountErrors++;
   Console.WriteLine("Err_5t639fd! Expected value wasn't returned");
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
   Co3877IsDefined_tp_o oCbTest = new Co3877IsDefined_tp_o();
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
