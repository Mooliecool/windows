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
using System.Reflection;
using System.Collections;
using System.Threading;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co3868op_ComparisonOperators : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(TimeSpan);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("op_GreaterThan", new Type[]{typeof(TimeSpan), typeof(TimeSpan)}));
   list.Add(type.GetMethod("op_GreaterThanOrEqual", new Type[]{typeof(TimeSpan), typeof(TimeSpan)}));
   list.Add(type.GetMethod("op_LessThan", new Type[]{typeof(TimeSpan), typeof(TimeSpan)}));
   list.Add(type.GetMethod("op_LessThanOrEqual", new Type[]{typeof(TimeSpan), typeof(TimeSpan)}));
   list.Add(type.GetMethod("op_Inequality", new Type[]{typeof(TimeSpan), typeof(TimeSpan)}));
   list.Add(type.GetMethod("op_Equality", new Type[]{typeof(TimeSpan), typeof(TimeSpan)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "TimeSpan.op_GreaterThan(TimeSpan, TimeSpan)";
 public static String s_strTFName        = "Co3868op_ComparisonOperators.cs";
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
   TimeSpan ts1;
   TimeSpan ts2;
   try
     {
     ts1 = TimeSpan.MinValue;
     ts2 = TimeSpan.MaxValue;							
     iCountTestcases++;
     if(ts1>ts2){
     iCountErrors++;
     Console.WriteLine("Loc_653fd! op_GreaterThan giving problems here, " + ts1 + " " + ts2);
     }
     iCountTestcases++;
     if(ts1>=ts2){
     iCountErrors++;
     Console.WriteLine("Loc_6753cd! op_GreaterThan giving problems here");
     }
     iCountTestcases++;
     if(ts1==ts2){
     iCountErrors++;
     Console.WriteLine("Loc_753fd! op_GreaterThan giving problems here");
     }
     ts1 = TimeSpan.MaxValue;
     ts2 = TimeSpan.MaxValue;
     iCountTestcases++;
     if(ts1!=ts2){
     iCountErrors++;
     Console.WriteLine("Loc_753cds! op_GreaterThan giving problems here");
     }
     ts1 = TimeSpan.MaxValue;
     ts2 = TimeSpan.MinValue;
     iCountTestcases++;
     if(ts1<ts2){
     iCountErrors++;
     Console.WriteLine("Loc_653cd! op_GreaterThan giving problems here");
     }
     iCountTestcases++;
     if(ts1<=ts2){
     iCountErrors++;
     Console.WriteLine("Loc_753cd! op_GreaterThan giving problems here");
     }
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3868op_ComparisonOperators oCbTest = new Co3868op_ComparisonOperators();
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
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
