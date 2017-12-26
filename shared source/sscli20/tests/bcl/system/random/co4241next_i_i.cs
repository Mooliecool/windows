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
public class Co4241Next_i_i
{
 internal static String strName = "Random.Co4241Next_i_i";
 internal static String strTest = "Co4241Next_i_i";
 public virtual bool runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   Random rdm = null;
   String strLoc = null;
   int retVal;
   iCountTestcases++;
   rdm = null;
   try 
     {
     rdm.Next(5, 10);
     iCountErrors++;
     }
   catch (NullReferenceException ) {}
   rdm = new Random();
   iCountTestcases++;
   try 
     { 
     rdm.Next(10, 5);
     Console.WriteLine("Err_752dsgf! No exception thrown");
     iCountErrors++;
     }
   catch (ArgumentException ) {}
   catch (Exception ex)
     {
     Console.WriteLine("Err_752dsgf! exception thrown, "+ ex);
     iCountErrors++;
     }
   iCountTestcases++;
   try 
     { 
     rdm.Next(0, -5);
     Console.WriteLine("Err_752dsgf! No exception thrown");
     iCountErrors++;
     }
   catch (ArgumentException ) {}
   catch (Exception ex)
     {
     Console.WriteLine("Err_752dsgf! exception thrown, "+ ex);
     iCountErrors++;
     }
   iCountTestcases++;
   try 
     { 
     rdm.Next(-3, -5);
     Console.WriteLine("Err_752dsgf! No exception thrown");
     iCountErrors++;
     }
   catch (ArgumentException ) {}
   catch (Exception ex)
     {
     Console.WriteLine("Err_752dsgf! exception thrown, "+ ex);
     iCountErrors++;
     }
   try 
     { 
     rdm.Next(Int32.MaxValue, Int32.MinValue);
     Console.WriteLine("Err_752dsgf! No exception thrown");
     iCountErrors++;
     }
   catch (ArgumentException ) {}
   catch (Exception ex)
     {
     Console.WriteLine("Err_752dsgf! exception thrown, "+ ex);
     iCountErrors++;
     }
   try 
     {
     iCountTestcases++; strLoc = "L_gkjhe";
     if  ((rdm.Next(5, 5) != 5)) 
       {
       iCountErrors++;
       print ("E_fy387"); strLoc = "L_fg48";
       }
     iCountTestcases++; strLoc = "L_37gd";
     if  ((rdm.Next(-5, -5) != -5))
       {
       iCountErrors++;
       print ("E_t328"); strLoc = "L_874f";
       }
     strLoc = "L_37gd";
     for (int i = 0; i < 100; i++) 
       {
       iCountTestcases++;
       retVal = rdm.Next(-10, -5);
       if (retVal > -5 || retVal < -10) 
	 {
	 iCountErrors++;
	 print ("E_yu28");
	 }
       }
     }
   catch (Exception exc) { iCountErrors++;print (strLoc); printexc (exc);}
   rdm = new Random();
   for (int i = 0; i < 100; i++) 
     {
     iCountTestcases++;
     retVal = rdm.Next(50, 150);
     if ( retVal < 50 || retVal > 150) 
       {
       Console.WriteLine("Err_752ef! Unexpecetd value man, " + retVal);
       iCountErrors++;
       print ("E_t27");
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
   Co4241Next_i_i oCbTest = new Co4241Next_i_i();
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
