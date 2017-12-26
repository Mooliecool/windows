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
using GenStrings;
using System;
public class Co4000ctor_StrArr
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4000ctor_StrArr.  runTest() started." );
   StringBuilder sblMsg = null;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String[] strArr1 = null;
   String strConstructed = null;
   String strExpectedResult = null;
   char[] chArr2 = new char[1];
   String str3 = null;
   int inw1 = -2;
   do
     {
     chArr2[0] = '\0';
     str3 = new String( chArr2 );
     inw1 = str3.Length;
     ++iCountTestcases;
     if ( inw1 != 1 )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: Error E_831gm (Co4000ctor_StrArr)  inw1==" + inw1  );
       }
     IntlStrings intl = new IntlStrings();
     strArr1 = new String[2];
     strArr1[0] = intl.GetString(4, true, true);
     strArr1[1] = intl.GetString(6, true, true);
     strExpectedResult = String.Concat(strArr1[0], strArr1[1]);
     String testString = strExpectedResult;
     if(testString.Equals(strExpectedResult) == false) {
     ++iCountErrors;
     }
     ++iCountTestcases;
     strArr1 = new String[5];
     strArr1[0] = "bc";
     strArr1[1] = "cd";
     strArr1[2] = "de";
     strArr1[3] = "ef";
     strArr1[4] = "fg";
     strExpectedResult = "bccddeeffg";
     strConstructed = String.Concat( strArr1 );
     if ( strConstructed == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_3b62 (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_3b62 ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     if ( strConstructed.Equals( strExpectedResult ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_09pv (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_09pv ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     ++iCountTestcases;
     strArr1 = new String[5];
     strArr1[0] = "bc";
     strArr1[1] = "cd";
     strArr1[2] = "\0";     
     strArr1[3] = "ef";
     strArr1[4] = "fg";
     strExpectedResult = "bccd\0effg";
     strConstructed = String.Concat( strArr1 );
     if ( strConstructed == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_g4v87 (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_g4v87 ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     if ( strConstructed.Equals( strExpectedResult ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_89fr (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_89fr ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     ++iCountTestcases;
     strArr1 = new String[5];
     strArr1[0] = "bc";
     strArr1[1] = "cd";
     strArr1[2] = null;      
     strArr1[3] = "ef";
     strArr1[4] = "fg";
     strExpectedResult = "bccdeffg";  
     strConstructed = String.Concat( strArr1 );
     if ( strConstructed == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_f3d6 (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_f3d6 ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     if ( strConstructed.Equals( strExpectedResult ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_173fn (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_173fn ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     ++iCountTestcases;
     strArr1 = new String[5];
     strArr1[0] = "bc";
     strArr1[1] = "cd";
     strArr1[2] = "";
     strArr1[3] = "ef";
     strArr1[4] = "fg";
     strExpectedResult = "bccdeffg";
     strConstructed = String.Concat( strArr1 );
     if ( strConstructed == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_f4j2 (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_f4j2 ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     if ( strConstructed.Equals( strExpectedResult ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_290kp (Co4000ctor_StrArr)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_290kp ,Co4000ctor_StrArr).  strConstructed==" );
       sblMsg.Append( strConstructed );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     }
   while ( false );
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4000ctor_StrArr cbA = new Co4000ctor_StrArr();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4000ctor_StrArr."  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
