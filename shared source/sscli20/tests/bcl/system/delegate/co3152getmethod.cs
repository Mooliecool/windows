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
using System.Text;
using System;
using System.Collections;
using System.Reflection;
public delegate void Co3152_dlg_1();
public delegate int Co3152_dlg_3(int i);
public delegate int Co3152_dlg_4(int i, short  j);
public delegate void Co3152_dlgmc_1();
public class Co3152GetMethod
{
 public Co3152_dlgmc_1 MCHandler = null;
 public virtual void method1() {}
 public virtual int method1(int i) {return 0;}
 public virtual int method2(int i) {return 0;}
 public virtual void method3(int i) {}
 public virtual void method3() {}
 public virtual int method4(int i, short j) {return (i+(int)j);}
 public static int method5(int i) {return 0;}
 public virtual void AddToDelegate(Co3152_dlgmc_1 _handler)
   {
   MCHandler = (Co3152_dlgmc_1) Delegate.Combine(MCHandler, _handler);
   }
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Delegate\\Co3152GetMethod. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   Co3152GetMethod cb1 = new Co3152GetMethod();
   Co3152GetMethod cb2 = new Co3152GetMethod();
   Co3152_dlg_1 dlg1 = null;
   Co3152_dlg_1 dlg2 = null;
   Co3152_dlg_3 dlg3 = null;
   Co3152_dlg_4 dlg4 = null;
   Co3152_dlgmc_1 mcDlg1a = null;
   Co3152_dlgmc_1 mcDlg1b = null;
   dlg3 = new Co3152_dlg_3(Co3152GetMethod.method5);
   iCountTestcases++;
   try {
   if(!"method5".Equals(dlg3.Method.Name ))
     {
     iCountErrors++;
     print("E_47hq");
     }
   iCountTestcases++;
   if(!"Co3152GetMethod".Equals(dlg3.Method.DeclaringType.ToString()))
     {
     iCountErrors++;
     print("E_791n");
     }
   if(!"Int32 method5(Int32)".Equals(dlg3.Method.ToString()))
     {
     iCountErrors++;
     print("E_34hq Expected: Int32 method5 (Int32); Got: " + dlg3.Method.ToString());
     }
   } catch (Exception exc) {
   iCountErrors++;
   print("E_84qw");
   strError = "EXTENEDINFO: "+exc.ToString();
   Console.Error.WriteLine(strError);
   }
   dlg1 = new Co3152_dlg_1(cb1.method1);
   iCountTestcases++;
   if(!"method1".Equals(dlg1.Method.Name ))
     {
     iCountErrors++;
     print("E_239a");
     }
   iCountTestcases++;
   if(!"Co3152GetMethod".Equals(dlg1.Method.DeclaringType.ToString()))
     {
     iCountErrors++;
     print("E_94id");
     }
   iCountTestcases++;
   if("method3".Equals(dlg1.Method.Name ))
     {
     iCountErrors++;
     print("E_39dn");
     }
   iCountTestcases++;
   if(!"Void method1()".Equals(dlg1.Method.ToString()))
     {
     iCountErrors++;
     print("E_741f! got=="+dlg1.Method.ToString());
     }
   Console.WriteLine(dlg1.Method.Name);
   Console.WriteLine(dlg1.Method.ToString());
   dlg3 = new Co3152_dlg_3(cb1.method1);
   iCountTestcases++;
   if(!"method1".Equals(dlg3.Method.Name ))
     {
     iCountErrors++;
     print("E_20ka");
     }
   iCountTestcases++;
   if(!"Co3152GetMethod".Equals(dlg3.Method.DeclaringType.ToString()))
     {
     iCountErrors++;
     print("E_348i");
     }
   dlg3 = new Co3152_dlg_3(cb1.method2);
   iCountTestcases++;
   if(!"method2".Equals(dlg3.Method.Name ))
     {
     iCountErrors++;
     print("E_239a");
     }
   iCountTestcases++;
   if(!"Int32 method2(Int32)".Equals(dlg3.Method.ToString()))
     {
     iCountErrors++;
     print("E_974h");
     }
   dlg4 = new Co3152_dlg_4(cb1.method4);;
   iCountTestcases++;
   if(!"method4".Equals(dlg4.Method.Name ))
     {
     iCountErrors++;
     print("E_u893");
     }
   iCountTestcases++;
   if(!"Co3152GetMethod".Equals(dlg4.Method.DeclaringType.ToString()))
     {
     iCountErrors++;
     print("E_72jd");
     }
   iCountTestcases++;
   if(!"Int32 method4(Int32, Int16)".Equals(dlg4.Method.ToString()))
     {
     iCountErrors++;
     print("E_947hw");
     }
   dlg1 = new Co3152_dlg_1(cb1.method1);
   dlg3 = new Co3152_dlg_3(cb1.method1);
   iCountTestcases++;
   if((dlg1.Method.ToString()).Equals(dlg3.Method.ToString()))
     {
     iCountErrors++;
     print("E_93gb");
     }
   dlg1 = new Co3152_dlg_1(cb1.method1);
   dlg2 = new Co3152_dlg_1(cb1.method1);
   iCountTestcases++;
   if(!(dlg1.Method.ToString()).Equals(dlg2.Method.ToString()))
     {
     iCountErrors++;
     print("E_49xx");
     }
   mcDlg1a = new Co3152_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3152_dlgmc_1(cb1.method3);
   try {
   AddToDelegate(mcDlg1a);
   AddToDelegate(mcDlg1b); 
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_389a");
     strError = "EXTENEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   if(!"method3".Equals(MCHandler.Method.Name))
     {
     iCountErrors++;
     print("E_23ak");
     Console.WriteLine (MCHandler.Method.Name);
     Console.WriteLine (MCHandler.GetInvocationList().Length);
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3152GetMethod.)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3152GetMethod cb0 = new Co3152GetMethod();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3152GetMethod."  );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
