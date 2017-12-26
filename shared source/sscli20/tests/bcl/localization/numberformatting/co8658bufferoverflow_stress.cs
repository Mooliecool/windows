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
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Collections;
public class Co8655BufferOverflow
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "NumberFormatting.BufferOverflow";
 public static String s_strTFName        = "Co8655BufferOverflow.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 const Int32 loopLimit = Int32.MaxValue;
 const Int32 loopInc   = 10;
 private Boolean debug = true;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   NumberFormatInfo info;
   Int32 i, j, k;
   Boolean loopOut=false;
   Int32 iPrevious;
   String temp;
   Object[] values = {
     Double.MinValue, Single.MinValue, Decimal.MinValue, Int64.MinValue, Int32.MinValue, Int16.MinValue, SByte.MinValue, 
     (double)(Double.MinValue+1), (double)(Double.MinValue+Double.Epsilon), (Single)(Single.MinValue+1), (Single)(Single.MinValue+Single.Epsilon), 
     (Decimal)(Decimal.MinValue+1), (Decimal)(Decimal.MinValue+Decimal.Parse("0.0000000000000000000000001")),
     (Int64)(Int64.MinValue+1), (Int32.MinValue+1), (Int16)(Int16.MinValue+1), (SByte)(SByte.MinValue+1), 
     -50, -5, -1, 0, 1, 5, 50,
     (SByte)(SByte.MaxValue-1), (Byte)(Byte.MaxValue-1), (Int16)(Int16.MaxValue-1), (UInt16)(UInt16.MaxValue-1), 
     (Int32)(Int32.MaxValue-1), (UInt32)(UInt32.MaxValue-1), 
     (Int64)(Int64.MaxValue-1), (UInt64)(UInt64.MaxValue-1), 
     (Decimal)(Decimal.MaxValue-1), (Decimal)(Decimal.MaxValue-Decimal.Parse("0.0000000000000000000000001")),
     (Single)(Single.MaxValue-1), (Single)(Single.MaxValue-Single.Epsilon),
     (Double)(Double.MaxValue-1),(double)(Double.MinValue+Double.Epsilon),
     SByte.MaxValue, Byte.MaxValue, Int16.MaxValue, UInt16.MaxValue, Int32.MaxValue, UInt32.MaxValue, Int64.MaxValue, 
     UInt64.MaxValue, Decimal.MaxValue, Single.MaxValue, Double.MaxValue,
     Single.NegativeInfinity, Single.PositiveInfinity, Single.NaN, Single.Epsilon,
     Double.NegativeInfinity, Double.PositiveInfinity, Double.NaN, Double.Epsilon,
   };
   MethodInfo method;
   Object returnObj;
   String[] formats = {
     "D", "E", "F", "G", "X",
   };
   String[] customFormats = {
     "",
     "00.##",
     "...............####......00000",
     "###,###%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
     "#####,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
     ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
     ",,,000,,#####,,,,,,%%%%%,,,,,,,,,,,,,,",
     "#00‰",
     "#00‰‰‰‰######‰‰‰‰000000‰‰‰‰",
     "e+0;00.##;##.##",
     "E+0;E-0;e+0",
     "E+0;E-0;e+0;e-0",
     "e-0",
     ";;;;;;;;;;;;;;;;;;;;;;;",
     "####\\%000",
     "\\C",
     "000\\.####",
     "ABC",
     "{{000000}}#######",
     "{{{{{{{{{{{{{{{{{{{{",
     "}}}}}}}}}}}}}}}}}}}}",
   };
   ArrayList list;
   Random random;
   Int32 customLoopSize;
   Int32 customStringSize;
   String formatString;
   Int64 ticks1;
   Int64 ticks2;
   try
     {
     ticks1 = Environment.TickCount;
     strLoc = "Loc_873425sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;		
     random = new Random();
     list = new ArrayList();
     list.Add("0");
     list.Add("#");
     list.Add(".");
     list.Add(",");
     list.Add("%");
     list.Add("E+0");
     list.Add("E-0");
     list.Add("e+0");
     list.Add("e-0");
     list.Add("\\");
     list.Add("{{");
     list.Add("}}");
     customLoopSize = random.Next(10000, 20000);
     formatString = String.Empty;
     for(i=0; i<customLoopSize; i++){
     customStringSize = random.Next(300, 1000);
     while(customStringSize>formatString.Length){
     temp = (String)list[random.Next(list.Count)];
     switch(temp){
     case "0":
       for(j=0;j<100;j++)
	 formatString += temp;
       break;
     default:
       formatString += temp;
       break;
     }
     }
     if(debug)
       Console.WriteLine("formatString: <{0}>", formatString);
     foreach(Object value in values){
     Console.WriteLine(value);
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{formatString});
     }catch{
     }
     }
     }
     strLoc = "Loc_8374sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;			
     foreach(String format in formats){
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{format});
     }catch{
     }
     for(i = 0; i<100; i++){
     temp = format + i.ToString();
     try{    						
     returnObj = method.Invoke(value, new Object[]{temp});
     }catch{
     }
     }
     }
     }
     strLoc = "Loc_8374sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=0; i<100; i++){    				
     info.CurrencyDecimalDigits = i;    					
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_9347tsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     Console.WriteLine("i: {0}", i);
     try{
     info.CurrencyDecimalSeparator = new String('a', i);
     foreach(Object value in values){
     Console.WriteLine(value);
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     Console.WriteLine(format);
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_034tsdg! Exception thrown for CurrencyDecimalSeparator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_8365tsfg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_2974tsgf";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     Console.WriteLine("i:{0}", i);
     info.CurrencyGroupSeparator = new String('a', i);
     foreach(Object value in values){
     Console.WriteLine(value);
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     Console.WriteLine(format);
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_9347tsdg! Exception thrown for CurrencyGroupSeparator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_836tg! Wrong exception thrown, " + ex.GetType().Name);
     }    		
     strLoc = "Loc_21076tsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=1; i<10; i++){
     for(j=1; j<10; j++){
     for(k=0; k<10; k++){
     Console.WriteLine("i:{0} j:{1} k:{2}", i, j, k);
     info.CurrencyGroupSizes = new Int32[]{i, j, k};
     foreach(Object value in values){
     Console.WriteLine(value);
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_93476tsfg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_2467sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=0; i<16; i++){
     info.CurrencyNegativePattern = i;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_93745tsdg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_9357tsdfg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=0; i<4; i++){
     info.CurrencyPositivePattern = i;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_1074tsg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_73406t879sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     Console.WriteLine("i: {0}", i);
     try{
     info.CurrencySymbol = new String('a', i);
     foreach(Object value in values){
     Console.WriteLine(value);
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     Console.WriteLine(format);
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_0347tsdfg! Exception thrown for CurrencySymbol at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_39476sfg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_02745rsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     info.CurrencyDecimalDigits = 99;
     iPrevious = 100;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.CurrencyDecimalSeparator = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.CurrencyDecimalSeparator = new String('a', iPrevious);
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.CurrencyGroupSeparator = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.CurrencyGroupSeparator = new String('a', iPrevious);
     info.CurrencyGroupSizes = new Int32[]{1, 1, 1};
     info.CurrencyNegativePattern = 3;
     info.CurrencyPositivePattern = 1;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.CurrencySymbol = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.CurrencySymbol = new String('a', iPrevious);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_324sdg! Wrong exception thrown, " + ex.GetType().Name);			
     }
     strLoc = "Loc_49724tsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.NaNSymbol = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_93467tsdg! Exception thrown for NaNSymbol at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_93475fsg! Exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_39567sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.NegativeInfinitySymbol = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_09347tsdg! Exception thrown for NaNSymbol at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_93475fsg! Exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_520-375wdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     info.CurrencyNegativePattern = 1;
     info.NumberNegativePattern = 1;
     info.PercentNegativePattern = 1;
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.NegativeSign = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     try{    						
     returnObj = method.Invoke(value, new Object[]{"N", info});
     }catch{
     }
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					    					
     }catch(Exception e){
     Console.WriteLine("Loc_02375rsdg! Exception thrown for NegativeSign at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_93475fsg! Exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_602734sfdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=0; i<100; i++){    				
     try{
     info.NumberDecimalDigits = i;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"N", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_9723sdg! Exception thrown for NumberDecimalDigits at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_20745gs! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_70247t9sdfg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.NumberDecimalSeparator = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"N", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_90375trsfg! Exception thrown for NumberDecimalSeparator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_30745trdfg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_87049trsdfg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.NumberGroupSeparator = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"N", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_30947tsdfg! Exception thrown for NumberGroupSeparator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_4902735radg! Wrong exception thrown, " + ex.GetType().Name);
     }    		
     strLoc = "Loc_97438trsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=1; i<10; i++){
     for(j=1; j<10; j++){
     for(k=0; k<10; k++){
     info.NumberGroupSizes = new Int32[]{i, j, k};
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"N", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_974325sg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_0487tsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=0; i<5; i++){
     info.NumberNegativePattern = i;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"N", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_5290345radg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_21973rsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     info.NumberDecimalDigits = 99;
     iPrevious = 100;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.NumberDecimalSeparator = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.NumberDecimalSeparator = new String('a', iPrevious);
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.NumberGroupSeparator = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.NumberGroupSeparator = new String('a', iPrevious);
     info.NumberGroupSizes = new Int32[]{1, 1, 1};
     info.NumberNegativePattern = 3;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"N", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_324sdg! Wrong exception thrown, " + ex.GetType().Name);			
     }
     strLoc = "Loc_1-45wtsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=0; i<100; i++){    				
     try{
     info.PercentDecimalDigits = i;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_9347tsdfg! Exception thrown for PercentDecimalDigits at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_623058dag! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_209437tsfg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PercentDecimalSeparator = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_34907wsg! Exception thrown for PercentDecimalSeparator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_723957rsdg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_309723fdsagk";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PercentGroupSeparator = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }
     }catch(Exception e){
     Console.WriteLine("Loc_8346tsfdg! Exception thrown for PercentGroupSeparator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_39457dsg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_409237fadg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=1; i<10; i++){
     for(j=1; j<10; j++){
     for(k=0; k<10; k++){
     info.PercentGroupSizes = new Int32[]{i, j, k};
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_974325sg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_09324sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=0; i<3; i++){
     info.PercentNegativePattern = i;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_842375sdg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_903475tsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     for(i=0; i<3; i++){
     info.PercentPositivePattern = i;
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_924sdg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_8947sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PercentSymbol = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"#‰", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_9247sdg! Exception thrown for PercentSymbol at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_0275rdag! Wrong exception thrown, " + ex.GetType().Name);
     }    	
     strLoc = "Loc_1497tfsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     info.PercentDecimalDigits = 99;
     iPrevious = 100;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PercentDecimalSeparator = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.PercentDecimalSeparator = new String('a', iPrevious);
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PercentGroupSeparator = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.PercentGroupSeparator = new String('a', iPrevious);
     info.PercentGroupSizes = new Int32[]{1, 1, 1};
     info.PercentNegativePattern = 0;
     info.PercentPositivePattern = 0;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PercentSymbol = new String('a', i);
     iPrevious = i;
     }catch(Exception){
     loopOut = true;
     }
     }				
     info.PercentSymbol = new String('a', iPrevious);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"P", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_324sdg! Wrong exception thrown, " + ex.GetType().Name);			
     }
     strLoc = "Loc_94785tsdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PositiveInfinitySymbol = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_987ergsfg! Exception thrown for NaNSymbol at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_93475fsg! Exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_07245twrdsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;
     try{
     info = new NumberFormatInfo();
     loopOut=false;
     for(i=1; (i<loopLimit && !loopOut); i*=loopInc){
     try{
     info.PositiveSign = new String('a', i);
     foreach(Object value in values){
     method = value.GetType().GetMethod("ToString", new Type[]{typeof(String), typeof(IFormatProvider)});
     try{    						
     returnObj = method.Invoke(value, new Object[]{"C", info});
     }catch{
     }
     foreach(String format in customFormats){
     try{    						
     returnObj = method.Invoke(value, new Object[]{format, info});
     }catch{
     }
     }
     }    					
     }catch(Exception e){
     Console.WriteLine("Loc_089sdgsf! Exception thrown for NaNSymbol at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }     			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_93475fsg! Exception thrown, " + ex.GetType().Name);
     }
     ticks2 = Environment.TickCount;
     Console.WriteLine("Time taken to run this code, {0} minutes ", (ticks2-ticks1)/60000);
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
   Co8655BufferOverflow cbA = new Co8655BufferOverflow();
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
