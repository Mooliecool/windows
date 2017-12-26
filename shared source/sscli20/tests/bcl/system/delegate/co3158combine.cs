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
public delegate void Co3158_dlg_1();
public delegate int Co3158_dlg_3(int i);
public delegate void Co3158_dlgmc_1();
public class Co3158Combine
{
 public Co3158_dlgmc_1 MCHandler;
 public virtual void method1() {}
 public virtual int method1(int i) {return 0;}
 public virtual int method2(int i) {return 0;}
 public virtual void method2() {}
 public virtual void method3(int i) {}
 public virtual void method3() {}
 public static void static_method1() {}
 public static int static_method1(int i) {return 0;}
 public static int static_method2(int i) {return 0;}
 public static void static_method2() {}
 public static void static_method3(int i) {}
 public static void static_method3() {}
 public virtual void AddToDelegate(Co3158_dlgmc_1 _handler)
   {
   MCHandler = (Co3158_dlgmc_1) Delegate.Combine(MCHandler, _handler);
   }
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Delegate\\Co3158Combine. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   Co3158Combine cb1 = new Co3158Combine();
   Co3158Combine cb2 = new Co3158Combine();
   Co3158_dlg_1 dlg1 = null;
   Co3158_dlg_1 dlg2 = null;
   Co3158_dlg_3 dlg3 = null;
   Co3158_dlgmc_1 mcDlg1a = null;
   Co3158_dlgmc_1 mcDlg1b = null;
   Co3158_dlgmc_1 mcDlg1c = null;
   Delegate[] dlgs = null;
   dlg1 = new Co3158_dlg_1(cb1.method1);
   dlg3 = new Co3158_dlg_3(cb1.method1);
   iCountTestcases++;
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(dlg1, dlg3);
   iCountErrors++;
   print("E_342o");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_82sj");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   dlg1 = new Co3158_dlg_1(cb1.method1);
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   iCountTestcases++;
   try {
   mcDlg1a = (Co3158_dlgmc_1)Delegate.Combine(mcDlg1a, dlg1);
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_2ksl");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   dlg1 = new Co3158_dlg_1(Co3158Combine.static_method1);
   dlg3 = new Co3158_dlg_3(Co3158Combine.static_method1);
   iCountTestcases++;
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(dlg1, dlg3);
   iCountErrors++;
   print("E_482hs");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_823sj");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   dlg1 = new Co3158_dlg_1(Co3158Combine.static_method1);
   mcDlg1a = new Co3158_dlgmc_1(Co3158Combine.static_method1);
   iCountTestcases++;
   try {
   mcDlg1a = (Co3158_dlgmc_1)Delegate.Combine(mcDlg1a, dlg1);
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_2ksl");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   dlg1 = new Co3158_dlg_1(cb1.method1);
   dlg2 = new Co3158_dlg_1(cb1.method1);
   iCountTestcases++;
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(dlg1,dlg2);
   iCountErrors++;
   print("E_4ksl");
   } catch (MulticastNotSupportedException ) {}
   catch (InvalidCastException) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_49nc");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   dlg1 = new Co3158_dlg_1(Co3158Combine.static_method1);
   dlg2 = new Co3158_dlg_1(Co3158Combine.static_method1);
   iCountTestcases++;
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(dlg1,dlg2);
   iCountErrors++;
   print("E_43ksl");
   } catch (MulticastNotSupportedException ) {}
   catch (InvalidCastException ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_494nc");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3158_dlgmc_1(cb1.method3);
   iCountTestcases++;
   try {
   dlg1 = (Co3158_dlg_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   print("E_49jf");iCountErrors++;
   }
   catch (InvalidCastException ) {}
   catch (Exception ) {print("E_58wf");iCountErrors++;}
   iCountTestcases++;
   try {
   if(Delegate.Combine(null, null) != null)
     {
     iCountErrors++;
     print("E_9dml");
     }
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_439o");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1b = null;
   try {
   iCountTestcases++;
   if(!(Delegate.Combine(mcDlg1a, mcDlg1b)).Equals(mcDlg1a))
     {
     iCountErrors++;
     print("E_1wer");
     }
   iCountTestcases++;
   if(!(Delegate.Combine(mcDlg1b, mcDlg1a)).Equals(mcDlg1a))
     {
     iCountErrors++;
     print("E_3jao");
     }
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_3kwc");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   dlg1 = new Co3158_dlg_1(cb1.method1);
   mcDlg1a = null;
   iCountTestcases++;
   try {
   mcDlg1a = (Co3158_dlgmc_1)Delegate.Combine(dlg1, null);  
   iCountErrors++;
   print("E_597sx");
   }
   catch (InvalidCastException ){}
   catch (Exception exc)
     {
     iCountErrors++;
     print("E_483aj");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   iCountTestcases++;
   try {
   dlg2 = (Co3158_dlg_1)Delegate.Combine(dlg1, null);
   } catch (Exception exc)
     {
     iCountErrors++;
     print("E_32sk");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3158_dlgmc_1(cb1.method3);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   } catch (Exception ) {iCountErrors++;print("E_929f");}
   dlgs = mcDlg1a.GetInvocationList();
   if(dlgs.Length != 2)
     {
     iCountErrors++;
     print("E_49la");
     }
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1c = new Co3158_dlgmc_1(cb1.method1);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1c);
   } catch (Exception ) {iCountErrors++;print("E_23dn");}
   dlgs = mcDlg1a.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs[0].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_939s");
     }
   iCountTestcases++;
   if(!(dlgs[1].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_37jd");
     }
   iCountTestcases++;
   if(!(dlgs[2].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_399f");
     }
   mcDlg1a = new Co3158_dlgmc_1(Co3158Combine.static_method1);
   mcDlg1b = new Co3158_dlgmc_1(Co3158Combine.static_method1);
   mcDlg1c = new Co3158_dlgmc_1(Co3158Combine.static_method1);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1c);
   } catch (Exception ) {iCountErrors++;print("E_232dn");}
   dlgs = mcDlg1a.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs[0].Method.Name ).Equals("static_method1"))
     {
     iCountErrors++;
     print("E_9393s");
     }
   if(!(dlgs[1].Method.Name ).Equals("static_method1"))
     {
     iCountErrors++;
     print("E_374jd");
     }
   if(!(dlgs[2].Method.Name ).Equals("static_method1"))
     {
     iCountErrors++;
     print("E_3996f");
     }
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3158_dlgmc_1(Co3158Combine.static_method1);
   mcDlg1c = new Co3158_dlgmc_1(cb1.method1);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1c);
   } catch (Exception ) {iCountErrors++;print("E_232dn");}
   dlgs = mcDlg1a.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs[0].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_934ss");
     }
   if(!(dlgs[1].Method.Name ).Equals("static_method1"))
     {
     iCountErrors++;
     print("E_342d");
     }
   if(!(dlgs[2].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_3996f");
     }
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3158_dlgmc_1(cb1.method3);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   } catch (Exception ) {iCountErrors++;print("E_93mx");}
   dlgs = mcDlg1a.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs[0].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_29sj");
     }
   if(!(dlgs[1].Method.Name ).Equals("method3"))
     {
     iCountErrors++;
     print("E_283f");
     }
   mcDlg1a = new Co3158_dlgmc_1(Co3158Combine.static_method1);
   mcDlg1b = new Co3158_dlgmc_1(Co3158Combine.static_method3);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   } catch (Exception ) {iCountErrors++;print("E_93mx");}
   dlgs = mcDlg1a.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs[0].Method.Name ).Equals("static_method1"))
     {
     iCountErrors++;
     print("E_294sj");
     }
   if(!(dlgs[1].Method.Name ).Equals("static_method3"))
     {
     iCountErrors++;
     print("E_2832f");
     }
   dlgs = new Delegate[5];
   mcDlg1a = new Co3158_dlgmc_1(cb1.method1);
   mcDlg1b = new Co3158_dlgmc_1(cb2.method1);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   } catch (Exception ) {iCountErrors++;print("E_41ma");}
   dlgs = mcDlg1a.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs[0].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_91di");
     }
   if(!(dlgs[1].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_24ls");
     }
   dlgs = new Delegate[5];
   mcDlg1a = new Co3158_dlgmc_1(Co3158Combine.static_method1);
   mcDlg1b = new Co3158_dlgmc_1(cb2.method1);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3158_dlgmc_1) Delegate.Combine(mcDlg1a, mcDlg1b);
   } catch (Exception ) {iCountErrors++;print("E_41ma");}
   dlgs = mcDlg1a.GetInvocationList();
   iCountTestcases++;
   if(!(dlgs[0].Method.Name ).Equals("static_method1"))
     {
     iCountErrors++;
     print("E_91di");
     }
   if(!(dlgs[1].Method.Name ).Equals("method1"))
     {
     iCountErrors++;
     print("E_24ls");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3158Combine.)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3158Combine cb0 = new Co3158Combine();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3158Combine."  );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
