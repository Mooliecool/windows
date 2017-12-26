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
namespace System.Diagnostics {
    using System;
    using System.Security.Permissions;
    using System.IO;
    using System.Reflection;
    using System.Runtime.CompilerServices;
    // Class which handles code asserts.  Asserts are used to explicitly protect
    // assumptions made in the code.  In general if an assert fails, it indicates 
    // a program bug so is immediately called to the attention of the user.
	// Only static data members, does not need to be marked with the serializable attribute
    internal static class Assert
    {
        private static AssertFilter[] ListOfFilters;
        private static int iNumOfFilters;
        private static int iFilterArraySize;

        static Assert()
        {
            Assert.AddFilter(new DefaultFilter());
        }
    
    	// AddFilter adds a new assert filter. This replaces the current
    	// filter, unless the filter returns FailContinue.
    	//
    	public static void AddFilter(AssertFilter filter)
    	{
    		if (iFilterArraySize <= iNumOfFilters)
    		{
    				AssertFilter[] newFilterArray = new AssertFilter [iFilterArraySize+2];
    
    	            if (iNumOfFilters > 0) 
    					Array.Copy(ListOfFilters, newFilterArray, iNumOfFilters);
    
    				iFilterArraySize += 2;
    
    		        ListOfFilters = newFilterArray;
    		}
    
    		ListOfFilters [iNumOfFilters++] = filter;			
    	}
    
    	// Called when an assertion is being made.
    	//
    	public static void Check(bool condition, String conditionString, String message)
    	{
    		if (!condition)
    		{
    			Fail (conditionString, message);
    		}
    	}
    
    
    	public static void Fail(String conditionString, String message)
    	{
    		// get the stacktrace
    		StackTrace st = new StackTrace();
    
    		// Run through the list of filters backwards (the last filter in the list
    		// is the default filter. So we're guaranteed that there will be atleast 
    		// one filter to handle the assert.
    
    		int iTemp = iNumOfFilters;
    		while (iTemp > 0)
    		{
    
    			AssertFilters iResult = ListOfFilters [--iTemp].AssertFailure (conditionString, message, st);
    
    			if (iResult == AssertFilters.FailDebug)
    			{
    				if (Debugger.IsAttached == true)
    					Debugger.Break();
    				else
    				{
    					if (Debugger.Launch() == false)
    					{
    						throw new InvalidOperationException(
    								Environment.GetResourceString("InvalidOperation_DebuggerLaunchFailed"));
    					}						
    				}
    
    				break;
    			}
    			else if (iResult == AssertFilters.FailTerminate)
    				Environment.Exit(-1);
    			else if (iResult == AssertFilters.FailIgnore)
    				break;
    
    			// If none of the above, it means that the Filter returned FailContinue.
    			// So invoke the next filter.
    		}
    
    	}
    
      // Called when an assert happens.
      //
      [MethodImplAttribute(MethodImplOptions.InternalCall)]
      public extern static int ShowDefaultAssertDialog(String conditionString, String message);
    }
}
