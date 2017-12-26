//------------------------------------------------------------------------------
// <copyright file="reflectioninvoke.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.IO;
using System.Reflection;

public class Invoke
{
  /// <summary>
  /// Getting or setting the value of a specified property are actions available through the BindingFlags enumeration. 
  /// The second parameter of InvokeMethod is a combination of the BindingFlags actions you specify. 
  /// For example, if you want to invoke a static method on a class, you would include the static element in BindingFlags, 
  /// and the InvokeMethod BindingFlag. 
  /// 
  /// The following example demonstrates how to invoke a hypothetical method called SayHello, 
  /// where SayHello is a static method. 
  /// </summary>
  /// <param name="cmdargs"></param>
  public static void Main(string [] cmdargs)
	{
		// Declare a type object, used to call our InvokeMember method.
    //Call a static method
		Type t = typeof(TestClass);

		Console.WriteLine();
		Console.WriteLine("Invoking the static method SayHello()");
		Console.WriteLine("---------------------------------");

    t.InvokeMember("SayHello", BindingFlags.InvokeMethod, null, null, new object [] {});

   // Examine the rest of the parameters that were passed to the Invoke method. 
   // The first null argument passed is requesting that the default binder be used to bind the method you are invoking. 
   // Instead of null as the third parameter, you can specify a Binder object that defines a set of properties and 
   // enables binding, which may involve selection of an overloaded method or coercion of argument types. 
   // The second null argument is the object on which to invoke the method you chose. 
   // Finally, pass an object array of the arguments that the member receives. 
   // In this case, the SayHello method receives no arguments, therefore we pass an empty array		

		// Call an instance method
    // You can also invoke instance methods. To do this, pass in an object of the type of which you want to invoke
    //  your method on as the third parameter. This example also demonstrates that you do not have to have an actual 
    //  Type object to use InvokeMember. In this case, you will generally want to use the class instance you have to call 
    //  GetType. Note that the BindingFlags have changed, now that we are not invoking a static method. 
		TestClass c = new TestClass();

		Console.WriteLine();
		Console.WriteLine("Invoking the instance method AddUp()");
		Console.WriteLine("---------------------------------");

		c.GetType().InvokeMember("AddUp", BindingFlags.InvokeMethod, null, c, new object [] {});
		c.GetType().InvokeMember("AddUp", BindingFlags.InvokeMethod, null, c, new object [] {});

    // Call another static method called ComputeSum, but in this case, the method needs two arguments. 
    // Therefore populate an object array with those arguments, and pass them into InvokeMember as the last parameter. 
    // We know that this particular method returns a value, being the computed sum,
    // so we create a variable to hold the return.
    // Note the datatype of the return is object, the only datatype that InvokeMethod returns.
		object [] args = new object [] {100.09, 184.45};
		object result;

		Console.WriteLine();
		Console.WriteLine("Invoking static method ComputeSum() with arguments/parameters");
		Console.WriteLine("---------------------------------");

		result = t.InvokeMember("ComputeSum", BindingFlags.InvokeMethod, null, null, args);

		Console.WriteLine("{0} + {1} = {2}", args[0], args[1], result);

		Console.WriteLine();
		Console.WriteLine("Invoking the field Name (Setting and Getting)");
		Console.WriteLine("---------------------------------");
		//Get a field value
		result = t.InvokeMember("Name", BindingFlags.GetField, null, c, new object [] {});
		Console.WriteLine("Name contains {0}", result);

		//Set a field
		t.InvokeMember("Name", BindingFlags.SetField, null, c, new object [] {"NewName"});
		result = t.InvokeMember("Name", BindingFlags.GetField, null, c, new object [] {});
		Console.WriteLine("Name contains {0}", result);

    // You can also get and set a property, but in this example, imagine that the property you are setting 
    // is an array or collection, which has multiple elements. To specify the setting of a particular element, 
    // you need to specify which index. To set a property, assign the BindingFlags.SetProperty. 
    // To specify an index of a collection or array for the property, place the index value of the element 
    // you want to set in the first element of the object array, then the value you want to set as the second element. 
    // To get the property back out, pass the index as the only element in the object array, specifying BindingFlags.GetProperty. 

		Console.WriteLine();
		Console.WriteLine("Invoking the indexed property Item (Setting and Getting)");
		Console.WriteLine("---------------------------------");
		//Get an indexed property value
		int  index = 3;
		result = t.InvokeMember("Item", BindingFlags.GetProperty , null, c, new object [] {index});
		Console.WriteLine("Item[{0}] contains {1}", index, result);

		//Set an indexed property value
		index = 3;
		t.InvokeMember("Item", BindingFlags.SetProperty, null, c, new object [] {index, "NewValue"});
		result = t.InvokeMember("Item", BindingFlags.GetProperty , null, c, new object [] {index});
		Console.WriteLine("Item[{0}] contains {1} after setting", index, result);

		Console.WriteLine();
		Console.WriteLine("Getting the field Name and property Value");
		Console.WriteLine("---------------------------------");
		//Get a field or property
		result = t.InvokeMember("Name", 
                             BindingFlags.GetField | BindingFlags.GetProperty,
                             null, 
                             c, 
                             new object [] {});
		Console.WriteLine("Name contains {0}", result);
		result = t.InvokeMember("Value", 
                             BindingFlags.GetField | BindingFlags.GetProperty, 
                             null, 
                             c, 
                             new object [] {});
		Console.WriteLine("Value contains {0}", result);

    //You can also use named arguments, in which case you need to use a different overloaded version 
    //of the InvokeMember method. Create the array of object arguments as you have been doing so far, and 
    //also create a string array of the names of the parameters being passed. The overloaded method you want to 
    //use accepts the list of parameter names as the last parameter, and the list of values you want to set as 
    //the fifth parameter. In this demonstration, all other parameters can be null (except, of course, the first two). 

		Console.WriteLine();
		Console.WriteLine("Invoking the method static method PrintName() with named arguments");
		Console.WriteLine("---------------------------------");
		//Call a method using named arguments
		object[] argValues = new object [] {"Mouse", "Micky"};
		string [] argNames = new string [] {"lastName", "firstName"};
		t.InvokeMember("PrintName", BindingFlags.InvokeMethod, null, null, argValues, null, null, argNames);

    //The following example uses a slightly different process to invoke a method. 
    //Rather than using the Type object directly, create a separate MethodInfo object directly to represent 
    //the method you will be invoking. Then, call the Invoke method on your MethodInfo object, 
    //passing an instance of the object you need to invoke your method on (if you are invoking an instance method, 
    //but null if your method is static). As before, an object array of the parameters is required. 
    //This particular sample allows you to pass parameters by reference, if required. 

		Console.WriteLine();
		Console.WriteLine("Invoking the method Swap() with ByRef parameters");
		Console.WriteLine("---------------------------------");
		//Invoking a ByRef member
		MethodInfo m = t.GetMethod("Swap");
		args = new object[2];
		args[0] = 1;
		args[1] = 2;
		m.Invoke(new TestClass(),args);
		Console.WriteLine("{0}, {1}", args[0], args[1]);

    //The following demonstrates how to invoke the default member on a class. 
    //Make sure that the class you are invoking upon has a default member specified. 
    //Then, in the InvokeMember method, do not specify a name for the member to invoke. 

    Console.WriteLine();
    Console.WriteLine("Invoking the default member of the TestClass2 type, PrintTime()");
    Console.WriteLine("---------------------------------");
    //Call the default member of a type
    Type t3 = typeof(TestClass2);
    t3.InvokeMember("", BindingFlags.InvokeMethod, null, new TestClass2(), new object [] {});

	}//Main()
}//class Invoke

