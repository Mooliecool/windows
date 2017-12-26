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

namespace Microsoft.JScript {
    
    using System;
    using System.Reflection;

    /*
    This interface is implemented by all objects taking part in a scope chain.
    
    The first method either returns the contained object of a with scope, or the global scope.
    The second method runs up the scope chain until it reaches a scope that comprises static variables, for example a module scope.
    The third method is used to find local declarations. This is used at compile time for error checking.
    
    The latter two methods are used to search the scope chain for a variable that could not be finally resolved at compile time.
    This happens when a function that contains an eval refers to a variable in outer scope. Since the eval may declare a local
    with the same name, it is not safe to do early binding to the outer scope variable. Rather than search the entire scope
    chain, these functions search only the part preceding the scope known to have such a variable. If they fail to find such an item
    the early bound code is invoked. 
    
    There are two big advantages to doing this:
    1) Successful hashtable lookups (of strings) are much more expensive than unsuccessful lookups and should be avoided.
    2) StackFrames avoid allocating hash tables until a search is actually done on them. By avoiding the lookup
       we avoid the allocation and population of the hash table until it is really necessary.
    */
    
    public interface IActivationObject{
      Object GetDefaultThisObject();
      GlobalScope GetGlobalScope(); //Never returns null.
      FieldInfo GetLocalField(String name);
      Object GetMemberValue(String name, int lexlevel);
      FieldInfo GetField(String name, int lexLevel);
    }
}
