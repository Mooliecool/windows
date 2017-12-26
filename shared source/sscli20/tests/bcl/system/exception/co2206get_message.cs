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
using System;
using System.Collections; 
public class Co2206get_Message
{
 static String s_strTFName = "Co2206get_Message";
 public Boolean runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   Exception myException = null;
   String strException = "Co2206get_Message Exception Test";
   String strCaught = "Exception caught" ;
   Console.Out.WriteLine( "Co2206get_Message.cs  runTest() started." );
   iCountTestcases++;
   Console.Error.WriteLine( "Create exception with a string message and verify string." );
   myException = new Exception( strException );
   try
     {
     throw myException;
     }
   catch ( Exception ex )
     {
     Console.Error.WriteLine( strCaught );
     if ( String.Compare( strException, ex.Message ) != 0 )
       {
       iCountErrors++;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_36ua (Co2206get_Message)"  );
       }
     else
       Console.Error.WriteLine( "GetMessage() successfully verified" );
     }
   try
     {
     Console.Error.WriteLine(  "Level 0"  );
     try
       {
       Console.Error.WriteLine(  "Level 1"  );
       try
	 {
	 Console.Error.WriteLine(  "Level 2"  );
	 try
	   {
	   Console.Error.WriteLine(  "Level 3"  );
	   throw myException;
	   }
	 catch ( Exception ex )
	   {
	   iCountTestcases++;
	   if ( String.Compare( strException, ex.Message ) != 0 )
	     {
	     iCountErrors++;
	     Console.Error.WriteLine(  "POINTTOBREAK: find error E_41he (Co2206get_Message)"  );
	     }
	   else
	     Console.Error.WriteLine(  "Level 3 exception caught and verified"  );
	   }
	 throw myException;
	 }
       catch ( Exception ex )
	 {
	 iCountTestcases++;
	 if ( String.Compare( strException, ex.Message ) != 0 )
	   {
	   iCountErrors++;
	   Console.Error.WriteLine(  "POINTTOBREAK: find error E_58mq (Co2206get_Message)"  );
	   }
	 else
	   Console.Error.WriteLine(  "Level 2 exception caught and verified"  );
	 }
       throw myException;
       }
     catch ( Exception ex )
       {
       iCountTestcases++;
       if ( String.Compare(strException, ex.Message ) != 0 )
	 {
	 iCountErrors++;
	 Console.Error.WriteLine(  "POINTTOBREAK: find error E_62di (Co2206get_Message)"  );
	 }
       else
	 Console.Error.WriteLine(  "Level 1 exception caught and verified"  );
       }
     }
   catch ( Exception ex )
     {
     iCountTestcases++;
     if ( String.Compare(strException, ex.Message ) != 0 )
       {
       iCountErrors++;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_79ko (Co2206get_Message)"  );
       }
     else
       Console.Error.WriteLine(  "Level 1 exception caught and verified"  );
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   String strW = null;
   Co2206get_Message cbA = new Co2206get_Message();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co2206get_Message.cs"  );
     strW = "EXTENDEDINFO: ";
     strW = strW + exc.ToString();
     Console.Error.WriteLine( strW  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
