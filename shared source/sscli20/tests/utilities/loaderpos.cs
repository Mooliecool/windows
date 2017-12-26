// ==++==
//
//   Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
//
// Verifier Test Driver - Negaitive Tests
//
    
using System;
using System.Reflection;
public class LoaderMis
{
 public static void Main(String[] args)
   {
   if (args.Length!=1)
     {
     Console.WriteLine("Please give the assembly name.");
     System.Environment.ExitCode = 1;		
     return;
     }
   
   try
     {
     Assembly a = Assembly.LoadFrom(args[0]);
     if (a==null)
     {
       Console.WriteLine("Fail to load assembly;; " + args[0]);
       System.Environment.ExitCode = 1;
       return;
     }
     
     Type t = a.GetType( "TestClass" );
     if ( t==null)
     {
       Console.WriteLine("Failed to load type TestClass from " + a.ToString());
       System.Environment.ExitCode = 1;
       return;
     } 
     MethodInfo y = t.GetMethod( "Main" );
     if ( y==null)
     {
       Console.WriteLine("Failed to method Main from " + t.ToString());
       System.Environment.ExitCode = 1;
       return;
     } 
     Object[] para=new Object[1];
     para[0] = null;
     Object z = y.Invoke( Activator.CreateInstance(t), para );

     Console.WriteLine("Test :: " + args[0] + " --Pass!!!");
     
     System.Environment.ExitCode = 0;
     }
   catch (Exception e)
     {
     Console.WriteLine(e.ToString());
     
     Console.WriteLine();
     Console.WriteLine("Test :: " + args[0] + " --Fail!!!");
     
     System.Environment.ExitCode = 1;
     }
   
   }
 
}
