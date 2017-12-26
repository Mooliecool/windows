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

// Tests all special cases for Math.Pow

using System;

class My {
   static double NaN = Double.NaN;
   static double Infinity = Double.PositiveInfinity;

   static double[] values = { 0, NaN, -Infinity, Infinity, -3.5, 3.5, -3, 3, -2.5, 2.5, -2, 2, -1, 1, 
        -1E300, 1E300, -1E12-1, 1E12+1, -1E12-0.5, 1E12+0.5, -1E12, 1E12, -1E9, 1E9, -1E9-0.5, 1E9+0.5,
        -1E9-1, 1E9+1, -1E-8, 1E-8, -1E-300, 1E-300 };

   static double[,] results = {
{1,NaN,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0},
{NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,0,Infinity,0,-Infinity,0,Infinity,0,Infinity,0,-Infinity,0,Infinity,0,-Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,-Infinity,0,Infinity,0,Infinity},
{1,NaN,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity},
{1,NaN,0,Infinity,NaN,NaN,-0.0233236151603499,-42.875,NaN,NaN,0.0816326530612245,12.25,-0.285714285714286,-3.5,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,0.0124669967072851,80.2117802289664,0.0233236151603499,42.875,0.0436344884754979,22.9176514939904,0.0816326530612245,12.25,0.285714285714286,3.5,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.99999998747237,1.00000001252763,1,1},
{1,NaN,0,Infinity,NaN,NaN,-0.037037037037037,-27,NaN,NaN,0.111111111111111,9,-0.333333333333333,-3,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,0.0213833433033195,46.7653718043597,0.037037037037037,27,0.0641500299099584,15.5884572681199,0.111111111111111,9,0.333333333333333,3,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999989013877,1.00000001098612,1,1},
{1,NaN,0,Infinity,NaN,NaN,-0.064,-15.625,NaN,NaN,0.16,6.25,-0.4,-2.5,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,0.0404771540501553,24.7052942200655,0.064,15.625,0.101192885125388,9.88211768802619,0.16,6.25,0.4,2.5,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999990837093,1.00000000916291,1,1},
{1,NaN,0,Infinity,NaN,NaN,-0.125,-8,NaN,NaN,0.25,4,-0.5,-2,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,0.0883883476483184,11.3137084989848,0.125,8,0.176776695296637,5.65685424949238,0.25,4,0.5,2,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999993068528,1.00000000693147,1,1},
{1,NaN,NaN,NaN,NaN,NaN,-1,-1,NaN,NaN,1,1,-1,-1,1,1,-1,-1,NaN,NaN,1,1,1,1,NaN,NaN,-1,-1,NaN,NaN,NaN,NaN},
{1,NaN,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,NaN,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,0,Infinity,-1E-300,-1E+300,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,1E-300,1E+300,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999993092268579,1.00000690777914,1,1},
{1,NaN,0,Infinity,NaN,NaN,-9.99999999997E-37,-1.000000000003E+36,NaN,NaN,9.99999999998E-25,1.000000000002E+24,-9.99999999999E-13,-1000000000001,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,9.999999999965E-43,1.0000000000035E+42,9.99999999997E-37,1.000000000003E+36,9.999999999975E-31,1.0000000000025E+30,9.99999999998E-25,1.000000000002E+24,9.99999999999E-13,1000000000001,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999723689827,1.00000027631025,1,1},
{1,NaN,0,Infinity,NaN,NaN,-9.999999999985E-37,-1.0000000000015E+36,NaN,NaN,9.99999999999E-25,1.000000000001E+24,-9.999999999995E-13,-1000000000000.5,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,9.9999999999825E-43,1.00000000000175E+42,9.999999999985E-37,1.0000000000015E+36,9.9999999999875E-31,1.00000000000125E+30,9.99999999999E-25,1.000000000001E+24,9.999999999995E-13,1000000000000.5,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999723689827,1.00000027631025,1,1},
{1,NaN,0,Infinity,NaN,NaN,-1E-36,-1E+36,NaN,NaN,1E-24,1E+24,-1E-12,-1000000000000,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,1E-42,1E+42,1E-36,1E+36,1E-30,1E+30,1E-24,1E+24,1E-12,1000000000000,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999723689827,1.00000027631025,1,1},
{1,NaN,0,Infinity,NaN,NaN,-1E-27,-1E+27,NaN,NaN,1E-18,1E+18,-1E-09,-1000000000,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,3.16227766016838E-32,3.16227766016838E+31,1E-27,1E+27,3.16227766016838E-23,3.16227766016838E+22,1E-18,1E+18,1E-09,1000000000,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999792767363,1.00000020723268,1,1},
{1,NaN,0,Infinity,NaN,NaN,-9.999999985E-28,-1.0000000015E+27,NaN,NaN,9.99999999E-19,1.000000001E+18,-9.999999995E-10,-1000000000.5,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,3.16227765463439E-32,3.16227766570237E+31,9.999999985E-28,1.0000000015E+27,3.16227765621553E-23,3.16227766412123E+22,9.99999999E-19,1.000000001E+18,9.999999995E-10,1000000000.5,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999792767363,1.00000020723268,1,1},
{1,NaN,0,Infinity,NaN,NaN,-9.99999997E-28,-1.000000003E+27,NaN,NaN,9.99999998E-19,1.000000002E+18,-9.99999999E-10,-1000000001,0,Infinity,0,-Infinity,NaN,NaN,0,Infinity,0,Infinity,NaN,NaN,0,-Infinity,NaN,NaN,NaN,NaN},
{1,NaN,0,Infinity,3.16227764910041E-32,3.16227767123635E+31,9.99999997E-28,1.000000003E+27,3.16227765226268E-23,3.16227766807407E+22,9.99999998E-19,1.000000002E+18,9.99999999E-10,1000000001,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0.999999792767363,1.00000020723268,1,1},
{1,NaN,Infinity,0,NaN,NaN,-1E+24,-1E-24,NaN,NaN,1E+16,1E-16,-100000000,-1E-08,Infinity,0,-Infinity,0,NaN,NaN,Infinity,0,Infinity,0,NaN,NaN,-Infinity,0,NaN,NaN,NaN,NaN},
{1,NaN,Infinity,0,1E+28,1E-28,1E+24,1E-24,1E+20,1E-20,1E+16,1E-16,100000000,1E-08,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,1.00000018420682,0.99999981579321,1,1},
{1,NaN,Infinity,0,NaN,NaN,-Infinity,0,NaN,NaN,Infinity,0,-1E+300,-1E-300,Infinity,0,-Infinity,0,NaN,NaN,Infinity,0,Infinity,0,NaN,NaN,-Infinity,0,NaN,NaN,NaN,NaN},
{1,NaN,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,1E+300,1E-300,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,Infinity,0,1.00000690777914,0.999993092268579,1,1}
};

   static void Main() {
       bool failed = false;
       for (int i = 0; i < values.Length; i++) {
          for (int j = 0; j < values.Length; j++) {
              double expected = results[i,j];
              double actual = Math.Pow(values[i], values[j]);
              if (expected == actual)
                continue;
              if (Double.IsNaN(expected) && Double.IsNaN(actual))
                continue;
              double diff = Math.Abs((expected - actual) / (expected + actual));
              if (0.0 < diff && diff < 0.001)
                continue;
              Console.WriteLine("pow("+values[i]+","+values[j]+")" + " Expected:"+expected + " Actual:"+actual);
              failed = true;
          }
       }
       if (failed) { Console.WriteLine("FAILED"); Environment.Exit(1); }
       Console.WriteLine("PASSED");
   }
};
