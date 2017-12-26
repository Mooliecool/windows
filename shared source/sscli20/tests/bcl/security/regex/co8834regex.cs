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
using System.Reflection.Emit;
using System.Security;
using System.Security.Permissions; 
using System.Text.RegularExpressions;
using System.Diagnostics;
public class Co8834Regex
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Security - Regex";
 public static String s_strTFName        = "Co8834Regex.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     strLoc = "Loc_9347sg";
     iCountTestcases++;
     try{				
     TestReflectionEmit(); 				
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_347tsfg! Wrong exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_3486tsdg";
     iCountTestcases++;
     try
       {				
       TestReflectionEmitIsNotPropagated();
       iCountErrors++;
       Console.WriteLine("Err_3045rtsg! No exception thrown");
       }
     catch(SecurityException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_13057sdg! Wrong exception thrown, " + ex.GetType().Name);
       }
     strLoc = "Loc_3497tsg";
     iCountTestcases++;
     try
       {				
       TestFileIOPermisssion();
       iCountErrors++;
       Console.WriteLine("Err_9347sdg! No exception thrown");
       }
     catch(SecurityException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_2076sg! Wrong exception thrown, " + ex.GetType().Name);
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
 private void TestReflectionEmit()
   {		
   ReflectionPermission perm = new ReflectionPermission(ReflectionPermissionFlag.ReflectionEmit);
   perm.Deny();
   RegexCompilationInfo[] compiles;
   AssemblyName asmName;
   compiles = new RegexCompilationInfo[1];
   compiles[0] = new RegexCompilationInfo(@"[a-z]+\d+", RegexOptions.None, "RegexPatOne", "RegexPatterns", true);
   asmName = new AssemblyName();
   asmName.Name = "RegexPatAsm1";
   Regex.CompileToAssembly(compiles, asmName);
   if(File.Exists("RegexPatAsm1.dll"))
     File.Delete("RegexPatAsm1.dll");
   }
 private void TestReflectionEmitIsNotPropagated()
   {		
   ReflectionPermission perm = new ReflectionPermission(ReflectionPermissionFlag.ReflectionEmit);
   perm.Deny();
   RegexCompilationInfo[] compiles;
   AssemblyName asmName;
   compiles = new RegexCompilationInfo[1];
   compiles[0] = new RegexCompilationInfo(@"[a-z]+\d+", RegexOptions.None, "RegexPatOne", "RegexPatterns", true);
   asmName = new AssemblyName();
   asmName.Name = "RegexPatAsm2";
   Regex.CompileToAssembly(compiles, asmName);
   if(File.Exists("RegexPatAsm2.dll"))
     File.Delete("RegexPatAsm2.dll");
   Debug.WriteLine(String.Format("Finished with regex"));
   String g = "tempfile" + Guid.NewGuid().ToString();
   AssemblyName asmname = new AssemblyName();
   asmname.Name = g;
   AssemblyBuilder asmbuild = System.Threading.Thread.GetDomain().
     DefineDynamicAssembly(asmname, AssemblyBuilderAccess.RunAndSave);   
   Debug.WriteLine(String.Format("AssemblyBuilder created, {0}", asmbuild));
   ModuleBuilder mod = asmbuild.DefineDynamicModule("Mod1", asmname.Name + ".exe" );
   Debug.WriteLine(String.Format("Module created, {0}", mod));
   throw new Exception("We shouldn't have gotten this far");
   }
 private void TestFileIOPermisssion()
   {		
   FileIOPermission perm = new FileIOPermission(PermissionState.Unrestricted);
   perm.Deny();
   RegexCompilationInfo[] compiles;
   AssemblyName asmName;
   compiles = new RegexCompilationInfo[1];
   compiles[0] = new RegexCompilationInfo(@"[a-z]+\d+", RegexOptions.None, "RegexPatOne", "RegexPatterns", true);
   asmName = new AssemblyName();
   asmName.Name = "RegexPatAsm3";
   Regex.CompileToAssembly(compiles, asmName);
   }
 public static void Main(String[] args)
   {
   Debug.Listeners.Add(new TextWriterTraceListener(Console.Out));
   bool bResult = false;
   Co8834Regex cbA = new Co8834Regex();
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
