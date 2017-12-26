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

#ifndef _DEBUGRETURN_H_
#define _DEBUGRETURN_H_

#ifdef _DEBUG

// Code to generate a compile-time error if return statements appear where they
// shouldn't.
//
// Here's the way it works...
//
// We create two classes with a safe_to_return() method.  The method is static,
// returns void, and does nothing.  One class has the method as public, the other
// as private.  We introduce a global scope typedef for __ReturnOK that refers to
// the class with the public method.  So, by default, the expression
//
//      __ReturnOK::safe_to_return()
//
// quietly compiles and does nothing.  When we enter a block in which we want to
// inhibit returns, we introduce a new typedef that defines __ReturnOK as the
// class with the private method.  Inside this scope,
//
//      __ReturnOK::safe_to_return()
//
// generates a compile-time error.
//
// To cause the method to be called, we have to #define the return keyword.
// The simplest working version would be
//
//   #define return if (0) __ReturnOK::safe_to_return(); else return
//
// but we've used
//
//   #define return for (;1;__ReturnOK::safe_to_return()) return
//
// because it happens to generate somewhat faster code in a checked build.  (They
// both introduce no overhead in a fastchecked build.)
//
class __SafeToReturn {
public:
    static int safe_to_return() {return 0;};
};

class __YouCannotUseAReturnStatementHere {
private:
    static int safe_to_return() {return 0;};
public:
    // Some compilers warn if all member functions in a class are private.
    // Rather than disable the warning, we'll work around it here.
    static void dont_call_this() { _ASSERTE(!"Don't call this!"); }
};

typedef __SafeToReturn __ReturnOK;

// Use this to ensure that it is safe to return from a given scope
#define DEBUG_ASSURE_SAFE_TO_RETURN     __ReturnOK::safe_to_return()

#define return if (0 && __ReturnOK::safe_to_return()) { } else return

#define DEBUG_ASSURE_NO_RETURN_BEGIN { typedef __YouCannotUseAReturnStatementHere __ReturnOK; 
#define DEBUG_ASSURE_NO_RETURN_END   }

#define DEBUG_OK_TO_RETURN_BEGIN { typedef __SafeToReturn __ReturnOK; 
#define DEBUG_OK_TO_RETURN_END }

#define DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT DEBUG_ASSURE_NO_RETURN_BEGIN
#define DEBUG_ASSURE_NO_RETURN_END_SO_INTOLERANT DEBUG_ASSURE_NO_RETURN_END

#define DEBUG_OK_TO_RETURN_BEGIN_SO_INTOLERANT DEBUG_OK_TO_RETURN_BEGIN
#define DEBUG_OK_TO_RETURN_END_SO_INTOLERANT DEBUG_OK_TO_RETURN_END

#else


#define DEBUG_ASSURE_NO_RETURN_BEGIN
#define DEBUG_ASSURE_NO_RETURN_END

#define DEBUG_OK_TO_RETURN_BEGIN
#define DEBUG_OK_TO_RETURN_END

#define DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT 
#define DEBUG_ASSURE_NO_RETURN_END_SO_INTOLERANT 

#define DEBUG_OK_TO_RETURN_BEGIN_SO_INTOLERANT 
#define DEBUG_OK_TO_RETURN_END_SO_INTOLERANT 


#endif

#endif  // _DEBUGRETURN_H_
