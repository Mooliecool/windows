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
public delegate void Co3156_dlg_1();
public delegate int Co3156_dlg_3(int i);
public delegate void Co3156_dlgmc_1();
public class Co3156GetInvocationList
{
 public Co3156_dlgmc_1 MCHandler;
 public virtual void method1() {}
 public virtual int method1(int i) {return 0;}
 public virtual int method2(int i) {return 0;}
 public virtual void method2() {}
 public virtual void method3(int i) {}
 public virtual void method3() {}
 public static void method4() {}
 public static int method4(int i) {return 0;}
 public virtual void AddToDelegate(Co3156_dlgmc_1 _handler)
   {
   MCHandler = (Co3156_dlgmc_1) Delegate.Combine(MCHandler, _handler);
   }
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Delegate\\Co3156GetInvocationList. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   Co3156GetInvocationList cb1 = new Co3156GetInvocationList();
   Co3156GetInvocationList cb2 = new Co3156GetInvocationList();
   Co3156_dlg_1 dlg1 = null;
   Co3156_dlg_1 dlg2 = null;
   Co3156_dlg_3 dlg3 = null;
   Co3156_dlgmc_1 mcDlg1a = null;
   Co3156_dlgmc_1 mcDlg1b = null;
   Co3156_dlgmc_1 mcDlg1c = null;
   Delegate[] dlgs1 = null;
   Delegate[] dlgs2 = null;
   dlgs1 = new Delegate[2];
   dlg1 = new Co3156_dlg_1(Co3156GetInvocationList.method4);
   dlgs1 = dlg1.GetInvocationList();
   iCountTestcases++;
   if(!dlgs1[0].Method.Name.ToString().Equals("method4"))
     {
     iCountErrors++;
     print("E_471h");
     }
   dlgs1 = new Delegate[2];
   dlg1 = new Co3156_dlg_1(cb1.method1);
   dlgs1 = dlg1.GetInvocationList();
   iCountTestcases++;
   if(!cb1.Equals(dlgs1[0].Target))
     {
     iCountErrors++;
     print("E_393k");
     }
   iCountTestcases++;
   if(!"method1".Equals(dlgs1[0].Method.Name ))
     {
     iCountErrors++;
     print("E_94eu");
     }
   dlgs1 = new Delegate[1];
   dlgs2 = new Delegate[1];
   dlg1 = new Co3156_dlg_1(cb1.method1);
   dlg2 = new Co3156_dlg_1(cb1.method1);
   dlgs1 = dlg1.GetInvocationList();
   dlgs2 = dlg2.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs1[0].Target).Equals(dlgs2[0].Target))
     {
     iCountErrors++;
     print("E_439n");
     }
   iCountTestcases++;
   if(!(dlgs1[0].Method.Name ).Equals(dlgs2[0].Method.Name ))
     {
     iCountErrors++;
     print("E_31jk");
     }
   dlgs1 = new Delegate[1];
   dlgs2 = new Delegate[1];
   dlg1 = new Co3156_dlg_1(cb1.method1);
   dlg2 = new Co3156_dlg_1(cb1.method3);
   dlgs1 = dlg1.GetInvocationList();
   dlgs2 = dlg2.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs1[0].Target).Equals(dlgs2[0].Target))
     {
     iCountErrors++;
     print("E_55ak");
     }
   if((dlgs1[0].Method.Name ).Equals(dlgs2[0].Method.Name ))
     {
     iCountErrors++;
     print("E_21dl");
     }
   dlgs1 = new Delegate[1];
   dlgs2 = new Delegate[1];
   dlg1 = new Co3156_dlg_1(cb1.method1);
   dlg3 = new Co3156_dlg_3(cb1.method1);
   dlgs1 = dlg1.GetInvocationList();
   dlgs2 = dlg3.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs1[0].Target).Equals(dlgs2[0].Target))
     {
     iCountErrors++;
     print("E_48jd");
     }
   if(!(dlgs1[0].Method.Name ).Equals(dlgs2[0].Method.Name ))
     {
     iCountErrors++;
     print("E_42ll");
     }
   dlgs1 = new Delegate[1];
   dlgs2 = new Delegate[1];
   dlg1 = new Co3156_dlg_1(cb1.method1);
   dlg2 = new Co3156_dlg_1(cb2.method1);
   dlgs1 = dlg1.GetInvocationList();
   dlgs2 = dlg2.GetInvocationList();
   iCountTestcases++;
   if((dlgs1[0].Target).Equals(dlgs2[0].Target))
     {
     iCountErrors++;
     print("E_25kd");
     }
   dlgs1 = new Delegate[5];
   mcDlg1a = new Co3156_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3156_dlgmc_1(cb1.method3);
   mcDlg1c = new Co3156_dlgmc_1(cb1.method2);
   try {
   AddToDelegate(mcDlg1a);
   AddToDelegate(mcDlg1b); 
   AddToDelegate(mcDlg1c);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_389a");
     strError = "EXTENEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   dlgs1 = MCHandler.GetInvocationList();
   iCountTestcases++;
   if(!"method1".Equals(dlgs1[0].Method.Name ))
     {
     iCountErrors++;
     print("E_38fd! , Name=="+dlgs1[0].Method.Name);
     }
   iCountTestcases++;
   if(!"method3".Equals(dlgs1[1].Method.Name ))
     {
     iCountErrors++;
     print("E_49er");
     }
   iCountTestcases++;
   if(!"method2".Equals(dlgs1[2].Method.Name ))
     {
     iCountErrors++;
     print("E_12ld! , Name=="+dlgs1[2].Method.Name);
     }
   iCountTestcases++;
   if(!(dlgs1[0].Target).Equals(dlgs1[1].Target))
     {
     iCountErrors++;
     print("E_41fo");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3156GetInvocationList.)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3156GetInvocationList cb0 = new Co3156GetInvocationList();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3156GetInvocationList.") ;
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
