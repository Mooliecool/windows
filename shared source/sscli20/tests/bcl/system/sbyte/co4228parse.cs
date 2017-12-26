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
using System.IO;
using System.Text;
public class Co4228Parse
{
 static String strName = "Byte.Parse";
 static String strTest = "Co4228Parse";
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   String strValue = null;
   SByte SByteVal;
   SByte sb = (SByte)4;
   sb = (SByte) (sb + (SByte)2);
   for ( SByteVal = (SByte)SByte.MinValue;  ; SByteVal = (SByte)(SByteVal + (SByte)(1))) 
     {
     iCountTestcases++;
     strValue = Convert.ToString( (long)SByteVal, 10 );
     SByte SByteRet = 0x1;
     try
       {
       SByteRet = SByte.Parse( strValue );
       if ( SByteVal != SByteRet )
	 {
	 String strInfo =  "Parse error: " ;
	 strInfo = strInfo + "Expected Result = <" ;
	 strInfo = strInfo + SByteVal.ToString() ;
	 strInfo = strInfo + "> " ;
	 strInfo = strInfo + "Returned Result = <" ;
	 strInfo = strInfo + SByteRet.ToString() ;
	 strInfo = strInfo + "> " ;
	 Console.WriteLine( strTest+ "E_101" );
	 Console.WriteLine( strTest+ strInfo );
	 ++iCountErrors;
	 break;
	 }
       else if ( SByteVal == SByte.MaxValue )
	 break;
       }
     catch( Exception exc )
       {
       ++iCountErrors;
       print ("E_uey7" );
       printexc (exc);
       break;
       }
     }
   try
     {
     iCountTestcases++;
     strValue =  "1000" ;
     SByteVal = SByte.Parse( strValue );
     ++iCountErrors;
     print ("E_o88" );
     }
   catch(OverflowException )
     {
     }
   catch( Exception exc )
     {
     ++iCountErrors;
     print ("E_i28" );
     printexc (exc);
     }
   try
     {
     iCountTestcases++;
     strValue =  "-1000" ;
     SByteVal = SByte.Parse( strValue );
     ++iCountErrors;
     print ("E_hj28" );
     }
   catch(OverflowException )
     {
     }
   catch( Exception exc )
     {
     ++iCountErrors;
     print ("E_hjk3" );
     printexc (exc);
     }
   try
     {
     iCountTestcases++;
     strValue =  "This is an invalid string" ;
     SByteVal = SByte.Parse( strValue );
     ++iCountErrors;
     print ("E_h282" );
     }
   catch( FormatException  )
     {
     }
   catch( Exception exc )
     {
     ++iCountErrors;
     print ("E_h287" );
     printexc (exc);
     }
   try
     {
     iCountTestcases++;
     strValue = null;
     SByteVal = SByte.Parse( strValue );
     ++iCountErrors;
     print ("E_k39" );
     }
   catch( ArgumentException  )
     {
     }
   catch( Exception exc )
     {
     ++iCountErrors;
     print ("E_1414" );
     printexc (exc);
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
   output.Append(" (Cb4201Put.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co4228Parse oCbTest = new Co4228Parse();
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
