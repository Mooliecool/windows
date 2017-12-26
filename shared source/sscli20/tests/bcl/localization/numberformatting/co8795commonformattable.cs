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
using System.Globalization;
class Co8795CommonFormatTable
{
 private Boolean debug = true;
 void DoThings()
   {
   Int32 iValue;
   Double dValue;
   Decimal dcmValue;
   String format;
   if(debug)
     Console.WriteLine("Rounding");
   format = "0.00";
   dValue = 1.245;
   Console.WriteLine("{0} Value: {1} ToString: {2}", dValue.GetType(), dValue, dValue.ToString(format));
   Console.WriteLine("{0} Value: {1} Round: {2}", dValue.GetType(), dValue, Math.Round(dValue, 2));
   dValue = 1.255;
   Console.WriteLine("{0} Value: {1} ToString: {2}", dValue.GetType(), dValue, dValue.ToString(format));
   Console.WriteLine("{0} Value: {1} Round: {2}", dValue.GetType(), dValue, Math.Round(dValue, 2));
   dcmValue = 1.245m;
   Console.WriteLine("{0} Value: {1} ToString: {2}", dcmValue.GetType(), dcmValue, dcmValue.ToString(format));
   Console.WriteLine("{0} Value: {1} Round: {2}", dcmValue.GetType(), dcmValue, Math.Round(dcmValue, 2));
   dcmValue = 1.255m;
   Console.WriteLine("{0} Value: {1} ToString: {2}", dcmValue.GetType(), dcmValue, dcmValue.ToString(format));
   Console.WriteLine("{0} Value: {1} Round: {2}", dcmValue.GetType(), dcmValue, Math.Round(dcmValue, 2));
   }
 static void Main()
   {
   new Co8795CommonFormatTable().DoThings();
   }
}
