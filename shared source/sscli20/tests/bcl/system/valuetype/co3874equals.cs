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
   MyStruct1 mystr11;
   MyStruct2 mystr2;
   MyStruct2 mystr22;
   MyStruct3 mystr3;
   MyStruct3 mystr33;
   MyStruct4 mystr4;
   MyStruct4 mystr44;
   MyStruct5 mystr5;
   MyStruct5 mystr55;
   MyStruct6 mystr6;
   MyStruct6 mystr66;
   MyStruct7 mystr7;
   MyStruct7 mystr77;
   Color enumCol1;
   Color enumCol2;
   object objValue1;
   object objValue2;
   try {
   mystr1 = new MyStruct1(5);								
   mystr11 = mystr1;
   iCountTestcases++;
   if(!mystr1.Equals(mystr11)) {
   iCountErrors++;
   Console.WriteLine("Err_53vd! ValueType change detected, please take a look");
   }									
   vt1 = mystr1;
   iCountTestcases++;
   if(!vt1.Equals(mystr1)) {
   iCountErrors++;
   Console.WriteLine("Err_8536cd! ValueType change detected, please take a look");
   }			
   objValue1 = mystr1;
   iCountTestcases++;
   if(!objValue1.Equals(mystr1)) {
   iCountErrors++;
   Console.WriteLine("Err_7563fdg! ValueType change detected, please take a look");
   }	
   mystr1 = new MyStruct1(5);
   mystr11 = new MyStruct1(5);
   iCountTestcases++;
   if(!mystr1.Equals(mystr11)) {
   iCountErrors++;
   Console.WriteLine("Err_175td! ValueType change detected, please take a look");
   }
   mystr1 = new MyStruct1(5);
   mystr2 = new MyStruct2(5);
   iCountTestcases++;
   if(mystr1.Equals(mystr2)) {
   iCountErrors++;
   Console.WriteLine("Err_62c8sb! ValueType change detected, please take a look");
   }												
   mystr2 = new MyStruct2(5);
   mystr22 = new MyStruct2(5);
   iCountTestcases++;
   if(!mystr2.Equals(mystr22)) {
   iCountErrors++;
   Console.WriteLine("Err_753cs! ValueType change detected, please take a look");
   }												
   mystr2 = new MyStruct2(5);
   mystr22 = new MyStruct2(5);
   vt1 = mystr22;
   iCountTestcases++;
   if(!mystr2.Equals(vt1)) {
   iCountErrors++;
   Console.WriteLine("Err_653cds! ValueType change detected, please take a look");
   }												
   mystr3 = new MyStruct3();
   mystr33 = new MyStruct3();
   iCountTestcases++;
   if(!mystr3.Equals(mystr33)) {
   iCountErrors++;
   Console.WriteLine("Err_105ds! ValueType change detected, please take a look");
   }												
   vt1 = mystr33;
   iCountTestcases++;
   if(!mystr3.Equals(vt1)) {
   iCountErrors++;
   Console.WriteLine("Err_01435xs! ValueType change detected, please take a look");
   }												
   mystr4 = new MyStruct4(5);
   mystr44 = new MyStruct4(15);
   iCountTestcases++;
   if(!mystr4.Equals(mystr44)) {
   iCountErrors++;
   Console.WriteLine("Err_642cd! ValueType change detected, please take a look");
   }												
   mystr44 = new MyStruct4(20);
   vt1 = mystr44;
   iCountTestcases++;
   if(!mystr4.Equals(vt1)) {
   iCountErrors++;
   Console.WriteLine("Err_5420csd! ValueType change detected, please take a look");
   }												
   mystr5 = new MyStruct5(5);
   mystr55 = new MyStruct5(5);
   iCountTestcases++;
   if(!mystr5.Equals(mystr55)) {
   iCountErrors++;
   Console.WriteLine("Err_519amg! ValueType change detected, please take a look");
   }												
   vt1 = mystr55;
   iCountTestcases++;
   if(!mystr5.Equals(vt1)) {
   iCountErrors++;
   Console.WriteLine("Err_17cdx ValueType change detected, please take a look");
   }												
   mystr5 = new MyStruct5(5);
   mystr55 = new MyStruct5(15);
   iCountTestcases++;
   if(mystr5.Equals(mystr55)) {
   iCountErrors++;
   Console.WriteLine("Err_105xcs! ValueType change detected, please take a look");
   }												
   mystr6 = new MyStruct6();
   mystr66 = new MyStruct6();
   iCountTestcases++;
   if(!mystr6.Equals(mystr66)) {
   iCountErrors++;
   Console.WriteLine("Err_0356cd! ValueType change detected, please take a look");
   }												
   vt1 = mystr66;
   iCountTestcases++;
   if(!mystr6.Equals(vt1)) {
   iCountErrors++;
   Console.WriteLine("Err_045ds! ValueType change detected, please take a look");
   }												
   mystr7 = new MyStruct7(5);
   mystr77 = new MyStruct7(5);
   iCountTestcases++;
   if(mystr7.Equals(mystr77)) {
   iCountErrors++;
   Console.WriteLine("Err_02345cd! ValueType change detected, please take a look");
   }												
   vt1 = mystr77;
   iCountTestcases++;
   if(mystr7.Equals(vt1)) {
   iCountErrors++;
   Console.WriteLine("Err_2106fcd ValueType change detected, please take a look");
   }												
   mystr7 = new MyStruct7(5);
   mystr77 = new MyStruct7(15);
   iCountTestcases++;
   if(mystr7.Equals(mystr77)) {
   iCountErrors++;
   Console.WriteLine("Err_0436cs! ValueType change detected, please take a look");
   }												
   enumCol1 = Color.Red;
   enumCol2 = Color.Red;
   iCountTestcases++;
   if(!enumCol1.Equals(enumCol2)) {
   iCountErrors++;
   Console.WriteLine("Err_0436cs! ValueType change detected, please take a look");
   }												
   objValue1 = enumCol1;
   objValue2 = enumCol2;
   iCountTestcases++;
   if(!objValue1.Equals(objValue2)) {
   iCountErrors++;
   Console.WriteLine("Err_0436cs! ValueType change detected, please take a look");
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
 public static Int32 GetStatValue()
   {
   return stValue;
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
   iArrFirst[j] = j;
   }
   }
}
enum Color {
  Red, Blue, Black
};
