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

public interface IFoo
{
	
} 

public class FooClass : IFoo
{

}

public struct FooStruct : IFoo
{

}

public class GenClass<T> where T : IFoo
{
	public static IFoo ConvertToConstraint(T t)
	{
        	return t;
	}
}

public struct GenStruct<T> where T : IFoo
{
	public static IFoo ConvertToConstraint(T t)
	{
        	return t;
    	}
}
public class Test
{
	public static int counter = 0;
	public static bool result = true;
	public static void Eval(bool exp)
	{
		counter++;
		if (!exp)
		{
			result = exp;
			Console.WriteLine("Test Failed at location: " + counter);
		}
	
	}
	
	public static int Main()
	{
		Eval(GenClass<FooClass>.ConvertToConstraint(new FooClass()).GetType().Equals(typeof(FooClass)));
		Eval(GenClass<FooStruct>.ConvertToConstraint(new FooStruct()).GetType().Equals(typeof(FooStruct)));

		Eval(GenStruct<FooClass>.ConvertToConstraint(new FooClass()).GetType().Equals(typeof(FooClass)));
		Eval(GenStruct<FooStruct>.ConvertToConstraint(new FooStruct()).GetType().Equals(typeof(FooStruct)));

		if (result)
		{
			Console.WriteLine("Test Passed");
			return 0;
		}
		else
		{
			Console.WriteLine("Test Failed");
			return 1;
		}
	}
		
}

