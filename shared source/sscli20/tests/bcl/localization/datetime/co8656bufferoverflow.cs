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
public class Co8656BufferOverflow
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.BufferOverflow";
 public static String s_strTFName        = "Co8656BufferOverflow.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 const Int32 loopLimit = 1000;
 const Int32 loopInc   = 10;
 private Boolean debug = true;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTimeFormatInfo info;
   String[] values;
   DateTime date;
   String returnValue;
   Int32 i, j;
   Boolean loopOut;
   Int32 DTFIlimits;
   Int64 ticks1;
   Int64 ticks2;
   try
     {
     iCountTestcases++;		
     iCountTestcases++;		
     ticks1 = Environment.TickCount;
     strLoc = "Loc_873425sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;		
     try{
     info = new DateTimeFormatInfo();
     DTFIlimits = 7;
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     values = new String[DTFIlimits];
     for(j=0; j<DTFIlimits; j++)
       values[j] = new String('a', i);
     info.AbbreviatedDayNames = values;
     for(j=0; j<DTFIlimits; j++){
     date = new DateTime(2001, 4, (5+j), 21, 3, 4, 980);
     returnValue = date.ToString("ddd/MM/yyyy", info);		
     returnValue = date.ToString("R", info);
     }
     }catch(Exception e){
     Console.WriteLine("Loc_034tsdg! Exception thrown for AbbreviatedDayNames at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_02735sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     DTFIlimits = 12;
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     values = new String[DTFIlimits+1];
     for(j=0; j<DTFIlimits; j++)
       values[j] = new String('a', i);
     info.AbbreviatedMonthNames  = values;
     for(j=0; j<DTFIlimits; j++){
     date = new DateTime(2001, (1+j), 5, 21, 3, 4, 980);
     returnValue = date.ToString("dd/MMM/yyyy", info);		
     returnValue = date.ToString("R", info);
     }
     }catch(Exception e){
     Console.WriteLine("Loc_034tsdg! Exception thrown for AbbreviatedMonthNames at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_2046tsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     info.LongTimePattern = "h:mm:ss tt";
     info.ShortTimePattern = "h:mm tt";
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.AMDesignator  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("f", info);		
     returnValue = date.ToString("F", info);		
     returnValue = date.ToString("g", info);		
     returnValue = date.ToString("G", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for AMDesignator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_2046tsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.DateSeparator  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("d", info);		
     returnValue = date.ToString("g", info);		
     returnValue = date.ToString("G", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for AMDesignator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_2047fgs";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;		
     try{
     info = new DateTimeFormatInfo();
     DTFIlimits = 7;
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     values = new String[DTFIlimits];
     for(j=0; j<DTFIlimits; j++)
       values[j] = new String('a', i);
     info.DayNames = values;
     for(j=0; j<DTFIlimits; j++){
     date = new DateTime(2001, 4, (5+j), 21, 3, 4, 980);
     returnValue = date.ToString("dddd/MM/yyyy", info);		
     returnValue = date.ToString("D", info);		
     returnValue = date.ToString("U", info);		
     }
     }catch(Exception e){
     Console.WriteLine("Loc_034tsdg! Exception thrown for AbbreviatedDayNames at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_927sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.FullDateTimePattern  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("F", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for FullDateTimePattern at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_927sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.LongDatePattern  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("D", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for LongDatePattern at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_043257sg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info = new DateTimeFormatInfo();
     info.LongTimePattern  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("F", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for LongTimePattern at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }    			
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_84sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.MonthDayPattern  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("M", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for MonthDayPattern at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_02735sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     DTFIlimits = 12;
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     values = new String[DTFIlimits+1];
     for(j=0; j<DTFIlimits; j++)
       values[j] = new String('a', i);
     info.MonthNames  = values;
     for(j=0; j<DTFIlimits; j++){
     date = new DateTime(2001, (1+j), 5, 21, 3, 4, 980);
     returnValue = date.ToString("dd/MMMM/yyyy", info);		
     returnValue = date.ToString("M", info);
     returnValue = date.ToString("f", info);
     returnValue = date.ToString("D", info);
     returnValue = date.ToString("F", info);
     returnValue = date.ToString("Y", info);
     }
     }catch(Exception e){
     Console.WriteLine("Loc_034tsdg! Exception thrown for AbbreviatedMonthNames at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_2046tsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     info.LongTimePattern = "h:mm:ss tt";
     info.ShortTimePattern = "h:mm tt";
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.PMDesignator  = new String('a', i);
     date = new DateTime(2001, 1, 5, 15, 3, 4, 980);
     returnValue = date.ToString("f", info);		
     returnValue = date.ToString("F", info);		
     returnValue = date.ToString("g", info);		
     returnValue = date.ToString("G", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for PMDesignator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_39476sg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info = new DateTimeFormatInfo();
     info.ShortDatePattern  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("d", info);	
     returnValue = date.ToString("g", info);	
     returnValue = date.ToString("G", info);	
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for ShortDatePattern at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_39476sg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info = new DateTimeFormatInfo();
     info.ShortTimePattern  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("f", info);	
     returnValue = date.ToString("g", info);	
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for ShortDatePattern at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_2046tsg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.TimeSeparator  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("f", info);		
     returnValue = date.ToString("F", info);		
     returnValue = date.ToString("g", info);		
     returnValue = date.ToString("G", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for AMDesignator at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     strLoc = "Loc_84sdg";
     if(debug)
       Console.WriteLine(strLoc);
     iCountTestcases++;					
     try{
     info = new DateTimeFormatInfo();
     loopOut=false;
     for(i=0; (i<loopLimit && !loopOut); i+=loopInc){
     try{
     info.YearMonthPattern  = new String('a', i);
     date = new DateTime(2001, 1, 5, 5, 3, 4, 980);
     returnValue = date.ToString("Y", info);		
     }catch(Exception e){
     Console.WriteLine("Loc_23745sdg! Exception thrown for MonthDayPattern at {0}, Type {1}", i, e.GetType().Name);
     loopOut = true;
     }
     }
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_9374tsg! Wrong exception thrown, " + ex.GetType().Name);
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
   Co8656BufferOverflow cbA = new Co8656BufferOverflow();
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
