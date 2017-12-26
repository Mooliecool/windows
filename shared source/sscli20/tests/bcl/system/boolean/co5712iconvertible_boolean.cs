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
using System.Text;
using System.Reflection;
interface IDescribeTestedMethods
{
 MethodInfo[] GetTestedMethods();
}
public class Co5712IConvertible_Boolean : IDescribeTestedMethods
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Iconvertible";
 public static String s_strTFName        = "Co5712IConvertible_Boolean.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 String strLoc = "Loc_000oo";
 String strValue = String.Empty;
 int iCountErrors = 0;
 int iCountTestcases = 0;
 public MethodInfo[] GetTestedMethods()
   {
   Type typeTarget = typeof(Boolean);
   Type typeOrigin = typeof(IConvertible);
   ArrayList list = new ArrayList();
   MethodInfo[] mis = typeOrigin.GetMethods();
   for(int i = 0 ; i < mis.Length ; i++) {
   if(mis[i].Name.Equals("ToType")) 
     list.Add(typeTarget.GetMethod(mis[i].Name, new Type[]{typeof(Type)}));
   else					
     list.Add(typeTarget.GetMethod(mis[i].Name, new Type[]{}));
   }
   mis = new MethodInfo[list.Count];		
   list.CopyTo(mis, 0);
   return mis;		
   }
 public Boolean TestToBoolean(Boolean testSubject){
 strLoc = "Loc_498yv";
 Boolean b1 = false;
 Boolean b2 = false;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToBoolean(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToBoolean(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }
 public Boolean TestToChar(Boolean testSubject) {
 strLoc = "498vy3";
 Char b1 = '\0';
 Char b2 = '\0';
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToChar(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToChar(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }
 public Boolean TestToSByte(Boolean testSubject) {
 strLoc = "r4987";
 SByte b1 = 0;
 SByte b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToSByte(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToSByte(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToByte(Boolean testSubject) {
 strLoc = "099d8";
 Byte b1 = 0;
 Byte b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToByte(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToByte(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToInt16(Boolean testSubject) {
 strLoc = "099d8";
 Int16 b1 = 0;
 Int16 b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToInt16(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToInt16(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToUInt16(Boolean testSubject) {
 strLoc = "099d8";
 UInt16 b1 = 0;
 UInt16 b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToUInt16(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToUInt16(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToInt32(Boolean testSubject) {
 strLoc = "099d8";
 Int32 b1 = 0;
 Int32 b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToInt32(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToInt32(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToUInt32(Boolean testSubject) {
 strLoc = "099d8";
 UInt32 b1 = 0;
 UInt32 b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToUInt32(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToUInt32(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToInt64(Boolean testSubject) {
 strLoc = "099d8";
 Int64 b1 = 0;
 Int64 b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToInt64(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToInt64(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToUInt64(Boolean testSubject) {
 strLoc = "099d8";
 UInt64 b1 = 0;
 UInt64 b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToUInt64(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToUInt64(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToSingle(Boolean testSubject) {
 strLoc = "099d8";
 Single b1 = 0;
 Single b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToSingle(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToSingle(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToDouble(Boolean testSubject) {
 strLoc = "099d8";
 Double b1 = 0;
 Double b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToDouble(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToDouble(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToDecimal(Boolean testSubject) {
 strLoc = "099d8";
 Decimal b1 = 0;
 Decimal b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToDecimal(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToDecimal(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1 == b2)
   return true;
 else
   return false;
 }	
 public Boolean TestToDateTime(Boolean testSubject) {
 strLoc = "099d8";
 DateTime b1 = DateTime.Now;
 DateTime b2 = DateTime.Now;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToDateTime(null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToDateTime(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1.Equals(b2))
   return true;
 else
   return false;
 }	
 public Boolean TestToString(Boolean testSubject) {
 strLoc = "099d8";
 String b1 = "";
 String b2 = "";
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = testSubject.ToString();
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ToString(testSubject);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else
     return false;
 else if(pass && b1.Equals(b2))
   return true;
 else
   return false;
 }	
 public Boolean TestToType(Boolean testSubject, Type typecode) {
 strLoc = "099d8";
 Object b1 = 0;
 Object b2 = 0;
 Boolean pass = false;
 Boolean excthrown = false;
 Object exc1 = null;
 Object exc2 = null;
 try {
 b1 = ((IConvertible)testSubject).ToType(typecode, null);
 pass = true;
 excthrown = false;
 } catch (Exception exc) {
 exc1 = exc;
 pass = false;
 excthrown = true;
 }
 try {
 b2 = Convert.ChangeType(testSubject, typecode);
 pass = pass & true;
 } catch (Exception exc) {
 exc2 = exc;
 pass = false;
 excthrown = excthrown & true;
 }
 if(excthrown)
   if(exc1.GetType() == exc2.GetType())
     return true;
   else {
   Console.WriteLine("Exception1: "+exc1);
   Console.WriteLine("Exception2: "+exc2);					
   return false;
   }
 else if(pass && b1.Equals(b2))
   return true;
 else {
 Console.WriteLine("b1=="+b1.ToString()+" , b2=="+b2.ToString());
 return false;
 }
 }	
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   try
     {
     Boolean testSubject = false;
     Boolean[] testSubjects = new Boolean[]{false, true};
     Type[] typecodes = { 
       typeof(Object),         
       typeof(DBNull),         
       typeof(Boolean),        
       typeof(Char),           
       typeof(SByte),          
       typeof(Byte),           
       typeof(Int16),          
       typeof(UInt16),         
       typeof(Int32),          
       typeof(UInt32),        
       typeof(Int64),         
       typeof(UInt64),        
       typeof(Single),        
       typeof(Double),        
       typeof(Decimal),       
       typeof(DateTime),      
       typeof(String)        
     };
     strLoc = "Loc_2y8f8";
     testSubject = false;
     TypeCode t1 = testSubject.GetTypeCode();
     iCountTestcases++;
     if(t1 != TypeCode.Boolean) {
     iCountErrors++;
     printerr( "Error_298yv! Incorrect typecode returned");
     }		   			
     strLoc = "Loc_498yg";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToBoolean(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToBoolean failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49800";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToChar(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToChar failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49801";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToSByte(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToSByte failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49802";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToByte(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToByte failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49803";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToInt16(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToInt16 failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49804";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToUInt16(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToUInt16 failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49805";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToInt32(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToInt32 failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49806";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToUInt32(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToUInt32 failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49807";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToInt64(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToInt64 failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49808";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToUInt64(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToUInt64 failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49809";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToSingle(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToSingle failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49810";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToDouble(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToDouble failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49811";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToDecimal(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToDecimal failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49812";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToDateTime(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToDateTime failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49813";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     iCountTestcases++;
     if(!TestToString(testSubjects[i])) {
     iCountErrors++;
     printerr( "Error_248v8! ToString failed=="+testSubjects[i]);
     }
     }			   
     strLoc = "Loc_49814";
     for(int i = 0 ; i < testSubjects.Length ; i++) {
     for(int j = 0 ; j < typecodes.Length ; j++) {
     iCountTestcases++;
     if(!TestToType(testSubjects[i], typecodes[j])) {
     iCountErrors++;
     printerr( "Error_248v8! ToType failed=="+testSubjects[i]);
     }
     }
     }			   
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5712IConvertible_Boolean cbA = new Co5712IConvertible_Boolean();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
