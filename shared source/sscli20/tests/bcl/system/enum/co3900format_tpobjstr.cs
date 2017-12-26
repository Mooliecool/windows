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
public class Co3900Format_TpObjStr
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Enum.ToString()";
 public static String s_strTFName        = "Co3900Format_TpObjStr.cs";
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
   MyEnum myn1;
   Int32 i32Value;
   MyEnumFlags myn2;
   Enum en1;
   String strValue;
   String strRetValue;
   MyEnumByte myen_b;
   MyEnumSByte myen_sb;
   MyEnumShort myen_sob;
   MyEnumInt myen_i;
   MyEnumLong myen_l;
   MyEnumUShort myen_us;
   MyEnumUInt myen_ui;
   MyEnumULong myen_ul;
   Type[] tpArrEnums = {typeof(MyEnumByte), typeof(MyEnumSByte), typeof(MyEnumShort), typeof(MyEnumInt), 
			typeof(MyEnumLong), typeof(MyEnumUShort), typeof(MyEnumUInt), typeof(MyEnumULong)};
   Object oValue;
   Object[] oArrValues = {(Byte)5, (SByte)5, (Int16)5, 5, (Int64)5, (UInt16)5, (UInt32)5, (UInt64)5};
   ArrayList alst;
   ArrayList alstBackup;
   SpecialFlags spen1;
   NegativeEnum ngEn1;
   NegativeEnumWithFlags gnEnF1;
   try {
   strLoc="Loc_7539fd";
   myn1 = MyEnum.ONE;			
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "D").Equals("1")) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "D"));
   }	
   strLoc="Loc_57234df";
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "X").Equals("00000001")) {
   iCountErrors++;
   Console.WriteLine("Err_9473fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "X"));
   }	
   i32Value = 4;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "G").Equals("FOUR")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "G"));
   }	
   i32Value = 32;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "G").Equals("32")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "G"));
   }	
   i32Value = 4;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "D").Equals("4")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "D"));
   }	
   i32Value = 4;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "X").Equals("00000004")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "X"));
   }	
   try{
   strValue = "1";
   iCountTestcases++;
   strRetValue = Enum.Format(myn1.GetType(), strValue, "G");
   iCountErrors++;
   Console.WriteLine("Err_758sff! Unexpected exception! This wasn't working before");
   }catch{
   }
   try{
   strValue = "ONE";
   iCountTestcases++;
   strRetValue = Enum.Format(myn1.GetType(), strValue, "G");
   iCountErrors++;
   Console.WriteLine("Err_7352sdg! Unexpected exception! This wasn't working before");
   }catch{
   }
   myn1 = MyEnum.ONE | MyEnum.TWO;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "g").Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "g"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "d").Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "d"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "x").Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_120753fdf! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "x"));
   }	
   try{
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "f").Equals("ONE, TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_120753fdf! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "f"));
   }	
   }catch{
   }
   i32Value = 3;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "G").Equals("3")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "G"));
   }	
   i32Value = 3;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "D").Equals("3")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "D"));
   }	
   i32Value = 3;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "X").Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "X"));
   }	
   try{
   i32Value = 3;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), i32Value, "f").Equals("ONE, TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_93752fsdg! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), i32Value, "f"));
   }	
   }catch{
   }
   strLoc="Loc_412ds";
   myn1 = MyEnum.ONE & MyEnum.TWO;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "G").Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "D").Equals((1 & 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "X").Equals("00000000")) {
   iCountErrors++;
   Console.WriteLine("Err_109753vdf! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "X"));
   }	
   myn1 = MyEnum.FOUR ^ MyEnum.EIGHT;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "G").Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "D").Equals((4 ^ 8).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "X").Equals("0000000C")) {
   iCountErrors++;
   Console.WriteLine("Err_2193457fvd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "X"));
   }	
   myn1 = ~MyEnum.FOUR;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "G").Equals((~4).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "D").Equals((~4).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "X").Equals("FFFFFFFB")) {
   iCountErrors++;
   Console.WriteLine("Err_12935fdvf! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "X"));
   }	
   strLoc="Loc_429ds";
   myn1 = MyEnum.FOUR;
   myn1++;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "G").Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "D").Equals((4+1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "X").Equals("00000005")) {
   iCountErrors++;
   Console.WriteLine("Err_324fdsr! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "X"));
   }	
   strLoc="Loc_5423ecsd";
   myn1 = MyEnum.FOUR;
   myn1--;
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "G").Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "D").Equals((4-1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn1.GetType(), myn1, "X").Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_48dfvbd! Expected value wasn't returned, " + Enum.Format(myn1.GetType(), myn1, "X"));
   }	
   strLoc="Loc_4562fewd";
   myn2 = MyEnumFlags.ONE | MyEnumFlags.TWO;
   iCountTestcases++;
   if(!Enum.Format(myn2.GetType(), myn2, "G").Equals("ONE, TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myn2.GetType(), myn2, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn2.GetType(), myn2, "D").Equals((1 | 2).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myn2.GetType(), myn2, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myn2.GetType(), myn2, "x").Equals("00000003")) {
   iCountErrors++;
   Console.WriteLine("Err_39527fvdg! Expected value wasn't returned, " + Enum.Format(myn2.GetType(), myn2, "x"));
   }	
   strLoc="Loc_53fd";
   myen_b = MyEnumByte.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_b.GetType(), myen_b, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_b.GetType(), myen_b, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_b.GetType(), myen_b, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_b.GetType(), myen_b, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_b.GetType(), myen_b, "X").Equals("01")) {
   iCountErrors++;
   Console.WriteLine("Err_233057fvdg! Expected value wasn't returned, " + Enum.Format(myen_b.GetType(), myen_b, "X"));
   }	
   myen_sb = MyEnumSByte.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_sb.GetType(), myen_sb, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_sb.GetType(), myen_sb, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_sb.GetType(), myen_sb, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_sb.GetType(), myen_sb, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_sb.GetType(), myen_sb, "x").Equals("01")) {
   iCountErrors++;
   Console.WriteLine("Err_23507dfvs! Expected value wasn't returned, " + Enum.Format(myen_sb.GetType(), myen_sb, "x"));
   }	
   myen_sob = MyEnumShort.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_sob.GetType(), myen_sob, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_sob.GetType(), myen_sob, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_sob.GetType(), myen_sob, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_sob.GetType(), myen_sob, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_sob.GetType(), myen_sob, "X").Equals("0001")) {
   iCountErrors++;
   Console.WriteLine("Err_32247vdg! Expected value wasn't returned, " + Enum.Format(myen_sob.GetType(), myen_sob, "X"));
   }	
   myen_i = MyEnumInt.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_i.GetType(), myen_i, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_i.GetType(), myen_i, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_i.GetType(), myen_i, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_i.GetType(), myen_i, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_i.GetType(), myen_i, "x").Equals("00000001")) {
   iCountErrors++;
   Console.WriteLine("Err_20357fdg! Expected value wasn't returned, " + Enum.Format(myen_i.GetType(), myen_i, "x"));
   }	
   myen_l = MyEnumLong.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_l.GetType(), myen_l, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_l.GetType(), myen_l, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_l.GetType(), myen_l, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_l.GetType(), myen_l, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_l.GetType(), myen_l, "x").Equals("0000000000000001")) {
   iCountErrors++;
   Console.WriteLine("Err_21053bdg! Expected value wasn't returned, " + Enum.Format(myen_l.GetType(), myen_l, "x"));
   }	
   myen_us = MyEnumUShort.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_us.GetType(), myen_us, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_us.GetType(), myen_us, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_us.GetType(), myen_us, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_us.GetType(), myen_us, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_us.GetType(), myen_us, "x").Equals("0001")) {
   iCountErrors++;
   Console.WriteLine("Err_2q0753fg! Expected value wasn't returned, " + Enum.Format(myen_us.GetType(), myen_us, "x"));
   }	
   myen_ui = MyEnumUInt.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_ui.GetType(), myen_ui, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_ui.GetType(), myen_ui, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_ui.GetType(), myen_ui, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_ui.GetType(), myen_ui, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_ui.GetType(), myen_ui, "x").Equals("00000001")) {
   iCountErrors++;
   Console.WriteLine("Err_107453fbdg! Expected value wasn't returned, " + Enum.Format(myen_ui.GetType(), myen_ui, "x"));
   }	
   myen_ul = MyEnumULong.ONE;
   iCountTestcases++;
   if(!Enum.Format(myen_ul.GetType(), myen_ul, "G").Equals("ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(myen_ul.GetType(), myen_ul, "G"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_ul.GetType(), myen_ul, "D").Equals((1).ToString())) {
   iCountErrors++;
   Console.WriteLine("Err_12357gsdd! Expected value wasn't returned, " + Enum.Format(myen_ul.GetType(), myen_ul, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(myen_ul.GetType(), myen_ul, "x").Equals("0000000000000001")) {
   iCountErrors++;
   Console.WriteLine("Err_10742dvggfh! Expected value wasn't returned, " + Enum.Format(myen_ul.GetType(), myen_ul, "x"));
   }	
   strLoc="Loc_1639dvs";
   alst = new ArrayList();
   for(int i=0; i<oArrValues.Length;i++){
   alst.Add(oArrValues[i]);
   }
   for(int i=0;i<tpArrEnums.Length;i++){
   alstBackup = (ArrayList)alst.Clone();
   oValue = alstBackup[i];
   iCountTestcases++;
   if(!Enum.Format(tpArrEnums[i], oValue, "D").Equals("5")) {
   iCountErrors++;
   Console.WriteLine("Err_2495gd,! Expected value wasn't returned, " + Enum.Format(tpArrEnums[i], oValue, "D"));
   }	
   alstBackup.RemoveAt(i);
   for(int j=0; j<alstBackup.Count; j++){
   try {
   iCountTestcases++;
   Enum.Format(tpArrEnums[i], alstBackup[j], "G");
   iCountErrors++;
   Console.WriteLine("Err_4325fds! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   }
   }
   try {
   iCountTestcases++;
   Enum.Format(null, 5, "G");
   iCountErrors++;
   Console.WriteLine("Err_4325fds! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Format(typeof(Int32), 5, "G");
   iCountErrors++;
   Console.WriteLine("Err_4325fds! Exception not thrown");
   }catch(ArgumentException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Format(typeof(MyEnumUInt), null, "G");
   iCountErrors++;
   Console.WriteLine("Err_4325fds! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Format(typeof(MyEnumInt), 5, null);
   iCountErrors++;
   Console.WriteLine("Err_4325fds! Exception not thrown");
   }catch(ArgumentNullException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }
   try {
   iCountTestcases++;
   Enum.Format(typeof(MyEnumInt), 5, "M");
   iCountErrors++;
   Console.WriteLine("Err_4325fds! Exception not thrown");
   }catch(System.FormatException){
   }catch(Exception ex){
   iCountErrors++;
   Console.WriteLine("Err_9823gdf! wrong Exception thrown, " + ex);
   }												
   spen1 = SpecialFlags.ZERO;
   iCountTestcases++;
   if(!Enum.Format(spen1.GetType(), spen1, "G").Equals("ZERO")) {
   iCountErrors++;
   Console.WriteLine("Err_752vgf! Expected value wasn't returned, " + Enum.Format(spen1.GetType(), spen1, "G").Equals("ZERO"));
   }	
   iCountTestcases++;
   if(!Enum.Format(spen1.GetType(), spen1, "D").Equals("0")) {
   iCountErrors++;
   Console.WriteLine("Err_752vgf! Expected value wasn't returned, " + Enum.Format(spen1.GetType(), spen1, "D"));
   }	
   iCountTestcases++;
   if(!Enum.Format(spen1.GetType(), spen1, "X").Equals("00")) {
   iCountErrors++;
   Console.WriteLine("Err_752vgf! Expected value wasn't returned, " + Enum.Format(spen1.GetType(), spen1, "X"));
   }	
   ngEn1 = NegativeEnum.MINUS_TWO;
   if(!Enum.Format(ngEn1.GetType(), ngEn1, "G").Equals("MINUS_TWO")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(ngEn1.GetType(), ngEn1, "G"));
   }	
   gnEnF1 = NegativeEnumWithFlags.MINUS_TWO | NegativeEnumWithFlags.MINUS_ONE;
   if(!Enum.Format(gnEnF1.GetType(), gnEnF1, "G").Equals("MINUS_ONE")) {
   iCountErrors++;
   Console.WriteLine("Err_648fd! Expected value wasn't returned, " + Enum.Format(gnEnF1.GetType(), gnEnF1, "G"));
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
   Co3900Format_TpObjStr oCbTest = new Co3900Format_TpObjStr();
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
 ONE = (sbyte)1,
   TWO = (sbyte)2,
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
 ONE = 1,
   TWO = 2,
   }
public enum MyEnumUInt:uint
{
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
   }
public enum MyEnumULong:ulong
{
 ONE	= 1,
   TWO	=	2,
   THREE	= 3
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
  public enum SpecialFlags:byte
{
 ZERO = 0,
   ONE = 1,
   }
public enum NegativeEnum
{
  MINUS_TWO = -2,
  MINUS_ONE,
  ZERO,
}
[Flags]
  public enum NegativeEnumWithFlags
{
  MINUS_TWO = -2,
  MINUS_ONE,
  ZERO,
}
