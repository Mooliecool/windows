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
using System.IO; 
using System;
using System.IO;
using System.Reflection;
using System.Collections;
delegate void dlgMethod1();
delegate Int32 dlgMethod2(Int32 iValue);
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
public class Co3871op_ComparisonOperators : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Delegate);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("op_Inequality", new Type[]{typeof(Delegate), typeof(Delegate)}));
   list.Add(type.GetMethod("op_Equality", new Type[]{typeof(Delegate), typeof(Delegate)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public void Co3871_1()
   {
   }
 public void Co3871_2()
   {
   }
 public Int32 Co3871_3(Int32 i)
   {
   return ++i;
   }
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Delegate.op_GreaterThan(Delegate, Delegate)";
 public static String s_strTFName        = "Co3871op_ComparisonOperators";
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
   Delegate dlg1;
   Delegate dlg2;
   dlgMethod1 MyMethod1;
   dlgMethod1 MyMethod2;
   try
     {
     Co3871op_ComparisonOperators Co3871 = new Co3871op_ComparisonOperators();
     MyMethod1 = new dlgMethod1(Co3871.Co3871_1);
     MyMethod2 = new dlgMethod1(Co3871.Co3871_1);
     iCountTestcases++;
     if(MyMethod1!=MyMethod2){
     iCountErrors++;
     Console.WriteLine("Err_753cds! op_GreaterThan giving problems here");
     }
     dlg1 = new dlgMethod1(Co3871.Co3871_1);
     dlg2 = new dlgMethod1(Co3871.Co3871_1);
     iCountTestcases++;
     if(dlg1!=dlg2){
     iCountErrors++;
     Console.WriteLine("Err_653fcd! op_GreaterThan giving problems here");
     }
     dlg1 = new dlgMethod1(Co3871.Co3871_1);
     dlg2 = new dlgMethod1(Co3871.Co3871_2);
     iCountTestcases++;
     if(dlg1==dlg2){
     iCountErrors++;
     Console.WriteLine("Err_9345dxs! op_GreaterThan giving problems here");
     }
     dlg1 = new dlgMethod1(Co3871.Co3871_1);
     dlg2 = new dlgMethod2(Co3871.Co3871_3);
     iCountTestcases++;
     if(dlg1==dlg2){
     iCountErrors++;
     Console.WriteLine("Err_105xds! op_GreaterThan giving problems here");
     }
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3871op_ComparisonOperators oCbTest = new Co3871op_ComparisonOperators();
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
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
