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
using System.Globalization;
using GenStrings;
using System.Text;
using System;
public class Co1454ctor_Strings
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1454ctor_Strings.  runTest started." );
   int iCountErrors = 0;
   StringBuilder sbl2 = null;
   String str2b = null;
   String str2c = null;
   String str2d = null;
   String str2e = null;
   String str2_All = null;
   String str2_Some = null;
   String str9 = null;
   IntlStrings intl = new IntlStrings();
   str9 = intl.GetString(6, true, true);
   str2_All = str9;
   if ( ! str9.Equals( str2_All ) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine(  "POINTTOBREAK:  Error E_6555im! (Co1454ctor_Strings)"  );
     }
   str2b = "Bb";
   str2c = "Cc";
   str2d = "Dd";
   str2e = "Ee";
   str2_All = "BbCcDdEe";
   str9 =  "BbCcDdEe" ;
   if ( ! str9.Equals( str2_All ) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine(  "POINTTOBREAK:  Error E_638im! (Co1454ctor_Strings)"  );
     }
   sbl2 = new StringBuilder( "BbCcDd" );
   sbl2.Append( str2e );
   str9 = sbl2.ToString();
   if ( ! str9.Equals( str2_All ) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine(  "POINTTOBREAK:  Error E_517cw! (Co1454ctor_Strings)"  );
     }
   str9 =  "Bb" + "Cc" +"Dd" ;
   str2_Some = "BbCcDd";
   if ( ! str9.Equals( str2_Some ) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine(  "POINTTOBREAK:  Error E_460is! (Co1454ctor_Strings)"  );
     }
   str9 =  str2b + str2c + str2d ;
   str9 = new StringBuilder( str9 ).Append( str2e ).ToString();
   if ( ! str9.Equals( str2_All ) )
     {
     iCountErrors += 1;
     Console.Error.WriteLine(  "POINTTOBREAK:  Error E_253xi! (Co1454ctor_Strings)"  );
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co1454ctor_Strings o2Co1454ctor_Strings = null;
   try
     {
     o2Co1454ctor_Strings = new Co1454ctor_Strings();
     bResult = o2Co1454ctor_Strings.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     sblW = new StringBuilder( "POINTTOBREAK:  String- Co1454ctor_Strings main caught Exception!  exc.ToString==" );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1454ctor_Strings."  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
} 
