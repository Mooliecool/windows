// ==++==
//
//   Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
    
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
				MethodInfo y = t.GetMethod( "Main" );
				Object[] para=new Object[1];
				para[0] = null;
				Object z = y.Invoke( Activator.CreateInstance(t), para );

				Console.WriteLine("Test :: " + args[0] + "-- No exception --Fail!!!");

				System.Environment.ExitCode = 1;
			}
			catch (TargetInvocationException e)
			{
				if (e.InnerException is System.MissingMethodException)
				{
					Console.WriteLine("MissingMethodException");
					Console.WriteLine("Test :: " + args[0] + " --Pass!!!");
					System.Environment.ExitCode = 0;
				}
				else if (e.InnerException is System.Security.VerificationException)
				{
					System.Security.VerificationException e1 = (System.Security.VerificationException)e.InnerException;;
					if (e1!=null)
					{
						Console.WriteLine(e1.ToString());

						Console.WriteLine();
						Console.WriteLine("Test :: " + args[0] + "-- Security Exception --Pass!!!");
						System.Environment.ExitCode = 0;
					}
					else
					{
						Console.WriteLine(e.ToString());

						Console.WriteLine();
						Console.WriteLine("Test :: " + args[0] + "-- Security Exception --Fail!!!");
						System.Environment.ExitCode = 1;
					}
				}
                                else if (e.InnerException is System.InvalidProgramException)
				{
					Console.WriteLine("InvalidProgramException");
					Console.WriteLine("Test :: " + args[0] + "-- Invalid Code Exception --Pass!!!");
					System.Environment.ExitCode = 0;
				}
                                else if (e.InnerException is System.Runtime.Remoting.RemotingException )
				{
					Console.WriteLine("Runtime.Remoting.RemotingException");
					Console.WriteLine("Test :: " + args[0] + "-- Remoting Exception --Pass!!!");
					System.Environment.ExitCode = 0;
				}
                                else if (e.InnerException is System.TypeLoadException )
				{
					Console.WriteLine("TypeLoadException");
					Console.WriteLine("Test :: " + args[0] + "-- TypeLoadException --Pass!!!");
					System.Environment.ExitCode = 0;
				}
                                else if (e.InnerException is System.ArgumentException )
				{
					Console.WriteLine("ArgumentException");
					Console.WriteLine("Test :: " + args[0] + "-- ArgumentException --Pass!!!");
					System.Environment.ExitCode = 0;
				}
                                else if (e.InnerException is System.NotSupportedException )
				{
					Console.WriteLine("NotSupportedException");
					Console.WriteLine("Test :: " + args[0] + "-- NotSupported --Pass!!!");
					System.Environment.ExitCode = 0;
				}
				else
				{
					Console.WriteLine(e.ToString());

					Console.WriteLine();
					Console.WriteLine("Test :: " + args[0] + "-- Unknown exception --Fail!!!");
					System.Environment.ExitCode = 1;
				}

			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());

				Console.WriteLine();
				Console.WriteLine("Test :: " + args[0] + "-- Unknown exception --Fail!!!");

				System.Environment.ExitCode = 1;
			}
			
		}
    
    }
