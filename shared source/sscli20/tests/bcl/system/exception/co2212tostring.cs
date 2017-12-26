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
using System.Text;
public class Co2212ToString
{
 static String strName = "Exception.ToString";
 static String s_strTFName = "Co2212ToString";
 public Boolean runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFName );
   Console.Out.WriteLine( " runTest started..." );
   Exception myException = null;
   StringBuilder strResult = new StringBuilder("Exception.ToString: Co2212ToString iCountErrors==");
   String strMessage = "Co2212ToString Exception Test";
   String strHeader =  "System.Exception" ;
   String strCaught = "Exception caught" ;
   String strException = null;
   String str2 = null;
   String str3 = null;
   Console.Error.WriteLine( "Create exception with a string message and verify string." );
   myException = new Exception( strMessage );
   strException = strHeader;
   strException = strException +  ": "  ;
   strMessage = strMessage;
   strException = strException + strMessage;
   iCountTestcases++;
   try
     {
     throw myException;
     }
   catch ( Exception ex )
     {
     Console.WriteLine((ex.ToString()).IndexOf(strException));
     Console.Error.WriteLine( strCaught );
     if ( (ex.ToString()).IndexOf(strException) == -1 )
       {
       Console.Out.WriteLine ("Error_132aa!");
       Console.Out.WriteLine( strException );
       Console.Out.WriteLine( ex.ToString() );
       iCountErrors++;
       }
     else
       Console.Error.WriteLine( "ToString() successfully verified" );
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
	   iCountTestcases++;
	   throw myException;
	   }
	 catch ( Exception ex )
	   {
	   Console.Error.WriteLine( strException );
	   Console.Error.WriteLine( ex.ToString() );
	   if ( (ex.ToString()).IndexOf(strException) == -1 )
	     {
	     Console.Out.WriteLine ("Error_132aw!");
	     Console.Out.Write( "strException = ");
	     Console.Out.WriteLine( strException );
	     Console.Out.Write( "ex.ToString() = ");
	     Console.Out.WriteLine( ex.ToString() );
	     iCountErrors++;
	     }
	   else
	     Console.Error.WriteLine(  "Level 3 exception caught and verified"  );
	   }
	 iCountTestcases++;
	 throw myException;
	 }
       catch ( Exception ex )
	 {
	 if ( (ex.ToString()).IndexOf(strException) == -1 )
	   {
	   Console.Out.WriteLine ("Error_131aa!");
	   Console.Out.Write( "strException = ");
	   Console.Out.WriteLine( strException );
	   Console.Out.Write( "ex.ToString() = ");
	   Console.Out.WriteLine( ex.ToString() );
	   iCountErrors++;
	   }
	 else
	   Console.Error.WriteLine(  "Level 2 exception caught and verified"  );
	 }
       iCountTestcases++;
       throw myException;
       }
     catch ( Exception ex )
       {
       if ( (ex.ToString()).IndexOf( strException) == -1 )
	 {
	 Console.Out.WriteLine ("Error_342aa!");
	 Console.Out.Write( "strException = ");
	 Console.Out.WriteLine( strException);
	 Console.Out.Write( "ex.ToString() = ");
	 Console.Out.WriteLine( ex.ToString() );
	 iCountErrors++;
	 }
       else
	 Console.Error.WriteLine(  "Level 1 exception caught and verified"  );
       }
     }
   catch ( Exception ex )
     {
     if ( (ex.ToString()).IndexOf( strException ) == -1 )
       {
       Console.Out.WriteLine ("Error_122aa!");
       iCountErrors++;
       }
     else
       Console.Error.WriteLine(  "Level 1 exception caught and verified"  );
     }
   iCountTestcases++;
   Console.Error.WriteLine( "Have bad parm to myString.Substring() cause the throw to enable myExc.ToString() ()." );
   str2 = "Construct Exc I_252tr" ;
   try
     {
     str2 = str2.Substring( -2 ,-3 );
     Console.Out.WriteLine ("Error_123cv!");
     iCountErrors++;
     Console.Error.WriteLine( "Exception\\Cb1373ToString.cs Error E_748sv!" );
     }
   catch ( Exception ex )
     {
     try
       {
       str3 = ex.ToString(); 
       }
     catch ( Exception )
       {
       Console.Out.WriteLine ("Error_112ga!");
       iCountErrors++;
       Console.Error.WriteLine( "Exception\\Cb1373ToString.cs Error E_084ft!" );
       }
     }
   Console.Error.WriteLine( strResult.Append(iCountErrors ) );
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
   Co2212ToString oCbTest = new Co2212ToString();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co2212ToString.cs"  );
     strW = "EXTENDEDINFO: ";
     strW = strW + exc.ToString() ;
     Console.Error.WriteLine( strW  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
} 
