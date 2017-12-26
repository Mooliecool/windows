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
using System.Reflection;
public class Co1186ctor_String
{
 protected static String s_strGuid2 = "123456Ab-1629-11d2-8879-00c04fb990b0";
 public Boolean runTest()
   {
   Console.WriteLine( "Co1186ctor_String.cs  runTest() started." );
   StringBuilder sblMsg = null;
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
	 Console.WriteLine(  "POINTTOBREAK:  E_251wn!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_251wn ,Co1186ctor_String)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 sblMsg.Append( " ,guid3.ToString()==" );
	 sblMsg.Append( guid3.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       if ( guid2.GetHashCode() != guid3.GetHashCode() )
	 {
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_228kc!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_228kc ,Co1186ctor_String)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 sblMsg.Append( " ,guid3.ToString()==" );
	 sblMsg.Append( guid3.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       try
	 {
	 ++iCountTestcases;
	 guid2 = new Guid( "123456Ab 1629-11d2-8879-00c04fb990b0" );  
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_312to!  (Co1186ctor_String)"  );
	 }
       catch ( FormatException )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_311so!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_311so ,Co1186ctor_String)  exc.ToString()==" );
	 sblMsg.Append( exc.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       try
	 {
	 ++iCountTestcases;
	 guid2 = new Guid( "123456A-1629-11d2-8879-00c04fb990b0" );  
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_323yp!  (Co1186ctor_String)"  );
	 }
       catch ( FormatException )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_324rp!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_324rp ,Co1186ctor_String)  exc.ToString()==" );
	 sblMsg.Append( exc.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       try
	 {
	 ++iCountTestcases;
	 guid2 = new Guid( "123456Az-1629-11d2-8879-00c04fb990b0" );  
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_334if!  (Co1186ctor_String)"  );
	 }
       catch ( FormatException  )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_335nf!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_335nf ,Co1186ctor_String)  exc.ToString()==" );
	 sblMsg.Append( exc.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       guid2 = new Guid( s_strGuid2 );
       guid3 = new Guid( s_strGuid2 );
       ++iCountTestcases;
       if ( ! guid2.Equals( guid3 ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_241xl!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_241xl ,Co1186ctor_String)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 sblMsg.Append( " ,guid3.ToString()==" );
	 sblMsg.Append( guid3.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       if ( 0 != String.Compare( guid2.ToString(), s_strGuid2 , true ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_266uj!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_266uj ,Co1186ctor_String)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       ++iCountTestcases;
       if ( guid2.GetHashCode() != 0x41D47C9 )
	 {
	 ++iCountErrors;
	 Console.WriteLine(  "POINTTOBREAK:  E_237hg!  (Co1186ctor_String)"  );
	 sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_237hg ,Co1186ctor_String)  guid2.ToString()==" );
	 sblMsg.Append( guid2.ToString() );
	 Console.WriteLine(  sblMsg.ToString()  );
	 }
       iCountTestcases++;
       try {
       new Guid((String)null);
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_587yb! ArgumentNullException expected");
       } catch (ArgumentNullException aExc) {
       Console.WriteLine("Info_298ch! Caught expected ArgumentNullException, exc=="+aExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_2098c! ArgumentNullException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("123456123445");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_209ux! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_298hw! Caught expected FormatException , exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_09u2d! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("{00000000-0000-0000-0000-000000000000");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_298hd! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_029uc! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_29hcs! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("{00000000-0000-0000-0000-000000000000}0");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_28hxx! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_1289x! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( "POINTTOBREAK: Error_298cy! FormatException expected, got exc=="+exc.ToString());
       }
       try {
       new Guid("00000000-0000-0000-0000-0000000000000");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_239uu! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_3498f! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: ERror_20jf9 FormatException expected, got exc=="+exc.ToString());
       }
       try {
       new Guid("0000000-00000-0000-0000-000000000000");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_208cx! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_298hc! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_389ux! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("00000000-00000-000-0000-000000000000");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_2hx83! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_298hx! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_r98hc! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("00000000-0000-00000-000-000000000000");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_27hxy! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_298fh! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_9yc88! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("00000000-0000-0000-00000-00000000000");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_28yc8! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_98y8e! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_8938x! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("00000000-0000-0000-0000-00L000000000");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_209ux! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_290xh! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_019uz! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_109sj! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_9828z! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_209uz! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       new Guid("{00000000d,0x0000}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_109us! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_1099x! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_2875v! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       Guid g = new Guid("{0x,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_1098u! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_10uzx! Caught expected FormatException , exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_109ux! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       Guid g = new Guid("{0x0000,0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_1890u! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_18yx2! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTTOBREAK: Error_109ux! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       Guid g = new Guid("{0x0000,0x0000,0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_1890u! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_18yx2! Caught expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTTOBREAK: Error_109ux! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       Guid g = new Guid("{0x0000,0x0000,0x0000, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_98s81! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_98v81! Cauht expected FormatException, exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_010x9! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       Guid g = new Guid("{0x0000,0x0000,0x0000,{0x00,0x00,00,0x00,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_2998s! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_2091x! Caught expected FormatException , exc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_109uc! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       Guid g = new Guid("{0x0000,0x0000,0x0000,{0x00,0x00,0x00,0x,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_1908x! FormatException expected");
       } catch (FormatException fExc) {
       Console.WriteLine("Info_198yc! Caught expected FormatException, fExc=="+fExc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_18yv8! FormatException expected, got exc=="+exc.ToString());
       }
       iCountTestcases++;
       try {
       Guid g = new Guid("{0x0000,0x0000,0x0000,{0x00,0x00,0xk0,0x00,0x00,0x00,0x00,0x00}}");
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_9058c! FormatException expected");
       } catch (FormatException fexc) {
       Console.WriteLine("Info_298gr! Caught expected FormatException , fexc=="+fexc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine("POINTTOBREAK: Error_298g7! FormatException expected, got exc=="+exc.ToString());
       }
       Guid gg = new Guid("{0x11111111,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11}}");
       iCountTestcases++;
       if(!gg.ToString().Equals("11111111-0000-0000-0000-000000000011"))
	 {
	 iCountErrors++;
	 Console.WriteLine("POINTTOBREAK: Error_398hc! Incorrect guid=="+gg.ToString());
	 }
       } while (false);
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine(  "POINTTOBREAK: find error E_876yxw, general exception error! (Co1186ctor_String)"  );
     sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_876yxw ,Co1186ctor_String)  exc_general.ToString()==" );
     sblMsg.Append( exc_general.ToString() );
     Console.WriteLine(  sblMsg.ToString()  );
     }
   if ( iErrorBits != 0 )
     {
     ++iCountErrors;
     Console.WriteLine(  "POINTTOBREAK: find error E_678xy (Co1186ctor_String)"  );
     sblMsg = new StringBuilder( "EXTENDEDINFO:  (E_678xy ,Co1186ctor_String)  iErrorBits==" );
     sblMsg.Append( iErrorBits );
     Console.WriteLine(  sblMsg.ToString()  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Write( "Guid\\Co1186ctor_String.cs:  paSs.  iCountTestcases==" );
     Console.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Write( "Co1186ctor_String.cs iCountErrors==" );
     Console.WriteLine( iCountErrors );
     Console.WriteLine(  "Co1186ctor_String.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co1186ctor_String cbA = new Co1186ctor_String();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.WriteLine(  "Co1186ctor_String.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
