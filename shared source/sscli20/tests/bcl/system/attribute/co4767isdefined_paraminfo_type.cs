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
using ClassLib_Attributes;
using ClassLib_Test;
namespace ClassLib_Attributes
{
  [AttributeUsage(AttributeTargets.All, AllowMultiple = false, Inherited = false)]
public class CA_AMfalse_INfalse : Attribute
  {
  public CA_AMfalse_INfalse (String s)
    {
    name = s;
    }
  public override string ToString()
    {
    return "CA_AMfalse_INfalse ToString : " + name;
    }
  public override bool Equals(Object obj)
    {
    if (obj is CA_AMfalse_INfalse) {
    return name.Equals (((CA_AMfalse_INfalse)obj).name);
    }
    else {
    return false;
    }
    }
  public string name;
  }
  [AttributeUsage(AttributeTargets.All, AllowMultiple = false, Inherited = true)]
public class CA_AMfalse_INtrue : Attribute
  {
  public CA_AMfalse_INtrue (String s)
    {
    name = s;
    }
  public override string ToString()
    {
    return "CA_AMfalse_INtrue ToString : " + name;
    }
  public override bool Equals(Object obj)
    {
    if (obj is CA_AMfalse_INtrue) {
    return name.Equals (((CA_AMfalse_INtrue)obj).name);
    }
    else {
    return false;
    }
    }
  public string name;
  }
  [AttributeUsage(AttributeTargets.All, AllowMultiple = true, Inherited = true)]
public class CA_AMtrue_INtrue : Attribute
  {
  public CA_AMtrue_INtrue (String s)
    {
    name = s;
    }
  public override string ToString()
    {
    return "CA_AMtrue_INtrue ToString : " + name;
    }
  public override bool Equals(Object obj)
    {
    if (obj is CA_AMtrue_INtrue) {
    return name.Equals (((CA_AMtrue_INtrue)obj).name);
    }
    else {
    return false;
    }
    }
  public string name;
  }
  [AttributeUsage(AttributeTargets.All, AllowMultiple = true, Inherited = false)]
public class CA_AMtrue_INfalse : Attribute
  {
  public CA_AMtrue_INfalse (String s)
    {
    name = s;
    }
  public override string ToString()
    {
    return "CA_AMtrue_INfalse ToString : " + name;
    }
  public override bool Equals(Object obj)
    {
    if (obj is CA_AMtrue_INfalse) {
    return name.Equals (((CA_AMtrue_INfalse)obj).name);
    }
    else {
    return false;
    }
    }
  public string name;
  }
} 
namespace ClassLib_Test
{
public enum MyColorEnum
{
  RED = 1,
  BLUE = 2,
  GREEN = 3
}
public class Util
{
 public static Int32 DEBUG = 0;	
 public static void print(Object str)
   {
   Console.Error.WriteLine(str.ToString());
   }
 public static void printerr(string errLoc)
   {
   string output = "POINTTOBREAK: find " + errLoc;
   print(output);
   }
 public static void printexc(Exception exc)
   {
   string output = "EXTENDEDINFO: "+ exc.ToString();
   print(output);
   }
  }
public interface MyInterface
{
 String MyMethod (
		  [
		   CA_AMfalse_INfalse ("ICA_AMfalse_INfalse")
				      ,CA_AMfalse_INtrue ("ICA_AMfalse_INtrue")
				      ,CA_AMtrue_INtrue ("ICA_AMtrue_INtrue")
				      ,CA_AMtrue_INtrue ("ICA_AMtrue_INtrue2")
				      ,CA_AMtrue_INfalse ("ICA_AMtrue_INfalse")
				      ,CA_AMtrue_INfalse ("ICA_AMtrue_INfalse2")
		  ]
		  String str
		  );
}
public class MyClass : MyInterface
{
 public ClassLib_Attributes.CA_AMfalse_INfalse caAmfInf = null;
 public ClassLib_Attributes.CA_AMfalse_INtrue caAmfInt = null;
 public ClassLib_Attributes.CA_AMtrue_INtrue caAmtInt = null;
 public ClassLib_Attributes.CA_AMtrue_INtrue caAmtInt2 = null;
 public ClassLib_Attributes.CA_AMtrue_INfalse caAmtInf = null;
 public ClassLib_Attributes.CA_AMtrue_INfalse caAmtInf2 = null;
 public int iCountAMfalseINfalseAttrs = 1;
 public int iCountAMfalseINtrueAttrs = 1; 
 public int iCountAMtrueINtrueAttrs = 2;
 public int iCountAMtrueINfalseAttrs = 2;
 public int iCountTotalAttrs = -1;
 public MyClass () {
 }
 public MyClass  (
		  [
		   CA_AMfalse_INfalse ("CA_AMfalse_INfalse")
				      ,CA_AMfalse_INtrue ("CA_AMfalse_INtrue")
				      ,CA_AMtrue_INtrue ("CA_AMtrue_INtrue")
				      ,CA_AMtrue_INtrue ("CA_AMtrue_INtrue2")
				      ,CA_AMtrue_INfalse ("CA_AMtrue_INfalse")
				      ,CA_AMtrue_INfalse ("CA_AMtrue_INfalse2")
		  ]
		  String str
		  )
   {
   iCountTotalAttrs = iCountAMfalseINfalseAttrs + iCountAMfalseINtrueAttrs
     + iCountAMtrueINtrueAttrs + iCountAMtrueINfalseAttrs;
   caAmfInf = new CA_AMfalse_INfalse ("CA_AMfalse_INfalse");
   caAmfInt = new CA_AMfalse_INtrue ("CA_AMfalse_INtrue");
   caAmtInt = new CA_AMtrue_INtrue ("CA_AMtrue_INtrue");
   caAmtInt2 =  new CA_AMtrue_INtrue ("CA_AMtrue_INtrue2");
   caAmtInf = new CA_AMtrue_INfalse ("CA_AMtrue_INfalse");
   caAmtInf2 = new CA_AMtrue_INfalse ("CA_AMtrue_INfalse2");
   }
 virtual public String MyMethod (
				 [
				  CA_AMfalse_INfalse ("CA_AMfalse_INfalse")
						     ,CA_AMfalse_INtrue ("CA_AMfalse_INtrue")
						     ,CA_AMtrue_INtrue ("CA_AMtrue_INtrue")
						     ,CA_AMtrue_INtrue ("CA_AMtrue_INtrue2")
						     ,CA_AMtrue_INfalse ("CA_AMtrue_INfalse")
						     ,CA_AMtrue_INfalse ("CA_AMtrue_INfalse2")
				 ]
				 String str
				 )
   {
   return "myMethod";
   }
 virtual public String MyProp {
 get { return "myProp"; }
 set { }
 }
 public String MyField = "myField";
 public int iCountErrors = 0;		
 public int iCountTestcases = 0;		
 virtual public bool TestIsDefined_Type (ParameterInfo param)
   {
   Util.print ("\n\n" + this.GetType ().ToString () + " - TestIsDefined_Type() started.");
   Util.print ("For ParamInfo on: " + param.Member.GetType ().ToString () + "\n");
   string strLoc="L_11_2_000";
   bool isattr = false;
   try  
     {
     do
       {
       iCountTestcases++;
       strLoc="L_11_2_001";
       iCountTestcases++;
       strLoc="L_11_2_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMfalse_INfalse), false);
       if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_11_2_gfh4 - IsDefined returned false");
       }
       iCountTestcases++;
       strLoc="L_11_3_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMfalse_INtrue), true);
       if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_11_3_gfh4 - IsDefined returned false");
       }
       iCountTestcases++;
       strLoc="L_11_4_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMtrue_INtrue), true);
       if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_11_4_gfh4 - IsDefined returned false");
       }
       iCountTestcases++;
       strLoc="L_11_5_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMtrue_INfalse), false);
       if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_11_5_gfh4 - IsDefined returned false");
       }
       iCountTestcases++;
       strLoc="L_11_6_002.2";
       isattr = Attribute.IsDefined (param, typeof (System.Attribute), true);
       if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_11_6_gfh4 - IsDefined returned false");
       }
       } while ( false );
     }
   catch( Exception exc_runTest ) {
   ++iCountErrors;
   Util.printerr ("Err_333un! - Uncaught Exception caught in TestIsDefined_Type(); strLoc == " + strLoc);
   Util.printexc (exc_runTest);
   Util.print (exc_runTest.StackTrace);
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
}	
public class MyClass2 : MyClass
{
 public ClassLib_Attributes.CA_AMfalse_INfalse caAmfInf2 = null;
 public ClassLib_Attributes.CA_AMfalse_INtrue caAmfInt2 = null;
 public ClassLib_Attributes.CA_AMtrue_INtrue caAmtInt3 = null;
 public ClassLib_Attributes.CA_AMtrue_INfalse caAmtInf3 = null;
 new public int iCountAMfalseINfalseAttrs = 0;
 new public int iCountAMfalseINtrueAttrs = 0; 
 new public int iCountAMtrueINtrueAttrs = 2;
 new public int iCountAMtrueINfalseAttrs = 1;
 public MyClass2 () {
 }
 public MyClass2 (
		  [
		   CA_AMtrue_INfalse ("CA_AMtrue_INfalse3")	
		  ]
		  String str
		  ) 
   : base (str)
   {
   iCountTotalAttrs = iCountAMfalseINfalseAttrs + iCountAMfalseINtrueAttrs
     + iCountAMtrueINtrueAttrs + iCountAMtrueINfalseAttrs;
   caAmfInf2 = new CA_AMfalse_INfalse ("CA_AMfalse_INfalse2");
   caAmfInt2 = new CA_AMfalse_INtrue ("CA_AMfalse_INtrue2");
   caAmtInt3 =  new CA_AMtrue_INtrue ("CA_AMtrue_INtrue3");
   caAmtInf3 = new CA_AMtrue_INfalse ("CA_AMtrue_INfalse3");
   }
 override public String MyMethod (
				  [
				   CA_AMtrue_INfalse ("CA_AMtrue_INfalse3")	
				  ]
				  String str
				  ) 
   {
   return "myMethod";
   }
 override public String MyProp {
 get { return "myProp"; }
 set { }
 }
 new public String MyField = "myField";
 override public bool TestIsDefined_Type (ParameterInfo param)
   {
   Util.print ("\n\n" + this.GetType ().ToString () + " - TestIsDefined_Type() started.");
   Util.print ("For ParamInfo on: " + param.Member.GetType ().ToString () + "\n");
   string strLoc="L_2_000";
   bool isattr = false;
   try  
     {
     do
       {
       iCountTestcases++;
       strLoc="L_2_001";
       iCountTestcases++;
       strLoc="L_2_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMfalse_INfalse), false);
       if (isattr) {
       iCountErrors++;
       Util.printerr ("E_2_gfh4 - IsDefined returned true");
       }
       iCountTestcases++;
       strLoc="L_3_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMfalse_INtrue), true);
       if (param.Member is ConstructorInfo) {
       if (isattr) {
       iCountErrors++;
       Util.printerr ("E_3_fkj2 - IsDefined returned true");
       }				
       }
       else if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_3_gfh4 - IsDefined returned false");
       }
       iCountTestcases++;
       strLoc="L_4_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMtrue_INtrue), true);
       if (param.Member is ConstructorInfo) {
       if (isattr) {
       iCountErrors++;
       Util.printerr ("E_4_fkj2 - IsDefined returned true");
       }				
       }
       else if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_4_gfh4 - IsDefined returned false");
       }
       iCountTestcases++;
       strLoc="L_5_002.2";
       isattr = Attribute.IsDefined (param, typeof (ClassLib_Attributes.CA_AMtrue_INfalse), false);
       if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_5_gfh4 - IsDefined returned false");
       }
       iCountTestcases++;
       strLoc="L_6_002.2";
       isattr = Attribute.IsDefined (param, typeof (System.Attribute), true);
       if (!isattr) {
       iCountErrors++;
       Util.printerr ("E_6_gfh4 - IsDefined returned false");
       }
       } while ( false );
     }
   catch( Exception exc_runTest ) {
   ++iCountErrors;
   Util.printerr ("Err_333un! - Uncaught Exception caught in TestIsDefined_Type(); strLoc == " + strLoc);
   Util.printexc (exc_runTest);
   Util.print (exc_runTest.StackTrace);
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
}
public class Co4767IsDefined_ParamInfo_Type
{
 public static string strTest = "Co4767IsDefined_ParamInfo_Type";
 public static string strPath = "";
 public static string strBug = "";
 public int iCountTestcases = 0;
 public int iCountErrors = 0;
 public void Dummy (String str)
   {
   }
 public bool RunTest()
   {
   print (strPath + strTest + "  RunTest() started.");
   string strLoc="L_000";
   MyClass myclass = new MyClass ("myclass");
   MyClass2 myclass2 = new MyClass2 ("myclass2");
   bool isattr = false;
   ParameterInfo param = null;
   ParameterInfo param2 = null;
   Type type = null;
   Type type2 = null;
   try  
     {
     iCountTestcases++;
     strLoc="L_200_001";
     param = this.GetType ().GetMethod ("Dummy").GetParameters () [0];
     if (param == null) {
     iCountErrors++;
     Util.printerr ("E_200_dj13 - param == null");
     }
     iCountTestcases++;
     strLoc="L_200_001.2";
     isattr = Attribute.IsDefined (param, typeof (System.Attribute), true);
     if (isattr) {
     iCountErrors++;
     Util.printerr ("E_200_oi32 - IsDefined returned true!c");
     }
     iCountTestcases++;
     strLoc="L_200_001.3";
     try {
     isattr = Attribute.IsDefined (param, typeof (System.Int32), true);
     iCountErrors++;
     Util.printerr ("E_200_oi32 - Should 've thrown ArgExc");
     }
     catch (ArgumentException ) {
     }
     iCountTestcases++;
     strLoc="L_200_002";
     type = myclass.GetType ();
     param = type.GetMethod ("MyMethod").GetParameters () [0];
     if (param == null) {
     iCountErrors++;
     Util.printerr ("E_200_75yhg - param == null");
     }
     iCountTestcases++;
     strLoc="L_200_002.1";
     try {
     isattr = Attribute.IsDefined (param, null, false);
     iCountErrors++;
     Util.printerr ("E_200_oi32 - Should 've thrown ArgNullExc");
     }
     catch (ArgumentException ) {
     }
     iCountTestcases++;
     strLoc="L_200_002.1.2";
     try {
     isattr = Attribute.IsDefined ((ParameterInfo) null, typeof (ClassLib_Attributes.CA_AMfalse_INfalse), false);
     iCountErrors++;
     Util.printerr ("E_200_oi322 - Should 've thrown ArgNullExc");
     }
     catch (ArgumentException ) {
     }
     type = myclass.GetType ();
     iCountTestcases++;
     strLoc="L_200_003.4";
     param = type.GetConstructor (new Type [] {typeof (String)}).GetParameters () [0];
     myclass.TestIsDefined_Type (param);
     iCountTestcases++;
     strLoc="L_200_003.5";
     param = type.GetMethod ("MyMethod").GetParameters () [0];
     myclass.TestIsDefined_Type (param);
     type2 = myclass2.GetType ();
     iCountTestcases++;
     strLoc="L_2_200_003.4";
     param2 = type2.GetConstructor (new Type [] {typeof (String)}).GetParameters () [0];
     myclass2.TestIsDefined_Type (param2);
     iCountTestcases++;
     strLoc="L_2_200_003.5";
     param2 = type2.GetMethod ("MyMethod").GetParameters () [0];
     myclass2.TestIsDefined_Type (param2);
     }
   catch( Exception exc_runTest ) {
   ++myclass2.iCountErrors;
   printerr ("Err_888un! - Uncaught Exception caught in runTest(); strLoc == " + strLoc);
   printexc (exc_runTest);
   print (exc_runTest.StackTrace);
   }
   iCountErrors = myclass.iCountErrors + myclass2.iCountErrors;
   iCountTestcases = myclass.iCountTestcases + myclass2.iCountTestcases;
   if ( iCountErrors == 0 ) {   return true; }
   else {
   print  ("Related Bugs: " + strBug);
   print ("FAiL!   " + strPath +strTest + "  iCountErrors==" +  iCountErrors.ToString ());
   return false;
   }
   }
 private static void print(Object str)
   {
   Console.Error.WriteLine(str.ToString());
   }
 private static void printerr(string errLoc)
   {
   string output = "POINTTOBREAK: find " + errLoc + " (" + strTest + ")";
   print(output);
   }
 private static void printexc(Exception exc)
   {
   string output = "EXTENDEDINFO: "+ exc.ToString();
   print(output);
   }
 public static void Main( string[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co4767IsDefined_ParamInfo_Type coA = new Co4767IsDefined_ParamInfo_Type();
   try {
   bResult = coA.RunTest();
   }
   catch (Exception exc_main) {
   bResult = false;
   printerr ("Err_999zzz! - Uncaught Exception caught in main(): ");
   printexc (exc_main);
   print (exc_main.StackTrace);
   }
   if (!bResult)
     print ("PATHTOSOURCE:  " + strPath + strTest + "  FAiL!");
   Environment.ExitCode = ( (bResult) ? 0 : 11);  
   }
}
}
