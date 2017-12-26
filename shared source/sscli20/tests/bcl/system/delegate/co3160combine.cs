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
public delegate void Co3160_dlg_1();
public delegate int Co3160_dlg_3(int i);
public delegate void Co3160_dlgmc_1();
public class Co3160Combine
{
 public Co3160_dlgmc_1 MCHandler;
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
 public virtual void AddToDelegate(Co3160_dlgmc_1 _handler)
   {
   MCHandler = (Co3160_dlgmc_1) Delegate.Combine(MCHandler, _handler);
   }
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Delegate\\Co3160Combine. runTest() started." );
   int inCountErrors = 0;
   int inCountTestcases = 0;
   int inErrorBits = 0;
   int in4w1=0 ,in4w3=0;
   String strError = null;
   Co3160Combine cb1 = new Co3160Combine();
   Co3160Combine cb2 = new Co3160Combine();
   Co3160_dlg_1 dlg1 = null;
   Co3160_dlg_1 dlg2 = null;
   Co3160_dlgmc_1 mcDlg1a = null;
   Delegate[] dlgs = null;
   Delegate[] inputdlgs = null;
   inputdlgs = new Delegate[5];;
   inputdlgs[0] = new Co3160_dlg_1(cb1.method1);
   inputdlgs[1] = new Co3160_dlg_3(cb1.method1);
   inputdlgs[2] = new Co3160_dlg_1(cb1.method1);
   inCountTestcases++;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   inCountErrors++;
   print("E_342o");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     inCountErrors++;
     print("E_82sj");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlg_1(cb1.method1);
   inputdlgs[1] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[2] = new Co3160_dlg_3(cb1.method1);
   inCountTestcases++;
   try {
   mcDlg1a = (Co3160_dlgmc_1)Delegate.Combine(inputdlgs);
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     inCountErrors++;
     print("E_2ksl");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];;
   inputdlgs[0] = new Co3160_dlg_1(Co3160Combine.static_method1);
   inputdlgs[1] = new Co3160_dlg_3(Co3160Combine.static_method1);
   inputdlgs[2] = new Co3160_dlg_1(cb1.method1);
   inCountTestcases++;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   inCountErrors++;
   print("E_3428o");
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     inCountErrors++;
     print("E_824sj");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlg_1(Co3160Combine.static_method1);
   inputdlgs[1] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[2] = new Co3160_dlg_3(cb1.method1);
   inCountTestcases++;
   try {
   mcDlg1a = (Co3160_dlgmc_1)Delegate.Combine(inputdlgs);
   } catch (ArgumentException ) {}
   catch (Exception exc2)
     {
     inCountErrors++;
     print("E_2ksl");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Out.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlg_1(cb1.method1);
   inputdlgs[1] = new Co3160_dlg_1(cb1.method1);
   inputdlgs[2] = new Co3160_dlg_1(cb1.method1);
   inCountTestcases++;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   inCountErrors++;
   print("E_4k7sl");
   } catch (MulticastNotSupportedException ) {}
   catch (InvalidCastException ) {}
   catch (Exception exc2)
     {
     inCountErrors++;
     print("E_499nc");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlg_1(Co3160Combine.static_method1);
   inputdlgs[1] = new Co3160_dlg_1(Co3160Combine.static_method1);
   inputdlgs[2] = new Co3160_dlg_1(Co3160Combine.static_method1);
   inCountTestcases++;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   inCountErrors++;
   print("E_4ksl");
   } catch (MulticastNotSupportedException ) {}
   catch (InvalidCastException ) {}
   catch (Exception exc2)
     {
     inCountErrors++;
     print("E_49nc");
     strError = "EXTENDEDINFO: "+exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[1] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[2] = new Co3160_dlgmc_1(cb1.method1);
   inCountTestcases++;
   try {
   dlg1 = (Co3160_dlg_1) Delegate.Combine(inputdlgs);
   } catch (InvalidCastException) {}
   catch (Exception exc)
     {
     inCountErrors++;
     print("E_48kf");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Out.WriteLine(strError);
     }
   inCountTestcases++;
   try {
   if(Delegate.Combine(null) != null)
     {
     inCountErrors++;
     print("E_9dml");
     }
   } catch (Exception exc)
     {
     inCountErrors++;
     print("E_439o");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[1] = null;
   inputdlgs[2] = null;
   try {
   inCountTestcases++;
   if(!(Delegate.Combine(inputdlgs)).Equals(inputdlgs[0]))
     {
     inCountErrors++;
     print("E_1wer");
     }
   } catch (Exception exc)
     {
     inCountErrors++;
     print("E_3kwc");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs[0] = null;
   inputdlgs[1] = null;
   inputdlgs[2] = new Co3160_dlgmc_1(cb1.method1);
   try {
   inCountTestcases++;
   if(!(Delegate.Combine(inputdlgs)).Equals(inputdlgs[2]))
     {
     inCountErrors++;
     print("E_4ism");
     }
   } catch (Exception exc)
     {
     inCountErrors++;
     print("E_9ksm");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[1] = null;
   inputdlgs[2] = null;
   try {
   inCountTestcases++;
   if(!(Delegate.Combine(inputdlgs)).Equals(inputdlgs[0]))
     {
     inCountErrors++;
     print("E_15wer");
     }
   } catch (Exception exc)
     {
     inCountErrors++;
     print("E_39kwc");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs[0] = null;
   inputdlgs[1] = null;
   inputdlgs[2] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   try {
   inCountTestcases++;
   if(!(Delegate.Combine(inputdlgs)).Equals(inputdlgs[2]))
     {
     inCountErrors++;
     print("E_422sm");
     }
   } catch (Exception exc)
     {
     inCountErrors++;
     print("E_90ksm");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlg_1(cb1.method1);
   mcDlg1a = null;
   inCountTestcases++;
   inCountTestcases++;
   try {
   dlg2 = (Co3160_dlg_1)Delegate.Combine(inputdlgs);
   } catch (Exception exc)
     {
     inCountErrors++;
     print("E_32sk");
     strError = "EXTENDEDINFO: "+exc.ToString();
     Console.Error.WriteLine(strError);
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[1] = new Co3160_dlgmc_1(cb1.method3);
   inputdlgs[2] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[3] = new Co3160_dlgmc_1(cb1.method3);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_929f");}
   dlgs = mcDlg1a.GetInvocationList();
   if(dlgs.Length != 4)
     {
     inCountErrors++;
     print("E_49la");
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[1] = new Co3160_dlgmc_1(Co3160Combine.static_method3);
   inputdlgs[2] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[3] = new Co3160_dlgmc_1(cb1.method3);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_9329f");}
   dlgs = mcDlg1a.GetInvocationList();
   if(dlgs.Length != 4)
     {
     inCountErrors++;
     print("E_491la");
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[1] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[2] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[3] = new Co3160_dlgmc_1(cb1.method1);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_23dn");}
   dlgs = mcDlg1a.GetInvocationList();
   if(!(dlgs[0].Method.Name ).Equals("method1"))
     {
     inCountErrors++;
     print("E_939s");
     }
   if(!(dlgs[1].Method.Name ).Equals("method1"))
     {
     inCountErrors++;
     print("E_37jd");
     }
   if(!(dlgs[2].Method.Name ).Equals("method1"))
     {
     inCountErrors++;
     print("E_399f");
     }
   if(!(dlgs[2].Method.Name ).Equals("method1"))
     {
     inCountErrors++;
     print("E_93kz");
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[1] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[2] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[3] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   dlgs = new Delegate[5];
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_23dn");}
   dlgs = mcDlg1a.GetInvocationList();
   if(!(dlgs[0].Method.Name ).Equals("static_method1"))
     {
     inCountErrors++;
     print("E_9394s");
     }
   if(!(dlgs[1].Method.Name ).Equals("static_method1"))
     {
     inCountErrors++;
     print("E_375jd");
     }
   if(!(dlgs[2].Method.Name ).Equals("static_method1"))
     {
     inCountErrors++;
     print("E_3992f");
     }
   if(!(dlgs[2].Method.Name ).Equals("static_method1"))
     {
     inCountErrors++;
     print("E_932kz");
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[1] = null; 
   inputdlgs[2] = new Co3160_dlgmc_1(cb1.method3);
   dlgs = new Delegate[5];
   mcDlg1a = null;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_93mx");}
   dlgs = mcDlg1a.GetInvocationList();
   in4w1 = 0x1 | 0x2;
   inErrorBits = inErrorBits | 0x1 | 0x2;
   for ( int aa = 0 ;aa < dlgs.Length ;aa++ )  
     {
     if ( dlgs[aa].Method.Name .Equals( "method1" ) == true )
       inErrorBits &= ~(0x1);
     if ( dlgs[aa].Method.Name .Equals( "method3" ) == true )
       inErrorBits &= ~(0x2);
     }
   ++inCountTestcases;
   if ( (inErrorBits & in4w1) != 0 )  
     {
     ++inCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_29sj2!  inErrorBits==" + inErrorBits  );
     }
   inputdlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[1] = null; 
   inputdlgs[2] = new Co3160_dlgmc_1(Co3160Combine.static_method3);
   dlgs = new Delegate[5];
   mcDlg1a = null;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_934mx");}
   dlgs = mcDlg1a.GetInvocationList();
   in4w1 = 0x1 | 0x2;
   inErrorBits = inErrorBits | 0x1 | 0x2;
   for ( int aa = 0 ;aa < dlgs.Length ;aa++ )  
     {
     if ( dlgs[aa].Method.Name .Equals( "static_method1" ) == true )
       inErrorBits &= ~(0x1);
     if ( dlgs[aa].Method.Name .Equals( "static_method3" ) == true )
       inErrorBits &= ~(0x2);
     }
   ++inCountTestcases;
   if ( (inErrorBits & in4w1) != 0 )  
     {
     ++inCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_291j2!  inErrorBits==" + inErrorBits  );
     }
   inputdlgs = new Delegate[5];
   dlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(cb1.method1);
   inputdlgs[1] = new Co3160_dlgmc_1(cb2.method1);
   inputdlgs[2] = new Co3160_dlgmc_1(cb1.method3);
   inputdlgs[3] = new Co3160_dlgmc_1(cb2.method3);
   dlgs = new Delegate[5];
   mcDlg1a = null;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_41ma");}
   dlgs = mcDlg1a.GetInvocationList();
   in4w1 = 2;   in4w3 = 2;
   for ( int aa = 0 ;aa < dlgs.Length ;aa++ )  
     {
     if ( dlgs[aa].Method.Name .Equals( "method1" ) == true )
       --in4w1;
     if ( dlgs[aa].Method.Name .Equals( "method3" ) == true )
       --in4w3;
     }
   ++inCountTestcases;
   if ( in4w1 != 0  ||  in4w3 != 0 )
     {
     ++inCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_38jw2!  in4w1==" + in4w1 + " ,in4w3==" + in4w3  );
     }
   inputdlgs = new Delegate[5];
   dlgs = new Delegate[5];
   inputdlgs[0] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[1] = new Co3160_dlgmc_1(Co3160Combine.static_method1);
   inputdlgs[2] = new Co3160_dlgmc_1(Co3160Combine.static_method3);
   inputdlgs[3] = new Co3160_dlgmc_1(Co3160Combine.static_method3);
   dlgs = new Delegate[5];
   mcDlg1a = null;
   try {
   mcDlg1a = (Co3160_dlgmc_1) Delegate.Combine(inputdlgs);
   } catch (Exception ) {inCountErrors++;print("E_413ma");}
   dlgs = mcDlg1a.GetInvocationList();
   in4w1 = 2;   in4w3 = 2;
   for ( int aa = 0 ;aa < dlgs.Length ;aa++ )  
     {
     if ( dlgs[aa].Method.Name .Equals( "static_method1" ) == true )
       --in4w1;
     if ( dlgs[aa].Method.Name .Equals( "static_method3" ) == true )
       --in4w3;
     }
   ++inCountTestcases;
   if ( in4w1 != 0  ||  in4w3 != 0 )
     {
     ++inCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_39jw2!  in4w1==" + in4w1 + " ,in4w3==" + in4w3  );
     }
   if ( inCountErrors == 0 )
     {
     Console.Error.Write( "Delegate\\Co3160Combine.: paSs.  inCountTestcases==" );
     Console.Error.WriteLine( inCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine(  "Co3160Combine."  );
     Console.Error.Write( "Co3160Combine. inCountErrors==" );
     Console.Error.WriteLine( inCountErrors );
     Console.Error.WriteLine( "Delegate\\Co3160Combine.: FAiL!" );
     return false;
     }
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3160Combine.)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3160Combine cb0 = new Co3160Combine();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3160Combine."  );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
