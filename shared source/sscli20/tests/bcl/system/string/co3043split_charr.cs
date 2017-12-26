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
using System;
public class Co3043Split_chArr
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "String\\Co3043Split_chArr. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   bool passed = false;
   String str1 = null;
   String[] strSplit;
   String seperator = null;
   String [] result1 = {"ab","ba","cd","dc","ef","fe"};
   str1 = "ab ba cd dc ef fe";
   strSplit = str1.Split(String.Empty.ToCharArray());
   passed = true;
   for(int i=0;i<strSplit.Length;i++)
     if(!(strSplit[i].Equals(result1[i])))
       {
       Console.Out.WriteLine ("Expected: result1[i] == " + result1[i]);
       Console.Out.WriteLine ("Returned: strSplit[i] == " + strSplit[i]);
       passed = false;
       break;
       }
   iCountTestcases++;
   if(!passed)
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666t (Co3043Split_chArr.)");
     }
   iCountTestcases++;
   if(strSplit.Length != result1.Length)
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666y (Co3043Split_chArr.)");
     }
   str1 = "ab!ba#cd@dc!ef@fe";
   seperator = "!@#";
   strSplit= str1.Split(seperator.ToCharArray());
   passed = true;
   for (int i=0;i<result1.Length;i++)
     if(!(strSplit[i].Equals(result1[i])))
       {
       Console.Out.WriteLine ("Expected: result1[i] == " + result1[i]);
       Console.Out.WriteLine ("Returned: strSplit[i] == " + strSplit[i]);
       passed = false;
       break;
       }
   iCountTestcases++;
   if(!passed)
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666u (Co3043Split_chArr.)");
     }
   iCountTestcases++;
   if(strSplit.Length != result1.Length)
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666i (Co3043Split_chArr.)");
     }
   str1 = "abEbaAcdkdcCefDfe";
   seperator = "ABCDEFk";
   strSplit = str1.Split(seperator.ToCharArray());
   passed = true;
   for(int i=0;i<result1.Length; i++)
     if(!(strSplit[i].Equals(result1[i])))
       {
       Console.Out.WriteLine ("Expected: result1[i] == " + result1[i]);
       Console.Out.WriteLine ("Returned: strSplit[i] == " + strSplit[i]);
       passed = false;
       break;
       }
   iCountTestcases++;
   if(!passed)
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666o (Co3043Split_chArr.)");
     }
   str1 = "ab.ba1cd7dc6ef9fe4";
   seperator = "145679.";
   strSplit = str1.Split(seperator.ToCharArray());
   passed = true;
   for(int i=0;i<result1.Length;i++)
     if(!(strSplit[i].Equals(result1[i])))
       {
       Console.Out.WriteLine ("Expected: result1[i] == " + result1[i]);
       Console.Out.WriteLine ("Returned: strSplit[i] == " + strSplit[i]);
       passed = false;
       break;
       }
   iCountTestcases++;
   if(!passed)
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666p (Co3043Split_chArr.)");
     }
   str1 = "abcdefghijlmnopqrs";
   seperator = "12345";
   strSplit = str1.Split(seperator.ToCharArray());
   iCountTestcases++;
   if(!(strSplit[0].Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666m (Co3043Split_chArr.)");
     }
   str1 = "abcdefghijkl";
   seperator = "DEF";
   strSplit = str1.Split(seperator.ToCharArray());
   iCountTestcases++;
   if(!(strSplit[0].Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666n (Co3043Split_chArr.)");
     }
   str1 = "abcdefghiDSFGJSD";
   strSplit = str1.Split(String.Empty.ToCharArray());
   iCountTestcases++;
   if(!(strSplit[0].Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666b (Co3043Split_chArr.)");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3043Split_chArr cb0 = new Co3043Split_chArr();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3043Split_chArr." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
