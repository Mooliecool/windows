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
using System.Text;
using System.Runtime.Serialization;
using System.IO;
using System.Collections;
using System.Reflection;
interface IDescribeTestedMethods
{
	MemberInfo[] GetTestedMethods();
}
public class Co3888Get_Most:IDescribeTestedMethods
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "SerializationInfo.GetXXX: multiple methods";
	public static String s_strTFName        = "Co3888Get_Most.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public MemberInfo[] GetTestedMethods()
	{
		Type type = typeof(SerializationInfo);
		ArrayList list = new ArrayList();
		list.Add(type.GetMethod("GetDouble", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetDateTime", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetSingle", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetInt64", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetInt32", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetInt16", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetUInt64", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetUInt32", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetUInt16", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetBoolean", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetChar", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetByte", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetSByte", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetString", new Type[]{typeof(String)}));
		list.Add(type.GetMethod("GetDecimal", new Type[]{typeof(String)}));
		MethodInfo[] methods = new MethodInfo[list.Count];
		list.CopyTo(methods, 0);
		return methods;
	}
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		SerializationInfo serinfo1 = null;
		Boolean fValue;
		Char chValue;
		SByte sbtValue;
		Byte btValue;
		Int16 i16Value;
		Int32 i32Value;
		Int64 i64Value;
		UInt16 ui16Value;
		UInt32 ui32Value;
		UInt64 ui64Value;
		Double dblValue;
		Single sglValue;
		DateTime dtValue;
		Decimal dcmValue;
		StringBuilder sbldr1;
		String strValue;
		Random rnd1;
		try {
			do
			{
				strLoc="Loc_6573cd";
				serinfo1 = new SerializationInfo(typeof(Int32), new FormatterConverter());
				iCountTestcases++;
				if(serinfo1.MemberCount != 0)
				{
					iCountErrors++;
					Console.WriteLine("Err_0246sd! Wrong number of members, " + serinfo1.MemberCount.ToString());
				}
				strLoc="Loc_6853vd";
				fValue = false;
				serinfo1.AddValue("Boolean_1", fValue);
				iCountTestcases++;
				if(serinfo1.GetBoolean("Boolean_1") != fValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_0945csd! wrong value returned, " + serinfo1.GetBoolean("Boolean_1"));
				}
				fValue = true;
				serinfo1.AddValue("Boolean_2", fValue);
				iCountTestcases++;
				if(serinfo1.GetBoolean("Boolean_2") != fValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_6753vd! wrong value returned, " + serinfo1.GetBoolean("Boolean_2"));
				}
				try {
					iCountTestcases++;
					serinfo1.AddValue("Boolean_2", fValue);
					iCountErrors++;
					Console.WriteLine("Err_1065753cd! Exception not thrown");
					}catch(SerializationException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_5739cd! Wrong exception thrown, " + ex);
				}
				Console.WriteLine("Large String, ticks, " + Environment.TickCount);
				sbldr1 = new StringBuilder("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
				fValue = false;
				serinfo1.AddValue(sbldr1.ToString(), fValue);
				iCountTestcases++;
				if(serinfo1.GetBoolean(sbldr1.ToString()) != fValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_6538fvd! wrong value returned, " + serinfo1.GetBoolean(sbldr1.ToString()));
				}
				try {
					iCountTestcases++;
					serinfo1.AddValue(null, fValue);
					iCountErrors++;
					Console.WriteLine("Err_0156ds! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_57834fd! Wrong exception thrown, " + ex);
				}
				Console.WriteLine("Char ticks, " + Environment.TickCount);
				rnd1 = new Random();
				for(int i=0; i<50; i++) {
					strLoc="Loc_6753cd_" + i;
					chValue = (Char)(65536 * rnd1.NextDouble());
					strValue = "Char_" + i;
					serinfo1.AddValue(strValue, chValue);
					iCountTestcases++;
					if(serinfo1.GetChar(strValue)!= chValue)
					{
						iCountErrors++;
						Console.WriteLine("Err_65730dsw_" + i + "! Wrong Char returned, " + serinfo1.GetChar(strValue));
					}
				}
				try {
					iCountTestcases++;
					serinfo1.AddValue("Char_1", 'a');
					iCountErrors++;
					Console.WriteLine("Err_643cd! Exception not thrown");
					}catch(SerializationException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_02457fd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					serinfo1.AddValue("Boolean_1", 'a');
					iCountErrors++;
					Console.WriteLine("Err_5732fcd! Exception not thrown");
					}catch(SerializationException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_024568fd! Wrong exception thrown, " + ex);
				}
				Console.WriteLine("SByte ticks, " + Environment.TickCount);
				for(int i=(int)SByte.MinValue; i<(int)SByte.MaxValue; i++) {
					strLoc="Loc_56473vd_" + i;
					sbtValue = (SByte)i;;
					strValue = "SByte_" + i;
					serinfo1.AddValue(strValue, sbtValue);
					iCountTestcases++;
					if(serinfo1.GetSByte(strValue)!= sbtValue)
					{
						iCountErrors++;
						Console.WriteLine("Err_4627fds_" + i + "! Wrong Sbyte returned, " + serinfo1.GetSByte(strValue));
					}
				}
				Console.WriteLine("Byte ticks, " + Environment.TickCount);
				for(int i=(int)Byte.MinValue; i<(int)Byte.MaxValue; i++) {
					strLoc="Loc_01192ds_" + i;
					btValue = (Byte)i;;
					strValue = "Byte_" + i;
					serinfo1.AddValue(strValue, btValue);
					iCountTestcases++;
					if(serinfo1.GetByte(strValue)!= btValue)
					{
						iCountErrors++;
						Console.WriteLine("Err_0267fe_" + i + "! Wrong byte returned, " + serinfo1.GetByte(strValue));
					}
				}
				Console.WriteLine("Int16 ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_012965565ds_" + i;
					i16Value = (short)((int)Int16.MaxValue * rnd1.NextDouble());
					if(rnd1.NextDouble()<0.5)
					i16Value = (short)(-1 * i16Value);
					strValue = "Int16_" + i;
					serinfo1.AddValue(strValue, i16Value);
					iCountTestcases++;
					if(serinfo1.GetInt16(strValue)!= i16Value)
					{
						iCountErrors++;
						Console.WriteLine("Err_0267fe_" + i + "! Wrong value returned, " + serinfo1.GetInt16(strValue));
					}
				}
				Console.WriteLine("Int32 ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_015643ds_" + i;
					i32Value = (int)(Int32.MaxValue * rnd1.NextDouble());
					if(rnd1.NextDouble()<0.5)
					i32Value = (-1 * i32Value);
					strValue = "Int32_" + i;
					serinfo1.AddValue(strValue, i32Value);
					iCountTestcases++;
					if(serinfo1.GetInt32(strValue)!= i32Value)
					{
						iCountErrors++;
						Console.WriteLine("Err_5427ds_" + i + "! Wrong value returned, " + serinfo1.GetInt32(strValue));
					}
				}
				Console.WriteLine("Int64 ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_625bfg_" + i;
					i64Value = (long)((long)Int64.MaxValue * rnd1.NextDouble());
					if(rnd1.NextDouble()<0.5)
					i64Value = (long)(-1 * i64Value);
					strValue = "Int64_" + i;
					serinfo1.AddValue(strValue, i64Value);
					iCountTestcases++;
					if(serinfo1.GetInt64(strValue)!= i64Value)
					{
						iCountErrors++;
						Console.WriteLine("Err_6427dc_" + i + "! Wrong value returned, " + serinfo1.GetInt64(strValue));
					}
				}
				Console.WriteLine("UInt16 ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_6473cd_" + i;
					ui16Value = (ushort)((int)UInt16.MaxValue * rnd1.NextDouble());
					strValue = "UInt16_" + i;
					serinfo1.AddValue(strValue, ui16Value);
					iCountTestcases++;
					if(serinfo1.GetUInt16(strValue)!= ui16Value)
					{
						iCountErrors++;
						Console.WriteLine("Err_748vd_" + i + "! Wrong value returned, " + serinfo1.GetUInt16(strValue));
					}
				}
				Console.WriteLine("UInt32 ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_7573cd_" + i;
					ui32Value = (uint)(UInt32.MaxValue * rnd1.NextDouble());
					strValue = "UInt32_" + i;
					serinfo1.AddValue(strValue, ui32Value);
					iCountTestcases++;
					if(serinfo1.GetUInt32(strValue)!= ui32Value)
					{
						iCountErrors++;
						Console.WriteLine("Err_4738cd_" + i + "! Wrong value returned, " + serinfo1.GetUInt32(strValue));
					}
				}
				Console.WriteLine("UInt64 ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_63dc_" + i;
					ui64Value = (ulong)(UInt64.MaxValue * rnd1.NextDouble());
					strValue = "UInt64_" + i;
					serinfo1.AddValue(strValue, ui64Value);
					iCountTestcases++;
					if(serinfo1.GetUInt64(strValue)!= ui64Value)
					{
						iCountErrors++;
						Console.WriteLine("Err_6583fd_" + i + "! Wrong value returned, " + serinfo1.GetUInt64(strValue));
					}
				}
				Console.WriteLine("Double ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_7539cd_" + i;
					dblValue = Double.MaxValue * rnd1.NextDouble();
					if(rnd1.NextDouble()<0.5)
					dblValue = (-1 * dblValue);
					strValue = "Double_" + i;
					serinfo1.AddValue(strValue, dblValue);
					iCountTestcases++;
					if(serinfo1.GetDouble(strValue)!= dblValue)
					{
						iCountErrors++;
						Console.WriteLine("Err_653cfd_" + i + "! Wrong value returned, " + serinfo1.GetDouble(strValue));
					}
				}
				Console.WriteLine("Single ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_0247fd_" + i;
					sglValue = (float)(Single.MaxValue * rnd1.NextDouble());
					if(rnd1.NextDouble()<0.5)
					sglValue = (-1 * sglValue);
					strValue = "Single_" + i;
					serinfo1.AddValue(strValue, sglValue);
					iCountTestcases++;
					if(serinfo1.GetSingle(strValue)!= sglValue)
					{
						iCountErrors++;
						Console.WriteLine("Err_0468fd_" + i + "! Wrong value returned, " + serinfo1.GetSingle(strValue));
					}
				}
				strValue = "This is a String";
				serinfo1.AddValue("String_1", strValue);
				iCountTestcases++;
				if(!serinfo1.GetString("String_1").Equals(strValue))
				{
					iCountErrors++;
					Console.WriteLine("Err_0945csd! wrong value returned, " + serinfo1.GetString("String_1"));
				}
				strValue = "";
				serinfo1.AddValue("String_2", strValue);
				iCountTestcases++;
				if(!serinfo1.GetString("String_2").Equals(String.Empty))
				{
					iCountErrors++;
					Console.WriteLine("Err_0945csd! wrong value returned, " + serinfo1.GetString("String_2"));
				}
				strValue = null;
				serinfo1.AddValue("String_3", strValue);
				iCountTestcases++;
				if(serinfo1.GetString("String_3") != null)
				{
					iCountErrors++;
					Console.WriteLine("Err_0945csd! wrong value returned, " + serinfo1.GetString("String_3"));
				}
				try {
					iCountTestcases++;
					serinfo1.AddValue("String_2", fValue);
					iCountErrors++;
					Console.WriteLine("Err_1065753cd! Exception not thrown");
					}catch(SerializationException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_5739cd! Wrong exception thrown, " + ex);
				}
				Console.WriteLine("Single ticks, " + Environment.TickCount);
				for(int i=0; i<50; i++) {
					strLoc="Loc_0247fd_" + i;
					dblValue = (double)(DateTime.MaxValue.ToOADate() * rnd1.NextDouble());
					strValue = "DateTime_" + i;
					dtValue = DateTime.FromOADate(dblValue);
					serinfo1.AddValue(strValue, dtValue);
					iCountTestcases++;
					if(serinfo1.GetDateTime(strValue)!= dtValue)
					{
						iCountErrors++;
						Console.WriteLine("Err_0468fd_" + i + "! Wrong value returned, " + serinfo1.GetDateTime(strValue));
					}
				}
				for(int i=0; i<50; i++) {
					strLoc="Loc_0247fd_" + i;
					dcmValue = (Decimal)((double)Decimal.MaxValue * rnd1.NextDouble());
					if(rnd1.NextDouble()<0.5)
						dcmValue = (Decimal)(-1 * dcmValue);
					strValue = "Decimal_" + i;
					serinfo1.AddValue(strValue, dcmValue);
					iCountTestcases++;
					if(serinfo1.GetDecimal(strValue)!= dcmValue)
					{
						iCountErrors++;
						Console.WriteLine("Err_2342fdsg_" + i + "! Wrong value returned, " + serinfo1.GetDecimal(strValue));
					}
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general.StackTrace);
		}
		if ( iCountErrors == 0 )
		{
			Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
			return true;
		}
		else
		{
			Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
			return false;
		}
	}
	public static void Main(String[] args)
	{
		bool bResult = false;
		Co3888Get_Most cbA = new Co3888Get_Most();
		try {
			bResult = cbA.runTest();
			} catch (Exception exc_main){
			bResult = false;
			Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
		}
		if (!bResult)
		{
			Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
			Console.WriteLine( " " );
			Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
			Console.WriteLine( " " );
		}
		if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
	}
}
