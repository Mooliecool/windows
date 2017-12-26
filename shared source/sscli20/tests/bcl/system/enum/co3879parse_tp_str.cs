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
public class Co3879Parse_tp_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.Parse(Type, String)";
 public static String s_strTFName        = "Co3879Parse_tp_str.cs";
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
   MyEnum myen1;
   MyEnumFlags myf1;
   try {
   strLoc="Loc_7539fd";
   iCountTestcases++;
   if((int)Enum.Parse(typeof(MyEnum), "ONE")!=1) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if((int)Enum.Parse(typeof(MyEnum), "TWO")!=2) {
   iCountErrors++;
   Console.WriteLine("Err_835df! Expected value wasn't returned");
   }						
   strLoc="Loc_7635fdd";
   iCountTestcases++;
   if((long)Enum.Parse(typeof(MyEnumLong), "TWO")!=2) {
   iCountErrors++;
   Console.WriteLine("Err_0257dfs! Expected value wasn't returned");
   }
   strLoc="Loc_7635fdd";
   iCountTestcases++;
   if((int)Enum.Parse(typeof(MyEnumFlags), "TWO")!=2) {
   iCountErrors++;
   Console.WriteLine("Err_347tsfg! Expected value wasn't returned");
   }
   strLoc="Loc_82475sdg";
   iCountTestcases++;
   myen1 = (MyEnum)Enum.Parse(typeof(MyEnum), "1");
   if((int)myen1!=1) {
   iCountErrors++;
   Console.WriteLine("Err_87trsg! Expected value wasn't returned");
   }
   strLoc="Loc_82475sdg";
   iCountTestcases++;
   myf1 = (MyEnumFlags)Enum.Parse(typeof(MyEnumFlags), "1");
   if((int)myf1!=1) {
   iCountErrors++;
   Console.WriteLine("Err_294357tsd! Expected value wasn't returned");
   }
   strLoc="Loc_82475sdg";
   iCountTestcases++;
   myen1 = (MyEnum)Enum.Parse(typeof(MyEnum), "500");
   if((int)myen1!=500) {
   iCountErrors++;
   Console.WriteLine("Err_348ytsg! Expected value wasn't returned");
   }
   strLoc="Loc_39567tfgs";
   iCountTestcases++;
   myen1 = (MyEnum)Enum.Parse(typeof(MyEnum), "ONE, TWO");
   if((int)myen1!=3) {
   iCountErrors++;
   Console.WriteLine("Err_3497sdg! Expected value wasn't returned, " + myen1);
   }
   iCountTestcases++;
   myen1 = (MyEnum)Enum.Parse(typeof(MyEnum), "ONE, TWO, THREE");
   if((int)myen1!=3) {
   iCountErrors++;
   Console.WriteLine("Err_3497sdg! Expected value wasn't returned, " + myen1);
   }
   try {
   iCountTestcases++;
   Enum.Parse(typeof(MyEnumLong), "FOUR");
   iCountErrors++;
   Console.WriteLine("Err_47cdw! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_53598cd! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Parse(typeof(Int32), "FOUR");
   iCountErrors++;
   Console.WriteLine("Err_025dswom! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0556cfd! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Parse(null, "FOUR");
   iCountErrors++;
   Console.WriteLine("Err_046289dfs! expected exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0157fs! unexpected exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Parse(typeof(MyEnumLong), null);
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
   Co3879Parse_tp_str oCbTest = new Co3879Parse_tp_str();
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
  public enum MyEnumFlags
{
  ONE	= 1,
  TWO	=	2,
  FOUR	= 4,
  EIGHT	= 8,
}
