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
using System.Collections;
using System.IO;
class MyCollection : DictionaryBase {
 public void UseOnSet(object key, object oldVal, object newVal) {
 this.OnSet(key, oldVal, newVal);
 }
}
class Test {
 public static void Main() {
 int errors = 0;
 int testcases = 0;
 testcases++;
 MyCollection MyColl = new MyCollection();
 object obj = (object)8 ;
 MyColl.UseOnSet((object)0, (object)5, obj);
 if(! obj.Equals((object)8)) {
 errors++;
 }
 testcases++;
 MyColl = new MyCollection();
 obj = (object)8 ;
 MyColl.UseOnSet((object)5, (object)(-2), obj);
 if(! obj.Equals((object)8)) {
 errors++;
 }
 testcases++;
 MyColl = new MyCollection();
 obj = (object)(-19) ;
 MyColl.UseOnSet((object)0, (object)5, obj);
 if(! obj.Equals((object)(-19))) {
 errors++;
 }
 testcases++;
 MyColl = new MyCollection();
 obj = (object)8 ;
 MyColl.UseOnSet((object)-1, (object)5, obj);
 if(! obj.Equals((object)8)) {
 errors++;
 }
 Environment.ExitCode = errors;
 }
}
