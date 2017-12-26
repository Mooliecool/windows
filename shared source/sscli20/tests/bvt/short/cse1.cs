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
public class CL
{
 public int item;
};
public class CSE1
{
 static int sa;
 static int sb;
 static int [] arr1d = { 10, 20, 30, 40, 50 };
 static int DoIt(ref int pa)
   { 
   int result = 0;
   if (sa + sb == 0)
     result++;
   pa = 1;
   if (sa + sb == 1)
     result++;
   pa = 2;
   if (sa + sb == 2)
     result++;
   pa = 3;
   if (sa + sb == 3)
     result++;
   return result;
   }
 static int DoAdd(ref int pa)
   { 
   int result = 0;
   if (sa + sb == 0)
     result+=arr1d[sa+sb];
   pa = 1;
   if (sa + sb == 1)
     result+=arr1d[sa+sb];
   pa = 2;
   if (sa + sb == 2)
     result+=arr1d[sa+sb];
   pa = 3;
   if (sa + sb == 3)
     result+=arr1d[sa+sb];
   result+=arr1d[sa+sb+1];
   return result;
   }
 static int DoSub(ref int pa)
   { 
   int result = 0;
   if (sa - sb == 3)
     result+=arr1d[sa-sb-3];
   pa = 1;
   if (sa - sb == 1)
     result+=arr1d[sa-sb];
   pa = 2;
   if (sa - sb == 2)
     result+=arr1d[sa-sb];
   pa = 3;
   if (sa - sb == 3)
     result+=arr1d[sa-sb];
   result+=arr1d[sa-sb+1];
   return result;
   }
 static int DoMul(ref int pa)
   { 
   int result = 0;
   if (sa * sb == 3)
     result+=arr1d[sa*sb*result];
   pa = 1;
   if (sa * sb == 1)
     result+=arr1d[sa*sb];
   pa = 2;
   if (sa * sb == 2)
     result+=arr1d[sa*sb];
   pa = 3;
   if (sa * sb == 3)
     result+=arr1d[sa*sb];
   result+=arr1d[sa*sb+1];
   return result;
   }
 static int DoDiv(ref int pa)
   { 
   int result = 0;
   if (sa / sb == 3)
     result+=arr1d[sa/sb];
   pa = 1;
   if (sa / sb == 1)
     result+=arr1d[sa/sb];
   pa = 2;
   if (sa / sb == 2)
     result+=arr1d[sa/sb];
   pa = 3;
   if (sa / sb == 3)
     result+=arr1d[sa/sb-3];
   result+=arr1d[sa/sb+1];
   return result;
   }
 public static int Main(string[] args)
   {
   int result;
   sa = 0;
   sb = 0;
   result = DoIt(ref sa);
   if ((result != 4) || (sa != 3))
     {
     Console.WriteLine("testcase 0 FAILED, result is {0}, sa is {1}", result, sa);
     return 1;
     }
   sa = 0;
   result = DoAdd(ref sa);
   if ((result != 150) || (sa != 3))
     {
     Console.WriteLine("testcase 1 FAILED, result is {0}, sa is {1}", result, sa);
     return 1;
     }
   result = DoSub(ref sa);
   if ((result != 150) || (sa != 3))
     {
     Console.WriteLine("testcase 2 FAILED, result is {0}, sa is {1}", result, sa);
     return 1;
     }
   sb = 1;
   result = DoMul(ref sa);
   if ((result != 150) || (sa != 3))
     {
     Console.WriteLine("testcase 3 FAILED, result is {0}, sa is {1}", result, sa);
     return 1;
     }
   result = DoDiv(ref sa);
   if ((result != 150) || (sa != 3))
     {
     Console.WriteLine("testcase 4 FAILED, result is {0}, sa is {1}", result, sa);
     return 1;
     }
   CL CL1 = new CL();
   CL1.item = 10;
   if( CL1.item*2 < 30)
     {
     CL1.item = CL1.item*2;
     }
   else
     {
     CL1.item = 5*(CL1.item*2);
     }
   if (CL1.item*2!=40)
     {
     Console.WriteLine("testcase 5 FAILED, CL1.item is {0}", CL1.item);
     return 1;
     }
   Console.WriteLine("PASSED");
   return 0;
   }
}
