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
public class Co3962Parse_TpStrBln
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.FromString(Type, String, Boolean)";
 public static String s_strTFName        = "Co3962Parse_TpStrBln.cs";
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
   strLoc="Loc_7539fd";
   iCountTestcases++;
   if((int)Enum.Parse(typeof(MyEnum), "ONE", false)!=1) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if((int)Enum.Parse(typeof(MyEnum), "one", true)!=1) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if((int)Enum.Parse(typeof(MyEnum), "OnE", true)!=1) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned");
   }						
   try {
   iCountTestcases++;
   Enum.Parse(typeof(MyEnumLong), "One", false);
   iCountErrors++;
   Console.WriteLine("Err_47cdw! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_53598cd! unexpected exception thrown, " + ex);
   }
   iCountTestcases++;
   if((int)Enum.Parse(typeof(MyEnum), "TWO", false)!=2) {
   iCountErrors++;
   Console.WriteLine("Err_835df! Expected value wasn't returned");
   }						
   strLoc="Loc_7635fdd";
   iCountTestcases++;
   if((long)Enum.Parse(typeof(MyEnumLong), "two", true)!=2) {
   iCountErrors++;
   Console.WriteLine("Err_0257dfs! Expected value wasn't returned");
   }						
   try {
   iCountTestcases++;
   Enum.Parse(typeof(MyEnumLong), "FOUR", true);
   iCountErrors++;
   Console.WriteLine("Err_47cdw! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_53598cd! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Parse(typeof(Int32), "FOUR", false);
   iCountErrors++;
   Console.WriteLine("Err_025dswom! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0556cfd! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Parse(null, "FOUR", true);
   iCountErrors++;
   Console.WriteLine("Err_046289dfs! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0157fs! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Parse(typeof(MyEnumLong), null, false);
   iCountErrors++;
   Console.WriteLine("Err_653dfs! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_015xs! unexpected exception thrown, " + ex);
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
   Co3962Parse_TpStrBln oCbTest = new Co3962Parse_TpStrBln();
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