/// <summary>
/// Class to use in reflection invoke tests.
/// </summary>
public class TestClass
{
	public string Name;
	private object [] values = new object [] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	public object this [int index]
	{
		get {
			return values[index];
		}//get
		set {
			values[index] = value;
		}//set
	}//property Item

	public object Value {
		get
		{
			return "the value";
		}//get
	}//property Value

	public TestClass()
	{
		Name = "initalName";
	}//constructor

	int methodCalled = 0;

	public static void SayHello()
	{
		Console.WriteLine("Hello");
	}//SayHello()

	public void AddUp()
	{
		methodCalled++;
		Console.WriteLine("AddUp Called {0} times", methodCalled);
	}//AddUp()

	public static double ComputeSum(double d1, double d2)
	{
		return d1 + d2;
	}//ComputeSum()

	public static void PrintName(string firstName, string lastName)
	{
		Console.WriteLine("{0},{1}", lastName, firstName);
	}//PrintName()

	public void Swap(ref int a, ref int b)
	{
		int x = a;
		a = b;
		b = x;
	}//Swap()
}//class TestClass

[DefaultMemberAttribute("PrintTime")]
public class TestClass2
{
	public void PrintTime()
	{
		Console.WriteLine(DateTime.Now);
	}//PrintTime()
}//class TestClass2
