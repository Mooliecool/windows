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
public class Co3898IConvertible_All : IDescribeTestedMethods
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.ToString()";
 public static String s_strTFName        = "Co3898IConvertible_All.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Enum);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToBoolean"));
   list.Add(type.GetMethod("ToByte"));
   list.Add(type.GetMethod("ToChar"));
   list.Add(type.GetMethod("ToDateTime"));
   list.Add(type.GetMethod("ToDecimal"));
   list.Add(type.GetMethod("ToDouble"));
   list.Add(type.GetMethod("ToInt16"));
   list.Add(type.GetMethod("ToInt32"));
   list.Add(type.GetMethod("ToInt64"));
   list.Add(type.GetMethod("ToUInt16"));
   list.Add(type.GetMethod("ToUInt32"));
   list.Add(type.GetMethod("ToUInt64"));
   list.Add(type.GetMethod("ToSByte"));
   list.Add(type.GetMethod("ToSingle"));
   list.Add(type.GetMethod("ToType", new Type[] {typeof(Type)}));
   list.Add(type.GetMethod("GetTypeCode"));
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
   MyEnumUShort myn2;
   MyEnumSByte myn3;
   MyEnumLongFlags myn4;
   MyEnum[] arrMyEnum;
   MyEnumUShort[] arrMyEnumUShort;
   MyEnumSByte[] arrMyEnumSByte;
   MyEnumLongFlags[] arrMyEnumLongFlags;
   Type tpValue;
   Byte btValue;
   SByte sbtValue;
   Int16 i16Value;
   Int32 i32Value;
   Int64 i64Value;
   UInt16 ui16Value;
   UInt32 ui32Value;
   UInt64 ui64Value;
   Object oValue;		
   Type[] arrTpValid = {typeof(Byte), typeof(SByte), typeof(Int16), typeof(Int32), typeof(Int64), typeof(UInt16), 
			typeof(UInt32), typeof(UInt64), typeof(Decimal), typeof(Double), typeof(Single), 
			typeof(Boolean), typeof(Char),
   };
   Type[] arrTpInValid = {typeof(DateTime), 
   };
   try {		
   arrMyEnum = (MyEnum[])Enum.GetValues(typeof(MyEnum));
   for(int i=0; i<arrMyEnum.Length; i++){
   strLoc="Loc_7432fd_" + i;
   myn1 = arrMyEnum[i];				
   strLoc="Loc_000oo_" + i;
   btValue = (byte)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToByte(null) != btValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToByte(null) + ", Expected, " + btValue);
   }	
   strLoc="Loc_001oo_" + i;
   sbtValue = (sbyte)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToSByte(null) != sbtValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToSByte(null) + ", Expected, " + sbtValue);
   }						
   strLoc="Loc_002oo_" + i;
   i16Value = (short)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToInt16(null) != i16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToInt16(null) + ", Expected, " + i16Value);
   }						
   i32Value = (int)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToInt32(null) != i32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToInt32(null) + ", Expected, " + i32Value);
   }						
   i64Value = (long)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToInt64(null) != i64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToInt64(null) + ", Expected, " + i64Value);
   }
   ui16Value = (ushort)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToUInt16(null) != ui16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToUInt16(null) + ", Expected, " + ui16Value);
   }						
   ui32Value = (uint)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToUInt32(null) != ui32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToUInt32(null) + ", Expected, " + ui32Value);
   }						
   ui64Value = (ulong)myn1;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToUInt64(null) != ui64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).ToUInt64(null) + ", Expected, " + ui64Value);
   }
   iCountTestcases++;
   if(!((IConvertible)(Enum)myn1).ToString(null).Equals(myn1.ToString())){
   iCountErrors++;
   Console.WriteLine("Err_275dgd_" + i + " wrong value returned, <<" + ((IConvertible)(Enum)myn1).ToString(null) + ">>, Expected, <<" + myn1.ToString() + ">>");
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToBoolean(null)!= Convert.ToBoolean((int)myn1)){
   iCountErrors++;
   Console.WriteLine("Err_523ds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_ercsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToChar(null)!= Convert.ToChar((int)myn1)){
   iCountErrors++;
   Console.WriteLine("Err_452fwr_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_2135df_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   ((IConvertible)(Enum)myn1).ToDateTime(null);
   iCountErrors++;
   Console.WriteLine("Err_ver_" + i + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_qw3421_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToDecimal(null)!= Convert.ToDecimal((int)myn1)){
   iCountErrors++;
   Console.WriteLine("Err_254vds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_190w34vs_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToDouble(null)!= Convert.ToDouble((int)myn1)){
   iCountErrors++;
   Console.WriteLine("Err_dsf32_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_78342fsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).ToSingle(null)!= Convert.ToSingle((int)myn1)){
   iCountErrors++;
   Console.WriteLine("Err_342dfs_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_dfeewe_" + i + " wrong Exception thrown, " + ex);
   }
   for(int j=0; j<arrTpValid.Length;j++){
   strLoc="Loc_5724gdf_" + i + "_" + j;
   oValue = ((IConvertible)(Enum)myn1).ToType(arrTpValid[j], null);
   iCountTestcases++;
   if(arrTpValid[j]==typeof(Boolean)){
   if(((Boolean)oValue) != Convert.ToBoolean((int)myn1)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn1).ToInt32(null));
   }
   }else{
   if(((IConvertible)oValue).ToInt32(null) != ((IConvertible)(Enum)myn1).ToInt32(null)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn1).ToInt32(null));
   }
   }
   }
   for(int j=0; j<arrTpInValid.Length;j++){
   strLoc="Loc_75234gd_" + i + "_" + j;
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn1).ToType(arrTpInValid[j], null);
   iCountErrors++;
   Console.WriteLine("Err_75424gdg_" + i + "_" + j + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_75694fd_" + i + "_" + j + " wrong Exception thrown, " + ex);
   }
   }				
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn1).ToType(myn1.GetType(), null);
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf_" + i  + " wrong Exception thrown, " + ex);
   }
   iCountTestcases++;
   if(((IConvertible)(Enum)myn1).GetTypeCode() != new Int32().GetTypeCode()){
   iCountErrors++;
   Console.WriteLine("Err_5732vdf_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn1).GetTypeCode() + ", Expected, " + new Int32().GetTypeCode());
   }					
   }	
   strLoc="Loc_84523fvd";
   tpValue = typeof(MyEnumUShort);
   arrMyEnumUShort = (MyEnumUShort[])Enum.GetValues(tpValue);
   for(int i=0; i<arrMyEnumUShort.Length; i++){
   strLoc="Loc_7439df_" + i;
   myn2 = arrMyEnumUShort[i];
   btValue = (byte)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToByte(null) != btValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToByte(null) + ", Expected, " + btValue);
   }						
   sbtValue = (sbyte)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToSByte(null) != sbtValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToSByte(null) + ", Expected, " + sbtValue);
   }						
   i16Value = (short)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToInt16(null) != i16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToInt16(null) + ", Expected, " + i16Value);
   }						
   i32Value = (int)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToInt32(null) != i32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToInt32(null) + ", Expected, " + i32Value);
   }						
   i64Value = (long)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToInt64(null) != i64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToInt64(null) + ", Expected, " + i64Value);
   }
   ui16Value = (ushort)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToUInt16(null) != ui16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToUInt16(null) + ", Expected, " + ui16Value);
   }						
   ui32Value = (uint)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToUInt32(null) != ui32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToUInt32(null) + ", Expected, " + ui32Value);
   }						
   ui64Value = (ulong)myn2;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToUInt64(null) != ui64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).ToUInt64(null) + ", Expected, " + ui64Value);
   }
   iCountTestcases++;
   if(!((IConvertible)(Enum)myn2).ToString(null).Equals(myn2.ToString())){
   iCountErrors++;
   Console.WriteLine("Err_20765sdg_" + i + " wrong value returned, <<" + ((IConvertible)(Enum)myn2).ToString(null) + ">>, Expected, <<" + myn2.ToString() + ">>");
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToBoolean(null)!= Convert.ToBoolean((ushort)myn2)){
   iCountErrors++;
   Console.WriteLine("Err_523ds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_ercsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToChar(null)!= Convert.ToChar((ushort)myn2)){
   iCountErrors++;
   Console.WriteLine("Err_452fwr_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_2135df_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   ((IConvertible)(Enum)myn2).ToDateTime(null);
   iCountErrors++;
   Console.WriteLine("Err_ver_" + i + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_qw3421_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToDecimal(null)!= Convert.ToDecimal((ushort)myn2)){
   iCountErrors++;
   Console.WriteLine("Err_254vds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_190w34vs_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToDouble(null)!= Convert.ToDouble((ushort)myn2)){
   iCountErrors++;
   Console.WriteLine("Err_dsf32_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_78342fsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).ToSingle(null)!= Convert.ToSingle((ushort)myn2)){
   iCountErrors++;
   Console.WriteLine("Err_342dfs_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_dfeewe_" + i + " wrong Exception thrown, " + ex);
   }				
   for(int j=0; j<arrTpValid.Length;j++){
   strLoc="Loc_5724gdf_" + i + "_" + j;
   oValue = ((IConvertible)(Enum)myn2).ToType(arrTpValid[j], null);
   iCountTestcases++;
   if(arrTpValid[j]==typeof(Boolean)){
   if(((Boolean)oValue) != Convert.ToBoolean((ushort)myn2)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn2).ToInt32(null));
   }
   }else if(((IConvertible)oValue).ToInt32(null) != ((IConvertible)(Enum)myn2).ToInt32(null)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn2).ToInt32(null));
   }
   }
   for(int j=0; j<arrTpInValid.Length;j++){
   strLoc="Loc_75234gd_" + i + "_" + j;
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn2).ToType(arrTpInValid[j], null);
   iCountErrors++;
   Console.WriteLine("Err_27854cdg_" + i + "_" + j + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_75694fd_" + i + "_" + j + " wrong Exception thrown, " + ex);
   }
   }				
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn2).ToType(myn2.GetType(), null);
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf_" + i  + " wrong Exception thrown, " + ex);
   }
   iCountTestcases++;
   if(((IConvertible)(Enum)myn2).GetTypeCode() != new UInt16().GetTypeCode()){
   iCountErrors++;
   Console.WriteLine("Err_4234_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn2).GetTypeCode() + ", Expected, " + new UInt16().GetTypeCode());
   }	
   }									
   strLoc="Loc_7429fEDf";
   tpValue = typeof(MyEnumSByte);
   arrMyEnumSByte = (MyEnumSByte[])Enum.GetValues(tpValue);
   for(int i=0; i<arrMyEnumSByte.Length; i++){
   strLoc="Loc_742fd_" + i;
   myn3 = arrMyEnumSByte[i];
   btValue = (byte)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToByte(null) != btValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToByte(null) + ", Expected, " + btValue);
   }						
   sbtValue = (sbyte)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToSByte(null) != sbtValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToSByte(null) + ", Expected, " + sbtValue);
   }						
   i16Value = (short)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToInt16(null) != i16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToInt16(null) + ", Expected, " + i16Value);
   }						
   i32Value = (int)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToInt32(null) != i32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToInt32(null) + ", Expected, " + i32Value);
   }						
   i64Value = (long)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToInt64(null) != i64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToInt64(null) + ", Expected, " + i64Value);
   }
   ui16Value = (ushort)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToUInt16(null) != ui16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToUInt16(null) + ", Expected, " + ui16Value);
   }						
   ui32Value = (uint)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToUInt32(null) != ui32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToUInt32(null) + ", Expected, " + ui32Value);
   }						
   ui64Value = (ulong)myn3;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToUInt64(null) != ui64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).ToUInt64(null) + ", Expected, " + ui64Value);
   }
   iCountTestcases++;
   if(!((IConvertible)(Enum)myn3).ToString(null).Equals(myn3.ToString())){
   iCountErrors++;
   Console.WriteLine("Err_2076sfg_" + i + " wrong value returned, <<" + ((IConvertible)(Enum)myn3).ToString(null) + ">>, Expected, <<" + myn3.ToString() + ">>");
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToBoolean(null)!= Convert.ToBoolean((sbyte)myn3)){
   iCountErrors++;
   Console.WriteLine("Err_523ds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_ercsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToChar(null)!= Convert.ToChar((sbyte)myn3)){
   iCountErrors++;
   Console.WriteLine("Err_452fwr_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_2135df_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   ((IConvertible)(Enum)myn3).ToDateTime(null);
   iCountErrors++;
   Console.WriteLine("Err_ver_" + i + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_qw3421_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToDecimal(null)!= Convert.ToDecimal((sbyte)myn3)){
   iCountErrors++;
   Console.WriteLine("Err_254vds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_190w34vs_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToDouble(null)!= Convert.ToDouble((sbyte)myn3)){
   iCountErrors++;
   Console.WriteLine("Err_dsf32_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_78342fsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).ToSingle(null)!= Convert.ToSingle((sbyte)myn3)){
   iCountErrors++;
   Console.WriteLine("Err_342dfs_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_dfeewe_" + i + " wrong Exception thrown, " + ex);
   }
   for(int j=0; j<arrTpValid.Length;j++){
   strLoc="Loc_5724gdf_" + i + "_" + j;
   oValue = ((IConvertible)(Enum)myn3).ToType(arrTpValid[j], null);
   iCountTestcases++;
   if(arrTpValid[j]==typeof(Boolean)){
   if(((Boolean)oValue) != Convert.ToBoolean((sbyte)myn3)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn3).ToInt32(null));
   }
   }else if(((IConvertible)oValue).ToInt32(null) != ((IConvertible)(Enum)myn3).ToInt32(null)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn3).ToInt32(null));
   }
   }
   for(int j=0; j<arrTpInValid.Length;j++){
   strLoc="Loc_75234gd_" + i + "_" + j;
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn3).ToType(arrTpInValid[j], null);
   iCountErrors++;
   Console.WriteLine("Err_762cdgsad_" + i + "_" + j + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_75694fd_" + i + "_" + j + " wrong Exception thrown, " + ex);
   }
   }				
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn3).ToType(myn3.GetType(), null);
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf_" + i  + " wrong Exception thrown, " + ex);
   }
   iCountTestcases++;
   if(((IConvertible)(Enum)myn3).GetTypeCode() != new SByte().GetTypeCode()){
   iCountErrors++;
   Console.WriteLine("Err_4234_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn3).GetTypeCode() + ", Expected, " + new SByte().GetTypeCode());
   }	
   }						
   strLoc="Loc_7429fEDf";
   tpValue = typeof(MyEnumLongFlags);
   arrMyEnumLongFlags = (MyEnumLongFlags[])Enum.GetValues(tpValue);
   for(int i=0; i<arrMyEnumLongFlags.Length; i++){
   strLoc="Loc_742fd_" + i;
   myn4 = arrMyEnumLongFlags[i];
   btValue = (byte)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToByte(null) != btValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToByte(null) + ", Expected, " + btValue);
   }						
   sbtValue = (sbyte)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToSByte(null) != sbtValue){
   iCountErrors++;
   Console.WriteLine("Err_5324dvs_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToSByte(null) + ", Expected, " + sbtValue);
   }						
   i16Value = (short)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToInt16(null) != i16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToInt16(null) + ", Expected, " + i16Value);
   }						
   i32Value = (int)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToInt32(null) != i32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToInt32(null) + ", Expected, " + i32Value);
   }						
   i64Value = (long)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToInt64(null) != i64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToInt64(null) + ", Expected, " + i64Value);
   }
   ui16Value = (ushort)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToUInt16(null) != ui16Value){
   iCountErrors++;
   Console.WriteLine("Err_74539d_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToUInt16(null) + ", Expected, " + ui16Value);
   }						
   ui32Value = (uint)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToUInt32(null) != ui32Value){
   iCountErrors++;
   Console.WriteLine("Err_4562ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToUInt32(null) + ", Expected, " + ui32Value);
   }						
   ui64Value = (ulong)myn4;
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToUInt64(null) != ui64Value){
   iCountErrors++;
   Console.WriteLine("Err_7429ds_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).ToUInt64(null) + ", Expected, " + ui64Value);
   }
   iCountTestcases++;
   if(!((IConvertible)(Enum)myn4).ToString(null).Equals(myn4.ToString())){
   iCountErrors++;
   Console.WriteLine("Err_1076zgdg_" + i + " wrong value returned, <<" + ((IConvertible)(Enum)myn4).ToString(null) + ">>, Expected, <<" + myn4.ToString() + ">>");
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToBoolean(null)!= Convert.ToBoolean((long)myn4)){
   iCountErrors++;
   Console.WriteLine("Err_523ds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_ercsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToChar(null)!= Convert.ToChar((long)myn4)){
   iCountErrors++;
   Console.WriteLine("Err_452fwr_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_2135df_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   ((IConvertible)(Enum)myn4).ToDateTime(null);
   iCountErrors++;
   Console.WriteLine("Err_ver_" + i + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_qw3421_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToDecimal(null)!= Convert.ToDecimal((long)myn4)){
   iCountErrors++;
   Console.WriteLine("Err_254vds_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_190w34vs_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToDouble(null)!= Convert.ToDouble((long)myn4)){
   iCountErrors++;
   Console.WriteLine("Err_dsf32_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_78342fsd_" + i + " wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).ToSingle(null)!= Convert.ToSingle((long)myn4)){
   iCountErrors++;
   Console.WriteLine("Err_342dfs_" + i + " Exception not thrown");
   }
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_dfeewe_" + i + " wrong Exception thrown, " + ex);
   }
   for(int j=0; j<arrTpValid.Length;j++){
   strLoc="Loc_5724gdf_" + i + "_" + j;
   oValue = ((IConvertible)(Enum)myn4).ToType(arrTpValid[j], null);
   iCountTestcases++;
   if(arrTpValid[j]==typeof(Boolean)){
   if(((Boolean)oValue) != Convert.ToBoolean((int)myn4)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn4).ToInt32(null));
   }
   }else if(((IConvertible)oValue).ToInt32(null) != ((IConvertible)(Enum)myn4).ToInt32(null)){
   iCountErrors++;
   Console.WriteLine("Err_4523fdw_" + i + "_" + j + " wrong value returned, " + ((IConvertible)oValue).ToInt32(null) + ", Expected, " + ((IConvertible)(Enum)myn4).ToInt32(null));
   }
   }
   for(int j=0; j<arrTpInValid.Length;j++){
   strLoc="Loc_75234gd_" + i + "_" + j;
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn4).ToType(arrTpInValid[j], null);
   iCountErrors++;
   Console.WriteLine("Err_472f8dg_" + i + "_" + j + " Exception not thrown");
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_75694fd_" + i + "_" + j + " wrong Exception thrown, " + ex);
   }
   }				
   try {
   iCountTestcases++;
   oValue = ((IConvertible)(Enum)myn4).ToType(myn4.GetType(), null);
   }catch(InvalidCastException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf_" + i  + " wrong Exception thrown, " + ex);
   }
   iCountTestcases++;
   if(((IConvertible)(Enum)myn4).GetTypeCode() != new Int64().GetTypeCode()){
   iCountErrors++;
   Console.WriteLine("Err_4234_" + i + " wrong value returned, " + ((IConvertible)(Enum)myn4).GetTypeCode() + ", Expected, " + new Int64().GetTypeCode());
   }									
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
   Co3898IConvertible_All oCbTest = new Co3898IConvertible_All();
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
  FOUR	= 4,
  EIGHT = 8,
  SIXTEEN = 16,
}
[Flags]
  public enum MyEnumByteFlags:byte
{
 ONE	= 1,
   TWO	=	2,
   FOUR	= 4,
   EIGHT = 8,
   SIXTEEN = 16,
   }
[Flags]
  public enum MyEnumShortFlags:short
{
 ONE	= 1,
   TWO	=	2,
   FOUR	= 4,
   EIGHT = 8,
   SIXTEEN = 16,
   }
[Flags]
  public enum MyEnumIntFlags:int
{
 ONE	= 1,
   TWO	=	2,
   FOUR	= 4,
   EIGHT = 8,
   SIXTEEN = 16,
   }
[Flags]
  public enum MyEnumLongFlags:long
{
 ONE	= 1,
   TWO	=	2,
   FOUR	= 4,
   EIGHT = 8,
   SIXTEEN = 16,
   }
public enum MyEnumUShort:ushort
{
 ONE	= (ushort)1,
   TWO	=	(ushort)2,
   THREE	= (ushort)3
   }
public enum MyEnumSByte:sbyte
{
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
   }
