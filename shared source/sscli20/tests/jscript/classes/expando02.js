//# ==++== 
//# 
//#   
//#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//#   
//#    The use and distribution terms for this software are contained in the file
//#    named license.txt, which can be found in the root of this distribution.
//#    By using this software in any fashion, you are agreeing to be bound by the
//#    terms of this license.
//#   
//#    You must not remove this notice, or any other, from this software.
//#   
//# 
//# ==--== 
//####################################################################################
@cc_on


import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;


var iTestID = 198993;

/*/////////////////////////////////////////////////////////////////////////////


	purpose:  to test expando classes in different circumstances/scopes



////////////////////////////////////////////////////////////////////////////*/


var t1fail:boolean = false
var t2fail:boolean = false
expando class cls1{
  var x = 5
}
expando class cls2{}

var tvar1 = new cls1				// create an instance of expando class
tvar1["x"] = 20					// create an expando property
if (tvar1.x != 5) t1fail = true			// ensure that expandos and fields are
tvar1.x = 10					//   separate, and do not interfere
if (tvar1["x"] != 20) t1fail = true;
tvar1[tvar1] = 1234				// create expando using non string key
if (tvar1[tvar1] != 1234) t1fail = true		// ensure the new expando can be accessed


var tvar2 = new cls2				// create an instance of expando class
tvar2[cls1] = new cls1				// create a nested expando
tvar2[cls1]["x"] = 20				// create an expando property
if (tvar2[cls1].x != 5) t2fail = true		// ensure that expandos and fields are
tvar2[cls1].x = 10				//   separate, and do not interfere
if (tvar2[cls1]["x"] != 20) t2fail = true;
tvar2[cls1][tvar2] = 1234			// create expando using non string key
if (tvar2[cls1][tvar2] != 1234) t2fail = true	// ensure the new expando can be accessed


class wrapper{
  var t1fail:boolean = false
  var t2fail:boolean = false

  expando class cls1{
    var x = 5
  }
  expando class cls2{}

  function test(){
    var tvar1 = new cls1
    tvar1["x"] = 20
    if (tvar1.x != 5) t1fail = true
    tvar1.x = 10
    if (tvar1["x"] != 20) t1fail = true;
    tvar1[tvar1] = 1234
    if (tvar1[tvar1] != 1234) t1fail = true


    var tvar2 = new cls2
    tvar2[cls1] = new cls1
    tvar2[cls1]["x"] = 20
    if (tvar2[cls1].x != 5) t2fail = true
    tvar2[cls1].x = 10
    if (tvar2[cls1]["x"] != 20) t2fail = true;
    tvar2[cls1][tvar2] = 1234
    if (tvar2[cls1][tvar2] != 1234) t2fail = true

  }
}

package pack{
  expando class cls1p{
    var x = 5
  }
  internal expando class cls2p{}

  class testclass{
    var t1fail:boolean = false
    var t2fail:boolean = false

    function test(){
      var tvar1 = new cls1p
      tvar1["x"] = 20
      if (tvar1.x != 5) t1fail = true
      tvar1.x = 10
      if (tvar1["x"] != 20) t1fail = true;
      tvar1[tvar1] = 1234
      if (tvar1[tvar1] != 1234) t1fail = true 


      var tvar2 = new cls2p
      tvar2[cls1] = new cls1
      tvar2[cls1]["x"] = 20
      if (tvar2[cls1].x != 5) t2fail = true
      tvar2[cls1].x = 10
      if (tvar2[cls1]["x"] != 20) t2fail = true;
      tvar2[cls1][tvar2] = 1234
      if (tvar2[cls1][tvar2] != 1234) t2fail = true

    }
  }
}



var test3and4 = new wrapper
test3and4.test()
var test9and10 = new pack.testclass
test9and10.test()

function expando02(){

  apInitTest("Expando02")

  apInitScenario("Expando class in the global scope")
  if (t1fail) apLogFailInfo("value return failure encountered", "", "", "")

  apInitScenario("Expando subclass in the global scope")
  if (t2fail) apLogFailInfo("value return failure encountered", "", "", "")


  apInitScenario("Expando class in a class from global scope")
  if (test3and4.t1fail) apLogFailInfo("value return failure encountered", "", "", "")

  apInitScenario("Expando subclass in a class from global scope")
  if (test3and4.t2fail) apLogFailInfo("value return failure encountered", "", "", "")



  apInitScenario("Expando class in a function scope")
  var t5fail:boolean = false
  var tvar5 = new cls1				// create an instance of expando class
  tvar5["x"] = 20				// create an expando property
  if (tvar5.x != 5) t5fail = true		// ensure that expandos and fields are
  tvar5.x = 10					//   separate, and do not interfere
  if (tvar5["x"] != 20) t5fail = true;
  tvar5[tvar5] = 1234				// create expando using non string key
  if (tvar5[tvar5] != 1234) t5fail = true	// ensure the new expando can be accessed
  if (t5fail) apLogFailInfo("value return failure encountered", "", "", "")

  apInitScenario("Expando subclass in a function scope")
  var t6fail:boolean = false

  var tvar6 = new cls2				// create an instance of expando class
  tvar6[cls1] = new cls1			// create a nested expando
  tvar6[cls1]["x"] = 20				// create an expando property
  if (tvar6[cls1].x != 5) t6fail = true		// ensure that expandos and fields are
  tvar6[cls1].x = 10				//   separate, and do not interfere
  if (tvar6[cls1]["x"] != 20) t6fail = true;
  tvar6[cls1][tvar6] = 1234			// create expando using non string key
  if (tvar6[cls1][tvar6] != 1234) t6fail = true	// ensure the new expando can be accessed
  if (t6fail) apLogFailInfo("value return failure encountered", "", "", "")


  apInitScenario("Expando class in a class in a function scope")
  var test7and8 = new wrapper
  test7and8.test()
  if (test7and8.t1fail) apLogFailInfo("value return failure encountered", "", "", "")

  apInitScenario("Expando subclass in a class in a function scope")
  if (test7and8.t2fail) apLogFailInfo("value return failure encountered", "", "", "")


  apInitScenario("Expando class in a package from global scope")
  if (test9and10.t1fail) apLogFailInfo("value return failure encountered", "", "", "")

  apInitScenario("Expando subclass in a package from global scope")
  if (test9and10.t2fail) apLogFailInfo("value return failure encountered", "", "", "")


  apInitScenario("Expando class in a package from a function scope")
  var test11and12 = new pack.testclass
  test11and12.test()
  if (test11and12.t1fail) apLogFailInfo("value return failure encountered", "", "", "")

  apInitScenario("Expando subclass in a package from a function scope")
  if (test11and12.t2fail) apLogFailInfo("value return failure encountered", "", "", "")




  apEndTest();

}


expando02();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  //navigator.appVersion.toUpperCase().charAt(navigator.appVersion.toUpperCase().indexOf("MSIE")+5);
    apGlobalObj.apGetHost = function Funcp() { return "Rotor " + sVer; }
    print ("apInitTest: " + stTestName);
}

function apInitScenario(stScenarioName) {print( "\tapInitScenario: " + stScenarioName);}

function apLogFailInfo(stMessage, stExpected, stActual, stBugNum) {
    lFailCount = lFailCount + 1;
    print ("***** FAILED:");
    print ("\t\t" + stMessage);
    print ("\t\tExpected: " + stExpected);
    print ("\t\tActual: " + stActual);
}

function apGetLocale(){ return 1033; }
function apWriteDebug(s) { print("dbg ---> " + s) }
function apEndTest() {}
