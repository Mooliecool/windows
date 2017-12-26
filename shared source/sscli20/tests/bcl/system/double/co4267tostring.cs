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
public class Co4267ToString
{
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co4267ToString.cs  runTest() started." );
   StringBuilder sblMsg = null;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strMade = null;
   String strKnown = null;
   double dubValue = 0.0;
   do
     {
     ++iCountTestcases;
     dubValue = 0.0;
     strMade = dubValue.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_dw13 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_dw13 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strKnown = ( "0" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_fj22 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_fj22 ,Co4267ToString).  dubValue== " );
       sblMsg.Append( dubValue );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     ++iCountTestcases;
     dubValue = 7.3;
     strMade = dubValue.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_mm35 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_mm35 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strKnown = ( "7.3" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_fw94 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_ss34 ,Co4267ToString).  dubValue== " );
       sblMsg.Append( dubValue );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     ++iCountTestcases;
     dubValue = 1.79769313486231570E+308;
     strMade = dubValue.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_dk84 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_dk84 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     Console.WriteLine( "Testing " + strMade );
     Console.WriteLine( "Testing " + dubValue );
     strKnown = ( "1.79769313486232E+308" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_de42 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_de42 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       Console.Error.WriteLine( "strKnown == " + strKnown );
       }
     ++iCountTestcases;
     dubValue = -1.79769313486231570E+308;
     strMade = dubValue.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_le21 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_le21 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strKnown = ( "-1.79769313486232E+308" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_af24 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_af24 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       Console.Error.WriteLine( "strKnown == " + strKnown );
       }
     ++iCountTestcases;
     dubValue = 1.0/0.0;
     strMade = dubValue.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_ss24 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_ss24 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strKnown = ( "Infinity" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_mm24 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_mm24 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       Console.Error.WriteLine( "strKnown == " + strKnown );
       }
     ++iCountTestcases;
     dubValue = -1.0/0.0;
     strMade = dubValue.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_pe25 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_pe25 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strKnown = ( "-Infinity" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_gh43 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_gh43 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       Console.Error.WriteLine( "strKnown == " + strKnown );
       }
     ++iCountTestcases;
     dubValue = -1.0/0.0;
     strMade = dubValue.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_nr59 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_nr59 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strKnown = ( "-Infinity" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_rr33 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_rr33 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       Console.Error.WriteLine( "strKnown == " + strKnown );
       }
     ++iCountTestcases;
     double dubValue2 = 0.0/0.0;
     strMade = dubValue2.ToString();
     if ( strMade == null )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_bo75 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_bo75 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       }
     strKnown = ( "NaN" );
     if ( strMade.Equals( strKnown ) == false )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_rr33 (Co4267ToString)"  );
       sblMsg = new StringBuilder( "EXTENDEDINFO: (E_rr33 ,Co4267ToString).  strMade== " );
       sblMsg.Append( strMade );
       Console.Error.WriteLine(  sblMsg.ToString()  );
       Console.Error.WriteLine( "strKnown == " + strKnown );
       }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Double\\Co4267ToString.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4267ToString.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4267ToString.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co4267ToString cbA = new Co4267ToString();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4267ToString.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
