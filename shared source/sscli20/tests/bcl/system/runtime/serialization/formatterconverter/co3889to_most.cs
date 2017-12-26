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
public class Co3889To_most:IDescribeTestedMethods
{
	public static String s_strActiveBugNums = "";
	public static String s_strDtTmVer       = "";
	public static String s_strClassMethod   = "FormatterConverter.ToXXX: multiple methods";
	public static String s_strTFName        = "Co3889To_most.cs";
	public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
	public static String s_strTFPath        = Environment.CurrentDirectory;
	public MemberInfo[] GetTestedMethods()
	{
		Type type = typeof(FormatterConverter);
		ArrayList list = new ArrayList();
		MethodInfo[] methods = type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static);
		for(int i=0; i<methods.Length; i++){
			if (methods[i].IsPublic && (methods[i].DeclaringType == methods[i].ReflectedType)) {
				list.Add(methods[i]);
			}
		}
		ConstructorInfo[] ctors = type.GetConstructors(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static);
		for(int i=0; i<ctors.Length; i++) {
			if (ctors[i].IsPublic  && (ctors[i].DeclaringType == ctors[i].ReflectedType)) {
				list.Add(ctors[i]);
			}
		}
		MemberInfo[] members = new MemberInfo[list.Count];
		list.CopyTo(members, 0);
		return members;
	}
	public bool runTest()
	{
		Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver : " + s_strDtTmVer);
		int iCountErrors = 0;
		int iCountTestcases = 0;
		String strLoc = "Loc_000oo";
		FormatterConverter fmtcnv1 = null;
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
		Decimal dcValue;
		Object oValue;
		Object oRtnValue;
		String strValue;
		TypeCode tpcd1;
		Object[] oTpCodeArr;
		TypeCode[] arrTpCodeArr;
		Object[] oArr = {false, (SByte)5, (Byte)5, (Int16)5, (Int32)5, (Int64)5,
		(UInt16)5, (UInt32)5, (UInt64)5, (Single)5.0, (Double)5.0, };
		Type[] tpArr  = {typeof(Boolean), typeof(SByte), typeof(Byte), typeof(Int16), typeof(Int32),
			typeof(Int64), typeof(UInt16), typeof(UInt32), typeof(UInt64), typeof(Single),
		typeof(Double), };
		try {
			do
			{
				strLoc="Loc_6573cd";
				fmtcnv1 = new FormatterConverter();
				strValue = "false";
				fValue = false;
				iCountTestcases++;
				if(fmtcnv1.ToBoolean(strValue) != fValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_0246sd! Wrong value returned, " + fmtcnv1.ToBoolean(strValue));
				}
				i32Value = 5;
				fValue = true;
				iCountTestcases++;
				if(fmtcnv1.ToBoolean(i32Value) != fValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_87430cd! Wrong value returned, " + fmtcnv1.ToBoolean(i32Value));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToBoolean(strValue);
					iCountErrors++;
					Console.WriteLine("Err_1065753cd! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_5739cd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToBoolean(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				btValue = 5;
				iCountTestcases++;
				if(fmtcnv1.ToByte(i32Value) != btValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_0753cfd! Wrong value returned, " + fmtcnv1.ToByte(i32Value));
				}
				strValue = "5";
				btValue = 5;
				iCountTestcases++;
				if(fmtcnv1.ToByte(strValue) != btValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_0483fd! Wrong value returned, " + fmtcnv1.ToByte(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToByte(strValue);
					iCountErrors++;
					Console.WriteLine("Err_034752fsd! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_04729cd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i32Value = 743290;
					fmtcnv1.ToByte(i32Value);
					iCountErrors++;
					Console.WriteLine("Err_047239fd! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_017s! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToByte(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				chValue = (Char)5;
				iCountTestcases++;
				if(fmtcnv1.ToChar(i32Value)!=chValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_8475320vdef! Wrong value returned, " + fmtcnv1.ToChar(i32Value));
				}
				strValue = "5";
				chValue = (Char)53;
				iCountTestcases++;
				if(fmtcnv1.ToChar(strValue)!=chValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_01237xs! Wrong value returned, " + fmtcnv1.ToChar(strValue) + " " + chValue);
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToChar(strValue);
					iCountErrors++;
					Console.WriteLine("Err_048329fde! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1093cd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i32Value = 743290;
					fmtcnv1.ToChar(i32Value);
					iCountErrors++;
					Console.WriteLine("Err_0483vfd! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_017s! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToChar(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				strLoc="Loc_34732fd";
				dtValue = DateTime.Now;
				strValue = dtValue.ToString();
				iCountTestcases++;
				if(!fmtcnv1.ToDateTime(strValue).ToString().Equals(dtValue.ToString()))
				{
					iCountErrors++;
					Console.WriteLine("Err_0278423d! Wrong value returned, " + fmtcnv1.ToDateTime(strValue) + " " + dtValue);
				}
				strLoc="Loc_047gd";
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToDateTime(strValue);
					iCountErrors++;
					Console.WriteLine("Err_34234dsf! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_dfsdfsfsng exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i32Value = 743290;
					fmtcnv1.ToByte(i32Value);
					iCountErrors++;
					Console.WriteLine("Err_0453fd! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_10084523f! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToByte(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				strLoc="Loc_44732fd";
				dcValue = (Decimal)123.23;
				strValue = "123.23";
				if (!dcValue.ToString().Equals(strValue))
					throw new Exception("Decimal ToString doesn't match expected value!  Decimal: "+dcValue.ToString());
				iCountTestcases++;
				if(fmtcnv1.ToDecimal(strValue)!=dcValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_342fsd! Wrong value returned, " + fmtcnv1.ToDecimal(strValue) );
				}
				strLoc="Loc_047gd";
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToDecimal(strValue);
					iCountErrors++;
					Console.WriteLine("Err_342dfs! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_8342vds exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					dblValue = Double.MaxValue;
					fmtcnv1.ToDecimal(dblValue);
					iCountErrors++;
					Console.WriteLine("Err_1091212dsdas! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_10874cd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToDecimal(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				strLoc="Loc_24732fd";
				dblValue = 123.23;
				strValue = "123.23";
				iCountTestcases++;
				if(fmtcnv1.ToDouble(strValue)!=dblValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_10742cd! Wrong value returned, " + fmtcnv1.ToDouble(strValue) );
				}
				strLoc="Loc_047gd";
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToDouble(strValue);
					iCountErrors++;
					Console.WriteLine("Err_197wc! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1087wxs exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fValue = true;
					Double d = fmtcnv1.ToDouble(fValue);
                                        if ( d != 1 )
                                        {
					        iCountErrors++;
					        Console.WriteLine("FormatConvertor.ToDouble method returns unexpected value." + d);
                                        }        
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_02834cd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToDouble(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				i16Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToInt16(i32Value) != i16Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_045fdc! Wrong value returned, " + fmtcnv1.ToInt16(i32Value));
				}
				strValue = "5";
				i16Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToInt16(strValue) != i16Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_04523vdf! Wrong value returned, " + fmtcnv1.ToInt16(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToInt16(strValue);
					iCountErrors++;
					Console.WriteLine("Err_134782ds! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_107342dcs! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i32Value = Int32.MaxValue;
					fmtcnv1.ToInt16(i32Value);
					iCountErrors++;
					Console.WriteLine("Err_93742cvsd! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_0134vsdf! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToInt16(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				i64Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToInt32(i64Value) != i32Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_0753cfd! Wrong value returned, " + fmtcnv1.ToInt32(i64Value));
				}
				strValue = "5";
				i32Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToInt32(strValue) != i32Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_197axs! Wrong value returned, " + fmtcnv1.ToInt32(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToInt32(strValue);
					iCountErrors++;
					Console.WriteLine("Err_1112s! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_dasda213! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i64Value = Int64.MaxValue;
					fmtcnv1.ToInt32(i64Value);
					iCountErrors++;
					Console.WriteLine("Err_2423cds! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_0231das! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToInt32(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				i64Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToInt64(i32Value) != i64Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_452cds! Wrong value returned, " + fmtcnv1.ToInt64(i32Value));
				}
				strValue = "5";
				i64Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToInt64(strValue) != i64Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_234dcd! Wrong value returned, " + fmtcnv1.ToInt64(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToInt64(strValue);
					iCountErrors++;
					Console.WriteLine("Err_134! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_fdvw! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					dblValue = Double.MaxValue;
					fmtcnv1.ToInt64(dblValue);
					iCountErrors++;
					Console.WriteLine("Err_23084ds! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1084d! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToInt64(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				sbtValue = 5;
				iCountTestcases++;
				if(fmtcnv1.ToSByte(i32Value) != sbtValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_013248csd! Wrong value returned, " + fmtcnv1.ToSByte(i32Value));
				}
				strValue = "5";
				sbtValue = 5;
				iCountTestcases++;
				if(fmtcnv1.ToSByte(strValue) != sbtValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_10732sx! Wrong value returned, " + fmtcnv1.ToSByte(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToSByte(strValue);
					iCountErrors++;
					Console.WriteLine("Err_632sa! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_6732ds! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i32Value = 743290;
					fmtcnv1.ToSByte(i32Value);
					iCountErrors++;
					Console.WriteLine("Err_003sw! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_13853ds! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToSByte(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				sglValue = 5.0f;
				iCountTestcases++;
				if(fmtcnv1.ToSingle(i32Value) != sglValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_0527fds! Wrong value returned, " + fmtcnv1.ToSingle(i32Value));
				}
				strValue = "5";
				sglValue = 5;
				iCountTestcases++;
				if(fmtcnv1.ToSingle(strValue) != sglValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_842fsd! Wrong value returned, " + fmtcnv1.ToSingle(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToSingle(strValue);
					iCountErrors++;
					Console.WriteLine("Err_3421da! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_34123! Wrong exception thrown, " + ex);
				}
				dblValue = Double.MaxValue;
				sglValue = Single.PositiveInfinity;
				iCountTestcases++;
				if(fmtcnv1.ToSingle(dblValue) != sglValue)
				{
					iCountErrors++;
					Console.WriteLine("Err_2231dfs! Wrong value returned, " + fmtcnv1.ToSingle(dblValue));
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToSingle(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				strValue = "5";
				iCountTestcases++;
				if(!strValue.Equals(fmtcnv1.ToString(i32Value)))
				{
					iCountErrors++;
					Console.WriteLine("Err_93472fsd! Wrong value returned, " + fmtcnv1.ToString(i32Value));
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToString(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				ui16Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToUInt16(i32Value) != ui16Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_045fvd! Wrong value returned, " + fmtcnv1.ToUInt16(i32Value));
				}
				strValue = "5";
				ui16Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToUInt16(strValue) != ui16Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_1834dcvds! Wrong value returned, " + fmtcnv1.ToUInt16(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToUInt16(strValue);
					iCountErrors++;
					Console.WriteLine("Err_183cs! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_189ws! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i32Value = Int32.MaxValue;
					fmtcnv1.ToUInt16(i32Value);
					iCountErrors++;
					Console.WriteLine("Err_342fds! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_6454cd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToUInt16(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				ui32Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToUInt32(i64Value) != ui32Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_7834vfd! Wrong value returned, " + fmtcnv1.ToUInt32(i64Value));
				}
				strValue = "5";
				ui32Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToUInt32(strValue) != ui32Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_1243vcd! Wrong value returned, " + fmtcnv1.ToUInt32(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToUInt32(strValue);
					iCountErrors++;
					Console.WriteLine("Err_1234csx! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_5634fsd! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					i64Value = Int64.MaxValue;
					fmtcnv1.ToUInt32(i64Value);
					iCountErrors++;
					Console.WriteLine("Err_0453vdf! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_0342vfdf! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToUInt32(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				i32Value = 5;
				ui64Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToUInt64(i32Value) != ui64Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_38503dc! Wrong value returned, " + fmtcnv1.ToUInt64(i32Value));
				}
				strValue = "5";
				ui64Value = 5;
				iCountTestcases++;
				if(fmtcnv1.ToUInt64(strValue) != ui64Value)
				{
					iCountErrors++;
					Console.WriteLine("Err_0173xs! Wrong value returned, " + fmtcnv1.ToUInt64(strValue));
				}
				try {
					iCountTestcases++;
					strValue = "Hey man";
					fmtcnv1.ToUInt64(strValue);
					iCountErrors++;
					Console.WriteLine("Err_013csd! Exception not thrown");
					}catch(FormatException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_75421xs! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					dblValue = Double.MaxValue;
					fmtcnv1.ToUInt64(dblValue);
					iCountErrors++;
					Console.WriteLine("Err_3472ds! Exception not thrown");
					}catch(OverflowException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_vefe! Wrong exception thrown, " + ex);
				}
				try {
					iCountTestcases++;
					fmtcnv1.ToUInt64(null);
					iCountErrors++;
					Console.WriteLine("Err_84532fd! Exception not thrown");
					}catch(ArgumentNullException){
					}catch(Exception ex){
					iCountErrors++;
					Console.WriteLine("Err_1382cs! Wrong exception thrown, " + ex);
				}
				iCountTestcases++;
				for(int i=0;i<oArr.Length; i++){
					oValue = oArr[i];
					for(int j=0;j<tpArr.Length; j++){
						try{
							oRtnValue = fmtcnv1.Convert(oValue, tpArr[j]);
							}catch(Exception ex){
							if(ex.ToString().IndexOf("InvalidCastException")==-1)
							Console.WriteLine(i + " " + j + " " + ex);
						}
					}
				}
				iCountTestcases++;
				arrTpCodeArr = (TypeCode[])Enum.GetValues(typeof(TypeCode));
				for(int i=0;i<oArr.Length; i++){
					oValue = oArr[i];
					for(int j=0;j<arrTpCodeArr.Length; j++){
						tpcd1 = arrTpCodeArr[j];
						try{
							oRtnValue = fmtcnv1.Convert(oValue, tpcd1);
							}catch(Exception ex){
							if(ex.ToString().IndexOf("InvalidCastException")==-1)
							Console.WriteLine(i + " " + j + " " + ex);
						}
					}
				}
			} while (false);
			} catch (Exception exc_general ) {
			++iCountErrors;
			Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general==\n"+exc_general);
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
		Co3889To_most cbA = new Co3889To_most();
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
