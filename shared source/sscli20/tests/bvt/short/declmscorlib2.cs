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
using System.Security;
using System.Security.Permissions;
[assembly:SecurityPermissionAttribute(SecurityAction.RequestMinimum, Execution=true),
	 SecurityPermissionAttribute(SecurityAction.RequestMinimum, ControlPolicy=true)]
  [assembly:PermissionSetAttribute(SecurityAction.RequestOptional, Name="Nothing")]
  public class MinPermCheck
{
 public static void Main(String[] args)
   {
   try
     {
     SecurityManager.PolicyHierarchy();
     Console.Out.WriteLine("Policy manipulation allowed --  Pass.");
     System.Environment.ExitCode = 0;
     }
   catch(Exception e)
     {
     Console.Out.WriteLine(e.ToString());
     Console.Out.WriteLine("Exception thrown -- Fail.");
     System.Environment.ExitCode = 101;
     }
   }
}
