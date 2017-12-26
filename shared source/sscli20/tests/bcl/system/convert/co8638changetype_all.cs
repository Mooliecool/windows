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
using System.Collections;
using System.Globalization;
using System.IO;
using System.Threading;
public class Co8638ChangeType_all
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ChangeType: all";
 public static String s_strTFName        = "Co8638ChangeType_all.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Boolean[] blnValues = {true, false};
   SByte[] sbytes = {-128, -5, 0, 5, 127};
   Byte[] bytes = {0, 5, 127, 255};
   Int16[] i16s =  {Int16.MinValue, -5, 0, 5, Int16.MaxValue};
   Int32[] i32s =  {Int32.MinValue, -5, 0, 5, Int32.MaxValue};
   Int64[] i64s =  {Int64.MinValue, -5, 0, 5, Int64.MaxValue};
   UInt16[] ui16s = {0, 5, 127, UInt16.MaxValue};
   UInt32[] ui32s = {0, 5, 127, UInt32.MaxValue};
   UInt64[] ui64s = {0, 5, 127, UInt64.MaxValue};
   Single[] singles = {Single.MinValue, -1.2f, 0.0f, 1.23f, Single.MaxValue, Single.NaN, Single.PositiveInfinity, Single.NegativeInfinity, Single.Epsilon};
   Double[] doubles = {Double.MinValue, -1.2, 0.0, 1.23, Double.MaxValue, Double.NaN, Double.PositiveInfinity, Double.NegativeInfinity, Double.Epsilon};
   Decimal[] decis = {Decimal.MinValue, -1.23m, 0, 1.23m, Decimal.MaxValue};
   Char[] chars = {'a', 'b', 'A', '1'};
   DateTime[] dates = {DateTime.MinValue, DateTime.Now, new DateTime(2001, 4, 24), DateTime.MaxValue};
   String[] strings = {"1", "0",  "5"};
   String[] exceptionStrings = {"ThrowAnExceptionHEre", "HElloWorld",  ""};
   TimeSpan ts1; 
   TypeCode[] codes;
   TypeCode code;
   Object returnedValue;
   IConvertible icon;
   DBNull dbn1;
   try
     {
     strLoc = "Loc_9347sg";
     iCountTestcases++;
     code = TypeCode.Boolean;
     try{
     codes = (TypeCode[])Enum.GetValues(typeof(TypeCode));
     for(int i=0; i<codes.Length; i++){					
     code = codes[i];
     for(int j=0; j<blnValues.Length; j++){
     try{
     returnedValue = Convert.ChangeType(blnValues[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_3497sdg! Value: {0}, Type: {1}, Code: {2}", returnedValue, returnedValue.GetType(), code);
     }
     }
     }catch(InvalidCastException){
     if(code != TypeCode.Empty
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.Char)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3847dg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_348sg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<sbytes.Length; j++){
     try{
     returnedValue = Convert.ChangeType(sbytes[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if((code != TypeCode.Char)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.UInt64)
	){
     iCountErrors++;
     Console.WriteLine("ERr_23947tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_07245tzdg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<bytes.Length; j++){
     try{
     returnedValue = Convert.ChangeType(bytes[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3947tsdg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if(code != TypeCode.SByte){
     iCountErrors++;
     Console.WriteLine("ERr_3942tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_1974trsg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     iCountTestcases++;
     for(int j=0; j<i16s.Length; j++)
       {
       try{
       returnedValue = Convert.ChangeType(i16s[j], code);
       icon = returnedValue as IConvertible;
       if(icon==null){
       iCountErrors++;
       Console.WriteLine("Err_7495wsdg! Unexpected value returned");
       }else{
       if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
       iCountErrors++;
       Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
       }
       }
       }catch(InvalidCastException){
       if((code != TypeCode.Empty)
	  && (code != TypeCode.DBNull)
	  && (code != TypeCode.DateTime)
	  ){
       iCountErrors++;
       Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
       }
       }catch(OverflowException){
       if((code != TypeCode.Char)
	  && (code != TypeCode.Byte)
	  && (code != TypeCode.SByte)
	  && (code != TypeCode.UInt16)
	  && (code != TypeCode.UInt32)
	  && (code != TypeCode.UInt64)
	  ){
       iCountErrors++;
       Console.WriteLine("ERr_2497tsg! Unexpected Typecode returned " + code);
       }
       }catch(Exception ex){
       iCountErrors++;
       Console.WriteLine("Err_297trsgf! Unexpected exception returned, " + ex.GetType().Name);
       }
       }
     iCountTestcases++;
     for(int j=0; j<i32s.Length; j++)
       {
       try{
       returnedValue = Convert.ChangeType(i32s[j], code);
       icon = returnedValue as IConvertible;
       if(icon==null){
       iCountErrors++;
       Console.WriteLine("Err_7495wsdg! Unexpected value returned");
       }else{
       if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
       iCountErrors++;
       Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
       }
       }
       }catch(InvalidCastException){
       if((code != TypeCode.Empty)
	  && (code != TypeCode.DBNull)
	  && (code != TypeCode.DateTime)
	  ){
       iCountErrors++;
       Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
       }
       }catch(OverflowException){
       if((code != TypeCode.Char)
	  && (code != TypeCode.Byte)
	  && (code != TypeCode.SByte)
	  && (code != TypeCode.UInt16)
	  && (code != TypeCode.UInt32)
	  && (code != TypeCode.UInt64)
	  && (code != TypeCode.Int16)
	  ){
       iCountErrors++;
       Console.WriteLine("ERr_207056tsfg! Unexpected Typecode returned Code: {0}, Value: {1}", code, i32s[j]);
       }
       }catch(Exception ex){
       iCountErrors++;
       Console.WriteLine("Err_497tsg! Unexpected exception returned, " + ex.GetType().Name);
       }
       }
     for(int j=0; j<i64s.Length; j++){
     try{
     returnedValue = Convert.ChangeType(i64s[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if((code != TypeCode.Char)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.Int32)
	){
     iCountErrors++;
     Console.WriteLine("ERr_20745sg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_590374tsfg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }	
     for(int j=0; j<ui16s.Length; j++){
     try{
     returnedValue = Convert.ChangeType(ui16s[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3947tsdg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if(code != TypeCode.SByte
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	){
     iCountErrors++;
     Console.WriteLine("ERr_2347tsdg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_6034tsdg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<ui32s.Length; j++){
     try{
     returnedValue = Convert.ChangeType(ui32s[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3947tsdg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if(code != TypeCode.SByte
	&& (code != TypeCode.Char)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.Int32)
	){
     iCountErrors++;
     Console.WriteLine("ERr_237095tsdg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_757trsg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<ui64s.Length; j++){
     try{
     returnedValue = Convert.ChangeType(ui64s[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3947tsdg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if(code != TypeCode.SByte
	&& (code != TypeCode.Char)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.Int64)
	){
     iCountErrors++;
     Console.WriteLine("ERr_12076trsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_84975tsgd! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<singles.Length; j++){
     try{
     returnedValue = Convert.ChangeType(singles[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.Char)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if((code != TypeCode.Byte)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.Decimal)
	){
     iCountErrors++;
     Console.WriteLine("ERr_23947tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_92745tsdg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<doubles.Length; j++){
     try{
     returnedValue = Convert.ChangeType(doubles[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.Char)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if((code != TypeCode.Byte)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.Decimal)
	){
     iCountErrors++;
     Console.WriteLine("ERr_23947tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_0237rsdg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<decis.Length; j++){
     try{
     returnedValue = Convert.ChangeType(decis[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.Char)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if((code != TypeCode.Byte)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.Decimal)
	){
     iCountErrors++;
     Console.WriteLine("ERr_23947tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_1038sdg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<chars.Length; j++){
     try{
     returnedValue = Convert.ChangeType(chars[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.Boolean)
	&& (code != TypeCode.Single)
	&& (code != TypeCode.Double)
	&& (code != TypeCode.Decimal)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(OverflowException){
     if((code != TypeCode.Char)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.UInt64)
	){
     iCountErrors++;
     Console.WriteLine("ERr_23947tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_20357sdfg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     if(Thread.CurrentThread.CurrentCulture.LCID==0x0409){
     for(int j=0; j<dates.Length; j++){
     try{
     returnedValue = Convert.ChangeType(dates[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.Boolean)
	&& (code != TypeCode.Char)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Single)
	&& (code != TypeCode.Double)
	&& (code != TypeCode.Decimal)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_30784trsfg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     }
     for(int j=0; j<strings.Length; j++){
     try{
     returnedValue = Convert.ChangeType(strings[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(FormatException){
     if((code != TypeCode.Char)
	&& (code != TypeCode.Boolean)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_23947tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_4028rfdsg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     for(int j=0; j<exceptionStrings.Length; j++){
     try{
     returnedValue = Convert.ChangeType(exceptionStrings[j], code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_932475sdg! Value: {0}, Type: {1}", returnedValue, returnedValue.GetType());
     }
     }
     }catch(InvalidCastException){
     if((code != TypeCode.Empty)
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.DateTime)
	){
     iCountErrors++;
     Console.WriteLine("ERr_324957tsg! Unexpected Typecode returned " + code);
     }
     }catch(FormatException){
     if((code != TypeCode.Char)
	&& (code != TypeCode.Boolean)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Decimal)
	&& (code != TypeCode.Single)
	&& (code != TypeCode.Double)
	){
     iCountErrors++;
     Console.WriteLine("ERr_23947tsg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_50347txfg! Unexpected exception returned, " + ex.GetType().Name);
     }
     }
     dbn1 = DBNull.Value;
     try{
     returnedValue = Convert.ChangeType(dbn1, code);
     icon = returnedValue as IConvertible;
     if(icon==null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned");
     }else{
     if(code!=TypeCode.Object && icon.GetTypeCode()!=code){
     iCountErrors++;
     Console.WriteLine("Err_3497sdg! Value: {0}, Type: {1}, Code: {2}", returnedValue, returnedValue.GetType(), code);
     }
     }
     }catch(InvalidCastException){
     if(code != TypeCode.Empty
	&& (code != TypeCode.DBNull)
	&& (code != TypeCode.Char)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.Boolean)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Decimal)
	&& (code != TypeCode.Single)
	&& (code != TypeCode.Double)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3847dg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_604327rsg! Unexpected exception returned, " + ex.GetType().Name);
     }
     try{
     returnedValue = Convert.ChangeType(null, code);
     icon = returnedValue as IConvertible;
     if(icon!=null){
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned, " + code);
     }
     }catch(InvalidCastException){
     if((code != TypeCode.DBNull)
	&& (code != TypeCode.Char)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.Boolean)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Decimal)
	&& (code != TypeCode.Single)
	&& (code != TypeCode.Double)
	&& (code != TypeCode.Object)
	&& (code != TypeCode.String)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3847dg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_74325resgd! Unexpected exception returned, " + ex.GetType().Name);
     }					
     ts1 = TimeSpan.MaxValue;
     try{
     returnedValue = Convert.ChangeType(ts1, code);
     iCountErrors++;
     Console.WriteLine("Err_7495wsdg! Unexpected value returned, " + code);
     }catch(InvalidCastException){
     if((code != TypeCode.DBNull)
	&& (code != TypeCode.Char)
	&& (code != TypeCode.DateTime)
	&& (code != TypeCode.Boolean)
	&& (code != TypeCode.SByte)
	&& (code != TypeCode.Byte)
	&& (code != TypeCode.Int16)
	&& (code != TypeCode.UInt16)
	&& (code != TypeCode.Int32)
	&& (code != TypeCode.UInt32)
	&& (code != TypeCode.Int64)
	&& (code != TypeCode.UInt64)
	&& (code != TypeCode.Decimal)
	&& (code != TypeCode.Single)
	&& (code != TypeCode.Double)
	&& (code != TypeCode.Object)
	&& (code != TypeCode.String)
	&& (code != TypeCode.Empty)
	){
     iCountErrors++;
     Console.WriteLine("ERr_3847dg! Unexpected Typecode returned " + code);
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_845sdg! Unexpected exception returned, " + ex.GetType().Name);
     }					
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_9347tsfgbf! Unexpected exception returned, " + ex.GetType().Name);
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8638ChangeType_all cbA = new Co8638ChangeType_all();
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
