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
using System.Runtime.CompilerServices;
public class Co8826BoxedObjectCheck
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Boxed value types security hole";
 public static String s_strTFName        = "Co8826BoxedObjectCheck.cs";
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
   object [] tests = new object[] {
     true, false,
     Byte.MinValue,   Byte.MaxValue, (Byte)0,
     SByte.MinValue,  SByte.MaxValue, (SByte)0,
     Int16.MinValue,  Int16.MaxValue, (Int16)0,
     UInt16.MinValue, UInt16.MaxValue, (UInt16)0,
     Int32.MinValue,  Int32.MaxValue, (Int32)0,
     UInt32.MinValue, UInt32.MaxValue, (UInt32)0,
     Int64.MinValue,  Int64.MaxValue, (Int64)0,
     UInt64.MinValue, UInt64.MaxValue, (UInt64)0,
     Char.MinValue,   Char.MaxValue, (Char)0,
     Double.MinValue, Double.MaxValue, (Double)0,
     Single.MinValue, Single.MaxValue, (Single)0,
     DateTime.MinValue, DateTime.Now, DateTime.MaxValue};
   try {
   iCountTestcases++;	
   strLoc="Loc_642cdf";
   iCountTestcases++;	
   foreach(Object o in tests){								
   if((ValueTypeSafety.GetSafeObject(o)==o)){
   iCountTestcases++;
   Console.WriteLine("Err_9345sgd! Wrong value returned");
   }
   }
   strLoc="Loc_642cdf";
   iCountTestcases++;	
   foreach(Object o in tests){								
   if(!(ValueTypeSafety.GetSafeObject(o).Equals(o))){
   iCountTestcases++;
   Console.WriteLine("Err_93427rsg! Wrong value returned");
   }
   }
   strLoc="Loc_642cdf";
   iCountTestcases++;	
   foreach(Object o in tests){								
   if(!(o.Equals(ValueTypeSafety.GetSafeObject(o)))){
   iCountTestcases++;
   Console.WriteLine("Err_93427rsg! Wrong value returned");
   }
   }
   strLoc="Loc_642cdf";
   iCountTestcases++;	
   foreach(Object o in tests){								
   if(!(Object.Equals(ValueTypeSafety.GetSafeObject(o), o))){
   iCountTestcases++;
   Console.WriteLine("Err_93427rsg! Wrong value returned");
   }
   }
   strLoc="Loc_642cdf";
   iCountTestcases++;	
   foreach(Object o in tests){								
   if((Object.ReferenceEquals(ValueTypeSafety.GetSafeObject(o), o))){
   iCountTestcases++;
   Console.WriteLine("Err_93427rsg! Wrong value returned");
   }
   }
   Object o1 = E.ONE;
   Console.WriteLine(Object.ReferenceEquals(ValueTypeSafety.GetSafeObject(o1), o1));
   Console.WriteLine((ValueTypeSafety.GetSafeObject(o1) == o1));
   Console.WriteLine(o1.GetType().IsPrimitive);
   IConvertible icon = (IConvertible)o1;
   Object o2 = icon.ToType(typeof(object), null);
   Console.WriteLine(Object.ReferenceEquals(ValueTypeSafety.GetSafeObject(o2), o2));
   Console.WriteLine((ValueTypeSafety.GetSafeObject(o2) == o2));
   strLoc="Loc_642cdf";
   iCountTestcases++;	
   Int32 count=0;
   foreach(Type type in typeof(String).Module.GetTypes()){
   if(type.IsValueType && !type.IsEnum){
   try{
   Object oo1 = Activator.CreateInstance(type) as IConvertible;
   if((oo1)!=null){
   Object oo2 = ((IConvertible)oo1).ToType(typeof(object), null);
   count++;
   if(Object.ReferenceEquals(oo2, oo1)){
   iCountTestcases++;
   Console.WriteLine("Err_92475sg! Unexpected value returned, {0}", type);
   }
   }
   }catch{
   }
   }
   }
   Console.WriteLine("Checked {0} Types, ", count);
   }catch (Exception exc_general){
   ++iCountErrors;
   Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
   }
   if ( iCountErrors == 0 ){
   Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
   return true;
   } else {
   Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +""+ s_strActiveBugNums );
   return false;
   }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co8826BoxedObjectCheck oCbTest = new Co8826BoxedObjectCheck();
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
class ValueTypeSafety
{
 public static object GetSafeObject(object theValue)
   {
   if(null == theValue)
     return null;
   else if(theValue.GetType().IsPrimitive)
     return ((IConvertible)theValue).ToType(typeof(object), null);
   else
     return RuntimeHelpers.GetObjectValue(theValue);
   }
}
enum E
{
  ONE=1,
}
