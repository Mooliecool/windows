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
public class Co4239ctor_i
{
 internal static String strName = "Random.Random";
 internal static String strTest = "Co4239ctor_i";
 public virtual bool runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   Random rdm = null;
   Random rdm2 = null;
   String strLoc = null;
   int retVal;
   try {
   iCountTestcases++;
   rdm = new Random (-4); strLoc = "L_fg265"; 
   iCountTestcases++;
   rdm = new Random (unchecked(Int32.MaxValue + 1)); strLoc = "L_tr309";
   iCountTestcases++;
   rdm = new Random (unchecked(Int32.MinValue - 1)); strLoc = "L_iu32";
   iCountTestcases++;
   rdm = new Random (unchecked((int)Int64.MaxValue + 1)); strLoc = "L_gf38";
   iCountTestcases++;
   rdm = new Random (unchecked((int)Int64.MinValue - 1));
   }
   catch (Exception ) { print (strLoc);}
   rdm = new Random(4); 
   for (int i = 0; i < 100; i++) {
   iCountTestcases++;
   retVal = rdm.Next();
   if ( retVal < 0 || retVal > Int32.MaxValue) {
   iCountErrors++;
   print ("E_gh46");
   }
   }
   rdm = new Random(4); 
   rdm2 = new Random(4); 
   for (int i = 0; i < 100; i++) {
   iCountTestcases++;
   if (rdm.Next() != rdm2.Next()) {
   iCountErrors++;
   print ("E_g32");
   }
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strTest + "   iCountTestCases == " + iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL. " + strTest + "    iCountErrors==" + iCountErrors);
     return false;
     }
   }
 private void printexc(Exception exc)
   {
   String output = "EXTENDEDINFO: "+exc.ToString();
   Console.Error.WriteLine(output);
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (" + strTest + ")");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co4239ctor_i oCbTest = new Co4239ctor_i();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.Error.Write( strTest );
     Console.Error.WriteLine(  ": Main() Uncaught exception" );
     Console.Error.WriteLine( ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
