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
/////////////////////////////////////////////////////////////////////////
import System

class foo extends System.Object{
  var abc : System.Int32 = 1;
  static var def : System.Int32 = 2;
  static foo {def *= 10}
  function foo(x){
    print("initializing foo "+x);
  }
  function bar(i : System.Int32) : System.Int32{
    return i - this.abc + foo.def;
  }
}

var f = new foo("asdfasdf")
if(f.bar(1) == 20) {
	System.Environment.ExitCode = 0;
} else {
	System.Environment.ExitCode = 1;
}
