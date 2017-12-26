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
delegate void Co3154_dlg_1();
delegate int Co3154_dlg_3(int i);
delegate int Co3154_dlg_4(int i, String s);
public class Co3154GetTarget
{
 public virtual void method1() {}
 public virtual int method1(int i) {return 0;}
 public virtual int method2(int i) {return 0;}
 public virtual void method3(int i) {}
 public static void method3() {}
 public virtual int method4(int i, String s) {return i;}
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Delegate\\Co3154GetTarget runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Co3154GetTarget cb1 = new Co3154GetTarget();
   Co3154GetTarget cb2 = new Co3154GetTarget();
   Co3154_dlg_1 dlg1 = null;
   Co3154_dlg_4 dlg4 = null;
   Object objReturned;
   dlg1 = new Co3154_dlg_1(cb1.method1);
   objReturned = dlg1.Target;
   iCountTestcases++;
   if(!objReturned.Equals(cb1))
     {
     iCountErrors++;
     print("E_944q");
     }
   dlg1 = new Co3154_dlg_1(Co3154GetTarget.method3);
   objReturned = dlg1.Target;
   iCountTestcases++;
   if(objReturned != null)
     {
     iCountErrors++;
     print("E_871h");  
     }
   dlg4 = new Co3154_dlg_4(cb1.method4);
   objReturned = dlg4.Target;
   iCountTestcases++;
   if(!objReturned.Equals(cb1))
     {
     iCountErrors++;
     print("E_471j");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3154GetTarget.exe)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3154GetTarget cb0 = new Co3154GetTarget();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3154GetTarget")  ;
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
