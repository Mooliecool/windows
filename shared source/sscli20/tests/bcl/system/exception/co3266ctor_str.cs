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
using System.Text;
using System;
using System.Collections;
using System.IO;
public class Co3266ctor_str
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Exception\\Co3266ctor_str.cs runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   Exception excName1 = null;
   Exception excName2 = null;
   try {
   iCountTestcases++;
   try {
   excName1 = new Exception(null);
   }
   catch (Exception exc2) {
   iCountErrors++;
   print("E_3j3e");
   printexc(exc2);
   }
   excName1 = new Exception("This is an exception");
   iCountTestcases++;
   if(!excName1.Message.Equals("This is an exception"))
     {
     iCountErrors++;
     print("E_j23t");
     }
   iCountTestcases++;
   if(!excName1.GetType().FullName.Equals("System.Exception"))
     {
     iCountErrors++;
     print("E_3uy9");
     }
   iCountTestcases++;
   if(excName1.InnerException != null)
     {
     iCountErrors++;
     print("E_234j");
     }
   } catch (Exception exc)
     {
     iCountErrors++;
     Console.Out.WriteLine("Uncaught Exception");
     print("E_3r8s");
     printexc(exc);
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Exception\\Co3266ctor_str.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine(  "Co3266ctor_str.cs"  );
     Console.Error.Write( "Co3266ctor_str.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Exception\\Co3266ctor_str.cs: FAiL!" );
     return false;
     }
   }
 private void printexc(Exception exc)
   {
   String output = "EXTENDEDINFO: "+exc.ToString();
   Console.Out.WriteLine(output);
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3266ctor_str.cs)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3266ctor_str cb0 = new Co3266ctor_str();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3266ctor_str.cs"  );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
