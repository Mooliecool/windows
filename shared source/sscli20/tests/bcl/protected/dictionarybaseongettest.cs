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
 public void UseOnGet(object key, object value) {
 this.OnGet(key, value);
 }
}
class Test {
 public static void Main() {
 int errors = 0;
 int testcases = 0;
 try {
 testcases++;
 MyCollection MyColl = new MyCollection();
 MyColl.UseOnGet((object)3, (object)5);
 } catch(Exception e) {
 errors++;
 }
 try {
 testcases++;
 MyCollection MyColl = new MyCollection();
 MyColl.UseOnGet((object)(-7), (object)5);
 } catch(Exception e) {
 errors++;
 }
 try {
 testcases++;
 MyCollection MyColl = new MyCollection();
 MyColl.UseOnGet((object)3, (object)(-18));
 } catch(Exception e) {
 errors++;
 }
 try {
 testcases++;
 MyCollection MyColl = new MyCollection();
 MyColl.UseOnGet((object)(-2), (object)(-12));
 } catch(Exception e) {
 errors++;
 }
 Environment.ExitCode = errors;
 }
}
