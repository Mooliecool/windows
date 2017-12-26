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
using System.Globalization;
using System;
using System.Collections;
public class Co3265ctor
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co3265ctor.cs runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Exception excName1 = null;
   try {
   excName1 = new Exception();
   iCountTestcases++;
   if(!excName1.Message.Equals("Exception of type 'System.Exception' was thrown."))
     {
     if(CultureInfo.CurrentCulture.Equals(new CultureInfo("en-US")))
       {
       iCountErrors++;
       Console.WriteLine(excName1.Message + " Expected " + "An exception of type 'System.Exception' was thrown.");
       print("E_j23t");
       }
     }
   iCountTestcases++;
   if(!excName1.GetType().FullName.Equals("System.Exception"))
     {
     iCountErrors++;
     print("E_3uy9 exc=="+excName1.ToString());
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
     Console.Error.Write( "Co3265ctor.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine(  "Co3265ctor.cs"  );
     Console.Error.Write( "Co3265ctor.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine( "Co3265ctor.cs: FAiL!" );
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
   output.Append(" (Co3265ctor.cs)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3265ctor cb0 = new Co3265ctor();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3265ctor.cs");
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
