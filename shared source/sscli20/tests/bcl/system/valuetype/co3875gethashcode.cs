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
public class Co3874Equals
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "ValueType.ctor()";
 public static String s_strTFName        = "Co3874Equals.cs";
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
   MyStruct3 mystr3;
   MyStruct4 mystr4;
   MyStruct5 mystr5;
   MyStruct6 mystr6;
   MyStruct7 mystr7;
   Int32 iValue;
   try {
   iValue = 5;
   mystr1 = new MyStruct1(5);											
   iCountTestcases++;
   if(mystr1.GetHashCode() == 0) {
   iCountErrors++;
   Console.WriteLine("Err_53vd! ValueType change detected, please take a look, " + mystr1.GetHashCode());
   }									
   vt1 = mystr1;
   iCountTestcases++;
   if(vt1.GetHashCode() != mystr1.GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_8536cd! ValueType change detected, please take a look, " + mystr1.GetHashCode());
   }			
   iValue = 5;
   mystr2 = new MyStruct2(5);			
   iCountTestcases++;
   if(mystr2.GetHashCode() != iValue*2) {
   iCountErrors++;
   Console.WriteLine("Err_539fvd! ValueType change detected, please take a look, " + mystr1.GetHashCode());
   }									
   vt1 = mystr2;
   iCountTestcases++;
   if(vt1.GetHashCode() != iValue*2) {
   iCountErrors++;
   Console.WriteLine("Err_0565cs! ValueType change detected, please take a look, " + mystr1.GetHashCode());
   }			
   mystr3 = new MyStruct3();
   iCountTestcases++;
   if(mystr3.GetHashCode() == 0) {
   iCountErrors++;
   Console.WriteLine("Err_6553cs! ValueType change detected, please take a look, " + mystr3.GetHashCode());
   }									
   vt1 = mystr3;
   iCountTestcases++;
   if(vt1.GetHashCode() != mystr3.GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_014xs! ValueType change detected, please take a look, " + vt1.GetHashCode());
   }			
   mystr4 = new MyStruct4(10);
   iCountTestcases++;
   if(mystr4.GetHashCode() == 0) {	
   iCountErrors++;
   Console.WriteLine("Err_5538cd! ValueType change detected, please take a look, " + mystr1.GetHashCode());
   }									
   vt1 = mystr4;
   iCountTestcases++;
   if(vt1.GetHashCode() != mystr4.GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_510apm! ValueType change detected, please take a look, " + mystr1.GetHashCode());
   }			
   mystr5 = new MyStruct5();
   iCountTestcases++;
   if(mystr5.GetHashCode() == 0) {	
   iCountErrors++;
   Console.WriteLine("Err_295msn! ValueType change detected, please take a look, " + mystr5.GetHashCode());
   }
   vt1 = mystr5;
   iCountTestcases++;
   if(vt1.GetHashCode() != mystr5.GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_0432emc! ValueType change detected, please take a look, " + vt1.GetHashCode());
   }	
   mystr5 = new MyStruct5(10);
   iCountTestcases++;
   if(mystr5.GetHashCode() == 0) {	
   iCountErrors++;
   Console.WriteLine("Err_54238ds! ValueType change detected, please take a look, " + mystr5.GetHashCode());
   }
   vt1 = mystr5;
   iCountTestcases++;
   if(vt1.GetHashCode() != mystr5.GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_6530vf! ValueType change detected, please take a look, " + vt1.GetHashCode());
   }	
   mystr6 = new MyStruct6();
   iCountTestcases++;
   if(mystr6.GetHashCode() == 0) {
   iCountErrors++;
   Console.WriteLine("Err_753fsd! ValueType change detected, please take a look, " + mystr5.GetHashCode());
   }
   vt1 = mystr6;
   iCountTestcases++;
   if(vt1.GetHashCode() != mystr6.GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_7634vcd! ValueType change detected, please take a look, " + vt1.GetHashCode());
   }	
   mystr7 = new MyStruct7(5);
   iCountTestcases++;
   if(mystr7.GetHashCode() == 0) {	
   iCountErrors++;
   Console.WriteLine("Err_653cd! ValueType change detected, please take a look, " + mystr7.GetHashCode() + " " + typeof(MyStruct7).GetHashCode());
   }
   vt1 = mystr7;
   iCountTestcases++;
   if(vt1.GetHashCode() != mystr7.GetHashCode()) {
   iCountErrors++;
   Console.WriteLine("Err_6537dsfc ValueType change detected, please take a look, " + vt1.GetHashCode());
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
   Co3874Equals oCbTest = new Co3874Equals();
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
 internal Int32 lvalue;
 public MyStruct2(Int32 l)
   {
   lvalue = l;
   }
 public override String ToString()
   {
   return lvalue.ToString();
   }
 public override Boolean Equals(Object obj)
   {
   if (!(obj is MyStruct2)) {
   return false;
   }
   return lvalue == ((MyStruct2)obj).lvalue;	
   }
 public override Int32 GetHashCode()
   {
   return lvalue*2;
   }
}
public struct MyStruct3
{
}
public struct MyStruct4
{
 internal static Int32 stValue;
 public MyStruct4(Int32 iValue)
   {
   stValue = iValue;
   }
}
public struct MyStruct5
{
 internal static Int32 iValue;
 internal Int32 iFirst;
 internal Int32 iSec;
 public MyStruct5(Int32 i)
   {
   iValue = i;
   iFirst = i + 1;
   iSec = i + 2;
   }
}
public struct MyStruct6
{
 internal Int32[] iArrFirst;		
}
public struct MyStruct7
{
 internal Int32[] iArrFirst;
 public MyStruct7(Int32 i)
   {
   iArrFirst = new Int32[i];
   for(int j=0; j<iArrFirst.Length; j++){
   iArrFirst[j] = j + i * j + i + 1000;
   }
   }
}
