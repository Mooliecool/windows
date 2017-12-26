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
public class Co3873ToString
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "ValueType.ctor()";
 public static String s_strTFName        = "Co3873ToString.cs";
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
   ValueType vt1;
   MyStruct1 mystr1;
   MyStruct2 mystr2;
   Int64 lVal;
   try {
   mystr1 = new MyStruct1(5);								
   iCountTestcases++;
   if(!mystr1.ToString().Equals("MyStruct1")) {
   iCountErrors++;
   Console.WriteLine("Err_53vd! ValueType change detected, please take a look");
   }									
   vt1 = mystr1;
   iCountTestcases++;
   if(!vt1.ToString().Equals("MyStruct1")) {
   iCountErrors++;
   Console.WriteLine("Err_8536cd! ValueType change detected, please take a look");
   }			
   lVal = 50;
   mystr2 = new MyStruct2(lVal);
   iCountTestcases++;
   if(!mystr2.ToString().Equals("50")) {
   iCountErrors++;
   Console.WriteLine("Err_210cdp! ValueType change detected, please take a look, " + mystr2);
   }			
   vt1 = mystr2;
   iCountTestcases++;
   if(!vt1.ToString().Equals("50")) {
   iCountErrors++;
   Console.WriteLine("Err_93246cds! ValueType change detected, please take a look, " + vt1);
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
   Co3873ToString oCbTest = new Co3873ToString();
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
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }	
}
public struct MyStruct1
{
 internal Int32 iValue;
 public MyStruct1(Int32 i)
   {
   iValue = i;
   }	
}
public struct MyStruct2
{
 internal Int64 lvalue;
 public MyStruct2(Int64 l)
   {
   lvalue = l;
   }
 public override String ToString()
   {
   return lvalue.ToString();
   }
}
