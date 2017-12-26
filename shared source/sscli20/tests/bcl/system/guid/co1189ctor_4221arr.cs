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
public class Co1189ctor_4221Arr
{
 protected static String s_strGuid2 = "123456Ab-1629-11d2-8879-00c04fb990b0";
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co1189ctor_4221Arr.cs  runTest() started." );
   StringBuilder sblMsg = null;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int iErrorBits = 0;  
   String str2 = null;
   int in1 = -2;
   short sh2 = -2;
   short sh3 = -2;
   Byte[] byArr4 = new Byte[8]; 
   Guid guid2;
   try  
     {
     do
       {
       in1 = 1;
       sh2 = 2;
       sh3 = 3;
       byArr4[0] = (Byte)0;
       byArr4[1] = (Byte)1;
       byArr4[2] = (Byte)2;
       byArr4[3] = (Byte)3;
       byArr4[4] = (Byte)4;
       byArr4[5] = (Byte)5;
       byArr4[6] = (Byte)6;
       byArr4[7] = (Byte)7;
       guid2 = new Guid( in1 ,sh2 ,sh3 ,byArr4 );
       str2 = guid2.ToString();
       ++iCountTestcases;
       if ( 0 != String.Compare( str2, "00000001-0002-0003-0001-020304050607" , true ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  E_501ax!  (Co1189ctor_4221Arr)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_501ax ,Co1189ctor_4221Arr)  str2==" );
	 sblMsg.Append( str2 );
	 Console.Error.WriteLine(  sblMsg.ToString()  );
	 }
       in1 = -1;
       sh2 = -2;
       sh3 = -3;
       byArr4[0] = unchecked((Byte)(-0));
       byArr4[1] = unchecked((Byte)(-1));
       byArr4[2] = unchecked((Byte)(-2));
       byArr4[3] = unchecked((Byte)(-3));
       byArr4[4] = unchecked((Byte)(-4));
       byArr4[5] = unchecked((Byte)(-5));
       byArr4[6] = unchecked((Byte)(-6));
       byArr4[7] = unchecked((Byte)(-7));
       guid2 = new Guid( in1 ,sh2 ,sh3 ,byArr4 );
       str2 = guid2.ToString();
       ++iCountTestcases;
       if ( 0 != String.Compare(str2, "ffffffff-fffe-fffd-00ff-fefdfcfbfaf9" , true ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK:  E_512sc!  (Co1189ctor_4221Arr)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_512sc ,Co1189ctor_4221Arr)  str2==" );
	 sblMsg.Append( str2 );
	 Console.Error.WriteLine(  sblMsg.ToString()  );
	 }
       } while (false);
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: find error E_876yxw, general exception error! (Co1189ctor_4221Arr)"  );
     sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_876yxw ,Co1189ctor_4221Arr)  exc_general.ToString()==" );
     sblMsg.Append( exc_general.ToString() );
     Console.Error.WriteLine(  sblMsg.ToString()  );
     }
   if ( iErrorBits != 0 )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: find error E_678xy (Co1189ctor_4221Arr)"  );
     sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_678xy ,Co1189ctor_4221Arr)  iErrorBits==" );
     sblMsg.Append( iErrorBits );
     Console.Error.WriteLine(  sblMsg.ToString()  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Guid\\Co1189ctor_4221Arr.cs:  paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co1189ctor_4221Arr.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co1189ctor_4221Arr.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co1189ctor_4221Arr cbA = new Co1189ctor_4221Arr();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co1189ctor_4221Arr.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
