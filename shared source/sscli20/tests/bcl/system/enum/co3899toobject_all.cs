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
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co3899ToObject_All : IDescribeTestedMethods
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.ToOBject()";
 public static String s_strTFName        = "Co3899ToObject_All.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Enum);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(Byte)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(SByte)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(Int16)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(Int32)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(Int64)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(UInt16)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(UInt32)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(UInt64)}));
   list.Add(type.GetMethod("ToObject", new Type[] {typeof(Type), typeof(Object)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
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
   MyEnumByte myn3;
   MyEnumByte myn4;
   MyEnumSByte myn5;
   MyEnumSByte myn6;
   MyEnumShort myn7;
   MyEnumShort myn8;
   MyEnumLong myn9;
   MyEnumLong myn10;
   MyEnumUShort myn11;
   MyEnumUShort myn12;
   MyEnumUInt  myn13;
   MyEnumUInt  myn14;
   MyEnumULong  myn15;
   MyEnumULong  myn16;
   MyEnum[] ArrMyEnum;
   MyEnumByte[] ArrMyEnumByte;
   MyEnumSByte[] ArrMyEnumSByte;
   MyEnumShort[] ArrMyEnumShort;
   MyEnumLong[] ArrMyEnumLong;
   MyEnumUShort[] ArrMyEnumUShort;
   MyEnumUInt[] ArrMyEnumUInt;
   MyEnumULong[] ArrMyEnumULong;
   Type tpValue;
   Byte btValue;
   SByte sbtValue;
   Int16 i16Value;
   Int32 i32Value;
   Int64 i64Value;
   UInt16 ui16Value;
   UInt32 ui32Value;
   UInt64 ui64Value;
   Object objValue;
   Decimal decValue;
   Random random = new Random();
   try {
   strLoc="Loc_642cdf";
   i32Value = 32;
   tpValue = typeof(MyEnum);
   myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn1)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, 32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn1.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn1.GetType()).Equals(typeof(Int32))) {
   iCountErrors++;
   Console.WriteLine("Err_452gbd! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn1.GetType()).Name + " " + typeof(Int32).Name);
   }					
   myn2 = MyEnum.ONE;
   iCountTestcases++;
   if(myn2.CompareTo(myn1)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_523ds! Expected value wasn't returned, " + myn2.CompareTo(myn1));
   }					
   ArrMyEnum = (MyEnum[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnum.Length!=5) {
   iCountErrors++;
   Console.WriteLine("Err_742vdf! Expected value wasn't returned, " + ArrMyEnum.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnum.Length;i++){
   myn2 = ArrMyEnum[i];
   if(myn2.CompareTo(myn1)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   i32Value = 1;
   tpValue = typeof(MyEnum);
   myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn1)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     i32Value = random.Next(Int32.MinValue, 0);
     tpValue = typeof(MyEnum);
     myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
     if(myn1.ToString()!=i32Value.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_3we497sg! Expected value wasn't returned");
       }
     }
   i32Value = Int32.MinValue;
   tpValue = typeof(MyEnum);
   myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
   if(myn1.ToString()!=i32Value.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_3we497sg! Expected value wasn't returned");
     }
   i32Value = Int32.MaxValue;
   tpValue = typeof(MyEnum);
   myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
   if(myn1.ToString()!=i32Value.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_3we497sg! Expected value wasn't returned");
     }
   iCountTestcases++;
   try{
   tpValue = typeof(Int32);
   i32Value = 5;
   myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnumULong);
   i32Value = 5;
   myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
   iCountErrors++;
   Console.WriteLine("Err_743rrff! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_207efg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   i32Value = 5;
   tpValue = null;
   myn1 = (MyEnum)Enum.ToObject(tpValue, i32Value);
   iCountErrors++;
   Console.WriteLine("Err_0743fg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0145wgds! Unexpected exception thrown, " + ex.GetType().Name);
   }
   strLoc="Loc_432cfdg";
   btValue = 32;
   tpValue = typeof(MyEnumByte);
   myn3 = (MyEnumByte)Enum.ToObject(tpValue, btValue);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn3)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, (byte)32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn3.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn3.GetType()).Equals(typeof(Byte))) {
   iCountErrors++;
   Console.WriteLine("Err_453vdfxcc! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn3.GetType()).Name + " " + typeof(Byte).Name);
   }					
   myn4 = MyEnumByte.ONE;
   iCountTestcases++;
   if(myn4.CompareTo(myn3)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_84320fd! Expected value wasn't returned, " + myn4.CompareTo(myn3));
   }					
   ArrMyEnumByte = (MyEnumByte[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnumByte.Length!=2) {
   iCountErrors++;
   Console.WriteLine("Err_853fd! Expected value wasn't returned, " + ArrMyEnumByte.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnumByte.Length;i++){
   myn4 = ArrMyEnumByte[i];
   if(myn4.CompareTo(myn3)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   btValue = 1;
   tpValue = typeof(MyEnumByte);
   myn3 = (MyEnumByte)Enum.ToObject(tpValue, btValue);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn3)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }						
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     btValue = (Byte)random.Next(100, Byte.MaxValue);
     tpValue = typeof(MyEnumByte);
     myn3 = (MyEnumByte)Enum.ToObject(tpValue, btValue);
     iCountTestcases++;
     if(myn3.ToString() != btValue.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
       }						
     }				
   btValue = Byte.MaxValue;
   tpValue = typeof(MyEnumByte);
   myn3 = (MyEnumByte)Enum.ToObject(tpValue, btValue);
   iCountTestcases++;
   if(myn3.ToString() != btValue.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
     }						
   iCountTestcases++;
   try{
   tpValue = typeof(String);
   btValue = 5;
   myn3 = (MyEnumByte)Enum.ToObject(tpValue, btValue);
   iCountErrors++;
   Console.WriteLine("Err_7563rg! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_107345dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnumULong);
   btValue = 5;
   myn3 = (MyEnumByte)Enum.ToObject(tpValue, btValue);
   iCountErrors++;
   Console.WriteLine("Err_07734d! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0175dg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   btValue = 5;
   tpValue = null;
   myn3 = (MyEnumByte)Enum.ToObject(tpValue, btValue);
   iCountErrors++;
   Console.WriteLine("Err_087653fsdg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_021735dsf! Unexpected exception thrown, " + ex.GetType().Name);
   }
   strLoc="Loc_74329fd";
   sbtValue = 32;
   tpValue = typeof(MyEnumSByte);
   myn5 = (MyEnumSByte)Enum.ToObject(tpValue, sbtValue);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn5)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, (SByte)32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn5.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn5.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_78432few! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn5.GetType()).Equals(typeof(SByte))) {
   iCountErrors++;
   Console.WriteLine("Err_873fgd! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn5.GetType()).Name + " " + typeof(Int32).Name);
   }					
   myn6 = MyEnumSByte.ONE;
   iCountTestcases++;
   if(myn6.CompareTo(myn5)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_297653vdsf! Expected value wasn't returned, " + myn6.CompareTo(myn5));
   }					
   ArrMyEnumSByte = (MyEnumSByte[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnumSByte.Length!=3) {
   iCountErrors++;
   Console.WriteLine("Err_45723fd! Expected value wasn't returned, " + ArrMyEnumSByte.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnumSByte.Length;i++){
   myn6 = (MyEnumSByte)ArrMyEnumSByte[i];
   if(myn6.CompareTo(myn5)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   sbtValue = 1;
   tpValue = typeof(MyEnumSByte);
   myn5 = (MyEnumSByte)Enum.ToObject(tpValue, sbtValue);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn5)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }						
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     sbtValue = (SByte)random.Next(SByte.MinValue, 0);
     tpValue = typeof(MyEnumSByte);
     myn5 = (MyEnumSByte)Enum.ToObject(tpValue, sbtValue);
     iCountTestcases++;
     if(myn5.ToString()!=sbtValue.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
       }						
     }
   sbtValue = SByte.MaxValue;
   tpValue = typeof(MyEnumSByte);
   myn5 = (MyEnumSByte)Enum.ToObject(tpValue, sbtValue);
   iCountTestcases++;
   if(myn5.ToString()!=sbtValue.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
     }						
   iCountTestcases++;
   try{
   tpValue = typeof(Int32);
   sbtValue = 5;
   myn5 = (MyEnumSByte)Enum.ToObject(tpValue, sbtValue);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnumULong);
   sbtValue = 5;
   myn5 = (MyEnumSByte)Enum.ToObject(tpValue, sbtValue);
   iCountErrors++;
   Console.WriteLine("Err_743rrff! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_207efg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   sbtValue = 5;
   tpValue = null;
   myn5 = (MyEnumSByte)Enum.ToObject(tpValue, sbtValue);
   iCountErrors++;
   Console.WriteLine("Err_0743fg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0145wgds! Unexpected exception thrown, " + ex.GetType().Name);
   }
   strLoc="Loc_74329fd";
   i16Value = 32;
   tpValue = typeof(MyEnumShort);
   myn7 = (MyEnumShort)Enum.ToObject(tpValue, i16Value);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn7)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, (Int16)32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn7.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn7.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_78432few! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn7.GetType()).Equals(typeof(Int16))) {
   iCountErrors++;
   Console.WriteLine("Err_234dsg! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn7.GetType()).Name + " " + typeof(Int32).Name);
   }					
   myn8 = MyEnumShort.ONE;
   iCountTestcases++;
   if(myn8.CompareTo(myn7)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_523ds! Expected value wasn't returned, " + myn8.CompareTo(myn7));
   }					
   ArrMyEnumShort = (MyEnumShort[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnumShort.Length!=2) {
   iCountErrors++;
   Console.WriteLine("Err_4524fds! Expected value wasn't returned, " + ArrMyEnumShort.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnumShort.Length;i++){
   myn8 = (MyEnumShort)ArrMyEnumShort[i];
   if(myn8.CompareTo(myn7)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   i16Value = 1;
   tpValue = typeof(MyEnumShort);
   myn7 = (MyEnumShort)Enum.ToObject(tpValue, i16Value);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn7)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }		
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     i16Value = (Int16)random.Next(Int16.MinValue, 0);
     tpValue = typeof(MyEnumShort);
     myn7 = (MyEnumShort)Enum.ToObject(tpValue, i16Value);
     iCountTestcases++;
     if(myn7.ToString()!=i16Value.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
       }		
     }
   i16Value = Int16.MaxValue;
   tpValue = typeof(MyEnumShort);
   myn7 = (MyEnumShort)Enum.ToObject(tpValue, i16Value);
   iCountTestcases++;
   if(myn7.ToString()!=i16Value.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
     }		
   iCountTestcases++;
   try{
   tpValue = typeof(Int32);
   i16Value = 5;
   myn7 = (MyEnumShort)Enum.ToObject(tpValue, i16Value);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnumULong);
   i16Value = 5;
   myn7 = (MyEnumShort)Enum.ToObject(tpValue, i16Value);
   iCountErrors++;
   Console.WriteLine("Err_743rrff! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_207efg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   i16Value = 5;
   tpValue = null;
   myn7 = (MyEnumShort)Enum.ToObject(tpValue, i16Value);
   iCountErrors++;
   Console.WriteLine("Err_0743fg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0145wgds! Unexpected exception thrown, " + ex.GetType().Name);
   }
   strLoc="Loc_7423rtw";
   i64Value = 32;
   tpValue = typeof(MyEnumLong);
   myn9 = (MyEnumLong)Enum.ToObject(tpValue, i64Value);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn9)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, (Int64)32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn9.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn9.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_78432few! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn9.GetType()).Equals(typeof(Int64))) {
   iCountErrors++;
   Console.WriteLine("Err_89798dfgsd! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn9.GetType()).Name + " " + typeof(Int32).Name);
   }					
   myn10 = MyEnumLong.ONE;
   iCountTestcases++;
   if(myn10.CompareTo(myn9)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_523ds! Expected value wasn't returned, " + myn10.CompareTo(myn9));
   }					
   ArrMyEnumLong = (MyEnumLong[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnumLong.Length!=3) {
   iCountErrors++;
   Console.WriteLine("Err_047523fvdf! Expected value wasn't returned, " + ArrMyEnumLong.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnumLong.Length;i++){
   myn10 = ArrMyEnumLong[i];
   if(myn10.CompareTo(myn9)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   i64Value = 1;
   tpValue = typeof(MyEnumLong);
   myn9 = (MyEnumLong)Enum.ToObject(tpValue, i64Value);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn9)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }	
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     i64Value = random.Next(Int32.MinValue, 0);
     tpValue = typeof(MyEnumLong);
     myn9 = (MyEnumLong)Enum.ToObject(tpValue, i64Value);
     iCountTestcases++;
     if(myn9.ToString()!=i64Value.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
       }	
     }			
   i64Value = Int64.MaxValue;
   tpValue = typeof(MyEnumLong);
   myn9 = (MyEnumLong)Enum.ToObject(tpValue, i64Value);
   iCountTestcases++;
   if(myn9.ToString()!=i64Value.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
     }	
   iCountTestcases++;
   try{
   tpValue = typeof(Int32);
   i64Value = 5;
   myn9 = (MyEnumLong)Enum.ToObject(tpValue, i64Value);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnumULong);
   i64Value = 5;
   myn9 = (MyEnumLong)Enum.ToObject(tpValue, i64Value);
   iCountErrors++;
   Console.WriteLine("Err_743rrff! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_207efg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   i64Value = 5;
   tpValue = null;
   myn9 = (MyEnumLong)Enum.ToObject(tpValue, i64Value);
   iCountErrors++;
   Console.WriteLine("Err_0743fg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0145wgds! Unexpected exception thrown, " + ex.GetType().Name);
   }							
   strLoc="Loc_642cdf";
   ui16Value = 32;
   tpValue = typeof(MyEnumUShort);
   myn11 = (MyEnumUShort)Enum.ToObject(tpValue, ui16Value);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn11)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, (UInt16)32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn11.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn11.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_78432few! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn11.GetType()).Equals(typeof(UInt16))) {
   iCountErrors++;
   Console.WriteLine("Err_1324sdv! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn11.GetType()).Name + " " + typeof(Int32).Name);
   }					
   myn12 = MyEnumUShort.ONE;
   iCountTestcases++;
   if(myn12.CompareTo(myn11)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_523ds! Expected value wasn't returned, " + myn12.CompareTo(myn11));
   }					
   ArrMyEnumUShort = (MyEnumUShort[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnumUShort.Length!=3) {
   iCountErrors++;
   Console.WriteLine("Err_134ef! Expected value wasn't returned, " + ArrMyEnumUShort.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnumUShort.Length;i++){
   myn12 = ArrMyEnumUShort[i];
   if(myn12.CompareTo(myn11)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   ui16Value = 1;
   tpValue = typeof(MyEnumUShort);
   myn11 = (MyEnumUShort)Enum.ToObject(tpValue, ui16Value);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn11)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     ui16Value = (UInt16)random.Next(200, UInt16.MaxValue);
     tpValue = typeof(MyEnumUShort);
     myn11 = (MyEnumUShort)Enum.ToObject(tpValue, ui16Value);
     iCountTestcases++;
     if(myn11.ToString()!=ui16Value.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
       }
     }			
   ui16Value = UInt16.MaxValue;
   tpValue = typeof(MyEnumUShort);
   myn11 = (MyEnumUShort)Enum.ToObject(tpValue, ui16Value);
   iCountTestcases++;
   if(myn11.ToString()!=ui16Value.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
     }
   iCountTestcases++;
   try{
   tpValue = typeof(Int32);
   ui16Value = 5;
   myn11 = (MyEnumUShort)Enum.ToObject(tpValue, ui16Value);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnumULong);
   ui16Value = 5;
   myn11 = (MyEnumUShort)Enum.ToObject(tpValue, ui16Value);
   iCountErrors++;
   Console.WriteLine("Err_743rrff! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_207efg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   ui16Value = 5;
   tpValue = null;
   myn11 = (MyEnumUShort)Enum.ToObject(tpValue, ui16Value);
   iCountErrors++;
   Console.WriteLine("Err_0743fg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0145wgds! Unexpected exception thrown, " + ex.GetType().Name);
   }
   strLoc="Loc_7439dff";
   ui32Value = 32;
   tpValue = typeof(MyEnumUInt);
   myn13 = (MyEnumUInt)Enum.ToObject(tpValue, ui32Value);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn13)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, (UInt32)32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn13.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn13.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_78432few! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn13.GetType()).Equals(typeof(UInt32))) {
   iCountErrors++;
   Console.WriteLine("Err_1213dfs! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn13.GetType()).Name + " " + typeof(Int32).Name);
   }					
   myn14 = MyEnumUInt.ONE;
   iCountTestcases++;
   if(myn14.CompareTo(myn13)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_523ds! Expected value wasn't returned, " + myn14.CompareTo(myn13));
   }					
   ArrMyEnumUInt = (MyEnumUInt[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnumUInt.Length!=3) {
   iCountErrors++;
   Console.WriteLine("Err_29763fdbv! Expected value wasn't returned, " + ArrMyEnumUInt.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnumUInt.Length;i++){
   myn14 = ArrMyEnumUInt[i];
   if(myn14.CompareTo(myn13)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   ui32Value = 1;
   tpValue = typeof(MyEnumUInt);
   myn13 = (MyEnumUInt)Enum.ToObject(tpValue, ui32Value);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn13)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     ui32Value = (uint)random.Next(100, Int32.MaxValue);
     tpValue = typeof(MyEnumUInt);
     myn13 = (MyEnumUInt)Enum.ToObject(tpValue, ui32Value);
     if(myn13.ToString()!=ui32Value.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_3we497sg! Expected value wasn't returned");
       }
     }
   ui32Value = UInt32.MaxValue;
   tpValue = typeof(MyEnumUInt);
   myn13 = (MyEnumUInt)Enum.ToObject(tpValue, ui32Value);
   if(myn13.ToString()!=ui32Value.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_3we497sg! Expected value wasn't returned");
     }
   iCountTestcases++;
   try{
   tpValue = typeof(Int32);
   ui32Value = 5;
   myn13 = (MyEnumUInt)Enum.ToObject(tpValue, ui32Value);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnumULong);
   ui32Value = 5;
   myn13 = (MyEnumUInt)Enum.ToObject(tpValue, ui32Value);
   iCountErrors++;
   Console.WriteLine("Err_743rrff! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_207efg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   ui32Value = 5;
   tpValue = null;
   myn13 = (MyEnumUInt)Enum.ToObject(tpValue, ui32Value);
   iCountErrors++;
   Console.WriteLine("Err_0743fg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0145wgds! Unexpected exception thrown, " + ex.GetType().Name);
   }
   strLoc="Loc_7520s";
   ui64Value = 32;
   tpValue = typeof(MyEnumULong);
   myn15 = (MyEnumULong)Enum.ToObject(tpValue, ui64Value);
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, myn15)) {
   iCountErrors++;
   Console.WriteLine("Err_6438ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(Enum.IsDefined(tpValue, (ulong)32)) {
   iCountErrors++;
   Console.WriteLine("Err_5372ds! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!myn15.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }						
   strLoc="Loc_4732vdf";
   iCountTestcases++;
   if(!myn15.ToString().Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_78432few! Expected value wasn't returned");
   }						
   iCountTestcases++;
   if(!Enum.GetUnderlyingType(myn15.GetType()).Equals(typeof(UInt64))) {
   iCountErrors++;
   Console.WriteLine("Err_473fdg! Expected value wasn't returned, " + Enum.GetUnderlyingType(myn15.GetType()).Name + " " + typeof(Int32).Name);
   }					
   myn16 = MyEnumULong.ONE;
   iCountTestcases++;
   if(myn16.CompareTo(myn15)!=-1) {
   iCountErrors++;
   Console.WriteLine("Err_523ds! Expected value wasn't returned, " + myn16.CompareTo(myn15));
   }					
   ArrMyEnumULong = (MyEnumULong[])Enum.GetValues(tpValue);
   iCountTestcases++;
   if(ArrMyEnumULong.Length!=3) {
   iCountErrors++;
   Console.WriteLine("Err_10742vff! Expected value wasn't returned, " + ArrMyEnumULong.Length);
   }					
   iCountTestcases++;
   for(int i=0;i<ArrMyEnumULong.Length;i++){
   myn16 = ArrMyEnumULong[i];
   if(myn16.CompareTo(myn15)==0){
   iCountErrors++;
   Console.WriteLine("Err_743fdf! Found the newly created enum in the array");
   break;
   }
   }
   ui64Value = 1;
   tpValue = typeof(MyEnumULong);
   myn15 = (MyEnumULong)Enum.ToObject(tpValue, ui64Value);
   iCountTestcases++;
   if(!Enum.IsDefined(tpValue, myn15)) {
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
   }						
   iCountTestcases++;
   for(int i=0; i<20; i++)
     {
     ui64Value = (UInt64)random.Next(200, Int32.MaxValue);
     tpValue = typeof(MyEnumULong);
     myn15 = (MyEnumULong)Enum.ToObject(tpValue, ui64Value);
     iCountTestcases++;
     if(myn15.ToString()!=ui64Value.ToString()) 
       {
       iCountErrors++;
       Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
       }						
     }
   ui64Value = UInt64.MaxValue;
   tpValue = typeof(MyEnumULong);
   myn15 = (MyEnumULong)Enum.ToObject(tpValue, ui64Value);
   iCountTestcases++;
   if(myn15.ToString()!=ui64Value.ToString()) 
     {
     iCountErrors++;
     Console.WriteLine("Err_7432dfs! Expected value wasn't returned");
     }						
   iCountTestcases++;
   try{
   tpValue = typeof(Int32);
   ui64Value = 5;
   myn15 = (MyEnumULong)Enum.ToObject(tpValue, ui64Value);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   tpValue = typeof(MyEnum);
   ui64Value = 5;
   myn15 = (MyEnumULong)Enum.ToObject(tpValue, ui64Value);
   iCountErrors++;
   Console.WriteLine("Err_743rrff! Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_207efg! Unexpected exception thrown, " + ex.GetType().Name);
   }
   iCountTestcases++;
   try{
   ui64Value = 5;
   tpValue = null;
   myn15 = (MyEnumULong)Enum.ToObject(tpValue, ui64Value);
   iCountErrors++;
   Console.WriteLine("Err_0743fg! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_0145wgds! Unexpected exception thrown, " + ex.GetType().Name);
   }
   strLoc="Loc_8756sgf";
   iCountTestcases++;
   i32Value = 64;
   objValue = i32Value;
   tpValue = typeof(MyEnum);
   myn1 = (MyEnum)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn1.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_5834fvd! Expected value wasn't returned");
   }
   btValue = 64;
   objValue = btValue;
   tpValue = typeof(MyEnumByte);
   myn3 = (MyEnumByte)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn3.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_346sg! Expected value wasn't returned");
   }
   sbtValue = 64;
   objValue = sbtValue;
   tpValue = typeof(MyEnumSByte);
   myn5 = (MyEnumSByte)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn5.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_346sg! Expected value wasn't returned");
   }
   i16Value = 64;
   objValue = i16Value;
   tpValue = typeof(MyEnumShort);
   myn7 = (MyEnumShort)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn7.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_346sg! Expected value wasn't returned");
   }
   i64Value = 64;
   objValue = i64Value;
   tpValue = typeof(MyEnumLong);
   myn9 = (MyEnumLong)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn9.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_346sg! Expected value wasn't returned");
   }
   ui16Value = 64;
   objValue = ui16Value;
   tpValue = typeof(MyEnumUShort);
   myn11 = (MyEnumUShort)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn11.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_346sg! Expected value wasn't returned");
   }
   ui32Value = 64;
   objValue = ui32Value;
   tpValue = typeof(MyEnumUInt);
   myn13 = (MyEnumUInt)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn13.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_346sg! Expected value wasn't returned");
   }
   ui64Value = 64;
   objValue = ui64Value;
   tpValue = typeof(MyEnumULong);
   myn15 = (MyEnumULong)Enum.ToObject(tpValue, objValue);
   iCountTestcases++;
   if(!myn15.ToString().Equals("64")) {
   iCountErrors++;
   Console.WriteLine("Err_346sg! Expected value wasn't returned");
   }
   strLoc="Loc_0734rsg";
   iCountTestcases++;
   try{
   objValue = null;
   myn1 = (MyEnum)Enum.ToObject(typeof(MyEnum), objValue);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   i32Value = 64;
   objValue = i32Value;
   try{
   tpValue = null;
   myn1 = (MyEnum)Enum.ToObject(tpValue, objValue);
   iCountErrors++;
   Console.WriteLine("Err_7543fgs! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_7432dfs! Unexpected exception thrown, " + ex.GetType().Name);
   }
   decValue = 64;
   objValue = decValue;
   try{
   Enum.ToObject(typeof(MyEnum), objValue);
   iCountErrors++;
   Console.WriteLine("Err_97235! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_83745sgd! Unexpected exception thrown, " + ex.GetType().Name);
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
   Co3899ToObject_All oCbTest = new Co3899ToObject_All();
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
public enum MyEnumSByte:sbyte
{
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
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
public enum MyEnumUShort:ushort
{
 ONE	= (ushort)1,
   TWO	=	(ushort)2,
   THREE	= (ushort)3
   }
public enum MyEnumUInt:uint
{
 ONE	= (uint)1,
   TWO	=	(uint)2,
   THREE	= (uint)3
   }
public enum MyEnumULong:ulong
{
 ONE	= (ulong)1,
   TWO	=	(ulong)2,
   THREE	= (ulong)3
   }
