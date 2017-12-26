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
   // A Filter is used to decide whether an assert failure 
   // should terminate the program (or invoke the debugger).  
   // Typically this is done by popping up a dialog & asking the user.
   // 
   // The default filter brings up a simple Win32 dialog with 3 buttons.
    
	[Serializable()]
	abstract internal class AssertFilter
    {
    
    	// Called when an assert fails.  This should be overridden with logic which
    	// determines whether the program should terminate or not.  Typically this
    	// is done by asking the user.
    	//
    
    	abstract public AssertFilters  AssertFailure(String condition, String message, 
    							  StackTrace location);
    
    }
    // No data, does not need to be marked with the serializable attribute
    internal class DefaultFilter : AssertFilter
    {
    	internal DefaultFilter()
    	{
    	}
    
    	public override AssertFilters  AssertFailure(String condition, String message, 
    							  StackTrace location)
    
    	{
    		return (AssertFilters) Assert.ShowDefaultAssertDialog (condition, message);
    	}
    }

}
