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
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co8596ToString_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Version.ToString()"; 
 public static String s_strTFName        = "Co8596ToString_int.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String retValue;
   try
     {
     Version vTest1 = new Version (5,3);
     strLoc = "Loc_100vy";
     iCountTestcases++;
     if (vTest1.ToString(1) != "5" )
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_100aa! Expected=={0}", vTest1.ToString(1));
       }
     if (vTest1.ToString(0) != String.Empty)			
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_100aa! Expected=={0}", vTest1.ToString(0));
       }
     if (vTest1.ToString(2) != "5.3")			
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_100aa! Expected=={0}", vTest1.ToString(2));
       }
     iCountTestcases++;
     try{
     retValue = vTest1.ToString(3);
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
     }catch(ArgumentException){
     }catch(Exception ex){
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
     }
     iCountTestcases++;
     try{
     retValue = vTest1.ToString(4);
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
     }catch(ArgumentException){
     }catch(Exception ex){
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
     }
     iCountTestcases++;
     try{
     retValue = vTest1.ToString(-1);
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
     }catch(ArgumentException){
     }catch(Exception ex){
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
     }
     try{
     retValue = vTest1.ToString(5);
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
     }catch(ArgumentException){
     }catch(Exception ex){
     ++iCountErrors;	
     Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
     }
     try
       {
       retValue = vTest1.ToString(Int32.MinValue);
       ++iCountErrors;	
       Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
       }
     catch(ArgumentException)
       {
       }
     catch(Exception ex)
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
       }
     try
       {
       retValue = vTest1.ToString(Int32.MaxValue);
       ++iCountErrors;	
       Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
       }
     catch(ArgumentException)
       {
       }
     catch(Exception ex)
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
       }
     Random random = new Random();
     for(int i=0;i<20;i++)
       {
       try
	 {
	 retValue = vTest1.ToString(random.Next(-1000, 0));
	 ++iCountErrors;	
	 Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;	
	 Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
	 }
       }
     for(int i=0;i<20;i++)
       {
       try
	 {
	 retValue = vTest1.ToString(random.Next(3, 1000));
	 ++iCountErrors;	
	 Console.WriteLine( "Error_100aa! Exception now thrown=={0}", vTest1.ToString(2));
	 }
       catch(ArgumentException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;	
	 Console.WriteLine( "Error_100aa! Wrong Exception  thrown=={0}", ex.GetType().Name);
	 }
       }
     vTest1 = new Version (10,11,12);
     strLoc = "Loc_400vy";
     iCountTestcases++;
     if (vTest1.ToString(3) != "10.11.12" )
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_400aa! Expected==10.11.12");
       }
     vTest1 = new Version (550,500,2,7);
     strLoc = "Loc_700vy";
     iCountTestcases++;
     if (vTest1.ToString(4) != "550.500.2.7" )
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_700aa! Expected==550.500.2.7");
       }
     strLoc = "Loc_110zz";
     vTest1 = new Version (Int32.MaxValue,500,0,1);
     iCountTestcases++;
     if (vTest1.ToString(4) != Int32.MaxValue.ToString() + ".500.0.1" )
       {
       ++iCountErrors;	
       Console.WriteLine( "Error_100xx! Expected==" + Int32.MaxValue.ToString() + ".500.0.1");
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8596ToString_int cbA = new Co8596ToString_int();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
