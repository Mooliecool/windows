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
using System.Reflection;
using System.Diagnostics;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;
public class Co8518GetCommandLineArgs
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Environment.GetCommandLineArgs()";
 public static String s_strTFName        = "Co8518GetCommandLineArgs.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String[] cmdArgs;
   String cmdLine;
   Process p;
   try
     {
     strLoc = "Loc_8234dgs";
     iCountTestcases++;
     cmdArgs = new String[0];
     CreateTestFile(cmdArgs);
     BuildFile ("test.cs");
     p = Process.Start ("test.exe");
     p.WaitForExit (2000000);
     if (p.ExitCode != 0)
       {
       iCountErrors++;
       Console.WriteLine("Err_8745sgf! unexpected value returnes, " + p.ExitCode);
       }
     FileDelete();
     strLoc = "Loc_9245sdg";
     iCountTestcases++;
     cmdArgs = new String[1];
     cmdArgs[0] = "ThisIsAPublicTest";
     cmdLine = String.Empty;
     for(int i=0; i<cmdArgs.Length; i++)
       cmdLine += cmdArgs[i] + " ";
     CreateTestFile(cmdArgs);
     BuildFile ("test.cs");
     p = Process.Start ("test.exe", cmdLine);
     p.WaitForExit (2000000);
     if (p.ExitCode != 0)
       {
       iCountErrors++;
       Console.WriteLine("Err_2903475tdg! unexpected value returnes, " + p.ExitCode);
       }
     FileDelete();
     strLoc = "Loc_3986tsdg";
     iCountTestcases++;
     cmdArgs = new String[2];
     cmdArgs[0] = "Hello";
     cmdArgs[1] = "World";
     cmdLine = String.Empty;
     for(int i=0; i<cmdArgs.Length; i++)
       cmdLine += cmdArgs[i] + " ";
     CreateTestFile(cmdArgs);
     BuildFile ("test.cs");
     p = Process.Start ("test.exe", cmdLine);
     p.WaitForExit (2000000);
     if (p.ExitCode != 0)
       {
       iCountErrors++;
       Console.WriteLine("Err_9347sg! unexpected value returnes, " + p.ExitCode);
       }
     FileDelete();
     strLoc = "Loc_9374sg";
     iCountTestcases++;
     cmdArgs = new String[10];
     for(int i=0; i<cmdArgs.Length; i++)
       cmdArgs[i] = i.ToString();
     cmdLine = String.Empty;
     for(int i=0; i<cmdArgs.Length; i++)
       cmdLine += cmdArgs[i] + " ";
     CreateTestFile(cmdArgs);
     BuildFile ("test.cs");
     p = Process.Start ("test.exe", cmdLine);
     p.WaitForExit (2000000);
     if (p.ExitCode != 0)
       {
       iCountErrors++;
       Console.WriteLine("Err_9347sg! unexpected value returnes, " + p.ExitCode);
       }
     FileDelete();
     strLoc = "Loc_3967tsg";
     iCountTestcases++;
     cmdArgs = new String[100];
     for(int i=0; i<cmdArgs.Length; i++)
       cmdArgs[i] = i.ToString();
     cmdLine = String.Empty;
     for(int i=0; i<cmdArgs.Length; i++)
       cmdLine += cmdArgs[i] + " ";
     CreateTestFile(cmdArgs);
     BuildFile ("test.cs");
     p = Process.Start ("test.exe", cmdLine);
     p.WaitForExit (2000000);
     if (p.ExitCode != 0)
       {
       iCountErrors++;
       Console.WriteLine("Err_94327sg! unexpected value returnes, " + p.ExitCode);
       }
     FileDelete();
     iCountTestcases++;
     {
     strLoc = "Loc_825sdg";
     cmdArgs = new String[500];
     for(int i=0; i<cmdArgs.Length; i++)
       cmdArgs[i] = i.ToString();
     cmdLine = String.Empty;
     for(int i=0; i<cmdArgs.Length; i++)
       cmdLine += cmdArgs[i] + " ";
     Console.WriteLine(cmdLine);
     CreateTestFile(cmdArgs);
     BuildFile ("test.cs");
     p = Process.Start ("test.exe", cmdLine);
     p.WaitForExit (2000000);
     if (p.ExitCode != 0)
       {
       iCountErrors++;
       Console.WriteLine("Err_923745sdgg! unexpected value returnes, " + p.ExitCode);
       }
     FileDelete();
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
   Co8518GetCommandLineArgs cbA = new Co8518GetCommandLineArgs();
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
 void CreateTestFile(String[] values)
   {
   StreamWriter sw = new StreamWriter("test.cs");
   CSharpCodeProvider cdp = new CSharpCodeProvider();
   ICodeGenerator cg = cdp.CreateGenerator();
   CodeNamespace cnspace = new CodeNamespace("N");
   cnspace.Imports.Add(new CodeNamespaceImport("System"));
   CodeTypeDeclaration co = new CodeTypeDeclaration ("C");
   co.IsClass = true;
   cnspace.Types.Add (co);
   co.TypeAttributes  = TypeAttributes.Public;
   CodeMemberMethod cmm = new CodeMemberMethod();
   cmm.Name = "Main";
   cmm.ReturnType = null;
   cmm.Attributes = MemberAttributes.Public | MemberAttributes.Static;
   CodeVariableDeclarationStatement cvar = new CodeVariableDeclarationStatement(typeof(String[]), "args", new CodeMethodInvokeExpression(new CodeTypeReferenceExpression("Environment"), "GetCommandLineArgs"));
   cmm.Statements.Add(cvar);  
   cvar = new CodeVariableDeclarationStatement(typeof(Int32), "exitCode", new CodeSnippetExpression("0"));
   cmm.Statements.Add(cvar);
   String strArgLength = (values.Length + 1).ToString();
   CodeConditionStatement ccs1 = new CodeConditionStatement(new CodeBinaryOperatorExpression(new CodeVariableReferenceExpression("args.Length"), CodeBinaryOperatorType.IdentityInequality, new CodeSnippetExpression(strArgLength)), new CodeStatement []{new CodeAssignStatement(new CodeVariableReferenceExpression("exitCode"), new CodeBinaryOperatorExpression(new CodeVariableReferenceExpression("exitCode"), CodeBinaryOperatorType.Add, new CodeSnippetExpression("1")))});
   cmm.Statements.Add(ccs1);
   ccs1 = new CodeConditionStatement(new CodeSnippetExpression("!args[0].ToLower().EndsWith(\"test.exe\")"), new CodeStatement []{new CodeAssignStatement(new CodeVariableReferenceExpression("exitCode"), new CodeBinaryOperatorExpression(new CodeVariableReferenceExpression("exitCode"), CodeBinaryOperatorType.Add, new CodeSnippetExpression("1")))});
   cmm.Statements.Add(ccs1);
   for(int i=0; i<values.Length; i++){
   ccs1 = new CodeConditionStatement(new CodeBinaryOperatorExpression(new CodeVariableReferenceExpression("args[" + (i+1).ToString() + "]"), CodeBinaryOperatorType.IdentityInequality, new CodePrimitiveExpression(values[i])), new CodeStatement []{new CodeAssignStatement(new CodeVariableReferenceExpression("exitCode"), new CodeBinaryOperatorExpression(new CodeVariableReferenceExpression("exitCode"), CodeBinaryOperatorType.Add, new CodeSnippetExpression("1")))});
   cmm.Statements.Add(ccs1);
   }
   cmm.Statements.Add(new CodeAssignStatement(new CodeVariableReferenceExpression("Environment.ExitCode"), new CodeVariableReferenceExpression("exitCode")));
   co.Members.Add(cmm);
   cg.GenerateCodeFromNamespace(cnspace, sw, null);
   sw.Flush();
   sw.Close();		
   }
 public static Boolean BuildFile (String strFileName)
   {
   Boolean bBuildOk = false;
   Process p;
#if !PLATFORM_UNIX
   string compilerName = "csc.exe";
#else 
   string compilerName = "csc";
#endif 
   p = Process.Start (compilerName,strFileName);
   if (p.WaitForExit (2000000) == true)
     {
     if (p.ExitCode == 0)
       {
       bBuildOk = true;
       }	
     else
       {
       bBuildOk = false;
       }
     }
   else
     {
     p.Kill();
     bBuildOk = false;
     }		
   return bBuildOk;
   }
 private void FileDelete()
   {
   if(File.Exists("test.cs"))
     File.Delete("test.cs");
   if(File.Exists("test.exe"))
     File.Delete("test.exe");
   }
}
