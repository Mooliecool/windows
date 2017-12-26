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
public class Co1188GetHashCode_Dupl2
{
 protected static String s_strGuid2 = "123456Ab-1629-11d2-8879-00c04fb990b0";
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co1188GetHashCode_Dupl2.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int iErrorBits = 0;  
   Guid guid2;
   Guid guid3;
   try  
     {
     do
       {
       ++iCountTestcases;
       guid2 = Guid.NewGuid();
       ++iCountTestcases;
       guid3 = new Guid( guid2.ToString() );
       ++iCountTestcases;
       if ( ! guid2.Equals( guid3 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  E_251wn!  (Co1188GetHashCode_Dupl2)"  );
	 sblMsg.Length =  0 ;
	 sblMsg.Append( "EXTENDEDINFO:  (E_251wn ,Co1188GetHashCode_Dupl2)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 sblMsg.Append( " ,guid3.ToString()==" );
	 sblMsg.Append( guid3.ToString() );
	 Console.Error.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       if ( guid2.GetHashCode() != guid3.GetHashCode() )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  E_228kc!  (Co1188GetHashCode_Dupl2)"  );
	 sblMsg.Length =  0 ;
	 sblMsg.Append( "EXTENDEDINFO:  (E_228kc ,Co1188GetHashCode_Dupl2)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 sblMsg.Append( " ,guid3.ToString()==" );
	 sblMsg.Append( guid3.ToString() );
	 Console.Error.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       guid2 = new Guid( s_strGuid2 );
       guid3 = new Guid( s_strGuid2 );
       ++iCountTestcases;
       if ( ! guid2.Equals( guid3 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  E_241xl!  (Co1188GetHashCode_Dupl2)"  );
	 sblMsg.Length =  0 ;
	 sblMsg.Append( "EXTENDEDINFO:  (E_241xl ,Co1188GetHashCode_Dupl2)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 sblMsg.Append( " ,guid3.ToString()==" );
	 sblMsg.Append( guid3.ToString() );
	 Console.Error.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       if ( 0 != String.Compare( guid2.ToString(), s_strGuid2 , true ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  E_266uj!  (Co1188GetHashCode_Dupl2)"  );
	 sblMsg.Length =  0 ;
	 sblMsg.Append( "EXTENDEDINFO:  (E_266uj ,Co1188GetHashCode_Dupl2)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 Console.Error.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       if ( guid2.GetHashCode() != 0x41D47C9 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  E_237hg!  (Co1188GetHashCode_Dupl2)"  );
	 sblMsg.Length =  0 ;
	 sblMsg.Append( "EXTENDEDINFO:  (E_237hg ,Co1188GetHashCode_Dupl2)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 Console.Error.WriteLine(  sblMsg.ToString()  );
	 }
       } while (false);
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: find error E_876yxw, general exception error! (Co1188GetHashCode_Dupl2)"  );
     sblMsg.Length =  0 ;
     sblMsg.Append( "EXTENDEDINFO:  (E_876yxw ,Co1188GetHashCode_Dupl2)  exc_general.ToString()==" );
     sblMsg.Append( exc_general.ToString() );
     Console.Error.WriteLine(  sblMsg.ToString()  );
     }
   if ( iErrorBits != 0 )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: find error E_678xy (Co1188GetHashCode_Dupl2)"  );
     sblMsg.Length =  0 ;
     sblMsg.Append( "EXTENDEDINFO:  (E_678xy ,Co1188GetHashCode_Dupl2)  iErrorBits==" );
     sblMsg.Append( iErrorBits );
     Console.Error.WriteLine(  sblMsg.ToString()  );
     }
   if ( iCountErrors == 0 )
     {
     sblMsg.Length =  0 ;
     sblMsg.Append( "Guid\\Co1188GetHashCode_Dupl2.cs:  paSs.  iCountTestcases==" );
     sblMsg.Append( iCountTestcases );
     Console.Error.WriteLine( sblMsg.ToString() );
     return true;
     }
   else
     {
     sblMsg.Length =  0 ;
     sblMsg.Append( "Co1188GetHashCode_Dupl2.cs iCountErrors==" );
     sblMsg.Append( iCountErrors );
     Console.Error.WriteLine( sblMsg.ToString() );
     Console.Error.WriteLine(  "Co1188GetHashCode_Dupl2.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1188GetHashCode_Dupl2 cbA = new Co1188GetHashCode_Dupl2();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co1188GetHashCode_Dupl2.cs"  );
     sblMsg.Length =  0 ;
     sblMsg.Append( "EXTENDEDINFO: (E_999zzz) " );
     sblMsg.Append( exc.ToString() );
     Console.Error.WriteLine(  sblMsg.ToString()  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1188GetHashCode_Dupl2.cs"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
