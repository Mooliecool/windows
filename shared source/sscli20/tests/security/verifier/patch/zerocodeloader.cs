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

//
// Zero Sized Class Test Loader 
//
    
using System;
using System.Reflection;
public class ZeroSizedClassLoader
{
 public static void Main(String[] args)
   {
 
   try
     {
       Assembly a = Assembly.Load("zerocode");
       if (a==null)
       {
         Console.WriteLine("Fail to load zerocode ");
         System.Environment.ExitCode = 1;
         return;
       }
     
       Type t = a.GetType( "TestClass" );
       MethodInfo y = t.GetMethod( "Main" );
       Object[] para=new Object[1];
       para[0] = null;
       Object z = y.Invoke( Activator.CreateInstance(t), para );
     
       Console.WriteLine("ZeroSized Test -- No exception --Fail!!!");
     
       System.Environment.ExitCode = 1;
     }
   catch (System.TypeLoadException e)
     {
       Console.WriteLine("ZeroSized Test -- TypeLoad Exception --Pass!!!");
       System.Environment.ExitCode = 0;
     }
   catch (Exception e)
     {
       Console.WriteLine(e.ToString());
     
       Console.WriteLine();
       Console.WriteLine("ZeroSized Test -- Unknown exception --Fail!!!");
     
       System.Environment.ExitCode = 1;
     }
   
   }
 
}
